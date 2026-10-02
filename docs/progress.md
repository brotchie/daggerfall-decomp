# Progress log

## 2026-10-01: game files

Source: Bethesda's free `DFInstall.zip`
(`cdnstatic.bethsoft.com/elderscrolls.com/assets/files/tes/extras/DFInstall.zip`).

| File | Size | SHA-1 | Notes |
|---|---|---|---|
| `DFInstall.zip` | 155,934,919 | `2190f4286712c85bf63269d666e021045a51a7e9` | |
| `DFCD/DAGGER/FALL.EXE` | 1,837,675 | `f249e30922ac16d689f21490eebe08b44c7cd063` | dated 1996-09-05, "TES: Daggerfall v1.0." |
| `DAGGER/DAG213.EXE` | 1,474,681 | `1832fdf2e171024ebf9b985a439ee07de353b0e0` | dated 1997-03-28, the 1.07.213 patcher |

First look, from `strings`:

- The CD's `FALL.EXE` embeds **CauseWay v3.17** and the runtime string
  "WATCOM C/C++32 Run-Time system … 1988-1994". A 1994 copyright year points at **Watcom 10.0**
  (10.5 says 1988-1995), the same family KKND-Decomp matches.
- `DAG213.EXE` is not a binary diff file but a DOS program: a CauseWay v3.32 executable (also
  Watcom, also "1988-1994") whose strings are mostly compressed. It says "This program will
  upgrade Daggerfall to version 1.07.213. You must run this patch from your Daggerfall
  subdirectory." It has to be run (or reimplemented) to get the 1.07.213 `FALL.EXE`.

## 2026-10-01: the 1.07.213 executable

`tools/patch_213.sh` runs the official `DAG213.EXE` patcher headless in DOSBox-X (Homebrew
`dosbox-x`) on a copy of the CD files, answering its prompts with `AUTOTYPE`. It produces:

| File | Size | SHA-1 |
|---|---|---|
| `orig/1.07.213/FALL.EXE` | 1,864,183 | `c49a2ceb677239af733d0e0127ac810ec859c0ac` |

This is the target (`config/fall.sha1`). Checks that it's the build UESP documents:

- 0x1B682A holds the item table: "Ruby", "Emerald", "Sapphire", 48-byte records.
- 0x1AA57C holds the debug-menu strings: "Get rumor", "Advance level", "Jump 1 month".
- Strings: "TES: Daggerfall v1.07.", **CauseWay v3.32** (the CD build had 3.17), the same Watcom
  runtime string dated 1988-1994.

The patch also updates 70-odd quest and text files in `ARENA2`, `SETUP.EXE` and `REPORT.EXE`,
and adds `FIXMAPS.EXE` and `FIXSAVE.EXE`. `DAGGER.EXE` is unchanged.

The game folder also ships `HMIDRV.386`, `HMIDET.386` and `HMIMDRV.386`: HMI's Sound Operating
System drivers, which tells us the licensed sound library to expect inside `FALL.EXE`.

## 2026-10-01: FALL.EXE's format (plan section 2)

**Plain, uncompressed LE.** The CauseWay MZ stub covers file offsets 0..0xB680, and `e_lfanew`
(0x3C) points straight at an `LE` header at 0xB680. Data pages start at file offset 0x5B600
(absolute), with no iterated or compressed pages. KKND's `le.py` loads it with one change
(`tools/le.py`): a few fixups have negative target offsets (`&table[-1]`-style), so the target is
wrapped mod 2^32.

| Obj | Base | Size | Flags | Contents (first look) |
|---|---|---|---|---|
| 1 | 0x010000 | 0x0AB27F | 0x2045 (R X) | Watcom-compiled game code, Watcom runtime at the end; entry 0x9DEB4 is Watcom's `_cstart_` (`jmp` over the "WATCOM C/C++32 Run-Time system" banner) |
| 2 | 0x0C0000 | 0x0A1568 | 0x2043 (R W) | Writable, but holds code too: rel32 calls go both ways between objects 1 and 2. Looks assembler-made (`8B D8 mov ebx,eax`, `33 C0`), reads the BIOS tick count at 0x46C, hooks interrupts with `cli`/`sti`. Probably HMI SOS and/or handwritten asm |
| 3 | 0x170000 | 0x045520 | 0x2043 (R W) | DGROUP data; the stack ends at its top (esp = 0x45520) |

Fixups: 37,520 internal, of kinds off32 (35,543), rel32 (1,018, only between objects 1 and 2)
and six 16-bit selector fixups to object 3, all in object 2: the asm loading DGROUP's selector,
as an interrupt handler does.

Object 1's first function (0x10010) starts `push ebp; mov ebp,esp; push ebx/ecx/esi/edi;
sub esp,imm32`, keeps its `eax`/`edx` arguments on the stack and re-reads them. So: the register
calling convention, frame pointers, no stack-check calls, and quite possibly little or no
optimisation for at least some modules. Functions are padded to 16 bytes with `nop`.

## 2026-10-01: compiler (plan sections 3 and 4)

**Version: Watcom 10.0 or 10.0a.** FALL.EXE's runtime string says "1988-1994", the same string
KKND-Decomp uses to pin Watcom 10.0/10.0a (10.5 would say 1995, 10.6 1996). So Daggerfall was
built with the compiler generation KKND's patch targets, even though it shipped in 1996 and was
patched in 1997.

**Toolchain: KKND's patched Open Watcom v2, built natively on macOS.** `tools/build_ow.sh` clones
open-watcom-v2, checks out `91922ea`, applies `tools/owpatch/daggerfall-wcc386.patch` and runs
OW's `build.sh boot` with clang (about 3 minutes). The patch is KKND's `kknd-wcc386.patch`
plus one fix: its off-by-default `KKND_CONSTREG` experiment in `regalloc.c` used `HW_COvlap`
on `HW_EAX`, which only compiles in KKND's Windows build. It's now `HW_Ovlap` under
`#if _TARGET_INTEL`.

Outputs in `third_party/open-watcom-v2/build/binbuild/`: `bwcc386`, `bwlink`, `bwasm`, `bwlib`.
The boot `bwdis` segfaults on arm64 macOS, but it isn't needed: `tools/omf.py` (from KKND) reads
the OMF objects and capstone disassembles them.

Smoke test: `bwcc386 -s -of+ -5r -omilert -zm -zp1` gives register-convention code (`f_`) with
Watcom's `89 xx` encodings, the same style as FALL.EXE's object 1.

## 2026-10-01: function discovery

`tools/find_functions.py` (adapted from KKND's) writes `config/functions.csv` and
`config/code_data.csv`:

| Object | Functions | Bytes decoded |
|---|---|---|
| 1 (game + Watcom runtime) | 3,315 | 689,585 / 701,055 (98.4%) |
| 2 (writable, asm + data) | 348 | 67,102 / 660,840 (10.2%, the rest is mostly data) |

Plus 55 switch jump tables. What the code looks like:

- **No padding between functions** in object 1: 2,350 of 2,689 `push ebp; mov ebp,esp`
  prologues follow a `ret` directly. KKND's executable has 16-byte zero fill from `-zm`, so the
  segment and alignment setup differs here.
- **Switches:** `shl eax,2; jmp cs:[eax + table]`, or for sparse cases `repne scasb` over a
  byte table of case values, then `jmp cs:[ecx*4 + table - 4]`. The tables sit in the middle of
  the function, before the dispatch, and the code after them is reached by jumps.
- **Tables are 16-byte aligned with `lea eax,[eax]`** (`8D 40 00`, at 0x609CD for example).
  KKND found no `lea` NOPs at all and patches the padding out by default, so Daggerfall may
  need `KKND_LOOPALIGN=1`, or a finer-grained switch.
- Prologue styles among the 3,315: about 2,466 `push ebp; mov ebp,esp; push regs`, 221
  `push regs; push ebp; mov ebp,esp`, 836 without a frame. At least 1,450 functions store
  `eax` into `[ebp-x]` in their first few instructions, so much of the game looks compiled
  **without optimisation** (arguments spilled to the frame and reloaded), which should make it
  far easier to match than KKND's `-omilert` code.
- The undecoded rest of object 1 is mostly runtime switch tables and the `int NNh; ret` stubs
  Watcom's `int386()` uses (0xAC551..0xAC7F6).

## 2026-10-01: first matches (plan step 4)

`tools/match.py` (adapted from KKND's) compiles a C file with the patched `bwcc386`, cuts each
`func_XXXXXXXX` out of the OMF object and compares it with FALL.EXE, masking relocations on
both sides.

The game code is Watcom's **`-od`** output: every function saves `ebx ecx edx esi edi` after
`push ebp; mov ebp,esp`, reserves locals with `sub esp, imm32`, spills its register arguments
to the frame and returns through a stack temp. With **`-od -s -of+`** (`config/cflags.txt`) the
first probes match byte for byte (`src/leaf_probes.c`):

| Function | C |
|---|---|
| `func_00048CDB` | `return D_0018467C;` |
| `func_000443F2` | `D_00195E7A = 1; return 0;` |
| `func_000478CF` | `return D_00190DF4 + 3;` |
| `func_00016507` | `int f(int a) { return 0; }` |

Differences from Watcom 10 found so far, all from Open Watcom 2.0's `-od` code generator:

1. **Branches.** Watcom 10 inverts the condition and uses short jumps
   (`cmp; jle next; mov [ret],1; jmp end`). OW emits `jg +5; jmp near` with every jump near
   (`func_000252FD`).
2. **Call results** go through an extra temp in Watcom 10:
   `call; mov [ebp-0x20],eax; mov eax,[ebp-0x20]; mov [ebp-0x1c],eax` (`func_000192EE`).
3. **Void functions.** OW reserves a return-value slot even for `void`, so a lone argument
   lands at `[ebp-0x1c]` instead of `[ebp-0x18]` (`func_00010AF6`).
4. **Narrow arguments.** Watcom 10 spills a `short` argument as the whole `eax` and reads it
   back with `movsx`. OW stores `ax` (`func_000192EE`). This may just be the declared type.

These are candidates for `-od`-specific patches, in the same spirit as KKND's.

## 2026-10-01: first `-od` compiler patch

**Branches (difference 1 above) are fixed.** At `-od`, OW generates code one statement at a
time and calls `FlushOpt()` after every block (`generate.c` `FlushBlocks()`), which pulls the
peephole queue down to 10 instructions. A forward jump leaves the queue before its label
arrives, so it's fixed as near, and a `jcc` over a `jmp` can't be inverted. Watcom 10 kept the
queue. The patch skips that flush (`DAGGER_FLUSH=1` restores it), and `func_000252FD` now
matches: `cmp; jle else; mov [ret],1; jmp short end`.

Evidence from the original's game code (0x10000..0x9D000): no `jcc near +5; jmp near` at all,
1,955 `jcc short +2; jmp short` pairs (so Watcom 10 still left many jcc-over-jmp pairs
uninverted, all short) and 157 `jcc +5; jmp near` pairs.

Two other experiments made no difference and were dropped: letting the object-level optimiser
(`optins.c`, `optrel.c`, `optcom.c`) run at `-od`, and running `BlockTrim()` at `-od`.

`tools/build_ow.sh` now builds a modified OW tree as is, without resetting it, so the patch can
be developed in place. Save it with
`git -C third_party/open-watcom-v2 diff > tools/owpatch/daggerfall-wcc386.patch`.
`tools/cc_dis.py` prints the disassembly of every function in a C file, for probing.

Matched so far: 5 functions (`src/leaf_probes.c`).

## 2026-10-01: second `-od` patch, void functions

**Void functions (difference 3) are fixed.** OW's C front end creates the hidden return
variable `.R` with the function's return type even when that type is `void`, and at `-od` every
temp gets a stack slot. So `void f(void) {}` reserved `sub esp,4`, where Watcom 10 has
`sub esp,0` (288 such prologues in the original). `cc/c/cgen.c` now skips the slot for `void`
(`DAGGER_VOIDRET=1` restores it). That matches `func_00010AF6` and the ten empty stubs.

Register saves at `-od` follow the parameters: a function saves every one of
`ebx ecx edx esi edi` that doesn't carry an argument. In the original: 1,650 functions save
all five (no argument, or one in `eax`), 321 save `ebx ecx esi edi` (argument in `edx`), 181
save `ecx esi edi`, 130 save `esi edi`. OW already does this.

Matched so far: 16 functions (`src/leaf_probes.c`). Still open: call results going through an
extra temp (`func_000192EE`, 387 sites in the original) and narrow arguments.

## 2026-10-01: CPU flag `-4r`, stack-slot order

**`-4r`, not `-5r`.** The original sign-extends a `short` with `movsx edx, word [ebp-x]` and
zero-extends a byte with `xor edx,edx; mov dl,[ebp-x]`. With `-3r`, OW uses `movzx` for the
byte. With `-5r`/`-6r` (OW's default), it uses `mov edx,[ebp-x-2]; sar edx,16` for the short.
Only `-4r` gives both, so `config/cflags.txt` is now `-od -s -of+ -4r`. That also fixes
difference 4 (a `short` argument is spilled as the whole `eax`) and matches `func_0003081B`.

**Still open: the order of stack slots.** `func_000192EE` is
`int f(short a) { int r; r = g(D_0019672C, a); return r; }`. The instructions now match, but
Watcom 10 puts the return variable above the local (`a` -0x18, return -0x1c, `r` -0x20), and
OW puts it below (`r` -0x1c, return -0x20). At `-od`, OW gives slots in the order temps first
appear in the instruction stream (`cg/c/temps.c` `AssgnMoreTemps()`, operands before
results), one statement at a time. Declaring `.R` before the locals in the front end made no
difference and was dropped. What's known:

| Function | Watcom 10 slots, top down |
|---|---|
| `int f(int a) { return 0; }` (`func_00016507`) | return -0x18, `a` -0x1c (OW agrees) |
| `int f(short a) { int r; r = g(a); return r; }` (`func_000192EE`) | `a` -0x18, return -0x1c, `r` -0x20 (OW swaps the last two) |

Next step: collect more functions with locals and a return value from the original, pin the
rule (for example "the return variable gets its slot when the first statement is generated")
and patch `AssgnMoreTemps()` to match.

Matched so far: 17 functions (`src/leaf_probes.c`).

## 2026-10-01: build and verify (plan step 5)

`tools/build-and-verify.sh` (`tools/build_fall.py`, adapted from KKND's `build_kknd.py`)
compiles every `src/**/*.c`, checks each function against the original, splices it into the
object images, rewrites the LE pages to `build/FALL.EXE` and compares the SHA-1 with
`config/fall.sha1`. A function counts as matched only if:

- every byte outside relocations is identical and the length is the same, and
- every relocation resolves to the original's target. A call or jump inside an object must have
  the same displacement. A call between objects 1 and 2 must hit the same LE rel32 fixup
  target, and a data reference the same LE off32 fixup target plus addend.

The splice writes relocated fields as a linker would (displacements, 0 for LE rel32 fields,
object-relative offsets for off32 fields) rather than keeping the original's bytes. With
`--blank`, every matched function is first filled with `int3`, and the output is still
byte-identical, so the matched code in `build/FALL.EXE` comes from our C. The LE fixup tables
are kept as they are, because the checks show every relocation in the new code is one the
original already has.

Negative tests fail as they should, with the reason: a data reference 4 bytes off
(`reference _D_00184680+0 (0x184680) != original 0x18467c`), a call to the wrong function, and
a changed constant.

`build/matched.txt` lists the matched functions. Current state: **17 / 3,663 functions, 542
bytes, `build/FALL.EXE: OK`.**

## 2026-10-01: third `-od` patch, stack-slot order

**Solved.** Watcom 10 gives stack slots to declared variables in a fixed order, from the top of
the frame down:

1. 2-byte (`short`) parameters, last to first, then 2-byte locals, last to first;
2. the return variable;
3. the locals, last to first;
4. the parameters, last to first.

Evidence, top down (P = parameter, L = local, R = return variable, s = short):

| Function | Slots |
|---|---|
| `func_00013A00` | R, P3, P2, P1 |
| `func_0001811D` | R, L, P1 |
| `func_000192EE` | P1s, R, L |
| `func_000309E8` | P2s, Ls, R, P1 |
| `func_00030A23` | P3s, P2s, R, L, P1 |
| `func_00045AED` | R, L2, L1, P2, P1 |
| `func_00046C62` | R, L2, L1 |

Open Watcom gives slots at first use, statement by statement (`cg/c/temps.c`). The patch records
each declaration as it is made: parameters in `DoParmDecl()`, locals in `BGAutoDecl()`, the
return variable in `CGTemp()`. `DaggerAllocDeclared()` gives the slots in the order above when
`Generate()` handles the function's first statement. `DAGGER_FIRSTUSE=1` restores Open Watcom.
The front end declares the return variable before the body's locals (they come from a separate
block node), which is why the patch orders by kind and not just by declaration order.

The rule was worked out from `func_000192EE` and then checked against eight more functions
from a survey of small branch-free functions with locals. Seven matched on the first try, and
the eighth (`func_00030A23`) after swapping the operands of a multiply. All are in
`src/slot_probes.c`.

Open questions: whether 1-byte variables are ordered like 2-byte ones, and how nested-block
locals are placed.

Also noticed: `func_0001DA2D` calls `func_000A1023(dst, src, n, "faction.c", 0x751, 4)`. The
string and line number look like `__FILE__`/`__LINE__` from a checked-copy or allocation
macro, so **the original source file names and line numbers are in the executable**. That's
strong evidence for naming units and ordering functions.

Matched: **26 / 3,663 functions, 1,191 bytes, `build/FALL.EXE: OK`** (also with `--blank`).

## 2026-10-01: the original source files

`tools/find_units.py` writes `config/units.csv`. 529 functions pass a source file name
(`"faction.c"`, `"talk.c"`, ...) to helpers, most likely `__FILE__` in checked-memory or
assert macros. Each of the **84 names** is referenced from exactly one contiguous run of
functions, no function references two names, and the runs don't interleave. So the runs give
the original object files **in link order**:

`main.c` (0x10010), `sosez.c`, `profile.c`, `archive.c`, `steal.c`, `camera.c`, `rumor.c`,
`engsupp.c`, `talk.c`, `faction.c`, ... `inven.c`, `int.c`, `color.c`, `travel.c` (ends
0x9D986). Then comes the Watcom runtime, with two more names inside it (`xxdef.c` at
0xA2B64 and `xerftrc.c` at 0xB7622), which probably belong to a library.

Functions that sit between two runs belong to one of the neighbours. That's not settled yet,
but it's a small search per boundary. This sets the layout for `src/`: one C file per original
unit, named as the original.

## 2026-10-01: regions (plan section 6)

`config/regions.csv` splits the code three ways, and the build now reports progress against the
game region only:

| Region | Range | Functions | Bytes | What |
|---|---|---|---|---|
| game | 0x10010-0x9DA1C | 2,297 | 580,076 | the game's own C, 84 units, all `-od` |
| library | 0x9DA1C-0xBB27F | 1,018 | 109,364 | StratosWare MemCheck, Watcom runtime, others |
| xngine | object 2 | 348 | 67,023 | XnGine 3D engine, handwritten asm |

- The boundary is exact: the last `travel.c` function (`func_0009D986`) ends at 0x9DA1C, and
  the next function uses a different convention (stack arguments, `ret 8`).
- **MemCheck** strings ("MemCheck Internal: Could not set DPMI vector") start at 0x9DBDD, the
  same library KKND found. The `__FILE__`/`__LINE__` calls are its checked wrappers:
  `func_000A1023(dst, src, n, "faction.c", 1873, 4)` is a checked `memcpy`. The literal line
  numbers have to be written out in our C, since our files won't have the same line count.
- **Object 2 is XnGine**, the engine shared with Redguard and Battlespire: "XnGine: Jump to
  zero-page.", "ENGINE: Out of memory for shaders.", `$`-terminated DOS strings. It's
  handwritten asm (MASM-style encodings) in a writable object, likely self-modifying, so it
  stays asm, as the plan expected.
- `HMI*.386` drivers ship with the game, and `sosez.c` (the second unit) wraps HMI's Sound
  Operating System.

Game progress: **26 / 2,297 functions, 1,191 / 580,076 bytes (0.21%)**.

## 2026-10-01: `src/` by original unit

`src/` now has one file per original source unit (`src/talk.c`, `src/qmisc.c`, ...), with
functions in address order and shared declarations in `include/dagger.h`, so every global has
one type across units. That fixed a real conflict: `D_00195AC4` had been declared `int` in one
probe and `char *` in another. `tools/units.py` maps an address to its unit. A function between
two units' `__FILE__` runs is filed with the earlier unit and marked
`/* between the X and Y runs: unit not certain */`.

The build doesn't depend on the file layout (it splices by address), so these assignments can
be corrected at any time.

## Phase summary: build and verify (plan step 5) is done

- `tools/build-and-verify.sh`: full splice-and-SHA-1 build with relocation checks, negative
  tests and a `--blank` self-test.
- `tools/split.py`: per-function asm listings (gitignored `asm/`).
- Compiler: three `-od` patches (peephole flush, void return slot, stack-slot order), each
  switchable back to stock OW with an environment variable.
- Layout: 84 original units recovered from `__FILE__` strings, regions for game, library and
  XnGine, `src/` organised by unit.
- **26 / 2,297 game functions, 1,191 / 580,076 bytes, `build/FALL.EXE: OK`.**

Next phase: match functions in bulk. Rank the game functions by difficulty (size, branches,
calls, floating point), work through the easy ones unit by unit, and collect the compiler
differences that come up along the way.

## 2026-10-01: the batch lifter

Working rule from here: trade model turns for CPU time. Tools run over every function in a
batch, and turns go to the residue and to improving the tools.

`tools/lift.py` turns a game function's `-od` code back into C. Watcom `-od` keeps no register
value between statements, so it executes each statement symbolically (register → C
expression) and emits a statement at each store, unused call result or branch. Control flow
comes out as `if (...) goto` / `goto`, which `-od` compiles back to the same `cmp`/`jcc`/`jmp`.
Globals are `char D_X[]` accessed through casts, so no type inference is needed yet. Callees are
unprototyped `int func_X();`, and arguments come from whichever of `eax edx ebx ecx` the
statement loaded.

`tools/lift_all.py` lifts, compiles and checks (bytes plus relocation targets, as in the
build) all 2,297 game functions on every core, **in about 2 seconds**. It writes
`build/lift/report.csv` and a summary of the most common unsupported instructions, compile
errors and first differences, which is the to-do list.

First run: **537 / 2,297 match** (23%), against 26 by hand. 815 hit an instruction or shape
the lifter doesn't handle yet (three-operand `imul` 297, stack-argument calls 120,
`idiv`/`div` 149, ...), 529 produce C that doesn't compile, and 416 compile but differ.

## 2026-10-01: lifter v2 and the first batch-found compiler differences

Lifter v2 types every stack slot from its accesses in a pre-pass, and handles three-operand
`imul`, `idiv`/`div` (with the `x - (x >> 31) >> 1` idiom for `/ 2`), cdecl calls (stack
arguments with caller cleanup, declared `int f(int, ...)`) and `if (...) return;` for jumps
to the epilogue. The report now ignores relocated bytes and branch displacements when it looks
for the first difference, so it names the real cause.

**674 / 2,297 game functions match** (29%), the whole batch in 2 seconds. Remaining: 1,110
differ, 512 unsupported, 2 lifter errors.

The batch found compiler differences that hand matching would have taken weeks to hit:

1. **`x = x + 1`** (fixed, `DAGGER_RMW=1` restores OW). Watcom 10 compiled it as
   `mov eax,[x]; inc eax; mov [x],eax`; only `x++`/`x += 1` became `inc [x]` (1,181 of those in
   the original). OW's `MakeGets()` points the add straight at `x`, which the encoder turns
   into `inc [x]`. The patch keeps the temp when the destination is also an operand.
2. **An unused `x++`** (open, about 140 functions). Watcom 10 still loads the old value:
   `mov eax,[x]; inc [x]`. `y = x++` gives that shape in OW, so only the discarded case
   differs. A debugger trace shows `DeadTemps()` (`cg/c/optimize.c`, called from
   `BlockToCode()` at `-od`) frees the load. With that blocked by a temp flag, `AxeDeadCode()`
   does, and with that blocked too it still disappears, so there's a third pass. Reverted for
   now. Next: trace again with both blocked.
3. **Byte/word compares** (open, about 150 functions; see the update below). Watcom 10 compares
   named variables and `*p` narrow (`cmp byte ptr [x], 2`: 687 globals, 548 locals, 138
   `*p == 0`), but widens fields read through pointer arithmetic (`mov al,[eax+0x22];
   and eax,0xff; cmp eax,2`, about 1,600 cases). OW narrows them all. Turning off demotion in
   `TGCompare()` changed nothing, so the narrowing happens earlier, probably in the C front
   end's folding. Reverted for now.

Lesson for the workflow: fix compiler differences only once the batch shows a cluster, A/B
each patch across the whole batch with its env switch, and timebox the debugging. The
debugger is the fast way to find which pass drops an instruction.

## 2026-10-01: lifted functions in the build

- **One signature per function.** `lift.signature()` derives each game function's return type
  and parameter types from its own prologue and slot types, and every caller declares it that
  way. This turned up a lifter bug that unprototyped calls had hidden (an earlier call's result
  passed as an extra argument): argument counts now come from the callee's signature, and
  registers are cleared after every store, since nothing survives a statement at `-od`. With
  real prototypes the batch matches **688** (674 without).
- `tools/promote_lifted.py` writes every matched lifted function into `src/lifted/<unit>.c`,
  grouped by original unit, leaving out anything hand-written in `src/*.c`. `src/lifted/` is
  generated: to work on a function by hand, move it to `src/<unit>.c`.

**The build: 692 / 2,297 game functions, 52,358 / 580,076 bytes (9.03%), `build/FALL.EXE:
OK`** (also with `--blank`).

Loop from here: `tools/lift_all.py` (2 s), fix the top cluster in the lifter or compiler,
`tools/promote_lifted.py`, `tools/build-and-verify.sh`, commit.

### Update: byte compares

Tallied over the original's game code: compares of a byte global are narrow
(`cmp byte ptr [D], K`) 686 times, only with `je`/`jne`, and widened
(`xor eax,eax; mov al,[D]; cmp eax,K`) 354 times, including ordered compares. For byte locals
it's 548 narrow (mostly unsigned `<`/`<=`) against 23 widened. One function (`func_000160D0`)
has both side by side: `cmp byte [D_001966B1], 0` (probably a truth test, `if (flag)`) next to
a widened `== 1`.

In OW, the front end passes the raw byte operand to `TGCompare()` with an `int` compare type,
and `ResultType()` demotes it (seen in lldb). A switchable policy in `TGCompare()`
(never / only against 0 or 1 / only against 0, with or without locals) changed nothing for the
widened cases: OW still emits `cmp byte` with demotion off there, so **a later,
instruction-level pass also narrows**. Next: find that pass with lldb (break where the compare
instruction gets a byte type class), then measure the policies again with the batch.

## 2026-10-01: more lifter coverage; the build passes 10%

Lifter additions, each driven by a cluster in the batch report:

- **Stack-convention callees.** MemCheck's location hook `func_000A0ED9(line, file)` is called
  with `push "file.c"; push line` before every checked operation and pops its own arguments
  (`ret 8`). Callees that pop with no register arguments get
  `#pragma aux f parm routine [];`. Five-argument calls (four registers plus the stack, callee
  pops) are Watcom's normal convention. A pushed register counts as consumed.
- **Unused and array locals.** Every 4-byte slot between the saved registers and the frame
  bottom belongs to a declared variable, so gaps become unused `int` locals in the right
  declaration order, and an address-taken slot becomes a `char l_X[n]` reaching up to the next
  variable.
- **Idioms:** signed division by 2^k (`sar edx,31; shl edx,k; sbb eax,edx; sar eax,k`),
  `xor ah,ah` (zero-extend to 16 bits), `test ah,K`, calls through function pointers, and
  registers that survive a call (Watcom callees preserve everything but `eax`, so
  `if (f(1) > x + 10)` keeps `x + 10` in `ebx` across the call).

Batch: **738 / 2,297** lift and match. Build: **742 / 2,297 game functions, 61,837 / 580,076
bytes (10.66%), `build/FALL.EXE: OK`.**

Investigated and parked: the return variable's slot. It's above all locals in 444 functions
but in the middle or at the bottom in 86. Neither address-taking, nor `register`, nor
first-use order explains it (tested with a switchable compiler patch and a whole-game tally).
Locals themselves follow source declaration order, which the lifter reproduces.

## 2026-10-01: compiler fix for an unused `x++` (+51 functions)

Watcom 10 compiled an `x++` whose value is unused as `mov eax,[x]; inc [x]` (the old value is
loaded into a register and dropped). OW drops the load. Tracing with lldb (breakpoints on
`FreeIns`/`DoNothing`, rerun after each fix) found four places, all now handled for a temp
flagged `DAGGER_KEEP` in `TNPostGets()` (`cg/c/tree.c`):

1. `DeadTemps()` (`optimize.c`), called from `BlockToCode()` at `-od`, freed the load;
2. `AxeDeadCode()` (`optimize.c`) then dropped it as a dead instruction;
3. `AssignConflicts()` (`regalloc.c`) put the never-read temp in memory (`savings == 0`), so
4. `ScanForLastUse()` (`temps.c`) deleted the dead store.

The flagged temp now gets a register. `DAGGER_DEADDEF=1` restores OW. Batch: **789 / 2,297**
(738 without).

## 2026-10-01: compiler fix for widened compares (+247 functions, 20% of game code)

**Found with a debug build of the compiler.** `third_party/ow-debug` is a git worktree of the
same OW tree built with `OWDEBUGBUILD=1` (no optimisation, symbols), so lldb shows locals.
Breakpoints walked the compare from the front end to the instruction: `TGCompare()` builds an
`int` compare with a proper `O_CONVERT` node, then `FoldCompare()` (`cg/c/treefold.c`) calls
`BurnToBase()` to "get rid of some lame converts the C++ compiler likes to emit", which
compares the byte at its own size.

The fix, `DaggerStripConvert()`, makes that strip a policy, and the batch measured each
candidate rule (`DAGGER_STRIP=ow|never|local|eq|localeq0`). The rule that matches Watcom 10
is the simplest: **never strip an explicit conversion at `-od`**. A plain `x == K` is still
narrowed by `ResultType()`, so both of the original's forms are reachable from C:

| C | Code |
|---|---|
| `x == 1` | `cmp byte ptr [x], 1` |
| `(int)x == 1` | `xor eax,eax; mov al,[x]; cmp eax,1` |

The lifter now writes widened values with an explicit cast (`(int)(unsigned char)x`,
`(int)(signed char)x`, `(int)(short)x`). The same widened shape turns up in arithmetic and call
arguments too, which is why the gain is much larger than the compare cluster suggested.

Batch: **1,040 / 2,297** (793 before). Build: **1,044 / 2,297 game functions, 118,554 /
580,076 bytes (20.44%), `build/FALL.EXE: OK`** (also with `--blank`).

Also from the batch, new clusters to look at next:

- **Short stack variables are stored as full dwords**: `mov [x], eax` 1,780 times against
  `mov word ptr [x], ax` 5 times. OW stores a word.
- **Locals are pushed through `eax`**: `mov eax,[x]; push eax` 133 times against `push [x]` 9
  times, while globals are pushed directly (70).
- Pointer arithmetic: an `add` whose result is used as a base is emitted as `char *`
  arithmetic, so OW loads the base pointer into a register as Watcom 10 did (+4). One remaining
  shape (`add eax,edx` versus OW's `add edx,eax`) is register allocation and has no C-level
  fix.

## 2026-10-01: unused `x++` keeps its load only for locals (+32)

The original's unused `g++` on a global is a plain `inc [g]`; only stack variables get the
`mov eax,[x]` first. In `TNPostGets()` the lvalue is a leaf whose address name (`u.addr`) has
class `CL_ADDR_GLOBAL` for globals (found in the debug compiler after two wrong guesses about
the tree shape). The keep flag now skips those. Batch **1,072**.

## 2026-10-01: calls by callee convention; stack parameters

- The batch report now also ranks causes **by bytes of code blocked**, since a big function
  fails on its first mismatch.
- **Call arguments come from the callee's convention**: register and stack argument counts
  from its signature, its `ret N` (stack bytes it pops), or a cdecl cleanup. The most recent
  pushes are its stack arguments. Other pushes and registers belong to an enclosing call,
  because Watcom evaluates `f(g(x), 1, 2, 3, 4)` as `push 4; mov ecx,3; mov ebx,2; mov edx,1;
  <g(x) into eax>; call f`, and a register that isn't an argument survives a call
  (callee-saved). Nested calls use the inner result as `eax`.
- **Chained assignments** (`a = b = x`): the stored value stays readable after the store, but
  is never taken as an implicit call argument.
- **More than four parameters**: `ret N` gives N/4 stack parameters `a5..` read from `[ebp+8]`
  onwards, included in the signature.
- Regression check: the previous lifter ran alongside the new one and the reports were
  diffed. 71 functions had been lost by an over-eager register-parameter test for library
  callees; fixed, nothing lost now.

Batch **1,083**, unsupported down to 178.

## 2026-10-01: nested-block locals (+43)

Slot layouts like `R, P1, L` (a local *below* the parameters) are locals of a nested block
`{ int x; ... }`. The front end declares the function body's own locals with the function
(`CurFunc->u.func.locals`, with `.R`), and every `OPR_NEWBLOCK` is a nested block. The patch
marks autos declared from a block node as `DAGGER_NESTED` and gives them slots after the
parameters. Pending declarations now get slots at every `-od` statement, not just the first,
so a block's locals get theirs when it starts. The lifter declares locals deeper than every
parameter inside a nested block wrapping the body (jumping into a block with `goto` is legal
C89). Batch **1,126** (+43, nothing lost).

## 2026-10-01: types from the frame layout (+15)

`tools/slot_plan.py` explains a frame with the slot rule by trying every subset of parameters
as 2-byte. Locals' relative order is free (the lifter declares them in slot order), the
parameters' isn't. **The rule plus inferred 2-byte variables explains 2,229 of the 2,297 game
frames** (68 unexplained). About 115 functions need variables the access-based typing called
`int` declared `short` (a short read as a dword compiles to the same `mov eax,[x]` through
`*(int *)&x`), and the plan also names the nested-block locals.

The inferred types stay inside the function: callers keep the access-based prototype, because
changing a callee's prototype to `short` changed how 15 callers evaluate arguments.
`promote_lifted.py` gives a function whose definition differs from its callers' prototype a
file of its own. Batch **1,140**. Build **1,144 / 2,297, 23.39%**.

## 2026-10-01: locals' `l = l + K`, pointer post-increments (+27)

- **`l = l + K` on a local** still compiled in place. For a stack variable, `l + K` is folded
  into a temp-plus-offset address (`CL_TEMP_OFFSET`), and `GetValue()` (`cg/c/addrfold.c`)
  builds `ADD l, K` straight into the suggested destination `l`. Found by stepping through
  `MakeGets()` in the debug compiler (it takes the `NF_ADDR` branch for locals). The patch
  drops the suggestion when it's the operand (same switch, `DAGGER_RMW=1`).
- **`mov eax,[p]; add [p],K`** is a post-increment of a pointer to a K-byte object (`p++` over
  an array of structs). The lifter now writes `(*(char (**)[K])&p)++`, which compiles to that
  shape.

Batch **1,167**.

## 2026-10-01: stack-argument types and pushes (+38)

- **Stack parameters get types** from how the callee reads `[ebp+8..]`, just like slots. A
  `short`/`char` stack parameter makes callers push through a register
  (`mov eax,0x9c; push eax`), which OW reproduces once the prototype says so (+9).
- **Stack variables are pushed through a register**: Watcom 10 compiled a local argument as
  `mov eax,[ebp-x]; push eax` (133 times in the original) but pushed globals directly (70).
  `PushOneParm()` (`cg/intel/c/x86call.c`) now loads a 4-byte stack variable into a temp first
  at `-od` (`DAGGER_PUSHMEM=1` restores OW). +29, -1.

Batch **1,204**.

### Parked: sosez.c's calling convention

About 25 functions in `sosez.c` (the HMI sound wrapper) save only `esi`/`edi` and take their
arguments on the stack with caller cleanup. OW's `__cdecl` and `-3s/-4s/-5s` all also save
`ebx` and copy the stack parameter into a local at `-od`, so this needs its own compiler
study. Also parked: register assignment within a statement (Watcom 10 puts the first-evaluated
subexpression in `edx` and the second in `eax`; OW the reverse; about 20 KB). The default
register order is clearly best (a sweep of `KKND_REGORDER` only lost matches).
