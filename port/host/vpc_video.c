/* vpc_video.c: the virtual PC's VGA (port/include/port_vpc.h). Mode 13h is 320x200 with one
   byte a pixel at 0xA0000 of low memory and a palette of 256 colours of 6-bit components,
   written through the DAC's ports (3C8h index, 3C9h red, green, blue). The status port (3DAh)
   gives a 70 Hz vertical retrace, as a VGA in mode 13h does; the screen is shown when the
   engine sees a retrace start, and every 14 ms when nobody waits for one.

   The window shows the 320x200 screen at 4:3, as a monitor did (each pixel 1.2 times as tall
   as it is wide), as large as the window allows. Alt+Enter toggles full screen; F12 saves the
   screen as dagger_NNN.bmp. */
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

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

void vpc_video_init(const char *title)
{
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
}

void vpc_present(void)
{
    SDL_FRect dst = {0, 0, VGA_W, 240};

    last_present = SDL_GetTicksNS();
    if (renderer == NULL || texture == NULL)
        return;
    render_pixels();
    SDL_UpdateTexture(texture, NULL, pixels, VGA_W * 4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, &dst);
    SDL_RenderPresent(renderer);
}

void vpc_video_maybe_present(void)
{
    if (SDL_GetTicksNS() - last_present >= RETRACE_PERIOD_NS)
        vpc_present();
}

int vpc_screenshot(const char *path)
{
    SDL_Surface *s;
    int ok;

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
        if (e->key.scancode == SDL_SCANCODE_F12) {
            char name[32];
            snprintf(name, sizeof name, "dagger_%03d.bmp", screenshot_count++);
            if (vpc_screenshot(name) == 0)
                fprintf(stderr, "port: saved %s\n", name);
            return 1;
        }
    }
    if (e->type == SDL_EVENT_KEY_UP &&
        (e->key.scancode == SDL_SCANCODE_F12 ||
         (e->key.scancode == SDL_SCANCODE_RETURN && (e->key.mod & SDL_KMOD_ALT))))
        return 1;
    return 0;
}
