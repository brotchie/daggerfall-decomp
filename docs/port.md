# The native port: FALL.EXE's C on arm64 macOS with SDL3

The goal is the game built from `src/` by the host's clang as a native program, with SDL3 in
place of DOS, VGA, the keyboard and timer interrupts and HMI SOS. arm64 macOS comes first.
Work happens on the `port` branch, in the worktree `~/dagger_comp-port`, alongside the XnGine
work on `main`.

## Rules

- **One source, two builds.** The matching build (`tools/build-and-verify.sh`) stays
  byte-identical. It checks every change the port makes to `src/` and `include/`. When the
  native build needs a different type than Watcom's bytes allow, the difference goes into a
  header (`#ifdef DAGGER_PORT`), not into a copy of the source.
- **The engine belongs to the XnGine work.** `src/engine/`, `tools/xn_*.py`,
  `config/xngine_*`, `docs/xngine*.md` and `docs/engine/` change only on `main`. For now the
  native build stubs the engine; it links `src/engine/` once the canonical C has landed.
- **Nothing in `build/port/gen/` is committed.** `tools/port_build.py` regenerates it on every
  build.

## Layout

| Path | What |
|---|---|
| `port/CMakeLists.txt` | the native build: the game's C as an object library, the shims, the generated definitions, SDL3 |
| `port/include/port.h` | included ahead of every game file: Watcom's far-pointer keywords become nothing, and the C library calls whose DOS behaviour differs go to `port/shim` (function-like macros, so a parameter named `close` stays) |
| `port/include/dos.h`, `i86.h` | Watcom's `REGS`, `SREGS` and the packed 43-byte `find_t`. Far pointers: a selector stands for a pointer's top 32 bits, so `MK_FP(FP_SEG(p), FP_OFF(p)) == p` for 64-bit pointers; selector 0 and the flat selector `segread` gives are the program's own 4 GB window |
| `port/shim/libc.c`, `watcom.c` | Watcom's `rand` (so a native run can follow an emulated one), `itoa`, `utoa`, `stricmp`, `strnicmp`, `exit`, `pow` (Watcom's square-and-multiply for integral exponents, which libm differs from in the last bits), `fscanf`/`sscanf`, `_msize`, `_nheapwalk` |
| `port/shim/memcheck.c`, `mcheck2.c` | MemCheck's `mc_*` replacements, passed to the C library. MemCheck is never started in FALL.EXE, so its checks return 0 and its debug output goes nowhere, natively as in the original |
| `port/shim/sos.c` | HMI SOS (timer, digital, MIDI, songs) as the game calls it. The timer events really fire, on SDL's timer thread: the 140 Hz event is the game's frame clock. There is no audio yet; samples "end" after their length's time |
| `port/shim/except.c`, `xgfx.c` | MemCheck's exception hooks (no-ops); two pixel routines in the library region (`gfx_put_pixel`, `gfx_get_pixel`) |
| `port/shim/dosfile.c` | DOS files: `C:\` and relative paths are the install folder, names match without regard to case, writes go to an overlay folder (read first), Watcom's `open` flags; `_dos_findfirst`/`_dos_findnext` as DOS matches (`*.*` matches every name, 8.3 upper case, directories only with `_A_SUBDIR`) |
| `port/shim/dos.c` | `int386`/`int386x`: DPMI 0500h (free memory, the emulator's figures), 0600h-0603h, 0100h/0101h/0006h (DOS memory as host blocks), CauseWay FF30h; anything else logs and sets carry |
| `port/include/port_vpc.h`, `port/host/vpc*.c` | the virtual PC on SDL3 (phase 5, below): low memory, VGA mode 13h and its DAC, the 70 Hz retrace, the keyboard (scan codes, int 9, the BIOS buffer), the mouse driver (int 33h), the PIT and BIOS tick, the vector table, DOS/BIOS/DPMI services with a register file, the sound card |
| `port/host/opl3.c` | a YMF262 (OPL3) from the chip's documented behaviour: log-sine and exponent tables, the 9-bit envelope generator and its rates, the eight waveforms, feedback, two- and four-operator channels, the rhythm section and its noise, tremolo, vibrato, OPL3 stereo; 49716 Hz |
| `port/host/hmi_opl.c` | HMI SOS's OPL MIDI drivers (fmmidi3.com 0xA009, fmmidi.com 0xA002), run from the install's HMIMDRV.386 as the game ran them: the driver's image is its memory, so its tables, state and quirks are the original's |
| `port/host/hmi_seq.c` | HMI SOS's song player as FALL.EXE links it: "HMI-MIDISONG061595" songs at 120 ticks a second, its loops and branches, track groups on hardware channels, CC7 by the master volume |
| `port/host/vpc_music.c` | the OPL3 in its own SDL3 stream at 49716 Hz; the player ticks inside the stream, so notes land on their sample |
| `port/test/opltest.c`, `musictest.c` | the OPL3 against the datasheet's numbers; a song rendered offline through the whole music path to a WAV (`--all`: every song) |
| `port/test/vpcdemo.c` | the virtual PC on its own, through XnGine's entry points, with the game's image, palette and sounds; `--selftest` checks it end to end |
| `port/host/host.c` | stopping SDL and the virtual PC, stopping on a stub or fault with the call chain, `port_check_ptr` |
| `port/host/launch.c` | the game folder (`--game`, `$DAGGER_GAME`, the remembered one, or a folder dialog) and the overlay (`--overlay`, `$DAGGER_OVERLAY`, `~/Library/Application Support/Daggerfall`) |
| `port/host/packed.c` | ARCH3D.BSA and DAGGER.SND unpacked from PACKED.DAT into the overlay (PKWARE DCL) |
| `port/host/zeropage.c` | reads and writes through a null pointer finished against the virtual PC's low memory, as DOS's flat memory gave them |
| `port/host/vpc_script.c` | the scripted driver: `PORT_SCRIPT` (a timeline of `shot`, `key`, `type`, `click` ... steps), `PORT_SHOT_EVERY`/`PORT_SHOT_DIR`, `PORT_EXIT_AFTER` |
| `port/host/main.c` | `main`: SDL, the folders, then the game's main (0x10010) with `Z.CFG`, as `FALL.EXE Z.CFG` ran; a backtrace for a stub or a fault; `port_check_ptr` stops on a pointer that lost its top half |
| `tools/port_build.py` | configure, build and generate; `run` prepares a game folder as `tools/fallemu.py` does and starts the build; `missing` lists the stubs |
| `tools/port_census.py` | the 64-bit worklist: clang's diagnostics on the game's C, by kind and by file |
| `tools/port_lowmem.py` | the game's reads of real-mode memory by address (68 of the BIOS tick at 0x46C, 44 of VGA memory at 0xA0000) as `DOS_LOW(addr)` (include/doslow.h): the same constant under Watcom, the virtual PC's low memory natively; lists the sites, `--apply` rewrites them |
| `port/include/port_gl.h`, `port/engine_gl/glcap.c`, `port/host/vpc_gl.c`, `port/host/pgl_share.c` | the OpenGL renderer for the 3D view (`--gl`, "OpenGL" below): the capture on the game's thread, the renderer on the main thread, the hand-over between them |
| `tools/port_gl_shots.py` | every classic save through the software and the OpenGL renderer, with a same-frame score |
| `tools/port_data.py` | the game's data (phase 2): object 3 in address order, globals whose types hold pointers in native layouts with 8-byte relocations from FALL.EXE's fixups; `report` lists declarations still too narrow |

## Building and running

As an app: `tools/port_build.py` then `tools/port_build.py app` makes
`build/port/Daggerfall.app`, with SDL3 inside the bundle and an ad-hoc signature. On its first
launch it asks for the folder Daggerfall is installed in (the one with ARENA2; Bethesda's free
release of the CD works) and remembers it. The game's files are only read. Saves, the game's
config and the archives unpacked from PACKED.DAT go to `~/Library/Application Support/Daggerfall`,
and the install's own SAVE0-SAVE5 show on the load screen too.

```sh
.venv/bin/python tools/port_build.py                 # build/port/fall
DAGGER_GAME=~/dagger_comp/build/game .venv/bin/python tools/port_build.py run [--nosound]
build/port/vpcdemo --game ~/dagger_comp/build/game        # the virtual PC on its own (with music)
build/port/musictest --game ~/dagger_comp/build/game --song D1.HMI --seconds 60 --out d1.wav
SDL_VIDEO_DRIVER=offscreen SDL_AUDIO_DRIVER=dummy DAGGER_OVERLAY=build/port/run \
    build/port/vpcdemo --game ~/dagger_comp/build/game --selftest
# a run without a person: screenshots every 2 s, keys and clicks on a timeline, quit at 60 s
PORT_SHOT_EVERY=2 PORT_SHOT_DIR=/tmp/shots PORT_EXIT_AFTER=60 \
    PORT_SCRIPT="8 key Escape; 12 click 160 100; 20 shot /tmp/a.bmp" \
    DAGGER_GAME=~/dagger_comp/build/game .venv/bin/python tools/port_build.py run
.venv/bin/python tools/port_build.py missing         # what the stubs stand in for
.venv/bin/python tools/port_census.py                # the 64-bit worklist
```

The game's C is compiled as Watcom 10.0a compiled it:
- C89 with extensions (`-std=gnu89`);
- plain `char` unsigned (`-funsigned-char`);
- signed overflow wraps (`-fwrapv`);
- no strict aliasing (decompiled code reads one type through another).

`int` and `long` are 32 bits on both, but pointers are 64 bits in the native build.

How the link is completed: the game's objects name about 4,000 functions and globals. What
the port defines links as it is, and so does the short list of C library functions the game
takes from the host (`HOST_LIBC`). Everything else gets a generated definition in
`build/port/gen/`:
- a function becomes a stub that names itself and stops;
- a global becomes its bytes from FALL.EXE, from its address to the next known symbol.

A game global that shares a name with the host's C library gets its own definition, which the
linker prefers.

## OpenGL

`fall --gl` (or `PORT_GL=1`) draws the 3D view with OpenGL 4.1 and shaders, at the window's
resolution (1920x1440 in the default window on a Retina display), instead of 320x200, with
4x multisampled edges. While the game runs:
- F11 switches the 3D view between OpenGL and the software renderer;
- F10 switches OpenGL's textures between texels (the engine's look, the default) and smooth:
  each pixel blends its four nearest texels, each through the shade and haze rows and the
  palette first, and the sky backdrop behind the scene is filtered the same way.

`PORT_GL_MSAA=N` sets the samples (1: none), `PORT_GL_SMOOTH=1` starts smooth,
`PORT_GL_SOFTWARE=1` starts with the software 3D view.

```sh
DEVELOPER_DIR=/Library/Developer/CommandLineTools .venv/bin/python tools/port_build.py
build/port/fall --game build/game --gl
.venv/bin/python tools/port_gl_shots.py [--jobs 6] [NAME ...]  # both renderers, every save
```

How it works:
- **The software renderer still runs.** The game sees the frame it always saw: the pick reads
  the S-buffer, and the HUD and the weapon are drawn over `screen_buffer`. OpenGL draws the
  same frame again, so the two can be compared frame by frame, and the software picture is
  there wherever OpenGL has nothing to add.
- **The capture** (`port/engine_gl/glcap.c`, on the game's thread, built with the engine)
  takes the frame as the engine builds it, through hooks in the renderer: `XN_GLCAP(...)` in
  poly.c (a model face's or a clipped polygon's camera-space vertices; a textured face's
  image), terrain.c (a ground cell's corners), flat.c (a flat's quad), render.c and rframe.c
  (the frame's start, the screen before the 3D pass, the frame's end), water.c (the dungeon
  water was drawn) and gfx.c (the present).
  Outside the native build `XN_GLCAP` is nothing; the hooks are under `DAGGER_PORT`.
  - At the frame's end, every pool record whose span routine is no longer its first-span setup
    (the S-buffer showed it somewhere) becomes triangles; every flat whose light setup ran
    (Q-FLAT-01: the others were never drawn) becomes a quad.
  - Ground cells are the exception: the terrain keeps a record for every cell it projects, and
    a distant cell thinner than a pixel at 320x200 never gets a span, so its setup never runs.
    Left out, it shows the sky through the ground near the horizon at the window's
    resolution. The capture finishes those cells' setup itself (the texels, and
    `xn_light_setup_terrain` for the shade row, which writes only the cell's own record) and
    draws them. A model face with no span at all is not recoverable this way: the model
    reuses its record for the next face.
  - Each vertex carries the texel the span routines would give at that point: the polygon's
    u/z and v/z gradients at the vertex's screen position, times z, as `tex_span`
    (u/z * z << 6 >> 32, 8.8 texels) and `tex64_span` (the ground's: * z >> 3, low dwords)
    compute them, plus the texture origin (swapped for the lit routine). GL's
    perspective-correct interpolation is linear in u/z and 1/z across the screen, as the
    gradients are, so the texels land where the engine puts them, without the 8- and 16-pixel
    linear steps.
  - The shade: the polygon's one shade row; a light shader (kind 8: torches, spells) goes to
    the fragment shader whole (its base shade, and per light the foot point, intensity and
    falloff), which evaluates `xn_light_shade` at every pixel with LIGHT.DAT as a texture: the
    pixel's view-space point from its camera-space position, the squared distances as the
    squares table has them, the falloff's high dword, the intensity >> 12.
    `PORT_GL_VERTEX_LIGHT=1` samples `xn_light_shade` itself at the vertices instead, the
    triangles split until their sides are under 12 pixels (the span routines sample every 16).
  - Textures are registered once by a hash of their pixels (an image's pixels at the 256-byte
    stride: a face's wrap size, a flat's size, the ground's 64x64) and uploaded as 8-bit
    indices.
  - The shade table and the haze table are copied with each frame.
- **The renderer** (`port/host/vpc_gl.c`, on the main thread) draws into a multisampled
  colour framebuffer with a float depth linear in camera z, resolved so that the scene's
  alpha is its coverage:
  - the fragment shader takes the texel (wrapped for faces and the ground; clamped, with
    texel 0 transparent, for flats), then the shade row, a flat's second row, and the
    distance fog's haze row where the span hook would apply it (`xn_shade_fog_span`: z >> 8
    past the fog's start, `fog_step` table bytes a unit), then the palette: the colour index
    is the one the span routines would write;
  - the world writes depth; the flats test it without writing, back to front, as the engine
    draws them; translucent flats (ghosts) are dithered;
  - the composite then picks, per window pixel: the software screen (VGA memory) outside the
    3D window or where the game drew after the 3D pass (the screen differs from the copy taken
    after it: the HUD, the weapon, rain and snow); else the OpenGL pixel
    over what was on the screen before the 3D pass (the sky backdrop), or indoors the
    background colour, by its coverage. The scene is drawn again at every present with the
    palette of the moment, so fades and the palette's effects apply as before.
  - **The dungeon water** (`xn_water_draw`, after the 3D pass) is drawn again at the window's
    resolution. The capture's hook at its end takes the water's plane in camera space (three
    of its points through the view matrix, as `water_end` puts them: a point p is seen
    through the water where n . p > 1), the rows it drew, each row's ripple (the jitter
    table at the row's phase, already moved on, plus its base) and WATER.TBL; then it copies
    the screen again as the post-pass snapshot, so the water's pixels are not taken for the
    game's 2D. While there is water the scene also writes a second colour attachment: the
    pixel's colour index (the nearest texel's, through its rows), covered, and the world's
    test against the plane (the flats leave it alone: the S-buffer the software tests has
    none; where no polygon is, it stays set, as the software draws the water between the
    spans). Integer, so the multisample resolve takes one sample. In the composite a pixel
    on the water's rows with the test set becomes the tint of the pixel jitter columns along
    its row (a frame column is several window pixels: the shift is scaled, the sub-column
    position kept). In a wide frame the ripple is the wide pass's (water.c's longer jitter
    table), so the sides ripple as the middle does. The software goes left to right in place, so a jitter of -1 reads the
    pixel to its left already redrawn when that is water too: the tint of its tint, as far as
    a run of -1s goes (nearly half the jitters are -1, and WATER.TBL is far from idempotent:
    repeated, it runs to colour 106 or to black). The composite follows the run back (up to
    16) and applies the table that many times. Water pixels are texels even in smooth mode,
    and hard-edged.
  - The 3D window is the rows the frame loop draws (the clip top to as far below the view
    centre as it is above it, Q-RENDER-05), not the whole clip window: the HUD's rows stay on
    the screen from frame to frame inside it.
  - A frame the game has drawn over (more than 60% of the 3D window changed since the 3D
    pass: the inventory, the map) is not shown; a menu over the frozen view keeps the frame.
- **The hand-over** (`port/host/pgl_share.c`): three scenes (filled by the game's thread,
  latest, shown) and a queue of new textures. The scene structs in `port_gl.h` are packed to 8
  bytes explicitly: the engine's objects are compiled with `-fpack-struct=1`.

Checks:
- `tools/port_gl_shots.py` loads every classic save in both renderers and saves the same
  moments; with `PORT_GL_SW_SHOTS=1` an OpenGL run saves the software screen of the same frame
  next to each shot, and the tool scores the OpenGL picture sampled at the 320x200 pixels'
  centres against it. Close views score 0.83-0.94 (the rest is a texel boundary or a light
  band moved by a pixel: exact perspective against the engine's linear steps); distant ground
  scores lower, since a minified texture's texel under a pixel centre depends on the exact
  sample point.
- 2026-10-18: all 18 classic saves render, and the screens around the world (the map, the
  options, the info and character screens) composite as they should. Two and a half minutes
  of random keys and clicks on six saves under AddressSanitizer (`PORT_SANITIZE=address`,
  OpenGL on) report only the original's stat-bar over-read. The game's loop runs at the same
  ~54 Hz in both renderers.
- No classic save stands in a flooded dungeon cell (the game takes `dungeon_water_level` from
  the player's cell every frame: 10000 in all 18). `PORT_GL_WATER=LEVEL` floods every dungeon
  cell to LEVEL (world y; a test: `port_wide_sky_drawn` sets it after `sky_update` took the
  cell's, in every run and in `PORT_GL_WIDE_CHECK`'s copy too).
  2026-10-18, `PORT_GL_WATER=-560 tools/port_gl_shots.py` on the seven dungeon saves (blades,
  mord, orcs, sentinel, uking, wayrest, worms; water on rows 29-200 down to 102-200): the
  same scores as without the water (0.84-0.99), the ripple and the tint where the software
  has them, the water line and the jitter's edges at the window's resolution. The eye under
  the water was not seen: levels above the eye reach only the rows under the HUD's icons
  there. 2026-10-20, on the wide view: blades, orcs and worms at 16:9 and 21:9 score the
  same with water (0.87-0.99, sides 0.79-0.99), the water spans the sides, and
  `PORT_GL_WIDE_CHECK` with both passes flooded finds the middle's colours as close to the
  original's as without water (1.5-2.8% of the pixels, at the old edges).
- `PORT_GL_DEBUG=1` prints, every 2 s, how many presents showed the 3D scene, and the scene's
  vertices and draws, and whether it had water (its rows).

Not yet:
- The sky backdrop stays the software renderer's pixels (320x200, filtered in smooth mode): it
  is a 2D image the game copies in.
- Translucent flats are a dither, not the blend table.
- macOS only (`<OpenGL/gl3.h>`); another platform needs a GL loader.

## Widescreen and smooth motion

`fall --wide` (or `PORT_GL=1 PORT_GL_ASPECT=fill`) fills the window with the 3D view, however
wide it is, and with OpenGL every refresh of the display shows a picture with the eye where it
is at that moment, not only the ~54 frames a second the game draws. Both are settings
(`<overlay>/settings.txt`, each overridden by its environment variable):

| Setting | settings.txt | Environment | Key | Default |
|---|---|---|---|---|
| the view's shape | `aspect=4:3\|fill` | `PORT_GL_ASPECT` | Shift+F11 switches | `4:3` |
| the widest horizontal view, degrees (0: none) | `fov_max=N` | `PORT_GL_FOV_MAX` | | `110` |
| how the 320x200 pixels scale | `hud_filter=sharp\|nearest` | `PORT_GL_HUD_FILTER` | | `sharp` |
| presents paced by the display | `vsync=on\|adaptive\|off` | `PORT_GL_VSYNC` | | `on` |
| smooth motion | `interpolate=on\|off` | `PORT_GL_INTERP` | Shift+F10 switches | `on` |
| the margin each side for smooth motion, columns | `overscan=N` | `PORT_GL_OVERSCAN` | | `24` |

The keys save their setting. The settings file keeps every line now (`launch_set_setting`);
it used to hold only `game=`.

**The wide pass** (`port/engine_gl/wide.c`, on the game's thread). The game draws into a
320x200 screen, and so does the engine, so the port widens the engine around the world's
drawing instead: `game_frame` calls `port_wide_enter` before `sky_update` and
`port_wide_exit` after `world_render` (src/lifted/main.c, `DAGGER_PORT`). In between the
engine draws into a W x 200 frame (`port_wide_buffer`) whose middle 320 columns start as the
game's screen and go back into it at the end, with the view window as wide as the frame
(`xn_cam_set_view_window(W/2, hh, W/2, cy)`) and the focal the game's: the vertical field of
view stays the game's and the horizontal one widens (Hor+). A 320x200 picture at 4:3 has
pixels 1.2 times as tall as wide, so a view of aspect A is 240 * A columns: 428 at 16:9 (94
degrees), 570 at 21:9 (110 degrees; `fov_max` stops it there and pillarboxes a wider window),
1024 at most (the ray tables cover 1024 columns). The HUD, the weapon and the pick run on the
game's screen as they always did. What the frame needed:
- the sky: `sky_draw_day`/`_night` draw a wide frame's sky straight into it
  (`sky_draw_wide`, src/lifted/sky.c): the original's window shows panorama column
  `column + k` at screen column k up to column 510 and then the next section's from 0 (it
  copies `511 - column` columns: column 511 is never shown), and the columns either side carry
  on the same way, 511 a section, with the original's choice of image per section;
  `sky_update`'s fills are as wide as the frame;
- the pools: the engine takes its polygons, spans and light lists from pools sized for 320
  columns, with no bounds (the polygon pool runs straight into the span rows). A wide frame
  takes larger ones of its own (`port_poly_pool` 6000, `port_span_pool` 60000,
  `port_light_pool` 9600; `xn_render_begin_frame`), and the light shaders' bytes past
  big_buffer's 100 KB (Q-LIGHT-07) go to a sink (light.c);
- `xn_poly_setup_textured` keeps mode 13h's choice of 8- and 16-pixel routines (it tested
  `xn_gfx_width == 320`);
- 2^45 / scale_x (`xn_cam_inv_scale_x`) is 2^31 at a half width of 200, the focal: past
  398 columns it no longer fits in 32 bits, and the models' unscaled matrix
  (`xn_model_scale_matrix`, the textures' gradients) and the sphere test's pick point came
  out wrong (textures stretched across every model). `xn_cam_unscale_x` (xpipe.h) divides in
  64 bits there;
- the pick (`xn_render_pick`) reads the S-buffer the last frame left: after a wide frame it
  runs in the wide frame's view, the point moved to its column (`port_wide_pick_begin`);
- the dungeon water: a row's ripple reads a jitter a column from phase + base (up to 131) on,
  and the 512 jitters run out past 381 columns, into whatever follows them (the offsets of
  the pixels the water takes its colours from). A wide frame reads a longer table, the
  original's over and over, shifted so that the game's 320 columns read what they always did
  (water.c);
- the weather: the game draws its rain and snow over its own columns after the 3D pass; the
  sides get as many streaks and flakes of their own, from a generator of their own
  (`port_wide_weather`, from `weather_draw_precipitation`), so the engine's random numbers
  are as they were.

The middle 320 columns are the original's pass in their sky and their geometry, not in every
texel: the spans of a polygon that crosses the 320-column view's left or right edge start
farther out in the wide frame (the span routines step their perspective every 8 or 16 pixels
from a span's start), a face's texture origin goes through the view's scaling, whose rounding
changes with the half width, so a whole face can land a texel over, and a flat that crosses
the old edge steps its texels from farther out too (2-5% of the pixels). `PORT_GL_WIDE_CHECK=N`
checks the rest: every Nth wide frame the game's thread forks at the frame's start, the copy
draws the frame the original way and hands back through shared memory its screen, its screen
after the sky (before the 3D pass) and its depth (1/z of what covers each pixel, from the
S-buffer's spans as the pick reads them), and the two are compared (`pgl_share.c`). Any pixel
of the sky different, or more than 3% of the pixels at another depth (beyond 1/32), is a
failure; the first failure's pictures are saved as `wide_check_*.pgm`. Polygons' edges differ
by a pixel here and there (0-1.2% of the pixels over the classic saves, more while the view
moves); a wrong sky seam showed as hundreds of sky pixels, the 32-bit 2^45 / scale as 50% of
the depths. A frame with no 3D pass (a menu) compares only the sky.
`PORT_WEATHER=4` or `5` draws rain or snow whatever the weather, to look
at the sides' (port/host/main.c).

**The picture** (`port/host/vpc_gl.c`). With `aspect=fill` the window's rectangle is its
whole height and 240 * A columns of the frame wide; the scene's framebuffers are that size,
and the vertex shader maps the frame's columns into it. The composite works in frame pixels:
the game's screen is the 4:3 column in the middle, the full height of the window (the HUD,
menus and screens over it, at their 320x200 resolution), as before; beside it the wide frame
as the game left it (`side`: the software pixels, the weather), with the OpenGL scene where
the 3D pass's pixel is still there, within the 3D window's rows. Below them (the HUD bar's
rows), and beside a screen with no 3D frame (the title, a menu), the sides are black. With
F11 the sides are the software renderer's wide frame. `hud_filter=sharp` scales the 320x200
pixels whole, blending neighbours only over the window pixel that straddles their edge, so a
scale that is not a whole number (7.2 at 1440 rows) has no uneven rows. The mouse's mapping is
the 4:3 column's, as before (`picture_scale`).

**Smooth motion** (`vpc_gl.c`):
1. *The hand-over.* `xn_glcap_present` runs after `xn_gfx_drv0_present` has copied the frame
   to the VGA (it ran before: the main thread could pair a new scene with the last picture),
   and the scene keeps that picture (`vga`): while no present has come since
   (`pgl_present_serial`), the composite shows the scene's copy, never half of the next.
2. *The pacing.* With `vsync=on` the main thread presents at every pass and the swap waits for
   the display's refresh (no 2 ms event wait; at most 250 presents a second when the window
   is hidden and the swap does not wait).
3. *The clock.* Each scene carries the eye (`xn_cam_x/y/z`, pitch, yaw, roll) and the game's
   clock: its frame times summed (`port_game_ms`, the time `frame_ticks` moves the player
   by). The main thread keeps the last 8 eyes, and maps the host's clock to the game's
   (smoothed); a present draws the eye of a frame and a half ago (between the two frames
   around it: angles the short way round; the position a line fitted to the frames within
   two frame times, nearer ones weighing more, since the game keeps the eye in whole units
   and a slow move comes as a unit every few frames; `PORT_GL_FIT=0` is the straight line
   between two frames) and never further back than the last present. A jump (more than 4000 units, a third of a turn,
   250 ms) starts again without blending.
4. *Re-posing.* The latest frame's geometry is drawn from that eye: camera space back to the
   world with the frame's rotation (the engine's own formula, `xn_mat_from_angles`, in
   floating point), into the eye's with its rotation, `c' = S Rt Rs^T S^-1 c + S Rt (eye_s -
   eye_t) << 8` (S: the view's scaling, focal / half size), one matrix and a shift for the
   vertex shader. The light shaders and the fog keep the frame's camera space; the depth is
   the new one. The sky backdrop moves with it (the panorama a column a step of yaw, the
   horizon focal_y tan(pitch)).
5. *Late look.* While the mouse looks around, the view's angles are the mouse's now: every
   mickey the game has not read into the frame's angles yet is added (the driver counts what
   fn 0Bh has handed over, `pgl_mouse_used`, and what is waiting, `pgl_mouse_total`), up to 64
   steps. Turning shows no delay; walking shows the frame and a half's.
6. *The margin.* A view moved this way shows a strip at the side the frame never drew: the
   wide pass draws `overscan` columns past each side (24: about 3 degrees at a 16:9 view's
   edge), which the window does not show. With 4:3 and smooth motion the wide pass runs too,
   for those columns. Vertically the scene is drawn 0.85% larger while the mouse looks around,
   as before.

Measured (`tools/port_gl_motion.py run`, 4:3, a 60 Hz display; the game draws 57-60 frames a
second on an M4): presents every 16.67 ms (sd 0.9). The eye's step per present, its
variation about each step's neighbours (judder, apart from the walk's own changes of speed):

| | smooth motion off | the line between two frames | the fitted line |
|---|---|---|---|
| kral, walking | 44% (20 presents repeat a frame) | 13% (none) | 8% (none) |
| kral, turning with the keys | 58% (17 repeat) | 6% | 8% |
| orcs, walking (it slides along walls) | 89% | 34% | 27% |
| orcs, turning | 45% (14 repeat) | 8% | 11% |

On orcs the game moves the player a whole unit every third frame along a wall, and speeds up
and slows down as it bumps along: what is left is mostly that. The game's loop stays
capped at 70 Hz: uncapped (`PORT_FRAME_CAP=0`) it walks and turns about a third slower (the
frame times come in 7.14 ms steps, and shorter frames lose them).

What stays at the game's rate: creatures, flats' animation, projectiles, the HUD and the
weapon. They are the latest frame's, so they hold still on the screen while the eye moves
smoothly past them. Smooth motion off (`interpolate=off`) is the earlier smooth look: the
fraction of a step the mouse's last read left.

Checks:
- `tools/port_gl_shots.py --aspect 4:3,16:9,21:9` (windows 960x720, 1280x720, 1680x720 points;
  smooth motion off, so the shots are the frames as drawn): the 4:3 column scored against the
  game's screen as before, and the sides against the software renderer's wide frame
  (`gl_T_swwide.bmp`, `PORT_GL_SW_SHOTS`).
- `PORT_GL_WIDE_CHECK=N` over the saves (above).
- `tools/port_gl_motion.py run [--interp on|off] [NAME]` walks and turns with `PORT_GL_RECORD`
  (whose header now has the eye each present was drawn from) and measures the eye's step per
  present, and the picture's shift per present turning.
- `tools/port_monkey.py` (random keys, clicks, mouse motion, `--resize`d windows) with
  AddressSanitizer.
- `PORT_GL_DEBUG=1` adds the presents' spacing, the frames' rate, snaps, the interpolation's
  range, the frame's width and its pools' use. F8 reports carry the view's settings
  (`meta.json`'s `view`).
- `PORT_WINDOW=WxH` sets the window's first size; `PORT_SCRIPT`'s `resize W H` changes it.

## Modern controls

`fall --modern` (or `PORT_CONTROLS=modern`) plays with mouse look and WASD. It is the game's
own controls, set up differently; nothing in the game's logic changes:
- **VIEW mode.** The CONTROLS screen's mouse setting VIEW (`mouse_control_mode` 1) is the
  original's mouse look: `player_mouse_look` adds the mouse's motion to the player's yaw and
  pitch (pitch +-256, a quarter turn). The window captures the mouse at the first click
  (Ctrl+G lets it go), so the motion never stops at the window's edge.
- **The keys** are `key_map` bindings (the 38 actions of the CONTROLS screen):

  | action | key | was |
  |---|---|---|
  | FORWARD, REVERSE, SLIDE LEFT, SLIDE RIGHT | W, S, A, D | Up, Down, Left Shift, Right Shift |
  | RUN (held) | Left Shift | P |
  | JUMP, CROUCH | Space, C | J, D |
  | ACTIVATE CENTER OBJECT (what the crosshair is on) | E | Space |
  | ACTIVATE CURSOR (a cursor for the HUD, and back) | Tab | Enter |
  | SWING WEAPON (held while the mouse moves) | left mouse button | right mouse button |
  | READY WEAPON, SWITCH HAND | F, H | A, S |
  | INVENTORY, STATUS, TRAVEL MAP, ABORT SPELL | I, J, V, G | F6, I, W, E |

  TURN LEFT and RIGHT stay on the arrows, and the rest keep their keys (R rest, T transport,
  M automap, L logbook, Q recast, Backspace cast, F1-F4 interaction modes, F5 the sheet).
- **When it applies.** `init_game_data` calls `port_controls_loaded` (port/host/main.c, under
  `DAGGER_PORT`) once the controls file is read. The preset replaces controls that are still
  the classic defaults (no controls file, or one saved unchanged). Controls changed on the
  CONTROLS screen are the player's, and they stay.
- **The crosshair** at the view's centre (both renderers; at the window's resolution with
  OpenGL) shows while the mouse looks around: VIEW mode, no cursor, no screen over the world,
  and a 3D frame drawn in the last 200 ms (`port_render_frames`, counted in rframe.c).
- **Smooth look (OpenGL).** The engine's angles are whole steps of 1/2048 of a turn
  (0.18 degrees): at 320x200 a step is under a pixel, at 1920 pixels across it is about 5,
  and a slow turn showed as 5-pixel jumps between frames that did not move. The mouse driver
  keeps the fraction of a mickey each read (fn 0Bh, once a frame) leaves behind; the capture
  stores it with the scene (`mouse_rem_x/_y`), and the vertex shader turns the scene by that
  fraction of a step (the yaw about the world's vertical under the scene's pitch, then the
  pitch), so the view stands where the mouse was when the frame read it. While the crosshair
  shows, the scene is drawn 0.85% larger, so the edge the fraction brings in is never
  empty. It applies only when the next frame turns the view (`look_probe` in main.c:
  player_mouse_look's conditions, SWING WEAPON not held). Measured with `PORT_GL_RECORD`
  (below) on a slow turn: every new frame moves 0.6-2 pixels, where it moved 5 or 0.
- `PORT_GL_RECORD=FILE` records a strip of every presented picture with the scene's angles
  (`PORT_GL_RECORD_COLUMNS=1`: a vertical strip), for looking at motion frame by frame:
  screenshots stall the main thread and so cannot.
- **Sensitivity.** The mouse driver scales its motion counts by int 33h fn 1Ah's sensitivity,
  as a DOS driver does, so the HORIZONTAL and VERTICAL settings of the game's mouse options set
  the speed of the look (the default 8 is the speed before).
- `PORT_SCRIPT` has `mdown B` and `mup B` (a mouse button held and let go) for scripted swings,
  and `mrel DX DY` (relative motion in game pixels, fractions kept, as a real mouse's).

## Feedback and crash reports

The port reports bugs to a service on this Mac:
- **F8**, anywhere (the title, character creation, the world, a menu), captures at once: the
  window's picture and the VGA's 320x200 one, the game's memory, the session's log and input,
  and, when the game is in the world, a save. Then a dialog asks for notes; Send uploads the
  report, Cancel drops it.
- **A crash** (a fault, a stub, `port_fatal`, an abort) writes a report from the fault handler
  with plain system calls (the cause and call chain, the screen as raw VGA bytes, the memory,
  the log and input); the process then execs itself as `fall --upload-reports SPOOL
  --after-crash`, which sends it, shows a notification and exits 70.

A report (port/host/report.c) is a folder, first in the spool `<overlay>/reports/pending/`:

| file | what |
|---|---|
| meta.json | id, kind, time, the commit built, the notes, the game's facts (engine_running, game_mode, region, game_minutes, camera, mouse mode) |
| screen.bmp, screen320.bmp | the window's picture (OpenGL's when on), the VGA's |
| log.txt | the session's stderr (teed from the start), with the first srand seed |
| input.script | every key, click and mouse motion that reached the game, as PORT_SCRIPT steps on the script clock |
| memory/ | fall_data.bin (FALL.EXE's data block in address order, `port_data_start`..`port_data_end` from tools/port_data.py), low_memory.bin (the virtual PC's 1 MB + 64K: VGA, BIOS), regions.txt (their native addresses), fall.symbols (`nm -n` of the build) |
| save/ | in the world: `save_game(5, "FEEDBACK")` from main()'s loop on the game's thread (`port_frame_hook`, src/hand/func_00010010.c under DAGGER_PORT), with the DOS layer sending `SAVE5\` to this folder (`dos_redirect_dir`): the player's slot 5 is untouched |
| crash.txt | a crash's cause, signal, address, pc and call chain |

**The service** is tools/feedbackd.py (Python's standard library only):
- `init` makes a bearer token and the URL from this Mac's Bonjour name
  (`http://<LocalHostName>.local:8742/v1/report`) in `~/.config/daggerfall-feedback/`;
- `install` also installs a LaunchAgent that runs `serve` at login and keeps it running
  (`uninstall` removes it);
- `serve` takes `POST /v1/report` (the bearer token, compared in constant time; X-Report-Id,
  X-Report-Kind; a gzip'd tar up to 1 GB), unpacks it with tarfile's `data` filter into
  `build/feedback/<time>_<kind>_<id>/` (gitignored), logs it in `index.jsonl` and shows a
  notification; `status` and `list` show the rest.

**The build** carries the URL and token: tools/port_build.py writes them, with the commit, into
`gen/feedback_config.c` (never committed) from `~/.config/daggerfall-feedback` or
`PORT_FEEDBACK_URL`/`PORT_FEEDBACK_TOKEN`. A build without them keeps its reports in the
spool. The port sends with `/usr/bin/tar` and `/usr/bin/curl`, the token in a 0600 header file
rather than on curl's command line; pending reports go at the next start too.

**Reading a report:**
- `tools/port_replay.py build/feedback/<report>` replays the session: input.script as
  PORT_SCRIPT, the first srand seed as `PORT_SRAND` (port/shim/libc.c forces main's seed only:
  the game reseeds its generators all the time), screenshots every second into
  `<report>/replay/`. It runs without a window (SDL's offscreen driver; `--show` for one) and
  with `PORT_INPUT_OFF=1`, which keeps the host's keyboard and mouse from the game, so nothing
  but the report's input drives it. A short character-creation report replayed matched its
  screen pixel for pixel; a longer one follows the same screens, but the game draws random
  numbers every frame, so rolls differ, and clicks closer together than a debounce (the
  attribute and skill arrows take one click per 6 BIOS ticks) can land differently: replaying
  by the game's BIOS tick instead of the script clock would close that.
- save/ loads as any slot: copy it to an overlay's SAVE0.
- fall_data.bin is FALL.EXE's data in place: `fall.symbols` gives each global's native address,
  and regions.txt the dump's start.
- Script steps for testing: `feedback` (F8; `PORT_FEEDBACK_NOTES` gives the notes without the
  dialog), `crash` and `crash segv`.

## Why the 64-bit work cannot be skipped

The game keeps pointers in `int`s everywhere: casts, globals typed `int`, functions declared
without parameters that return `int`. On a 64-bit host each of those loses the pointer's top
half. A 32-bit address space would hide that, but arm64 macOS has none:
- there is no ILP32 target (`arm64_32` is watchOS only);
- the kernel keeps the low 4 GB unmapped: `mmap(0x10000000, MAP_FIXED)` fails;
- a binary linked with a smaller `__PAGEZERO` (`-pagezero_size 0x4000`) is killed at exec
  (SIGKILL, tested 2026-10-05).

So every value that holds an address must have a type as wide as a pointer.

## Status

**2026-10-19, the first crash report fixed.** Starting a new game: quest_init_person's second
location, `D_001970C8`, a struct loaded_location that location_pick_random_town fills and
location_free frees, was declared only as bytes, so it stayed in place as 20 bytes while its
pointer fields (`D_001970D0/D4/D8`, read by name) were moved out of line: the fields read null
and location_free freed what the native struct had overrun. A port_data TYPES entry lays it out
as the struct (its fields' names on its native fields).

**2026-10-19, the first report fixed.** Character creation's skill points: the three groups'
points are shorts at 0x190DEA-0x190DEE in the temporaries block, where the native layout widens
talk's pointer slots, so the names fell apart (the arrows spent one place, OK checked another).
The two chargen files now take all three as `D_00190DEA[group]` under DAGGER_PORT; a scan of the
symbol map found no other name moved this way.

**2026-10-19, feedback and crash reports.** F8 and crashes send reports to a service on this
Mac (see "Feedback and crash reports").

**2026-10-18, modern controls.** `fall --modern`: mouse look, WASD, Shift to run, E to use,
a crosshair (see "Modern controls").

**2026-10-18, OpenGL.** `fall --gl` draws the 3D view with OpenGL at the window's resolution
(see "OpenGL"); F11 switches between it and the software renderer.

**2026-10-06, the game runs natively.** With XnGine linked (see "Linking the engine"), `fall`:
- plays the intro movie with its sound;
- loads the classic saves (SAVE0-SAVE5 of the install, from the load screen);
- lets the player walk and turn in town, with sound effects and the OPL3 music;
- shows the character sheet, the options panel and the other screens.

What it took, beyond the engine:
- **Threads.** The game runs on a thread of its own (`vpc_run`). The main thread takes SDL's
  events, shows the screen at 70 Hz and runs the scripted driver, whatever loop the game is
  in. A wait for a key with no port I/O (`options_open`'s `while (key_down_esc)`) used to
  hang.
- **The zero page.** Under the DOS extender a null pointer reads linear 0, the real-mode
  interrupt table. The game does that: enemy slots of factions, flats FLATS.CFG does not list,
  the settings before a save sets them.
  - `port/host/zeropage.c` finishes such faults against the virtual PC's low memory: it
    decodes arm64 LDR/STR/LDP/STP and resumes, and reports each place once.
  - `DOS_NULL(p)` (doslow.h) marks the known places in the source.
- **Packing.** Watcom 10.0a packs structs to 1 byte by default; a wcc386 probe puts
  `{char; int}`'s int at 1. The game and the engine are compiled with `-fpack-struct=1`, and
  port_data lays out i386 and native data the same way.
  - The link uses `-no_fixup_chains`: chained fixups cannot relocate pointers at odd offsets.
- **Data.** Every game table holding pointers is now declared so: port_data's narrow rows went
  from 494 to 0. MemCheck's and Watcom's own data (`library_data`) stays as FALL.EXE's 32-bit
  values, since only the library code the native build replaces reads it.
- **The freed-pointer sentinel** 0x97979797 is written one way, `(T *)(iptr)-1751672937`. A
  zero-extended spelling never compared equal to a sign-extended one.
- **Boundary adapters.** `xn_draw_paperdoll_mask`'s game call passes the engine's argument
  order natively. The other adapters only reproduce registers the asm left behind
  (Q-DRAW-02/03/05, Q-FONT-01, Q-MATH-02, Q-KBD-01); natively the game sites see their C
  values.
- **DOS handles.** File handles are DOS's (the lowest free from 5), each standing for a host
  descriptor, so the game's 20-handle archive tables hold.
- **SOS's sample callback.** The mixer calls it at a voice's end, under the interrupt lock,
  and plays on any data it gives (the movies stream their sound this way).
- **The installer's files.** ARCH3D.BSA and DAGGER.SND are unpacked from PACKED.DAT
  (`port/host/packed.c`, PKWARE DCL) into the overlay on the first run.
- **The data in place.** port_data keeps FALL.EXE's data byte for byte where it was, so every
  global keeps its distance from every other: the code reaches tables through shifted bases
  (climate's categories from 223 bytes before them). The globals holding pointers are laid
  out natively out of line.
- **Against the original.** `tools/port_compare.py` renders each classic save in the emulator
  (FALL.EXE itself) and natively, and compares the frames pixel by pixel (the best of nine native
  shots a second apart: torch light and lightning move a single shot by up to 30%). All 18 saves
  score 0.95-1.00, seven of them 0.995 or more; the rest is random (rain, clouds, walking people).
  - The comparison found two bugs: the sun's direction (three locals the asm took as a vector)
    and the weapon's colours (an offset counted from the 71-byte header).
- **Random inputs and AddressSanitizer.** `PORT_SANITIZE=address` builds a folder with ASan
  (recovering, so a run logs every report; run it with
  `ASAN_OPTIONS=handle_segv=0:handle_sigbus=0:detect_leaks=0:halt_on_error=0` so the zero page
  works). Two minutes of random keys and clicks after loading each classic save, plain and
  under ASan, found:
  - the faction list's use-after-free (DOS's heap took it; macOS's malloc traps);
  - a pointer cut to 32 bits in a compare (the biography: strlen off the heap);
  - the rank names' 4-byte stride;
  - overlapping strcpy, now copied forward as Watcom's library did;
  - the original's own reads a little past a buffer (the stat bars, a search one past its
    table), which are left alone.
- **Frame rate.** The world runs at the VGA's 70 Hz at most, about a quarter of a core, while
  the yield of the game's loop sleeps out each frame.
- **Checks without a person.**
  - `PORT_SCRIPT` (keys, clicks and screenshots on a timeline), `PORT_SHOT_EVERY`,
    `PORT_EXIT_AFTER`;
  - `PORT_AUDIO_RAW` (the final mix), `PORT_FPS`.

**2026-10-05, phase 1 done.**
- `build/port/fall` is a Mach-O arm64 executable built from all 427 game files.
- The build compiles with no errors. The 39 game structs that hold pointers check their size
  with `RECORD_SIZE_P`, which is exact under Watcom and only lets the struct grow in the
  native build.
- The link needs:
  - 220 function stubs: 175 engine functions, 45 unnamed library functions;
  - 2,624 globals defined from FALL.EXE's bytes.
- A run reaches the game's main. It asks DPMI for free memory (int 31h 0500h, not done yet)
  and stops at `func_0009DB3F`, MemCheck's handler registration.

**2026-10-05, the library.** All 45 unnamed library functions the game calls have native
implementations.
- Each has a name in `config/names.csv`:
  - HMI SOS: timer, digital, MIDI and songs;
  - MemCheck's API, found through its own table of names at 0x188F01;
  - Watcom's `pow`, `_msize`, `fscanf`, `_dos_findfirst`/`_dos_findnext`.
- Three of the existing names were wrong:
  - 0xA0AD9 is MemCheck's `strcpy`, not `strncpy`;
  - 0xA16F8 "fprintf" is `sscanf`;
  - 0xA31A6 "fprintf_2" is `fscanf`.
- The only stubs left are the 175 engine functions. A run gets through the DPMI and MemCheck
  setup and stops at the first engine call, `xn_sys_install_crit_error_handler`.
- The data generator is wired in. Of 2,346 globals, 391 have native layouts.
- Next in `main`:
  - `srand(*(int *)0x46c)` reads the BIOS tick at a fixed address, which the SDL layer has to
    provide;
  - `config_read` opens Z.CFG through a declaration that returns an int.

**2026-10-05, the virtual PC (phase 5, platform side).** XnGine reaches the hardware through
a few helpers, which the canonical engine keeps (docs/xngine_canonical.md on main):
- port I/O: `xn_inb`, `xn_outb`, `xn_inw`, `xn_outw`;
- services with a register file: `xn_int10`, `xn_int15`, `xn_int16`, `xn_int21`,
  `xn_int2f`, `xn_int31`, `xn_int33`;
- `_dos_getvect`/`_dos_setvect` for its five handlers;
- VGA memory and the BIOS data area by address.

`port/host/vpc*.c` gives the native build those same helpers on SDL3:
- **Video:** mode 13h at 0xA0000 of a 1 MB + 64 KB low-memory block, through the 256-colour
  DAC (3C7h-3C9h), shown at 4:3 in a resizable window. Alt+Enter toggles full screen; F12
  saves a screenshot.
- **Retrace:** 3DAh gives a 70 Hz vertical retrace. The screen is shown when the engine
  sees a retrace start, otherwise every 14 ms.
- **Interrupts:** they run on SDL's timer thread one at a time, under a lock that
  `port_cli`/`port_sti` hold off, as one CPU takes them:
  - the PIT's 18.2 Hz tick (int 8, the BIOS's: 0x46C and int 1Ch);
  - the keyboard (int 9 for each scan code byte at port 60h);
  - HMI SOS's timer events.
- **Keyboard:** SDL keys become scan code set 1. The default int 9 is the BIOS's: shift
  flags at 0x417, a keyboard buffer for int 16h.
- **Mouse:** int 33h with mickeys, ranges and press counts. A click captures the mouse;
  Ctrl+G releases it.
- **DOS and DPMI:** the int 21h file calls go to the DOS file layer; DPMI covers
  selectors, DOS memory and exception vectors. There is no VESA, so XnGine stays in
  mode 13h.
- **Sound:** SOS's samples play on a mixer of 160 voices into one SDL3 stream, with SOS's
  volume and pan. The overlay gets an HMISET.CFG with a Sound Blaster 16 (its digital
  device and its FM synthesiser at 0x388), so the game turns its sound effects and music
  on (`--nosound` keeps the install's). Music: below.
- **Data:** `screen_buffer`, whose initial value in FALL.EXE is 0xA0000, now points into the
  native low memory (tools/port_data.py).

`vpcdemo --selftest` checks it end to end, and every check passes:
- mode 13h and the DAC read-back;
- an ARENA2 image read through the DOS layer;
- scan codes 1E 9E 2A 1E 9E AA E0 48 through an installed int 9 at port 60h, and the BIOS
  buffer's a, A, Up when the handler chains;
- int 33h motion;
- frames paced at 70 Hz;
- the BIOS tick at 18.2 Hz;
- a DAGGER.SND sample that plays and finishes.

With a window the demo shows CHGN00I0.IMG with its palette and a mouse cursor, and clicks
play sounds.

`tools/port_lowmem.py` finds the game's own reads of real-mode memory by address: 68 of the
tick at 0x46C, 44 of VGA memory. It rewrites them as `DOS_LOW()`. A trial on three files kept
all 49 functions byte-identical. It is applied once the 64-bit pass has finished with `src/`.

**2026-10-05, music.** The game's music plays as it did on a Sound Blaster 16. The HMI
format and SOS's player were worked out statically from FALL.EXE, and HMI's OPL drivers from
HMIMDRV.386. The two analyses are in `build/port/agents/hmi/` and `build/port/agents/opl/`,
each with a Python reference model.
- **Songs** (`hmi_seq.c`):
  - The format: tracks with MIDI channels and device designations.
  - Timing: one tick is 1/120 s. There are no tempo events, and note-ons carry their
    durations.
  - HMI's 0xFE events: branch points, local and global loop ends (a count of 0xFF loops
    for ever), loop-counter resets.
  - Channels: tracks with the same MIDI channel share one hardware channel, the first free
    of 0-8 and 10-15; drums stay on 9.
  - Volume: CC7 is scaled by the master volume, and drum velocities by the drum channel's
    volume.
  - Every game song but FOLK3.HMI loops for ever inside the player, as in DOS.
- **The driver** (`hmi_opl.c`): HMI's own fmmidi3.com, from HMIMDRV.386.
  - 9 voices, each written to both OPL3 register sets: set 0 heard on one side, set 1 on the
    other, with CC10 pan making one quieter.
  - Instruments from MELODIC.BNK. Drums from DRUM.BNK by note, at the pitch in byte 2 of
    the drum's name record.
  - Volume: TL from a 64-entry volume table at the scaled velocity.
  - Its quirks are kept: programs 125-127 silent, voice stealing that spares channels which
    had a pitch bend, the overlapping register shadows.
  - The analysis found that pan comes out mirrored on an SB16 (MIDI left on the right
    speaker); the port keeps that.
- **Checks:**
  - The note counts after 60 s match the reference simulator exactly: D1.HMI 107, 04.HMI 589,
    and TAVERN.HMI 1,089 with its local loop followed.
  - All 131 songs play: notes and level, no silence beyond the songs' own rests, no clipping.
  - The song length the player works out matches all 131 MIDI.BSA records (sosMIDIInitSong
    is given no length).
  - `vpcdemo --selftest` drives the game's own SOS MIDI calls (sos_init's order: the driver
    for 0xA009 at port 0x388, the two banks, a MIDI.BSA song) and finds the OPL3 playing
    from the live audio stream.
  - Rendering runs at about 15 times real time.

**2026-10-05, phase 3 done (the 64-bit pass, cb305f2).**
- The census of diagnostics that lose half a pointer is down from 7,243 to 19. All 19 are
  calls through `struct rect`'s button handler, which its tables call with 0 to 4 arguments.
- How:
  - `iptr`/`uptr` for ints that hold addresses (`tools/port_iptr.py`, a dataflow over
    clang's AST);
  - `include/clib.h` for the library prototypes;
  - prototypes for every declaration without parameters (`tools/port_protos.py`);
  - 165 pointer tables retyped (`tools/port_globals.py`).
  FALL.EXE stays byte-identical throughout.
- The data generator gives 521 globals native layouts. Two lists are left, both in
  `build/port/agents/x64/` with the reasons:
  - 510 relocated pointers in 18 globals whose neighbours have no symbols;
  - 76 declaration disagreements, 54 of them only in pointee type.
- What the pass left alone: the record and struct size literals and the save/load paths
  (the next pass, with the disk converters); 35 four-byte strides over pointer data.
- `tools/port_lowmem.py` has been applied: the game's 112 reads of real-mode memory (the
  BIOS tick, VGA memory) go through `DOS_LOW()` (include/doslow.h), with FALL.EXE
  byte-identical.

**2026-10-05, saves and sizes (step 1 of the plan after phase 3): done.**
- **Save and data files** (f71d9cc): every struct the game reads or writes in its 32-bit
  layout goes through a converter natively (port/include/disk.h, port/shim/persist.c), hooked
  by macros that are the original code under Watcom (include/portio.h). The DOS formats are
  kept bit for bit: classic saves load, and the game's own saves stay DOS Daggerfall's.
  `port/test/savetest` runs 868,399 checks:
  - 16,465 records of the 18 classic saves round-trip, and their SAVETREE.DAT and SAVEVARS.DAT
    come back identical through the game's hooks;
  - MAPS.BSA, BLOCKS.BSA's RMB blocks and the QBN files convert, with fields checked.
- **Sizes** (c92e896): about 180 struct sizes, strides and offsets written as numbers are now
  `REC_SIZEOF`/`REC_OFFSETOF`/fields (records.h). `tools/port_sizes.py check` confirms each is
  the old number under i386.
- **Narrow frame slots** (911d2cd and the cleanup after it): locals the code reaches through
  a wider type (`short x; *(int *)&x = v;`) use include/ptrint.h's `slot16`/`pslot16`, the
  declared type under Watcom and room enough natively. More hidden BIOS-tick and VGA addresses
  now go through `DOS_LOW`. `tools/port_slots.py` finds none left.
- **Pointer tables at a 4-byte stride** use `PTR_SHIFT`/`PTR_SIZE` (ptrint.h): the list
  popups' scratch tables (scratch_190be4/de4/ee4, typed for the data generator), the talk
  lines, the spell and item-maker lists.
- **Left:**
  - 510 relocated pointers in 18 globals whose neighbours have no symbols, and 70
    declaration disagreements: lists in `build/port/agents/x64/`;
  - natively written saves have not been loaded by DOS or Daggerfall Unity yet;
  - none of this has run in the game yet. The engine is the remaining blocker.

The 64-bit worklist (`port_census.py`) has 7,243 diagnostics that lose half a pointer:

| Kind | Count | Files |
|---|---|---|
| pointer-to-int casts | 4,638 | 148 |
| calls through a declaration without parameters | 2,128 | 173 |
| int-to-pointer casts | 448 | 96 |
| other narrowing (shorten, int-conversion, compares) | 29 | 15 |

## Linking the engine

XnGine's canonical C (`src/engine/`, from `main`) is compiled natively in place of the 175
stubs: CMake's `engine` object library, the game's dialect plus `-DPORT_ENGINE` (port.h leaves
the engine's own `open`/`read` members alone). `glue.asm` is the matching build's; natively the
virtual PC is the glue. What the engine has under `DAGGER_PORT`:
- **Registers and services.** `xn_regs` is pointer-wide (`unsigned long`, the layout of
  `struct vpc_regs`): a DOS read passes its buffer's address in EDX. `xn_int10`..`xn_int33` and
  `xn_inb`/`xn_outb`/`xn_inw`/`xn_outw` are the virtual PC's (port/host/vpc.c); xngine.h's
  arithmetic pragmas are inline C there.
- **Real-mode memory** through the virtual PC's low memory: xpc.h's BIOS tick and keyboard
  flags, gfx.c's `VGA_MEMORY` and VESA window, DOS buffers through `DOS_LOW()`.
- **Interrupts.** A vector is a C function in the virtual PC's table. pc.c's get/set/install
  hand out an index for a saved vector, since a 32-bit offset cannot hold a native pointer. The
  entries the Watcom build makes as stubs (`xn_kbd_int9_entry`, `xn_joy_timer_entry`,
  `xn_sys_crit_error_entry`, `xn_sys_divide_error_entry`, `xn_serial_irq_handlers`) are C
  functions at the end of their modules, and the lock regions' bounds are dummies. `cli`/`sti`
  and `pushfd`/`popfd` (xsysutil.h) are the interrupt lock (`port_cli`/`port_sti`,
  `port_cli_depth`).
- **No hardware exceptions**: an arm64 divide does not trap, and its 0 for a zero divisor is
  what Q-SYS-01's handler gave.
- **Names.** The game calls five engine functions by address in the sources. Four of them
  (`xn_vec_dir_to_angles`, `xn_model_push_player_from_pick`, `xn_shade_init_reserved`,
  `spell_has_no_effects`) were promoted to strong in names.csv. `xn_timer_tick_callback` was
  named `D_000CDDA8` as data. The matching build is unchanged.
- **The engine's data** comes from tools/port_data.py, the way the game's data does:
  - all of object 2's data, in address order, between its functions;
  - types from the engine's headers and .c files (several declarators on a line are parsed);
  - names from names.csv at any confidence and from xngine_aliases.csv (a struct and its first
    field get one layout, the other name as a label);
  - clang's canonical layouts, so typedefs (`u32`, function-pointer arrays) lay out.
  `xn_poly_ring_tables`, which the C reaches only through the rings, is typed in port_data's
  TYPES as pointers. A pointer at -1 stays all ones.
- Watcom's `malloc`/`free` (0xA10A8, 0xA117E), which the engine calls, are in port/shim/watcom.c.

The engine's own 64-bit pass (the struct checks in xnstruct.h, pointers through `u32`,
pointer-table strides, data-file pointer slots) is the engine agent's, merged separately.
The platform files' changes compile to identical Watcom objects (all nine checked with
tools/wcc10.py).

## Phases

1. **A native build that links.** Done.
2. **Data as C.** Typed definitions of the game's globals:
   - pointers become relocations (`&symbol`, function pointers);
   - pointer-holding globals become 8 bytes;
   - unnamed spans stay byte arrays;
   - AddressSanitizer finds code that relies on two globals being adjacent.
   - XnGine's data waits for the canonical engine, which decides what the game can see.
3. **64-bit clean.** Every value that holds an address gets a pointer-wide type:
   - `iptr`/`uptr` where the code does integer arithmetic on addresses. They are `int` under
     Watcom, so FALL.EXE cannot change, and `long` in the native build;
   - real pointer types where Watcom's bytes allow.

   It is driven by clang's diagnostics and its AST, with the matching build as the check.

   **Saves and data files keep their 32-bit formats.** The save-data audit is in
   `build/port/agents/persist/report.md`, regenerable from its `sites.csv` patterns.
   - What is on disk:
     - SAVETREE.DAT writes records byte for byte: the 71-byte header, then the character,
       monster and QBN quest data, model instances and links.
     - SAVEVARS.DAT holds the quest faces, a copy of the player's header, and the factions.
     - MAPS, the RMB blocks and the QBN files are read raw into growing structs.
   - Pointer fields on disk are ids (an unlink pass before saving, a relink after loading)
     or garbage the loader ignores.
   - Classic saves and Daggerfall Unity both use these formats, so the native build keeps
     them bit for bit:
     - packed `*_disk` structs with 32-bit slots;
     - a converter at each function that touches the disk;
     - under Watcom the original call.
   - Everywhere else, about 75 header literals (`+71`, `55`, `0x47`) and the struct sizes
     and strides become `sizeof`/`offsetof`, the same constants under Watcom.
   - `model_instance` and `block_model` hide the engine model handle's `lights` and
     `matrix` pointers in padding. They must be declared as pointers, or the game and the
     engine disagree about where the model's angles and position are.
4. **The engine as plain C.** That is the XnGine work on `main` (the canonical C).
5. **SDL3 platform layer:**
   - video: an 8-bit framebuffer and palette;
   - input: SDL keys into the keyboard handler; the mouse;
   - time: the BIOS tick from SDL's clock;
   - files: done;
   - sound: the SOS API on an SDL mixer, HMI music;
   - DPMI: memory as `malloc`.
6. **Bring-up against the original.** A headless native run with `fallplay`'s input
   scripts, compared frame by frame with `fallemu` in lockstep. Milestones: the title, a
   loaded classic save, walking, sound, all 18 saves.
