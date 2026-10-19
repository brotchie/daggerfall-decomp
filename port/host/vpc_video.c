/* vpc_video.c: the virtual PC's VGA (port/include/port_vpc.h). Mode 13h is 320x200 with one
   byte a pixel at 0xA0000 of low memory and a palette of 256 colours of 6-bit components,
   written through the DAC's ports (3C8h index, 3C9h red, green, blue). The status port (3DAh)
   gives a 70 Hz vertical retrace, as a VGA in mode 13h does; the screen is shown when the
   engine sees a retrace start, and every 14 ms when nobody waits for one.

   The window shows the 320x200 screen at 4:3, as a monitor did (each pixel 1.2 times as tall
   as it is wide), as large as the window allows. Alt+Enter toggles full screen; F12 saves the
   screen as dagger_NNN.bmp.

   PORT_GL=1 (fall --gl) shows the screen through OpenGL instead of SDL's renderer, with the 3D
   view drawn by OpenGL (port/host/vpc_gl.c); F11 switches the 3D view between OpenGL and the
   software renderer, F10 OpenGL's textures between texels and smooth. */
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_gl.h"
#include "port_vpc.h"

#define VGA_W 320
#define VGA_H 200
#define RETRACE_PERIOD_NS 14285714ull       /* 70 Hz */
#define RETRACE_PULSE_NS 600000ull          /* the part of it with the retrace bit set */

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static Uint32 pixels[VGA_W * VGA_H];

static unsigned char dac[256][3];           /* 6-bit components */
static unsigned int dac_write, dac_read, dac_wc, dac_rc;
static unsigned char dac_state;             /* 3C7h: 0 writing, 3 reading */
static unsigned char pel_mask = 0xFF;
static unsigned int mode = 3;
static Uint64 last_present, last_period;
static int screenshot_count;
static int use_gl;
static int (*view_probe)(int *cx, int *cy);
static int (*look_probe)(int *reverse, int *pitch);

void vpc_set_view_probe(int (*probe)(int *cx, int *cy))
{
    view_probe = probe;
}

void vpc_set_look_probe(int (*probe)(int *reverse, int *pitch))
{
    look_probe = probe;
}

/* The engine turns the view in whole steps of 1/2048 of a turn, which at the window's
   resolution are several pixels: OpenGL adds the mouse's motion its scene does not show yet
   (vpc_gl.c), so the view turns as smoothly as the mouse moves. look: {reverse, pitch} from
   the game's side; 0 when the next frame will not turn the view. */
static int smooth_look(int look[2])
{
    return look_probe != NULL && look_probe(&look[0], &look[1]);
}

/* where the crosshair goes (1), or none (0): while the mouse looks around the world */
static int crosshair(int *cx, int *cy)
{
    return view_probe != NULL && mode == 0x13 && view_probe(cx, cy) && *cx > 4 &&
           *cx < VGA_W - 4 && *cy > 4 && *cy < VGA_H - 4;
}

void vpc_video_init(const char *title)
{
    const char *gl = SDL_getenv("PORT_GL");

    if (gl && *gl && *gl != '0') {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        window = SDL_CreateWindow(title, 960, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL |
                                                       SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (window && vpc_gl_init(window)) {
            use_gl = 1;
            return;
        }
        fprintf(stderr, "port: no OpenGL; the software renderer shows the screen\n");
        vpc_gl_shutdown();
        if (window)
            SDL_DestroyWindow(window);
    }
    window = SDL_CreateWindow(title, 960, 720, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "port: SDL_CreateWindow: %s\n", SDL_GetError());
        return;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        fprintf(stderr, "port: SDL_CreateRenderer: %s\n", SDL_GetError());
        return;
    }
    SDL_SetRenderVSync(renderer, 0);
    /* 320x240 logical: the 320x200 picture stretched to 4:3, letterboxed in the window */
    SDL_SetRenderLogicalPresentation(renderer, VGA_W, 240, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING,
                                VGA_W, VGA_H);
    if (texture != NULL)
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
}

void vpc_video_shutdown(void)
{
    if (use_gl)
        vpc_gl_shutdown();
    if (texture)
        SDL_DestroyTexture(texture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    texture = NULL;
    renderer = NULL;
    window = NULL;
}

/* ---- the picture ----------------------------------------------------------------------- */

static Uint32 colour(unsigned int i)
{
    const unsigned char *c = dac[i & pel_mask];
    /* 6 bits to 8, as a VGA's DAC drives its 6-bit levels */
    Uint32 r = (Uint32)(c[0] << 2 | c[0] >> 4), g = (Uint32)(c[1] << 2 | c[1] >> 4),
           b = (Uint32)(c[2] << 2 | c[2] >> 4);
    return 0xFF000000u | r << 16 | g << 8 | b;
}

static void render_pixels(void)
{
    Uint32 pal[256];
    const unsigned char *vga = port_low_memory + 0xA0000;
    int i;

    if (mode != 0x13) {
        memset(pixels, 0, sizeof pixels);
        return;
    }
    for (i = 0; i < 256; i++)
        pal[i] = colour((unsigned int)i);
    for (i = 0; i < VGA_W * VGA_H; i++)
        pixels[i] = pal[vga[i]];
    {
        /* the crosshair: a white plus in a dark outline, three pixels each way */
        int cx, cy, d;

        if (crosshair(&cx, &cy)) {
            for (d = -4; d <= 4; d++) {
                pixels[(cy - 1) * VGA_W + cx + d] = pixels[(cy + 1) * VGA_W + cx + d] = 0xFF000000u;
                pixels[(cy + d) * VGA_W + cx - 1] = pixels[(cy + d) * VGA_W + cx + 1] = 0xFF000000u;
            }
            for (d = -3; d <= 3; d++)
                if (d < -1 || d > 1)
                    pixels[cy * VGA_W + cx + d] = pixels[(cy + d) * VGA_W + cx] = 0xFFFFFFFFu;
        }
    }
}

/* the window is the main thread's: the game's thread (a retrace it saw, a mode set) asks for
   the screen to be shown, and the main thread shows it within a couple of ms */
static SDL_AtomicInt present_wanted;

/* PORT_FPS=1: how many different pictures the game showed each second (stderr, every 5 s) */
static void count_frames(void)
{
    static int on = -1;
    static Uint64 last_hash, since;
    static int frames;
    const unsigned char *vga = port_low_memory + 0xA0000;
    Uint64 h = 1469598103934665603ull, now = SDL_GetTicksNS();
    int i;

    if (on < 0)
        on = SDL_getenv("PORT_FPS") != NULL;
    if (!on)
        return;
    for (i = 0; i < VGA_W * VGA_H; i += 8)
        h = (h ^ *(const Uint64 *)(vga + i)) * 1099511628211ull;
    if (h != last_hash)
        frames++;
    last_hash = h;
    if (since == 0)
        since = now;
    if (now - since >= 5000000000ull) {
        static unsigned long last_yields;
        extern volatile unsigned long vpc_frame_yields;
        unsigned long y = vpc_frame_yields;
        fprintf(stderr, "port: %.1f pictures a second, the game's loop %.1f times\n",
                frames * 1e9 / (double)(now - since), (y - last_yields) * 1e9 / (double)(now - since));
        last_yields = y;
        frames = 0;
        since = now;
    }
}

static void present_now(void)
{
    SDL_FRect dst = {0, 0, VGA_W, 240};

    count_frames();
    last_present = SDL_GetTicksNS();
    if (use_gl) {
        static const unsigned char black[VGA_W * VGA_H];
        unsigned char rgb[256 * 3];
        int i;

        for (i = 0; i < 256; i++) {
            Uint32 c = colour((unsigned int)i);

            rgb[i * 3] = (unsigned char)(c >> 16);
            rgb[i * 3 + 1] = (unsigned char)(c >> 8);
            rgb[i * 3 + 2] = (unsigned char)c;
        }
        {
            int cross[2];
            int look[2];
            int turn = smooth_look(look);

            vpc_gl_present(mode == 0x13 ? port_low_memory + 0xA0000 : black, rgb,
                           crosshair(&cross[0], &cross[1]) ? cross : NULL, turn ? look : NULL);
        }
        return;
    }
    if (renderer == NULL || texture == NULL)
        return;
    render_pixels();
    SDL_UpdateTexture(texture, NULL, pixels, VGA_W * 4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, &dst);
    SDL_RenderPresent(renderer);
}

void vpc_present(void)
{
    if (vpc_on_main_thread())
        present_now();
    else
        SDL_SetAtomicInt(&present_wanted, 1);
}

void vpc_video_maybe_present(void)
{
    if (SDL_GetAtomicInt(&present_wanted) || SDL_GetTicksNS() - last_present >= RETRACE_PERIOD_NS) {
        SDL_SetAtomicInt(&present_wanted, 0);
        present_now();
    }
}

int vpc_screenshot(const char *path)
{
    SDL_Surface *s;
    int ok;

    if (use_gl) {
        /* PORT_GL_SW_SHOTS=1: the software screen of the same frame too (NAME_sw.bmp) */
        const char *both = SDL_getenv("PORT_GL_SW_SHOTS");

        present_now();

        if (both && *both == '1') {
            char sw[1024];
            size_t n = strlen(path);

            if (n > 4 && n < sizeof sw - 4 && strcmp(path + n - 4, ".bmp") == 0) {
                snprintf(sw, sizeof sw, "%.*s_sw.bmp", (int)(n - 4), path);
                render_pixels();
                s = SDL_CreateSurfaceFrom(VGA_W, VGA_H, SDL_PIXELFORMAT_XRGB8888, pixels,
                                          VGA_W * 4);
                if (s) {
                    SDL_SaveBMP(s, sw);
                    SDL_DestroySurface(s);
                }
            }
        }
        return vpc_gl_screenshot(path);
    }
    render_pixels();
    s = SDL_CreateSurfaceFrom(VGA_W, VGA_H, SDL_PIXELFORMAT_XRGB8888, pixels, VGA_W * 4);
    if (s == NULL)
        return -1;
    ok = SDL_SaveBMP(s, path);
    SDL_DestroySurface(s);
    return ok ? 0 : -1;
}

/* ---- mode ----------------------------------------------------------------------------- */

void vpc_video_set_mode(unsigned int m)
{
    unsigned int keep = m & 0x80;

    m &= 0x7F;
    mode = m;
    port_low_memory[0x449] = (unsigned char)m;
    port_low_memory[0x44A] = m == 0x13 ? 40 : 80;
    if (!keep && m == 0x13)
        memset(port_low_memory + 0xA0000, 0, 0x10000);
    vpc_present();
}

unsigned int vpc_video_mode(void)
{
    return mode;
}

/* ---- ports ------------------------------------------------------------------------------- */

unsigned char vpc_video_inb(unsigned int port)
{
    unsigned char v;

    switch (port) {
    case 0x3C6:
        return pel_mask;
    case 0x3C7:
        return dac_state;
    case 0x3C8:
        return (unsigned char)dac_write;
    case 0x3C9:
        v = dac[dac_read][dac_rc];
        if (++dac_rc == 3) {
            dac_rc = 0;
            dac_read = (dac_read + 1) & 0xFF;
        }
        return v;
    case 0x3DA: {
        /* bit 3 vertical retrace, bit 0 any blanking. The screen is shown when a read sees
           a new retrace; a read outside one waits a little, so a wait loop does not spin */
        Uint64 now = SDL_GetTicksNS();
        Uint64 period = now / RETRACE_PERIOD_NS, phase = now % RETRACE_PERIOD_NS;
        if (phase < RETRACE_PULSE_NS) {
            if (period != last_period) {
                last_period = period;
                vpc_present();
            }
            return 0x09;
        }
        if (RETRACE_PERIOD_NS - phase > 1000000)
            SDL_DelayNS(500000);
        return 0x00;
    }
    }
    return 0xFF;
}

void vpc_video_outb(unsigned int port, unsigned char v)
{
    switch (port) {
    case 0x3C6:
        pel_mask = v;
        break;
    case 0x3C7:
        dac_read = v;
        dac_rc = 0;
        dac_state = 3;
        break;
    case 0x3C8:
        dac_write = v;
        dac_wc = 0;
        dac_state = 0;
        break;
    case 0x3C9:
        dac[dac_write][dac_wc] = v & 0x3F;
        if (++dac_wc == 3) {
            dac_wc = 0;
            dac_write = (dac_write + 1) & 0xFF;
        }
        break;
    }
}

/* ---- the window's own keys --------------------------------------------------------------- */

int vpc_video_handle_event(const void *ev)
{
    const SDL_Event *e = ev;

    if (e->type == SDL_EVENT_KEY_DOWN && !e->key.repeat) {
        if (e->key.scancode == SDL_SCANCODE_RETURN && (e->key.mod & SDL_KMOD_ALT)) {
            SDL_SetWindowFullscreen(window, !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
            return 1;
        }
        if (e->key.scancode == SDL_SCANCODE_F11 && use_gl) {
            vpc_gl_toggle();
            return 1;
        }
        if (e->key.scancode == SDL_SCANCODE_F10 && use_gl) {
            vpc_gl_toggle_smooth();
            return 1;
        }
        if (e->key.scancode == SDL_SCANCODE_F12) {
            char name[32];
            snprintf(name, sizeof name, "dagger_%03d.bmp", screenshot_count++);
            if (vpc_screenshot(name) == 0)
                fprintf(stderr, "port: saved %s\n", name);
            return 1;
        }
    }
    if (e->type == SDL_EVENT_KEY_UP &&
        (e->key.scancode == SDL_SCANCODE_F12 || e->key.scancode == SDL_SCANCODE_F11 ||
         (e->key.scancode == SDL_SCANCODE_F10 && use_gl) ||
         (e->key.scancode == SDL_SCANCODE_RETURN && (e->key.mod & SDL_KMOD_ALT))))
        return 1;
    return 0;
}
