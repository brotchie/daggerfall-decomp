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

#endif
