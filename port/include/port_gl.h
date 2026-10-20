/* port_gl.h: the OpenGL renderer's interface (docs/port.md, "OpenGL"). PORT_GL=1 (or --gl)
   swaps XnGine's software 3D for OpenGL with shaders:

   - The capture (port/engine_gl/glcap.c, on the game's thread, compiled with the engine) takes
     each frame's polygons and flats as the engine builds them: their camera-space vertices,
     and, once the frame is drawn, the routine, texture and light the engine chose for each.
     It turns them into triangles whose vertices carry the engine's own texel coordinates
     and shade rows (computed as the span routines compute them), and hands the frame over
     at the engine's present.
   - The renderer (port/host/vpc_gl.c, on the main thread) draws the triangles into a palette
     index framebuffer at the window's resolution, with the engine's shade and haze tables
     in the fragment shader, then puts the software screen's 2D on top: a pixel the game drew
     after the 3D pass (the HUD, the weapon, weather, water) shows the software screen.

   The software renderer still runs, so the game sees exactly what it saw before (the pick,
   the screen) and the software picture is there to fall back on. Nothing here is engine
   data: plain C types only, so both sides can include it. */
#ifndef PORT_GL_H
#define PORT_GL_H

#include <stddef.h>
#include <stdint.h>

/* the engine's objects are built with -fpack-struct=1 (Watcom's packing), the host's are not:
   these structs have one layout for both */
#pragma pack(push, 8)

/* a vertex: camera space (the engine's view space: the side planes are x = +-z, y = +-z),
   the texel it maps to (texels; an image's own coordinates), and its shade (a row of the
   tables: 0-63 the shade table, 64-127 the haze table, with a fraction; < 0 none) */
typedef struct pgl_vertex {
    float x, y, z;
    float u, v;
    float shade;
} pgl_vertex;

/* how a draw's pixels are made */
enum {
    PGL_TEX_WRAP,           /* a model face or ground cell: the texture repeats */
    PGL_SOLID,              /* a solid colour */
    PGL_FLAT,               /* a billboard: clamped, texel 0 transparent */
    PGL_FLAT_TRANSLUCENT    /* a translucent billboard (ghosts): every other pixel */
};

/* a run of triangles with one texture and one way of drawing */
typedef struct pgl_draw {
    int first, count;       /* vertices: count / 3 triangles */
    int mode;               /* PGL_* */
    int tex;                /* the texture's id (pgl_texture_*), -1 for none */
    int colour;             /* PGL_SOLID: the colour index */
    int row2;               /* a second row every texel goes through (a flat's fog), or -1 */
    int fog;                /* the frame's distance fog applies */
    int layer;              /* 0 the world (depth written), 1 flats (tested, not written) */
    /* a light shader (xlight.h), evaluated per pixel: 1-3 point lights, else 0. Its base
       shade (table bytes from the shade table's start) and per light the foot point on the
       face's plane (view space >> 8), the intensity and the falloff */
    int nlights;
    float lbase;
    float lights[3][5];
} pgl_draw;

#define PGL_W 320               /* the game's screen */
#define PGL_H 200
#define PGL_MAX_W 1024          /* the widest frame: the ray tables cover 1024 columns */

/* the wide pass's own pools (the engine's were sized for 320 columns and have no bounds):
   polygon records, span nodes, light references (port/engine_gl/wide.c) */
#define PORT_POLY_POOL 6000
#define PORT_SPAN_POOL 60000
#define PORT_LIGHT_POOL 9600

/* a frame of the 3D view */
typedef struct pgl_scene {
    pgl_vertex *verts;
    int nverts, cap_verts;
    pgl_draw *draws;
    int ndraws, cap_draws;
    int valid;              /* 0: no 3D view (show the software screen) */
    unsigned long serial;   /* counts the frames drawn */
    /* the frame: the game's 320 columns, or the wide pass's W (port/engine_gl/wide.c) with the
       game's screen at column dx */
    int w, dx;
    /* the view: the centre and half size on the frame, the near and far planes (camera z) */
    float cx, cy, hw, hh, near_z, far_z;
    float focal_x, focal_y;                 /* pixels (the light shaders' rays) */
    int cam_yaw, cam_pitch, cam_roll;       /* the view's angles (2048 a turn) */
    int cam_x, cam_y, cam_z;                /* the eye, world units */
    double mouse_rem_x, mouse_rem_y;        /* the fractions of a mickey (of a step of the
                                               view's angles) the frame's read of the mouse
                                               left: vpc_gl.c's smooth look */
    double mouse_used_x, mouse_used_y;      /* the mickeys the game had read, all told, when
                                               the frame was drawn (vpc_gl.c's late look) */
    double game_ms;                         /* the game's clock: its frame times summed (the
                                               time the eye's motion is measured in) */
    unsigned long long host_ns;             /* when it was handed over */
    unsigned long present_serial;           /* the present that showed it (vga's) */
    int clip_l, clip_t, clip_r, clip_b;     /* the 3D window on the frame */
    int bg_fill, bg_colour;                 /* indoors: what no polygon covers */
    /* the distance fog: camera z >> 8 beyond start goes step table bytes a unit toward
       haze row 0 (xshade.h) */
    int fog_on;
    float fog_start, fog_step, fog_far;
    int pool_polys, pool_spans;             /* the frame's use of the pools (PORT_GL_DEBUG) */
    unsigned char tables[128 * 256];        /* the shade table, then the haze table */
    unsigned short falloff[32768];          /* LIGHT.DAT: a light's falloff by distance */
    unsigned char vga[PGL_W * PGL_H];       /* the game's screen as the present showed it */
    /* w x 200 each (rows of w): the frame before the 3D pass (the sky), after it (before the
       HUD), and as presented (the weather on it; a wide frame only) */
    unsigned char pre[PGL_MAX_W * PGL_H];
    unsigned char post[PGL_MAX_W * PGL_H];
    unsigned char side[PGL_MAX_W * PGL_H];
} pgl_scene;

#pragma pack(pop)

/* ---- the game's side (port/engine_gl) -------------------------------------------------- */

/* set by the main thread before the game starts: capture frames */
extern volatile int pgl_enabled;
/* the host's realloc (the engine's objects name only what the native build defines: a C
   library name there would be taken for a game symbol, tools/port_build.py) */
void *pgl_realloc(void *p, size_t n);
/* the fractions of a mickey the last read of the mouse's motion left (int 33h fn 0Bh) */
void pgl_mouse_remainder(double *rx, double *ry);
/* PORT_GL_VERTEX_LIGHT=1: light shaders sampled per vertex, not per pixel */
int pgl_vertex_light(void);
/* the mickeys the game has read through int 33h fn 0Bh, all told; and with those it has not
   read yet (the main thread's late look) */
void pgl_mouse_used(double *x, double *y);
void pgl_mouse_total(double *x, double *y);
/* the host's clock, ns */
unsigned long long pgl_now_ns(void);
/* the frame width the main thread wants from the wide pass (columns; 0 or 320: none) */
extern volatile int pgl_wide_request;
/* the presents so far (xn_gfx_drv0_present, after its copy to the VGA) */
extern volatile unsigned long pgl_present_serial;
/* PORT_GL_WIDE_CHECK=N: every Nth wide frame a copy of the process draws it the original way
   and the two screens are compared. begin: 1 in the copy (it draws the narrow frame); the
   copy's screen at the frame's end (child_done ends it); end: the wide pass's screen, compared */
int pgl_check_fork_begin(void);
int pgl_check_child(void);              /* in the copy */
int pgl_check_pending(void);            /* a copy is drawing this frame */
/* the screens, the depth of each pixel (1/z of what covers it, 0 none), and the screen after
   the sky (before the 3D pass) */
void pgl_check_child_done(const unsigned char *screen, const unsigned int *depth,
                          const unsigned char *sky);
void pgl_check_fork_end(const unsigned char *screen, const unsigned int *depth,
                        const unsigned char *sky);

/* ---- the hand-over: a frame drawn on the game's thread, shown on the main thread ---------- */

/* the scene the game's thread fills (it owns it until pgl_publish) */
pgl_scene *pgl_building(void);
/* hands the building scene over as the latest; the game gets another to fill */
void pgl_publish(void);
/* the main thread: the latest scene (the same one until a newer is published), or NULL */
pgl_scene *pgl_latest(void);

/* Textures: the capture registers each image's pixels once (by a hash of them) and gets an id;
   the renderer uploads the ids it has not seen. */
int pgl_texture_id(uint64_t hash);                        /* -1 when new */
int pgl_texture_add(uint64_t hash, int w, int h, const unsigned char *pixels, int stride);
/* the main thread: the next texture to upload (its id, size and pixels; free them), 0 when
   none is waiting */
int pgl_texture_next(int *id, int *w, int *h, unsigned char **pixels);

/* ---- the main thread's side (port/host/vpc_gl.c) -------------------------------------------- */

/* the window to create (SDL_WINDOW_OPENGL) and its context; 0 on failure (then the software
   path shows the screen) */
int vpc_gl_init(void *sdl_window);
void vpc_gl_shutdown(void);
/* shows the screen: the 320x200 picture vga (the VGA's memory) through the palette (256 RGB
   bytes, 8 bits each), with the latest 3D scene composited in; cross: where a crosshair goes
   (320x200 pixels), or NULL */
/* look: the mouse turns the view ({REVERSE VERTICAL's sign, the view's pitch now}), or NULL:
   the fraction of a step the mouse had moved past the scene's whole angles (the scene's
   mouse_rem) is added as a turn, with a small overscan that covers the strip it brings in at
   the edge */
void vpc_gl_present(const unsigned char *vga, const unsigned char *rgb, const int *cross,
                    const int *look);
/* the last frame shown, as a .bmp at the window's resolution; 0 on success */
int vpc_gl_screenshot(const char *path);
/* the 320x200 picture the last present composited (the software screen of the frame shown) */
const unsigned char *vpc_gl_last_vga(void);
/* the last scene's wide frame as the software renderer drew it (the game's screen in its
   middle; rgb: the palette), as a .bmp; -1 when the last present showed no wide frame */
int vpc_gl_screenshot_wide_sw(const char *path, const unsigned char *rgb);
/* F11: the 3D view through OpenGL or the software renderer */
void vpc_gl_toggle(void);
/* F10: OpenGL's textures as texels or smooth (bilinear, through the palette) */
void vpc_gl_toggle_smooth(void);
/* Shift+F11: the view 4:3 or the window's shape; Shift+F10: smooth motion (both saved in the
   settings file) */
void vpc_gl_toggle_aspect(void);
void vpc_gl_toggle_interp(void);
/* vsync paces the presents: one at every pass of the main thread */
int vpc_gl_paced(void);
/* the view's settings, one line (reports) */
void vpc_gl_describe(char *buf, size_t size);

#endif
