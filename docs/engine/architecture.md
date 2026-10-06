# XnGine in C: the architecture

XnGine is Bethesda's 1996 engine, object 2 of Daggerfall's `FALL.EXE`. In this repository it exists
three ways:
- as the original asm, reassembled byte for byte (`src/xngine/`, docs/xngine.md);
- as a literal C translation (`src/xngine_c/`);
- as **canonical C** (`src/engine/`), the version this page describes.

The canonical C is written as a person would write it:
- plain prototypes in documented headers (`x<module>.h`);
- typed structs (`xnstruct.h`) and named globals;
- loops, with no generated code and no self-modifying code.

It is shown equivalent to the asm at the engine's boundary (docs/xngine_canonical.md). The original
bugs are kept and catalogued in docs/engine/quirks.md.

## Layers

| Layer | Modules | What |
|---|---|---|
| Platform | `pc.c` (xpc.h), `dos.c` (xdos.h), `xngine.h`'s port and `int` helpers, `timer.c`, the interrupt handlers in `kbd.c` (int 9), `joy.c` (int 1Ch), `serial.c` (COM1), `sys.c` (int 24h, the divide error) | Everything that touches DOS, DPMI, the BIOS or a port. A port to another platform replaces this layer. |
| Arithmetic | `arith.c`, `math.c`, `vec.c`, `mat.c`, `smc.c`, `pipe.c` | Fixed point. Angles are 2048 steps to a turn. Rotations are 3x3 rows of 2.28 fixed point. Sine and cosine come from 2.28 tables. Divisions the asm let fault give 0 (Q-SYS-01). |
| Utilities | `str.c`, `bits.c`, `rand.c`, `mem.c`, `img.c` | Strings and arrays, the LCG and value noise, the engine's work memory and its reciprocal and fill tables, image records. |
| Input | `kbd.c`, `mouse.c`, `joy.c`, `helmet.c`, `serial.c`, `input.c` | Key states from int 9, the mouse driver and the software cursor, the joystick on port 201h, three serial head trackers. |
| 2D | `gfx.c`, `pal.c`, `draw.c`, `drawimg.c`, `drawline.c`, `drawscl.c`, `drawunr.c`, `font.c`, `vid.c` | Mode 13h or VESA, the back buffer and present, the DAC, blits on `screen_buffer` (images, RLE and CEL, scaled, lines, rectangles), fonts, and the VID movie player. |
| 3D | `cam.c`, `model.c`, `terrain.c`, `flat.c`, `poly.c`, `light.c`, `shade.c`, `tex.c`, `tmap.c`, `span.c`, `render.c`, `rframe.c`, `water.c`, `sky.c` | The renderer (below). |
| World | `world.c`, `collide.c`, `colmodel.c`, `anim.c` | WOODS.WLD streaming and the procedural ground; segment and sphere collision against models, flats and the ground; the ASCR animation scripts. |

Each module's header opens with what the module does, its state, its formats and its quirks. Every
function is documented at its declaration or definition.

## The frame

The game (object 1, `world_render` and `game_frame`) drives one frame:

1. **Start.**
   - `xn_tex_cache_begin_frame` clears the texture cache's use counts.
   - `xn_render_begin_frame` resets the frame's pools: matrices, the polygon pool (1000 x
     `struct xn_poly`), light lists, the model queue, the flat list, and one span list per screen
     row. It also sets the frame's ambient shade row.
   - `xn_light_reset`, then the camera matrices (`xn_mat_from_angles`, `xn_cam_scale_matrix`).
2. **Submit.** For each object, the game calls:
   - `xn_model_submit`: sphere cull, then a queued {distance, model};
   - `xn_flat_add`: billboards, transformed and culled into the flat list;
   - `xn_light_add`.
   Outdoors, `xn_world_update` streams cells, and `xn_terrain_draw` makes the 31x31 ground cells
   into polygons.
3. **`xn_render_frame`** (`rframe.c`), a plain loop:
   1. **Models.** Sorted far to near. Each is occlusion-tested against the span lists, then
      transformed, its faces projected and clipped (`poly.c`: outcodes, then Sutherland-Hodgman,
      then 1/z = 2^40/z), and its light list built.
   2. **The S-buffer.** `xn_poly_rasterize` walks each polygon's edges. `xn_span_insert` keeps per
      row a sorted list of non-overlapping spans {next, x_end, x_start, 1/z, polygon}; where two
      overlap, the nearer by 1/z wins. Nothing is drawn yet, so each pixel is written once.
   3. **The span pass.** Row by row, each span calls its polygon's `span_fn` (`span.c`).
      - The first span of a polygon runs its setup, which lights the polygon. `light.c` gives a
        lighting index: saturated, one shade row, or 1-3 point lights through a shader record.
      - The setup also computes the texture gradients (`poly.c`) and installs the real routine.
      - The routines divide at every 8 or 16 pixels and interpolate in between; the shade comes from
        `shade.c`'s tables, and the distance fog is the span hook.
   4. **The background** fills the pixels no span covered (indoors).
   5. **Flats** are drawn far to near over the finished rows, clipped per row against the world's
      spans.
4. **After.** `xn_water_draw` post-processes the pixels below dungeon water. The HUD and UI draw over
   `screen_buffer` with the 2D blits and fonts. Weather draws there too (`sky.c`). Then
   `xn_gfx_present` copies the back buffer to the screen.

## State and data

- **The game sees some of the engine's memory, and only some.** `config/xngine_boundary.csv` maps
  it, measured by running the game:
  - the view globals;
  - each polygon's first 8 bytes, which the pick reads;
  - the screen and the back buffer;
  - the collision hit list in the work buffer;
  - and, through a stale pointer, the light shaders' image (Q-LIGHT-07).
  The rest is the engine's own, and the C keeps it however it likes: polygons hold C function
  pointers and shader records.
- **Data as C.** The engine's globals and tables are declared in its headers. In the test harness
  they resolve to FALL.EXE's addresses, where the original game reads them. For a build without
  FALL.EXE, `src/engine_data/` defines them: computed tables by code, and initial values loaded from
  the user's own FALL.EXE (docs/engine/data.md).

## The boundary

The game enters the engine at:
- 174 functions it calls;
- 3 functions it holds as pointers;
- the sound library's timer callback;
- the 5 interrupt vectors the engine installs.
The engine calls out to 15 game functions (malloc, free, the heap check, the sound library...), 40
int-service forms and 13 port forms. `config/xngine_boundary.csv` lists them all with evidence.

Inside the engine, functions call each other with plain C prototypes. Where a game call site reads
registers the asm left, a small `NAME_b` adapter gives the game exactly that; each of the 8 is
catalogued as a quirk.

## How it is known to be equivalent

docs/xngine_canonical.md has the argument and the tools. In short:
- **Records:** the 8,794 recorded calls replay through test shims, one per function, comparing
  everything the callers see.
- **The game's calls:** compared on game-visible memory, return values and port and DOS I/O.
- **Scenarios:** 39 scripted lockstep runs (1,675 frames, from all 18 saves: walking, towns,
  weather, water, every UI screen, combat, a movie), identical on the screen, game memory and I/O.
- **Coverage:** basic-block coverage of the asm under all of the above.
- **Pure helpers:** checked exhaustively, or with millions of samples, against the asm.
