/* model_t.c: test shims of src/engine/model.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call and back. The draw's
   helpers took the draw's state from operands of their code that xn_model_draw patched (smc
   MODEL-*): their shims read it there. The vertex-flag clear's asm entry is an unrolled body
   its caller stops with a planted `ret`: its shim counts the steps to it. */
#include "xmodel.h"

/* the asm draw's state, operands of its helpers' code */
extern struct xn_model_handle *xn_model_draw_handle;   /* 140497 */
extern s32 xn_model_eye_bf_x, xn_model_eye_bf_y, xn_model_eye_bf_z;   /* 14040B.. (faces) */
extern s32 xn_model_eye_x, xn_model_eye_y, xn_model_eye_z;            /* 14052D.. (vertices) */
extern s32 xn_model_depth_x, xn_model_depth_y, xn_model_depth_z;      /* 140446.. */
extern struct xn_model_matrix_slot *xn_model_matrix_slot;             /* 14053E */
extern u8 *xn_model_points_x;                                         /* 14051C */
extern void asm_xn_model_clear_vert_flags(void);       /* the unrolled body */

void xn_model_set_angles_r(xn_regs *r)
{
    xn_model_set_angles(r->eax, r->edx, r->ebx, (s16 *)r->ecx);
}

void xn_model_compose_angles_r(xn_regs *r)
{
    xn_model_compose_angles((s16 *)r->eax, r->edx, r->ebx, r->ecx);
}

void xn_model_set_angles_yaw_offset_r(xn_regs *r)
{
    xn_model_set_angles_yaw_offset((s16 *)r->eax, r->edx);
}

void xn_model_centroid_to_pick_r(xn_regs *r)
{
    xn_model_centroid_to_pick((const struct xn_model_handle *)r->eax);
}

void xn_model_calc_centroid_r(xn_regs *r)
{
    xn_model_calc_centroid((const struct xn_model *)r->eax);
}

void xn_model_push_player_from_pick_r(xn_regs *r)
{
    xn_model_push_player_from_pick((const struct xn_poly *)r->eax);
}

void xn_model_max_y_r(xn_regs *r)
{
    r->eax = xn_model_max_y((const struct xn_model *)r->eax);
}

void xn_model_xz_extent_r(xn_regs *r)
{
    xn_model_xz_extent((const struct xn_model *)r->eax, (u32 *)r->edx, (u32 *)r->ebx);
}

void xn_model_prepare_r(xn_regs *r)
{
    r->eax = (u32)xn_model_prepare((struct xn_model *)r->eax);
}

void xn_model_calc_uv_axes_r(xn_regs *r)
{
    xn_model_calc_uv_axes((struct xn_model *)r->eax);
}

/* the model in ESI, the face in EBX, its data in EDI */
void xn_model_calc_face_uv_axes_r(xn_regs *r)
{
    xn_model_calc_face_uv_axes((const struct xn_model *)r->esi,
                               (const struct xn_model_face *)r->ebx,
                               (struct xn_model_face_data *)r->edi);
}

void xn_model_calc_face_planes_r(xn_regs *r)
{
    xn_model_calc_face_planes((struct xn_model *)r->eax);
}

void xn_model_calc_radius_r(xn_regs *r)
{
    xn_model_calc_radius((struct xn_model *)r->eax);
}

void xn_model_calc_face_normals_r(xn_regs *r)
{
    xn_model_calc_face_normals((struct xn_model *)r->eax, r->edx);
}

/* the model in EAX, the face in EDX, the normal's place in EBX, the bits in ESI */
void xn_model_calc_face_normal_r(xn_regs *r)
{
    xn_model_calc_face_normal((const struct xn_model *)r->eax,
                              (const struct xn_model_face *)r->edx, (xn_vec3 *)r->ebx, r->esi);
}

void xn_model_submit_r(xn_regs *r)
{
    xn_model_submit((struct xn_model_handle *)r->eax, r->edx);
}

/* the handle in EDI, the frame in AL */
void xn_model_cull_and_queue_r(xn_regs *r)
{
    xn_model_cull_and_queue((struct xn_model_handle *)r->edi, (u8)r->eax);
}

/* the handle in EDI -> CF when a texture lookup failed */
void xn_model_draw_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_model_draw((struct xn_model_handle *)r->edi));
}

/* the model in EAX, the frame in EDX -> the model in EAX, its face data offset in EDX */
void xn_model_set_frame_r(xn_regs *r)
{
    struct xn_model *m = (struct xn_model *)r->eax;

    xn_model_set_frame(m, r->edx);
    r->edx = m->face_data_offset;
}

/* the frame in EAX, the frame count in EBX (its callers pass the model's own), the model in
   ESI -> the model in EAX, its face data offset in EDX */
void xn_model_set_frame_regs_r(xn_regs *r)
{
    struct xn_model *m = (struct xn_model *)r->esi;

    xn_model_set_frame(m, r->eax);
    r->eax = (u32)m;
    r->edx = m->face_data_offset;
}

/* the model in ESI -> CF when a texture lookup failed; the draw's state as xn_model_draw left
   it in its helpers' operands */
void xn_model_draw_faces_r(xn_regs *r)
{
    struct xn_model_draw_state s;

    s.handle = xn_model_draw_handle;
    s.rel.x = xn_model_eye_bf_x;
    s.rel.y = xn_model_eye_bf_y;
    s.rel.z = xn_model_eye_bf_z;
    s.matrix = xn_model_matrix_slot;
    s.dz_row.x = xn_model_depth_x;
    s.dz_row.y = xn_model_depth_y;
    s.dz_row.z = xn_model_depth_z;
    XN_SETFLAG(r, XN_CF, xn_model_draw_faces((const struct xn_model *)r->esi, &s));
}

/* the face in ESI -> the outcodes' AND and OR in BX */
void xn_model_transform_face_verts_r(xn_regs *r)
{
    struct xn_model_draw_state s;

    s.points = (xn_vec3 *)xn_model_points_x;
    s.matrix = xn_model_matrix_slot;
    s.rel.x = xn_model_eye_x;
    s.rel.y = xn_model_eye_y;
    s.rel.z = xn_model_eye_z;
    r->ebx = xn_model_transform_face_verts((const struct xn_model_face *)r->esi, &s);
}

void xn_model_build_light_list_r(xn_regs *r)
{
    (void)r;
    xn_model_build_light_list(xn_model_draw_handle);
}

/* the pool's next slot -> its m[1][2] in EDX (the asm's last product) */
void xn_model_scale_matrix_r(xn_regs *r)
{
    xn_model_scale_matrix(xn_render_matrix_next);
    r->edx = xn_render_matrix_next->m[1][2];
}

/* the model in ESI, the handle in EDI -> CF when hidden */
void xn_model_is_occluded_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_model_is_occluded((const struct xn_model *)r->esi,
                                              (const struct xn_model_handle *)r->edi));
}

/* the model in ESI, the handle in EDI -> the bounds in EAX EDX EBX ECX and 1/z in EDI, CF
   clear; or CF set with the centre in EAX EDX EBX and the rotation's address in ECX */
void xn_model_project_bounds_r(xn_regs *r)
{
    s32 x0, y0, x1, y1, inv_z;
    xn_vec3 c;

    if (xn_model_project_bounds((const struct xn_model *)r->esi,
                                (const struct xn_model_handle *)r->edi,
                                &x0, &y0, &x1, &y1, &inv_z, &c)) {
        r->eax = x0;
        r->edx = y0;
        r->ebx = x1;
        r->ecx = y1;
        r->edi = inv_z;
        XN_SETFLAG(r, XN_CF, 0);
    } else {
        r->eax = c.x;
        r->edx = c.y;
        r->ebx = c.z;
        r->ecx = (u32)&xn_cam_rotation;
        XN_SETFLAG(r, XN_CF, 1);
    }
}

/* the unrolled body's entry: the value in AL, the flags at EDI + 100h; the count is the step
   (6 bytes each, 1024 of them) where the caller planted a `ret` (C3h) */
void xn_model_clear_vert_flags_r(xn_regs *r)
{
    const u8 *body = (const u8 *)asm_xn_model_clear_vert_flags;
    int n;

    for (n = 0; n < 1024 && body[n * 6] != 0xC3; n++)
        ;
    xn_model_clear_vert_flags((struct xn_vert_flags *)(r->edi + 0x100), n, (u8)r->eax);
}
