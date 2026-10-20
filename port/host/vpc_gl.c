/* vpc_gl.c: the OpenGL renderer (port/include/port_gl.h), on the main thread. Each present:

   1. the textures the capture registered since the last frame are uploaded (8-bit indices);
   2. the latest 3D scene is drawn into the scene framebuffer, at the window's resolution
      (4x multisampled unless PORT_GL_MSAA says otherwise), with a depth linear in camera z.
      The fragment shader finds each pixel's texel (the engine's texel coordinates,
      interpolated perspective-correctly), takes it through the polygon's shade row, a flat's
      second row and the distance fog's haze row as the span routines do, so the colours are
      the engine's own, then through the palette. F10 blends the four nearest texels, each
      through the tables and the palette (smooth textures). The world's polygons write
      depth; the flats test it and are drawn back to front, as the engine draws them. The
      scene's alpha is its coverage (fractional on the edges once resolved). The scene is
      drawn from where the eye is at this present (below), not only where it was when the
      game drew the frame;
   3. the composite: per pixel of the window, the software screen's pixel (VGA memory) where
      the game drew after the 3D pass (the screen differs from the copy taken after it) or
      outside the 3D window; else the scene over what was there before the 3D pass (the sky)
      or the background colour indoors.

   The view (docs/port.md, "Widescreen"): with aspect=fill the view fills the window. The
   engine draws a frame as much wider than 320 columns as the window is wider than 4:3 (the
   wide pass, port/engine_gl/wide.c: the focal stays the game's, so the field of view widens
   sideways), with the game's 320x200 screen as its middle; the HUD stays in that 4:3 column,
   the full height of the window, and the sides are the wide frame's. With aspect=4:3 the
   picture is 4:3, letterboxed, as the software path shows it. The HUD's pixels are scaled
   sharply: whole pixels, with a one-pixel blend at their edges where the scale is not whole.

   Smooth motion: the game draws about 54 frames a second; the window shows one at every
   refresh of the display (vsync). The eye's position and angles are kept with each frame,
   with the game's clock (its frame times summed), and each present draws the latest frame
   from the eye's place a little in the past (one and a half frames), between the two frames
   around it: the latest frame's geometry moved by the change of view. While the mouse looks
   around, the view's angles are the mouse's now (the motion the game has not read yet is
   added). The wide pass draws a margin of columns past each side that the window does not
   show, so a view moved this way has something at its edges.

   Settings (<overlay>/settings.txt; each PORT_GL_* environment variable overrides its line):
     aspect=4:3|fill        PORT_GL_ASPECT       the view's shape (Shift+F11 switches)
     fov_max=DEGREES        PORT_GL_FOV_MAX      the widest horizontal view (110; 0 none):
                                                 a wider window is pillarboxed
     hud_filter=sharp|nearest PORT_GL_HUD_FILTER how the 320x200 pixels are scaled
     vsync=on|adaptive|off  PORT_GL_VSYNC        presents paced by the display
     interpolate=on|off     PORT_GL_INTERP       smooth motion (Shift+F10 switches)
     overscan=COLUMNS       PORT_GL_OVERSCAN     the margin each side (24) */
#define GL_SILENCE_DEPRECATION
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <SDL3/SDL.h>
#include <OpenGL/gl3.h>

#include "port_gl.h"
#include "port_host.h"
#include "port_vpc.h"

static SDL_Window *window;
static SDL_GLContext context;
static int ready, gl_on = 1, smooth, msaa = 4;

/* the settings */
static int aspect_fill;                 /* the view fills the window (else 4:3) */
static int fov_max = 110;               /* degrees; 0 none */
static int hud_sharp = 1;
static int vsync = 1;                   /* 1 on, -1 adaptive, 0 off */
static int interp = 1;
static int overscan = 24;               /* columns past each side */

static GLuint scene_prog, comp_prog;
static GLuint vao, vbo, empty_vao;
static GLuint tex_vga, tex_pre, tex_post, tex_side, tex_tables, tex_pal, tex_falloff;
static GLuint scene_fbo, scene_colour, scene_depth;     /* (resolved) colour, and its FBO */
static GLuint ms_fbo, ms_colour, ms_depth;              /* the multisampled target */
static GLuint out_fbo, out_colour;
static int fbo_w, fbo_h;
static GLuint *textures;
static int ntextures;
static int *tsize;                      /* w, h per texture id */

/* scene uniforms */
static GLint u_view, u_frame, u_mode, u_colour, u_row2, u_fog, u_tsize, u_fogp, u_depth, u_tex,
    u_tables, u_pal, u_smooth, u_focal, u_nl, u_lbase, u_lights, u_falloff, u_move, u_shift,
    u_zoom;
/* composite uniforms */
static GLint c_vga, c_pre, c_post, c_side, c_scene, c_pal, c_valid, c_gl, c_sides, c_clip, c_bg,
    c_smooth, c_cross, c_map, c_sharp, c_scale, c_presh;

static const char *scene_vs =
    "#version 410 core\n"
    "layout(location = 0) in vec3 a_cam;\n"
    "layout(location = 1) in vec2 a_uv;\n"
    "layout(location = 2) in float a_shade;\n"
    "uniform vec4 u_view;\n"                /* centre x, y; half width, height (frame pixels) */
    "uniform vec2 u_frame;\n"               /* the frame's first column shown, the columns shown */
    "uniform mat3 u_move;\n"                /* the frame's camera space to the eye's now */
    "uniform vec3 u_shift;\n"
    "uniform float u_zoom;\n"               /* the overscan for a turn's new edge */
    "out vec2 v_uv;\n"
    "out float v_shade;\n"
    "out float v_z;\n"
    "out float v_zd;\n"
    "out vec3 v_cam;\n"
    "void main() {\n"
    "    vec3 c = u_move * a_cam + u_shift;\n"
    "    c.xy *= u_zoom;\n"
    "    float z = c.z;\n"
    /* the frame's column x = cx + x * hw / z, row y = cy + y * hh / z; the window shows the
       columns from u_frame.x, u_frame.y of them, and the 200 rows: as clip coordinates with
       w = z (GL clips what is behind the eye) */
    "    gl_Position = vec4((2.0 * (u_view.x - u_frame.x) / u_frame.y - 1.0) * z +\n"
    "                           c.x * 2.0 * u_view.z / u_frame.y,\n"
    "                       (1.0 - u_view.y / 100.0) * z - c.y * u_view.w / 100.0,\n"
    "                       0.0, z);\n"
    "    v_uv = a_uv;\n"
    "    v_shade = a_shade;\n"
    "    v_z = a_cam.z;\n"
    "    v_zd = z;\n"
    "    v_cam = a_cam;\n"
    "}\n";

static const char *scene_fs =
    "#version 410 core\n"
    "in vec2 v_uv;\n"
    "in float v_shade;\n"
    "in float v_z;\n"
    "in float v_zd;\n"
    "in vec3 v_cam;\n"
    "uniform vec4 u_view;\n"
    "uniform vec2 u_focal;\n"
    "uniform int u_nl;\n"                    /* a light shader: its lights (0: none) */
    "uniform float u_lbase;\n"
    "uniform float u_lights[15];\n"
    "uniform usampler2D u_falloff;\n"        /* LIGHT.DAT, 256 x 128 */
    "uniform usampler2D u_tex;\n"
    "uniform usampler2D u_tables;\n"        /* 256 x 128: the shade rows, then the haze rows */
    "uniform sampler2D u_pal;\n"
    "uniform int u_mode, u_colour, u_row2, u_fog, u_smooth;\n"
    "uniform ivec2 u_tsize;\n"
    "uniform vec3 u_fogp;\n"                /* start, step, far (camera z >> 8) */
    "uniform float u_depth;\n"
    "layout(location = 0) out vec4 o;\n"
    "uint tab(int row, uint c) { return texelFetch(u_tables, ivec2(int(c), row), 0).r; }\n"
    /* a texel through the polygon's rows and the fog, as the span routines map it */
    /* xn_light_shade at this pixel: its point in view space >> 8, each light's squared
       distance (the squares table's k * k << 8) times its falloff's high dword, LIGHT.DAT
       there times the intensity >> 12 on the base shade, to the last row */
    "float light_shade() {\n"
    "    vec3 p = vec3(v_cam.x * u_view.z / u_focal.x, v_cam.y * u_view.w / u_focal.y,\n"
    "                  v_cam.z) / 256.0;\n"
    "    float s = u_lbase;\n"
    "    for (int i = 0; i < 3; i++) {\n"
    "        if (i >= u_nl)\n"
    "            break;\n"
    "        vec3 k = floor(vec3(u_lights[i * 5], u_lights[i * 5 + 1], u_lights[i * 5 + 2]) +\n"
    "                       p);\n"
    "        float h = floor(256.0 * dot(k, k) * u_lights[i * 5 + 4] / 4294967296.0);\n"
    "        if (h < 32768.0) {\n"
    "            int hi = int(h);\n"
    "            float f = float(texelFetch(u_falloff, ivec2(hi & 255, hi >> 8), 0).r);\n"
    "            s += floor(f * u_lights[i * 5 + 3] / 4096.0);\n"
    "        }\n"
    "    }\n"
    "    return min(s, 63.0 * 256.0) / 256.0;\n"
    "}\n"
    "uint lit(uint c) {\n"
    "    if (u_nl > 0)\n"
    "        c = tab(clamp(int(floor(light_shade())), 0, 63), c);\n"
    "    else if (v_shade >= 0.0)\n"
    "        c = tab(clamp(int(floor(v_shade)), 0, 127), c);\n"
    "    if (u_row2 >= 0)\n"
    "        c = tab(u_row2, c);\n"
    "    if (u_fog != 0) {\n"
    "        float zz = min(v_z / 256.0, u_fogp.z);\n"
    "        if (zz > u_fogp.x) {\n"
    "            float b = 63.0 * 256.0 + u_fogp.y * (u_fogp.x - zz);\n"
    "            c = tab(64 + clamp(int(floor(b / 256.0)), 0, 63), c);\n"
    "        }\n"
    "    }\n"
    "    return c;\n"
    "}\n"
    "vec3 rgb(uint c) { return texelFetch(u_pal, ivec2(int(c), 0), 0).rgb; }\n"
    "uint texel(ivec2 t) {\n"
    "    if (u_mode == 0)\n"
    "        t = ivec2(mod(vec2(t), vec2(u_tsize)));\n"
    "    else\n"
    "        t = clamp(t, ivec2(0), u_tsize - 1);\n"
    "    return texelFetch(u_tex, t, 0).r;\n"
    "}\n"
    "void main() {\n"
    "    vec3 c;\n"
    "    if (u_mode == 1) {\n"
    "        c = rgb(lit(uint(u_colour)));\n"
    "    } else if (u_smooth == 0) {\n"
    "        uint t = texel(ivec2(floor(v_uv)));\n"
    "        if (u_mode >= 2 && t == 0u)\n"
    "            discard;\n"
    "        c = rgb(lit(t));\n"
    "    } else {\n"
    /* the four texels around the point, each lit and through the palette, blended; a
       flat's transparent texels weigh nothing */
    "        vec2 p = v_uv - 0.5;\n"
    "        vec2 f = fract(p);\n"
    "        ivec2 b = ivec2(floor(p));\n"
    "        vec4 sum = vec4(0.0);\n"
    "        for (int k = 0; k < 4; k++) {\n"
    "            ivec2 d = ivec2(k & 1, k >> 1);\n"
    "            float w = (d.x == 1 ? f.x : 1.0 - f.x) * (d.y == 1 ? f.y : 1.0 - f.y);\n"
    "            uint t = texel(b + d);\n"
    "            if (u_mode >= 2 && t == 0u)\n"
    "                continue;\n"
    "            sum += vec4(rgb(lit(t)) * w, w);\n"
    "        }\n"
    "        if (sum.a < 0.5)\n"
    "            discard;\n"
    "        c = sum.rgb / sum.a;\n"
    "    }\n"
    "    if (u_mode == 3 && ((int(gl_FragCoord.x) + int(gl_FragCoord.y)) & 1) == 1)\n"
    "        discard;\n"
    "    o = vec4(c, 1.0);\n"
    "    gl_FragDepth = clamp(v_zd * u_depth, 0.0, 1.0);\n"
    "}\n";

/* a triangle over the whole viewport; v_pos is (0, 0) at its top left, (1, 1) bottom right */
static const char *comp_vs =
    "#version 410 core\n"
    "out vec2 v_pos;\n"
    "void main() {\n"
    "    vec2 p = vec2(float((gl_VertexID & 1) * 4 - 1), float((gl_VertexID & 2) * 2 - 1));\n"
    "    v_pos = vec2(p.x * 0.5 + 0.5, 0.5 - p.y * 0.5);\n"
    "    gl_Position = vec4(p, 0.0, 1.0);\n"
    "}\n";

/* The composite works on the frame's pixels: the frame is the wide pass's (or the game's
   320 columns), the window shows u_map.y of its columns from u_map.x, all 200 rows. The game's
   screen (VGA) is the frame's columns u_map.z.. u_map.z + 319; the sides are the wide frame
   as the game left it (u_side). A frame pixel is the 3D scene where the 3D pass's pixel is
   still there (the screen equals the copy after the pass), else the software pixel. */
static const char *comp_fs =
    "#version 410 core\n"
    "in vec2 v_pos;\n"
    "uniform usampler2D u_vga, u_pre, u_post, u_side;\n"
    "uniform sampler2D u_scene, u_pal;\n"
    "uniform int u_valid;\n"                /* a 3D frame goes with this picture */
    "uniform int u_gl;\n"                   /* drawn by OpenGL (else the software pixels) */
    "uniform int u_sides;\n"                /* the wide frame's sides are there to show */
    "uniform ivec4 u_clip;\n"               /* the 3D window: left, top, right, bottom */
    "uniform ivec2 u_bg;\n"                 /* fill?, colour */
    "uniform int u_smooth;\n"
    "uniform vec3 u_cross;\n"              /* the crosshair: x, y (the game's screen), on */
    "uniform vec4 u_map;\n"                /* first column shown, columns shown, the game's
                                              screen's column 0, the frame's width */
    "uniform int u_sharp;\n"
    "uniform vec2 u_scale;\n"              /* window pixels a frame pixel */
    "uniform vec2 u_presh;\n"              /* the sky backdrop's move with the scene's (frame
                                              pixels): the panorama a column a step of yaw,
                                              the horizon with the pitch */
    "layout(location = 0) out vec4 o;\n"
    "vec3 pal(uint c) { return texelFetch(u_pal, ivec2(int(c), 0), 0).rgb; }\n"
    "int frame_w() { return int(u_map.w); }\n"
    /* the picture before the 3D pass at q (frame pixels), filtered through the palette */
    "vec3 pre_smooth(vec2 q) {\n"
    "    vec2 p = q - 0.5;\n"
    "    vec2 f = fract(p);\n"
    "    ivec2 b = ivec2(floor(p));\n"
    "    vec3 s = vec3(0.0);\n"
    "    for (int k = 0; k < 4; k++) {\n"
    "        ivec2 d = ivec2(k & 1, k >> 1);\n"
    "        float w = (d.x == 1 ? f.x : 1.0 - f.x) * (d.y == 1 ? f.y : 1.0 - f.y);\n"
    "        s += pal(texelFetch(u_pre, clamp(b + d, ivec2(0), ivec2(frame_w() - 1, 199)), 0).r) * w;\n"
    "    }\n"
    "    return s;\n"
    "}\n"
    /* the crosshair at the window's resolution: a white plus, its arms 1.2 to 4.2 pixels from
       the centre and 0.6 of a pixel thick (a pixel is 1.2 times as tall as wide), in a dark
       outline */
    "float cross_at(vec2 q, float grow) {\n"
    "    vec2 d = abs(q - u_cross.xy) * vec2(1.0, 1.2);\n"
    "    float t = 0.3 + grow, a = 1.2 - grow, b = 4.2 + grow;\n"
    "    return (d.x < t && d.y > a && d.y < b) || (d.y < t && d.x > a && d.x < b) ? 1.0 : 0.0;\n"
    "}\n"
    /* the scene here (its colour premultiplied by its coverage) over what is behind it */
    "vec3 scene_at(vec2 q, ivec2 p) {\n"
    "    vec4 s = texelFetch(u_scene, ivec2(gl_FragCoord.xy), 0);\n"
    "    if (s.a >= 1.0 || u_bg.x != 0)\n"
    "        return u_bg.x != 0 ? s.rgb + pal(uint(u_bg.y)) * (1.0 - s.a) : s.rgb;\n"
    "    vec2 k = q + u_presh;\n"
    "    ivec2 kp = clamp(ivec2(floor(k)), ivec2(0), ivec2(frame_w() - 1, 199));\n"
    "    vec3 u = u_smooth != 0 ? pre_smooth(k) : pal(texelFetch(u_pre, kp, 0).r);\n"
    "    return s.rgb + u * (1.0 - s.a);\n"
    "}\n"
    /* the frame's pixel p (q: this fragment's point in the frame) */
    "vec3 frame_px(ivec2 p, vec2 q) {\n"
    "    int dx = int(u_map.z);\n"
    "    p.y = clamp(p.y, 0, 199);\n"
    "    bool in3d = u_valid != 0 && p.x >= u_clip.x && p.x < u_clip.z && p.y >= u_clip.y &&\n"
    "                p.y < u_clip.w;\n"
    "    if (p.x >= dx && p.x < dx + 320) {\n"
    "        uint c = texelFetch(u_vga, ivec2(p.x - dx, p.y), 0).r;\n"
    "        if (in3d && u_gl != 0 && c == texelFetch(u_post, p, 0).r)\n"
    "            return scene_at(q, p);\n"
    "        return pal(c);\n"
    "    }\n"
    /* a side: the wide frame's, within its 3D window's rows (below them it is not drawn) */
    "    if (u_sides == 0 || p.x < 0 || p.x >= frame_w() || p.y >= u_clip.w)\n"
    "        return vec3(0.0);\n"
    "    uint c = texelFetch(u_side, p, 0).r;\n"
    "    if (in3d && u_gl != 0 && c == texelFetch(u_post, p, 0).r)\n"
    "        return scene_at(q, p);\n"
    "    return pal(c);\n"
    "}\n"
    "void main() {\n"
    "    vec2 q = vec2(u_map.x + v_pos.x * u_map.y, v_pos.y * 200.0);\n"
    "    if (u_cross.z > 0.5) {\n"
    "        vec2 h = q - vec2(u_map.z, 0.0);\n"
    "        if (cross_at(h, 0.0) > 0.5) { o = vec4(1.0); return; }\n"
    "        if (cross_at(h, 0.25) > 0.5) { o = vec4(0.0, 0.0, 0.0, 1.0); return; }\n"
    "    }\n"
    "    if (u_sharp == 0) {\n"
    "        o = vec4(frame_px(ivec2(floor(q)), q), 1.0);\n"
    "        return;\n"
    "    }\n"
    /* sharp: each pixel whole, blended with its neighbour only over the window pixel that
       straddles their edge */
    "    vec2 g = q - 0.5;\n"
    "    vec2 b = floor(g);\n"
    "    vec2 t = clamp((g - b - 0.5) * u_scale + 0.5, 0.0, 1.0);\n"
    "    ivec2 i = ivec2(b);\n"
    "    vec3 c00 = frame_px(i, q);\n"
    "    vec3 c10 = t.x > 0.0 ? frame_px(i + ivec2(1, 0), q) : c00;\n"
    "    vec3 c01 = t.y > 0.0 ? frame_px(i + ivec2(0, 1), q) : c00;\n"
    "    vec3 c11 = t.x > 0.0 && t.y > 0.0 ? frame_px(i + ivec2(1, 1), q) : c00;\n"
    "    o = vec4(mix(mix(c00, c10, t.x), mix(c01, c11, t.x), t.y), 1.0);\n"
    "}\n";

static GLuint compile(GLenum type, const char *src)
{
    GLuint s = glCreateShader(type);
    GLint ok;

    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];

        glGetShaderInfoLog(s, sizeof log, NULL, log);
        fprintf(stderr, "port: GL shader: %s\n", log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

static GLuint program(const char *vs, const char *fs)
{
    GLuint v = compile(GL_VERTEX_SHADER, vs), f = compile(GL_FRAGMENT_SHADER, fs), p;
    GLint ok;

    if (!v || !f)
        return 0;
    p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];

        glGetProgramInfoLog(p, sizeof log, NULL, log);
        fprintf(stderr, "port: GL program: %s\n", log);
        return 0;
    }
    return p;
}

static GLuint index_texture(int w, int h)
{
    GLuint t;

    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    return t;
}

/* ---- settings ------------------------------------------------------------------------------- */

/* a setting: its environment variable, else its line in the settings file */
static int setting(const char *env, const char *key, char *out, size_t size)
{
    const char *e = SDL_getenv(env);

    if (e && *e) {
        snprintf(out, size, "%s", e);
        return 1;
    }
    return launch_setting(key, out, size);
}

static int is_on(const char *v)
{
    return strcasecmp(v, "on") == 0 || strcmp(v, "1") == 0 || strcasecmp(v, "yes") == 0 ||
           strcasecmp(v, "true") == 0;
}

static void read_settings(void)
{
    char v[64];

    if (setting("PORT_GL_ASPECT", "aspect", v, sizeof v))
        aspect_fill = strcasecmp(v, "fill") == 0 || strcasecmp(v, "wide") == 0;
    if (setting("PORT_GL_FOV_MAX", "fov_max", v, sizeof v))
        fov_max = atoi(v);
    if (setting("PORT_GL_HUD_FILTER", "hud_filter", v, sizeof v))
        hud_sharp = strcasecmp(v, "nearest") != 0;
    if (setting("PORT_GL_VSYNC", "vsync", v, sizeof v))
        vsync = strcasecmp(v, "adaptive") == 0 ? -1 : is_on(v) ? 1 : 0;
    if (setting("PORT_GL_INTERP", "interpolate", v, sizeof v))
        interp = is_on(v);
    if (setting("PORT_GL_OVERSCAN", "overscan", v, sizeof v))
        overscan = atoi(v);
    if (fov_max < 0 || fov_max >= 175)
        fov_max = 0;
    if (overscan < 0)
        overscan = 0;
    if (overscan > 64)
        overscan = 64;
}

int vpc_gl_init(void *sdl_window)
{
    const char *env = SDL_getenv("PORT_GL_SOFTWARE");

    window = sdl_window;
    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        fprintf(stderr, "port: SDL_GL_CreateContext: %s\n", SDL_GetError());
        return 0;
    }
    SDL_GL_MakeCurrent(window, context);
    read_settings();
    if (vsync < 0 && !SDL_GL_SetSwapInterval(-1))
        vsync = 1;
    if (vsync >= 0)
        SDL_GL_SetSwapInterval(vsync);
    fprintf(stderr, "port: OpenGL %s, %s; view %s, vsync %s, smooth motion %s\n",
            (const char *)glGetString(GL_VERSION), (const char *)glGetString(GL_RENDERER),
            aspect_fill ? "fill" : "4:3", vsync < 0 ? "adaptive" : vsync ? "on" : "off",
            interp ? "on" : "off");

    scene_prog = program(scene_vs, scene_fs);
    comp_prog = program(comp_vs, comp_fs);
    if (!scene_prog || !comp_prog)
        return 0;
    u_view = glGetUniformLocation(scene_prog, "u_view");
    u_frame = glGetUniformLocation(scene_prog, "u_frame");
    u_mode = glGetUniformLocation(scene_prog, "u_mode");
    u_colour = glGetUniformLocation(scene_prog, "u_colour");
    u_row2 = glGetUniformLocation(scene_prog, "u_row2");
    u_fog = glGetUniformLocation(scene_prog, "u_fog");
    u_tsize = glGetUniformLocation(scene_prog, "u_tsize");
    u_fogp = glGetUniformLocation(scene_prog, "u_fogp");
    u_depth = glGetUniformLocation(scene_prog, "u_depth");
    u_tex = glGetUniformLocation(scene_prog, "u_tex");
    u_tables = glGetUniformLocation(scene_prog, "u_tables");
    u_pal = glGetUniformLocation(scene_prog, "u_pal");
    u_smooth = glGetUniformLocation(scene_prog, "u_smooth");
    u_focal = glGetUniformLocation(scene_prog, "u_focal");
    u_move = glGetUniformLocation(scene_prog, "u_move");
    u_shift = glGetUniformLocation(scene_prog, "u_shift");
    u_zoom = glGetUniformLocation(scene_prog, "u_zoom");
    u_nl = glGetUniformLocation(scene_prog, "u_nl");
    u_lbase = glGetUniformLocation(scene_prog, "u_lbase");
    u_lights = glGetUniformLocation(scene_prog, "u_lights");
    u_falloff = glGetUniformLocation(scene_prog, "u_falloff");
    c_vga = glGetUniformLocation(comp_prog, "u_vga");
    c_pre = glGetUniformLocation(comp_prog, "u_pre");
    c_post = glGetUniformLocation(comp_prog, "u_post");
    c_side = glGetUniformLocation(comp_prog, "u_side");
    c_scene = glGetUniformLocation(comp_prog, "u_scene");
    c_pal = glGetUniformLocation(comp_prog, "u_pal");
    c_valid = glGetUniformLocation(comp_prog, "u_valid");
    c_gl = glGetUniformLocation(comp_prog, "u_gl");
    c_sides = glGetUniformLocation(comp_prog, "u_sides");
    c_clip = glGetUniformLocation(comp_prog, "u_clip");
    c_bg = glGetUniformLocation(comp_prog, "u_bg");
    c_smooth = glGetUniformLocation(comp_prog, "u_smooth");
    c_cross = glGetUniformLocation(comp_prog, "u_cross");
    c_map = glGetUniformLocation(comp_prog, "u_map");
    c_sharp = glGetUniformLocation(comp_prog, "u_sharp");
    c_scale = glGetUniformLocation(comp_prog, "u_scale");
    c_presh = glGetUniformLocation(comp_prog, "u_presh");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pgl_vertex),
                          (void *)offsetof(pgl_vertex, x));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(pgl_vertex),
                          (void *)offsetof(pgl_vertex, u));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(pgl_vertex),
                          (void *)offsetof(pgl_vertex, shade));
    glGenVertexArrays(1, &empty_vao);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    tex_vga = index_texture(PGL_W, PGL_H);
    tex_pre = index_texture(PGL_MAX_W, PGL_H);
    tex_post = index_texture(PGL_MAX_W, PGL_H);
    tex_side = index_texture(PGL_MAX_W, PGL_H);
    tex_tables = index_texture(256, 128);
    glGenTextures(1, &tex_falloff);
    glBindTexture(GL_TEXTURE_2D, tex_falloff);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, 256, 128, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT,
                 NULL);
    glGenTextures(1, &tex_pal);
    glBindTexture(GL_TEXTURE_2D, tex_pal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    if (env && *env == '1')
        gl_on = 0;
    env = SDL_getenv("PORT_GL_MSAA");
    if (env)
        msaa = atoi(env);
    {
        GLint max = 1;

        glGetIntegerv(GL_MAX_SAMPLES, &max);
        if (msaa > max)
            msaa = max;
        if (msaa < 1)
            msaa = 1;
    }
    env = SDL_getenv("PORT_GL_SMOOTH");
    smooth = env && *env == '1';
    ready = 1;
    pgl_enabled = 1;
    return 1;
}

void vpc_gl_shutdown(void)
{
    pgl_enabled = 0;
    pgl_wide_request = 0;
    if (context)
        SDL_GL_DestroyContext(context);
    context = NULL;
    ready = 0;
}

void vpc_gl_toggle(void)
{
    gl_on = !gl_on;
    fprintf(stderr, "port: 3D view by %s\n", gl_on ? "OpenGL" : "the software renderer");
}

void vpc_gl_toggle_smooth(void)
{
    smooth = !smooth;
    fprintf(stderr, "port: OpenGL textures %s\n", smooth ? "smooth" : "as texels");
}

void vpc_gl_toggle_aspect(void)
{
    aspect_fill = !aspect_fill;
    launch_set_setting("aspect", aspect_fill ? "fill" : "4:3");
    fprintf(stderr, "port: the view %s\n", aspect_fill ? "fills the window" : "is 4:3");
}

void vpc_gl_toggle_interp(void)
{
    interp = !interp;
    launch_set_setting("interpolate", interp ? "on" : "off");
    fprintf(stderr, "port: smooth motion %s\n", interp ? "on" : "off");
}

int vpc_gl_paced(void)
{
    return ready && vsync != 0;
}

void vpc_gl_describe(char *buf, size_t size)
{
    snprintf(buf, size, "gl=%s aspect=%s fov_max=%d hud_filter=%s vsync=%s interpolate=%s "
             "overscan=%d frame=%d", gl_on ? "on" : "off", aspect_fill ? "fill" : "4:3", fov_max,
             hud_sharp ? "sharp" : "nearest", vsync < 0 ? "adaptive" : vsync ? "on" : "off",
             interp ? "on" : "off", overscan, pgl_wide_request);
}

/* ---- textures ------------------------------------------------------------------------------ */

static void upload_textures(void)
{
    int id, w, h;
    unsigned char *px;

    while (pgl_texture_next(&id, &w, &h, &px)) {
        if (id >= ntextures) {
            int n = ntextures ? ntextures : 1024;

            while (n <= id)
                n *= 2;
            textures = realloc(textures, (size_t)n * sizeof *textures);
            tsize = realloc(tsize, (size_t)n * 2 * sizeof *tsize);
            memset(textures + ntextures, 0, (size_t)(n - ntextures) * sizeof *textures);
            ntextures = n;
        }
        if (textures[id] == 0)
            textures[id] = index_texture(w, h);
        else {
            glBindTexture(GL_TEXTURE_2D, textures[id]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE,
                         NULL);
        }
        glBindTexture(GL_TEXTURE_2D, textures[id]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED_INTEGER, GL_UNSIGNED_BYTE, px);
        tsize[id * 2] = w;
        tsize[id * 2 + 1] = h;
        free(px);
    }
}

/* ---- framebuffers ---------------------------------------------------------------------------- */

static void make_fbos(int w, int h)
{
    if (w == fbo_w && h == fbo_h)
        return;
    if (scene_fbo) {
        glDeleteFramebuffers(1, &scene_fbo);
        glDeleteTextures(1, &scene_colour);
        glDeleteRenderbuffers(1, &scene_depth);
        glDeleteFramebuffers(1, &out_fbo);
        glDeleteTextures(1, &out_colour);
        if (ms_fbo) {
            glDeleteFramebuffers(1, &ms_fbo);
            glDeleteRenderbuffers(1, &ms_colour);
            glDeleteRenderbuffers(1, &ms_depth);
            ms_fbo = 0;
        }
    }
    fbo_w = w;
    fbo_h = h;

    glGenTextures(1, &scene_colour);
    glBindTexture(GL_TEXTURE_2D, scene_colour);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glGenRenderbuffers(1, &scene_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, scene_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, w, h);
    glGenFramebuffers(1, &scene_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_colour, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, scene_depth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "port: GL scene framebuffer incomplete\n");

    if (msaa > 1) {
        glGenRenderbuffers(1, &ms_colour);
        glBindRenderbuffer(GL_RENDERBUFFER, ms_colour);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGBA8, w, h);
        glGenRenderbuffers(1, &ms_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, ms_depth);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT32F, w, h);
        glGenFramebuffers(1, &ms_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, ms_fbo);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ms_colour);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ms_depth);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "port: GL multisampled framebuffer incomplete: no MSAA\n");
            glDeleteFramebuffers(1, &ms_fbo);
            ms_fbo = 0;
        }
    }

    glGenTextures(1, &out_colour);
    glBindTexture(GL_TEXTURE_2D, out_colour);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glGenFramebuffers(1, &out_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, out_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_colour, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* ---- the view: the window's rectangle and the frame's columns in it ----------------------- */

/* The window's picture: its rectangle (window pixels) and the frame columns it shows (cols:
   320 for 4:3, more for a wider window with aspect=fill). A 320x200 picture at 4:3 has pixels
   1.2 times as tall as wide, so a view of aspect A is 240 * A columns. */
static void view_rect(int *x, int *y, int *w, int *h, int *cols)
{
    int ww, wh, c = PGL_W;

    SDL_GetWindowSizeInPixels(window, &ww, &wh);
    if (ww < 1 || wh < 1) {
        *x = *y = 0;
        *w = *h = 0;
        *cols = PGL_W;
        return;
    }
    if (aspect_fill && ww * 3 > wh * 4) {
        int most = PGL_MAX_W - 2 * overscan;

        c = (int)(240.0 * ww / wh + 0.5) & ~1;
        if (fov_max > 0) {
            int f = (int)(2.0 * 200.0 * tan(fov_max * M_PI / 360.0)) & ~1;

            if (f < most)
                most = f;
        }
        if (c > most)
            c = most;
        if (c < PGL_W)
            c = PGL_W;
        *h = wh;
        *w = (int)((double)wh * c / 240.0 + 0.5);
        if (*w > ww)
            *w = ww;
    } else if (ww * 3 > wh * 4) {
        *h = wh;
        *w = wh * 4 / 3;
    } else {
        *w = ww;
        *h = ww * 3 / 4;
    }
    *x = (ww - *w) / 2;
    *y = (wh - *h) / 2;
    *cols = c;
}

/* ---- the eye's motion: frames' poses, and where the eye is at a present --------------------- */

typedef struct pose {
    double t;                           /* the game's clock, ms */
    double host;                        /* the host's, ms, when handed over */
    double x, y, z;                     /* world units */
    double pitch, yaw, roll;            /* 2048ths of a turn */
    unsigned long serial;
} pose;

#define HISTORY 8
static pose hist[HISTORY];
static int nhist;                       /* the newest last */
static double clock_off, frame_dt = 18.5, last_render_t;
static int clock_set;
/* PORT_GL_DEBUG */
static int st_snaps, st_held, st_scenes;
static double st_alpha_min = 1e9, st_alpha_max = -1e9;

static double wrap(double a)             /* to -1024..1024 */
{
    a = fmod(a, 2048.0);
    if (a >= 1024.0)
        a -= 2048.0;
    if (a < -1024.0)
        a += 2048.0;
    return a;
}

static void push_pose(const pgl_scene *s)
{
    pose p;
    double dt = 0;

    p.t = s->game_ms;
    p.host = (double)s->host_ns / 1e6;
    p.x = s->cam_x;
    p.y = s->cam_y;
    p.z = s->cam_z;
    p.pitch = s->cam_pitch;
    p.yaw = s->cam_yaw;
    p.roll = s->cam_roll;
    p.serial = s->serial;
    st_scenes++;
    if (nhist > 0) {
        const pose *q = &hist[nhist - 1];
        double d = fabs(p.x - q->x) + fabs(p.y - q->y) + fabs(p.z - q->z);

        dt = p.t - q->t;
        /* a cut (a teleport, a door to another place, a load): no motion between them */
        if (dt <= 0 || dt > 250 || d > 4000 || fabs(wrap(p.yaw - q->yaw)) > 300 ||
            fabs(wrap(p.pitch - q->pitch)) > 300) {
            nhist = 0;
            st_snaps++;
        }
    }
    if (nhist == HISTORY) {
        memmove(hist, hist + 1, (HISTORY - 1) * sizeof *hist);
        nhist--;
    }
    hist[nhist++] = p;
    if (nhist >= 2) {
        if (dt > 0 && dt < 100)
            frame_dt += (dt - frame_dt) * 0.1;
        clock_off += (p.t - p.host - clock_off) * 0.05;
        if (fabs(p.t - p.host - clock_off) > 100)
            clock_off = p.t - p.host;
    } else {
        clock_off = p.t - p.host;
        last_render_t = -1e18;
    }
    clock_set = 1;
}

/* The game keeps the eye in whole world units, so a slow move (along a wall, up a slope)
   comes as a unit every few frames and a line between two frames is a step, then nothing.
   Between frames the eye's position is a line fitted to the frames within two frame times of
   t, nearer ones weighing more: the frames' own positions where the motion is even, and an
   even motion where the units come unevenly. It needs frames on both sides of t and three in
   all (else the line between the two frames stands). */
static void fit_position(double t, pose *out)
{
    double span = 2.0 * frame_dt, sw = 0, st = 0, sx = 0, sy = 0, sz = 0;
    double stt = 0, stx = 0, sty = 0, stz = 0, mt, d;
    int i, n = 0, before = 0, after = 0;
    static int on = -1;

    if (on < 0) {
        const char *e = SDL_getenv("PORT_GL_FIT");     /* 0: the line between two frames */

        on = !(e && *e == '0');
    }
    if (!on)
        return;
    for (i = 0; i < nhist; i++) {
        double w = 1.0 - fabs(hist[i].t - t) / span;

        if (w <= 0)
            continue;
        n++;
        before += hist[i].t <= t;
        after += hist[i].t >= t;
        sw += w;
        st += w * hist[i].t;
        sx += w * hist[i].x;
        sy += w * hist[i].y;
        sz += w * hist[i].z;
    }
    if (n < 3 || !before || !after)
        return;
    mt = st / sw;
    for (i = 0; i < nhist; i++) {
        double w = 1.0 - fabs(hist[i].t - t) / span, dt = hist[i].t - mt;

        if (w <= 0)
            continue;
        stt += w * dt * dt;
        stx += w * dt * (hist[i].x - sx / sw);
        sty += w * dt * (hist[i].y - sy / sw);
        stz += w * dt * (hist[i].z - sz / sw);
    }
    if (stt <= 0)
        return;
    d = t - mt;
    out->x = sx / sw + stx / stt * d;
    out->y = sy / sw + sty / stt * d;
    out->z = sz / sw + stz / stt * d;
}

/* the eye at the game's time t (between the frames around it; the newest's after it) */
static void pose_at(double t, pose *out)
{
    int i;

    *out = hist[nhist - 1];
    if (nhist < 2 || t >= hist[nhist - 1].t) {
        st_held += nhist >= 2 && t > hist[nhist - 1].t + 1.0;
        return;
    }
    if (t <= hist[0].t) {
        *out = hist[0];
        return;
    }
    for (i = nhist - 1; i > 0; i--)
        if (hist[i - 1].t <= t) {
            const pose *a = &hist[i - 1], *b = &hist[i];
            double k = (t - a->t) / (b->t - a->t);

            if (k < st_alpha_min)
                st_alpha_min = k;
            if (k > st_alpha_max)
                st_alpha_max = k;
            out->x = a->x + (b->x - a->x) * k;
            out->y = a->y + (b->y - a->y) * k;
            out->z = a->z + (b->z - a->z) * k;
            fit_position(t, out);
            out->pitch = a->pitch + wrap(b->pitch - a->pitch) * k;
            out->yaw = a->yaw + wrap(b->yaw - a->yaw) * k;
            out->roll = a->roll + wrap(b->roll - a->roll) * k;
            out->t = t;
            return;
        }
}

/* xn_mat_from_angles (src/engine/mat.c), in floating point: world to view */
static void rotation(double pitch, double yaw, double roll, double m[3][3])
{
    const double a = 2.0 * M_PI / 2048.0;
    double sp = sin(pitch * a), cp = cos(pitch * a), sy = sin(yaw * a), cy = cos(yaw * a);
    double sr = sin(roll * a), cr = cos(roll * a);

    m[0][0] = sp * (sr * sy) + cr * cy;
    m[0][1] = -sr * cp;
    m[0][2] = sr * (sp * cy) - sy * cr;
    m[1][0] = -sp * (sy * cr) + sr * cy;
    m[1][1] = cp * cr;
    m[1][2] = -cr * (sp * cy) - sr * sy;
    m[2][0] = sy * cp;
    m[2][1] = sp;
    m[2][2] = cp * cy;
}

/* The scene's camera space (the view's rows scaled so the side planes are x = +-z) moved to
   the eye at e: c' = S Rt Rs^T S^-1 c + S Rt (eye_s - eye_t) << 8 */
static void move_matrix(const pgl_scene *s, const pose *e, float m3[9], float shift[3])
{
    double rs[3][3], rt[3][3], m[3][3], sc[3], d[3], v[3];
    int i, j, k;

    rotation(s->cam_pitch, s->cam_yaw, s->cam_roll, rs);
    rotation(e->pitch, e->yaw, e->roll, rt);
    sc[0] = s->hw > 0 ? s->focal_x / s->hw : 1.0;
    sc[1] = s->hh > 0 ? s->focal_y / s->hh : 1.0;
    sc[2] = 1.0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++) {
            double t = 0;

            for (k = 0; k < 3; k++)
                t += rt[i][k] * rs[j][k];
            m[i][j] = sc[i] * t / sc[j];
        }
    d[0] = (s->cam_x - e->x) * 256.0;
    d[1] = (s->cam_y - e->y) * 256.0;
    d[2] = (s->cam_z - e->z) * 256.0;
    for (i = 0; i < 3; i++) {
        v[i] = 0;
        for (k = 0; k < 3; k++)
            v[i] += rt[i][k] * d[k];
        shift[i] = (float)(sc[i] * v[i]);
    }
    /* column-major for GL: m3[col * 3 + row] */
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            m3[j * 3 + i] = (float)m[i][j];
}

/* ---- the scene ------------------------------------------------------------------------------- */

static const pgl_scene *sort_scene;

static int by_state(const void *a, const void *b)
{
    const pgl_draw *p = &sort_scene->draws[*(const int *)a];
    const pgl_draw *q = &sort_scene->draws[*(const int *)b];

    if (p->mode != q->mode)
        return p->mode - q->mode;
    if (p->tex != q->tex)
        return p->tex - q->tex;
    if (p->colour != q->colour)
        return p->colour - q->colour;
    if (p->row2 != q->row2)
        return p->row2 - q->row2;
    if (p->fog != q->fog)
        return p->fog - q->fog;
    return *(const int *)a - *(const int *)b;
}

static int same_state(const pgl_draw *p, const pgl_draw *q)
{
    return p->mode == q->mode && p->tex == q->tex && p->colour == q->colour &&
           p->row2 == q->row2 && p->fog == q->fog && p->layer == q->layer &&
           p->nlights == 0 && q->nlights == 0;
}

static void set_draw_state(const pgl_draw *d)
{
    glUniform1i(u_mode, d->mode);
    glUniform1i(u_colour, d->colour);
    glUniform1i(u_row2, d->row2);
    glUniform1i(u_fog, d->fog);
    glUniform1i(u_nl, d->nlights);
    if (d->nlights) {
        glUniform1f(u_lbase, d->lbase);
        glUniform1fv(u_lights, 15, &d->lights[0][0]);
    }
    if (d->tex >= 0 && d->tex < ntextures && textures[d->tex]) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[d->tex]);
        glUniform2i(u_tsize, tsize[d->tex * 2], tsize[d->tex * 2 + 1]);
    }
}

/* runs of draws with one state as one glMultiDrawArrays each */
static void draw_runs(const pgl_scene *s, const int *order, int n)
{
    static GLint *firsts;
    static GLsizei *counts;
    static int cap;
    int i = 0;

    if (n > cap) {
        cap = n * 2;
        firsts = realloc(firsts, (size_t)cap * sizeof *firsts);
        counts = realloc(counts, (size_t)cap * sizeof *counts);
    }
    while (i < n) {
        const pgl_draw *d = &s->draws[order[i]];
        int j = i, k = 0;

        if (d->mode != PGL_SOLID && (d->tex < 0 || d->tex >= ntextures || !textures[d->tex])) {
            i++;
            continue;
        }
        set_draw_state(d);
        /* the run: this draw, and those after it with the same state (a lit draw has
           its own light uniforms, so its run is itself) */
        while (j < n && (j == i || same_state(&s->draws[order[j]], d))) {
            if (s->draws[order[j]].count > 0) {
                firsts[k] = s->draws[order[j]].first;
                counts[k] = s->draws[order[j]].count;
                k++;
            }
            j++;
        }
        if (k)
            glMultiDrawArrays(GL_TRIANGLES, firsts, counts, k);
        i = j;
    }
}

/* how the scene is drawn at this present: the frame's columns shown (from c0, cols of them),
   the eye's move from where the frame was drawn, and a zoom (the vertical overscan while the
   mouse turns the view) */
typedef struct view {
    float c0, cols;
    float move[9], shift[3];
    float zoom;
    float presh[2];             /* the sky backdrop's move (composite) */
} view;

static void draw_scene(const pgl_scene *s, const view *v)
{
    static int *order;
    static int cap;
    int i, nworld = 0, nflat = 0;
    int x0, y0, x1, y1;
    float depth_far = s->far_z > 0 ? s->far_z * 2.0f : 1e9f;

    glBindFramebuffer(GL_FRAMEBUFFER, ms_fbo ? ms_fbo : scene_fbo);
    glViewport(0, 0, fbo_w, fbo_h);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* the 3D window, in the window's pixels */
    x0 = (int)floor((s->clip_l - v->c0) * fbo_w / v->cols);
    x1 = (int)ceil((s->clip_r - v->c0) * fbo_w / v->cols);
    y0 = fbo_h - (s->clip_b * fbo_h + PGL_H - 1) / PGL_H;
    y1 = fbo_h - s->clip_t * fbo_h / PGL_H;
    if (x0 < 0)
        x0 = 0;
    if (x1 > fbo_w)
        x1 = fbo_w;
    if (y0 < 0)
        y0 = 0;
    if (x1 <= x0 || y1 <= y0)
        return;
    glEnable(GL_SCISSOR_TEST);
    glScissor(x0, y0, x1 - x0, y1 - y0);

    glUseProgram(scene_prog);
    glUniform4f(u_view, s->cx, s->cy, s->hw, s->hh);
    glUniform2f(u_frame, v->c0, v->cols);
    glUniformMatrix3fv(u_move, 1, GL_FALSE, v->move);
    glUniform3f(u_shift, v->shift[0], v->shift[1], v->shift[2]);
    glUniform1f(u_zoom, v->zoom);
    glUniform3f(u_fogp, s->fog_start, s->fog_step, s->fog_far);
    glUniform1f(u_depth, 1.0f / depth_far);
    glUniform1i(u_tex, 0);
    glUniform1i(u_tables, 1);
    glUniform1i(u_pal, 2);
    glUniform1i(u_smooth, smooth);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_pal);
    glUniform1i(u_falloff, 3);
    glUniform2f(u_focal, s->focal_x, s->focal_y);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tex_falloff);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 128, GL_RED_INTEGER, GL_UNSIGNED_SHORT,
                    s->falloff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_tables);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 128, GL_RED_INTEGER, GL_UNSIGNED_BYTE,
                    s->tables);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)s->nverts * (GLsizeiptr)sizeof(pgl_vertex),
                 s->verts, GL_STREAM_DRAW);

    if (s->ndraws > cap) {
        cap = s->ndraws * 2;
        order = realloc(order, (size_t)cap * sizeof *order);
    }
    for (i = 0; i < s->ndraws; i++)
        if (s->draws[i].layer == 0)
            order[nworld++] = i;
    sort_scene = s;
    qsort(order, (size_t)nworld, sizeof *order, by_state);
    for (i = 0; i < s->ndraws; i++)
        if (s->draws[i].layer == 1)
            order[nworld + nflat++] = i;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    draw_runs(s, order, nworld);
    /* the flats: back to front, tested against the world, not each other */
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    draw_runs(s, order + nworld, nflat);
    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    if (ms_fbo) {               /* resolve: the edges' coverage becomes their alpha */
        glBindFramebuffer(GL_READ_FRAMEBUFFER, ms_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, scene_fbo);
        glBlitFramebuffer(0, 0, fbo_w, fbo_h, 0, 0, fbo_w, fbo_h, GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);
    }
}

/* the share of the 3D window's pixels the game changed after the 3D pass: a scene the game
   has drawn over (a menu) is not shown. Only the game's screen can be drawn over: its
   columns of the frame. */
static int scene_current(const pgl_scene *s, const unsigned char *vga)
{
    int x, y, n = 0, diff = 0;

    for (y = s->clip_t; y < s->clip_b && y < PGL_H; y += 2)
        for (x = s->clip_l; x < s->clip_r && x < s->w; x += 2) {
            int gx = x - s->dx;

            if (gx < 0 || gx >= PGL_W)
                continue;
            n++;
            diff += vga[y * PGL_W + gx] != s->post[y * s->w + x];
        }
    return n > 0 && diff * 10 < n * 6;
}

/* ---- present ---------------------------------------------------------------------------------- */

/* PORT_GL_RECORD=FILE: every present's middle 32 rows of the picture (RGBA; with
   PORT_GL_RECORD_COLUMNS=1 the middle 32 columns, full height), after a header {time ns,
   scene serial, yaw, pitch, width, rows, the eye drawn: x, y, z (world units * 1000), yaw
   and pitch (2048ths * 1000), the game's time (us)}, for looking at motion frame by frame
   without screenshots stalling the main thread (tools/port_gl_motion.py) */
static void record(const pgl_scene *s, const pose *e, int w, int h)
{
    static FILE *f;
    static int tried, columns;
    static unsigned char *rows;
    long long head[12];
    int rw = w, rh = 32;

    if (!tried) {
        const char *path = SDL_getenv("PORT_GL_RECORD");

        tried = 1;
        if (path)
            f = fopen(path, "wb");
        columns = SDL_getenv("PORT_GL_RECORD_COLUMNS") != NULL;
    }
    if (f == NULL)
        return;
    if (columns) {
        rw = 32;
        rh = h;
    }
    rows = realloc(rows, (size_t)rw * rh * 4);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, out_fbo);
    if (columns)
        glReadPixels(w / 2 - 16, 0, 32, h, GL_RGBA, GL_UNSIGNED_BYTE, rows);
    else
        glReadPixels(0, h * 6 / 10, w, 32, GL_RGBA, GL_UNSIGNED_BYTE, rows);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    head[0] = (long long)SDL_GetTicksNS();
    head[1] = s ? (long long)s->serial : 0;
    head[2] = s ? s->cam_yaw : 0;
    head[3] = s ? s->cam_pitch : 0;
    head[4] = rw;
    head[5] = rh;
    head[6] = (long long)(e->x * 1000);
    head[7] = (long long)(e->y * 1000);
    head[8] = (long long)(e->z * 1000);
    head[9] = (long long)(e->yaw * 1000);
    head[10] = (long long)(e->pitch * 1000);
    head[11] = (long long)(e->t * 1000);
    fwrite(head, sizeof head, 1, f);
    fwrite(rows, (size_t)rw * rh * 4, 1, f);
    fflush(f);
}

static int debug = -1, presents, st_shown, st_stale, st_invalid, st_none;
static Uint64 last_report, last_swap;
static double st_gap_min = 1e9, st_gap_max, st_gap_sum, st_gap_sq;
static int st_gaps;
static const pgl_scene *last_scene;     /* (the screenshots of the software frame) */

/* where the eye is drawn from at this present, and the move to it from the scene's frame */
static void eye_now(const pgl_scene *s, const int *look, pose *e, int *moved)
{
    double now = (double)SDL_GetTicksNS() / 1e6;

    *moved = 0;
    e->x = s->cam_x;
    e->y = s->cam_y;
    e->z = s->cam_z;
    e->pitch = s->cam_pitch;
    e->yaw = s->cam_yaw;
    e->roll = s->cam_roll;
    e->t = s->game_ms;
    if (interp && nhist > 0 && hist[nhist - 1].serial == s->serial) {
        /* a frame and a half behind the game's clock now: between the frames around it */
        double delay = frame_dt * 1.5, t;

        if (delay < 8)
            delay = 8;
        if (delay > 60)
            delay = 60;
        t = now + clock_off - delay;
        if (t < last_render_t && last_render_t - t < 100)
            t = last_render_t;              /* never backwards */
        last_render_t = t;
        pose_at(t, e);
        *moved = 1;
    }
    if (look) {
        /* the mouse turns the view: its angles are the mouse's now. Smooth motion adds every
           mickey the game has not read into the frame's angles yet; otherwise the fraction of
           a step its last read left (the frame drawn where the mouse was when it read it) */
        double dy, dp, most = interp ? 64.0 : 1.0, p0;

        if (interp) {
            double tx, ty;

            pgl_mouse_total(&tx, &ty);
            dy = tx - s->mouse_used_x;
            dp = (ty - s->mouse_used_y) * look[0];
        } else {
            dy = s->mouse_rem_x;
            dp = s->mouse_rem_y * look[0];
        }
        dy = dy < -most ? -most : dy > most ? most : dy;
        dp = dp < -most ? -most : dp > most ? most : dp;
        p0 = wrap(s->cam_pitch);
        if (p0 + dp > 256)                      /* the engine stops the pitch at +-256 */
            dp = 256 - p0;
        if (p0 + dp < -256)
            dp = -256 - p0;
        e->yaw = s->cam_yaw + dy;
        e->pitch = s->cam_pitch + dp;
        *moved = 1;
    }
}

void vpc_gl_present(const unsigned char *vga_live, const unsigned char *rgb, const int *cross,
                    const int *look)
{
    unsigned char pal[256 * 4];
    const unsigned char *vga = vga_live;
    const pgl_scene *s;
    int i, rx, ry, rw, rh, cols, valid = 0, current = 0, want;
    view v;
    pose e;
    float c0, shown, dx, fw;

    if (!ready)
        return;
    SDL_GL_MakeCurrent(window, context);
    upload_textures();
    view_rect(&rx, &ry, &rw, &rh, &cols);
    if (rw < 1 || rh < 1)
        return;
    make_fbos(rw, rh);

    /* the frame the game is asked for: the columns shown, a margin each side for smooth
       motion; none past 320 is the original pass */
    want = cols + (interp ? 2 * overscan : 0);
    if (want > PGL_MAX_W)
        want = PGL_MAX_W;
    pgl_wide_request = want > PGL_W ? want : 0;

    for (i = 0; i < 256; i++) {
        pal[i * 4] = rgb[i * 3];
        pal[i * 4 + 1] = rgb[i * 3 + 1];
        pal[i * 4 + 2] = rgb[i * 3 + 2];
        pal[i * 4 + 3] = 255;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_pal);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 1, GL_RGBA, GL_UNSIGNED_BYTE, pal);

    s = pgl_latest();
    if (s && s->valid && (nhist == 0 || hist[nhist - 1].serial != s->serial))
        push_pose(s);
    /* the picture shown with the scene: the scene's own copy while no present has come since
       (the HUD drawn over that frame, never half of the next) */
    if (s && s->valid && vga_live != NULL && s->present_serial == pgl_present_serial)
        vga = s->vga;
    glBindTexture(GL_TEXTURE_2D, tex_vga);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PGL_W, PGL_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE, vga);

    if (s && s->valid && vga_live != NULL && scene_current(s, vga))
        current = 1;
    /* the columns shown: the frame's middle `cols` of its w, or a narrower frame in the
       middle of them */
    fw = s && current ? (float)s->w : (float)cols;
    shown = (float)cols;
    c0 = (fw - shown) / 2.0f;
    dx = (fw - PGL_W) / 2.0f;
    if (s && current)
        dx = (float)s->dx;

    if (debug < 0)
        debug = SDL_getenv("PORT_GL_DEBUG") != NULL;
    e.x = e.y = e.z = e.pitch = e.yaw = e.roll = e.t = 0;
    if (s && current) {
        int moved;

        eye_now(s, look, &e, &moved);
        v.c0 = c0;
        v.cols = shown;
        if (moved)
            move_matrix(s, &e, v.move, v.shift);
        else {
            memset(v.move, 0, sizeof v.move);
            v.move[0] = v.move[4] = v.move[8] = 1.0f;
            v.shift[0] = v.shift[1] = v.shift[2] = 0.0f;
        }
        v.zoom = 1.0f;
        /* the sky drawn for the frame (sky.c: panorama column = yaw + screen column; the
           horizon at -focal_y tan(pitch)) moved as the scene was */
        v.presh[0] = (float)wrap(e.yaw - s->cam_yaw);
        v.presh[1] = (float)(s->focal_y * (tan(wrap(e.pitch) * M_PI / 1024.0) -
                                           tan(wrap(s->cam_pitch) * M_PI / 1024.0)));
        if (cross && s->focal_x > 0 && s->focal_y > 0) {
            /* the field of view's half height, widened by the most a step of pitch can bring
               in (the sides have the overscan's columns) */
            const float step = 6.2831853f / 2048.0f;
            float hy = atanf(s->hh / s->focal_y);

            v.zoom = tanf(hy + step) / tanf(hy);
            if (!(interp && overscan > 0)) {
                float hx = atanf(s->hw / s->focal_x), zx = tanf(hx + step) / tanf(hx);

                if (zx > v.zoom)
                    v.zoom = zx;
            }
        }
        if (gl_on) {
            valid = 1;
            draw_scene(s, &v);
        }
        glBindTexture(GL_TEXTURE_2D, tex_pre);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->w, PGL_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE,
                        s->pre);
        glBindTexture(GL_TEXTURE_2D, tex_post);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->w, PGL_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE,
                        s->post);
        if (s->w > PGL_W) {
            glBindTexture(GL_TEXTURE_2D, tex_side);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->w, PGL_H, GL_RED_INTEGER,
                            GL_UNSIGNED_BYTE, s->side);
        }
        last_scene = s;
    } else {
        last_scene = NULL;
    }
    if (debug) {
        Uint64 now = SDL_GetTicksNS();

        presents++;
        if (s == NULL)
            st_none++;
        else if (!s->valid)
            st_invalid++;
        else if (!current)
            st_stale++;
        else
            st_shown++;
        if (now - last_report > 2000000000ull) {
            double mean = st_gaps ? st_gap_sum / st_gaps : 0;

            fprintf(stderr, "port: GL %d presents: %d shown, %d stale, %d no 3D, %d none; scene %lu: "
                    "%d vertices, %d draws; frame %d (%d shown), pools %d polygons %d spans\n",
                    presents, st_shown, st_stale, st_invalid, st_none, s ? s->serial : 0,
                    s ? s->nverts : 0, s ? s->ndraws : 0, s ? s->w : 0, cols,
                    s ? s->pool_polys : 0, s ? s->pool_spans : 0);
            fprintf(stderr, "port: GL presents every %.2f ms (sd %.2f, %.2f..%.2f); %d frames, "
                    "a frame every %.1f ms, %d snaps, %d held, between frames %.2f..%.2f\n",
                    mean, st_gaps ? sqrt(st_gap_sq / st_gaps - mean * mean) : 0,
                    st_gaps ? st_gap_min : 0, st_gap_max, st_scenes, frame_dt, st_snaps, st_held,
                    st_alpha_min < 1e8 ? st_alpha_min : 0, st_alpha_max > -1e8 ? st_alpha_max : 0);
            presents = st_shown = st_stale = st_invalid = st_none = 0;
            st_gaps = 0;
            st_gap_sum = st_gap_sq = st_gap_max = 0;
            st_gap_min = 1e9;
            st_snaps = st_held = st_scenes = 0;
            st_alpha_min = 1e9;
            st_alpha_max = -1e9;
            last_report = now;
        }
    }

    /* the composite, into the output framebuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, out_fbo);
    glViewport(0, 0, rw, rh);
    glUseProgram(comp_prog);
    glBindVertexArray(empty_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_vga);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_pre);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_post);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, scene_colour);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, tex_pal);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tex_side);
    glUniform1i(c_vga, 0);
    glUniform1i(c_pre, 1);
    glUniform1i(c_post, 2);
    glUniform1i(c_scene, 3);
    glUniform1i(c_pal, 4);
    glUniform1i(c_side, 5);
    glUniform1i(c_valid, current);
    glUniform1i(c_gl, valid);
    glUniform1i(c_sides, current && s->w > PGL_W);
    glUniform1i(c_smooth, smooth);
    glUniform4f(c_map, c0, shown, dx, fw);
    glUniform1i(c_sharp, hud_sharp);
    glUniform2f(c_scale, rw / shown, rh / (float)PGL_H);
    glUniform2f(c_presh, s && current ? v.presh[0] : 0.0f, s && current ? v.presh[1] : 0.0f);
    if (cross)
        glUniform3f(c_cross, cross[0] + 0.5f, cross[1] + 0.5f, 1.0f);
    else
        glUniform3f(c_cross, 0.0f, 0.0f, 0.0f);
    if (s && current) {
        glUniform4i(c_clip, s->clip_l, s->clip_t, s->clip_r, s->clip_b);
        glUniform2i(c_bg, s->bg_fill, s->bg_colour);
    } else {
        glUniform4i(c_clip, 0, 0, 0, 0);
        glUniform2i(c_bg, 0, 0);
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* to the window */
    glBindFramebuffer(GL_READ_FRAMEBUFFER, out_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    {
        int ww, wh;

        SDL_GetWindowSizeInPixels(window, &ww, &wh);
        glViewport(0, 0, ww, wh);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glBlitFramebuffer(0, 0, rw, rh, rx, ry, rx + rw, ry + rh, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    record(s, &e, rw, rh);
    SDL_GL_SwapWindow(window);
    {
        /* a swap the display did not wait for (a hidden window): no faster than 250 a second */
        Uint64 now = SDL_GetTicksNS();

        if (vsync && last_swap && now - last_swap < 4000000ull) {
            SDL_DelayNS(4000000ull - (now - last_swap));
            now = SDL_GetTicksNS();
        }
        if (debug && last_swap) {
            double gap = (double)(now - last_swap) / 1e6;

            st_gaps++;
            st_gap_sum += gap;
            st_gap_sq += gap * gap;
            if (gap < st_gap_min)
                st_gap_min = gap;
            if (gap > st_gap_max)
                st_gap_max = gap;
        }
        last_swap = now;
    }
}

static int save_rgba(const char *path, const unsigned char *px, int w, int h, int flip)
{
    SDL_Surface *surf = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
    int y, ok;

    if (surf == NULL)
        return -1;
    for (y = 0; y < h; y++)
        memcpy((unsigned char *)surf->pixels + (size_t)y * surf->pitch,
               px + (size_t)(flip ? h - 1 - y : y) * w * 4, (size_t)w * 4);
    ok = SDL_SaveBMP(surf, path);
    SDL_DestroySurface(surf);
    return ok ? 0 : -1;
}

int vpc_gl_screenshot(const char *path)
{
    unsigned char *px;
    int ok;

    if (!ready || fbo_w == 0)
        return -1;
    SDL_GL_MakeCurrent(window, context);
    px = malloc((size_t)fbo_w * fbo_h * 4);
    if (px == NULL)
        return -1;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, out_fbo);
    glReadPixels(0, 0, fbo_w, fbo_h, GL_RGBA, GL_UNSIGNED_BYTE, px);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    ok = save_rgba(path, px, fbo_w, fbo_h, 1);
    free(px);
    return ok;
}

/* the last scene's wide frame as the software renderer drew it, the game's screen in its
   middle (rgb: the palette), w x 200; -1 when the last present showed no wide frame */
int vpc_gl_screenshot_wide_sw(const char *path, const unsigned char *rgb)
{
    const pgl_scene *s = last_scene;
    unsigned char *px;
    int x, y, ok;

    if (s == NULL || s->w <= PGL_W)
        return -1;
    px = malloc((size_t)s->w * PGL_H * 4);
    if (px == NULL)
        return -1;
    for (y = 0; y < PGL_H; y++)
        for (x = 0; x < s->w; x++) {
            int gx = x - s->dx;
            unsigned c = gx >= 0 && gx < PGL_W ? s->vga[y * PGL_W + gx] : s->side[y * s->w + x];
            unsigned char *p = px + ((size_t)y * s->w + x) * 4;

            p[0] = rgb[c * 3];
            p[1] = rgb[c * 3 + 1];
            p[2] = rgb[c * 3 + 2];
            p[3] = 255;
        }
    ok = save_rgba(path, px, s->w, PGL_H, 0);
    free(px);
    return ok;
}
