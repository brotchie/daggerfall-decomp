# XnGine's run-time data structures

Groundwork for the readable C of XnGine (FALL.EXE object 2): the engine's run-time data as C
structs, with the evidence for every field. The files, all in `docs/engine/`:

- `engine_structs.h`: 64 structs, every member with its offset comment and every struct with a
  size check (`RECORD_SIZE`, or `RECORD_OFFSET` on a variable-length tail). It compiles with
  Watcom C32 10.0a (`work/compile_check.py`), and `work/offset_check.py` compiles one check per
  member comment (545 checks, all pass), so the comments are the layout.
- `fields.csv`: one row per field (`kind=field`, `address=STRUCT+0xOFF`), with confidence and
  evidence (instruction addresses). Union members share an address and have a row each.
- `globals.csv`: object-2 globals that the structs settle and that `config/names.csv` does not
  name yet (`kind=global`); every address was checked against names.csv.
- This file: per struct, its size, where its instances live, its users, the evidence per field,
  confidence and open questions. The sections after this overview are per subsystem.

How it was made: an annotated listing of all 719 functions (`work/listing.py`), the recorded
calls' memory read without an emulator (`work/recmem.py`: base snapshot + the record's pages),
the game's C and include/, the game's data files (ARCH3D.BSA, WOODS.WLD, TEXTURE.nnn,
FONT000n.FNT, MONSTER.BSA) and Daggerfall Unity's format code. The model and world sections
were written by two helper agents with the same tools; the coordinator merged and cross-checked
them (`work/merge.py`). `work/verify_evidence.py` checks that the instructions a field's
evidence cites address that offset.

## Using engine_structs.h

- Include it after `src/engine/xngine.h` when both are used: both define `xn_vec3` and
  `xn_mat3` (this header only when `XNGINE_H` is not defined). The compiler runs under DOS, so
  copy it to an 8.3 name where it is included (e.g. `xnstruct.h`).
- Everything is packed (`#pragma pack(1)`), as the asm lays it out; several fields are
  unaligned (`xn_view.flat_scale_x` at +0x79, the VID player).
- Unions are the engine's own reuse of bytes (a polygon's +14h is a light list, then a shade
  row or a shader; a flat's +4Ch a frame, then a blend table, then a fog row). They are
  anonymous, as include/records.h does it.
- Several structs are **overlays** of globals that names.csv already names one by one
  (`xn_view` at 0xCEA20, `xn_mouse_state`, `xn_kbd_state`, `xn_joy_state`, `xn_gfx_state`,
  `xn_font_state`, `xn_vid_player`, `xn_poly_clip_state`, `xn_light_point_slots`,
  `xn_collide_scratch`, `xn_scratch`, `xn_collide_seg_state`/`_sph_state`): they give types,
  units and the layout; the readable C may keep using the named globals.
- `xn_model_draw_state` is not memory: it gathers the values xn_model_draw patches into its own
  code, as the locals a C version would use.

## The structs

| struct | bytes | instances | main users | confidence |
|---|---|---|---|---|
| **renderer core** | | | | |
| xn_poly | 100 | frame pool 0xDB320 (1000) via xn_render_poly_next | model faces 1403AF, terrain 13EFD7, setups 15BB8C/12A700/12A740, light setup 15BC42, span routines, xn_span_insert, pick | confirmed |
| xn_span | 16 | row heads 0xF39F0, sentinel 0xF39E0, nodes to 0x116C80 | xn_span_insert 158D04, xn_render_frame 12A870, pick, occlusion 140910, flat clip 155200 | confirmed |
| xn_poly_vertex | 16 | xn_poly_vertex_buf_a/b 0x158EA0/0x1590A0 (32 each), water line points | projectors 158360/158420/1585C0, clipper 158678.., rasterizer 15B9A0 | confirmed |
| xn_poly_clip_state | 18 | overlay 0x158200 | clipper, cull sphere 15CF18 | confirmed |
| xn_vert_cam / xn_vert_screen / xn_vert_flags | 12 each | 0xCEAC0 / 0xD1AC0 / 0xD4AC0, 1024 each | terrain grid 13E8C8, model vertices 1404FA, projectors | confirmed (flags: strong) |
| xn_sort_pair | 8 | xn_model_queue 0x13F784 (200), xn_flat_sort_list 0x153C44 (512) | xn_render_sort_pairs 15810E, draw_models/draw_flats | confirmed |
| **lights, shading, textures** | | | | |
| xn_light | 29 | xn_light_table 0x136540 (32 + a terminator) | xn_light_add 136AD8, light lists, flats 155610 | confirmed |
| xn_light_ref | 24 | light-list pool 0x116C80 (400) | 140606, 13E63C, 15BC42, 15BC9C, dispatchers | confirmed |
| xn_light_point_slots | 64 | overlay 0x158C28 | 15BF75, shader builders 15BCF6.. | confirmed |
| xn_tex_block | 22 | the texture heap (cfg_texture_memory KB), head 0x1343E4 | heap routines 1360EE..13622F | confirmed |
| xn_tex_archive | 26 + 20n | heap blocks, xn_tex_archives 0x132F6C[512] | 135EAB, 135D00 | confirmed |
| xn_tex_entry | 20 | archive directories | xn_tex_cache_lookup 135D00, polygon +40h, flats | confirmed |
| xn_tex_image | 28 + 4 frames | in the archives | lookup, setup 15BB8C, tmap compile, flats | confirmed |
| xn_tex_frame | 4 + data | animated records | 136282, 13641D | strong |
| xn_tex_unpack_strip / _entry | 8 / 8 | 0x1343FA (256) / 0x134C02 (256) | 1362CF, 1363D9 | strong |
| xn_tmap_step / xn_tmap_copy | 52 / 418 | template 0x15C300, pool xn_tmap_pool (768) | 15C274, 15C2DC, xn_span_tex_lit 156A94 | confirmed |
| **models and collision** | | | | |
| xn_model | 64 | the game's model cache (ARCH3D records) | prepare 13FE15, draw 140284, collision | confirmed |
| xn_model_frame | 16 | frame table (no Daggerfall model has frames) | 13FF0E, 14037A | strong |
| xn_model_face / _face_point | 8 + 8n / 8 | model data | 1403AF, 158420, 15BB8C, 15BC42, collision | confirmed |
| xn_model_face_data | 24 | model data (one per face) | 13FF65, 15BAC0 | confirmed |
| xn_model_sphere / _sphere_face | 0x12 + 6n / 6 | model data (collision spheres) | 14A300, 14AA92 | strong |
| xn_model_handle | 58 | type-6/32 object data, block models, the arrow | 1401D4, 140284, 140606, C7F07.., 15BC42, collision | confirmed |
| xn_model_matrix_slot / _pool | 36 / 14400 | 0xD7AC0: view[200], light[200] | 140284, 1406D6, 15BAC0, 15C00F | confirmed |
| xn_model_draw_state | 56 | (patched immediates of 140284..) | 1403AF, 1404FA, 140606 | strong |
| xn_collide_probe / _probe_sphere | 0x1A + 16n / 16 | the game's probes (D_00196D4C) | 14AA92 | strong |
| xn_collide_hit / _hits | 30 / 4 + 30n | big_buffer | 14A300, 14AA92, the game (colstuff.c) | confirmed |
| xn_collide_scratch | 0x148 | 0x14A100 | collision, ground height 14B45B | strong |
| xn_collide_seg_state / _sph_state | 36 / 36 | after the code of 14A300 / 14AA92 | those two | strong |
| xn_scratch | 44 | 0x120288 (pick_distance is its a.z) | cull sphere, pick, flats, terrain, model u/v | strong |
| **world, terrain, flats, animation, sky** | | | | |
| xn_anim | 24 | creature data + 0x2C1 (records.h monster_anim) | C010F, C013B, C019C, opcodes | confirmed |
| xn_ascr | 6 + 2n + scripts | MONSTER.BSA records | C013B, opcodes | confirmed |
| xn_wld_header | 144 | xn_world_header 0xC27E9 | C310F, C322D, C3301 | confirmed |
| xn_wld_cell_header / xn_wld_cell | 22 / 47 | WOODS.WLD; xn_world_cell_header 0xC2879 | C3301, C343E, C355D, 13EFD7 | confirmed |
| xn_wld_bands | 4 | xn_world_height_bands 0xC23E9 (256) | C3B87, C3C3C, C3A60 | confirmed |
| xn_world_nature_odds | 129 | 0xC2BB8 | C3A60 | confirmed |
| xn_terrain_vert_coord | 12 | 0x138578, 0x13B578 (1024 each) | 13E8C8, 13EB21, 13EC22 | confirmed |
| xn_flat | 100 | the polygon pool (512 a frame) | xn_flat_add 154D00.., draw 154E20, flat spans 157620.., pick 155508 | confirmed |
| xn_sky_star / xn_snow_flake | 12 / 6 | 0xC547A (768) / 0xC7B8B (100) | C816E; C9BF7, C9A89 | strong |
| **camera, input, graphics, fonts, VID** | | | | |
| xn_view | 137 | overlay 0xCEA20 | camera setup 12A274..12A3AC, every projector and pool user | confirmed |
| xn_mouse_state / xn_mouse_press | 828 / 8 | overlay 0x12AC00 | 12B100..12B49E | confirmed |
| xn_kbd_state | 407 | overlay 0x142300 | 142700..1428ED, keys.c | confirmed |
| xn_joy_state / xn_joy_axes | 110 / 8 | overlay 0x152A00 | 152B00..152F41, options.c | confirmed |
| xn_gfx_state | 3158 | overlay 0x14291B | 143600.., every blit | confirmed |
| xn_rm_regs | 50 | 0x15FA08, 0x161300 | VESA driver, helmet driver C | strong |
| xn_vesa_mode_info | 256 | 0x15FA6D | 15FCA7, 143600 | strong |
| xn_fnt_file / xn_fnt_glyph | 964 / 4 | xn_font_table 0x12DA54 (8 slots) | 12DB28.., 12DBCC, 12DC44 | confirmed |
| xn_font_state | 64 | overlay 0x12DA38 | font module | confirmed |
| xn_vid_header / xn_vid_player | 15 / 1010 | 0xC1000 / overlay 0xC10FF | VID player C1500..C1E10 | strong |

## The frame's work areas (object 2 data, fixed addresses)

| Address | Contents | Count | Reset by |
|---|---|---|---|
| 0xCEA20 | `struct xn_view` (projection constants, pool cursors) | 1 | camera setup; cursors by xn_render_begin_frame |
| 0xCEAC0 | `struct xn_vert_cam` | 1024 | per terrain draw / per model |
| 0xD1AC0 | `struct xn_vert_screen` | 1024 | same |
| 0xD4AC0 | `struct xn_vert_flags` | 1024 | same |
| 0xD7AC0 | `struct xn_model_matrix_pool`: view[200], light[200] at +0x1C20 | 200 | begin_frame (xn_render_matrix_next) |
| 0xDB320 | `struct xn_poly` / `struct xn_flat` (100 bytes each) | 1000 | begin_frame (xn_render_poly_next) |
| 0xF39E0 | the sentinel `struct xn_span` | 1 | begin_frame |
| 0xF39F0 | a `struct xn_span` row head per screen row, then the span-node pool | 200 + ~8900 | begin_frame (xn_render_span_next) |
| 0x116C80 | `struct xn_light_ref` light lists | 400 | begin_frame (xn_render_light_list_next) |
| 0x119200 | 28800 zero bytes nothing addresses (1200 more light-list entries' worth) | | |
| 0x120280 | the light-list pool's start and end (two pointers, no reader) | | |
| 0x120288 | `struct xn_scratch` (shared scratch vectors; pick_distance) | 1 | |
| 0x1202C0 / 0x1212C0 | view-ray tables x[1024], y[768] | | xn_cam_update_derived |
| 0x121EC0 | squares table int[8192] (centred) | | xn_render_init |
| 0x129EC0 | texture size masks [256] | | xn_render_init |

None of the pools is bounds-checked except the model queue (199 models) and the flat list (512
flats): a frame with more than 1000 polygons and flats, ~8900 span nodes or 400 light-list
entries would run into the next area. The records' busiest frame had 362 polygons.

## Across the subsystems

- **The polygon pool holds two layouts.** Model faces and terrain cells are `struct xn_poly`,
  flats are `struct xn_flat`; both are 100 bytes and share +0 (face / image), +4 (owner: a model
  handle, 1 terrain, 0 a flat: include/structs.h `xn_pick_hit.model`), +3Ch (the span routine)
  and +40h (texels after setup). `xn_render_pick` returns either.
- **Model faces carry camera-space data until their first span.** A face's +18h..+20h hold its
  first vertex in camera space until `xn_poly_setup_textured` reads them and turns +18h into the
  packed texture origin; a terrain cell's +50h..+58h hold its normal until
  `xn_render_span_setup_terrain` replaces them with the 16-px steps. A C version must keep the
  order or use separate fields.
- **Lights change coordinate space mid-frame.** `xn_light` positions are world units until
  `xn_light_to_view` (from `xn_render_draw_flats`, after the models were drawn) rewrites them in
  view space << 8 in place; the flats read the view-space values.
- **Two matrices per model.** The matrix pool's `view[i]` (rescaled for the texture gradients by
  1406D6) and `light[i]` at +0x1C20 (for the point-light foot points, 15C00F) both come from the
  object x camera rotation; the model handle's +8 points at `view[i]`.
- **The vertex arrays' flag bytes differ by user.** For models, `xn_vert_flags` +0 is the
  transformed flag and +2 the outcode; for the terrain, +0 the flat-layer byte, +1 the outcode
  with the height's bit 7, and +2 the tile byte.
- **big_buffer is everybody's scratch** (see the camera and I/O section): the frame's light
  shaders, collision hit lists, the VID read buffer, the world generator's grid, scaled-image
  code and the game's own uses never overlap in time, but nothing enforces it.

## Conflicts with include/ (structs.h, records.h)

None is a wrong offset of something the game uses; they are fields the game calls padding, or
names that describe a different use:

1. **records.h `struct model_instance` (56 bytes) is too short and misnamed** (model section):
   the engine writes +0x38 (frame) and +0x39 (flags: bit 0 occluded, bit 1 drawn) through the
   handle, so the engine's struct is 58 bytes (type-6/32 objects get 62 bytes of data, so it
   fits). `pad04[8]` is the light list and the matrix slot; `angles[20]` at +0x0C is 2 unused
   bytes, the base angles (shorts at +0x0E/+0x10/+0x12) and the engine's eye-relative position
   (+0x14..+0x1C); `missile_angles` at +0x2C are every model's draw angles (pitch, yaw, roll),
   not only arrows'. For block models the engine writes the same fields over the RMB record's
   DFU Unknown2/Unknown3/XPos1..ZPos1 every frame.
2. **records.h `struct monster_anim`** (world section): +0x02 `pad02` is written with a copy of the
   frame; +0x0C `frame_count` is the tick divisor (ticks per step); +0x0E `timer` is the last
   step quotient; `pad12[2]` is the wait count (+0x12) and opcode 10's byte (+0x13). The engine's
   struct is 0x18 bytes; +0x18 is the game's.
3. **structs.h `struct collide_probe`**: `pad0C[12]` is the probe's rotation (pitch, yaw, roll),
   read by 14AA92 on every call (the game leaves it 0). **`struct collide_hit`**: `pad1C` is t/2
   (8000h = the segment's end) for the segment test and FFFFh for the sphere test; x/y/z are
   written only by the segment test.
4. **structs.h `struct arch3d_plane`**: `unknown1` (+0x01) is the face's base shade row
   (15BC5C); the points' u/v words are rewritten by xn_model_prepare (point 0's shifted left 4;
   points 1 and 2 become the plane constant and the face-data pointer).
5. **structs.h `struct arch3d_header`**: the layout agrees; +0x10/+0x14 (`null_value1`) are the
   engine's animation frame count and frame table (0 in every model).
6. **structs.h `struct tex_cache_entry`**: agrees (+0x0C); its `pad00[12]` hides +0x02 (the
   record header) and +0x0A (the flats' blend-table index). **`struct texture_header`** agrees;
   the engine also overwrites x/y with the packed wrap masks of compiled records.
7. **structs.h `struct sos_sample`**: the VID player (same driver) also writes +0x5C, the
   sample-done callback, inside `pad48`.
8. **names.csv**: `xn_cam_forward_x/y/z` (0x136E18..20) are row 2 of `xn_cam_rotation`, not
   separate globals; `xn_collide_matrix_b` (0x14A100) is filled as a ground triangle (three
   `xn_vec3`) by `xn_terrain_height_at`, not a rotation (world and model sections); `pick_distance`
   is the z of the shared scratch vector `xn_scratch.a`, meaningful only right after a pick.

## Open questions most worth an experiment

1. **Pool limits under stress** (render): record `xn_render_frame` 12A870 in the busiest scene
   available (a large city with many flats, a dungeon with many models) and take the maximum of
   polygons, span nodes and light-list entries: the readable C needs either the same limits or
   checks.
2. **The first piece of every flat** (world): `xn_flat_span_light_setup` 155610 installs the real
   span routine and returns without drawing; record `xn_flat_span_emit` 15526C for one flat in a
   town and compare the pixels written with the span lengths.
3. **Point-light shaders and light type 4** (light): `xn_span_tex_lit`'s compiled shaders and
   `xn_span_solid_lit` 155920 need a torch-lit dungeon; record 15BC42 with 2-3 point lights to
   see all of `xn_poly.shader_falloff` and `xn_light_point_slots`. Light type 4 is dead: every
   game caller passes 0 or 8 (args.c, objlib.c, sky.c, automap.c, func_000830C7.c).
4. **Translucent flats** (light/world): the ghost and wraith archives (273, 278) get
   `xn_tex_entry.blend_index` = 1 and should draw through 157E20, which has no records; put one
   in view.
5. **Handle flags bit 1** (model): set by the light setup when a model's polygon is drawn; who
   reads it? Watch an object's data+0x39 across visible and hidden frames.
6. **VID audio delay** (io): `xn_vid_player.audio_state` never moves from 2 to 3 when the delay
   runs out; scan ANIM*.VID's 7Ch chunks for a non-zero delay.
7. **Model frames** (model): `xn_model_frame` and 14037A never ran (no Daggerfall model has
   frames); a hand-made two-frame model would exercise them.

# Renderer core: polygons, spans, vertex buffers

## struct xn_poly (100 bytes)

- What: a polygon being drawn this frame: what its spans point at, and the state its span routine reads (texture gradients, steps, shading). Flats use the same 100 bytes with another layout (struct xn_flat, world section); the two views share +3Ch (span routine) and the first 8 bytes (xn_pick_hit).
- Instances: the frame pool xn_render_poly_pool 0xDB320 .. 0xF39E0 (1000 records), taken through xn_render_poly_next 0xCEA64 (+= 0x64; reset by xn_render_begin_frame 12A4F0, which takes record 0 as the background record: span routine 12A949). No overflow check: more than 1000 records in a frame would run into the span sentinel and the row heads (the most seen in the records: 362).
- Users: writers: xn_model_draw_faces 1403AF and xn_poly_project_face 158420 (model faces), xn_terrain_draw_cells 13EFD7, xn_terrain_face_plane_a/b 13EB21/13EC22 and the axis setters 13F4CC..13F608 (terrain), xn_flat_add 154D20 (flats); the setups xn_render_span_setup_solid 12A700, _terrain 12A740, _terrain_solid 12A7A0, xn_poly_setup_textured 15BB8C, xn_poly_tex_gradients 15BAC0, xn_light_setup_poly 15BC42, xn_light_setup_terrain 15BC9C, the shader builders 15BCF6/15BD78/15BE4D, xn_span_tex_16_setup 155F60, xn_span_solid_shaded_setup 155820; readers: every span routine (155800..157240), xn_span_insert 158D04 (+5Ch), xn_render_pick 12A608 (+5Ch), xn_flat_span_clip 155200 (+5Ch of world spans), the light dispatchers 15BF75/15C046/15C08C (+0, +4, +48h, +50h..58h), xn_model_push_player_from_pick C810C (+4, +48h), the game's engine_pick_object (src/lifted/engsupp.c: +0, +4).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | face | struct xn_model_face * (union: flat_image) | 1403AF/14048F mov [edi],esi (the face); 15BC58 mov eax,[edi]; mov bh,[eax+1] (base shade row); 15BBC5 [edx+1Ch] axes; 15BC2D [edx+0Ch] origin; flats: 154D20 image; records: 12A870 cheat_dorian__walk / cheat_gash__swing decoded with render/poolscan.py and poolscan2.py: 0182C906 = a face of model 82B06A | confirmed |
| +0x04 | handle | struct xn_model_handle * (union: owner) | 140494 mov [edi+4],imm (patch_140497 = the handle); 13EC17 mov [edi+4],1 (terrain); 15BC42 mov eax,[edi+4]; or byte [eax+39h],2; 15BBCA [ecx+8] matrix; engsupp.c pick_hit->model == 1; records: 01967214 (a handle), 1 (terrain) | confirmed |
| +0x08 | shader_falloff | int[3] | 15BD41 mov [edi+8],edx; 15BE16 mov [edi+0Ch],edx; 15BF3E mov [edi+10h],edx (xn_light_point_falloff[k]); the templates read them through patched addresses (15BD44 lea ebx,[edi+8] -> 136C57); records: 00048D15 in lit faces | strong |
| +0x14 | light_list / shade_row / shader | union of pointers | 13EC14 mov [edi+14h],edx (xn_render_light_list_next: terrain); 15BCC2 mov esi,[edi+14h] (15BC9C walks it); 15BCED mov [edi+14h],eax (xn_light_shade_row, index 4); 15BD5A mov [edi+14h],edx (xn_light_code_next, index 8); 155820 / 155920 call [esi+14h]; 156A9C mov eax,[esi+14h]; records: 01116C80 terrain, 01248800 shade row, 0120913C shader in big_buffer | confirmed |
| +0x18 | cam_x / tex_u0 | int (union: unsigned short u0, v0 at +18h/+1Ah) | 1404AD mov [edi+18h],ecx (the face's first vertex from xn_vert_cam); 15BBDB imul esi,[edi+18h],imm (read before setup); 15BC37 mov [edi+18h],edx (packed origin); terrain 13F4ED mov [edi+18h],ecx and 13F5A5 mov word [edi+1Ah],cx; span routines 155D9F mov eax,[esi+18h]; records: B8919C02 after setup | confirmed |
| +0x1C | cam_y | int | 1404B0 mov [edi+1Ch],edx; 15BBE2 imul ecx,[edi+1Ch],imm (half height) | strong |
| +0x20 | cam_z | int | 1404B3 mov [edi+20h],ebx; 15BBD8 mov ebx,[edi+20h]; 15BBFE imul [edi+20h] | strong |
| +0x24 | u_dx | int (u/z per screen x) | 15BAF2 mov [edi+24h],ebp (= [+50h] >> 3); 13F4D2 (terrain); 155D60 imul ebx,[esi+24h] (ebx = x - centre x); 156AEA | confirmed |
| +0x28 | u_dy | int | 15BB08 mov [edi+28h],ebp; 155D64 imul eax,[esi+28h] (eax = xn_render_row_y) | confirmed |
| +0x2C | u_c | int | 15BB24 mov [edi+2Ch],ebp; 155D68 add eax,[esi+2Ch] | confirmed |
| +0x30 | v_dx | int | 15BB57 mov [edi+30h],ebp; 13F58A; 155D41 imul eax,[esi+30h] | confirmed |
| +0x34 | v_dy | int | 15BB72 mov [edi+34h],ebp; 155D45 imul edx,[esi+34h] | confirmed |
| +0x38 | v_c | int | 15BB88 mov [edi+38h],ebp; 155D49 add edx,[esi+38h] | confirmed |
| +0x3C | span_fn | void (*)(void) | 12A939 call [eax+3Ch] (eax = the polygon); 1404DA mov [edi+3Ch],ebx (xn_render_span_setups[entry+10h]); 13F067 (terrain, patched); 15BBC7 / 12A716 / 12A75D store the real routine; 15BC3E jmp [eax+3Ch]; records: 0112A740, 011566C4, 01156A94 | confirmed |
| +0x40 | tex / texels / colour4 | union | 1404D1 mov [edi+40h],eax (xn_tex_cache_lookup's entry); 15BB90 mov esi,[eax+40h]; mov esi,[esi+0Ch]; 15BBA6 mov [edi+40h],esi (texels); 12A723 mov [edi+40h],eax (xn_colour_fill_table); 12A74F (terrain texels); 155DC2 mov esi,[esi+40h]; 155803 mov eax,[eax+40h] | confirmed |
| +0x44 | tmap | void (*)(void) | 15BBA0 mov [edi+44h],eax (image +0Ah); 156A97 mov edx,[eax+44h] -> xn_span_tex_lit_tmap; records: 017AB096 in the tmap pool | confirmed |
| +0x48 | normal | int * (12 bytes in the model's normal list) | 14046C mov [edi+48h],esi (the normal walked 12 bytes per face); 15BF75 mov ebx,[edi+48h]; 15C046 mov ecx,[edi+48h]; C810C reads pick+48h; records: 0183025A | confirmed |
| +0x4C | wrap_mask | unsigned int | 15BBA3 mov [edi+4Ch],ebx (image +0); 155DBF mov ebp,[esi+4Ch]; and ebx,ebp (155E32); records: 7FFF3FFF (u mask 3Fh, v mask 7Fh) | confirmed |
| +0x50 | u_step / nx | int (union) | 15BAEC mov [edi+50h],ebp; 155D8E mov eax,[esi+50h]; 155F60 shl [eax+50h],1; terrain 13EBD9 mov [edi+50h],ebp (normal), 15C095 imul eax,[edi+50h], 12A772 mov [edi+50h],eax | confirmed |
| +0x54 | v_step / ny | int (union) | 15BB51 mov [edi+54h],ebp; 13EBD3; 15C099; 12A775 | confirmed |
| +0x58 | inv_z_step / nz | int (union) | 140466 mov [edi+58h],eax; 155D85 mov eax,[esi+58h]; 155ECE add ecx,[esi+58h]; terrain 13EBD6 / 15C09D / 12A778 | confirmed |
| +0x5C | inv_z_dx | int | 14046F mov [edi+5Ch],eax (= [+58h] >> 3; also xn_span_dzdx 140472); 13EC06 (terrain); 158D30 imul edx,[eax+5Ch] (the S-buffer's depth test); 12A67A (pick); 15BBAE (the 16-px choice); 155A39 | confirmed |
| +0x60 | dx_per_inv_z | int | 15BC55 mov [edi+60h],eax (2^32 / [+5Ch] by idiv with edx = 1); 15BCA8; records: FFFDE5CA | confirmed |

- Confidence: strong overall; +0..+4, +14h, +18h, +24h..+60h confirmed by record values (model and terrain polygons decoded before and after xn_render_frame).
- Conflicts with include/: none. include/structs.h struct xn_pick_hit is this record's first 8 bytes (plane = +0 face, model = +4 handle; 1 terrain, 0 a flat) and agrees.
- Open: +08h..+10h: only the compiled light shaders' falloffs were seen written (15BD41 [edi+8], 15BE16 [edi+0Ch], 15BF3E [edi+10h]); flats use +08h/+0Ch for their own fields. A 15BC42 record with 2-3 point lights would show all three.
- Open: Terrain records never write +0 (it keeps a previous frame's value): readers of a terrain polygon's face would see garbage; only engine_pick_object reads it after checking +4 == 1. Confirm no other reader with a pick on the ground.
- Open: The pool limits (1000 polygons, ~8900 span nodes, 400 light-list entries) are not checked anywhere: a stress scene (big city, many flats) recorded at 12A870 would show how close play comes.

## struct xn_span (16 bytes)

- What: a span of a screen row: the S-buffer keeps, per row, a sorted list of non-overlapping spans, each the nearest polygon over its pixels; nothing is drawn until every polygon is inserted.
- Instances: row heads: xn_render_span_rows 0xF39F0 + row * 16 (one xn_span per screen row, only .next used); the sentinel xn_render_span_sentinel 0xF39E0 (next = itself, x_start = clip right, x_end = clip right + 1, poly = the background record 0xDB320: records show 010F39E0 01400141 00000000 010DB320); nodes from xn_render_span_next 0xCEA60, starting after the row heads (12A563) and running to the light-list pool at 0x116C80 (about 8900 nodes at 200 rows).
- Users: xn_render_begin_frame 12A4F0 (heads, sentinel), xn_span_insert 158D04 (from xn_poly_rasterize 15B9A0), xn_render_frame 12A870 (12A91E..12A939), xn_render_fill_background 12A98B, xn_render_pick 12A608, xn_model_is_occluded 140910, xn_flat_span_clip 155200, xn_water_draw 12F79C.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | next | struct xn_span * | 12A53C mov [edi],edi (sentinel); 12A55B mov [eax],edi (row heads); 158D06 mov esi,[esi]; 158DA8 mov [eax],esi; 12A91E push [esi] | confirmed |
| +0x04 | x_end | unsigned short | 12A549 mov [edi+4],ax (clip right + 1); 158D08 mov edx,[esi+4]; cmp dx,bp; 158DDF mov [eax+4],ebp (bx / bp << 16); 12A924 mov bp,[esi+4]; sub ebp,ebx (pixel count) | confirmed |
| +0x06 | x_start | unsigned short | 12A543 mov [edi+6],ax (clip right); 158D10 shr edx,16; 12A920 mov bx,[esi+6]; add edi,ebx | confirmed |
| +0x08 | inv_z | int (2^40/z at x_start) | 158DE2 mov [eax+8],ecx; 158D34 add edx,[esi+8]; 155D20 mov ecx,[esi+8]; shr ecx,0Dh (reciprocal table index); 12A67E | confirmed |
| +0x0C | poly | struct xn_poly * | 158DE5 mov [eax+0Ch],edi (xn_render_poly_next: the polygon being built); 12A54D (sentinel -> background record); 12A928 mov eax,[esi+0Ch]; 12A673 (pick returns it) | confirmed |

- Confidence: confirmed (asm and records: row 77 of a frame reads 0-113 | 113-125 | ... | 320-321 sentinel).
- Open: The row-head array's length follows xn_gfx_height (12A555 mov ecx,[0x142934]); with a VESA mode taller than 200 the node pool shrinks. Only 13h runs in play.

## struct xn_poly_vertex (16 bytes)

- What: a vertex of the polygon being clipped and projected.
- Instances: xn_poly_vertex_buf_a 0x158EA0 and xn_poly_vertex_buf_b 0x1590A0 (= a + 0x200), 32 each; the rings xn_poly_ring_a 0x159294 / _b 0x1592FC (29 pointers each, n = 0..28; the two arrays overlap by 3 entries, which n >= 3 never reads) point, per vertex count n, at 3n+1 vertex pointers into the buffer (the ring repeated so both edge walkers can step past the ends); the water code's line points xn_water_line_p0/p1 0x12F47A/0x12F48A are two more.
- Users: xn_poly_project_face 158420, xn_poly_project_terrain 1585C0, xn_poly_project_flat 158360, xn_poly_clip_frustum 158678, xn_poly_clip_plane 1587D4, the intersect routines 158864..158B1C, xn_poly_rasterize 15B9A0, xn_flat_raster 1552A0, the terrain builders 13EE77/13EF27/13EFD7, the flat quads 154EDC/154FD8, the water 12F59C.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | x | int | 15846A mov [edi],eax (xn_vert_cam x); 1583AB mov eax,[esi]; ... shr edx,3; mov [esi],edx (screen x << 5); 15B9E0 mov edx,[ecx] | confirmed |
| +0x04 | y | int | 15846C; 1583C0 mov eax,[esi+4] ... shr edx,8; mov [esi+4],edx (the row); 15B9D5 mov di,[ecx+4] | confirmed |
| +0x08 | z | int | 15846F; 15839A mov ebx,[esi+8]; div ebx (edx = 100h); mov [esi+8],eax (2^40/z); 15B9E2 mov eax,[ecx+8] | confirmed |
| +0x0C | outcode | unsigned char | 158472 mov [edi+0Ch],ch; 15880D mov al,[esi+0Ch]; 1588DA mov [edi+0Ch],cl; 155020 (flat quads) | confirmed |
| +0x0D | pad_0d | char[3] | never addressed | strong |

- Confidence: confirmed (asm); a polygon reaches xn_poly_rasterize with at most 27 vertices (1584E9 cmp cl,1Ch; jae reject).

## struct xn_poly_clip_state (18 bytes)

- What: the clipper's globals, contiguous at 0x158200 (an overlay; names.csv names each).
- Instances: 0x158200 (one).
- Users: xn_poly_project_* 158360/158420/1585C0, xn_poly_clip_frustum 158678, xn_poly_clip_plane 1587D4, the intersect routines, xn_cam_cull_sphere 15CF18 (sets outcode_or), the flat quads 154EDC/154FD8, the water 12F59C.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | src | struct xn_poly_vertex * | 158360 mov [158200h],esi; 1587DD; 158836 xchg (ping-pong) | confirmed |
| +0x04 | dst | struct xn_poly_vertex * | 15837D mov [158204h],eax (src + 200h); 1587FA | confirmed |
| +0x08 | src_end | struct xn_poly_vertex * | 158370; 1587F3 add [158208h],10h; 15882B | confirmed |
| +0x0C | intersect | void (*)(void) | 158686 mov [15820Ch],158A90h; 15884C call [15820Ch] | confirmed |
| +0x10 | outcode_and | unsigned char | 15842F mov [158210h],bx (and, or as a word); 1587D4 mov word [158210h],7Fh; 158821 and [158210h],al | confirmed |
| +0x11 | outcode_or | unsigned char | 158369 test byte [158211h],0FFh; 15881B or [158211h],al; 15CF8C mov [158211h],cl (cull sphere) | confirmed |

- Confidence: confirmed (asm).

## struct xn_vert_cam (12 bytes)

- What: a vertex in camera space (world << 8 through the scaled view matrix; the frustum is x = +-z, y = +-z).
- Instances: xn_vert_cam 0xCEAC0, 1024 elements (to 0xD1AC0), indexed by a byte offset (vertex index * 12: a model face's point offsets). The terrain fills it as a 32 x 32 grid (row stride 0x180); each drawn model reuses it for its vertices.
- Users: xn_terrain_transform_grid 13E8C8 (13EA01..13EA18), xn_model_transform_face_verts 1404FA (140547..140553), xn_poly_project_face 158458, xn_model_draw_faces 14049B (the polygon's +18h), the terrain triangles 13EE8A.., the terrain face planes 13EB21/13EC22, xn_terrain_add_nature_flats 13E718.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | x | int | 140547 mov [edi+0CEAC0h],eax; 13EA01; 158458 mov eax,[ebx+0CEAC0h] | confirmed |
| +0x04 | y | int | 14054D mov [edi+0CEAC4h],edx; 13EA12 | confirmed |
| +0x08 | z | int | 140553 mov [edi+0CEAC8h],ebx; 13EA18; 13EB3F | confirmed |

- Confidence: confirmed (asm).

## struct xn_vert_screen (12 bytes)

- What: a projected vertex (written only when its outcode is 0).
- Instances: xn_vert_screen 0xD1AC0, 1024 elements, parallel to xn_vert_cam.
- Users: xn_model_transform_face_verts 1404FA (1405AB/1405C4/1405DB), xn_terrain_transform_grid 13E8C8 (13EA69/13EA85/13EA9C), the unclipped path of xn_poly_project_face 158534, the terrain triangles 13ED23/13EDCD, xn_terrain_draw_cells 13F2CA.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | sx | int (screen x << 5) | 1405C4 mov [edi+0D1AC0h],edx (after shr edx,3); 158541 mov edx,[edi+0D1AC0h] | confirmed |
| +0x04 | sy | int (row) | 1405DB mov [edi+0D1AC4h],edx (after shr edx,8); 158552 | confirmed |
| +0x08 | inv_z | int (2^40/z) | 1405AB mov [edi+0D1AC8h],eax (div with edx = 100h); 158549 | confirmed |

- Confidence: confirmed (asm).

## struct xn_vert_flags (12 bytes)

- What: per-vertex flag bytes; the terrain and the models use them differently.
- Instances: xn_vert_flags 0xD4AC0, 1024 elements of 12 bytes (only 3 used), parallel to xn_vert_cam.
- Users: models: xn_model_clear_vert_flags 140A28 (clears byte 0 of nverts elements; the loop is stopped by a planted ret), xn_model_transform_face_verts 1404FA, xn_poly_project_face 158452; terrain: xn_terrain_transform_grid 13EAB2/13EABC, xn_terrain_draw_cells 13F02C/13F06E/13F074, the clipped triangles 13EEA1, xn_terrain_add_nature_flats 13E6FA.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | done / terrain_flat | unsigned char (union) | 140505 test byte [edi+0D4AC0h],1; 140513 mov byte [edi+0D4AC0h],1; 140A28 mov [edi+100h],al (edi = 0D49C0h); terrain 13EABC mov [edi+0D4AC0h],al, 13F074 mov bl,[esi+0D4AC0h] | strong |
| +0x01 | terrain_outcode | unsigned char | 13EAB2 mov word [edi+0D4AC1h],cx; 13EEA1 mov al,[esi+0D4AC1h]; 13E6FA test byte [edi+0D4AC1h],10h | strong |
| +0x02 | outcode / terrain_tile | unsigned char (union) | 140594 mov [edi+0D4AC2h],cl (models' outcode); 158452 mov ch,[ebx+0D4AC2h]; terrain 13F06E mov dl,[esi+0D4AC2h] (written with +1 by 13EAB2) | strong |
| +0x03 | pad_03 | char[9] | never addressed | strong |

- Confidence: strong (asm); the terrain bytes' meanings are the world section's.

## struct xn_sort_pair (8 bytes)

- What: a {key, value} entry of a draw list sorted ascending by key.
- Instances: xn_model_queue 0x13F784 (200 pairs; key = |p| - radius, value = the model handle: xn_model_submit 1401D4), xn_flat_sort_list 0x153C44 (512 pairs; key = -z, value = the flat's record: xn_flat_add).
- Users: xn_render_sort_pairs 15810E / _range 15811C (quicksort), xn_render_draw_models 12A7D0 (12A7FD mov edi,[esi+4]), xn_render_draw_flats 12A814 (12A848 mov edi,[eax+4]), the writers 140274.., 154D7x.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | key | int | 15812A mov ebp,[eax+ebp]; 15812D cmp [esi+eax],ebp (signed: jge) | confirmed |
| +0x04 | value | void * | 15814E mov ecx,[esi+eax+4]; 12A7FD mov edi,[esi+4] (-> xn_model_draw); 12A848 (-> xn_flat_draw) | confirmed |

- Confidence: confirmed (asm: the sort swaps [x] and [x+4] together, 158145..158156).

## Globals typed by these structs

- `0x0CEA60 xn_render_span_next`: `struct xn_span *`
- `0x0CEA64 xn_render_poly_next`: `struct xn_poly *`
- `0x0CEAA5 xn_render_recip_table`: `int *  /* 65537 ints, [n] = 2^24 / n (12A214..12A228) */`
- `0x0CEAC0 xn_vert_cam`: `struct xn_vert_cam [1024]`
- `0x0D1AC0 xn_vert_screen`: `struct xn_vert_screen [1024]`
- `0x0D4AC0 xn_vert_flags`: `struct xn_vert_flags [1024]`
- `0x0DB320 xn_render_poly_pool`: `struct xn_poly [1000]`
- `0x0F39E0 xn_render_span_sentinel`: `struct xn_span`
- `0x0F39F0 xn_render_span_rows`: `struct xn_span [200 rows + the node pool]  /* to 0x116C80 */`
- `0x121EC0 xn_squares_table`: `int [8192]  /* [i] = (i - 4096)^2 << 8; _mid = +4096 */`
- `0x125EC0 xn_squares_table_mid`: `int [] (centre of xn_squares_table)`
- `0x129EC0 xn_tex_size_mask`: `unsigned char [256]  /* n - 1 for a power of 2, else FFh */`
- `0x129FC1 xn_render_mode_tables`: `void (**[3])(void)  /* by byte offset 0/4/8 */`
- `0x129FCD xn_render_mode0_setups`: `void (*[5])(void)`
- `0x129FE1 xn_render_mode4_setups`: `void (*[5])(void)`
- `0x129FF5 xn_render_mode8_setups`: `void (*[5])(void)  /* 12A700 15BB8C 12A860 12A860 12A740 */`
- `0x12A009 xn_render_span_setups`: `void (*[5])(void)  /* the active mode's; [4] = terrain */`
- `0x12A019 xn_render_span_setup_terrain_ptr`: `void (*)(void)  /* = xn_render_span_setups[4] */`
- `0x12A01D xn_render_solid_span_fns`: `void (*[3])(void)  /* by light index 0/4/8 */`
- `0x12A029 xn_render_tmap_span_fns`: `void (*[6])(void)  /* 8 px [3], then 16 px [3] */`
- `0x12A041 xn_render_terrain_span_fns`: `void (*[3])(void)`
- `0x13F784 xn_model_queue`: `struct xn_sort_pair [200]`
- `0x153C44 xn_flat_sort_list`: `struct xn_sort_pair [512]`
- `0x158200 xn_poly_clip_src`: `struct xn_poly_vertex *  /* struct xn_poly_clip_state at 0x158200 */`
- `0x15820C xn_poly_clip_intersect_fn`: `void (*)(void)`
- `0x158EA0 xn_poly_vertex_buf_a`: `struct xn_poly_vertex [32]`
- `0x1590A0 xn_poly_vertex_buf_b`: `struct xn_poly_vertex [32]`
- `0x159294 xn_poly_ring_a`: `struct xn_poly_vertex **[29]  /* by vertex count */`
- `0x1592FC xn_poly_ring_b`: `struct xn_poly_vertex **[29]  /* overlaps ring_a[26..28] */`
- `0x15B980 xn_span_dzdx`: `int  /* the new polygon's d(1/z)/dx for xn_span_insert */`
- `0x15B984 xn_poly_vertex_count`: `unsigned char`
- `0x147980 xn_recip16_table`: `int [1024]  /* [0] = FFFFh, [i] = FFFFh / i to 1022 */`
- `0x148980 xn_recip32_table`: `unsigned int [1024]  /* [i] = FFFFFFFFh / i */`
- `0x149980 xn_colour_fill_table`: `unsigned int [256]  /* i * 01010101h */`

## Notes across subsystems

- There is no edge buffer: xn_poly_rasterize 15B9A0 walks the left edge forward and the right
  edge backward through the vertex ring and keeps each edge's x, slope and 1/z step in its own
  patched immediates (15B9C3/15BA67 ring pointers, 15BA3A/15BA40/15BA46 steps, from
  xn_recip16_table and xn_recip32_table); a C version holds them in locals.
- The world section's struct xn_flat shares the pool: flats write +0 (image), +14h (flags),
  +3Ch, +40h (texture entry), +44h (scale|light), +50h..+58h (view position); the polygon view
  calls those fields face/handle/... The pick returns either (xn_flat_pick 155508 first).
- Model faces: +18h..+20h hold the face's first vertex in camera space until
  xn_poly_setup_textured turns +18h into the packed texture origin (15BC37): a C version must
  keep that order (the setup reads +18h/+1Ch/+20h before writing +18h).
- xn_light_setup_poly (15BC42) writes the model handle's +39h (|= 2) through the polygon: the
  model section's xn_model_handle.flags.
- Terrain polygons: +4 = 1 (13EC17), +14h = the terrain light list, +50h..+58h = the camera-space
  face normal (13EB21/13EC22) until xn_render_span_setup_terrain (12A740) replaces them with
  the 16-px steps; +18h/+1Ah and +24h..+38h come from the axis setters (13F4CC..13F608).

# Lights, shading, fog, the texture cache and the tmap pool

## struct xn_light (29 bytes)

- What: a light of the current frame: a torch, the player's light, a spell, the sun.
- Instances: xn_light_table 0x136540: 32 slots and a 33rd (0x1368E0) that reset never writes, whose intensity 0 ends the scans when 32 are in use; xn_light_next 0x1368FD, xn_light_count 0x13690D (at most 31 stored: the count is incremented first and must stay below 32).
- Users: xn_light_reset 136AB4 (every slot's intensity = -1), xn_light_add 136AD8 / xn_light_add_regs 136AF0 (the game: the player's light args.c (x, y, z, 16, 128, 0), objects and flats func_000830C7.c (31, 256), (image, 255), (64 -> clamped 32, ..), the sun sky.c (x, y, z, sun_light, 0, 8), the automap (.., 28, 0, 8)), xn_light_to_view 136BD8, xn_model_build_light_list 140606, xn_terrain_build_light_list 13E63C, xn_flat_span_light_setup 155610, and through the lists the dispatchers.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | x | int | 136B21 mov [edi],eax; 136C1F mov [esi],eax (view space << 8); 140657 mov eax,[esi]; 15565E (flats) | confirmed |
| +0x04 | y | int | 136B23 mov [edi+4],edx; 136C21 | confirmed |
| +0x08 | z | int | 136B26 mov [edi+8],ebx; 136C24 | confirmed |
| +0x0C | intensity | int (1..32; -1 free; 0 end) | 136ABF mov [edi+0Ch],-1 (reset); 136B1E mov [edi+0Ch],ecx (clamped to 20h at 136B11); 14064D cmp [esi+0Ch],0; jle (end of the scan); 1406B2; 13E67E; 155694 imul [edi+0Ch]; 1556DB | confirmed |
| +0x10 | reach | int (intensity/2 * radius) | 136B41 mov [edi+10h],ecx; 136B69 mov ecx,[edi+10h]; shl ecx,6 (cull sphere radius); records: 1024 = 8 * 128 | confirmed |
| +0x14 | range_sq | int (radius^2 * intensity/2) | 136B4B mov [edi+14h],ebp; 14068F sub ebp,[esi+14h]; 1406B8 (<< 4 into the list); 155685 cmp eax,[edi+14h]; records: 131072 | confirmed |
| +0x18 | type | int (0 point, 4, 8 directional) | 136B1B mov [edi+18h],esi; 136B29 cmp esi,8; 14065F cmp [esi+18h],8; 13E65B; 155654; 15BC80 mov eax,[eax+17h] after inc eax (the list entry's light + 18h) -> call [eax+158C0Ch] | confirmed |
| +0x1C | pad_1c | unsigned char | never addressed; the stride is 1Dh (136AC6 add edi,1Dh; 136C27; 1406C4) | confirmed |

- Confidence: confirmed (asm and records: a torch is x/y/z, 16, 1024, 131072, 0, 0).
- Open: Type 4 is ignored by both dispatchers (15C045 and 15C08B are a lone ret), yet the flat setup 155610 treats every type but 8 as a point light, so a type-4 light would light flats only. No caller passes 4: every xn_light_add in the game C (args.c, objlib.c, sky.c, automap.c, func_000830C7.c) passes type 0 or 8, so type 4 is dead.
- Open: The position is world units when added and view space << 8 after xn_light_to_view (from xn_render_draw_flats, after the models were drawn): a C port must keep that order or keep two copies.

## struct xn_light_ref (24 bytes)

- What: a light as it applies to one model (or to the terrain): the light and its position or direction in that model's space.
- Instances: lists in xn_render_light_list_pool 0x116C80 .. 0x119200 (400 entries) through xn_render_light_list_next 0xCEA68, each ended by a -1 dword; the terrain's (written first: 13E63C) and one per drawn model (140606). No overflow check.
- Users: xn_model_build_light_list 140606 (the model handle's +4 = the list), xn_terrain_build_light_list 13E63C (every terrain polygon's +14h), xn_light_setup_poly 15BC42 and xn_light_setup_terrain 15BC9C (walk them: 15BC7B, 15BCC7), the dispatch routines xn_light_add_point 15BF75, xn_light_add_directional 15C046, xn_light_terrain_add_directional 15C08C.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | light | struct xn_light * (-1 ends) | 1406A7 mov [edi],esi; 13E67C; 15BC7B mov eax,[esi]; inc eax; je end; 1406C9 stosd -1 | confirmed |
| +0x04 | x | int | 1406A9 mov [edi+4],eax (after xn_mat_transform_transposed with xn_model_rot_matrix); 13E673 (terrain: through xn_cam_rotation); 15BF80 imul eax,[esi+4]; 15C049; 15C08C | confirmed |
| +0x08 | y | int | 1406AC mov [edi+8],edx; 13E676; 15BF84 | confirmed |
| +0x0C | z | int | 1406AF mov [edi+0Ch],ebx; 13E679; 15BF88 | confirmed |
| +0x10 | intensity | int | 1406B5 mov [edi+10h],eax; 13E683 (negated); 15BFB0 imul eax,[esi+10h]; 15C065; 15C0A7 | confirmed |
| +0x14 | range_sq | int | 1406BE mov [edi+14h],eax (light +14h << 4); 15BF9F mov eax,[esi+14h]; cmp edx,eax | confirmed |

- Confidence: confirmed (asm and records: 116C84: light 136540, xyz -39936 -81408 -228352, 16, 2097152 = 131072 << 4; 116C80 = -1, the empty terrain list).

## struct xn_light_point_slots (64 bytes)

- What: the up to three point lights that reach the polygon being set up, as parallel arrays indexed together by ebp = 0, 4, 8.
- Instances: 0x158C28 (one overlay; names.csv names each array: xn_light_point_intensity, _falloff, _x, _y, _z, xn_light_shade_row).
- Users: xn_light_add_point 15BF75 (fills [ebp/4]), xn_light_setup_poly 15BC42 and xn_light_setup_terrain 15BC9C (shade_row), the directional adders 15C046/15C08C (shade_row += rows), the shader builders 15BCF6/15BD78/15BE4D, xn_light_shade_constant 15BCE8.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | intensity | int[3] | 15BFBF mov [ebp+158C28h],eax; 15BD2E | confirmed |
| +0x0C | falloff | int[3] | 15BFD5 mov [ebp+158C34h],eax (2^30/d^2); 15BD33 | confirmed |
| +0x18 | x | int[3] | 15C023 mov [ebp+158C40h],eax (view space >> 8); 15BCF7 | confirmed |
| +0x24 | y | int[3] | 15C029 mov [ebp+158C4Ch],edx; 15BCFC | confirmed |
| +0x30 | z | int[3] | 15C02F mov [ebp+158C58h],ebx; 15BD02 | confirmed |
| +0x3C | shade_row | unsigned char * | 15BC70 mov [158C64h],ebx; 15BCBD; 15C071 add [158C64h],eax; 15BCE8 mov eax,[158C64h] | confirmed |

- Confidence: confirmed (asm).

## struct xn_tex_block (22 bytes)

- What: the header of a block of the texture heap ('SET:' cache); a used block holds one loaded TEXTURE.nnn archive (struct xn_tex_archive) after the header.
- Instances: the heap: cfg_texture_memory KB at xn_tex_heap_base 0x1343DC (xn_tex_cache_init 136026); xn_tex_heap_head 0x1343E4 is a header of its own (flags 1, so never merged) whose next is the first block.
- Users: xn_tex_cache_init 136026, xn_tex_cache_flush 135E39, xn_tex_heap_alloc 1360EE, xn_tex_heap_evict 136145, xn_tex_heap_find_lru 136173, xn_tex_heap_alloc_first_fit 1361B8, xn_tex_heap_free 13622F, xn_tex_heap_sum_free 1363F8, xn_tex_load_archive 135EAB (slot), xn_tex_cache_lookup 135D00 (tick).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | next | struct xn_tex_block * | 1361F1 mov ecx,[edx]; 1361F5 mov [edx],ebx; 136184..1361AD walk | confirmed |
| +0x04 | prev | struct xn_tex_block * | 1361F7 mov [ebx+4],edx; 13625F mov ebx,[eax+4]; 135E64 mov [eax+4],1343E4h | confirmed |
| +0x08 | size | int (data bytes) | 1361D0 cmp [edx+8],eax; 136206 mov [ebx+8],ecx; 136237 | confirmed |
| +0x0C | flags | unsigned short (bit 0 used) | 136209 or word [edx+0Ch],1; 136232 and word [eax+0Ch],0FFFEh; 136184 test word [edx+0Ch],1 | confirmed |
| +0x0E | slot | struct xn_tex_archive ** | 135F2F mov [eax-8],edx (eax = data = block + 16h); 136159 mov ecx,[edx+0Eh]; mov [ecx],0; 136191 | confirmed |
| +0x12 | last_tick | unsigned int (BIOS ticks) | 135D5A mov [ecx-1Eh],eax (ecx = archive + 1Ah); 13618C cmp eax,[edx+12h] | confirmed |

- Confidence: confirmed (asm and a heap walk in a 135D00 record: 378038 -> 3914CA -> ... -> 475413 (free, 2546703 bytes), prev of the first = 1343E4).
- Open: xn_tex_heap_evict 136145 never fails (stc falls into clc), so 'SET: Out of memory in find_memory.' cannot print; a tiny cfg_texture_memory would show the flush-and-redraw path instead (docs/xngine_map.md).

## struct xn_tex_entry (20 bytes)

- What: a record's directory entry in a loaded archive (DFU TextureFile RecordDirectoryEntry), with the two null dwords used by the cache: what xn_tex_cache_lookup returns and a polygon's +40h holds until its setup.
- Instances: xn_tex_archive.entries[]; xn_tex_cache_lookup 135D00 returns archive + 1Ah + xn_tex_record_offsets[record] (= record * 20, 512 entries at 0x135402).
- Users: xn_tex_load_archive 135EAB (relocates +2, sets +10h), xn_tex_cache_lookup 135D00 (+2, +0Ch), xn_tex_cache_lookup_image 135DE4 (+2), xn_tmap_compile 15C274 (+2), xn_model_draw_faces 1404CE (+10h), xn_render_span_setup_solid 12A719 (+1), xn_poly_setup_textured 15BB90 (+0Ch), xn_render_span_setup_terrain 12A749 (+0Ch), the game (include/structs.h tex_cache_entry: +0x0C).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | type1_lo | unsigned char | DFU Type1 (low byte); not read by the engine | candidate |
| +0x01 | solid_colour | unsigned char | 12A719 mov bl,[esi+1] (esi = [poly+40h]) -> xn_colour_fill_table; 12A7B9; records: 6Ah, 45h (TEXTURE.004) | strong |
| +0x02 | image | struct xn_tex_image * | 135F5E add [eax+2],edx (offset -> pointer); 135D64 mov edx,[ecx+2]; 135E2E; 15C29E | confirmed |
| +0x06 | type2 | unsigned short | DFU Type2; not read by the engine | candidate |
| +0x08 | unknown_08 | unsigned short | DFU Unknown1 (low word); not read | candidate |
| +0x0A | blend_index | unsigned short | CDC99 mov word [eax+0Ah],1 (every entry of the archive, eax = archive + 1Ah + 14h k); 154E38 movzx ebx,word [eax+0Ah] (eax = xn_tex_cache_lookup's entry); 154E3C mov ebx,[ebx*4+136921h] | confirmed |
| +0x0C | current | struct xn_tex_image * | 135DD7 mov [ecx+0Ch],edx; 15BB95 mov esi,[esi+0Ch]; 12A749 mov ecx,[ebx+0Ch]; records | confirmed |
| +0x10 | kind | int (0, 4: compiled) | 135F4E mov [eax+10h],0; 135FB0 mov [eax+10h],4; 1404CE mov ebx,[eax+10h] -> xn_render_span_setups[ebx] | confirmed |

- Confidence: confirmed (asm and records: TEXTURE.004 entry 0: 6A00, image 408B61, 44EA, 0, current 408B61, kind 4).
- Conflicts with include/: none: include/structs.h struct tex_cache_entry (pad00[12], image at +0x0C) is this entry's first 16 bytes; its pad00 hides two fields the engine uses, +0x02 (the record header) and +0x0A (the flats' blend index).
- Open: +1 as the solid colour of a non-compiled texture (12A719 mov bl,[esi+1] with esi = the entry) is DFU's Type1 high byte: confirm by drawing a model face whose texture has a 0 pixel in render mode 8 (it is drawn as solid colour).

## struct xn_tex_archive (26 + 20n bytes)

- What: a TEXTURE.nnn file loaded whole into a heap block (DFU TextureFile: header, directory, records).
- Instances: xn_tex_archives 0x132F6C: 512 pointers by archive number (0 = not loaded; 135E76 clears 200h dwords; the 4 KB area to 0x133F6C has room for 1024); xn_tex_archive_use 0x133F6C: 512 words, lookups this frame (cleared by xn_tex_cache_begin_frame 135E90, which clears 100h dwords).
- Users: xn_tex_load_archive 135EAB, xn_tex_cache_lookup 135D00, xn_tex_cache_lookup_image 135DE4, xn_tex_heap_find_lru 136173.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | record_count | unsigned short | 135F48 movzx ecx,word [eax] (the directory loop count) | confirmed |
| +0x02 | name | char[24] | DFU Name; records show the names | confirmed |
| +0x1A | entries | struct xn_tex_entry[] | 135F4B add eax,1Ah; 135D55 add ecx,1Ah | confirmed |

- Confidence: confirmed (records: 'Desert Terrain Rain', 56 records; 'Mosque texs (Sentienel)', 5).

## struct xn_tex_image (28 + 4 x frames bytes)

- What: a record's image header (DFU TextureFile RecordHeader) as the cache rewrites it.
- Instances: in the loaded archive, at entry.image; a decoded animation frame lives in the decode buffer and is found through data_offset.
- Users: xn_tex_load_archive 135EAB (+0, +0Ah), xn_tex_check_transparent 135FCF (+4, +6, +8, +0Eh, +14h, +1Ch), xn_tex_cache_lookup 135D00 (+8, +0Ah, +0Eh, +14h, +16h, +1Ch), xn_tmap_compile 15C274 (+0Eh), xn_poly_setup_textured 15BB98 (+0, +0Ah, +0Eh), xn_flat_draw 154E20 (+4, +6, +0Eh, +18h), xn_flat_pick 155508, the game (texture_header).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | x / wrap_mask | short x, y (union: unsigned int wrap_mask) | 135F98 mov [ebx],edx (masks from xn_tex_size_mask); 15BB98 mov ebx,[esi]; records: 3FFF3FFF, 7FFF3FFF | confirmed |
| +0x02 | y | short | DFU OffsetY (inside wrap_mask) | strong |
| +0x04 | width | unsigned short | 135F85 movzx ecx,word [ebx+4]; 13600D | confirmed |
| +0x06 | height | unsigned short | 135F7B movzx ecx,word [ebx+6]; 135FD0 | confirmed |
| +0x08 | flags | unsigned short | 135F64 test word [ebx+8],1000h; 136005 and word [ebx+8],0FEFFh; 13601E or word [ebx+8],100h | confirmed |
| +0x0A | size / tmap | int (union: void (*)(void)) | 135FAA mov [ebx+0Ah],eax (xn_tmap_compile); 135DBD mov esi,[edx+0Ah]; 15BB9A mov eax,[esi+0Ah] | confirmed |
| +0x0E | data_offset | int | 135DD3 mov [edx+0Eh],eax (decoded frame - header); 15BB9D add esi,[esi+0Eh]; 15C2A6 | confirmed |
| +0x12 | is_normal | unsigned short | DFU IsNormal; not read | candidate |
| +0x14 | frame_count | unsigned short | 135D6F cmp word [edx+14h],1; 135D8B | confirmed |
| +0x16 | frame_time | unsigned short | 135D7F movzx ebx,word [edx+16h]; div (ticks) | confirmed |
| +0x18 | x_scale | short | DFU XScale; 154E4F add bp,[eax+18h] (xn_flat_draw adds it to the flat's scale) | strong |
| +0x1A | y_scale | short | DFU YScale | candidate |
| +0x1C | frame_offsets | int[] | 135D9E lea esi,[edx+1Ch]; mov esi,[esi+ebx*4]; lea esi,[esi+edx+1Ch] | confirmed |

- Confidence: confirmed.
- Conflicts with include/: none: include/structs.h struct texture_header has the same layout; its +0x0A comment already says the cache keeps a pointer there. The engine also overwrites x/y with the packed wrap masks (compiled records only) and rewrites data_offset per frame for animated records.

## struct xn_tex_frame (4 + data bytes)

- What: an animated record's RLE frame.
- Instances: in the loaded archive, at image + 1Ch + frame_offsets[i].
- Users: xn_tex_decode_frame 136282, xn_tex_decode_to_big_buffer 13641D.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | width | unsigned short | 136287 movzx eax,word [esi] | strong |
| +0x02 | height | unsigned short | 13628A movzx edx,word [esi+2] | strong |
| +0x04 | data | unsigned char[] | 1362A1 mov cl,[esi] (zero run); 1362B4 (copy count) | strong |

- Confidence: strong.

## struct xn_tex_unpack_strip (8 bytes)

- What: a strip of rows of the decode buffer, filled left to right with this frame's decoded animation frames.
- Instances: xn_tex_unpack_strips 0x1343FA, up to 256 (0x800 bytes), xn_tex_unpack_strip_count 0x134BFA; the buffer xn_tex_unpack_buffer 0x1343D0 (0xC0000 bytes).
- Users: xn_tex_unpack_alloc 1362CF, xn_tex_cache_begin_frame 135E90 (count = 0).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | width_left | unsigned short | 13630D cmp [edi],ax; 136323 sub [edi],ax; 136347 mov word [edi],100h | strong |
| +0x02 | start | unsigned char * | 136320 add ecx,[edi+2]; 136344 mov [edi+2],ecx | strong |
| +0x06 | height | unsigned short | 136312 cmp [edi+6],dx; 136340 mov [edi+6],dx | strong |

- Confidence: strong (asm).

## struct xn_tex_unpack_entry (8 bytes)

- What: a frame decoded this frame: its key and where its pixels are.
- Instances: xn_tex_unpack_entries 0x134C02, up to 256 (xn_tex_unpack_used 0x134BFE counts bytes, fatal past 0x800); then 0x135402 xn_tex_record_offsets.
- Users: xn_tex_unpack_alloc 1362CF, xn_tex_unpack_find 1363D9.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | key | unsigned int | 136393 mov [ecx+134C02h],ebp (ebp = (archive << 7 / record) << 16 / frame, 1362D9..1362EB); 1363DB cmp [ecx+134C02h],ebp | strong |
| +0x04 | pixels | unsigned char * | 136399 mov [ecx+134C06h],edi; 1363E3 | strong |

- Confidence: strong.

## struct xn_tmap_step (52 bytes)

- What: two pixels of the texture-mapper template: the patched operands are the fields.
- Instances: xn_tmap_template 0x15C300 (8 steps, then ret) and its copies.
- Users: xn_tmap_compile 15C274 (15C2A9..15C2B5: +7, +0Fh, +21h, +29h per step), xn_tmap_rebase 15C2DC (+0Fh, +29h).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | code_00 | unsigned char[7] | 15C300 bswap eax; mov ah,bh; mov edx,esi; and eax, | confirmed |
| +0x07 | wrap_mask_a | unsigned int | 15C2AC mov [ebx+7],edx (v mask << 8 / u mask) | confirmed |
| +0x0B | code_0b | unsigned char[4] | 15C30B add ebx,ecx; mov dl,[eax+ | confirmed |
| +0x0F | texels_a | unsigned char * | 15C2A9 mov [ebx+0Fh],eax (image + data_offset); 15C2E2 mov [eax+0Fh],edx | confirmed |
| +0x13 | code_13 | unsigned char[14] | 15C313..15C320 | confirmed |
| +0x21 | wrap_mask_b | unsigned int | 15C2B2 mov [ebx+21h],edx | confirmed |
| +0x25 | code_25 | unsigned char[4] | 15C325 | confirmed |
| +0x29 | texels_b | unsigned char * | 15C2AF mov [ebx+29h],eax; 15C2E5 | confirmed |
| +0x2D | code_2d | unsigned char[7] | 15C32D..15C333 | confirmed |

- Confidence: confirmed (asm: 15C306 and eax,imm32 / 15C30D mov dl,[eax+disp32] / 15C320 / 15C327, step 34h).

## struct xn_tmap_copy (418 bytes)

- What: a compiled texture-mapper copy for one texture: 16 pixels of affine mapping through the shade row, called by xn_span_tex_lit.
- Instances: xn_tmap_pool 0x15C150 (32-aligned in xn_tmap_pool_block 0x15C14C, allocated 0x4E620 = 768 x 418 + 32 by xn_tmap_pool_alloc 15C200), xn_tmap_pool_count 0x15C158 used (the 768th sets xn_tex_cache_full: 15C2D1); 0x15C154 holds pool + 0x10000 and is never read.
- Users: xn_tmap_compile 15C274 (15C277 imul edi,[15C158h],1A2h), xn_tmap_rebase 15C2DC, xn_tmap_pool_reset 15C2F0, xn_span_tex_lit 156A94 (calls [156A40h], planting ret at xn_tmap_ret_offsets[n]).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | steps | struct xn_tmap_step[8] | 15C2B5 add ebx,34h; 15C2B8 loop 8 | confirmed |
| +0x1A0 | tail | unsigned char[2] | 15C4A0 ret (C3h) and a 00h; 15C2C2 mov ecx,1A2h | confirmed |

- Confidence: confirmed.

## Globals typed by these structs

- `0x116C80 xn_render_light_list_pool`: `struct xn_light_ref [400]`
- `0x132F58 xn_tex_cache_full`: `unsigned char`
- `0x132F5C xn_tex_cur_archive`: `int`
- `0x132F60 xn_tex_cur_record`: `int`
- `0x132F64 xn_tex_cur_frame`: `int  /* -1: by the clock */`
- `0x132F6C xn_tex_archives`: `struct xn_tex_archive *[512]  /* 1024 slots of room */`
- `0x133F6C xn_tex_archive_use`: `unsigned short [512]`
- `0x13436C xn_tex_filename`: `char [12]  /* "TEXTURE.nnn" */`
- `0x134378 xn_tex_path`: `char [65]`
- `0x1343B9 xn_tex_file_size`: `int`
- `0x1343C0 xn_anim_ticks`: `unsigned int`
- `0x1343C4 xn_tex_heap_size`: `int`
- `0x1343D0 xn_tex_unpack_buffer`: `unsigned char *  /* 0xC0000 bytes */`
- `0x1343DC xn_tex_heap_base`: `struct xn_tex_block *`
- `0x1343E0 xn_tex_heap_free_bytes`: `int`
- `0x1343E4 xn_tex_heap_head`: `struct xn_tex_block  /* the list head */`
- `0x1343FA xn_tex_unpack_strips`: `struct xn_tex_unpack_strip [256]`
- `0x134BFA xn_tex_unpack_strip_count`: `int`
- `0x134BFE xn_tex_unpack_used`: `int  /* bytes of entries */`
- `0x134C02 xn_tex_unpack_entries`: `struct xn_tex_unpack_entry [256]`
- `0x135402 xn_tex_record_offsets`: `int [512]  /* record * 20 */`
- `0x136525 xn_shade_filename`: `char [10]  /* "shade.000" */`
- `0x13652F xn_light_filename`: `char [10]  /* "light.dat" */`
- `0x136540 xn_light_table`: `struct xn_light [33]`
- `0x1368FD xn_light_next`: `struct xn_light *`
- `0x136901 xn_light_falloff`: `unsigned short *  /* light.dat: 32768 words, 32-aligned */`
- `0x136905 xn_light_falloff_alloc`: `void *`
- `0x136909 xn_light_code_next`: `unsigned char *  /* shader code pool, from big_buffer */`
- `0x13690D xn_light_count`: `int`
- `0x136911 xn_light_ambient`: `int  /* shade bytes: row << 8, clamped to 3F00h */`
- `0x136915 xn_shade_table`: `unsigned char (*)[256]  /* 64 rows, 16 KB-aligned */`
- `0x136919 xn_shade_table_alloc`: `void *`
- `0x13691D xn_shade_table_last_row`: `unsigned char *  /* xn_shade_table + 3F00h */`
- `0x136921 xn_shade_blend_tables`: `unsigned char *[2]  /* new name; [1] = xn_shade_translucent_table */`
- `0x136925 xn_shade_translucent_table`: `unsigned char *  /* 16 rows of 256 (CDC4B) */`
- `0x14CC00 xn_haze_filename`: `char [12]`
- `0x14CC0C xn_fog_table_last`: `unsigned char *  /* the 64-level fog table's last row */`
- `0x14CC10 xn_fog_start`: `int  /* view z >> 8 */`
- `0x14CC14 xn_fog_step`: `int  /* 3F00h / (far >> 8 - start) */`
- `0x14CC18 xn_fog_min_inv_z`: `int  /* 2^40 / far */`
- `0x14CC1C xn_fog_unroll_offsets`: `unsigned short [641]  /* 18 n */`
- `0x156A00 xn_tmap_ret_offsets`: `int [16]  /* 0, 28, 52, 80... */`
- `0x158C00 xn_light_terrain_dispatch`: `void (*[3])(void)  /* by type 0/4/8 */`
- `0x158C0C xn_light_dispatch`: `void (*[3])(void)`
- `0x158C18 xn_light_shader_builders`: `void (*[4])(void)  /* by point lights x 4 */`
- `0x158C28 xn_light_point_intensity`: `int [3]  /* struct xn_light_point_slots at 0x158C28 */`
- `0x15C14C xn_tmap_pool_block`: `void *`
- `0x15C150 xn_tmap_pool`: `struct xn_tmap_copy *`
- `0x15C158 xn_tmap_pool_count`: `int`

## Notes across subsystems

- Flats (world section, 155610) read lights directly from xn_light_table in view space (after
  xn_light_to_view): +0..+8, +0Ch intensity (directional: intensity << 8 shade rows), +14h
  range_sq (against the squared distance >> 16), +18h type (8 directional; every other type,
  4 included, is a point light for flats).
- The polygon's +08h..+10h are written by the shader builders (shader_falloff).
- The texture lookup is also the source of a polygon's +40h, +44h, +4Ch (xn_poly_setup_textured
  15BB8C) and of the flat sizes (xn_tex_cache_lookup_image 135DE4: width/height/+0Ah).

# ARCH3D models at run time, model drawing, collision

Sources: work/listing.txt (xn_13FE00, xn_C7F00 model helpers, xn_CE300 CE808/CE828, xn_14A300,
xn_14BC00, xn_15CB00), records read with recmem (xn_model_draw 140284: 3 records, xn_model_submit
1401D4: 6, xn_collide_spheres_model 14AA92: 1), a static survey of ARCH3D.BSA
(build/emu/overlay/ARENA2, 10251 records: header fields, face bytes, sphere lists), DFU
Arch3dFile.cs (fetched from github: FileHeader, PlaneHeader, PlanePoint, version handling, UV
unpack, PlaneData 24 bytes, ObjectData commented out), the game C (src/lifted args.c
object_draw_cb, automap.c, bank.c, colstuff.c; src/hand func_000830C7, func_000234EB,
func_0002257C, func_00036233). Units: a "world unit" is the game's coordinate unit; model
coordinates are 1/256 of it (24.8).

Structs: xn_model, xn_model_frame, xn_model_face_data, xn_model_face_point, xn_model_face,
xn_model_sphere_face, xn_model_sphere, xn_model_handle, xn_model_matrix_slot,
xn_model_matrix_pool, xn_model_draw_state (locals, not memory), xn_collide_probe_sphere,
xn_collide_probe, xn_collide_hit, xn_collide_hits, xn_collide_scratch, xn_collide_seg_state,
xn_collide_sph_state, xn_scratch.

## struct xn_model (64 bytes)
- What: the header of an ARCH3D.BSA record (DFU FileHeader) as the engine uses it after
  xn_model_prepare 0x13FE15. All lists stay offsets from the header (the engine adds the model
  address at every use); prepare rewrites data in place (normals, radius, face u/v, face data).
- Instances: one heap copy per model id + variant in the game's model cache (model_get /
  model_cache_add calls prepare); the handle's +0x00. Record layout in memory: header, points
  (+0x40), faces, normals, face data, spheres (10244 of 10251 records in this order).
- Users: xn_model_prepare 13FE15 and its helpers 13FF0E, 13FF65, 140790, 1407DE, 14081C,
  140845; xn_model_cull_and_queue 1401E0 (radius); xn_model_draw 140284 / xn_model_draw_faces
  1403AF / xn_model_set_frame_regs 14037A / xn_model_build_light_list 140606 /
  xn_model_is_occluded 140910 / xn_model_project_bounds 140981; collision 14A300, 14AA92,
  dead 14A6C0, 14B58E; xn_model_max_y CE808 (objlib.c), dead CE828, C80CC, 1408D3.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | version | unsigned int | 13FE16 `cmp dword ptr [eax], 0x362e3276` ("v2.6"): below it, 13FE3E..13FE43 multiply every face point's vertex offset by 3 and 13FE1E rejects >= 1024 vertices. ARCH3D.BSA: 10109 "v2.7", 134 "v2.6", 8 "v2.5"; DFU: v2.5 offsets * 3 | confirmed |
| 0x04 | point_count | int | 13FE1E, 13FE8F (validation bound), 1403AF (vertex flags to clear), 1407E1, 0C80ED; record (model 634): 0x203 = (face_offset - point_offset) / 12 | confirmed |
| 0x08 | face_count | int | loop counts 13FE2E 13FF3F 1403EE 140791 140825; 140911 `cmp [esi+8], 4` (no occlusion test for <= 4 faces); 1403F1 adds it to xn_render_poly_count; record 0x12E | confirmed |
| 0x0C | radius | int | 140817 writes max \|point\| (1407DE, isqrt64), equal to the file value (record 0xA3CB3); 140217 (cull), 140628 (light range), 1409B8 (bounds), 14A32D/14AAD4 `shr 8` (bounding test in world units) | confirmed |
| 0x10 | frame_count | int | 13FF14/13FF5E (uv axes for every frame), 140339 (frame selection when nonzero); 0 in all 10251 records (DFU NullValue1) | strong |
| 0x14 | frame_table_offset | int | 13FF1F `add eax, [esi+0x14]` (frame << 4), 140390 | strong |
| 0x18 | face_data_offset | int | 13FF32/1403A4 (from the frame), 13FF35, 14079A; record 0x5FF4 = normals + 12 * faces; DFU PlaneDataOffset | confirmed |
| 0x1C | sphere_offset | int | 14A448, 14ACA8; record 0x7C44; DFU ObjectDataOffset | confirmed |
| 0x20 | sphere_count | int | 14A44D, 14ACAD; record 10; DFU ObjectDataCount | confirmed |
| 0x24 | unknown_24 | int | DFU Unknown2 (674 distinct values); no read in object 2 | candidate |
| 0x28 | pad_28 | char[8] | DFU NullValue2; 0 in all records | confirmed |
| 0x30 | point_offset | int | 13FE95 13FF67 1403D3 140794 1407E4 140854 14A3EA 14AC4A 0C80D2 CE80D; frame copies 13FF26 140396; record 0x40 | confirmed |
| 0x34 | normal_offset | int | 1403EB 140797 140822 14A3F5 14AC55; frame copies 13FF2C 14039D; record 0x51CC | confirmed |
| 0x38 | unknown_38 | int | DFU Unknown3 (0 in 10232 records); no read | candidate |
| 0x3C | face_offset | int | 13FE31 13FE73 13FE92 13FF3A 140401 14079D 14081F; record 0x1864 = 0x40 + 12 * 515 | confirmed |

- Confidence: confirmed (asm + record model 634 + the file agree).
- Against include/structs.h arch3d_header: same offsets and sizes; the engine's reading differs
  in: +0x10/+0x14 (null_value1) are its animation frame count and frame table (unused by the
  data); the radius is the file's own unit (24.8, the same scale as the points), not shifted:
  "radius<<8" in the notes means "in model units"; no offset is turned into a pointer in the
  header. In place, prepare: recomputes the normals (8-bit fraction, 1.0 = 256, from the first
  three points: 140845; the file's values are the same within rounding: file (-181, 0, 181),
  memory (-182, 0, 181)), recomputes the face data (texture axes) and radius, shifts point 0's
  u/v dword left 4, and overwrites points 1 and 2's u/v (see xn_model_face_point).
- Validation (13FE6F..13FEC2): a face with more than 24 points or a vertex offset that is not
  a multiple of 12 below point_count shuts the engine down (kbd, joystick, render, video,
  memory) and prints xn_model_msg_too_many_verts / _corrupted (">24 points per face" prints
  the "too many vertices" text). v2.6+ files are not checked against the 1024-entry vertex
  arrays (xn_vert_*); the largest ARCH3D.BSA model has 1010 points.
- Open: none for the layout. xn_model_set_frame_regs 14037A never ran (no animated model in
  the data); a hand-made model with frame_count 2 would exercise it.

## struct xn_model_frame (16 bytes)
- What: one animation frame: offsets of its own point list, normal list and face data.
- Instances: model + frame_table_offset, frame_count entries. None in Daggerfall's data.
- Users: xn_model_calc_uv_axes 13FF0E (each frame at prepare), xn_model_set_frame_regs 14037A
  (clamps the handle's frame to 0..frame_count-1, copies, and recomputes the face planes with
  140790), dead xn_model_set_frame 140369.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | point_offset | int | 13FF24 / 140393 -> model +0x30 | strong |
| 0x04 | normal_offset | int | 13FF29 / 140399 -> model +0x34 | strong |
| 0x08 | face_data_offset | int | 13FF2F / 1403A0 -> model +0x18 | strong |
| 0x0C | pad_0c | int | never read (stride 16: 13FF1C `shl eax, 4`, 14038D) | strong |

- Confidence strong (asm only; no record, no data). Note: only the normals of the last frame
  are recomputed at prepare (14081C uses the header's normal_offset after 13FF0E's loop leaves
  the last frame selected), and the face planes are recomputed per frame at draw time.

## struct xn_model_face_data (24 bytes)
- What: a face's texture axes (DFU "PlaneData"): object-space gradient vectors of u and v.
- Instances: model + face_data_offset, one per face (face i at + 24 i); each face's point 2
  holds a pointer to its own (xn_model_face_point.data).
- Users: written by xn_model_calc_face_uv_axes 13FF65 (faces with texture >= 0x100, every
  frame); read by xn_poly_tex_gradients 15BAC0 (from xn_poly_setup_textured 15BBCD: edx =
  face->points[2].data, ecx = handle->matrix) into poly +0x24..+0x38, +0x50, +0x54.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | u_axis | xn_vec3 | 140145/14015C/14016F write `[ebp]`, `[ebp+4]`, `[ebp+8]` (ebp = the pushed edi = face data, 140110); 15BAC0 `mov eax,[edx]`, 15BACD, 15BAD0 | confirmed |
| 0x0C | v_axis | xn_vec3 | 14019E/1401B5/1401C8; 15BB27/15BB2A/15BB37 | confirmed |

- Computation (13FF65): e1 = p1 - p0, e2 = p2 - p1; t = (e1.e2 << 8) / (\|e1\|^2 >> 8) (16.16,
  0x1202B0); e2' = e2 - t e1; e1n = (e1 << 25) / (\|e1\|^2 >> 8), e2n likewise; u_axis = du1 e1n +
  (du2 - du1 t) e2n with du1, du2 = points 1, 2's u words (the file's deltas); v the same.
  So u_axis = 2^33 * grad(u) in model units. Record (model 634, faces 0-2): the memory values
  equal the file's PlaneData, so the file stores the same axes.
- Confidence: confirmed.

## struct xn_model_face_point (8 bytes)
- What: a point of a face: the vertex and a second dword that holds the file's u/v and, after
  prepare, three different things in points 0, 1, 2 (an anonymous union; every face has >= 3
  points).
- Instances: face +0x08, point_count of them.
- Users: 13FE3E (v2.5 *3), 13FEA8 (validation), 13FF70.. (uv axes), 140790, 140845,
  1404FA, 158420, 15CB00, 15CE7E, 15BC2D, 15BBCD, 15BF92, 14041F, 14A592/14ADEA (point 0's
  vertex as the plane's point).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | vertex | int | byte offset into the point list (index * 12) and into xn_vert_cam/_screen/_flags (1404FA `mov edi,[esi+8]` then `[edi+0xceac0]`); 13FEA8 `div 12`; record 0xC0, 0x90, 0xD8 | confirmed |
| 0x04 | uv | short[2] | the file's u, v; points 1, 2 are deltas from the previous point (DFU); 140119 `movsx eax, word ptr [edi+4]`, 140172 `[edi+6]` read points 1, 2 at prepare | strong |
| 0x04 | uv_packed | unsigned int | point 0: 13FE78 `shl dword ptr [esi+0xc], 4` (the whole dword: u's top 4 bits carry into v); 15BC2D `mov edx,[edx+0xc]` minus the gradient origin -> poly +0x18. Record: file (u 0, v 2048) -> 0x80000000; (5124, 4096) -> 0x00014040 | confirmed |
| 0x04 | plane_d | int | point 1: 1407C6 `mov [esi+0x14], eax` = dot(vertex of point 0, normal) (model units * 1/256); read 14041F (back-face), 15BF92 (point light distance to the plane); record 35586048 | confirmed |
| 0x04 | data | struct xn_model_face_data * | point 2: 1407CA `mov [esi+0x1c], ebp` (absolute pointer); 15BBCD; record 0x839F38 = model + 0x5FF4 | confirmed |

- Conflict with structs.h arch3d_plane_point: none in layout; after prepare the u/v of points
  0..2 are no longer the file's (the game reads only `texture` and the offsets: click_face_
  texture, footsteps).
- Open: the u/v units (12.4 texels?) and why the dword shift is acceptable (u >= 4096 leaks
  into v; 5124 occurs). Settle: compare a rendered face's texture origin with DFU's UVunpack.

## struct xn_model_face (8 + 8 * point_count bytes)
- What: a face (DFU PlaneHeader + points; the game's struct arch3d_plane).
- Instances: one after the other at model + face_offset (next = + 8 + 8 * point_count).
  Face i's normal is normals[i], its face data face_data[i]. Identified game-side and in the
  collision lists by its offset from the model's start.
- Users: everything in xn_13FE00; poly +0x00 points at it (14048F `mov [edi], esi`);
  xn_poly_project_face 158420; xn_poly_setup_textured 15BB8C; xn_light_setup_poly 15BC42;
  xn_light_add_point 15BF8E; collision 14A592, 14AE56, 15CB00, 15CE7E; the game (pick, collide).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | point_count | unsigned char | 13FE38, 13FE7C `cmp eax, 0x18` (max 24), 1404DF, 1407D0, 158420, 15CB1E (edge table index), 15CE87; ARCH3D.BSA: 3..24 | confirmed |
| 0x01 | shade | unsigned char | 15BC5C `mov bh, byte ptr [eax+1]` (eax = poly +0 = face): + 256 * shade to the shade-row base, >= xn_shade_table_last_row gives up; DFU Unknown1: 0 in 201212 of 204479 faces, else 16, 8, 4, 9, 30, 10, 12, 35, 6... | strong |
| 0x02 | texture | unsigned short | 13FF42 `cmp word ptr [ebx+2], 0x100` (no axes below); 1404B6 -> xn_tex_cache_lookup(ax >> 7, ax & 0x7F); record 0x303; 13156 faces < 0x100 | confirmed |
| 0x04 | unknown_04 | int | DFU PlaneHeader Unknown2 (the game's floor_sound low byte: 0 except 96 x 1, 94 x 2); no engine read | candidate |
| 0x08 | points | struct xn_model_face_point[] | 13FE3B, 140848, 158423 `add esi, 8`; stride 8 | confirmed |

- Conflict with structs.h arch3d_plane: `unknown1` is the face's shade offset (15BC5C); the rest
  agrees.

## struct xn_model_sphere_face (6 bytes) and struct xn_model_sphere (0x12 + 6n bytes)
- What: the collision spheres (DFU "ObjectData", which DFU does not decode): a sphere in model
  space and the faces it contains, sorted by face offset descending.
- Instances: model + sphere_offset, sphere_count spheres one after the other (next = + 0x12 + 6
  * face_count).
- Users: xn_collide_segment_model 14A300 (sphere vs segment 14C7E5, then merges the lists of
  the hit spheres), xn_collide_spheres_model 14AA92 (vs the probe's spheres 14CA44), dead
  builder 14B58E (writes them).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| sphere 0x00 | x, y, z | int | 14A459/14A45B/14A45E -> 14C7E5; 14ACCF.. via 14CA44 (esi); record (-114688, -489387, -114688) | strong |
| sphere 0x0C | radius | int | 14A461, 14ACB8; record 241357 | strong |
| sphere 0x10 | face_count | unsigned short | 14A478 `movzx edi, word ptr [esi+0x10]`, `lea esi,[esi+edi*6+0x12]`; 14ACEB, 14AD14 | confirmed |
| sphere 0x12 | faces | struct xn_model_sphere_face[] | 14A4B4, 14AD28 `add esi, 0x12`; stride 6 | confirmed |
| face 0x00 | face | int | 14A584 `add edx, dword ptr [ecx]` (model + it = the face), 14A654 -> hit +0x0C; merge 14A4C3 `cmp eax,[esi]` / `jl`: descending; ARCH3D.BSA: 669678 of 698716 entries are face offsets from the model start (the rest in a few odd records) | confirmed |
| face 0x04 | normal4 | unsigned short | 14A577 `movzx ebx, word ptr [ecx+4]` * 3 + normals; 14ADF3; ARCH3D.BSA: = face index * 4 in all but 23 entries | confirmed |

- Confidence: confirmed (asm + file survey + record model 634: sphere 0 count 8, first entry
  {0x3984, 0x2D4}).
- Open: the 29038 entries whose face offset is not a face start (sort order is 605351
  descending pairs vs 93365 others incl. firsts): probably a few malformed records; a check per
  record id would tell which models.

## struct xn_model_handle (58 bytes)
- What: a model instance as the engine sees it: the model, this frame's light list and matrix,
  the placement's base angles, the eye-relative and world position, the draw angles, frame,
  flags.
- Instances: type-6 and type-32 object data (record +0x47; object_create_child gives 62 bytes),
  D_001A945E (the arrow in flight; 58 bytes fit before D_001A949C), &block_model.model
  (block_model +0x04: RMB models of type-43 blocks and type-56 objects, bank_draw_preview's
  ship). Queue entries point at handles; poly +0x04 and the pick hit's +0x04 are handles.
- Users: game: object_draw_cb / func_000830C7 (fill model, x, y, z, angles; submit),
  automap_draw_object_cb, bank_draw_preview, colstuff.c (collision), loadsave/objlib (model).
  Engine: xn_model_submit 1401D4 / xn_model_cull_and_queue 1401E0 (frame, flags, rel), 140284
  (rel in object axes, matrix), 140606 (lights), 140910/140981 (position), C7F07/C7F14/C7F98
  (angles), 15BC42 (flags bit 1, lights), 15BF75/15C000 (rel, matrix + 0x1C20), 15BBCA
  (matrix), C810C (angles, through poly +4), 14A300/14AA92 (model, position, angles).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | model | struct xn_model * | 140284 `mov esi,[edi]`, 140215, 14A32B, 14AAD0; game model_get; record 0x833F44 ("v2.6") | confirmed |
| 0x04 | lights | struct xn_light_ref * | 140648 `mov [edi+4], eax` (xn_render_light_list_next); 15BC78 `mov esi,[esi+4]`; record 0x116C84: {0x136540, ...}, then -1 | confirmed |
| 0x08 | matrix | struct xn_model_matrix_slot * | 140330 `mov [edi+8], ebx`; 15BBD0 `mov ecx,[ecx+8]`; 15C00C (+0x1C20); record 0xD7AC0, 0xD7B08 | confirmed |
| 0x0C | pad_0c | short | no instruction addresses it; C7F07/C7F14/C7F98 get handle + 0x0C (records.h `angles`) | candidate |
| 0x0E | base_angle_x | short | C7F07 `mov word ptr [ecx+2], ax`; C7F22 `movzx eax, word ptr [eax+2]` (OR xn_model_angle_or_bits), C7FA4, C7F88 (movsx in the fallback) | strong |
| 0x10 | base_yaw | short | C7F0B, C7F1E, C7FA0 (+ the yaw offset in C7F98); records C7F07 / C7F98 | confirmed |
| 0x12 | base_angle_z | short | C7F0F, C7F1A, C7F9C | strong |
| 0x14 | rel_x | int | 14020C `mov [edi+0x14], eax` = (x - xn_cam_x) << 8; 1402D8 rewrites it in object axes (137514 by xn_model_rot_matrix); 15C003; record: entry 0xFFFC8400, after 0x9C00 = patch 0x14040B | confirmed |
| 0x18 | rel_y | int | 14020F, 1402DB, 15C006 | confirmed |
| 0x1C | rel_z | int | 140212, 1402DE, 15C009 | confirmed |
| 0x20 | x | int | 1401E8, 14060C, 140982, 14A322, 14A380, 14A622, 14AB72; game instance->x = object->x; record 0x00C6C480 | confirmed |
| 0x24 | y | int | 1401EB, 14060F, 140985, 14A325; record -726 | confirmed |
| 0x28 | z | int | 1401EE, 140612, 140988, 14A328 | confirmed |
| 0x2C | angle_x | int | 14029D -> 137000 eax (pitch), 14A35E, 14AB42, C8112; C7F70/C7F8D/C7FAB write it (ecx + 0x20) | confirmed |
| 0x30 | yaw | int | 1402A0, 14A361, 14AB45; C7F73/C7FAE; block_model +0x34; record 0x200, 0x240 | confirmed |
| 0x34 | angle_z | int | 1402A3, 14A364, 14AB48; C7F76/C7FB1 | confirmed |
| 0x38 | frame | unsigned char | 1401E1 `mov byte ptr [edi+0x38], al` (submit's edx: always 0 from the game); 140340 -> 14037A | strong |
| 0x39 | flags | unsigned char | 1401E4 `mov byte ptr [edi+0x39], 0`; 140363 `or [edi+0x39], 1` (occluded); 15BC48 `or byte ptr [eax+0x39], 2` (a polygon reached the rasterizer); record entry 0x02 then cleared | confirmed |

- Confidence: confirmed for the engine's fields; base angles strong to confirmed (record of
  C7F07: dx 0x201 with ecx = handle + 0x0C lands at handle +0x10; record of C7F98: handle +0x10
  = 0xE400 copied to +0x30: the angles are not reduced mod 2048, xn_mat_from_angles masks them).
  Records also show +0x04/+0x08 holding junk (0x82F47D88) in an object never drawn: they are
  valid only after a draw in the current frame.
- Conflicts with include/records.h model_instance (56 bytes): (1) the engine writes +0x38
  (frame) and +0x39 (flags), so the struct is 58 bytes (the object data is 62); (2) `pad04[8]`
  is the light-list and matrix pointers the engine writes every drawn frame; (3) `angles[20]`
  at +0x0C is pad (2), base angles x/yaw/z (3 shorts), then the engine's rel_x/y/z (12 bytes of
  per-frame scratch); C7F07/C7F14/C7F98 take its address; (4) `missile_angles[3]` at +0x2C are
  the draw angles of every model (pitch, yaw, roll for xn_mat_from_angles), which C7F14/C7F98
  fill for ordinary objects; arrows put missile_yaw in the pitch slot (the arrow model's axis).
  For block_model, the handle overlays DFU Unknown2/Unknown3 (+0x04/+0x08 -> block +0x08/+0x0C)
  and XPos1..ZPos1 (+0x14..+0x1C -> block +0x18..+0x23), which the engine overwrites each frame;
  frame/flags land on block +0x3C/+0x3D (DFU Unknown5 area).
- Open: who reads flags bit 1 (no reader in object 2 or src/; maybe none). Experiment: watch
  data+0x39 of a type-6 object across a frame where it is visible / hidden.

## The model queue (globals; struct xn_sort_pair is the render section's)
- xn_model_queue 0x13F784: struct xn_sort_pair[200] {int key; void *value = the handle};
  0x13F784..0x13FDC3. key = isqrt((a.x>>8)^2 + (a.y>>8)^2 + (a.z>>8)^2) - (radius >> 8) with a =
  xn_cam_cull_sphere's view-space centre (x, y scaled by 2 * inv_scale), 0 when that is negative
  or the centre is behind the eye (a.z < 0) (1401E0..14027E).
- xn_model_queue_count 0x13F76C: int, incremented for every model that passes the cull *before*
  the bound check (`inc; cmp 0xC8; jae`), so at most 199 entries are stored while the count
  keeps counting; world_render reads it. xn_model_queue_ptr 0x13F780: struct xn_sort_pair *,
  the next free entry. xn_model_drawn_count 0x13F770: int, models drawn (not occluded).
  All reset by xn_render_begin_frame 12A50F/12A572/12A57C.
- xn_render_draw_models 12A7D0 sorts the pairs (15810E) and calls 140284 nearest first; it stops
  at the first model whose texture lookup fails (CF).

## struct xn_model_matrix_slot (36 bytes) and struct xn_model_matrix_pool (14400 bytes)
- What: per drawn model, the object-to-camera rotation, later rescaled for the texture gradients,
  and its light-setup version 0x1C20 bytes (200 slots) on.
- Instances: xn_render_matrix_pool 0xD7AC0: view[200] then light[200] (0xD96E0), ends 0xDB300
  (xn_render_poly_pool 0xDB320 follows). xn_render_matrix_next 0xCEA70 (+0x24 per drawn model).
- Users: 140284 (1402BA-1402C0: view = xn_cam_view_matrix * xn_model_rot_matrix; row 0 for
  the 1/z gradient factor 140303..140328), 1404FA (vertex transform via patch 0x14053E),
  1406D6 (rescale + copy), 15BAC0 (texture gradients: view), 15BF75 (point lights: light).
- What each holds (record 140284, slot 0xD7AC0, focal 200/180): while 1403AF transforms the
  vertices the slot is the 2.28 rotation; after 1406D6: light[i] rows 0, 1 = (m * inv_scale_x/y)
  >> 31 (length ~2^28 at scale 1), row 2 = m; view[i] row 0 = light row 0 * 2^36/focal_x >> 32
  (ratio 12.5), row 1 = light row 1 * 2^33/focal_y >> 32 (ratio 90), row 2 = m * 2. So view[]
  maps object-space vectors to screen-space gradients (x with 3 more fraction bits than y,
  matching xn_vert_screen), light[] to view space at the view's aspect.
- Confidence: confirmed (asm and the record's values).
- Why x and y differ by 8 (2^36 vs 2^33), settled by the render section: row 0 gives the
  8-pixel x step directly (xn_poly_tex_gradients 15BAEC stores it as poly +50h, the span
  routines' 8-px u step, and 15BAEF/15BAF2 store it >> 3 as the per-pixel +24h); row 1 is the
  per-row y gradient (+28h), used as is.

## struct xn_model_draw_state (not in memory: the patched immediates of xn_model_draw)
| patch site(s) | field | written by | read by |
|---|---|---|---|
| 0x140497 | handle | 140297 | 140494 (poly +0x04), 140606 |
| 0x14040B/412/419 and 0x14052D/533/539 | rel (handle rel in object axes) | 1402E1..1402FD | 140409 back-face test `n.rel + plane_d >= 0` -> skip; 14052C added to each vertex before the matrix |
| 0x14053E | matrix | 140333 | 14053D `mov ecx, imm` -> 13749E |
| 0x140446/44D/454 | dz_row = high dword of slot row 0 * [0x14030A] | 140312..140328 | 140444..140451: n.dz_row << 13 / (n.rel + d) -> poly +0x58, >> 3 -> poly +0x5C and xn_span_dzdx |
| 0x14030A | k = 2^48 / (xn_cam_scale_x * focal_x) | 12A41A (view setup) | 140309 |
| 0x14051C/522/528 | points (+0, +4, +8) | 1403D6..1403E6 | 14051A..140526 |
| 0x1404EB | normals_end | 140404 | 1404E9 loop end |
| 0x140A28 + 6 * point_count | a planted `ret` (0xC3), then 0x88 back | 1403BE / 1403CA | xn_model_clear_vert_flags (unrolled `mov [edi+disp], al`, edi = 0xD49C0, disp 0x100 + 12 i) |
| 0x1405B4/1405BC/1405CC/1405D4 | half width, (centre_x << 8) + 0x80, half height, (centre_y << 8) + 0x80 | xn_cam_set_view_window 12A309..12A385 | projection in 1404FA |
| 0x140666/66C/672 | world (handle x, y, z) | 140615..140620 | 140665 light - model |
| 0x14068B | radius_sq = (r^2 + 0x8000) >> 16 | 140639 | 140689 range test `d^2 - radius_sq - light +0x14 < 0` |
| 0x1406DD/1406E2, 0x14072A/14072F | xn_cam_inv_scale_x, 2^36/focal_x; xn_cam_inv_scale_y, 2^33/focal_y | 12A3D3, 12A2B1, 12A3FF, 12A2C3 | 1406D6 |
The struct in model.h gathers the per-model ones (handle, rel, matrix, dz_row, points,
normals_end, world, radius_sq) as the locals a readable xn_model_draw would pass around; the
view constants belong to the camera.

## struct xn_collide_probe (0x1A + 16 n) and struct xn_collide_probe_sphere (16 bytes)
- What: the mover's shape: a position, a rotation, spheres around it (the game's collide_probe /
  probe_sphere).
- Instances: game statics D_00187B44 (creatures), D_00187B6E, D_00187BB8, D_00187C12,
  D_00187C3C (player), D_00179F48; D_00196D4C points at the one under test.
- Users: xn_collide_spheres_model 14AA92 (edx), dead 14AF30 (eax), 14B017.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | position | xn_vec3 | 14AAB3..14AAB8 (local_pos), 14AB70..14AB7E (minus the handle's position, << 8, into object space) | confirmed |
| 0x0C | angle_x, yaw, angle_z | int x 3 | 14ABAB/14ABAE/14ABB1 -> 137000 into xn_collide_matrix_b; dead 14AF50..14AF56 | strong |
| 0x18 | sphere_count | unsigned short | 14AAA7 `movzx ebp, word ptr [edx+0x18]`; 14AFD6 | confirmed |
| 0x1A | spheres | struct xn_collide_probe_sphere[] | 14AACD `add edx, 0x1a`, stride 0x10 (14AB28) | confirmed |
| s 0x00 | x, y, z | int | offsets from the position in the probe's axes: 14AAEF.. (bounding test unrotated), 14ABFB.. (<< 8, rel_matrix, + local_end) | confirmed |
| s 0x0C | radius | int | 14AAEC (world units), 14AC30 `shl eax, 8` | confirmed |

- Conflict with structs.h collide_probe: `pad0C[12]` is the probe's rotation (pitch, yaw, roll),
  read every call; the game leaves it 0 (statics), so the spheres are used unrotated.

## struct xn_collide_hit (30 bytes) and struct xn_collide_hits
- What: the result list of 14A300 / 14AA92 at big_buffer: count, then 30-byte hits.
- Users: written by 14A300 (14A5F2..14A670) and 14AA92 (14AE9F..14AEE7); read by colstuff.c
  (hits[0].face, hits[j].y, count), func_000234EB / func_0002257C (nx, ny, nz).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x00 | x, y, z | int | 14A62B/14A62D/14A630: model_matrix * (crossing point in object space), (+0x80) >> 8, + handle x/y/z: world units. 14AA92 does not write them (stale big_buffer) | confirmed |
| 0x0C | face | int | 14A656 / 14AEA7 from the sphere list entry: offset from the model start | confirmed |
| 0x10 | nx, ny, nz | int | 14A64A.. / 14AED1..: the model normal (8-bit fraction, unshifted) through the model matrix: world axes, 1.0 = 256 | confirmed |
| 0x1C | t_half | short | 14A65E `sar eax, 1` / 14A660 `mov word ptr [esi+0x1c], ax` (t from 14C38C `idiv`: 16.16 fraction of start..end, so 0x8000 = the end); 14AEAA `mov word ptr [ecx+0x1c], 0xffff` | strong |
| hits 0x00 | count | int | 14A439 = 0, 14A670 `inc dword ptr [eax]`, 14A681 `cmp [eax], 1` (-1 when 0) | confirmed |
| hits 0x04 | hits | struct xn_collide_hit[] | 14A43F `add edx, 4`, 14A669 `add [0x14a6af], 0x1e` | confirmed |

- Conflict with structs.h collide_hit: `pad1C` is t / 2 (segment) or -1 (spheres); note x/y/z are
  only valid for the segment test (colstuff.c reads hits[j].y only after
  xn_collide_segment_model: consistent). The hit list has no bound: 136 hits reach big_buffer
  +0x1000, which 14AA92 uses as a work area (14AADD) while still filling hits.
- Return values: eax = the list (big_buffer) when count >= 1 in mode 0; 0 for a hit in mode 1
  (first sphere hit) or 2 (bounding sphere only); -1 none.

## struct xn_collide_scratch (0x148 bytes at 0x14A100)
- What: the collision code's globals in data module xn_14A100 (0x14A100..0x14A247); the table
  xn_collide_flat_anchor_shift follows (2-byte entries at 0x14A248, high bytes read at 0x14A249).
- Users: 14A300, 14AA92, 14B1E0 (flat_normal, local_pos), 14B45B (ground_tri), dead 14A6C0,
  14A710, 14AF30, 14B017, 14B58E (build_*).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 0x000 | probe_matrix / ground_tri | union xn_mat3 / xn_vec3[3] | 14ABB4 (137000 of probe angles), 14ABC3 (15D170 input); 14B48F..14B551 (triangle for 14C021) | strong |
| 0x024 | pad_024 | char[24] | no reference to 0x14A124..0x14A13B | candidate |
| 0x03C | flat_normal | xn_vec3 | 14B341..14B34C, 14B3F6.. | strong |
| 0x048 | model_matrix | xn_mat3 | 14A359, 14A392, 14A5F8, 14AB4B, 14AEC6 | confirmed |
| 0x06C | model_matrix_inv | xn_mat3 | 14AB5A (15D2EC), 14AB8A | confirmed |
| 0x090 | model | struct xn_model_handle * | 14A303, 14AA96, 14A602 | confirmed |
| 0x094 | probe | struct xn_collide_probe * | 14AA9B, 14ABA5 | confirmed |
| 0x098 | seg_end | xn_vec3 * | 14A30E, 14A36C | confirmed |
| 0x09C | seg_start | xn_vec3 * | 14A308, 14A3AD | confirmed |
| 0x0A0 | local_end | xn_vec3 | 14A39C.. (end << 8 in object space), 14AB94.. (probe position) | confirmed |
| 0x0AC | local_start | xn_vec3 | 14A3D2.. | confirmed |
| 0x0B8 | hit_t | int | 14A5B1, 14A659 | strong |
| 0x0BC | mode | int | 14A314, 14AAA1, 14A346, 14A48B, 14AB15, 14ACFE | confirmed |
| 0x0C0 | local_pos | xn_vec3 | 14AABB.. (probe position, world), 14AAF7 | strong |
| 0x0CC | rel_matrix | xn_mat3 | 14ABC8 (15D170 out), 14AC0C | strong |
| 0x0F0..0x147 | build_* | see model.h | dead 14B58E only (cell size, radius, counts, centres, loop counters, bounding box) | candidate |

## struct xn_collide_seg_state / struct xn_collide_sph_state (36 bytes each)
- What: the two tests' own variables, stored in code right after their `ret`s (0x14A68F..,
  0x14AF0C..). Same contents, different order. See model.h and model_globals.csv; names.csv
  already has 0x14A68F, 0x14A693, 0x14A6A7, 0x14A6AB, 0x14AF18 (its notes for 0x14A697..0x14A6A3
  called them "xn_collide_seg_normals+4/+8/+C/+10").
- big_buffer during a test: +0x0000 the hit list; +0x1000 (14AA92) pointers to the probe spheres
  that meet the bounding sphere; +0x1400 those spheres in object space {x, y, z, r} 24.8;
  +0x2000 / +0x3000 the two face lists of the merge (6-byte xn_model_sphere_face).
- Confidence: strong.

## struct xn_scratch (44 bytes at 0x120288)
- What: XnGine's shared scratch vectors; no owner, every user writes before reading in the same
  call. Fields: a (0x120288 = xn_pick_view_x / _y / pick_distance), b (0x120294), flat
  (0x1202A0 = xn_pick_flat_x / _y / _z), len2 (0x1202AC), t (0x1202B0).
- xn_cam_cull_sphere 15CF18 writes a = (2 x * xn_cam_inv_scale_x >> 32, 2 y * inv_scale_y >> 32,
  z) of the view-space centre (24.8); 1401E0 reads it for the queue key. xn_render_pick leaves the
  picked point there, which the game reads as pick_distance.
- Type for the readable C: either this struct at 0x120288 or three xn_vec3 globals plus two ints
  (pick_distance must stay addressable as an int for the game).
- Confidence: strong (all 147 references in listing.txt fit these slots).

## Collision geometry tables
- xn_face_edge_tables 0x15C524: `int *[25]`, indexed by a face's point_count (0..2 null; 25+
  past the end), each pointing at n+1 point-slot offsets {0, 8, .., 8(n-1), 0} (0x15C588..
  0x15CA2F: xn_face_edge_slots): the edges (slot[i], slot[i+1]) of the face, used as
  `[face + slot + 8]` (the point's vertex) by 15CB00 (and dead 15CC36, 15CD33).
- xn_collide_point_in_face 15CB00: for each edge, ((A-P) x (B-P)) . normal (>> 16 each term);
  negative -> outside (returns it), all >= 0 -> 1. Its scratch: xn_collide_edge_v0 0x15C500,
  _v1 0x15C50C, xn_collide_test_point 0x15C518 (xn_vec3 each), the normal and points pointers
  at 0x15CC2E / 0x15CC32 (in code).

## Globals typed by these structs
- 0x0013F748 xn_model_rot_matrix: xn_mat3
- 0x0013F76C xn_model_queue_count: int
- 0x0013F770 xn_model_drawn_count: int
- 0x0013F780 xn_model_queue_ptr: struct xn_sort_pair *
- 0x0013F784 xn_model_queue: struct xn_sort_pair[200]
- 0x0013FDC5 xn_model_bounds_x0 .. 0x0013FDD1 _y1: int x 4 (0x13FDC4 is an unused byte)
- 0x000C5408 xn_model_base_matrix, 0x000C542C xn_model_object_matrix, 0x000C5450
  xn_model_combined_matrix: xn_mat3; 0x000C5404 xn_model_angle_or_bits: int
- 0x000D7AC0 xn_render_matrix_pool: struct xn_model_matrix_pool (view[200] at 0xD7AC0, light[200]
  at 0xD96E0)
- 0x000CEA70 xn_render_matrix_next: struct xn_model_matrix_slot *
- 0x00120288 xn_pick_view_x / 0x0012028C xn_pick_view_y / 0x00120290 pick_distance: struct
  xn_scratch.a (xn_vec3); 0x00120294 xn_scratch_vec_b (new): xn_vec3; 0x001202A0..A8
  xn_pick_flat_x/_y/_z: xn_vec3; 0x001202AC xn_model_uv_len2 (new): int; 0x001202B0
  xn_model_uv_t (new): int
- 0x0014A100..0x0014A247: struct xn_collide_scratch (existing names: xn_collide_matrix_b,
  xn_collide_flat_normal, xn_collide_model_matrix, _inv, xn_collide_model, xn_collide_spheres
  (= probe), xn_collide_seg_end, _start, xn_collide_local_end, _start, xn_collide_hit_t,
  xn_collide_mode, xn_collide_local_pos, xn_collide_rel_matrix; new build_* names for the dead
  builder 0x14A1F0..0x14A23C)
- 0x0014A68F..0x0014A6B2: struct xn_collide_seg_state (new: 0x14A697 xn_collide_seg_model,
  0x14A69B xn_collide_seg_normal, 0x14A69F xn_collide_seg_list, 0x14A6A3
  xn_collide_seg_list_next, 0x14A6AF xn_collide_seg_hit_next)
- 0x0014AF0C..0x0014AF2F: struct xn_collide_sph_state (new: 0x14AF0C, 0x14AF10, 0x14AF14,
  0x14AF1C, 0x14AF20, 0x14AF24, 0x14AF28, 0x14AF2C; 0x14AF18 xn_collide_sph_hits exists)
- 0x0015C524 xn_face_edge_tables: int *[25]; 0x0015C588 xn_face_edge_slots (new): int[319]
- 0x0015C500 xn_collide_edge_v0, 0x0015C50C xn_collide_edge_v1, 0x0015C518
  xn_collide_test_point (new): xn_vec3; 0x0015CC2E xn_collide_face_normal_ptr, 0x0015CC32
  xn_collide_face_points (new): xn_vec3 *; 0x0015CED6 xn_collide_edge_first (new): struct
  xn_model_face_point *; 0x0015CEDA xn_collide_edge_points (new): xn_vec3 *
- game side: 0x00196D4C xn_collide_probe: struct xn_collide_probe *; 0x00196D48
  xn_collide_result / D_00196D50: struct xn_collide_hits *

## Notes across subsystems
- render (xn_poly): xn_model_draw_faces 1403AF writes poly +0x00 = the face (14048F), +0x04 =
  the handle (140494, patch 0x140497), +0x18/+0x1C/+0x20 = xn_vert_cam of point 0 (1404AD..),
  +0x3C = xn_render_span_setups[tex +0x10] (1404DA), +0x40 = the texture cache entry (1404D1),
  +0x48 = the face's normal (a pointer into the model's normal list, 14046C; C810C and
  xn_light_add_point 15BF75 read it), +0x58 = d(1/z)/dx and +0x5C = it >> 3 (140466/14046F, also
  xn_span_dzdx). xn_poly_setup_textured 15BB8C then reads face->points[2].data (15BBCD), the
  handle's matrix (15BBD0) and face->points[0].uv_packed (15BC2D) and rewrites +0x18 with the
  packed texture origin. xn_poly_project_face 158420 reads only the face's point count and
  vertex offsets.
- render (vertex arrays): xn_model_transform_face_verts 1404FA indexes xn_vert_cam/_screen/_flags
  by the vertex byte offset (stride 12): flags +0 = 1 (transformed this model), +2 = clip code
  (bits: 1 x < -z, 2 x > z, 4 y > z, 8 y < -z, 0x10 z < near, 0x20 z > far); cam = matrix *
  (point + rel); screen +8 = 2^40 / z, +0 = (x * half_width * (1/z) >> 32 + (centre_x << 8) +
  0x80) >> 3 (5 fraction bits), +4 the same for y >> 8 (integer). Only vertices of front faces
  are transformed (lazily). xn_model_clear_vert_flags 140A28 clears byte +0 of point_count
  entries starting at xn_vert_flags (edi = 0xD49C0 + disp 0x100).
- render (spans): xn_model_is_occluded 140910 walks xn_render_span_rows over the projected
  sphere's rectangle and reads span +0x04 (x end), +0x06 (x start), +0x08 (1/z at x start),
  +0x0C (the polygon, its +0x5C); the model counts as occluded only if every pixel row of the
  rectangle is covered by spans whose 1/z is above 2^40 / (the centre's depth): the test uses the
  centre's depth, not the nearest point's (a model straddling a wall can be culled while its
  front half would show). Models with <= 4 faces are never tested.
- light: 140606 builds the light list (xn_light_ref): +0 light, +4..+0xC (light - model) << 8 in
  object axes (directional, type 8: the direction rotated), +0x10 light +0x0C, +0x14 light +0x14
  << 4; it stops at the first light whose +0x0C <= 0 and skips point lights with |d|^2 -
  radius_sq - light+0x14 >= 0. xn_light_add_point 15BF75 uses face plane_d (via poly +0 +0x14),
  the face normal (poly +0x48), handle rel (+0x14..+0x1C) and the light matrix (handle +8,
  + 0x1C20). Light setup sets handle flags bit 1 (15BC48).
- camera: xn_cam_cull_sphere 15CF18 also returns its view-space centre in xn_scratch.a and
  uses xn_poly_clip_outcode_or 0x158211 as scratch for the clip code.
- texture: xn_model_draw_faces looks textures up with xn_tex_cache_lookup(texture >> 7,
  texture & 0x7F, ebx = -1) and fails the whole model (CF: draw stops) when it fails.
- game (include/): see the conflicts above (records.h model_instance size and fields; structs.h
  collide_probe pad0C = angles, collide_hit pad1C = t/2, arch3d_plane unknown1 = shade).

# World (WOODS.WLD), terrain, flats, animation, sky

Structs: xn_anim, xn_ascr, xn_wld_header, xn_wld_cell_header, xn_wld_cell, xn_wld_bands,
xn_world_nature_odds, xn_terrain_vert_coord, xn_flat, xn_sky_star, xn_snow_flake. The streaming
window, the terrain tables and the flat globals are typed arrays (section "Globals typed by these
structs"); the four 256x256 layers' byte layouts and the terrain's vertex and polygon writes are
in their own sections below. DFU references: WoodsFile.cs fetched from GitHub (master).

## struct xn_anim (24 bytes)
- What: a creature's animation state, stepped by the ASCR script interpreter.
- Instances: the game's `struct monster_anim` (include/records.h) at monster+0x2C1 (character
  record + 0x27A) of every type-18 creature (load_relink_character also resets it for type 44).
  The game passes it in eax (xn_anim_reset C010F, xn_anim_update C013B).
- Users: xn_anim_reset C010F (monster_init, load_relink_character), xn_anim_update C013B
  (object_draw_cb), xn_anim_tick C019C, xn_anim_run_opcodes C01EB and the opcode handlers
  C020A..C02DF (esi = the state, edi = the script position); the game's monster_set_action,
  monster_set_action_seducer, object_draw_cb, equip.c (events), qkey.c, monster_reload_anim_cb.
- Fields:

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 00 | frame | u16 | C0206 `mov [esi],ax` (frame byte: -b-1), C0288 (opcode 3 operand), C011C reset; object_draw_cb passes anim_frame to xn_flat_add; records: 0..3 | confirmed |
| 02 | frame_copy | u16 | C0202 `mov [esi+2],ax` and C0284 write the same value as +0; C0118 reset; no engine or game reader (records.h: pad02) | strong |
| 04 | script | ptr | C016C `mov ebx,[eax+4]` + state table at +6; C0221/C026B/C02A0 rebase gotos on it; monster_init stores the ASCR record; records: 0x291D24, 0x236DCC (ASCR records, size words 0x162/0x185) | confirmed |
| 08 | pos | ptr | C0187 `mov [eax+8],ebx` (state entry), C01D6/C01DE (opcode loop), C0141 `test [eax+8]` (0 = idle), C012C reset; records: script+0x12C.. | confirmed |
| 0C | tick_divisor | u16 | C01AE `movzx ebx,[esi+0Ch]` divides xn_anim_ticks (C01A9/C01B5); the game copies the image's frame_time (+16h) and calls it frame_count; records: 0x0101 | confirmed |
| 0E | last_step | u16 | C01BA `cmp ax,[esi+0Eh]`, C01C0 store; records: 0x67 = 26643/257 | confirmed |
| 10 | events | u16 | C023F opcode 2 `or [esi+10h],ax`; C0256/C0260 opcode 9 bit 15; C02C9 opcode 8 test; C011F reset; game: anim_events (bit 0 strike, bit 1 missile), anim_flags bit 7 (= bit 15) mirror; object_draw_cb passes (bits >> 10) & 20h as the flat's mirror flag; records: 0x8000 | confirmed |
| 12 | wait | u8 | C01C4..C01D1 count down (FFh holds), C0297 opcode 3 sets it (0 -> FFh), C014A/C015F in update; C0123 reset; records 0 | confirmed |
| 13 | opcode10_byte | u8 | C024B opcode 10 store; C0114 reset to FFh; no reader anywhere (engine or game) | strong |
| 14 | request | u8 | C013B `cmp [eax+14h],0FFh`, C0165 read, C018C clear to FFh; game monster_set_action writes the action and waits for FFh | confirmed |
| 15 | facing | u8 | C0126 reset clears it; game monster_set_action writes 0..4; object_draw_cb draws record_group + facing | strong |
| 16 | record_group | u8 | C02E5 opcode 11 `mov [esi+16h],al`; C0129 reset; object_draw_cb image = group + facing; operands seen statically 0, 5, 10, 15, 20..25 | confirmed |
| 17 | state | u8 | C0169 `mov [eax+17h],dl` (the request it starts); game anim_current (equip.c compares it with 24) | confirmed |

- Confidence: confirmed overall (asm + records + game C agree). Size: 0x18 for the engine.
- Conflicts with include/records.h `struct monster_anim`:
  - +0x02 `pad02` is not padding: the engine writes the frame there too (C0202, C0284, C0118).
    Harmless for the game (nothing reads it), but a C port that keeps the layout keeps the write.
  - +0x0C `frame_count` is the tick divisor (C01AE divides the clock by it); the game does copy
    the image's frame_time (ticks per frame) into it, so "ticks per step" is the real meaning.
  - +0x0E `timer` is the last step quotient (C01BA/C01C0), not a counting timer.
  - +0x10/+0x11 `anim_events`/`anim_flags` are one word for the engine (C023F `or word`,
    C0256 `and word, 7FFFh`, C02C9 `test word`); records.h's union already allows that.
  - `pad12[2]`: +0x12 is the wait count and +0x13 the opcode-10 byte (both engine-written).
  - +0x18 `pad18` and the size 25: the engine's struct ends at +0x18; the byte is the game's.
- Open questions: none worth an experiment. +0x13 has no reader and no script uses opcode 10
  (a static walk of the 60 ASCR records finds only opcodes 2, 3, 4, 7, 11 and frame bytes).

## struct xn_ascr (variable: 6-byte header + state table)
- What: an ASCR animation script record of MONSTER.BSA, used in memory as read.
- Instances: one per loaded creature kind (monster_anim_records D_00190704[anim_slot]);
  xn_anim.script points at it.
- Users: xn_anim_update C013B (state table), xn_anim_op_restart C026B (+2), gotos C0221,
  C029C, C02A7, C02C5 (offsets from the record start).
- Fields:

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 00 | size | u16 | static: equals the BSA record length in 60/60 ASCR records; no engine reader | confirmed (file) |
| 02 | restart | u16 | C026E `movzx eax,word [edi+2]` (opcode 5); static: = state_entry[0] in 60/60 | strong |
| 04 | state_count | u16 | static: 98 in 59 records, 120 in ASCR0029 (the seducer, states 56..60); scripts start at 6+3*count in all 60; no engine reader | confirmed (file) |
| 06 | state_entry[] | u16[] | C016F `add ebx,6` / C0172 `movzx ebx,word [ebx+edx*2]`, C0176 8000h -> C0180 `movzx ebx,word [ebx+6]` | confirmed |

- After the word table come state_count bytes (5, 10, 15, 20, 25 at byte indices 16, 32, 48,
  64, 80, 96) that no engine code reads; then the scripts.
- Opcode operand layouts (from the handlers): 0 loop start {min, max, counter written into the
  script at +3}, 4 bytes; 1 loop end {u16 offset of the loop start}, 3; 2 set events {u16}, 3;
  3 frame+wait {frame, wait}, 3; 4 goto {u16}, 3; 5 restart (broken: C0274 stores the offset
  where a pointer belongs, C0277 jumps to it); 6 stop (pos = 0); 7 random goto {percent, u16},
  4; 8 goto if events {u16 bits, u16}, 5; 9 mirror {flag}, 2; 10 {byte}, 2; 11 record group, 2.
- Unused-state entries are 0 (not 8000h) from index 49 on: a request there would run the
  record header as script. The game only requests 0, 8, 16, 24, 32, 40, 48, 56..60.

## struct xn_wld_header (144 bytes)
- What: the WOODS.WLD header.
- Instances: xn_world_header 0xC27E9 (read by xn_world_read_header C322D, 0x90 bytes from file
  offset 0). The names.csv globals xn_world_width 0xC27ED, xn_world_height 0xC27F1,
  xn_world_offsets 0xC27F5 and xn_world_bands_offset 0xC27F9 are its fields +4, +8, +0Ch, +10h.
- Users: C310F (open: size_x/z from +4/+8), C31AE (+10h), C322D (+0), C3301 (+0, +0Ch), dead
  editor C30D6/C31DB/C33F4; C2D8F and the load routines (+4, +8).
- Fields:

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 00 | offsets_size | u32 | C3253 compared with the window max; C3318 `[0xc27e9]>>2` = cell count; file: 0x1E8480 = 4*1000*500; DFU OffsetSize | confirmed |
| 04 | width | u32 | C2DA2 `imul ebx,[0xc27ed]` (row * width), C314C size_x = width<<15; file 1000; DFU Width | confirmed |
| 08 | height | u32 | C3159 size_z = height<<15, C2FAB south edge; file 500; DFU Height | confirmed |
| 0C | offsets | ptr | C326E `mov [0xc27f5],eax` (the malloc'd window), C3388/C3370 read; file 0 (DFU NullValue1); record: 0x12E4038 | confirmed |
| 10 | bands_offset | u32 | C31AE `mov edx,[0xc27f9]` seek; file 0x1E8510 = 0x90 + offsets_size; DFU DataSection1Offset | confirmed |
| 14 | unknown_14 | u32 | file 1; DFU Unknown1; no reader | candidate |
| 18 | unknown_18 | u32 | file 22 = the cell header size (C33B3 reads 16h bytes); DFU Unknown2; no reader | candidate |
| 1C | height_map_offset | u32 | file 0x1E8910 = bands_offset + 0x400; the 500000-byte map ends at the first cell record 0x262A30; DFU HeightMapOffset; no XnGine reader | confirmed (file) |
| 20 | pad_20 | u32[28] | file zero; DFU NullValue2[28] | confirmed (file) |

- Notes: the dead editor (C30D6) sizes the offset table as (w*h+1)*4 and puts the first record
  at offsets_size + 0x490 (no byte map), so the shipped file was not written by this editor.
  The offset table is read through a sliding window: see the xn_world_offsets_window_* globals.

## struct xn_wld_cell_header (22 bytes) and struct xn_wld_cell (47 bytes)
- What: a WOODS.WLD cell record (one per map pixel, 1000x500): the header and a 5x5 grid of
  control heights. xn_world_cell_header 0xC2879 is a copy of the last cell's header (it is not a
  whole xn_wld_cell: the 25 grid bytes go to big_buffer, C33BD..C33D6).
- Instances: the file (offset table entry i = cell row*1000 + col; records 47 bytes apart in
  all 499999 gaps); one xn_wld_cell_header at 0xC2879. The names.csv globals 0xC287D, 0xC287F,
  0xC2881, 0xC2882 are its fields +4, +6, +8, +9.
- Users: xn_world_read_cell C3301, xn_world_unpack_cell C343E, xn_world_gen_heightmap C355D
  (seed), xn_world_gen_tiles C3B87 / fix_lone_tiles C3C3C / place_nature_flats C3A60
  (climate), xn_terrain_add_nature_flats 13E69C and xn_terrain_draw_cells 13EFD7 (archives),
  world_render (writes the archives), the dead path pass C3DA9 (+0Ah).
- Fields:

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 00 | seed | u32 | C356B `mov eax,[0xc2879]` -> C3570 xn_rand_seed; record 0xFA7 (cell 343397); DFU splits it wrongly as u16 Unknown1 + u32 NullValue1 | confirmed |
| 04 | nature_archive | u16 | 13E69C `cmp word [0xc287d],1`, 13E70C archive<<7; file 0 everywhere; world_render writes nature_texture_archive (501..511 in records) | confirmed |
| 06 | ground_archive | u16 | 13EFDC `movzx eax,word [0xc287f]` -> 135D00; file 2/102/302/402 (record 0x12E = 302); DFU FileIndex; world_render overwrites | confirmed |
| 08 | climate | u8 | C3BBD/C3C84/C3AC4 `movzx [0xc2881]` * 4 into the band table; file 0..1 seen (0..2 per part A); DFU Climate | confirmed |
| 09 | noise | u8 | C33C8/C343F/C355D `and 1Fh` (full-layer block); C33DB `shr 5` -> xn_world_noise_amp; record 0x40 (amp 2); file amps 3..6 in a 1/997 sample; DFU ClimateNoise | confirmed |
| 0A | path_starts[6] | u16[6] | dead C3DA9 `mov word [0xc2883],410h`, C3DD7/C3DE2 `[ecx+0xc2883]` 0-terminated; file zero; DFU NullValue2[3] | strong |
| 16 | elevation[5][5] | u8 | (xn_wld_cell only) C33BD..C33D6 read 19h bytes to big_buffer; C3473 `lodsb`, C3474 `add al,80h`, planted at grid_y[0][r]*256 + grid_x[0][c] (C3476/C347F); file values 0..109 | confirmed |

- Open question: header +9 bits 0-4 non-zero (a 0x10000-byte block of all four 128x128 layers
  instead of the grid: C33D1, C349F..C3513) never occurs in WOODS.WLD; nothing to test.

## struct xn_wld_bands (4 bytes; 256 of them = 0x400)
- What: one climate's height thresholds.
- Instances: xn_world_height_bands 0xC23E9 (struct xn_wld_bands[256]), read from the file by
  xn_world_read_height_bands C31AE (DFU DataSection1, "256 UInt32, purpose unknown").
- Users: xn_world_gen_tiles C3B87 (C3BC9, C3BD8, C3BE9), xn_world_fix_lone_tiles C3C3C (C3C8E:
  [0] = the water level written over lone class-0 tiles), xn_world_place_nature_flats C3A60
  (C3ACB [0], C3AD4 [3]).
- Field: +0 threshold[4] (u8): class = first i with h <= threshold[i] (else 3); for class > 0 a
  random 0..(t[i]-t[i-1]) compared with t[i]-h drops it to class-1 (dither). Records and the file
  agree: (3,15,75,110), (3,20,90,110), (3,10,20,60), the rest zero. Confidence confirmed.

## struct xn_world_nature_odds (129 bytes)
- What: the current region's nature-flat odds (density, then cumulative odds per height band).
- Instances: xn_world_nature_flat_odds 0xC2BB8 (climate_set_textures copies region_flats[].f2,
  129 bytes, capping density at 50).
- Users: xn_world_place_nature_flats C3A60.
- Fields: +0 density (u8; C3AA2 `cmp al,[ebp]` against a 0..100 roll; record 0x32); +1
  odds[4][32] (u8; C3ADD `and ebx,0FFFFFFE0h` = (height & 60h) selects the 32-byte row, C3AE7
  FFh ends it, C3AED roll < odds -> flat n = index+1, stored as n<<2 at C3AF9; record rows
  05 0F 14 1E ... FF). Confidence confirmed.

## The streaming window: four 256x256 layers (no struct; byte layouts)
- Allocation: xn_world_init C2D00 mallocs 0x40020 bytes (xn_world_layers_alloc 0xC28CC), aligns
  to 32 and cuts four 0x10000-byte layers; the four pointers at 0xC28BC are an array (C34F0
  `add edi,[ebx+0xc28bc]`, ebx = 0..12): [0] height, [1] flat, [2] tile, [3] water. Records:
  0x2A3040, 0x2B3040, 0x2C3040, 0x2D3040.
- Indexing: layer[(row & 255) << 8 | (col & 255)] with col = x >> 8 and row = (size_z - z) >> 8
  (row 0 north): 13E6AA..13E6CC, 13E954..13E976, 14B465..14B481. A cell is 128x128 squares; the
  2x2 window holds the cells around the eye in slots by parity: slot = (col_cell & 1) +
  2*(row_cell & 1) (C2DB8; xn_world_slot is slot*4), slot origin (xn_world_slot_x0[s],
  xn_world_slot_y0[s]) = (0|128, 0|128). The game's D_00187F30[slot] is the same origin as
  y0*256 + x0 (ground_tile_at, town_block_apply_ground, terrain_update_cells).
- Height layer (byte): bits 0-6 the height step 0..127 (world height = xn_world_height_scale
  [h]: 14B48C, 13E9BB indexes the cam tables with all 8 bits, C3FDF); bit 7 = the square
  (h, h+1, h+256, h+257) is not planar, set by xn_world_mark_nonplanar_quads C3FCB (C4057; it
  never clears it: the generator writes fresh 0..127, C3A38). xn_world_open fills the layer
  with FFh (C3135..C3145). Records: max 57, no bit 7 before C3FCB.
- Flat layer (byte): bits 0-1 the tile's mirror flips (bit 0 flips u, bit 1 flips v in
  13EFD7), written by xn_world_blend_tiles C3F9E from xn_terrain_corner_flip (values 0, 1, 2);
  bits 2-7 the nature flat 1..32 (C3AF2..C3AF9, n<<2; C3A88 keeps bits 0-1); 13E6E3..13E6F8
  draws n in 1..33 as record n-1 of the nature archive. RMB ground scenery bytes are copied in
  with the same layout (town_block_apply_ground: < 33 << 2, FFh keeps only bits 0-1). Records:
  bits 0-1 = 0/1/2 (63030/1303/1203 of 65536).
- Tile layer (byte): bits 0-5 the ground texture record (13F097 `and edx,3Fh` -> 135D00 with
  the ground archive; ground_tile_at returns & 63); bits 6-7 the rotation 0..3 (13F07C `shr
  al,3; and 0F8h` selects the axis setters). gen_tiles writes the class 0..3, blend_tiles the
  transition tile with its rotation (xn_terrain_corner_tile values C5, D4, 45, ...). Records:
  records 1..53, rotations 0..3.
- Water layer (byte): the generated value (0..7Fh) where it is below sea level (80h), 0 on land
  (C3A2C..C3A3E). Write-only: no reader in object 2 or the game (grep of all four pointers).

## The terrain grid and the vertex arrays (for the render section's xn_vert_* types)
- xn_terrain_transform_grid 13E8C8 fills 32x32 = 1024 vertices, vertex i at offset 12*i in each
  of xn_vert_cam 0xCEAC0, xn_vert_screen 0xD1AC0, xn_vert_flags 0xD4AC0 and the two
  xn_terrain_vert_coord arrays (stride 12: `add edi,0Ch` 13EAE0; rows of 0x180 bytes, the grid
  ends at 0x3000, patched 13EB16). Grid vertex (r, c) is layer square (row0 + r, col0 + c)
  with col0 = (cam_x >> 8) - 16, row0 = ((size_z - cam_z) >> 8) - 16 (13E954..13E976).
- xn_vert_cam (12 bytes): +0 x * xn_cam_scale_x >> 14 (13EA01), +4 y * xn_cam_scale_y >> 14
  (13EA12), +8 z (13EA18), camera space (24.8 world units). Not written when z <= -0x12C00.
- xn_vert_screen (12 bytes), written only when the vertex has no clip bit (13EA58): +0 screen x
  in 1/32 pixel ((x*half_width*2^40/z >> 32) + centre_x*256+80h, then >> 3: 13EA73..13EA85), +4
  screen y in pixels (same with half_height, >> 8: 13EA8B..13EA9C), +8 2^40 / z (13EA69).
- xn_vert_flags (12 bytes; the terrain writes +0..+2, the rest untouched):
  - +0: the flat-layer byte (13EAB9/13EABC): bits 0-1 the u/v flips, bits 2-7 the nature flat.
  - +1: the clip code (13EAB2 word store, low byte): 1 x < -z, 2 x > z, 4 y > z, 8 y < -z
    (scaled x/y, 13EA3A..13EA55), 10h nearer than xn_cam_near_z, 20h beyond xn_cam_far_z
    (13EA24..13EA37), ORed with bit 7 of the height byte (13EA1E: the non-planar flag). Quirk:
    a vertex with z <= -0x12C00 (13E9DF) keeps the whole height byte ORed with 10h here.
  - +2: the tile-layer byte (13EAA9; the same word store): bits 0-5 texture, 6-7 rotation.
- Records (13E8C8, at return): in all 1024 vertices +0, +2 and bit 7 of +1 equal the flat,
  tile and height layer bytes at (row0 + r, col0 + c); 522 vertices have +1 = 3Dh (behind the
  eye: height 2Dh | 10h), 25 have no clip bit. The height tables' copies at +0x200 equal the
  first halves.
- 13EFD7 reads +1 of the four corners (v, v+1 = +0Ch, v+row = +180h, v+row+1 = +18Ch): AND of
  the codes & 7Fh != 0 culls the cell, OR != 0 sends it to the clipped path, bit 7 of corner 0
  splits it into triangles a (v, v+row, v+row+1) and b (v, v+row+1, v+1). 13E69C skips nature
  flats on vertices with 10h or 20h.

## struct xn_terrain_vert_coord (12 bytes; 1024 each)
- What: one entry of the unscaled camera x (0x138578) and y (0x13B578) arrays of the grid.
- Users: 13E8C8 (13E9EB, 13E9F1 write), xn_terrain_face_plane_a 13EB21 and _b 13EC22 (with
  xn_vert_cam.z: edge vectors, normal, back-face test).
- Fields: +0 value (int, confirmed: 13E9EB/13E9F1 and the readers at +0, +0Ch, +180h, +18Ch);
  +4..+0B pad (no instruction addresses 0x13857C/0x138580/0x13B57C/0x13B580 or any +4/+8 of an
  entry). The tables are `int[1024*3]` in effect; the 12-byte stride lets the code use one index.
  Records: e.g. vertex 873 value 884686.

## Polygon fields the terrain writes (for the render section's struct xn_poly)
All on the record at xn_render_poly_next D_000CEA64 (edi), which the draw routines then advance
by 0x64 (13EDC4, 13EE6E, 13EF1E, 13EFCE, 13F2AA, 13F387). Records (13EFD7, cheat_dorian): +04 1,
+14h 0x116C80, +18h 0x45898408, +24h..2Ch FFFFFE9B FFFFFFC7 197E2, +30h..38h 20C FFFFFFDA 11600,
+3Ch 0x12A740, +40h 0x408729.
- +00: not written (stale); +04: 1 (13EC17, 13ED18): "no model" (xn_pick_hit.model 1 = nothing
  to pick).
- +14h: the light list (xn_render_light_list_next at the time, 13EC09/13EC14): all terrain
  polygons point at the list xn_terrain_build_light_list 13E63C writes after the cells
  (directional lights only, 24-byte entries).
- +18h (dword) then +1Ah (word): the packed texture origin: the u setter stores the u origin as
  a dword (13F4ED, 13F515, 13F54B, 13F57F; xn_world_scratch_x, scratch_y or 4000h minus them),
  then the v setter overwrites the high word with the v origin (13F5A5, 13F5CD, 13F603,
  13F637): low word u0, high word v0. 13EFD7 steps the origins by -3E00h per column/row
  (13F393, 13F3AE).
- +24h/+28h/+2Ch: the u gradient (per screen x, per screen y, constant) = +/- xn_terrain_u_axis
  or +/- xn_terrain_v_axis (13F4D2/4DB/4E4 ...); +30h/+34h/+38h the v gradient (13F58A/593/59C
  ...). The setter is chosen by xn_terrain_u_axis_fns[tile >> 6][flat & 1] and
  xn_terrain_v_axis_fns[tile >> 6][(flat >> 1) & 1] (13F087, 13F09A): u: rot 0 (x, -x), 1 (-z,
  z), 2 (-x, -x), 3 (z, z); v: rot 0 (z, -z), 1 (x, -x), 2 (-z, z), 3 (-x, x). So rotations 2
  and 3 ignore the u flip (table bytes at 0x138548..0x138554; records agree): a quirk of the
  data, to keep in a port. Whether +24h is u and +30h v is the render section's call (part D open
  question 3); the terrain only fills them symmetrically.
- +3Ch: the span setup routine from xn_render_span_setup_terrain_ptr D_0012A019, patched into
  13F067/13F0E0/13F17F by 13F004..13F013 (records 0x12A740).
- +40h: the texture: xn_tex_cache_lookup(ground archive, tile & 3Fh, -1) (13F0B0, 13F129,
  13F1C9).
- +50h/+54h/+58h: the face normal n = e1 x e2 from the unscaled camera coordinates, each
  product's high dword after <<4 (13EBD9 nx, 13EBD3 ny, 13EBD6 nz; 13ECD4..13ECDA for b).
- +5Ch: 4*nx*xn_cam_inv_focal_x / (n . v0) (13EC06, 13ED07), also stored in xn_span_dzdx
  0x15B980; n . v0 >= 0 is a back face (carry, 13EBFC).

## struct xn_flat (100 bytes)
- What: a flat (billboard) queued for this frame: image, flags, scale and light, view position,
  then the texture gradients and span routine its draw sets up.
- Instances: records of the render polygon pool (0x64 each, xn_render_poly_next D_000CEA64),
  at most 512 a frame (xn_flat_count 0x154C48, 154D59/154DAB), each listed in
  xn_flat_sort_list 0x153C44 as {-view_z, record}. The game keeps the address as the object's
  draw_handle (+2Fh) and pick_sprite_cb compares it with the picked record.
- Users: xn_flat_add 154D00(eax x, edx y, ebx z, ecx image; stack: frame, flags, scale |
  light << 16; world coordinates, moved to view space through xn_cam_view_matrix by 154D20;
  culled when z <= 0 or >= xn_cam_far_z; returns the record, or the scale argument when culled)
  and xn_flat_add_body 154D20 (game: object_draw_cb, automap...); xn_flat_add_view 154DA5
  (eax/edx/ebx = a view-space point, ecx = image; frame 0, flags 4, scale 100h, light 0; no
  culling but the 512 cap), called by xn_terrain_add_nature_flats 13E69C with the vertex's
  xn_vert_cam and image nature_archive << 7 | (n-1); xn_flat_draw 154E20, the quad builders 154EDC/154FD8,
  xn_flat_setup_gradients 1550E0, xn_flat_raster 1552A0, xn_flat_span_emit 15526C,
  xn_flat_pick 155508, xn_flat_span_light_setup 155610, the spans 157620, 157800, 157B20,
  157E20.
- Fields:

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| 00 | image | u32 | 154D72 `pop [edi]` (arg ecx), 154DC5; 154E20 `shr eax,7` / `and edx,7Fh`, 155548; records 0xFA9C (501<<7|28), 0x691C (210<<7|28) | confirmed |
| 04 | object | int | 154D83 / 154DDE `mov [edi+4],0`; xn_pick_hit.model 0 = a flat (engine_pick_object); records 0 | confirmed |
| 08 | du_dx | int | 155116 `mov [edi+8],edx`, 1551D4 neg (mirror); spans 157654 `mov eax,[esi+8]`; records 0x369C, 0x147A | confirmed |
| 0C | dv_dx | int | 155156 `mov [edi+0Ch],edx`; spans 15765D `imul ecx,[esi+0Ch]`; records 0 (upright flat) | strong |
| 10 | shade_row | ptr | 1556A9 (light row), 15571A (fog row); 157855, 157B7B read; record 0xB2DE06 = fog_last - (z>>8 - start)*step | confirmed |
| 14 | flags | u32 | 154D77 (arg), 154DCE = 4; 154E7B `and ebx,1Fh` -> quad table; 1551CB `test 20h` mirror; 15556C; records 4 | confirmed |
| 18 | u_offset | int | 15515C store; 1551E0/1551E6 mirror (neg, - tex_w<<23); spans 15763E `sub eax,[esi+18h]`; records 0xD320FBB0 | strong |
| 1C | v_offset | int | 15518D store; spans 15764E; records 0x160715DE | strong |
| 20 | pad_20 | | no flat code addresses +20h | strong |
| 24 | u_dx | int | 15511C (du_dx >> 3), 1551D7 neg; spans 157632; records 0x6D3 | confirmed |
| 28 | u_dy | int | 155126; 155190 patched into the row loop (15534D adds it to +2Ch per row); records 0 | strong |
| 2C | u_c | int | 155130, 1551A4 `add [edi+2Ch],edx` (top row); spans 157636 | strong |
| 30 | v_dx | int | 15515F (dv_dx >> 3); spans 157641; records 0 | strong |
| 34 | v_dy | int | 155169; 1551B0 (patched row step at 15535D); records 0x78C | strong |
| 38 | v_c | int | 155173, 1551BF; spans 157648; records 0x1517 | strong |
| 3C | span | code ptr | 154ECD = 155610; 1556AC/1556D0/1556EF/155713/15571E install 157800/157B20/157620/157800/157E20; 15527B `call [esi+3Ch]`; records 0x155610 -> 0x157620/0x157800 | confirmed |
| 40 | texels | ptr | 154E5B (image + [image+0Eh]); spans 15766C `mov esi,[esi+40h]` (texel = [v<<8 | u]) | confirmed |
| 44 | scale | u16 | 154D9E `pop [edi+44h]` (7th arg); 154E4F `add bp,[eax+18h]` (x_scale), 154E5E; 1550FD, 155557 `and ebp,0FFFFh`; records 0x100 -> 0x180 | confirmed |
| 46 | light | u8 | 15561D..15562E `shr ebp,8; and ebp,0FF00h` (row*256 + patched base 155634; >= xn_shade_table_last_row -> unshaded); object_draw_cb passes 64<<16 for glowing creatures | strong |
| 47 | unknown_47 | u8 | written by 154D9E (the arg's top byte); never used | strong |
| 48 | pad_48 | | no flat code addresses +48h | strong |
| 4C | frame / table | union | 154D74 `mov [edi+4Ch],ebp` (frame arg), 154DC7 = 0; 154E2A passes it to 135D00 (negative = animate by xn_anim_ticks, 135D76); 154E43 replaces it with D_00136921[word +0Ah of the cache entry]; 155610 tests it (non-zero -> 157E20); 1556D7 fog row; 157B84/157E7B read | confirmed |
| 50 | view_x | int | 154D7A, 154DD5; 154F13, 155666; records FFE9A111 | confirmed |
| 54 | view_y | int | 154D7D, 154DD8; 154F16 | confirmed |
| 58 | view_z | int | 154D80, 154DDB; 154D97 key = -z; 1556B3 fog distance (>>8); 155669 | confirmed |
| 5C | pad_5c | | no flat code addresses +5Ch/+60h | strong |

- Confidence: confirmed for the add/draw fields, strong for the gradient terms.
- Conflicts: none with include/ (the game only uses the address, and xn_pick_hit.model = 0 is
  this +04). The pool record is shared with the polygon (struct xn_poly): a flat's +08/+0C/+10/
  +18/+1C/+4C are not the polygon's fields of the same offsets.
- Open questions:
  1. 155610 sets the span routine and returns without drawing, so the first visible piece of
     each flat in a frame is not drawn (part E). Experiment: record 15526C for one flat in a
     town and compare pixels written with the span lengths.
  2. 157E20 (translucent: +4Ch = the translucency table) never ran in the records. Experiment:
     a ghost or wraith (archives 273/278, xn_tex_archive_set_translucent) in view.
  3. The meaning of the -8 in the patched v step (1551B3 `sub edx,8`): read the row loop once
     with a record of 1552A0 to see what 15535D holds.

## struct xn_sky_star (12 bytes; 768) and struct xn_snow_flake (6 bytes; 100)
- Stars: xn_sky_stars 0xC547A (768 * 12 = 0x2400, ending at xn_sky_star_colours 0xC787A).
  xn_sky_init_stars C816E (sky_init): random yaw < 400h and pitch >= 400h (C81C8/C81D0) turned
  into a vector of length 2^20 (C81C3, C82C2) stored at +0/+4/+8 (C81DC..C81E1); a colour per
  star from xn_sky_star_palette 0xC7B7A (16 bytes) into xn_sky_star_colours (C818D). The drawing
  code (C8292, C81F2) is dead. Confidence strong (asm).
- Snow: xn_snow_flakes 0xC7B8B (100 * 6 = 0x258, ending at xn_snow_speed_small 0xC7DE3). +0 x<<6
  (C9C51, C9AA4 adds xn_snow_turn_shift<<6, C9B59), +2 y<<6 (C9C2A, C9B5C), +4 drift +-1 (C9C64,
  C9B3C, C9B51 random flip). C9AE6/C9AEC: the loop counter counts down from 100, so flakes 0-19
  are 2x2 at xn_snow_speed_large, 20-49 2x2 at _medium, 50-99 single pixels at _small (colour
  70h, rows 0x140 apart). Confidence strong.
- Rain: xn_rain_streak_colours 0xC7DF1, 30 bytes, one streak top to bottom (C9D1E, C9D89..);
  C9CB9 draws 50 random streaks a frame, no state.

## Value noise tables (xn_C5200)
- xn_noise_values 0xC4100 int[256] (rand & FFh with xn_rand_seed = 1, so fixed: C522C) and
  xn_noise_values_copy 0xC4500 int[257] (the copy, C5232, plus values[0] at 0xC4900, C5242): one
  int[513] in effect, so C52B3/C52CC/C52D8 can read index 256.
- xn_noise_fade 0xC4D04 is the centre of int[512] at 0xC4904 (index -256..255, C526E): (3t^2 -
  2t^3) in 8.8 for t = i/256, ((300h - 2i)*i*i + 8000h) >> 16. xn_rand_noise_2d C5280 only
  indexes 0..255 (C5287/C528D `and 0FFh`).

## Globals typed by these structs
World (xn_C2300 data):
- 0x000C0000 xn_anim_opcodes: `void (*xn_anim_opcodes[12])(void);`
- 0x000C0030 xn_anim_rand_state: `unsigned short xn_anim_rand_state;`
- 0x000C233A xn_world_file: `int xn_world_file;` (DOS handle)
- 0x000C2342 xn_world_offsets_window_bytes (new): `unsigned int` bytes of the offset table in
  the window = min(window_max, header.offsets_size) (C3260; C3275 read size; C332D)
- 0x000C2346 xn_world_offsets_window_max (new): `unsigned int` 0x4000, the window's capacity (C324E)
- 0x000C234A xn_world_offsets_window_count (new): `unsigned int` entries in the window, bytes/4 (C32B1, C3307)
- 0x000C234E xn_world_offsets_window_first (new): `unsigned int` first cell index in the window (C32B6, C3346, C3382); record 0x53565 = cell - 0x800
- 0x000C2352 xn_world_lone_tile_table: `unsigned char xn_world_lone_tile_table[4][4];` [neighbour class][self class] (C3CF2..C3CF8)
- 0x000C2362 xn_world_tile_class: `unsigned char xn_world_tile_class[65];` indexed by tile & 3Fh
- 0x000C23A3 xn_world_dir_dx: `signed char xn_world_dir_dx[8];`
- 0x000C23AB xn_world_dir_dy (new): `signed char xn_world_dir_dy[8];` (0,1,1,1,0,-1,-1,-1; C3B32)
- 0x000C23D0 xn_world_size_x, 0x000C23D4 xn_world_size_z: `unsigned int` (cells << 15)
- 0x000C23D8 xn_world_eye_sub_x, 0x000C23DC xn_world_eye_sub_z: `int` 0..127
- 0x000C23E0 xn_world_eye_col (new): `int` cell column of the last xn_world_cell_index query (C2DAC; = x >> 15; the eye's in C2E05/C2FF5; read by C2EE1, C2F27)
- 0x000C23E4 xn_world_eye_row (new): `int` its row from the north (C2D9C; read by C2F66, C2FB1)
- 0x000C23E8 xn_world_loaded_dirs: `unsigned char`
- 0x000C23E9 xn_world_height_bands: `struct xn_wld_bands xn_world_height_bands[256];`
- 0x000C27E9 xn_world_header: `struct xn_wld_header xn_world_header;` (0xC27ED/F1/F5/F9 are its fields)
- 0x000C2879 xn_world_cell_header: `struct xn_wld_cell_header xn_world_cell_header;` (0xC287D/7F/81/82 are its fields)
- 0x000C2883 xn_world_cell_path_starts (new; = xn_world_cell_header.path_starts): `unsigned short[6]` (dead C3DA9)
- 0x000C2893 xn_world_slot_cells: `int xn_world_slot_cells[4];` (-1 none; set to -1 by C3161..C317F)
- 0x000C28A3 xn_world_noise_amp: `int`; 0x000C28A7 xn_world_noise_bias (new): `int` = amp >> 1, subtracted from each 0..amp roll (C33ED, C3632)
- 0x000C28AB xn_world_edge_near (17), 0x000C28AF xn_world_edge_far (111): `int`
- 0x000C28B3 xn_world_update_pass (new): `unsigned char` xn_world_update's two-pass counter (C2E0D, C2EB9, C2EBF)
- 0x000C28B4 xn_world_rand_elev_min (new), 0x000C28B8 xn_world_rand_elev_max (new): `int` clamp of the dead xn_world_random_elevations (C352D, C353A; both 7Fh)
- 0x000C28BC xn_world_height_layer .. 0x000C28C8 xn_world_water_layer: `unsigned char *xn_world_layers[4];` (C34F0 indexes them as an array)
- 0x000C28CC xn_world_layers_alloc: `void *`
- 0x000C28D4 xn_world_gen_step (new): `int` the midpoint-displacement step, 32 down to 1 (C3575, C39DF, C39E5)
- 0x000C28D8 xn_world_gen_count (new): `int` squares per side at the step, 4 up to 128 (C357F, C39D9)
- 0x000C28DC xn_world_scratch_x / 0x000C28E4 xn_world_scratch_y: `int` (also the terrain's u0/v0 texture origins, 13F42A/13F45C)
- 0x000C28E0 xn_world_scratch_x2 (new), 0x000C28E8 xn_world_scratch_y2 (new): `int` the far corner of the current square in C355D (C35C7, C35D1)
- 0x000C28F4 xn_world_slot: `int` slot*4
- 0x000C28F8 xn_world_slot_x0, 0x000C2908 xn_world_slot_y0: `int[4]`
- 0x000C2918 xn_world_grid_x (new): `int xn_world_grid_x[4][5];` slot x0 + 0, 32, ..., 128 (only [0] used: C347F)
- 0x000C2968 xn_world_grid_y (new): `int xn_world_grid_y[4][5];` slot y0 + 0..128 (only [0] used: C3476)
- 0x000C29B8 xn_world_height_scale: `int xn_world_height_scale[128];` (0, 40, 40, 40, 80 ... 7760)
- 0x000C2BB8 xn_world_nature_flat_odds: `struct xn_world_nature_odds xn_world_nature_flat_odds;`
- 0x000C4100 xn_noise_values: `int[256]`; 0x000C4500 xn_noise_values_copy: `int[257]`;
  0x000C4904 xn_noise_fade_table (new): `int xn_noise_fade_table[512];` (xn_noise_fade 0xC4D04 = &[256])

Sky (xn_C5400):
- 0x000C547A xn_sky_stars: `struct xn_sky_star xn_sky_stars[768];`
- 0x000C787A xn_sky_star_colours: `unsigned char[768]`; 0x000C7B7A xn_sky_star_palette: `unsigned char[16]`
- 0x000C7B8B xn_snow_flakes: `struct xn_snow_flake xn_snow_flakes[100];`
- 0x000C7DE3/7/B xn_snow_speed_small/medium/large: `int`; 0x000C7DEF xn_snow_drift_speed: `unsigned short`
- 0x000C7DF1 xn_rain_streak_colours: `unsigned char[30]`

Terrain:
- 0x00136E48 xn_terrain_step_x, 0x00136E54 xn_terrain_step_y, 0x00136E60 xn_terrain_step_z: `xn_vec3` each (rounded >> 8 by 13E7B5..13E847)
- 0x00137900 xn_terrain_height_cam_x, 0x00137D00 _y, 0x00138100 _z: `int[256]` (128 + the copy at +0x200 for height bit 7; 13E872/13E878 ...): step_y * (-height_scale[h] - cam_y), rounded >> 8
- 0x00138500 xn_terrain_tex_scale: `int` (0x0F800000)
- 0x00138520 xn_terrain_u_axis, 0x0013852C xn_terrain_v_axis: `xn_vec3`
- 0x00138538 xn_terrain_u_axis_fns, 0x00138558 xn_terrain_v_axis_fns: `void (*[4][2])(void)`
- 0x00138578 xn_terrain_vert_x, 0x0013B578 xn_terrain_vert_y: `struct xn_terrain_vert_coord[1024]`

Flats (xn_153C00 data):
- 0x00153C00 xn_flat_quad_table: `void (*xn_flat_quad_table[17])(void);` (154EDC x2, 154FD4 x2, 154FD8 x4, 1550BC x8, 1550C0)
- 0x00153C44 xn_flat_sort_list: `struct xn_sort_pair xn_flat_sort_list[512];` (key = -view_z, value = struct xn_flat *; ends at 0x154C44)
- 0x00154C44 xn_flat_sort_end: `struct xn_sort_pair *`
- 0x00154C48 xn_flat_count, 0x00154C4C xn_flat_drawn_count: `int`
- 0x00154C50 xn_flat_half_w, 0x00154C54 xn_flat_tex_w, 0x00154C58 xn_flat_half_h, 0x00154C5C xn_flat_tex_h: `int` (half_h is the full height for the standing quad, 154FE5)
- 0x00154C60 xn_flat_ignore_pitch (new): `unsigned char` 64h = build the flat matrix without the camera pitch (15541B); no writer, 0 in the records
- 0x00154C61 xn_flat_matrix, 0x00154C85 xn_flat_proj_matrix: `xn_mat3`

## Notes across subsystems
- render (xn_poly): see "Polygon fields the terrain writes". Terrain polygons have +04 = 1 and
  +14h = the terrain light list; flats share the pool with a different layout (struct xn_flat).
  The PATCHED placeholders at 15534D and 155357 show 0x154C58/0x154C50 in the listing, but
  1551AA/1551C5 re-point them at flat+2Ch and flat+38h at run time: they are not flat globals.
- render (vertex buffers): xn_vert_cam/xn_vert_screen/xn_vert_flags are 1024 x 12 bytes each
  (13E6FA indexes xn_vert_flags+1 with the 12-byte vertex offset). Terrain usage above; sx is in
  1/32 pixel and sy in pixels (13EA81 `shr edx,3`, 13EA99 `shr edx,8`), +8 = 2^40/z.
- render (sort): xn_render_sort_pairs 15810E sorts ascending, so the flat list (key -z) is drawn
  far to near (12A83D..12A858) and xn_flat_pick walks it backwards, nearest first (15553C).
- light: the terrain light list (13E63C) holds 24-byte entries {light *, view xyz, -[light+0Ch]}
  for type-8 lights only, at most 32 lights, -1 terminated; the flat light setup reads light
  +00/+04/+08 position, +0Ch intensity, +14h radius^2 (compared with the squared distance),
  +18h type (8 directional adds +0Ch << 8) (155654..1556E9).
- light/textures: xn_flat_draw reads the cache entry's word +0Ah as the index into the
  translucency table list D_00136921 (154E38/154E3C); include/structs.h tex_cache_entry calls
  +0x00..+0x0B pad00: +0Ah is that word (CDC99 sets it to 1 for archives 273/278). It also reads
  texture_header +0x04 width, +0x06 height, +0x0E data_offset, +0x18 x_scale (154E46..154E76).
- model/collision: xn_terrain_height_at 14B45B fills 0x14A100 (names.csv xn_collide_matrix_b,
  "3x3 rotation") as a ground triangle of three xn_vec3 {x 0 or 10000h, height<<8, z 0 or
  10000h} for xn_math_triangle_y_at 14C021, and returns -(y + 80h) >> 8; it also keeps x and
  size_z - z in xn_world_scratch_x/_y.
- io/camera: 0x154C60 (no writer) gates the flat matrix's pitch (15541B); flats then scale
  the matrix by xn_cam_flat_scale_x/_y (15544B, 155467).
- Quirk for the C port: xn_world_fix_lone_tiles C3C94 writes the water level at height[slot
  origin + climate*4] (edx still holds climate*4) before flattening the 3x3 block around a lone
  class-0 tile; the 3x3 writes wrap with dh/dl (16-bit row/col arithmetic), so neighbours of a
  slot-edge square land outside the slot.
- Quirk: xn_world_mark_nonplanar_quads reads +100h/+101h past the last row of the height layer
  (into the flat layer) for row 255; it only ever sets bit 7.

# Camera and view, input, graphics, fonts, VID, big_buffer, system

## struct xn_view (137 bytes)

- What: the projection constants and the frame pools' cursors, contiguous in the data module xn_CEA00: an overlay of globals names.csv already names one by one (the readable C can keep the globals; the struct gives their types and fixed point).
- Instances: 0xCEA20 (one; 0xCEAA9..0xCEABF is padding before xn_vert_cam).
- Users: writers: xn_cam_set_view_window 12A2D0, xn_cam_set_focal 12A274, xn_cam_update_derived 12A3AC, init_video (near_z 2560, far_z 393216), xn_render_init 12A100 (recip_table), xn_render_set_mode 12A254, xn_render_begin_frame 12A4F0, xn_render_frame 12A870, the pool users (158D7E, 1404D1, 140648, 13EC09...), xn_shade_set_fog 14D23C (span_hook), engine_pick_object (pick_skip_flats); readers: the outcode code (15830C, 1404FA, 13E8C8, 15CF18), xn_render_pick 12A608, xn_flat_* (flat scales), the span routines.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | near_z | int | 158311 cmp ebx,[0CEA20h]; init_video sets 2560 | confirmed |
| +0x04 | far_z | int | 15831C cmp ebx,[0CEA24h]; 14D24B (fog) | confirmed |
| +0x08 | half_width | int | 12A2D1 mov [0CEA28h],eax | confirmed |
| +0x0C | half_height | int | 12A2D6 mov [0CEA2Ch],edx | confirmed |
| +0x10 | centre_x | int | 12A2DC mov [0CEA30h],ebx; 12A698 sub eax,[0CEA30h] | confirmed |
| +0x14 | centre_y | int | 12A2E2 mov [0CEA34h],ecx; 12A8B6 | confirmed |
| +0x18 | focal_x | int | 12A275 mov [0CEA38h],eax; 12A6A0 idiv [0CEA38h] | confirmed |
| +0x1C | inv_focal_x | int (2^38/focal_x) | 12A287 div [0CEA38h] (edx = 40h); 12A28D mov [0CEA3Ch],eax; 13EBFE imul [0CEA3Ch] | confirmed |
| +0x20 | focal_y | int | 12A27A mov [0CEA40h],edx | confirmed |
| +0x24 | inv_focal_y | int (2^38/focal_y) | 12A29F mov [0CEA44h],eax | confirmed |
| +0x28 | render_mode | int | 12A257 mov [0CEA48h],eax | confirmed |
| +0x2C | poly_count | int | 1403F1 add [0CEA4Ch],eax (the model's face count); 12A59A mov [0CEA4Ch],0; records: 1015 | confirmed |
| +0x30 | unknown_30 | int | no reference | candidate |
| +0x34 | unknown_34 | int | 12A5A4 mov [0CEA54h],0 only | candidate |
| +0x38 | span_hook | void (*)(void) | 14D2C1 mov [0CEA58h],150040h; 12A940 call [0CEA58h] | confirmed |
| +0x3C | frame_flags | int | 12A871 mov [0CEA5Ch],eax; 12A98B test [0CEA5Ch],2 | confirmed |
| +0x40 | span_next | struct xn_span * | 12A563 mov [0CEA60h],eax; 158D89 add [0CEA60h],10h | confirmed |
| +0x44 | poly_next | struct xn_poly * | 12A4FB mov [0CEA64h],0DB320h; 140488 add [0CEA64h],64h | confirmed |
| +0x48 | light_list_next | struct xn_light_ref * | 12A505 mov [0CEA68h],116C80h; 1406CF | confirmed |
| +0x4C | row_y | int | 12A8BC mov [0CEA6Ch],eax (clip top - centre_y); 12A957 inc [0CEA6Ch]; 155D3B mov edx,[0CEA6Ch] | confirmed |
| +0x50 | matrix_next | struct xn_model_matrix_slot * | 12A4F1 mov [0CEA70h],0D7AC0h | confirmed |
| +0x54 | unknown_54 | int | no reference | candidate |
| +0x58 | scale_x | int (2.14) | 12A3BE mov [0CEA78h],eax (focal_x << 14 / half_width); 13772A mov ebx,[0CEA78h] (xn_cam_scale_matrix) | confirmed |
| +0x5C | inv_scale_x | int (2^45/scale_x) | 12A3CE mov [0CEA7Ch],eax; 15CF65 imul [0CEA7Ch] | confirmed |
| +0x60 | scale_y | int (2.14) | 12A3EA mov [0CEA80h],eax; 137752 | confirmed |
| +0x64 | inv_scale_y | int | 12A3FA mov [0CEA84h],eax; 15CF72 | confirmed |
| +0x68 | hfov_cos | int (16.16) | 12A467 mov [0CEA88h],eax (xn_vec_normalize(focal_x, half_width)); 15CFCC | confirmed |
| +0x6C | hfov_sin | int (16.16) | 12A46C mov [0CEA8Ch],ebx; 15CFD1 | confirmed |
| +0x70 | vfov_cos | int (16.16) | 12A484 mov [0CEA90h],eax; 15D004 | confirmed |
| +0x74 | vfov_sin | int (16.16) | 12A489 mov [0CEA94h],edx | confirmed |
| +0x78 | pick_skip_flats | unsigned char | 155513 test byte [0CEA98h],0FFh | strong |
| +0x79 | flat_scale_x | int | 12A435 mov [0CEA99h],eax ((half_width << 32) / focal_x^2 >> 1); snapshot build/xngine/bases/snap_cheat_dorian.snap: 8589934 | confirmed |
| +0x7D | flat_scale_y | int | 12A450 mov [0CEA9Dh],eax | confirmed |
| +0x81 | unknown_81 | int | no reference | candidate |
| +0x85 | recip_table | int * | 12A181 mov [0CEAA5h],eax (malloc 40004h); 12A21D [n] = 2^37 / (2^13 n) = 2^24/n; 12A245 (freed) | confirmed |

- Confidence: confirmed (asm; snapshot build/xngine/bases/snap_cheat_dorian.snap: near 2560, far 393216, 160/77/160/77, focal 200/180, inv_focal_x 51EB851Eh = 2^38/200, scale_x 20480, hfov 51200/40960).
- Open: +30h, +34h, +54h, +81h have no reader (+34h is zeroed each frame): leftovers of fields the engine dropped.

## struct xn_mouse_press (8 bytes)

- What: the last press of a mouse button, for double clicks.
- Instances: xn_mouse_state.press[2] at 0x12AF24 (left) and 0x12AF2C (right).
- Users: xn_mouse_poll 12B196.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | tick | unsigned int | 12B1E9 sub eax,[12AF24h]; 12B232 mov [12AF24h],edx (BIOS tick) | confirmed |
| +0x04 | x | short | 12B1FA mov ax,[12AF28h]; 12B23E | confirmed |
| +0x06 | y | short | 12B212 mov ax,[12AF2Ah]; 12B24A | confirmed |

- Confidence: confirmed.

## struct xn_mouse_state (828 bytes)

- What: the mouse and the 16x16 software cursor: an overlay of the names.csv globals (mouse_buttons, mouse_x, mouse_y, mouse_double_click, mouse_x_min.. are the game's names; the rest xn_mouse_*).
- Instances: 0x12AC00 (one).
- Users: xn_mouse_poll 12B196, xn_mouse_poll_clamped 12B136 (107 game sites), xn_mouse_cursor_move 12B2D3, _erase 12B2EB, _clip 12B326, _draw 12B3ED, xn_mouse_set_cursor_image 12B45B; the game reads buttons/x/y/double_click.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | buttons | unsigned char | 12B1AA mov [12AC00h],bl (int 33h 3) | confirmed |
| +0x01 | buttons_prev | unsigned char | 12B19E mov [12AC01h],al | confirmed |
| +0x02 | double_click | unsigned short | 12B1BE mov word [12AC02h],0; 12B22A or word [12AC02h],1; 12B2AC (2) | confirmed |
| +0x04 | x | short | 12B1B0 mov [12AC04h],cx; 12B162 (clamped) | confirmed |
| +0x06 | y | short | 12B1B7 mov [12AC06h],dx | confirmed |
| +0x08 | cursor_x | int | 12B35A mov [12AC08h],eax; 12B406 | confirmed |
| +0x0C | cursor_y | int | 12B39C mov [12AC0Ch],eax | confirmed |
| +0x10 | under | unsigned char[256] | 12B41F mov edi,12AC10h -> xn_draw_get_rect_regs; 12B318 (restore) | confirmed |
| +0x110 | clipped | unsigned char[256] | 12B3D2 mov edi,12AD10h; 12B44F | confirmed |
| +0x210 | image | unsigned char[256] | 12B460 mov edi,12AE10h (rep movsd 40h); 12B3D7 | confirmed |
| +0x310 | pad_310 | int | no reference | strong |
| +0x314 | cursor_w | short | 12B329 mov word [12AF14h],10h | confirmed |
| +0x316 | cursor_h | short | 12B332 mov word [12AF16h],10h | confirmed |
| +0x318 | hotspot_x | short | 12B46C mov [12AF18h],dx; 12B342; 12B381 (read for y: a bug) | confirmed |
| +0x31A | hotspot_y | short | 12B473 mov [12AF1Ah],bx; never read | confirmed |
| +0x31C | under_x | int | 12B426 mov [12AF1Ch],eax; 12B2FF | confirmed |
| +0x320 | under_y | int | 12B42B mov [12AF20h],edx | confirmed |
| +0x324 | press | struct xn_mouse_press[2] | 12B1E9 / 12B26B | confirmed |
| +0x334 | x_min | short | 12B142 cmp ax,[12AF34h] | confirmed |
| +0x336 | x_max | short | 12B153 cmp ax,[12AF36h] | confirmed |
| +0x338 | y_min | short | 12B16E cmp ax,[12AF38h] | confirmed |
| +0x33A | y_max | short | 12B17F cmp ax,[12AF3Ah] | confirmed |

- Confidence: confirmed (asm; snapshot build/xngine/bases/snap_cheat_dorian.snap: x 160, w/h 16, x range 0..319, y 0..199).
- Conflicts with include/: none (the game's globals are byte/short as declared here).

## struct xn_kbd_state (407 bytes)

- What: the keyboard handler's state and key tables (overlay of the xn_kbd_* globals and the game's key_down).
- Instances: 0x142300 (one); xn_kbd_install locks 0x397 bytes from it (the 512 bytes after the table, to 0x142697, are zero and unreferenced).
- Users: xn_kbd_install 142700, xn_kbd_remove 142764, xn_kbd_flush 142790, xn_kbd_read_key 1427A8, xn_kbd_wait_key 142808, the int 9 handler 142840..1428ED, the game's keys.c (key_down).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | old_int9_offset | unsigned int | 14271B mov [142300h],eax | confirmed |
| +0x04 | old_int9_selector | unsigned short | 142720 mov [142304h],dx | confirmed |
| +0x06 | installed | unsigned char | 14270A mov byte [142306h],1 | confirmed |
| +0x07 | last_scancode | unsigned char | 142892 mov [142307h],bl; 1428D4 (0 on break); 1427B2 | confirmed |
| +0x08 | key_down | unsigned char[128] | 142898 mov byte [ebx+142308h],1; 1428DB; 142791 (flush 20h dwords) | confirmed |
| +0x88 | shift_flags | unsigned int | 142867 or [142388h],8; 142873 (1); 14287F (2); 14288B (4); 1427C4 test [142388h],1 | confirmed |
| +0x8C | pad_8c | char[7] | no reference | strong |
| +0x93 | keymap | unsigned char * | 142757 mov [142393h],142397h; 1427BE add eax,[142393h] | confirmed |
| +0x97 | ascii | unsigned char[2][128] | 1427E6 mov al,[eax+ebx] (ebx = 80h with Shift) | confirmed |

- Confidence: confirmed (asm; snapshot build/xngine/bases/snap_cheat_dorian.snap: keymap = 142397h, the table 'ESC 1234567890-=').

## struct xn_joy_axes (8 bytes)

- What: an (x, y) pair of a stick's axes.
- Instances: xn_joy_state.center and .center_b.
- Users: xn_joy_calibrate 152C40, xn_joy_poll 152D00.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | center_x | int | 152C9D add [152A04h],eax; 152DD3 sub eax,[152A04h] | confirmed |
| +0x04 | center_y | int | 152CA8 add [152A08h],eax; 152E2E | confirmed |

- Confidence: confirmed.

## struct xn_joy_state (110 bytes)

- What: the joystick driver's state (port 201h, timed by an int 1Ch handler).
- Instances: 0x152A00 (one; 0x6E bytes locked by xn_joy_init 152B96).
- Users: xn_joy_init 152B00, xn_joy_shutdown 152BA8, xn_joy_reset_range 152BDC, xn_joy_calibrate 152C40, xn_joy_poll 152D00, xn_joy_timer_isr 152EA0..152F41; the game: options.c (status, dead zone, saves the 46 bytes from +4), keys.c (joystick_x/y, buttons).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | axis_mask | unsigned char | 152B4F mov [152A00h],ah | confirmed |
| +0x01 | installed | unsigned char | 152B0E mov byte [152A01h],1 | confirmed |
| +0x02 | status | unsigned char | 152B55 mov byte [152A02h],2; 152D29 cmp | confirmed |
| +0x04 | center | struct xn_joy_axes | 152C60 mov [152A04h],0 | confirmed |
| +0x0C | dead_zone | int | 152DF1 sub edx,[152A0Ch]; options.c 10/20/40 | confirmed |
| +0x10 | min_x | int | 152BDC mov [152A10h],7D00h; 152D63 | confirmed |
| +0x14 | max_x | int | 152D71 mov [152A14h],esi | confirmed |
| +0x18 | min_y | int | 152D7F mov [152A18h],edi | confirmed |
| +0x1C | max_y | int | 152D8D mov [152A1Ch],edi | confirmed |
| +0x20 | x | int | 152E21 mov [152A20h],eax (clamped -4095..4095) | confirmed |
| +0x24 | y | int | 152E7C mov [152A24h],eax | confirmed |
| +0x28 | raw_x | int | 152EFA mov [152A28h],esi | confirmed |
| +0x2C | raw_y | int | 152F00 mov [152A2Ch],edi | confirmed |
| +0x30 | button1 | unsigned char | 152F33 sete [152A30h] | confirmed |
| +0x31 | button2 | unsigned char | 152F2A sete [152A31h] | confirmed |
| +0x32 | pad_32 | short | 152C2C mov word [152A30h],0 clears both buttons | strong |
| +0x34 | center_b | struct xn_joy_axes | 152CB3 add [152A34h],eax | confirmed |
| +0x3C | pad_3c | int | no reference | strong |
| +0x40 | min_x_b | int | 152C04 mov [152A40h],7D00h; 152D9B | confirmed |
| +0x44 | max_x_b | int | 152DA9 | confirmed |
| +0x48 | min_y_b | int | 152DB7 | confirmed |
| +0x4C | max_y_b | int | 152DC5 | confirmed |
| +0x50 | x_b | int | 152D15 mov [152A50h],0 only | confirmed |
| +0x54 | y_b | int | 152D1F mov [152A54h],0 only | confirmed |
| +0x58 | raw_x_b | int | 152F06 mov [152A58h],ebx | confirmed |
| +0x5C | raw_y_b | int | 152F0C mov [152A5Ch],ecx | confirmed |
| +0x60 | button3 | unsigned char | 152F21 sete [152A60h] | confirmed |
| +0x61 | button4 | unsigned char | 152F18 sete [152A61h] | confirmed |
| +0x62 | old_int1c_sel | unsigned short | 152B67 mov [152A62h],dx | confirmed |
| +0x64 | old_int1c_off | unsigned int | 152B6E mov [152A64h],eax | confirmed |
| +0x68 | pad_68 | char[6] | no reference (inside the locked 6Eh) | strong |

- Confidence: confirmed (asm; snapshot build/xngine/bases/snap_cheat_dorian.snap: installed 1, status 2, centres 800h, minima 7D00h).
- Conflicts with include/: none; the game's xn_joy_calibration (char[46] in options.c) is +04h..+31h.

## struct xn_gfx_state (3158 bytes)

- What: the graphics driver's state: an overlay of the xn_gfx_* globals (and the game's pen_x/pen_y, screen_buffer).
- Instances: 0x14291B (one).
- Users: xn_gfx_set_mode 143600, xn_gfx_restore_mode 143700, xn_gfx_change_mode 14373F, xn_gfx_set_mode13 143870, xn_gfx_build_row_offsets 1438DC, xn_gfx_set_clip 1438FC, xn_gfx_clear 143914, xn_gfx_present 14395C and the driver routines, every 2D blit (clip and row table), xn_render_frame (rows, clip), xn_render_pick (clip), the font and line code, the VESA driver 15FC00..

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | mode | int | 14388E mov [14291Bh],13h | confirmed |
| +0x04 | saved_mode | int | 143889 mov [14291Fh],eax (int 10h 0Fh) | confirmed |
| +0x08 | driver | int | 1436E7 mov [142923h],0; 143701 mov eax,[142923h] | confirmed |
| +0x0C | pad_00c | unsigned char | no reference | strong |
| +0x0D | pen_x | int | 1532BD mov eax,[142928h]; 1532D4 | strong |
| +0x11 | pen_y | int | 1532C2 mov edx,[14292Ch]; 1532DA | strong |
| +0x15 | width | int | 14389F mov [142930h],140h | confirmed |
| +0x19 | height | int | 1438A9 mov [142934h],0C8h | confirmed |
| +0x1D | screen_size | int | 1438B3 mov [142938h],0FA00h | confirmed |
| +0x21 | page_count | int | 1436C3 mov [14293Ch],1 | confirmed |
| +0x25 | clip_left | int | 1438FC mov [142940h],eax; 12A60B | confirmed |
| +0x29 | clip_top | int | 143901 mov [142944h],edx | confirmed |
| +0x2D | clip_right | int (exclusive) | 143907 mov [142948h],ebx (= width); 12A617 cmp eax,[142948h]; jge out | confirmed |
| +0x31 | clip_bottom | int (exclusive) | 14390D mov [14294Ch],ecx | confirmed |
| +0x35 | row_offset | int[768] | 1438DF mov edi,142950h; ecx = 300h | confirmed |
| +0xC35 | screen_buffer | unsigned char * | 1436A6 mov [143550h],eax | confirmed |
| +0xC39 | buffer_base | unsigned char * | 1436AB mov [143554h],eax | confirmed |
| +0xC3D | buffer_alloc | void * | 14369B mov [143558h],eax; 14370C (freed) | confirmed |
| +0xC41 | present_mode | unsigned char | 143601 mov [14355Ch],dl; 14365D (3) | confirmed |
| +0xC42 | fill_scratch | int | only the dead FPU copy 1443D0 (names.csv) | candidate |
| +0xC46 | copy_clear_fn | void (*)(void) | names.csv xn_gfx_copy_clear_fn (143B80); snapshot build/xngine/bases/snap_cheat_dorian.snap | strong |
| +0xC4A | drv_shutdown | void (*[1])(void) | 143706 call [eax+143565h] | confirmed |
| +0xC4E | drv_present | void (*[1])(void) | names.csv; 14395C calls it | strong |
| +0xC52 | drv_clear | void (*[1])(void) | 14391B call [ebx+14356Dh] | confirmed |

- Confidence: confirmed (asm; snapshot build/xngine/bases/snap_cheat_dorian.snap: screen_buffer 1237040h, present 14396Ch).
- Conflicts with include/: none.

## struct xn_rm_regs (50 bytes)

- What: the DPMI real-mode call structure (int 31h AX=0300h; the DPMI standard layout).
- Instances: xn_gfx_vesa_rm_regs 0x15FA08 (VBE calls), xn_helmet_c_rm_regs 0x161300 (driver C's int 33h extension).
- Users: xn_gfx_vesa_init 15FC00 (15FC1A/15FC20 es, ds), xn_gfx_vesa_set_mode 15FCA7 (15FCBF ebx), the helmet driver C 161400...

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | edi | unsigned int | DPMI 0300h | strong |
| +0x04 | esi | unsigned int | DPMI | strong |
| +0x08 | ebp | unsigned int | DPMI | strong |
| +0x0C | reserved | unsigned int | DPMI | strong |
| +0x10 | ebx | unsigned int | 15FCBF mov eax,[15FA18h] | strong |
| +0x14 | edx | unsigned int | 15FA1C referenced twice | strong |
| +0x18 | ecx | unsigned int | 15FA20 | strong |
| +0x1C | eax | unsigned int | 15FA24 | strong |
| +0x20 | flags | unsigned short | DPMI | strong |
| +0x22 | es | unsigned short | 15FC20 mov [15FA2Ah],ax | strong |
| +0x24 | ds | unsigned short | 15FC1A mov [15FA2Ch],ax | strong |
| +0x26 | fs | unsigned short | DPMI | strong |
| +0x28 | gs | unsigned short | DPMI | strong |
| +0x2A | ip | unsigned short | DPMI | strong |
| +0x2C | cs | unsigned short | DPMI | strong |
| +0x2E | sp | unsigned short | DPMI | strong |
| +0x30 | ss | unsigned short | DPMI | strong |

- Confidence: strong (standard layout; the fields the code touches agree).

## struct xn_vesa_mode_info (256 bytes)

- What: VBE ModeInfoBlock (the VESA standard layout) as xn_gfx_vesa_set_mode copies it from the DOS buffer; only 13h runs in play (0 episodes).
- Instances: xn_gfx_vesa_mode_info 0x15FA6D.
- Users: xn_gfx_vesa_set_mode 15FCA7, xn_gfx_set_mode 143647 (image_pages).

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x12 | x_resolution | unsigned short | 15FD16 movzx eax,word [15FA7Fh] | strong |
| +0x14 | y_resolution | unsigned short | 15FD22 movzx edx,word [15FA81h] | strong |
| +0x1D | image_pages | unsigned char | 15FD5C inc byte [15FA8Ah]; 143647 | strong |
| +0x28 | phys_base | unsigned int | 15FCF7 cmp dword [15FA95h],0; 15FD45 | strong |

- Confidence: strong (standard; four fields read).

## struct xn_fnt_glyph (4 bytes)

- What: a glyph's entry in a FONT000n.FNT file.
- Instances: xn_fnt_file.glyphs.
- Users: xn_font_draw_string 12DBCC, the unlabelled 12DBA0, xn_font_glyph_width 12DD1C, the game's text width code.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | offset | unsigned short | 12DBF0 movzx esi,word [ecx+4] (entry + 4 = file + 4 + 4 (c - 21h)); add esi,[font_glyphs] | confirmed |
| +0x02 | width | unsigned short | 12DBFA movzx ebx,word [ecx+6]; 12DD2A | confirmed |

- Confidence: confirmed (asm and FONT0000.FNT: '!' = offset 964, width 3).

## struct xn_fnt_file (964 bytes)

- What: a FONT000n.FNT file, loaded whole by xn_font_load 12DB28 (xn_dos_load_file).
- Instances: xn_font_table 0x12DA54 (8 slots; slots 1-4 hold FONT0000-0003); font_glyphs 0x12DA74 = the selected one.
- Users: xn_font_select 12DB50 (+0, +2), xn_font_draw_string 12DBCC, xn_font_draw_glyph 12DC44 (rows), xn_font_glyph_width 12DD1C.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | space_width | unsigned short | 12DB6D movzx eax,word [esi] -> font_space_width | confirmed |
| +0x02 | height | unsigned short | 12DB75 movzx eax,word [esi+2] -> font_height | confirmed |
| +0x04 | glyphs | struct xn_fnt_glyph[240] | 12DBE7 shl ecx,2; add ecx,[font_glyphs]; [ecx+4] | confirmed |

- Confidence: confirmed (FONT0000.FNT: header 14, 11; glyph data from 964).
- Conflicts with include/: none in include/; DFU FntFile describes the same (its glyph table: 240 entries from character 33).
- Open: The glyph table's length (240) is read from the file (the first glyph's data offset); the engine only indexes characters 33..255.

## struct xn_font_state (64 bytes)

- What: the text drawing state (overlay of xn_font_* and the game's font_* globals).
- Instances: 0x12DA38 (one).
- Users: xn_font_init 12DB00, xn_font_select 12DB50, xn_font_draw_string 12DBCC, xn_font_glyph_width 12DD1C; the game sets font_char_spacing and reads the widths.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | text_x0 | int | 12DBD0 mov [12DA38h],eax; 12DC23 | confirmed |
| +0x04 | text_y0 | int | 12DBD5 mov [12DA3Ch],edx (never read) | strong |
| +0x08 | space_width | int | 12DB70 mov [12DA40h],eax | confirmed |
| +0x0C | height | int | 12DB79 mov [12DA44h],eax | confirmed |
| +0x10 | char_spacing | int | 12DB0F mov [12DA48h],1; 12DC05 | confirmed |
| +0x14 | line_gap | int | 12DB19 mov [12DA4Ch],1; 12DC35 | confirmed |
| +0x18 | current | int | 12DB5B mov [12DA50h],eax | confirmed |
| +0x1C | table | struct xn_fnt_file *[8] | 12DB01 (clears 8); 12DB45 mov [edx*4+12DA54h],eax | confirmed |
| +0x3C | glyphs | struct xn_fnt_file * | 12DB67 mov [12DA74h],esi | confirmed |

- Confidence: confirmed (snapshot build/xngine/bases/snap_cheat_dorian.snap: spacing 1, gap 1, current 4, table[1..4] set).

## struct xn_vid_header (15 bytes)

- What: a VID movie's file header.
- Instances: xn_vid_header 0xC1000 (read by xn_vid_open C173D: 0Fh bytes).
- Users: xn_vid_open C1654, xn_vid_decode_chunk C18A3 (+4), xn_vid_schedule_frame C1BBE (+0Bh), xn_vid_play C159C.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | magic | char[4] | the file ('VID'); not checked | strong |
| +0x04 | version | unsigned char | C1960 cmp byte [0C1004h],2 | strong |
| +0x05 | frames | unsigned short | C177A mov ax,[0C1005h] | confirmed |
| +0x07 | width | unsigned short | C1786 movzx eax,word [0C1007h] | confirmed |
| +0x09 | height | unsigned short | C1798 mov ax,[0C1009h] | confirmed |
| +0x0B | frame_delay | unsigned short | C1BBE add ax,[0C100Bh] | strong |
| +0x0D | flags | unsigned short | C176E mov ax,[0C100Dh]; C187F test [0C1460h],1 | strong |

- Confidence: strong (asm; all ANIM*.VID are VID, version 2, 320 x 200, flags 0Eh: part_A).

## struct xn_vid_player (1010 bytes)

- What: the VID player's state (overlay of the xn_vid_* globals after the header and the SOS sample descriptor).
- Instances: 0xC10FF (one; xn_vid_play locks 0xC1000..0xC14F0, 4F1h bytes, because the timer and sample callbacks run under interrupts).
- Users: xn_vid_play C1500, xn_vid_open C1654, xn_vid_update C17D6, xn_vid_finish C1844, xn_vid_decode_chunk C18A3, xn_vid_close_file C1B1A, xn_vid_refill C1B43, xn_vid_fill C1B9A, xn_vid_schedule_frame C1BBE, xn_vid_read_audio C1C4A, xn_vid_timer_cb C1CDC, xn_vid_audio_done_cb C1D00, xn_vid_audio_stop C1D4D, xn_vid_audio_start C1D6E.

| off | name | type | evidence | confidence |
|---|---|---|---|---|
| +0x00 | audio_state | unsigned char | C1670 mov byte [0C10FFh],0; C1BD5 inc; C17E0 cmp byte [0C10FFh],3 | confirmed |
| +0x01 | audio_buffer | unsigned char | C1CC7 inc byte [0C1100h]; and 1; C1C8E | strong |
| +0x02 | sample_handle | int | C1E00 mov [0C1101h],eax; C1D62 | strong |
| +0x06 | sample_rate | unsigned short | C1C1D mov [0C1105h],ax; C1DB7 | candidate |
| +0x08 | audio_delay | unsigned short | C1BFF mov [0C1107h],ax; C1C2F -> audio_ticks | candidate |
| +0x0A | audio_left | int | C1C56 mov [0C1109h],eax; C1C83 sub [0C1109h],ecx | strong |
| +0x0E | rate_byte | unsigned char | C1C08 mov [0C110Dh],al | candidate |
| +0x0F | audio_size | int | C1C51 mov [0C110Eh],eax; C1C63 | strong |
| +0x13 | audio_buf_a | unsigned char * | C169F mov [0C1112h],eax (4000h) | confirmed |
| +0x17 | audio_len_a | int | C1C9D mov [0C1116h],eax; C1D36 | strong |
| +0x1B | audio_buf_b | unsigned char * | C16B8 mov [0C111Ah],eax | confirmed |
| +0x1F | audio_len_b | int | C1CAA mov [0C111Eh],eax; C1D29 | strong |
| +0x23 | frame_waits | unsigned char | C19FB mov byte [0C1122h],1; C181A | candidate |
| +0x24 | sample_done | unsigned char | C1D41 mov byte [0C1123h],1; C17E9 | candidate |
| +0x25 | sample_restart | unsigned char | C1D12 mov byte [0C1124h],1; C17F9 | candidate |
| +0x26 | palette | unsigned char[768] | C1AEA mov edi,0C1125h; C160F (set at the end) | confirmed |
| +0x326 | dest_offset | int | C1758 mov [0C1425h],eax (row offset + x); C18AA | confirmed |
| +0x32A | frames_left | unsigned short | C1780 mov [0C1429h],ax; C19DD dec | confirmed |
| +0x32C | file | unsigned short | C1726 mov [0C142Bh],ax (int 21h 3Dh) | confirmed |
| +0x32E | file_pos | int | C1655 mov [0C142Dh],0; C1B64 add | strong |
| +0x332 | buf | unsigned char * | C167C mov [0C1431h],eax (big_buffer) | confirmed |
| +0x336 | read_ptr | unsigned char * | C18A4 mov esi,[0C1435h]; C19D0 | confirmed |
| +0x33A | buf_end | unsigned char * | C1686 mov [0C1439h],eax (+0FA00h) | confirmed |
| +0x33E | refill_mark | unsigned char * | C1690 mov [0C143Dh],eax (end - 81h); C192B | confirmed |
| +0x342 | frames_done | unsigned short | C17AA mov word [0C1441h],0; C19D6 inc | strong |
| +0x344 | x | int | C165F mov [0C1443h],eax; C1591 (clip) | confirmed |
| +0x348 | y | int | C1664 mov [0C1447h],edx | confirmed |
| +0x34C | path_arg | char * | C166A mov [0C144Bh],ebx; C1710 mov edx,[0C144Bh] (open) | strong |
| +0x350 | width | unsigned short | C1792 mov [0C144Fh],ax | strong |
| +0x352 | height | unsigned short | C17A4 mov [0C1451h],ax | strong |
| +0x354 | done | unsigned char | C1891 mov byte [0C1453h],1; C1601 | confirmed |
| +0x355 | dest_skip | int | C1769 mov [0C1454h],eax (screen width - width); C196C | confirmed |
| +0x359 | row_width | int | C178D mov [0C1458h],eax; C18C2 | confirmed |
| +0x35D | rows_left | unsigned short | C18BC mov [0C145Ch],ax; C1957 dec | confirmed |
| +0x35F | rows | unsigned short | C179E mov [0C145Eh],ax | confirmed |
| +0x361 | flags | unsigned short | C1774 mov [0C1460h],ax; C187F test 1 | strong |
| +0x363 | timer | int | C170B mov [0C1462h],eax (sound_timer_add); C1588 cmp -1 | confirmed |
| +0x367 | frame_ticks | short | C1BC5 add [0C1466h],ax; C1CDC dec (60 Hz); C1810 cmp 0 | confirmed |
| +0x369 | audio_ticks | unsigned short | C1C35 mov [0C1468h],ax; C1CED dec | strong |
| +0x36B | audio_waiting | unsigned short | C1BE6 mov word [0C146Ah],1; C1CE3 | strong |
| +0x36D | path | char[129] | C1548 mov edx,0C146Ch (xn_str_copy) | confirmed |
| +0x3EE | skippable | int | C1540 mov [0C14EDh],ecx; C15CF | confirmed |

- Confidence: strong (asm; the audio fields are candidates: the corpus has few VID records).
- Open: audio_state never goes from 2 to 3 when audio_ticks runs out (docs: VID audio). Scan the 7Ch chunks of ANIM*.VID for a non-zero delay.

## The camera matrices and view-ray tables (typed globals)

- `xn_cam_rotation` 0x136E00: `xn_mat3`, 2.28, from `xn_mat_from_angles(pitch, yaw, roll)`
  (137000). Its third row is what names.csv calls `xn_cam_forward_x/y/z` (0x136E18/1C/20):
  those three names are fields of this matrix, not separate globals.
- `xn_cam_view_matrix` 0x136E24: `xn_mat3`: `xn_cam_scale_matrix` (137725) copies
  xn_cam_rotation with rows 0 and 1 multiplied by scale_x / scale_y (2.14: `imul; shrd 14`),
  so the frustum's sides are x = +-z and y = +-z. Read by `xn_cam_cull_sphere` (15CF1B) and
  `xn_terrain_draw` (axis steps).
- `xn_cam_dir_x_table` 0x1202C0: `int [1024]`, x = -512..511 pixels from the centre:
  `(x << 18) / focal_x` (12A494..12A4B1); `xn_cam_dir_x_mid` 0x120AC0 = entry 0.
  `xn_cam_dir_y_table` 0x1212C0: `int [768]`, y = -384..383: `(y << 18) / focal_y`;
  `xn_cam_dir_y_mid` 0x1218C0. xn_render_frame patches dir_y_mid[row] into the lit span
  routines per row; xn_span_tex_lit reads dir_x_mid[x - centre].
- The shared scratch vectors at 0x120288 are the model section's `struct xn_scratch` (44
  bytes): `.a` = `xn_pick_view_x/y` and `pick_distance` (0x120288/8C/90: written by
  `xn_cam_cull_sphere` 15CF6B..15CF7E as the last culled sphere's view position, x and y
  scaled back by inv_scale; by `xn_render_pick` 12A68C..12A6BE as the picked point's view
  position; by `xn_flat_draw` 154EAB as a flat's), `.b` at 0x120294 (the pick's screen x/y at
  +0/+4, read by xn_render_pick 12A641/12A653 after `xn_flat_pick`; an edge vector in the
  terrain face planes and the model's u/v-axis code), `.flat` = `xn_pick_flat_x/y/z`. The
  game's `pick_distance` is `.a.z`: right after a pick, but whatever the last cull or flat
  left otherwise.

## big_buffer at run time

`big_buffer` (0x147954) is `xn_mem_init(102400)`'s block (149E00: malloc(size + 32, at least
64 KB), 32-aligned; `xn_mem_work_block` 0x147958 keeps the raw pointer): 102400 bytes that
every user treats as its own scratch for the duration of one call or one phase. Nothing
arbitrates; the users never overlap in time in the game's flow.

| User | Range | When |
|---|---|---|
| `xn_render_begin_frame` 12A5AE / the shader builders 15BCF6.. | from +0, 84/140/204 bytes per lit polygon (`xn_light_code_next`) | every 3D frame, from begin_frame to the end of xn_render_frame |
| `xn_water_clip_extent` 12F6B7.. | +0, 16 bytes (an `xn_poly_vertex` for the intersect routines) | after the frame |
| `xn_draw_image_scaled` C094C / C089E | +0: the compiled row (called) | one 2D blit |
| `xn_vid_open` C1677 | +0 .. +64000 (0xFA00): the read buffer | a movie |
| `xn_world_read_cell` C33BD | +0 .. +65536: a full cell's four 128 x 128 layers | world loads |
| `xn_world_unpack_cell` C3448 / `xn_world_gen_heightmap` C355D.. | +100h: the 129 x 129 work grid (16641 bytes) | world loads |
| `xn_tex_check_transparent` 135FFF / 13641D | +0: one animated frame at a 256-byte stride (up to 64 KB) | texture loads |
| `xn_collide_segment_model` 14A405, `xn_collide_spheres_model` 14AA92, `xn_collide_segment_flat` 14B1E0 | +0: {count, 30-byte hits} (include/structs.h collide_hits) | one collision query; the game reads it right after |
| `xn_model_push_player_from_pick` C811B | +0: a 36-byte matrix | building_exit |
| the game (names.csv): talk (+16384 topics, +90000, +95000), `rmb_block`, MAPNAMES, `disk_copy_file` (100 KB pieces), the save thumbnail (+24000, 4000 bytes), `xn_sky_copy_rows` (the sky image), the paperdoll background | as listed | their screens |

A C port can give each user its own buffer, except where two of them share data through it:
the collision hit list (the game reads `big_buffer` after the call) and the game's own users.

## Other typed globals

- Water (xn_12F500): `xn_water_jitter` 0x12DE18 `int [512]` (-1..1), `xn_water_row_velocity`
  0x12E618, `xn_water_row_phase` 0x12E938, `xn_water_row_base` 0x12EC58: `int [200]` each
  (per screen row, 12F538..12F598), `xn_water_unroll_offsets` 0x12EF78 `unsigned short [641]`,
  `xn_water_line_p0/p1` 0x12F47A/0x12F48A: `struct xn_poly_vertex` (outcode at +0Ch: 12F635),
  `xn_water_quadrant_dirs` 0x12F49A: 4 entries of 16 bytes indexed by the yaw's quadrant
  (12F5B2..12F5C4), `dungeon_water_level` 0x12DE00 `int` (10000 = none).
- System: `xn_recip16_table` 0x147980 `int [1024]` ([0] = FFFFh, [i] = FFFFh / i up to 1022,
  [1023] = 0), `xn_recip32_table` 0x148980 `unsigned int [1024]`, `xn_colour_fill_table`
  0x149980 `unsigned int [256]` (i * 01010101h), `xn_sel_bios_data` 0x147950 /
  `xn_sel_vga` 0x147952 `unsigned short`, `xn_sys_psp_addr` 0x147960 / `xn_sys_cmd_tail`
  0x14795C `unsigned int` (linear), `xn_mouse_cursor_drawn` 0x147964 `unsigned short` (bit 0).

## Globals typed by these structs

- `0x0C23B8 xn_cam_pitch`: `int  /* 0..2047; xn_cam_yaw +4, xn_cam_roll +8 */`
- `0x0C23C4 xn_cam_x`: `int  /* world units; xn_cam_y +4, xn_cam_z +8 */`
- `0x0C1000 xn_vid_header`: `struct xn_vid_header`
- `0x0C100F xn_vid_sample`: `struct sos_sample  /* include/structs.h; +5Ch the done callback */`
- `0x0C10FF xn_vid_audio_state`: `unsigned char  /* struct xn_vid_player at 0xC10FF */`
- `0x0CEA20 xn_cam_near_z`: `int  /* struct xn_view at 0xCEA20 */`
- `0x120288 xn_pick_view_x`: `int  /* struct xn_scratch at 0x120288 (model section): .a.x */`
- `0x1202C0 xn_cam_dir_x_table`: `int [1024]`
- `0x1212C0 xn_cam_dir_y_table`: `int [768]`
- `0x12AC00 mouse_buttons`: `unsigned char  /* struct xn_mouse_state at 0x12AC00 */`
- `0x12DA00 xn_font_filename`: `char [13]  /* "FONT0000.FNT" */`
- `0x12DA54 xn_font_table`: `struct xn_fnt_file *[8]`
- `0x12DA74 font_glyphs`: `struct xn_fnt_file *`
- `0x136E00 xn_cam_rotation`: `xn_mat3`
- `0x136E24 xn_cam_view_matrix`: `xn_mat3`
- `0x142300 xn_kbd_old_int9_offset`: `unsigned int  /* struct xn_kbd_state at 0x142300 */`
- `0x142308 key_down`: `unsigned char [128]`
- `0x142950 xn_gfx_row_offset`: `int [768]`
- `0x143550 screen_buffer`: `unsigned char *`
- `0x147954 big_buffer`: `unsigned char *  /* 102400 bytes */`
- `0x152A00 xn_joy_axis_mask`: `unsigned char  /* struct xn_joy_state at 0x152A00 */`
- `0x15FA08 xn_gfx_vesa_rm_regs`: `struct xn_rm_regs`
- `0x15FA6D xn_gfx_vesa_mode_info`: `struct xn_vesa_mode_info`
- `0x161300 xn_helmet_c_rm_regs`: `struct xn_rm_regs`

## Notes across subsystems

- The model section's xn_model_matrix_slot is what xn_view.matrix_next points at.
- xn_vid_sample (0xC100F) is include/structs.h struct sos_sample (240 bytes); the VID code
  also writes +5Ch (C1DEC: the sample-done callback xn_vid_audio_done_cb), which structs.h
  leaves in pad48: worth a field there.
