# model: drawing ARCH3D models (0x140284-0x142228)

A model draw is spread over five functions that pass values to each other through patched
operands:
- `xn_model_draw` writes the eye in object space, the depth row and the model handle into the
  operands of `_draw_faces` and `_transform_face_verts`;
- `_draw_faces` writes the point list's address and its loop end;
- the camera setup writes the focal and view constants.

The vertex done-flags are cleared by a 1024-step unrolled body stopped by a planted `ret`.
Rules (PF, KEEP, RET) are in index.md.

## Groups MODEL-DRAW, MODEL-FACES, MODEL-VERTS, MODEL-LIGHTS, MODEL-SCALE

| VA | Name | Patch fields it reads (writer) | Fields it writes |
|---|---|---|---|
| 140284 | `xn_model_draw` | 14030A `mov ebp, DEPTH_SCALE` (12A3AC: 2^48/(scale_x*focal_x)) | 140497 (handle), 14040B/412/419 and 14052D/533/539 (eye in object space, twice), 140446/44D/454 (depth row = first matrix row x DEPTH_SCALE, high dwords), 14053E (matrix slot) |
| 1403AF | `xn_model_draw_faces` | 14040B/412/419 (`imul eax, [ebx], EYE_X` ...: back-face test), 140446/44D/454 (depth term), 140497 (`mov [edi+4], HANDLE`: polygon +4), 1404EB (self: loop end) | 14051C/522/528 (point list address, +0/+4/+8), 1404EB; plants `ret` in 140A28 |
| 1404FA | `xn_model_transform_face_verts` | 14051C/522/528 (`mov eax, [edi + POINTS]`: address fields), 14052D/533/539 (`add eax, EYE_X`), 14053E (`mov ecx, MATRIX`), 1405B4/BC/CC/D4 (12A2D0) | - |
| 140606 | `xn_model_build_light_list` | 140497 **as data** (`mov edi, [140497h]`), 140666/66C/672 and 14068B (self) | 140666/66C/672 (object position), 14068B (radius^2) |
| 1406D6 | `xn_model_scale_matrix` | 1406DD, 14072A (12A3AC: 2^45/scale), 1406E2, 14072F (12A274: 2^36/focal_x, 2^33/focal_y) | - |
| 140A28 | `xn_model_clear_vert_flags` | (the planted `ret`) | - |

All of them become plain C reading and writing the named fields (rule PF). For example:

```c
extern xn_model_handle *xn_model_draw_handle;        /* 140497: an operand, and data to 140606 */
extern s32 xn_model_eye_bf_x, xn_model_eye_bf_y, xn_model_eye_bf_z;    /* 14040B 140412 140419 */
extern s32 xn_model_eye_x, xn_model_eye_y, xn_model_eye_z;             /* 14052D 140533 140539 */
extern s32 xn_model_depth_x, xn_model_depth_y, xn_model_depth_z;       /* 140446 14044D 140454 */
extern s32 *xn_model_matrix_slot;                                      /* 14053E */
extern s32 xn_model_depth_scale;                                       /* 14030A (12A3AC) */

/* in xn_model_draw, after the eye is moved into object space: */
xn_model_draw_handle = h;                            /* written first, at 140297 */
xn_model_eye_bf_x = xn_model_eye_x = ex;             /* 1402E1 1402E6 */
...
xn_model_depth_x = xn_mulhi(m[0], xn_model_depth_scale);
...
xn_model_matrix_slot = m;
```

In `xn_model_transform_face_verts`, the point fetch `mov eax, [edi + POINTS]` becomes
`p = (const s32 *)((const u8 *)xn_model_points_x + vi)` (vi = the vertex index times 12). The
three address fields are `points`, `points + 4` and `points + 8`: declare them as three
`const u8 *` externs, not one.

**Groups:** each function is its own group as far as the fields are concerned. MODEL-FACES
(1403AF) and 140A28 form a RET pair (below).

Records: 28 each:
- 140284: 84 runs in all-C mode;
- 1403AF: 112;
- 1404FA: 140;
- 140606: 112;
- 1406D6: 112;
- 140A28: 140.

They are all called from `xn_render_draw_models` (12A7D0, 28).

**Traps.**
- **140606 reads 140497 as memory.** While 140284 is asm and 140606 is C (or the reverse), the
  variable is the shared channel, and that works because it is a field at a fixed address.
  Never copy it into a C-side static.
- `xn_model_draw` writes the eye twice (two sets of fields, read by different functions). Both
  stores are needed.
- 1404EB (`cmp ebx, END`) is written per call by 1403AF: rule KEEP, or use the field directly.
- The 14051C/522/528 fields are *address* operands (`[edi + disp32]`), the base of the model's
  points. Their values are `model + model->points_offset` (+0, +4, +8). In C they are pointers.
- The ABI rows give 140284/1403AF CF outputs (an occluded model, a failed texture lookup).
  These need the flags adapter.

## Group MODEL-FACES-CLEAR: `xn_model_draw_faces` (1403AF) and `xn_model_clear_vert_flags` (140A28)

**What.** Before drawing a model's faces, it clears the "transformed" flag of each of its
vertices: `xn_vert_flags[12k] = 0` for k < nverts.

**Mechanism.**
- 140A28: 1024 steps of `mov [edi+100h+12k], al`, 6 bytes a step.
- 1403AF sets edi = D49C0h (so `edi+100h` is `xn_vert_flags`) and eax = 0, plants `C3` at
  `140A28 + 6 * nverts` (`[esi+4]` times 3, times 2), calls, and restores `88h`.

```c
static void clear_vert_flags(int n)              /* 140A28, unrolled x1024 */
{
    int k;
    for (k = 0; k < n; k++)
        xn_vert_flags[k * 12] = 0;
}
/* in xn_model_draw_faces: */
clear_vert_flags(model->nverts);                 /* [esi+4]: the vertex count */
```

**Group:** 1403AF and 140A28 (rule RET). Adapter for 140A28's 28 own records:
`xn_planted_count(0x140A28, 6, 1024)`, with al = the value and edi = flags - 100h.

**Traps.**
- `[esi+4]` is the model's vertex count (the 12-byte vertex stride of `xn_vert_flags` is the
  ARCH3D index scale; the planted offset is `6 * count`, one 6-byte step per vertex).
- **nverts = 1024 would corrupt the code.** The `ret` would be planted on 140A28's own final
  `ret` (142228), and the restore would write `88h` over it. `xn_model_prepare` rejects
  1024 or more vertices, so this cannot happen. A C loop simply clears 1024 flags; document
  the difference rather than reproducing a code write.
- The value stored is al (0 here, `xor eax, eax`). The body would store any al; the C takes no
  value parameter, and the adapter passes al through for the body's synthetic records.
