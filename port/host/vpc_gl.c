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
      scene's alpha is its coverage (fractional on the edges once resolved);
   3. the composite: per pixel of the window, the software screen's pixel (VGA memory) where
      the game drew after the 3D pass (the screen differs from the copy taken after it) or
      outside the 3D window; else the scene over what was there before the 3D pass (the sky)
      or the background colour indoors.
   The picture is 320x200 at 4:3, letterboxed in the window, as the software path shows it. */
#define GL_SILENCE_DEPRECATION
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <OpenGL/gl3.h>

#include "port_gl.h"
#include "port_vpc.h"

static SDL_Window *window;
static SDL_GLContext context;
static int ready, gl_on = 1, smooth, msaa = 4;

static GLuint scene_prog, comp_prog;
static GLuint vao, vbo, empty_vao;
static GLuint tex_vga, tex_pre, tex_post, tex_tables, tex_pal, tex_falloff;
static GLuint scene_fbo, scene_colour, scene_depth;     /* (resolved) colour, and its FBO */
static GLuint ms_fbo, ms_colour, ms_depth;              /* the multisampled target */
static GLuint out_fbo, out_colour;
static int fbo_w, fbo_h;
static GLuint *textures;
static int ntextures;
static int *tsize;                      /* w, h per texture id */

/* scene uniforms */
static GLint u_view, u_mode, u_colour, u_row2, u_fog, u_tsize, u_fogp, u_depth, u_tex,
    u_tables, u_pal, u_smooth, u_focal, u_nl, u_lbase, u_lights, u_falloff, u_turn;
/* composite uniforms */
static GLint c_vga, c_pre, c_post, c_scene, c_pal, c_valid, c_clip, c_bg, c_smooth, c_cross;

static const char *scene_vs =
    "#version 410 core\n"
    "layout(location = 0) in vec3 a_cam;\n"
    "layout(location = 1) in vec2 a_uv;\n"
    "layout(location = 2) in float a_shade;\n"
    "uniform vec4 u_view;\n"                /* centre x, y; half width, height (pixels) */
    "uniform vec2 u_focal;\n"
    "uniform vec4 u_turn;\n"                /* the turn not yet in the scene: yaw, pitch, the
                                               scene's pitch (radians), the overscan */
    "out vec2 v_uv;\n"
    "out float v_shade;\n"
    "out float v_z;\n"
    "out vec3 v_cam;\n"
    /* a rotation about x (pitch) and y (yaw), as the view's: y down, z ahead */
    "vec3 pitch_by(vec3 v, float a) {\n"
    "    float c = cos(a), s = sin(a);\n"
    "    return vec3(v.x, v.y * c - v.z * s, v.y * s + v.z * c);\n"
    "}\n"
    "vec3 yaw_by(vec3 v, float a) {\n"
    "    float c = cos(a), s = sin(a);\n"
    "    return vec3(v.x * c - v.z * s, v.y, v.x * s + v.z * c);\n"
    "}\n"
    "void main() {\n"
    "    vec3 c = a_cam;\n"
    "    if (u_turn.x != 0.0 || u_turn.y != 0.0) {\n"
    /* the view space unscaled (camera space has x * focal / half width), the yaw about the
       world's vertical (under the scene's pitch), then the pitch, and scaled back */
    "        vec3 v = vec3(c.x * u_view.z / u_focal.x, c.y * u_view.w / u_focal.y, c.z);\n"
    "        v = pitch_by(yaw_by(pitch_by(v, -u_turn.z), u_turn.x), u_turn.z + u_turn.y);\n"
    "        c = vec3(v.x * u_focal.x / u_view.z, v.y * u_focal.y / u_view.w, v.z);\n"
    "    }\n"
    "    c.xy *= u_turn.w;\n"
    "    float z = c.z;\n"
    /* screen x = cx + x * hw / z on 320 columns, y = cy + y * hh / z on 200 rows, as clip
       coordinates with w = z (GL clips what is behind the eye) */
    "    gl_Position = vec4((u_view.x / 160.0 - 1.0) * z + c.x * u_view.z / 160.0,\n"
    "                       (1.0 - u_view.y / 100.0) * z - c.y * u_view.w / 100.0,\n"
    "                       0.0, z);\n"
    "    v_uv = a_uv;\n"
    "    v_shade = a_shade;\n"
    "    v_z = a_cam.z;\n"
    "    v_cam = a_cam;\n"
    "}\n";

static const char *scene_fs =
    "#version 410 core\n"
    "in vec2 v_uv;\n"
    "in float v_shade;\n"
    "in float v_z;\n"
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
    "    gl_FragDepth = clamp(v_z * u_depth, 0.0, 1.0);\n"
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

static const char *comp_fs =
    "#version 410 core\n"
    "in vec2 v_pos;\n"
    "uniform usampler2D u_vga, u_pre, u_post;\n"
    "uniform sampler2D u_scene, u_pal;\n"
    "uniform int u_valid;\n"
    "uniform ivec4 u_clip;\n"               /* left, top, right, bottom */
    "uniform ivec2 u_bg;\n"                 /* fill?, colour */
    "uniform int u_smooth;\n"
    "uniform vec3 u_cross;\n"              /* the crosshair: x, y (320x200 pixels), on */
    "layout(location = 0) out vec4 o;\n"
    "vec3 pal(uint c) { return texelFetch(u_pal, ivec2(int(c), 0), 0).rgb; }\n"
    /* the picture before the 3D pass at q (320x200 pixels), filtered through the palette */
    "vec3 pre_smooth(vec2 q) {\n"
    "    vec2 p = q - 0.5;\n"
    "    vec2 f = fract(p);\n"
    "    ivec2 b = ivec2(floor(p));\n"
    "    vec3 s = vec3(0.0);\n"
    "    for (int k = 0; k < 4; k++) {\n"
    "        ivec2 d = ivec2(k & 1, k >> 1);\n"
    "        float w = (d.x == 1 ? f.x : 1.0 - f.x) * (d.y == 1 ? f.y : 1.0 - f.y);\n"
    "        s += pal(texelFetch(u_pre, clamp(b + d, ivec2(0), ivec2(319, 199)), 0).r) * w;\n"
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
    "void main() {\n"
    "    if (u_cross.z > 0.5) {\n"
    "        vec2 q = v_pos * vec2(320.0, 200.0);\n"
    "        if (cross_at(q, 0.0) > 0.5) { o = vec4(1.0); return; }\n"
    "        if (cross_at(q, 0.25) > 0.5) { o = vec4(0.0, 0.0, 0.0, 1.0); return; }\n"
    "    }\n"
    "    ivec2 p = clamp(ivec2(floor(v_pos * vec2(320.0, 200.0))), ivec2(0), ivec2(319, 199));\n"
    "    uint c = texelFetch(u_vga, p, 0).r;\n"
    "    if (u_valid != 0 && p.x >= u_clip.x && p.x < u_clip.z && p.y >= u_clip.y &&\n"
    "        p.y < u_clip.w && c == texelFetch(u_post, p, 0).r) {\n"
    /* the scene (its colour premultiplied by its coverage) over what is behind it */
    "        vec4 s = texelFetch(u_scene, ivec2(gl_FragCoord.xy), 0);\n"
    "        vec3 u = u_bg.x != 0 ? pal(uint(u_bg.y)) : u_smooth != 0 ?\n"
    "                 pre_smooth(v_pos * vec2(320.0, 200.0)) : pal(texelFetch(u_pre, p, 0).r);\n"
    "        o = vec4(s.rgb + u * (1.0 - s.a), 1.0);\n"
    "        return;\n"
    "    }\n"
    "    o = texelFetch(u_pal, ivec2(int(c), 0), 0);\n"
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
    SDL_GL_SetSwapInterval(0);
    fprintf(stderr, "port: OpenGL %s, %s\n", (const char *)glGetString(GL_VERSION),
            (const char *)glGetString(GL_RENDERER));

    scene_prog = program(scene_vs, scene_fs);
    comp_prog = program(comp_vs, comp_fs);
    if (!scene_prog || !comp_prog)
        return 0;
    u_view = glGetUniformLocation(scene_prog, "u_view");
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
    u_turn = glGetUniformLocation(scene_prog, "u_turn");
    u_nl = glGetUniformLocation(scene_prog, "u_nl");
    u_lbase = glGetUniformLocation(scene_prog, "u_lbase");
    u_lights = glGetUniformLocation(scene_prog, "u_lights");
    u_falloff = glGetUniformLocation(scene_prog, "u_falloff");
    c_vga = glGetUniformLocation(comp_prog, "u_vga");
    c_pre = glGetUniformLocation(comp_prog, "u_pre");
    c_post = glGetUniformLocation(comp_prog, "u_post");
    c_scene = glGetUniformLocation(comp_prog, "u_scene");
    c_pal = glGetUniformLocation(comp_prog, "u_pal");
    c_valid = glGetUniformLocation(comp_prog, "u_valid");
    c_clip = glGetUniformLocation(comp_prog, "u_clip");
    c_bg = glGetUniformLocation(comp_prog, "u_bg");
    c_smooth = glGetUniformLocation(comp_prog, "u_smooth");
    c_cross = glGetUniformLocation(comp_prog, "u_cross");

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
    tex_pre = index_texture(PGL_W, PGL_H);
    tex_post = index_texture(PGL_W, PGL_H);
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

/* the turn not yet in the scene ({yaw, pitch} in 2048ths of a turn, or NULL) and whether
   the view is the mouse's (the overscan, then, so a turn's new edge is never empty) */
#define LOOK_AHEAD 1.0                  /* the most of a turn added, in steps */
static float turn_yaw, turn_pitch;
static int turn_overscan;

static void draw_scene(const pgl_scene *s)
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

    /* the 3D window */
    x0 = s->clip_l * fbo_w / PGL_W;
    x1 = (s->clip_r * fbo_w + PGL_W - 1) / PGL_W;
    y0 = fbo_h - (s->clip_b * fbo_h + PGL_H - 1) / PGL_H;
    y1 = fbo_h - s->clip_t * fbo_h / PGL_H;
    glEnable(GL_SCISSOR_TEST);
    glScissor(x0, y0 < 0 ? 0 : y0, x1 - x0, y1 - y0);

    glUseProgram(scene_prog);
    glUniform4f(u_view, s->cx, s->cy, s->hw, s->hh);
    {
        const float step = 6.2831853f / 2048.0f;
        float zoom = 1.0f;
        float pitch = (float)(s->cam_pitch > 1023 ? s->cam_pitch - 2048 : s->cam_pitch);

        if (turn_overscan && s->focal_x > 0 && s->focal_y > 0) {
            /* the field of view's half angles, widened by the most the turn can bring in */
            float hx = atanf(s->hw / s->focal_x), hy = atanf(s->hh / s->focal_y);
            float a = (float)LOOK_AHEAD * step, zx = tanf(hx + a) / tanf(hx), zy = tanf(hy + a) / tanf(hy);

            zoom = zx > zy ? zx : zy;
        }
        glUniform4f(u_turn, turn_yaw * step, turn_pitch * step, pitch * step, zoom);
    }
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
   has drawn over (a menu) is not shown */
static int scene_current(const pgl_scene *s, const unsigned char *vga)
{
    int x, y, n = 0, diff = 0;

    for (y = s->clip_t; y < s->clip_b && y < PGL_H; y += 2)
        for (x = s->clip_l; x < s->clip_r && x < PGL_W; x += 2) {
            n++;
            diff += vga[y * PGL_W + x] != s->post[y * PGL_W + x];
        }
    return n > 0 && diff * 10 < n * 6;
}

/* ---- present ---------------------------------------------------------------------------------- */

static void letterbox(int *x, int *y, int *w, int *h)
{
    int ww, wh;

    SDL_GetWindowSizeInPixels(window, &ww, &wh);
    if (ww * 3 > wh * 4) {
        *h = wh;
        *w = wh * 4 / 3;
    } else {
        *w = ww;
        *h = ww * 3 / 4;
    }
    *x = (ww - *w) / 2;
    *y = (wh - *h) / 2;
}

/* PORT_GL_RECORD=FILE: every present's middle 32 rows of the picture (RGBA; with
   PORT_GL_RECORD_COLUMNS=1 the middle 32 columns, full height), after a header {time ns,
   scene serial, yaw, pitch, width, rows}, for looking at motion frame by frame without
   screenshots stalling the main thread */
static void record(const pgl_scene *s, int w, int h)
{
    static FILE *f;
    static int tried, columns;
    static unsigned char *rows;
    long long head[6];
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
    fwrite(head, sizeof head, 1, f);
    fwrite(rows, (size_t)rw * rh * 4, 1, f);
    fflush(f);
}

static int debug = -1, presents, st_shown, st_stale, st_invalid, st_none;
static Uint64 last_report;

void vpc_gl_present(const unsigned char *vga, const unsigned char *rgb, const int *cross,
                    const int *look)
{
    unsigned char pal[256 * 4];
    const pgl_scene *s;
    int i, rx, ry, rw, rh, valid = 0;

    if (!ready)
        return;
    SDL_GL_MakeCurrent(window, context);
    upload_textures();
    letterbox(&rx, &ry, &rw, &rh);
    if (rw < 1 || rh < 1)
        return;
    make_fbos(rw, rh);

    for (i = 0; i < 256; i++) {
        pal[i * 4] = rgb[i * 3];
        pal[i * 4 + 1] = rgb[i * 3 + 1];
        pal[i * 4 + 2] = rgb[i * 3 + 2];
        pal[i * 4 + 3] = 255;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_pal);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 1, GL_RGBA, GL_UNSIGNED_BYTE, pal);
    glBindTexture(GL_TEXTURE_2D, tex_vga);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PGL_W, PGL_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE, vga);

    turn_overscan = cross != NULL;
    s = pgl_latest();
    turn_yaw = turn_pitch = 0.0f;
    if (look && s && s->valid) {
        /* the fraction of a step past the scene's angles: the view at the mouse's exact
           place when the frame read it */
        double dy = s->mouse_rem_x, dp = s->mouse_rem_y * look[0];

        dy = dy < -LOOK_AHEAD ? -LOOK_AHEAD : dy > LOOK_AHEAD ? LOOK_AHEAD : dy;
        dp = dp < -LOOK_AHEAD ? -LOOK_AHEAD : dp > LOOK_AHEAD ? LOOK_AHEAD : dp;
        if (look[1] + dp > 256)                 /* the engine stops the pitch at +-256 */
            dp = 256 - look[1];
        if (look[1] + dp < -256)
            dp = -256 - look[1];
        turn_yaw = (float)dy;
        turn_pitch = (float)dp;
    }
    if (debug < 0)
        debug = SDL_getenv("PORT_GL_DEBUG") != NULL;
    if (debug) {
        Uint64 now = SDL_GetTicksNS();

        presents++;
        if (s == NULL)
            st_none++;
        else if (!s->valid)
            st_invalid++;
        else if (!scene_current(s, vga))
            st_stale++;
        else
            st_shown++;
        if (now - last_report > 2000000000ull) {
            fprintf(stderr, "port: GL %d presents: %d shown, %d stale, %d no 3D, %d none; scene %lu: "
                    "%d vertices, %d draws\n", presents, st_shown, st_stale, st_invalid, st_none,
                    s ? s->serial : 0, s ? s->nverts : 0, s ? s->ndraws : 0);
            presents = st_shown = st_stale = st_invalid = st_none = 0;
            last_report = now;
        }
    }
    if (gl_on && s && s->valid && scene_current(s, vga)) {
        valid = 1;
        draw_scene(s);
        glBindTexture(GL_TEXTURE_2D, tex_pre);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PGL_W, PGL_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE,
                        s->pre);
        glBindTexture(GL_TEXTURE_2D, tex_post);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PGL_W, PGL_H, GL_RED_INTEGER, GL_UNSIGNED_BYTE,
                        s->post);
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
    glUniform1i(c_vga, 0);
    glUniform1i(c_pre, 1);
    glUniform1i(c_post, 2);
    glUniform1i(c_scene, 3);
    glUniform1i(c_pal, 4);
    glUniform1i(c_valid, valid);
    glUniform1i(c_smooth, smooth);
    if (cross)
        glUniform3f(c_cross, cross[0] + 0.5f, cross[1] + 0.5f, 1.0f);
    else
        glUniform3f(c_cross, 0.0f, 0.0f, 0.0f);
    if (s) {
        glUniform4i(c_clip, s->clip_l, s->clip_t, s->clip_r, s->clip_b);
        glUniform2i(c_bg, s->bg_fill, s->bg_colour);
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
    record(s, rw, rh);
    SDL_GL_SwapWindow(window);
}

int vpc_gl_screenshot(const char *path)
{
    SDL_Surface *surf;
    unsigned char *px;
    int y, ok;

    if (!ready || fbo_w == 0)
        return -1;
    SDL_GL_MakeCurrent(window, context);
    px = malloc((size_t)fbo_w * fbo_h * 4);
    surf = SDL_CreateSurface(fbo_w, fbo_h, SDL_PIXELFORMAT_RGBA32);
    if (px == NULL || surf == NULL) {
        free(px);
        return -1;
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, out_fbo);
    glReadPixels(0, 0, fbo_w, fbo_h, GL_RGBA, GL_UNSIGNED_BYTE, px);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    for (y = 0; y < fbo_h; y++)
        memcpy((unsigned char *)surf->pixels + (size_t)y * surf->pitch,
               px + (size_t)(fbo_h - 1 - y) * fbo_w * 4, (size_t)fbo_w * 4);
    free(px);
    ok = SDL_SaveBMP(surf, path);
    SDL_DestroySurface(surf);
    return ok ? 0 : -1;
}
