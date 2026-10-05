# light: the per-pixel light shaders (templates 136C30/136C84/136D10, builders 15BCF6-15BF74)

A polygon lit by 1-3 point lights gets a small piece of machine code: a shader that returns
the shade at a view-space point. The builders patch one of three templates *in place* (object 2)
with the lights' positions, intensities and the polygon's base shade row. They copy the template
into a pool in `big_buffer` (`xn_light_code_next`, reset to `big_buffer` each frame by
`xn_render_begin_frame`) and store the copy's address in `poly->shade` (+14h). The span routines
`xn_span_solid_lit` (155920) and `xn_span_tex_lit` (156A94) call it at every 16-px boundary.
Two setup routines patch the templates' table address and clamp:
- `xn_light_init` (136A00): the light.dat falloff table address;
- `xn_shade_load` (136B88): the last shade row.

Rules (PF, KEEP, GEN, DIV ...) are in index.md.

## Group LIGHT-SHADERS

| VA | Name | Role | Records |
|---|---|---|---|
| 136C30 | `xn_light_tmpl_1` | template, 1 light, 0x54 bytes copied (ends with `ret`) | 2 (synthetic) |
| 136C84 | `xn_light_tmpl_2` | template, 2 lights, 0x8C bytes | 2 (synthetic) |
| 136D10 | `xn_light_tmpl_3` | template, 3 lights, 0xCC bytes (the `ret` at +C7h, then 4 bytes past it) | 2 (synthetic) |
| 15BCF6 | `xn_light_build_shader_1` | patch template 1, copy, poly->shade = copy, eax = 8 | 26 |
| 15BD78 | `xn_light_build_shader_2` | the same for 2 lights | 25 |
| 15BE4D | `xn_light_build_shader_3` | the same for 3 lights (also jumped to from 15C040) | 24 |
| 136A00 | `xn_light_init` | writes the falloff-table address into 6 template fields | 2 |
| 136B88 | `xn_shade_load` | writes `xn_shade_table + 3F00h` (last row) into 6 template fields | 2 |
| 15BCE8 | `xn_light_shade_constant` | the 0-light "builder": poly->shade = row, eax = 4 (no self-modifying code) | 28 |

The copies run inside 155920 (24 records) and 156A94 (26 records). Their records keep the
`big_buffer` pages, so the copies replay.

### 1. What a shader computes

Inputs:
- eax = the y ray of the pixel's row (`xn_cam_dir_y_mid[row]`);
- ebp = the x ray;
- ecx = z (from the 1/z table, `<< 9`).

Output: ebp = the shade (a pointer into the shade table, with an 8-bit fraction). eax, ebx, ecx
and edx are clobbered.

```
y = hi32(y_ray * z)             (imul)          x = hi32(x_ray * z)          zz = z >> 14
for each light i:
    d2_i  = SQ[y + ly_i] + SQ[x + lx_i] + SQ[zz + lz_i]           (u32; SQ = xn_squares_table_mid,
                                                                    SQ[k] = k*k << 8, k in -4096..4095)
    h_i   = hi32(d2_i * poly->falloff_i)                           (mul: unsigned)
    if (h_i & 0xFFFF8000) == 0:  shade += (u32)(xn_light_falloff[h_i] * intensity_i) >> 12
shade starts at ROW (the polygon's base row: ambient + directional), clamped: if shade > MAX
(signed) shade = MAX
```

`poly->falloff_i` lives in the polygon record (+08h, +0Ch, +10h). The builder stores it there,
and the shader's `mul` operand points at it. So a shader reads its polygon at run time.

### 2. Mechanism

**Template 1** (136C30), patch points (offsets from the template start):

| Offset | Instruction | Field | Written by | Value |
|---|---|---|---|---|
| +0Bh | `mov eax, [ebx*4 + SQ_Y]` | 136C3B | 15BD22 | `&SQ[ly]` (`lea [ly*4 + 125EC0h]`) |
| +15h | `add eax, [edx*4 + SQ_X]` | 136C45 | 15BD1D | `&SQ[lx]` |
| +1Ah | `mov ebp, ROW` | 136C4A | 15BD55 | `xn_light_shade_row` |
| +21h | `add eax, [ecx*4 + SQ_Z]` | 136C51 | 15BD28 | `&SQ[lz]` |
| +27h | `mul dword ptr [FALLOFF_P]` | 136C57 | 15BD44 | `&poly->falloff_1` (edi + 8) |
| +37h | `mov dx, [edx*2 + TAB]` | 136C67 | 136A46 | `xn_light_falloff` (light.dat) |
| +3Dh | `imul edx, edx, INTENSITY` | 136C6D | 15BD3C | `xn_light_point_intensity[0]` |
| +48h | `cmp ebp, MAX` | 136C78 | 136BB7 | `xn_shade_table + 3F00h` |
| +4Fh | `mov ebp, MAX` | 136C7F | 136BBC | the same |

**Templates 2 and 3** have the same fields per light:
- 136C84: SQ fields at +0Eh, +15h, +1Ch (light 1) and +23h, +2Ah, +31h (light 2); FALLOFF_P
  at +37h and +5Fh; ROW at +3Ch; TAB at +4Eh and +6Fh; INTENSITY at +54h and +75h; MAX at
  +80h and +87h.
- 136D10: SQ fields at +0Eh..+46h (nine), FALLOFF_P at +4Ch, +56h and +60h, ROW at +65h, TAB
  at +75h, +90h and +ABh, INTENSITY at +7Bh, +96h and +B1h, MAX at +BCh and +C3h.

The full list is in `patch_fields.csv` (group LIGHT-SHADERS).

**Who writes, who reads:**
- the builders write the per-polygon fields and then `rep movsd` the template into the pool;
- `xn_light_init` and `xn_shade_load` write the per-run fields (TAB, MAX), so every copy
  inherits them;
- the generated copy is called by 155920 (`call [esi+14h]`, 3 sites) and 156A94
  (`call [xn_span_tex_lit_shader]`, 3 sites).

**How the builders are reached:**
- `xn_light_setup_poly` (15BC42) and `xn_light_setup_terrain` (15BC9C) `jmp [ebp + 158C18h]`
  with ebp = 0/4/8/12 (the number of point lights collected times 4). The builder's `ret`
  returns to the setup routine's caller.
- `xn_light_add_point` (15BF75), when it fills the third slot, does `pop eax; jmp 15BE4D`. It
  drops its own return address and the dispatch loop's, so the builder returns straight to the
  setup routine's caller.

## 3. Readable C

The builders stay generators (rule GEN). Their writes are the behaviour: 6, 12 or 18 object-2
writes, one copy of 84/140/204 bytes into `big_buffer`, three dword writes into the polygon,
and `xn_light_code_next`.

```c
/* light.c */
extern u8  xn_light_tmpl_1[0x54], xn_light_tmpl_2[0x8C], xn_light_tmpl_3[0xCC]; /* object 2 */
extern u8 *xn_light_code_next;                       /* 136909 */
extern s32 xn_light_shade_row;                       /* 158C64 */
extern s32 xn_light_point_intensity[3], xn_light_point_falloff[3];   /* 158C28, 158C34 */
extern s32 xn_light_point_x[3], xn_light_point_y[3], xn_light_point_z[3];
extern s32 xn_squares_table_mid[];                   /* 125EC0: SQ[k] for k in -4096..4095 */

/* the patch points of a template, per light; offsets from the template's start */
typedef struct xn_shader_layout {
    int size, nlights;
    u8 sq_x[3], sq_y[3], sq_z[3], falloff_p[3], intensity[3];
    u8 row;
} xn_shader_layout;

static const xn_shader_layout shader_layout[3] = {
    { 0x54, 1, {0x15}, {0x0B}, {0x21}, {0x27}, {0x3D}, 0x1A },
    { 0x8C, 2, {0x15, 0x31}, {0x0E, 0x2A}, {0x1C, 0x23}, {0x37, 0x5F}, {0x54, 0x75}, 0x3C },
    { 0xCC, 3, {0x15, 0x31, 0x46}, {0x0E, 0x2A, 0x3F}, {0x1C, 0x23, 0x38},
               {0x4C, 0x56, 0x60}, {0x7B, 0x96, 0xB1}, 0x65 },
};
static u8 * const shader_tmpl[3] = { xn_light_tmpl_1, xn_light_tmpl_2, xn_light_tmpl_3 };

#define XN_PUT32(p, v)  (*(u32 *)(p) = (u32)(v))

/* asm: edi = poly; returns 8 (eax). The flags are the caller's (the glue keeps them). */
s32 xn_light_build_shader(xn_poly *poly, int nlights)
{
    const xn_shader_layout *l = &shader_layout[nlights - 1];
    u8 *t = shader_tmpl[nlights - 1];
    int i;

    for (i = 0; i < nlights; i++) {
        XN_PUT32(t + l->sq_x[i], &xn_squares_table_mid[xn_light_point_x[i]]);
        XN_PUT32(t + l->sq_y[i], &xn_squares_table_mid[xn_light_point_y[i]]);
        XN_PUT32(t + l->sq_z[i], &xn_squares_table_mid[xn_light_point_z[i]]);
        XN_PUT32(t + l->intensity[i], xn_light_point_intensity[i]);
        (&poly->falloff_1)[i] = xn_light_point_falloff[i];          /* +08h, +0Ch, +10h */
        XN_PUT32(t + l->falloff_p[i], &(&poly->falloff_1)[i]);
    }
    XN_PUT32(t + l->row, xn_light_shade_row);
    poly->shade = (u32)xn_light_code_next;
    memcpy(xn_light_code_next, t, l->size);
    xn_light_code_next += l->size;
    return 8;
}
```

The sketch's offsets for templates 2 and 3 must be copied from `patch_fields.csv` (the
writer-to-field pairs), not from this page's summary. The light-to-field order in template 2
is easy to get wrong: 15BDFD writes light 2's z into +23h, the field that comes *before*
light 2's y and x.

`xn_light_init` and `xn_shade_load` become plain C writing named template fields (rule PF):

```c
/* in xn_light_init: the light.dat falloff table, into the six TAB fields */
xn_light_t1_tab = xn_light_t2_tab1 = xn_light_t2_tab2 = xn_light_falloff;
xn_light_t3_tab1 = xn_light_t3_tab2 = xn_light_t3_tab3 = xn_light_falloff;
/* in xn_shade_load: the last shade row, into the six MAX fields */
xn_light_t1_max_a = xn_light_t1_max_b = xn_shade_table_last_row;
xn_light_t2_max_a = xn_light_t2_max_b = xn_shade_table_last_row;
xn_light_t3_max_a = xn_light_t3_max_b = xn_shade_table_last_row;
```

These are twelve named externs (`patch_fields.csv`): the fields are not contiguous, so they
are not an array in object 2. The per-light fields the builders write have names in the same
style (`xn_light_t2_sq_z2`, ...). The table-driven builder above is an equivalent that writes
them by offset; pick one style per file.

**The runner** has two honest forms, as for tmap.

- **Option A** (first): `xn_light_shade_at(poly, ray_y, ray_x, z)` calls `poly->shade` as code
  through `xn_asmcall` (eax = ray_y, ebp = ray_x, ecx = z; result in ebp). This is exact.
- **Option B** (port form, exact while the builder still writes the bytes): decode the copy.
  Which template a copy came from is in its bytes: at offset 2, `8B DA` is template 1, and
  `8B C5` is template 2 or 3. At offset 20h, `8B 0C` is template 2 and `8B 2C` is template 3.
  Then the C evaluates the formula in section 1 with the decoded fields:

```c
s32 xn_light_shade_eval(const u8 *code, s32 ray_y, s32 ray_x, u32 z)
{
    const xn_shader_layout *l = &shader_layout[xn_shader_kind(code)];
    s32 y = xn_mulhi(ray_y, (s32)z), x = xn_mulhi(ray_x, (s32)z);
    u32 zz = z >> 14;
    s32 shade = *(const s32 *)(code + l->row);
    int i;

    for (i = 0; i < l->nlights; i++) {
        const s32 *sx = *(s32 * const *)(code + l->sq_x[i]);
        const s32 *sy = *(s32 * const *)(code + l->sq_y[i]);
        const s32 *sz = *(s32 * const *)(code + l->sq_z[i]);
        u32 d2 = (u32)sy[y] + (u32)sx[x] + (u32)sz[(s32)zz];
        u32 h = xn_umulhi(d2, **(u32 * const *)(code + l->falloff_p[i]));
        if ((h & 0xFFFF8000u) == 0)
            shade += (s32)((u32)(xn_light_falloff[h] * *(const s32 *)(code + l->intensity[i]))
                           >> 12);
    }
    if (shade > xn_shade_table_last_row)        /* the MAX fields hold this value */
        shade = xn_shade_table_last_row;
    return shade;
}
```

`xn_umulhi` (`mul`, high dword) is a pragma to add to xngine.h.

## 4. Conversion groups and tests

- **LIGHT-SHADERS:** the three builders and the two template writers. Each can be converted
  alone (rule PF: the same bytes, to the same addresses).
  - The templates are never routed (`extern u8[]`).
  - Test: the builders' own records (26/25/24), and `xn_light_setup_poly` 15BC42 (3 records,
    112 runs in all-C mode).
  - 136A00 and 136B88 have 2 records each (init_video).
- **The runner** belongs to SPAN-SOLID-LIT and SPAN-TEX-LIT (span.md). Option A needs nothing
  else; option B needs the builders to keep writing the bytes.
- **LIGHT-SETUP** (15BC42, 15BC9C, the dispatch handlers 15BF75/15C045/15C046 and 15BCE8)
  needs no self-modifying-code design. There is one patch field:
  - `xn_light_setup_poly`'s `add ebx, AMBIENT_ROW` (15BC61), written by
    `xn_render_begin_frame`.

  The control flow needs care. The two `jmp [ebp + xn_light_shader_builders]` and
  15C03F's `pop eax; jmp 15BE4D` are tail calls that skip frames. In C:
  `return xn_light_build_shader(poly, n)` from the setup routine, and the dispatch handler
  returns a "full" status instead of jumping. While 15BF75 stays asm and 15BC42 becomes C,
  the asm handler's `pop eax; jmp` would skip the C frame: **convert 15BC42, 15BC9C and 15BF75
  together**. The ABI row of 15BF75 already sees the frame skip.

## 5. Exactness traps

- **The templates are written in place.** A C builder that patched only its copy would miss
  6, 12 or 18 object-2 writes per lit polygon.
- The copy sizes: template 3 copies 4 bytes past its `ret` (136DD8-136DDB, zeros); copy
  0x54/0x8C/0xCC bytes exactly.
- The FALLOFF_P field holds the address of a field in the polygon record, so the copy depends
  on that polygon. Never share copies between polygons in a C rewrite.
- `SQ[...]` has no bounds check: a light more than 4096 units away reads past the 8192-entry
  table. The C must index the same way (pointer arithmetic, no clamp).
- `mul` (unsigned) for d2 x falloff; `imul` (signed) for the rays; `imul edx, edx, imm` then
  `shr 12` (logical) for the intensity term.
- The clamp is a signed `cmp ebp, MAX; jle`.
- The builders do not change flags. A caller reads the CF it had before (ABI: "callers read
  flags it leaves alone: CF"), so the routing stub must keep EFLAGS.
- The builders' ABI rows list eax, ecx, edx, ebx and esi as outputs, because their callers are
  "partial" (they are reached by tail jumps). The real readers, 15BB8C and 12A700 after
  `call 15BC42`, overwrite everything but eax before reading it. Confirm that with the clobber
  test; until then, the glue returns the asm's values:
  - ecx = 0;
  - esi = the template's end;
  - edx = the copy's address;
  - ebx = the last `&poly->falloff_i`.

## 6. Where readable C cannot match

As for tmap:
- dropping code generation (a per-polygon `{row, lights[3]}` record instead of a copy) changes
  the `big_buffer` pool writes and the template writes;
- `xn_light_code_next` would still advance, but by a different amount.

Proposed honest form, for the port only:
1. A record-compare rule: the bytes the builders write between `big_buffer` and the frame's
   final `xn_light_code_next`, and the 45 template fields (9 + 15 + 21), are "generated code".
2. They are compared through a decoder: the Python twin of `xn_light_shade_eval`'s field
   reads.
3. `xn_light_code_next` is then compared as "advanced by the same number of shaders" rather
   than by bytes.

Until that rule exists, keep the builders byte-exact.
