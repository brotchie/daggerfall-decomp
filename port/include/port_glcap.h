/* port_glcap.h: the OpenGL renderer's capture hooks (port/engine_gl/glcap.c,
   port/include/port_gl.h), as XnGine calls them in the native build. xngine.h includes this
   under DAGGER_PORT; elsewhere XN_GLCAP(...) is nothing. */
#ifndef PORT_GLCAP_H
#define PORT_GLCAP_H

struct xn_poly_vertex;
struct xn_model_face;
struct xn_tex_image;
struct xn_flat;

extern volatile int pgl_enabled;
/* the 3D frames drawn (rframe.c), whatever renders them: the world is on the screen while it
   moves (port/host/main.c's crosshair) */
extern volatile unsigned long port_render_frames;

/* the polygon pool starts again (xn_render_begin_frame) */
void xn_glcap_begin_frame(void);
/* the camera-space vertices of the pool record being built: the clipper's (poly.c), a face's
   points through xn_vert_cam (poly.c), a ground cell's corners (terrain.c) */
void xn_glcap_poly(const void *record, const struct xn_poly_vertex *v, int n);
void xn_glcap_face(const void *record, const struct xn_model_face *face);
void xn_glcap_terrain(const void *record, unsigned int v, const unsigned int *corner, int n);
/* a textured face's image, on its first span (poly.c) */
void xn_glcap_tex(const void *record, const struct xn_tex_image *image);
/* a flat's quad (the four corners, camera space) and image (flat.c) */
void xn_glcap_flat(const struct xn_flat *flat, const struct xn_tex_image *image,
                   const struct xn_poly_vertex *quad);
/* the 3D pass begins and ends (rframe.c) */
void xn_glcap_pre(void);
void xn_glcap_end_frame(void);
/* the back buffer is shown (gfx.c) */
void xn_glcap_present(void);

#define XN_GLCAP(call) do { if (pgl_enabled) call; } while (0)

/* The wide 3D pass (port/engine_gl/wide.c): the frame's width while it runs (else 0) and the
   game's column 0 in it; the last frame's, whose S-buffer the pick reads */
extern int port_wide_cols, port_wide_dx, port_wide_last_cols, port_wide_last_dx;
/* the pick at a point of the game's screen (xn_render_pick): 1 if it goes to the last wide
   frame, with *x made its column (then port_wide_pick_end) */
int port_wide_pick_begin(int *x);
void port_wide_pick_end(void);
/* the light shaders' bytes (Q-LIGHT-07) past big_buffer's end go to a sink: a wide frame has
   more lit polygons than it holds */
#define PORT_BIG_BUFFER_SIZE 102400

#endif
