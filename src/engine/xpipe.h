/* xpipe.h: what the 3D pipeline's readable C shares (poly, render, flat, model, cam, tex):
   the view globals, a few one-instruction helpers that are not (yet) in xngine.h, and the
   struct tags of xnstruct.h. Candidates for xngine.h are marked "promote". */
#ifndef XPIPE_H
#define XPIPE_H

#include "xngine.h"
#include "xnstruct.h"

/* ---- helpers (promote) ------------------------------------------------------------------- */

/* (hi:lo) / d, unsigned (div ecx): a divide error when the quotient does not fit (the
   engine's handler then makes it 0) */
u32 xn_udiv64(u32 hi, u32 lo, u32 d);
#pragma aux xn_udiv64 = "div ecx" parm [edx] [eax] [ecx] value [eax] modify [edx];

/* (hi:lo) / d, signed (idiv ecx) */
s32 xn_sdiv64(s32 hi, u32 lo, s32 d);
#pragma aux xn_sdiv64 = "idiv ecx" parm [edx] [eax] [ecx] value [eax] modify [edx];

/* (hi:lo) % d, unsigned (div ecx; the remainder) */
u32 xn_umod64(u32 hi, u32 lo, u32 d);
#pragma aux xn_umod64 = "div ecx" parm [edx] [eax] [ecx] value [edx] modify [eax];

/* n dwords of v at dst (rep stosd) */
void xn_fill32(void *dst, u32 v, u32 n);
#pragma aux xn_fill32 = "rep stosd" parm [edi] [eax] [ecx] modify [edi ecx];

/* n bytes of v at dst (rep stosb) */
void xn_fill8(void *dst, u32 v, u32 n);
#pragma aux xn_fill8 = "rep stosb" parm [edi] [eax] [ecx] modify [edi ecx];

/* bits 16..47 of a * b (imul; shrd 16) */
s32 xn_fixmul16(s32 a, s32 b);
#pragma aux xn_fixmul16 = "imul edx" "shrd eax, edx, 16" parm [eax] [edx] value [eax] \
    modify [edx];

/* the low dword of a * b, as the asm's imul leaves it in EAX (for a register a function
   leaves behind) */
#define XN_LOW32(a, b) ((s32)((u32)(a) * (u32)(b)))

/* The flags an asm `add` leaves (ZF, SF, OF) for a + b: for glue whose callers branch on them */
u32 xn_add_flags(s32 a, s32 b);

/* A routine called through a pointer with registers (span routines, setups, the span hook):
   called from C with xn_asmcall */
typedef void (*xn_routine)(void);

/* Runs an asm routine that takes no register arguments on a register file of its own (zeros
   in, its results dropped): its registers, even EBP, cannot disturb the C */
void xn_call_asm(xn_routine fn);

/* A light setup (xn_light_setup_poly or _terrain: asm, rasteriser group) on the polygon, with
   a span's registers (r: ECX in): returns its lighting kind (0, 4 or 8, a byte offset) and
   leaves its EDX in r->edx, as the asm setups pass it on to the span routine */
s32 xn_call_light_setup(xn_routine fn, struct xn_poly *poly, xn_regs *r);

/* The setups' `jmp ecx`: the span routine fn runs on the span the registers r describe
   (eax = the polygon, ebx = x - centre x, ebp = the count, esi = the node, edi = the
   destination - 1), with ECX = fn; its registers and flags come back in r */
void xn_run_span(xn_routine fn, xn_regs *r);

/* The BIOS tick count (0040:006C) */
#define XN_BIOS_TICKS (*(u32 *)0x46C)

/* The entry `off` bytes into a table: the asm indexes its tables by byte offsets (0, 4, 8...) */
#define XN_AT(type, base, off) (*(type *)((u8 *)(base) + (off)))

/* A patch field the asm keeps its own state in: the store is kept for the records (index.md,
   rule KEEP) */
#define XN_KEEP(field, v) ((field) = (v))

/* ---- the view (struct xn_view at 0xCEA20, as separate globals) ---------------------------- */
extern s32 xn_cam_near_z, xn_cam_far_z;
extern s32 xn_cam_half_width, xn_cam_half_height;
extern s32 xn_cam_centre_x, xn_cam_centre_y;
extern s32 xn_cam_focal_x, xn_cam_inv_focal_x, xn_cam_focal_y, xn_cam_inv_focal_y;
extern s32 xn_cam_scale_x, xn_cam_inv_scale_x, xn_cam_scale_y, xn_cam_inv_scale_y;
extern s32 xn_cam_hfov_cos, xn_cam_hfov_sin, xn_cam_vfov_cos, xn_cam_vfov_sin;
extern s32 xn_cam_flat_scale_x, xn_cam_flat_scale_y;       /* unaligned (0xCEA99, 0xCEA9D) */
extern xn_mat3 xn_cam_rotation;         /* the eye's rotation, 2.28 */
extern xn_mat3 xn_cam_view_matrix;      /* the rotation with rows 0 and 1 scaled */
extern s32 xn_cam_x, xn_cam_y, xn_cam_z;
extern s32 xn_pick_view_x, xn_pick_view_y;
extern s32 pick_distance;


/* ---- the frame's pools and the S-buffer (xn_render_begin_frame resets them) ---------------- */
extern struct xn_poly xn_render_poly_pool[1000];
extern struct xn_poly *xn_render_poly_next;
extern struct xn_model_matrix_pool xn_render_matrix_pool;
extern struct xn_model_matrix_slot *xn_render_matrix_next;
extern struct xn_light_ref xn_render_light_list_pool[400];
extern struct xn_light_ref *xn_render_light_list_next;
extern struct xn_span xn_render_span_sentinel;
extern struct xn_span xn_render_span_rows[];       /* a head per screen row (only .next used) */
extern struct xn_span *xn_render_span_next;
extern s32 xn_render_poly_count;
extern s32 xn_render_mode;              /* 0 outline, 4 solid, 8 textured: a byte offset */
extern s32 xn_render_row_y;
extern s32 xn_render_frame_flags;       /* bit 1: no background fill (outdoors) */

/* ---- the screen (gfx group's globals) ------------------------------------------------------- */
extern s32 xn_gfx_width, xn_gfx_height;
extern s32 xn_gfx_clip_left, xn_gfx_clip_top, xn_gfx_clip_right, xn_gfx_clip_bottom;
extern s32 xn_gfx_row_offset[768];      /* y * width */
extern u8 *screen_buffer;
extern u8 *big_buffer;

#endif
