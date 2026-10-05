# The XnGine engine map

`build/names/xngine.csv` names every function of object 2 that `config/names.csv` does not already
name: **715 functions** (112 confirmed, 410 strong, 193 candidate). With the 4 game thunks already
named (`spell_cost_formula_dispatch` 0xCAE07, `spell_effect_dispatch` 0xCAE0E,
`spell_find_effect_type` 0xCE4C4, `spell_has_no_effects` 0xCE4E0), all 719 functions of
`config/xngine_functions.csv` have a name. It also names **481 globals and tables** (28 confirmed,
374 strong, 79 candidate). Every function name is `xn_<subsystem>_<what>`.

How the names were made: per-function dossiers (callers by game name, with their C call sites;
XnGine callers; data tables; callees; globals read and written; strings; ports and interrupts;
patch fields; play evidence; the asm) were built by `build/names/scratch/xngine/dossier.py`, and six
agents each read the asm of one address range (parts A-F; their notes are
`build/names/scratch/xngine/part_?.md` and hold more detail than this page).
`finalize.py` resolves the parts: it drops duplicate globals, qualifies duplicate function names
and folds in the experiments. `merge_parts.py` checks the result. There was one experiment,
`fallcall save_wayrest 0x12A254 0/4` with screenshots (below).

**Validation.** `merge_parts.py` merges the CSV into a copy of `config/names.csv`
(`scratch/xngine/names_copy.csv`) with `tools/names.py` (`load`, `merge`, `check`):

- The merge adds 1196 rows with 0 clashes, and no existing name or address is renamed.
- `names.check` then reports exactly **371 errors**, all "no function at": `check` accepts a
  function only if its address is in `config/functions.csv`, and that file lists only 348 of the
  719 XnGine functions.
- With a `functions.csv` that also lists the other 371 (a scratch copy), `check` reports **0
  errors**.
- **The coordinator must add the 371 XnGine functions to `config/functions.csv` before merging**,
  or teach `check` to accept addresses from `config/xngine_functions.csv`.

## Subsystems

| Word | Funcs | Where | What |
|---|---|---|---|
| anim | 20 | C0100 (+ table C0000) | ASCR monster animation-script interpreter: 12 opcodes, a 16-bit LCG |
| sys | 42 | C0500, C7F00, CE8C4, 149F4C-14A0CF | int 24h critical-error handler (always retry), DPMI divide-error handler (38 ModRM blocks), zero-page check, time-slice yield, CauseWay transfer buffer |
| draw | 60 | C0700, CB300-CE63F, 144D00-14501C, 153100, 15E400 | 2D blits on `screen_buffer`: scaled images (row code compiled into big_buffer), opaque/transparent images, rects, lines, UI effects |
| img | 7 | C0A36, C8017, CB381.. | RLE / row-table image decode, colour remap |
| dos | 23 | C0C00 | DOS file I/O: register-convention cores and unused C wrappers, load whole file, ARENA2 path, the 'DOS:' fatal tail |
| vid | 14 | C1500 | VID movie player: chunks (frames, palette, Creative-style audio on HMI SOS), 60 Hz pacing |
| vec, mat, math | 62 | C2000, C7FD9-C82C2, CE6D4-CE74F, 137000, 14BC00, 15D100, 15F50C | vectors, rotation matrices (2048-step angles, 2.28 sin/cos tables at 0x150200/0x150A00), sqrt, asin/acos, the game's distance and atan2 helpers |
| world | 35 | C2D00 | WOODS.WLD: 2x2 window of 256x256 layers around the eye, streaming, procedural heightmap and tiles |
| rand | 6 | C5200, 153978 | value noise (light flicker), LCG seed*797 mod 4099 |
| model, sky, shade, bits, str, timer, mouse, kbd, pal, gfx, input, spell | | C7F00-CE9xx | Daggerfall-specific asm helpers that the game C calls one at a time (none is in the 3D pipeline) |
| render | 23 | 12A100, 158100 | engine init/shutdown, render modes, begin frame, the frame renderer, pick, per-polygon setup, sorting |
| cam | 13 | 12A274-12A3AC, 1376C8.., 15CF18 | view window, focal lengths, derived projection constants, frustum sphere cull |
| mouse | 15 | 12B100, CE69A.. | int 33h poll and clamp, double-click, 16x16 software cursor |
| pal | 14 | 12D400, CD33A.. | VGA DAC set/get with retrace, fade, nearest colour |
| font | 8 | 12DB00, 15375C | FONT000n loading into slots, select, draw string/glyph |
| water | 6 | 12F500 | dungeon water post-process (ripple, `water.tbl`) |
| tex | 19 | 135D00, CDC99 | 'SET:' TEXTURE.nnn cache: LRU heap, decoded animation frames, compiled mapper per texture |
| light | 21 | 136A00, 15BC42-15C08C | 32 lights/frame, light.dat falloff, lighting shaders compiled from 3 templates |
| shade | 10 | 136B88, 14D200, 150040, C9EA7.. | SHADE.nnn tables, distance fog (unrolled remap with a planted ret), translucency table |
| terrain | 23 | 13E600, 14B45B | outdoor ground: 32x32 vertex grid, 31x31 cells, nature billboards, ground height |
| model | 27 | 13FE00 | ARCH3D model preparation (load time) and drawing (queue, occlusion, transform, faces) |
| kbd | 22 | 142700 | int 9 handler (15 run-time blocks), key_down, read key |
| gfx | 30 | 143600, 15FC00, CD308.. | mode 13h / VESA, back buffer, present, clear, retrace, the unused VESA driver |
| mem | 4 | 149E00 | 'SYSTEM:' work memory (big_buffer), DPMI lock, reciprocal and fill tables |
| collide | 37 | 14A300, 14BC00.., 15CB00 | segment/sphere-set vs ARCH3D model and flat; most primitives are dead |
| joy | 10 | 152B00 | joystick on port 201h timed by an int 1Ch handler |
| helmet, serial | 44 | 153500, 160900-161568 | three serial head-tracker (VR helmet) drivers on an 8250 UART driver; never run |
| flat | 18 | 154D00 | billboard sprites: add, sort, project, depth-clip against the world spans |
| span | 29 | 155800-157FA0, 158D04 | span routines (solid / textured 8 or 16 px / 64x64 terrain / flat sprites) and the S-buffer insert |
| poly | 19 | 158300-15BC41 | outcodes, Sutherland-Hodgman clip, the three projectors, edge walking, texture gradients |
| tmap | 6 | 15C200 | texture-mapper code generator: pool, compile, rebase, the 418-byte template |

## Startup and shutdown

- **main** (src/hand/func_00010010.c):
  1. Startup: `xn_sys_install_crit_error_handler` (C0520), then `init_video`.
  2. Once: `xn_render_set_mode(8)` and `xn_sys_zero_page_save` (CE8C4).
  3. Each loop iteration:
     - `game_frame`
     - `xn_timer_fps_update` (CE7BB; it fills `xn_timer_fps`)
     - `xn_mouse_read_motion` (12B47E)
     - `keys_world_actions`
     - `text_shadow_colour = D_00196283`
     - `xn_gfx_present_inclusive(1)` (CDD81)
     - `xn_sys_yield` (C7F00: int 2Fh 1680h)
- **init_video**, in order:
  1. `xn_kbd_numlock_off`
  2. `xn_timer_wait_ticks(18)`
  3. `xn_sys_set_dos_transfer_buffer` (C9F29: CauseWay FF26h)
  4. `xn_mouse_get_sensitivity`
  5. `xn_mem_init(102400)`: big_buffer, the reciprocal tables 0x147980/0x148980, the fill table
     0x149980
  6. `xn_gfx_set_mode(19,1)`: mode 13h or VESA, back buffer, clip, row table
  7. `xn_kbd_install`
  8. `xn_mouse_init`
  9. `xn_mouse_set_range_320x200`
  10. `xn_font_init`
  11. `xn_render_init`: installs the divide handler, builds the squares and size-mask tables, and
      allocates the 1/z table, patching its address into 25 span routines
  12. `xn_cam_set_view_window(160,77,160,77)`
  13. `xn_cam_set_focal(200,180)`
  14. `xn_light_init`: light.dat
  15. `xn_anim_rand_seed`
  16. The game sets `xn_light_ambient`, `xn_cam_near_z` = 2560 and `xn_cam_far_z` = 393216
  17. `xn_render_set_mode(8)`
  18. `xn_shade_load(0)`: SHADE.000
  19. `xn_shade_keep_colours_0_255`
  20. `xn_world_init`
  21. `xn_world_open` (WOODS.WLD)
- **init_game_data**:
  - `xn_font_load` x4 (FONT0000-0003)
  - `xn_water_init`
  - `xn_tex_cache_init(cfg_texture_memory << 10)`, which also allocates the tmap pool
  - `xn_rand_noise_init`
  - `xn_joy_init`
  - `xn_helmet_open(1, cfg_helmet == 1 ? 4 : 0)`, only when `cfg_helmet` != 0
  - the snow and translucency tables
- **Shutdown:**
  - `shutdown_video`: `xn_sys_restore_crit_error_handler`, `xn_kbd_remove`, `xn_gfx_restore_mode`
  - `shutdown_free_all`: `xn_render_shutdown` (12A230), which frees lights, textures and the world
    and removes the divide handler, then `xn_joy_shutdown`
  - every XnGine fatal path ('DOS:', 'SET:', 'XnGine: Object ...', 'SYSTEM:') runs the same teardown
    before printing and exiting

## The frame

`game_frame` calls `xn_sky_snow_update_speeds` (C9C70) and then, through `world_render`
(src/lifted/main.c), drives the 3D view. The game sets the eye `xn_cam_x/y/z` (C23C4/C8/CC) and the
angles `xn_cam_pitch/yaw/roll` (C23B8/BC/C0; 2048 = 360 degrees), then:

1. **`xn_tex_cache_begin_frame`** (135E90): clears the per-archive use counts and the decoded-frame buffer.
2. **`xn_render_begin_frame`** (12A4F0): resets the pools:
   - matrices 0xD7AC0, polygons 0xDB320 (100-byte records), light lists 0x116C80
   - the model queue 0x13F784 and the flat sort list 0x153C44
   - every screen row's span list (heads at 0xF39F0) points at the sentinel span 0xF39E0
   - light count 0; the shader pool is big_buffer
   - patches the ambient shade row and the subdivision shift into the span setup
3. **`xn_light_reset`** (136AB4).
4. **`xn_mat_from_angles`**(pitch, yaw, roll, `xn_cam_rotation`) and **`xn_cam_scale_matrix`**
   (`xn_cam_view_matrix`), so the frustum sides are x = +-z and y = +-z.
5. **`world_draw_objects`** (game C) runs `object_draw_cb` per object. That calls:
   - `xn_model_submit` (1401D4): sphere cull, then queue {distance, handle}; it does **not** draw
   - `xn_flat_add` (154D00): view transform, cull, a polygon record, the sort list
   - `xn_light_add` (136AD8)
   - `xn_anim_update` (C013B) for creatures
   - `xn_tex_cache_lookup_image` (135DE4) for flat sizes
6. **Outdoors** also:
   - `xn_world_update` (C2E05): streams WOODS.WLD cells when the eye nears the window edge
   - `xn_terrain_draw` (13E600): axis steps (137657), height tables, `xn_terrain_transform_grid`
     (32x32 vertices, 20 patch fields), nature billboards (`xn_terrain_add_nature_flats` -> 154DA5),
     `xn_terrain_draw_cells` (31x31 cells as a quad or two triangles, texture per cell from the
     ground archive), then the directional light list
7. **`xn_render_frame(2 outdoors / 0 inside)`** (12A870):
   1. returns 1 at once if `xn_tex_cache_full` is set
   2. `xn_render_draw_models`: sort the queue with `xn_render_sort_pairs`, then per model
      `xn_model_draw` (140284):
      - occlusion test against the span lists (`xn_model_is_occluded`)
      - object x camera matrix into the matrix pool, and the eye in object space into patch fields
      - faces (`xn_model_draw_faces` -> `xn_model_transform_face_verts` -> `xn_poly_project_face`)
      - light list (`xn_model_build_light_list`)
   3. `xn_render_fill_background` (indoors): fills the pixels no span covers with the byte at
      0x12B504
   4. **the span pass:** rows from the clip top. Each row:
      - patch the row's view ray (`xn_cam_dir_y_mid`) into the lit span routines
      - for each span node call `[poly+3Ch]`, then `[xn_render_span_hook]` (the fog:
        `xn_shade_fog_span` or the empty 14D300)
      - the sentinel's routine `xn_render_frame_row_end` ends the row
   5. `xn_render_draw_flats`: `xn_flat_begin_frame`, lights to view space (136BD8), sort far to
      near, then `xn_flat_draw` each sprite over the finished rows (depth-clipped per row against the
      world spans by `xn_flat_span_clip`)
   6. returns 0, or 1 on failure
8. On a non-zero result, or `xn_tex_cache_full` (the texture heap or the tmap pool overflowed), the
   game calls `xn_tex_cache_flush` (135E39) and draws the frame again. A second failure is fatal.
9. When the eye is not at `dungeon_water_level`, `xn_water_draw` (12F79C) post-processes the
   pixels behind the water plane.
10. Later in `game_frame`, the HUD and UI draw over `screen_buffer` with the 2D blits
    (`xn_draw_image` 144F68, `xn_draw_image_transparent` 144FB4, `xn_font_draw_string` 12DBCC ...).
    Weather draws there too: `xn_sky_draw_snow` C9A89 and `xn_sky_draw_rain` C9CB9.
11. main presents: `xn_gfx_present_inclusive` -> `xn_gfx_present` (14395C) -> the driver slot
    `xn_gfx_drv_present` (= 14396C), which copies the back buffer to A0000h (`xn_gfx_copy_rows` or
    `xn_gfx_copy_and_clear`), or flips VESA pages.

### The renderer pipeline (world/objects -> polygons -> spans -> pixels)

**1. Geometry to polygons.**
- Terrain cells, model faces and flats each have a projector:
  - `xn_poly_project_terrain` 1585C0
  - `xn_poly_project_face` 158420
  - `xn_poly_project_flat` 158360
- **Clipping.** Each projector clips with the outcodes of `xn_poly_outcode`: 1 x<-z, 2 x>z, 4 y>z,
  8 y<-z, 10h z<near, 20h z>far. `xn_poly_clip_frustum` then runs one Sutherland-Hodgman pass per
  plane present, in the order near, far, left, bottom, right, top.
- **Projection.** It stores 1/z = 2^40/z, with screen x = cx + x*sx/z (5 fraction bits). The view
  window patches sx, sy, cx and cy into each projector's immediates.

**2. Polygons to spans: an S-buffer.**
- `xn_poly_rasterize` (15B9A0) walks the polygon's two edges, using the reciprocal tables for the
  slopes.
- Per row it calls `xn_span_insert` (158D04). That keeps a sorted, non-overlapping list of 16-byte
  span nodes per row: {next, x_end, x_start, 1/z at x_start, polygon}.
- A new span is trimmed or split against the existing ones by 1/z: the nearer one wins.
- Nothing is drawn until every model and the terrain have been inserted, so each pixel is written
  once (flats excepted).

**3. Lazy polygon setup.**
- A polygon's `[+3Ch]` starts as a setup routine from `xn_render_span_setups` (12A009). The render
  mode copies that table (`xn_render_set_mode`: 0 outline, 4 solid colours, 8 textured).
- On the polygon's first span the setup does three things:
  - lights the polygon: `xn_light_setup_poly` 15BC42, which divides 2^32 by `[poly+5Ch]`. That
    divisor is 0 for walls seen straight on, so the divide-error handler returns 0 about 10 times a
    frame.
  - computes the texture gradients (`xn_poly_setup_textured` 15BB8C, `xn_poly_tex_gradients`)
  - picks the real span routine, stores it in `[+3Ch]` and jumps to it
- The lighting result is an index:

  | Index | Light result | Span routine |
  |---|---|---|
  | 0 | saturated | unshaded |
  | 4 | one constant shade row | shaded |
  | 8 | 1-3 point lights | a per-pixel shader compiled from the templates 136C30/136C84/136D10 into big_buffer |

**4. Span routine families.**

| Table | Polygon type | Routines |
|---|---|---|
| `xn_render_solid_span_fns` | solid colour | 155800 / 155820 / 155920 |
| `xn_render_tmap_span_fns` | textured, 8 or 16 px (16 when d(1/z)/dx is small) | `xn_span_tex_8/16`, `xn_span_tex_shaded_8/16`, `xn_span_tex_lit` |
| `xn_render_terrain_span_fns` | 64x64 terrain textures | `xn_span_tex64`, `xn_span_tex64_shaded` |
| set by `xn_flat_span_light_setup` | flats | `xn_span_flat_*` (colour 0 transparent; fogged; translucent) |

- Every perspective routine divides at each 8 or 16 px boundary and fills between boundaries
  affinely.
- The last partial run goes into an unrolled loop stopped by a **planted `ret`**: the offset tables
  `xn_span_tail_offsets_*` give where to plant it, and the original opcode is restored after the
  call.

**5. The texture mapper copies.**
- `xn_tmap_compile` (15C274) patches the 418-byte `xn_tmap_template` (15C300) with one texture's
  wrap mask and texel base and copies it into `xn_tmap_pool`: 768 slots, the engine's "Shaders".
- `xn_tmap_rebase` re-patches a copy when its texture moves in the heap.
- `xn_span_tex_lit` (156A94) calls the copy (`xn_span_tex_lit_tmap`) 16 pixels at a time, with
  the shade from the polygon's light shader at each boundary. For the last run it plants a `ret`
  at `xn_tmap_ret_offsets[n]` and puts the 0Fh back afterwards.
- 156A94 ran in 334 of the traced episodes, so the generated copies do run in play.

**6. Fog.** `xn_shade_set_fog` (14D23C, from `update_fog`) points `xn_render_span_hook` at
`xn_shade_fog_span` (150040). That routine remaps the span's pixels through the 64-level fog table,
using a 641-step unrolled loop with a planted `ret`. Flats apply the same fog in their own span
routines.

**7. Pick.** `xn_render_pick` (12A608, from `engine_pick_object`) finds the span under a screen
point (and `xn_flat_pick` for sprites) and returns its polygon.

### Self-modifying code, by purpose

The meaning of each patch field is listed in the part notes (A: C0700; C: the 12A100 writers;
D: terrain and model fields; E: fog; F: span, tmap and setup fields).

- **View constants.**
  - `xn_cam_set_view_window` writes the half width, half height and centre into the projectors and
    the flat/terrain/model code.
  - `xn_cam_set_focal` and `xn_cam_update_derived` write the focal and scale reciprocals.
  - `xn_render_init` writes the 1/z table address into 25 span routines.
- **Per frame.**
  - `xn_render_begin_frame`: the ambient shade row and the subdivision shift.
  - `xn_render_frame`: the per-row view ray, and the screen width and end row.
  - the terrain grid walker: its increments.
  - `xn_model_draw`: the eye in object space and the matrix rows.
  - `xn_shade_set_fog`: the fog constants.
- **Per span.** Loop ends, steps, fixed colour bytes, and the planted `ret`s:
  - span tails and the tmap copies
  - fog
  - water (12F9E0)
  - the transparent image row (14501C)
  - the model's vertex-flag clear (140A28)
  - the rain streak (C9D89)
- **Generated code.**
  - the tmap copies
  - the lighting shaders (templates 136C30.. patched by 15BCF6/15BD78/15BE4D)
  - `xn_draw_image_scaled`'s compiled row (C094A)

## Data structures (see the part notes for the full layouts)

- **Polygon record** (100 bytes from `xn_render_poly_pool`): +0 face/material, +4 model handle
  (+4 its light list), +14h shade row or shader code, +18h packed texture origin, +24h..38h u/z and
  v/z gradients, +3Ch span routine, +40h texels or colour, +44h compiled mapper, +4Ch wrap mask,
  +50h..58h 8-px steps, +5Ch d(1/z)/dx, +60h its reciprocal.
- **Span node** (16 bytes): next, x_end, x_start, 1/z at x_start, polygon.
- **Light** (29 bytes, 32 at `xn_light_table` 0x136540): x, y, z, radius (-1 free), falloff terms,
  type (0 point, 4 ignored, 8 directional).
- **Model handle** (game object +4): model, light list, matrix slot, eye-relative and world
  position, angles, frame, flags.
- **Model data** is the ARCH3D record: version, counts, radius, frames, plane data, points, normals,
  faces, and collision spheres at +1Ch/+20h.
- **Texture heap block** (0x16-byte header, LRU by BIOS tick) and the **loaded archive**: 20-byte
  record entries; image record: wrap masks, w, h, flags, compiled copy at +0Ah, frames at +1Ch.
- **Anim state** (monster +0x2C1): frame, script, position, tick divisor (the game calls it
  `frame_count`), events, wait, request, facing, group, state.
- **WORLD**:
  - WOODS.WLD: a 90h header, then 1000x500 cells of 47-byte records (seed, archives, climate,
    5x5 elevation).
  - In memory: four 256x256 layers, `xn_world_height_layer` .. `xn_world_water_layer`.

## Unsure, and what would settle it

1. **Light type 4** is ignored by both light dispatchers. What it is meant for is unknown. Record
   15BC42 in a torch-lit dungeon and dump `[light+17h]`.
2. **Water.** `xn_water_draw` (12F79C) and its helpers have 0 episodes, so their names come from
   the asm alone. Run a dungeon with water, with the eye above and then below the surface.
3. **Flats: the first visible piece is skipped.** `xn_flat_span_light_setup` (155610) is the first
   routine `[flat+3Ch]` holds. It installs the real routine and returns without drawing, and
   `xn_flat_span_emit` (15526C) is its only caller. So the first visible piece of every flat is
   not drawn. I checked this in the asm; a recording of 15526C would show the cost.
4. **Translucent flats.** Is 157E20 used? CDC99 marks archives 273/278 (ghost, wraith) translucent
   through `D_00136921`, which nothing writes, and 157E20 has 0 episodes. Dump `D_00136921` after
   init_game_data and draw a ghost.
5. **Head trackers.** The three drivers can be told apart only by their command bytes (driver A
   sends 'R', 'G', 'H'; B sends '!R', '!M2,C,B', '!V', 'S'; C uses int 33h 60xxh), and the asm does
   not name the products.
   - Driver A's read slot returns no angles.
   - Nothing in the frame polls the tracker (153550 and 15356C have no caller), yet
     `cast_fire_missile` adds `xn_helmet_pitch`/`yaw` and `runspell.c` adds them to the view.
6. **Model internals.** What `xn_model_scale_matrix`'s second copy (+0x1C20) feeds, and whether
   `xn_model_is_occluded` is a true occlusion test, need a recording of 140284 on a building.
7. **Angle conventions.** `xn_vec_dir_to_angles` (C2068): which output is pitch and which yaw. The
   game's `missile_yaw` (+17h) gets the y-derived angle, so the game name may be wrong. Also
   unchecked: `xn_math_angle_xy` (14BEE2), and the u/v labels of the terrain axis setters
   (13F4CC..13F608; their row 2 is (-x, -x)).
8. **VID audio.** The audio state never moves from 2 to 3 when a delay runs out. Scan the 7Ch
   chunks of ANIM*.VID for a non-zero delay.
9. **Weather kinds.** Snow = kind 5 and rain = kind 4 are read from the drawing code. Set
   `region_precipitation_override` to check.
10. **Smaller questions.**
    - What `xn_model_push_player_from_pick` (C810C, building_exit) pushes along.
    - Why model 610 ORs 3 into its base angles.
    - Who reads `xn_world_water_layer` (C28C8).
    - What tile 2Eh is.
    - What 0x149E00 locks at 0xA12B8 (object 1).

**Existing names that may be too narrow** (left unchanged, for the coordinator):
- `text_shadow_colour` (0x12B504) is also the 3D view's background fill (12A98B) and the colour
  the back buffer is cleared to; main sets it from D_00196283 before every present.
- `pick_distance` (0x120290) is the z of XnGine's shared scratch vector (written by the sphere cull
  15CF18 and the flat setup); the game reads whatever the last culled object left there.
- `cfg_helmet` (0x153404) is overwritten by `xn_helmet_open` with the driver offset, or -1.
- The anim struct's `frame_count` (+0Ch) is the animation's tick divisor.

## Dead code

**139 functions have no caller of any kind** (no call, jump, pointer or game call) and never ran in
the traced play; `build/names/scratch/xngine/dead.md` lists them by subsystem. A few of them are
reached by a fall-through or a jump into their body: C9D63, 155332, 15BA16, 1609F9. The groups:

- **XnGine's general library, linked in whole:**
  - the DOS C wrappers (`xn_dos_*_c`), find first/next, create, write, file size
  - string helpers (case conversion, insert/delete char, length)
  - alternate matrix, vector and math routines; HSV palette code
  - the world editor (`xn_world_create_file`, `_write_header`, `_write_cell`, `_random_elevations`)
  - most collision primitives, including a model-vs-model test whose detailed body is unreachable,
    and `xn_collide_build_model_spheres` (tool code)
  - the BIOS cursor wrappers, `xn_gfx_change_mode`, `xn_kbd_wait_key`
- **Disabled features:**
  - star rotation and drawing (`xn_sky_draw_stars`, `_rotate_stars`)
  - the world generator's path and river passes: a `ret` at C3DA8/C3E69, with the bodies at
    C3DA9/C3E6A
  - render modes 0 and 4: 12A7A0 and 12A860 run only when the mode is changed by hand
- **Never run, but reachable:** the VESA driver (the game runs in mode 13h), the serial and helmet
  drivers, `xn_span_flat_translucent`, and `xn_tmap_template` itself (only its copies run).
- **Not code at all:**
  - 157B02 and 157E02 are entries of offset tables
  - 160F00, 160F04 and 160F11 are driver B's command strings
  - a few lone `ret`s and stubs

**Code missing from the function map** has no rows (seeding these in `config/xngine_seeds.csv`
would make them functions). None of it is referenced:
- PIT programming at 0x12AB00, 0x12AB50 and 0x12AB68 (it explains `xn_timer_fps`), and lone `ret`s
  at 0x12AB91/95/98
- an unlabelled draw-one-character routine at 0x12DBA0
- the VGA display start at 0x143A4B
- a benchmark at 0x143A5E that times three copy-and-clear routines with the Zen timer, and the FPU
  copy at 0x1443D0 (named only as a global)
- 0xCE34E, an octant atan at 0xCE35A-0xCE3E2, a remap template at 0xCE674, and a mouse reset at
  0xCE691
- Abrash's Zen timer itself at 0x15F800-0x15F98A (suggested names in part_F.md)

## Bugs and quirks found (static reading)

- **Texture eviction never fails.** `xn_tex_heap_evict` (136145) does `stc` and falls into `clc`,
  so "SET: Out of memory in find_memory." can never print. A full cache sets the flush-and-redraw
  flag instead.
- **Pick.** `xn_render_pick` unbalances the stack when the point is outside the clip rect. The game
  checks the point first, so this is not reached.
- **Drawing.**
  - `xn_mouse_cursor_clip` uses the x hotspot for y.
  - `xn_font_glyph_width` treats '!' as a space.
  - `xn_draw_image_scaled` always uses a 256-byte source stride.
- **Seek.** `xn_world_read_height_bands` seeks without setting AL. This is the known seek-mode
  note in docs/xngine.md.
- **Collision.**
  - `collide_sphere_cb` passes scale 0 to `xn_collide_segment_flat_stk`, so flat collision is
    effectively off.
  - When a flat is culled, `xn_flat_add` returns its scale argument instead of 0.
- **The divide handler** handles no SIB forms (ModRM 74h, 7Ch, B4h, BCh); a fault there would be
  stepped over as 2 bytes.
- **Dead code:**
  - ASCR opcode 5 stores an offset where a pointer belongs.
  - 153978 reads 0x0040006C as a linear address.
  - 14C25F's z term uses the wrong offset.
  - 1373FE, 13742B, 12D6AB and C8054 have stack, field or register-flow bugs.

## Entry points from the game, by subsystem

There are 174 XnGine functions with game C callers. The table lists them, with the number of C
call sites and some callers.

| Subsystem | Entry points (address name: game C sites; example callers) |
|---|---|
| draw (25) | `C0700` xn_draw_image_scaled (4: inv_draw_cart_cell, inv_draw_cell_mark ...); `CB34E` xn_draw_copy_rect_stride (1: options_joystick_draw); `CB39A` xn_draw_cif_rle_frame (4: player_frame_update, weapon_bow_update ...); `CB473` xn_draw_img_masked_remap (1: weapon_player_update); `CB552` xn_draw_fullscreen_overlay_shaded (9: itemmaker_update, note_update ...); `CD0F1` xn_draw_darken_rect (1: info_popup_update); `CD126` xn_draw_cel_frame (1: class_question_scroll_step); `CD1C5` xn_draw_image_drop_shadow (2: potionmaker_ingredient_cb, potionmaker_update); `CD20E` xn_draw_spell_icon (4: spell_hud_draw_icons, spellbook_draw_spell ...); `CD262` xn_draw_paperdoll_mask (1: paperdoll_draw_item); `CD291` xn_draw_paperdoll_item (1: paperdoll_draw_item); `CD53C` xn_draw_view_checkerboard (1: damage_player_hurt); `CDB7A` xn_draw_cast_anim_mirrored (2: cast_anim_update); `CDCB8` xn_draw_image_masked_at_origin (1: people_debug_map); `CDD49` xn_draw_zoom4x (1: travel_draw_zoom); `CDD6C` xn_draw_mark_matching (1: travel_map_update); `CE31C` xn_draw_copy_rect_stride_bytes (7: inventory_draw, spellbook_draw_spell ...); `CE4FA` xn_draw_line_text_colour (4: picklist_draw); `144D00` xn_draw_fill_rect (13); `144E84` xn_draw_get_rect (7); `144ED8` xn_draw_put_rect (5); `144F68` xn_draw_image (68); `144FB4` xn_draw_image_transparent (34); `1531F0` xn_draw_line (2: inpstr_edit); `1532B4` xn_draw_line_to (12: note_draw_page, saveload_draw ...) |
| str, bits | `CE300` xn_str_copy_line, `CE3E3` xn_str_copy_word, `CE3FD` xn_str_copy_alnum (parse_expand), `CE44C` xn_str_find_u32, `CE45E` xn_str_find_u16, `CE46D` xn_str_copy_until, `CE483` xn_str_fill_u16, `CE49E` xn_str_append_char, `CE663` xn_str_fill_ascending, `CE77F` xn_str_count_nonzero, `CE790` xn_str_skip_fields, `CE7A7` xn_str_find_byte_pair, `CE87B` xn_str_find_nonzero; `CE4A9`/`CE4B5` xn_bits_set_or_clear_u8/u16 |
| math, vec, mat, cam | `C7FD9` xn_math_approx_dist2d (46 sites), `C7FF4` xn_math_approx_hypot (17), `C808D` xn_math_angle_to_point (20), `C8167` xn_math_fixmul28_v2, `CE6D4` xn_math_mul_sin, `CE6E2` xn_math_yaw_offset_xz (14), `CE70D` xn_math_advance_pitch_yaw, `CE74F` xn_math_fixmul28 (rotate_xz), `14BC00` xn_math_isqrt; `C2000` xn_vec_unit_direction, `C2043` xn_vec_advance, `C2068` xn_vec_dir_to_angles, `14BDDD` xn_vec_normalize_ptr; `137000` xn_mat_from_angles, `137486` xn_mat_transform_ptr, `1374FC` xn_mat_transform_transposed_ptr; `12A274` xn_cam_set_focal, `12A2D0` xn_cam_set_view_window, `1376C8` xn_cam_project_ptr, `137725` xn_cam_scale_matrix |
| render, tex, light, shade, model, flat, terrain, world, water | `12A100` xn_render_init, `12A230` xn_render_shutdown, `12A254` xn_render_set_mode, `12A4F0` xn_render_begin_frame, `12A608` xn_render_pick, `12A870` xn_render_frame; `135D00` xn_tex_cache_lookup, `135DE4` xn_tex_cache_lookup_image, `135E39` xn_tex_cache_flush, `135E90` xn_tex_cache_begin_frame, `136026` xn_tex_cache_init, `CDC99` xn_tex_archive_set_translucent; `136A00` xn_light_init, `136AB4` xn_light_reset, `136AD8` xn_light_add; `136B88` xn_shade_load, `14D23C` xn_shade_set_fog, `CB300`, `C9EA7`, `C9EB2`, `CDC4B` (shade tables); `13FE15` xn_model_prepare, `1401D4` xn_model_submit, `C7F07`/`C7F14`/`C7F98` model angles, `C810C`, `CE808` xn_model_max_y; `154D00` xn_flat_add; `13E600` xn_terrain_draw, `14B45B` xn_terrain_height_at (18); `C2D00` xn_world_init, `C310F` xn_world_open, `C2D81` xn_world_cell_at, `C2E05` xn_world_update, `C2FF5` xn_world_reload, `C3A60` xn_world_place_nature_flats, `C3FCB` xn_world_mark_nonplanar_quads; `12F500` xn_water_init, `12F79C` xn_water_draw |
| collide | `14A300` xn_collide_segment_model, `14AA92` xn_collide_spheres_model, `14B1C7` xn_collide_segment_flat_stk |
| anim, rand | `C0100` xn_anim_rand_seed, `C010F` xn_anim_reset, `C013B` xn_anim_update; `C5200` xn_rand_noise_init, `C5280` xn_rand_noise_2d |
| sky | `C816E` xn_sky_init_stars, `C9A89` xn_sky_draw_snow, `C9BF7` xn_sky_snow_init, `C9C70` xn_sky_snow_update_speeds, `C9CB9` xn_sky_draw_rain, `CB31E` xn_sky_copy_rows |
| input | `12B100` xn_mouse_init, `12B136` xn_mouse_poll_clamped (107 sites), `12B2D3`/`12B2EB`/`12B3ED` cursor move/erase/draw, `12B45B` xn_mouse_set_cursor_image, `12B47E` xn_mouse_read_motion, `12B49E` xn_mouse_set_position, `CE69A`, `CE88D`, `CE8A0` (mouse range, sensitivity); `142700` xn_kbd_install, `142764` xn_kbd_remove, `142790` xn_kbd_flush, `1427A8` xn_kbd_read_key, `CE92C` xn_kbd_wait_all_released, `CE957` xn_kbd_numlock_off; `152B00`..`152D00` joystick; `CAE00` xn_input_steer_dispatch |
| gfx, pal, font, vid | `143600` xn_gfx_set_mode, `143700` xn_gfx_restore_mode, `143914` xn_gfx_clear, `CDD81` xn_gfx_present_inclusive (28), `CD308`/`CD31A` retrace waits; `12D851` xn_pal_set, `12D887` xn_pal_get, `12D8BD` xn_pal_fade_to, `CD33A`, `CD367`, `CE758` (8-bit DAC helpers); `12DB00` xn_font_init, `12DB28` xn_font_load, `12DB50` xn_font_select (47), `12DBCC` xn_font_draw_string; `C1500` xn_vid_play |
| sys, mem, timer, helmet, spell | `C0520`, `C0580` int 24h, `C7F00` xn_sys_yield, `C9F29` xn_sys_set_dos_transfer_buffer, `CE8C4`/`CE8D5` zero page; `149E00` xn_mem_init, `149EF8` xn_mem_shutdown, `CE66C` xn_mem_align_up; `C9F08`, `CE7BB`, `CE8B2`, `CE918` timers, `CDDA8` xn_timer_tick_callback (address taken by sound_init_music); `153500` xn_helmet_open; `CAE1C` xn_spell_kludge_menu_dispatch, `CAE07`/`CAE0E` (already named) |

`scratch/xngine/entry_points.md` has the full table, generated with every caller.


The per-function names, with confidence and evidence, are in config/names.csv (the `xn_` rows).
