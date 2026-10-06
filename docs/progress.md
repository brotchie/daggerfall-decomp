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

## 2026-10-01: lifter round (+40): bit tests, char parameters, `*p++`, pointer adds

- `tools/cluster_peek.py` prints one example of each top diff cluster with context (using
  `match.py`, which now flags only instructions whose unmasked bytes differ).
- **Bit tests on bytes/words read unsigned**: `(*(unsigned char *)x & 0x80)` compiles to
  `test byte ptr [x], 0x80`; the signed spelling sign-extends first.
- **The layout plan leaves 1-byte variables alone** (it had retyped a `char` parameter as
  `short`).
- **Post-increments used as values**: `mov eax,[p]; inc [p]; cmp byte [eax],0` is `*p++`,
  now a deferred `p++` expression feeding the next instruction (int `x++`, or
  `(int)(*(char (**)[K])&p)++` for a stride-K pointer).
- **An add of a dword global is pointer arithmetic** (`(int)(*(char **)G + i)`): Watcom 10
  never folds a pointer load into the add (`mov edx,[G]; add eax,edx`). Against the committed
  lifter this gains 42 and loses 2 (`func_00069E3C`, `func_0008C462`), where `int` arithmetic
  happened to give the same register assignment.

Batch **1,244**.

## 2026-10-01: `mov eax,edx; sar edx,31` instead of `cdq` (+70)

The original sign-extends for division with `mov eax,edx; sar edx,31` 427 times and uses `cdq`
twice. OW's `V_CDQ` check (`cg/intel/c/x86ver.c`) picks `cdq` for a 486 target and only
splits it (`rCDQ()`) for a 586 optimising for time. At `-od` it now always splits
(`DAGGER_CDQ=1` restores OW). Batch **1,314** (+70, nothing lost).

## 2026-10-01: idiom statistics (`tools/idiom_diff.py`)

`lift_all.py` now keeps each compiled function's bytes (`build/lift/<func>.bin`), and
`tools/idiom_diff.py` normalises our instructions and the original's (for the differing
functions) to shapes such as `mov r8, byte[ebp-x]` or `cmp byte[ebp-x], imm`, and ranks the
shapes by the difference in counts. That's how the `cdq` difference was spotted, and in its
first run it gave:

- `cmp byte[ebp-x], imm`: original 436, ours 150. A switch's compare tree on a byte temp uses
  unsigned jumps, and the lifter's explicit `(unsigned)` cast widened the compare. Narrow
  operands are now read unsigned instead (+16).
- `ret imm`: original 26, ours 0. Our slot-order patch gave stack parameters frame slots
  (OW copied them in). Only register parameters are recorded now (+4).
- Still open: `add r32, dword[ebp-x]` (ours +264), `test byte[ebp-x], imm` against
  `test dword[ebp-x], imm` (OW narrows an int bit test), `xor r32,r32` (+501).

Batch **1,334**.

Tried and reverted: declaring locals whose value is used as a memory base as `char *` (so
`add` doesn't fold them, like the global-pointer case): 0 gained, 9 lost against the committed
lifter. The `add r32, dword[ebp-x]` difference has some other cause.

## 2026-10-01: `add r,r` is `x * 2` (+59)

The `add r32, dword[ebp-x]` excess was mostly 2-byte array indexing: `mov edx,[x]; add edx,edx`
is `x * 2`, which the lifter spelled `x + x` (OW: `add edx,[x]`). Batch **1,393**.

## 2026-10-01: 16-bit zero-extension and compares (+5)

- Watcom 10 zero-extends a byte into a 16-bit register as `mov al,[x]; xor ah,ah` (load
  first); OW's `rCLRHI_R()` (`cg/c/split.c`) clears the high half first. Patched to append the
  clear at `-od` (`DAGGER_CLRFIRST=1` restores OW). Returning the clear as the next
  instruction to expand crashed the compiler; it now returns the load.
- The lifter writes that value as `(unsigned short)(unsigned char)x`, and a compare of two
  16-bit operands casts the left side to the jump's signedness so it stays a 16-bit
  `cmp ax, word [x]`.

Batch **1,398**.

## 2026-10-01: switch groundwork (translator), compiler work outlined

The lifter now decodes around switch tables inside a function (skipping `config/code_data.csv`
ranges), drops unreachable padding after a jump, and turns a table dispatch into
`switch (sel) {` with `case k:`/`default:` labels at the real target addresses inside one
switch body running to the end of the function (legal C; table entries then point at real
code, not `goto` stubs). The range check before the dispatch becomes the switch's own. One
table switch per function for now; sparse `repne scasb` switches aren't handled yet.

These functions now compile but differ. With a lower table threshold (`KKND_SWOPT=4`) OW emits
**the same dispatch sequence** as Watcom 10. Three compiler pieces remain:

1. **Threshold**: Watcom 10 used a table for 4 dense byte cases at `-od`. KKND's `Balance()`
   floor is 15.
2. **Selector temp**: Watcom 10's switch temp *is* the slot the lifter declares as a local
   (`l_1C = D; switch (l_1C)`); OW copies the selector into another temp. At `-od`, use a
   plain local selector directly.
3. **Table placement**: Watcom 10 emits a switch's table before the *outermost statement*
   containing it (`func_00015A50`: at the start of the body, with `jmp` over it, ahead of an
   enclosing one-case switch). OW emits it at the next dead spot inside the statement
   (`jmp ...; lea eax,[eax]; table`). Both pad with `mov eax,eax`/`lea eax,[eax]`/`nop`.

Worth about 36 functions (41 KB, 7% of game code).

## 2026-10-02: switch tables in the compiler

The three compiler pieces outlined above, now done:

1. **Selector**: at `-od`, `BGSelect()` (`cg/c/bldsel.c`) switches on a declared local
   directly instead of copying it through its "bogus add 0" temp (`DAGGER_SELCOPY=1`
   restores OW). With the lifter's `l = expr; switch (l)` the dispatch is now identical:
   `cmp byte [l],3; ja; xor eax,eax; mov al,[l]; shl eax,2; jmp cs:[eax+table]`.
2. **Threshold**: KKND's floor of 15 for the switch cost balance now also applies at `-od`
   (where `OptForSize` is 50).
3. **Table placement**: Watcom 10 put a table at the start of the top-level statement
   containing the switch, with a `jmp` over it if control could fall in. The lifter's `goto`
   style has no statements left, but it knows where each table sat, so it emits a marker
   label `__dagger_tblXXXX:;` there (and drops the original's `jmp` over the table). The front
   end (`cc/c/cstmt.c` `GrabLabels()`) turns that label into a marker node; `cgen.c` calls
   `DaggerTableHere()`, which records the peephole-queue position (`optmain.c`).
   `MakeJmpTab()` (`cg/intel/c/x86sel.c`) inserts the table there, preceded by a `jmp` when
   the previous instruction falls through. A marker before the prologue defers the table until
   the encoder reaches the first instruction after the parameter spills (`object.c`).
4. **Alignment**: tables are aligned like labels, by padding to the current location, but the
   object's offsets differ from FALL.EXE's addresses. Labels are now aligned as if the
   function sat at its original address, read off its `func_XXXXXXXX` name
   (`cg/intel/c/x86esc.c`, `DAGGER_NOALIGNBIAS=1` restores OW).

`func_00015A50` and `func_0002814E` match. Batch **1,405**. Most switch functions are still
held up by lifter gaps: several switches per function, sparse `repne scasb` switches, and an
assignment nested in a call argument.

## 2026-10-02: unit-relative table alignment, operand order and a search over it

- **Several switches per function** in the lifter: each `switch` closes the previous one's
  body, and case labels belong to the switch being emitted.
- **Table alignment is relative to the unit, not the image.** The tables in one source file
  share their address mod 4 (all of `links.c`'s are at 1, `guilds.c`'s at 2, `inven.c`'s at
  0): Watcom aligned them in the object, and objects were linked at arbitrary byte offsets.
  The marker label already carries the table's original address, so the front end passes
  its low bits along and `x86esc.c` pads the table to land on the same address mod 16
  (`DAGGER_NOTBLALIGN=1` turns it off). +6.
- **Shift counts**: `x << n` with an `int n` loads only `cl` (`mov cl, byte [n]`); the lifter
  now reads that as `n`, and `ecx` is no longer mistaken for a call argument afterwards.
- **Operand order of commutative ops.** `TNBinary()` (`cg/c/tree.c`) evaluates the left
  operand first when its tree has at least as many nodes (`kids`) as the right one, else the
  right one first. So the operand the code computes first was the left one in the source,
  unless it is the bigger one: then both orders compile to the same evaluation order and
  only the register allocation tells them apart. The lifter now tracks where each value's
  evaluation began, applies the rule, and records the ambiguous cases as *choice points*.
- **Choice-point search** in `tools/lift_all.py`: when a function differs, flip the choice
  points nearest the first difference one at a time and keep a flip when the first
  difference moves later (up to 80 compiles per function). CPU instead of turns: the whole
  batch still runs in 7 s. Further ambiguities can become choice points the same way.

Batch **1,432** (from 1,405); build **1,435 functions, 36.85%** of game code.

## 2026-10-02: compare-tree switches

Most of FALL.EXE's switches aren't jump tables: a census over the game code finds 139
compare trees against 19 plain tables. Watcom copies the selector into a temp (`mov ax,[p];
mov [t],eax`, or `mov [t],al` for a char) and binary-searches the case values on it
(`cmp word [t],12; jb L1; cmp word [t],12; jbe Lcase12; ...`), sometimes ending in a table.
The lifter used to turn that into `l_t = ...; if (...) goto ...;` chains on a declared local,
which compiles to different code and a different frame.

- **Lifter** (`find_cswitches`/`parse_ctree`): a slot stored and then compared at a narrower
  width, touched by nothing but the tree, is a selector temp. The tree is walked tracking
  the set of selector values on each path (jcc signedness picks the sort); leaves give the
  case values, the target with the most values is the default, jump tables inside give
  their entries, and in-tree `jmp default` stubs are recognised as part of the tree. The
  output is `switch (expr) { case ...: }` with the temp's slot left undeclared.
- **Nested switches**: open switches are a stack; a switch inside a case body stays open
  until a label of an outer switch, and an inner switch whose default is shared with the
  outer one ends there.
- **Nested-block locals after a switch**: Watcom gives the selector temp its slot when the
  switch is reached; locals that sit below it were in a block that began later, so the
  lifter now opens that block at the first statement using them (inside the switch, closed
  with it).
- **Strategy choice** (`cg/c/bldsel.c`): Watcom's size/time balance for `-od` code fits
  31..33 over all 158 switches once the selector's own width caps `SelType` (char
  selectors: +2 bytes for a table, no extra compare byte), so the `-od` balance is now 32.
  Since the lifter marks every table's position anyway, a pending table mark now also forces
  a table and its absence rules one out (`DAGGER_NOSWFORCE=1` turns that off). A debug
  print of the costs is behind `DAGGER_SWDEBUG=1`.
- `lift_all.py` reports first differences decoding around tables and walking both
  instruction lists in step; `match.py` does the same; `--only` runs update the report
  instead of replacing it.

Batch **1,449**; build **1,452 functions, 37.86%**. 53 of the 122 functions with a switch
match; the rest mostly fail elsewhere (frame size, register choice, unsupported idioms),
and the `repne scasb`/`scasw` scan switches are still unsupported.

## 2026-10-02: scan switches, selector copy

- **Selector copy**: with the lifter now switching on expressions, the earlier patch that
  let `switch (local)` use the local directly is wrong in general: Watcom 10 copies even a
  parameter into the selector temp (`mov eax,[a1]; mov [t],eax; cmp word [t],5`). OW's copy
  is the default again (`DAGGER_SELDIRECT=1` for the old behaviour); the batch is unchanged.
- **Scan switches** (`repne scasb`/`scasw`): `find_functions.py` now records the value
  tables (`kind=scanvalues` in `config/code_data.csv`, three of them), so the lifter no
  longer decodes them as code. The lifter reads a scan dispatch (`mov al,[t]; mov ecx,n+1;
  mov edi,offset values; repne scasb; jmp cs:[ecx*4+labels]`; values stored largest first,
  label 0 the default) as a node of the compare tree and marks the tables with
  `__dagger_scnXXXX:` (XXXX = the value table). In the compiler, `MakeScanTab()` is placed
  and aligned at the mark like a jump table, including the deferred case after the prologue
  (from a copy of the select node), and a scan mark forces the scan strategy.
  `func_00070EC0` matches; the other two fail elsewhere.
- Fixed: the forced strategy fell back whenever `kind` was 0, which is `U_SCAN`.
- Int-width selector temps (`cmp dword [t]`) are taken for switches only with a table or at
  least three cases: a local assigned once and compared once looks the same.

Batch **1,451**; build **1,454 functions, 37.97%**.

## 2026-10-02: register lifetimes across statements and calls

- **Clear-high order**: Watcom 10 zero-extends a byte into a 16-bit register by clearing
  the high byte first (`xor dh,dh; mov dl,[x]`), as OW does; the earlier "clear after"
  patch is now opt-in (`DAGGER_CLRAFTER=1`). The lifter accepts the `xor dh,dh` before the
  load. +3.
- **Callee arity from saved registers**: a Watcom-compiled callee pushes every register it
  uses that isn't a parameter, so a runtime function that saves `edx` takes at most one
  register argument. Values kept in `edx`/`ebx` across such a call are no longer taken for
  arguments. +19.
- **Chained assignments**: `x = (p->f = 0)` stores and immediately re-reads the field
  through the same base register. The lifter keeps the address registers alive after a
  store and folds an immediate re-read of a non-stack lvalue into `(lhs = v)`. Pointer
  additions are now choice points too. +25.

Batch **1,479**.

## 2026-10-02: sosez.c's convention, narrowed tests

- **sosez.c/profile.c convention solved**: stack arguments, caller cleanup, only `esi`/`edi`
  saved, parameters used in place: `#pragma aux sosconv "*" parm caller [] value [eax]
  modify [eax ebx ecx edx]` reproduces the prologue exactly. The lifter detects it (only
  esi/edi pushed, no register spills), names the stack parameters `a1..`, and emits the
  pragma; `promote_lifted.py` puts a convention's definition before its uses. +8.
- **Narrowed tests**: `test ax,ax` on a call's int result is `(short)f(...) != 0` at the
  source level; the lifter casts a wider value used through a 16/8-bit register. +15.

Batch **1,502**.

## 2026-10-02: high-byte operations and bit-fields

- **High-byte immediates**: `and ah,3` / `or ah,0x80` / `add ah,0x19` are 16-bit operations
  whose constant leaves the low byte alone (`x & 0x3ff`, `x | 0x8000`, `x + 0x1900`); the
  lifter reads them so. +6.
- **The game uses bit-fields.** `mov ax,[p+2]; and ah,3` and `mov ax,[p+2]; shl eax,2;
  shr ax,13` are exactly Watcom's bit-field reads. The lifter emits per-use structs
  (`struct bf16_11_3 { unsigned short _:11; unsigned short f:3; }`) and reads
  `((struct bf16_11_3 *)p)->f`; a value widened with `cwde`/`movsx` was a plain `short`
  field (`struct bfs16_...`). Watcom 10 extracted such a signed field like an unsigned one
  but kept its signed type (`and ah,3; cwde`), where OW sign-extends with shifts: the code
  generator now does the former at `-od` (`TNBitShift`, `DAGGER_SIGNEDBF=1` for OW's).
  `promote_lifted.py` carries the struct declarations.

Batch **1,509**.

## 2026-10-02: byte masks, parameter types from call sites

- `and dl,0x80` on a 16/32-bit value keeps the high bits: the source was `x & ~0x7f`
  (`(a + 1) & ~1` is `and al,0xfe`). The lifter had read it as `x & 128`. +3.
- **Parameter types from the callers.** A caller converts each argument to the parameter's
  declared type, so `movsx edx, word ptr [x]` before a call means a `short` second
  parameter, even when the callee only ever reads the low byte. `lift.caller_types()`
  surveys every call site once (cached in `build/lift/caller_types.json`); a narrow load can
  also be a narrow value passed to an `int`, so the callee's spill width decides (char
  parameters are spilled as bytes, short ones as dwords). Callers' prototypes take this
  type; the definition keeps the type its frame shows, and `promote_lifted.py` gives such
  functions a file of their own. +56.
- `lift_all.py` imports the lifter and builds the shared caches before starting the pool,
  so a broken `lift.py` fails at once instead of hanging.

Batch **1,568**.

## 2026-10-02: parameter type choices, char parameters, indirect calls

- **Short or int parameter?** A parameter spilled as a dword and only read as a word is a
  `short`, or an `int` read through `(short)` casts; the two differ only in where the frame
  puts it (2-byte parameters first). It is now a choice point at the spill, settled by the
  batch search. +20.
- **Char parameters go first too**: 1-byte parameters sit at the top of the frame with the
  2-byte ones (a census of the prologues: always at the top, mostly last to first), so the
  compiler's small-parameter group includes them (`DAGGER_CHARPARMBIG=1` for the old
  order). +8. (Char *locals* in that group: +2 -1, left off, `DAGGER_CHARAUTOSMALL=1`.)
- **Indirect calls**: none of the 52 functions with a `call [p]` matched. The C front end
  declares a hidden auto symbol (`.F`) for every indirect call, and at `-od`
  `ForceTempsMemory()` gives every front-end temp a frame slot. Watcom 10 didn't: the
  symbol is now declared without a slot (`DAGGER_INDSLOT=1` restores it). +16.

Batch **1,612**.

## 2026-10-02: short variables are stored as dwords

- **Indirect calls with stack arguments** (four registers, then the stack) lift.
- **Left-over register copies aren't arguments**: after `mov ebx,eax` (a call's result
  saved) and `mov edx,ebx`, ebx still holds the same value but is not a third argument. +6.
- **Watcom 10 stores a 2-byte stack variable with the whole register.** In FALL.EXE every
  store into a slot that is only read as a word is a dword store (281 from registers, 41
  immediates, zero-extended from 16 bits), except where an `int` is narrowed to a `short`
  (`call f; mov [l],ax`). The code generator now widens such stores at `-od`
  (`DaggerWidenStore()` in `object.c`, when the previous instruction computed the value
  as 16 bits; `DAGGER_WORDSTORE=1` keeps OW's). The lifter types a slot read only as a
  word as `short` and assigns 16-bit values to it directly; whether such a slot is a
  `short` or an `int` read through `(short)` casts is a choice point. +12.
- (Unsigned globals from a census of their compares: net +1, off by default,
  `LIFT_UGLOBALS=1`.)

Batch **1,624**.

## 2026-10-02: per-function code generator switches, searched

Register allocation differs from Watcom 10 in ways that vary per function: reversing the
order in which equal-savings temps get registers (`KKND_CONFREV`) fixes 17 functions and
breaks 321 when applied everywhere. So the switches became per-function knobs:

- `#pragma dagger <SWITCH> <function>` (front end, `cpragma.c`) turns a code generator
  switch on for one function. All `DAGGER_*`/`KKND_*` switches in the code generator now go
  through `DaggerEnv()`, which checks the environment and then the pragmas for the function
  being generated.
- The lifter offers 17 of them as function-level choice points; `lift_all.py` tries them
  whenever nothing near the first difference helps. Settled so far: `DAGGER_DEADDEF` (21
  functions), `KKND_CONFREV` (13), `DAGGER_FIRSTUSE` (6), `DAGGER_PUSHMEM` (5),
  `DAGGER_RMW` (2). +46 in all. The batch takes 19 s.
- Fixed a leak between functions found by the full build (the batch compiles functions
  one at a time): the per-function list of slotless indirect-call symbols could survive into
  the next function when its `proc_def` reused the address. Per-function lists now reset at
  `CGProcDecl()`.

Batch **1,670**; build **1,671 functions, 47.90%**.

## 2026-10-02: more knobs, array extents, packed arrays

- Five more register-allocation knobs (`DAGGER_NOSAVES`, `DAGGER_REGLAST`,
  `DAGGER_NOGIVEN`, `DAGGER_CONFLIST`, `DAGGER_CONFLISTREV`): +2.
- **Array extent** is a choice point: a slot accessed directly between a local array and the
  next variable is an element or a variable of its own.
- **Call arity** choice points both ways: fewer arguments when a register is read after the
  call (kept across it), more than the callee's saved registers suggest (a callee may save
  an argument register). Neutral so far; they combine with other choices.
- **Odd-sized local arrays are packed**: `char a[14]; char b[30];` sit back to back in
  FALL.EXE, where OW rounds every local to 4 bytes. At `-od` local arrays and structs are now
  rounded to 2 bytes (`DAGGER_ROUND4=1` for OW, `DAGGER_ROUND1=1` for none; both 2 and 1
  give the same result). +7.

Batch **1,679**; build **1,680 functions, 48.31%**.

## 2026-10-02: post-increments everywhere, int parameters read as chars

- `arr[G++] = x` (`mov edx,[G]; inc [G]; shl edx,2; ...`): post-increments used as values
  now work through any register and with `inc`/`dec` and word operands, not just
  `mov eax,[p]; add [p],K`. +18.
- A parameter spilled as a dword is never a `char` (OW spills `char` parameters as bytes):
  it is an `int` read through `(char)` casts; callers still see the narrow type they
  convert to. +18.

Batch **1,715**.

## 2026-10-02: pointer spellings that steer evaluation order

- A pointer read from memory is spelled `*(char **)x`, not `(char *)*(int *)x`: the int
  load and cast keep Watcom from folding `p + 71` into `lea edx,[eax+0x47]`, and they change
  the tree size that decides evaluation order. A choice point per use (default the former).
  +7.
- Stack arguments pushed as immediates (`push 0x9c`) are `int` parameters (a narrow one
  goes through a register): the call-site census now covers stack arguments. +2.
- **`p->arr[i]`**: with the field offset written next to the pointer (`p + 367 + (i << 2)`)
  the pointer side is the bigger tree and is evaluated first, as in FALL.EXE
  (`mov edx,[G]; mov eax,[i]; shl eax,2; add eax,edx; mov eax,[eax+0x16f]`). The lifter
  tags pointer additions and offers this as a choice point at the memory access. +27.

Batch **1,751**.

## 2026-10-02: masks and unfolded loads

- `mov eax,[p+0x1f]; and eax,0xffff` is `x & 0xffff` on an int field: the
  `(unsigned short)` cast makes the compiler load a word instead. The lifter uses the mask
  for dword reads. +11.
- A dword read kept in a register for an add (`mov eax,[p+0x88]; add eax,edx` rather than
  `add edx,[p+0x88]`) is a pointer: Watcom 10 never folds a pointer load into an add. The
  lifter now spells such adds as pointer arithmetic by default (a choice point). +8.

Batch **1,770**.

## 2026-10-02: tree stubs, char returns, implicit int

- Compare trees also contain `jmp case` stubs (with dead `jmp` filler between them), not
  only `jmp default`: any jump a compare reaches inside the tree's own stretch of code is
  part of the tree. +3.
- Negative constants pushed through a register mark a signed narrow stack parameter. +2.
- The search now takes the flip that moves the first difference furthest at each step (not
  the first that helps), with a budget of 200 compiles. +2.
- **Char and short return values**: `mov al, byte ptr [ebp-x]` before the epilogue is a
  `char` function's return variable. +8.
- A `void` function whose eax a caller uses was called without a prototype in scope
  (implicit `int`): such callers declare it `extern int f();`, and `promote_lifted.py` gives
  a function whose declarations conflict with its unit's a file of its own.
- Address-taken stack parameters no longer leak into the local slot table.

Batch **1,786**; build **1,786 functions, 54.55%**.

## 2026-10-02: byte compares, 16-bit variables, nested-block split

- **Nested-block locals split at a switch temp**: locals above the temp's slot were in a
  block opened before the switch, those below it in one opened inside it. `func_00029958`
  (6.4 KB) now differs only in one call's argument order. +1.
- Stores in the middle of an argument list become comma expressions (`f(g((t = a, t << 2)),
  ...)`) when arguments are held in registers or pushed: 14 functions lift that didn't
  (none match yet: register allocation under pressure differs, see below).
- `test x,x; jle` is `x <= 0`; a `neg`/`not` of the return variable right after it is set
  folds into the `return`. +4.
- A failed compare-tree parse on a slot used elsewhere is just a local, not a switch. +1.
- **Byte compares**: `mov al,[p]; cmp al,[c]` needs both operands of the same char type
  (mixing signed and unsigned char promotes both to int). +11.
- A slot stored whole right after a 16-bit computation (`mov ax,[x]; mov [l],eax`) is a
  2-byte variable even when read whole (`l & 1` loads the dword); a choice point when it is
  also read as a dword. +4.

Open: Watcom 10's register allocation under register pressure (outer call arguments held
in esi/edi/ecx across an inner call, temps spilled to the stack) differs from OW's in
which register each held value gets; 16-bit division (`cwd; idiv bx`) for short operands
(OW always promotes); floating point (a handful of functions).

Batch **1,807**.

## 2026-10-02: subtraction of constants as a knob

FALL.EXE has both `sub [x],5` (68) and `add [x],-5` (31) on memory, so the source said
`x -= 5` in some places and `x += -5` in others. OW rewrites one into the other three
times over (`FoldMinus`, `foldins.c`, and the `V_OP2NEG` reduction back to `sub`). The new
`DAGGER_KEEPSUB` switch keeps in-place memory updates as written; applied everywhere it
breaks address folding (`[eax-1]`), so it is a per-function knob the search tries. +10.

Batch **1,817**.
- **Tests of stack variables stay whole**: every `test` of a stack slot with a constant in
  FALL.EXE is a dword test (36 of 36), where OW narrows `test dword [ebp-x],0x8000` to
  `test byte [ebp-x+1],0x80`; byte tests are on other memory (char fields). The code
  generator no longer narrows tests of stack variables at `-od`
  (`DAGGER_NARROWTEST=1` for OW's). +9. Batch **1,826**.

## 2026-10-02: shape census

`tools/shape_census.py` compares instruction shapes (mnemonic and operand kinds) between our
output and FALL.EXE over the functions that don't match yet. Findings acted on:

- `sub [ebp-x],k` never occurs in FALL.EXE (`add [ebp-x],-k` does): in-place constant
  subtraction on stack variables stays an add (`DAGGER_STACKSUB=1` for OW).
- 8-bit shifts (`add al,al; shr al,6`) are byte bit-field reads; the lifter emits
  `struct bf8_S_L { unsigned char _:S; unsigned char f:L; }` reads.
- An address-taken slot with one of its bytes read separately is a 4-byte array.
- Byte arithmetic between two variables (`add al,dl`, `add al,[x]`) is far rarer in
  FALL.EXE than in our output: Watcom 10 demoted less. Global changes to demotion lose
  hundreds of functions; `DAGGER_NODEMOTE` and `DAGGER_NOCVTDEMOTE` are per-function knobs
  (+1).

Batch **1,827**.

## 2026-10-02: pinned frame slots, a last resort

About 90 functions differed only in where their variables sit in the frame, and the slot
rules derived so far contradict each other on them (a short parameter sits with the int
ones in one function and above the 2-byte locals in another). Rather than keep guessing,
the search now has a last resort: `#pragma dagger slots <function> <var> <depth> ...`
gives every declared variable (and `ret`, the return variable) its depth below the saved
registers in FALL.EXE. The code generator reserves that region and places the variables
exactly (`DaggerAlloc()` in `bldcall.c`); compiler temps are allocated under it. The lifter
offers it as a function-level choice point, so it is used only where nothing else
reproduces the frame: 98 functions so far, +47 matches.

This makes those functions' layout an annotation rather than a consequence of the source,
which is worth revisiting once the layout rules are better understood: the pragmas mark
exactly the cases they don't explain.

Batch **1,876**.

## 2026-10-02: stack variables vs other memory

Two more places where Watcom 10 treated stack variables differently from other memory:

- `and`/`or`/`xor` of a constant into a stack variable keep its full width
  (`xor dword [ebp-x],1`), like tests; OW narrows to a byte. +4.
- `x++` as a statement loads the old value first only for stack variables (`mov eax,[l];
  inc [l]`); through a pointer it is a plain `inc [p]`. The dead load is now limited to
  stack variables and their members (`DAGGER_DEADDEFMEM` restores it everywhere, also a
  per-function knob). +8.

Batch **1,888**; build **59.28%**.
- **Mixed-signedness compares**: `(int)(unsigned short)x != (int)(short)y` written with
  explicit casts makes OW compare 16 bits; Watcom 10 compared the extended ints, which is
  what OW does for the implicit promotions (`*(unsigned short *)x != *(short *)y`). The
  lifter now writes such compares without the `(int)` casts. +15. (Dropping the casts
  everywhere loses 612: they're right almost everywhere else.) Batch **1,905**.
- **Pre-increments**: an in-place `inc`/`dec` of a stack variable without the dead load of
  its old value is `++x`/`--x` (Watcom 10 loads the old value only for post-increments),
  e.g. `if (--n == -1)` is `dec [n]; cmp [n],-1`. +3. Batch **1,908**.
- `f(g(), x, y)` with x and y loaded before the inner call: whether an earlier call's result
  still in eax is the next call's first argument is a per-call choice point (a runtime
  function with no arguments looks the same). +4. Batch **1,912**.
- `xor ah,ah` / `xor ax,ax` on a wide value are masks (`x & 255`, `x & -65536`), and a
  word read masked to a byte is `x & 255`. +6. Unsigned in-place `shr`, and unused char
  parameters typed by their spill width. +3. Batch **1,921**.
- **No lea with an index and a displacement**: FALL.EXE never folds a following
  `add reg,k` into an lea (`lea eax,[eax+eax*4]; add eax,5`, `shl eax,3; add eax,8`); OW's
  2004 GetNextAddConstant() does. Gated off at -od (`DAGGER_LEADISP` restores it). +6.

Batch **1,927**; build **61.34%**.
- **Short arithmetic in 32-bit registers**: `mov ax,[x]; sub eax,4; mov [l],eax` (and
  `xor ah,ah; sub eax,0x21; ...`) is a short variable assigned short arithmetic; Watcom 10
  also updates a short stack variable with the whole register (`add [l],eax`, never
  `add word [l],ax`): the compiler widens 16-bit add/sub/and/or/xor of a 2-byte stack
  variable with a register (`DAGGER_WORDRMW` keeps them), and the lifter types such slots
  short. Slot-type choice points are now tried when the first difference is near any access
  of the slot. +5. Batch **1,932**; build **61.44%**.
- **Indirect calls without arguments**: in `call [eax+D]` the register is (part of) the
  function's address, not an argument: the lifter stops the argument list there (a choice
  point keeps it). +22. Slot-type and array-extent choice points are tried near any access
  of their slots, and the search budget is 600 attempts. +3. Batch **1,956**.
- **Call results narrowed to a byte**: `call rand; and al,1; xor edx,edx; mov dl,al` is
  `rand() & 1` passed as an unsigned char (OW does the and at 8 bits); and when eax's value
  was already used before a call of unknown arity, whether it is also an argument is a
  choice point. +9.
- **Pointer plus constant**: `mov esi,[p]; add esi,14; add eax,esi` is `*(char **)p + 14 +
  i` (the pointer form puts the index in the other register). +8. Batch **1,972**.
- **Switches on int variables**: Watcom compares an int (or unsigned) selector variable
  itself, without the temp it copies a short or char selector into. The lifter recognises
  such a compare tree by the binary search's repeated compare at its root (`cmp v,3; jb;
  cmp v,3; jbe`), bounded to the run of compares and jumps (a case body may compare the
  variable again); a choice point keeps the chain of ifs. +6. Batch **1,978**.
- **Constants pushed through a register**: `mov eax,0x32; push eax` passes a narrow stack
  parameter, so the callee's parameter is a short even when it only ever reads it whole
  (the census also looks further back for calls with many stack arguments). +7.
  Batch **1,985**.
- `xor ah,ah` right before `mov al,[x]` clears the high byte for that load even when eax
  holds an unrelated value (an idiv quotient). +6. Calls with no prototype in scope
  (`mov ebx,eax` passing an int result to an unsigned char parameter): a per-callee choice
  point declares the callee `extern int f();` in that function. +3. lift_all keeps each
  function's winning flips (build/lift/flips.json) and retries them when a fresh greedy
  search ends worse. Batch **1,994**.
- Ordered word compares (`cmp dx,[x]; jb`) give both operands the jump's signedness. +3.
  A sum used as an address is pointer arithmetic or an int sum (a choice point:
  `D + ((t[i] * 2) + i * 14)`). +5. Batch **2,002**.
- Relocation mismatches now report where they are, so the choice point search runs on
  them too. Two more per-function register allocation knobs: `DAGGER_CONFPOS` /
  `DAGGER_CONFPOSREV` break ties in savings by where each live range starts (globally each
  loses ~300; per function +5). Batch **2,007**.
- The search also tries a nearby choice point together with each compiler knob, at every
  new first difference it gets stuck on. +4. Batch **2,011**.
- **Short functions**: a 16-bit value stored whole into the return variable (`mov ax,[x];
  mov [ret],eax`) marks a short function (its return variable is read back whole either
  way), unless callers use the result whole (a census of the instruction after each call:
  `cwde` says short, eax used as an int says int). +3. Batch **2,014**.
- Short multiplications: `movsx ax,byte [x]` is a short operand, `imul edx,eax` joins the
  16-bit chain of a short variable's store, and the compiler widens a short store right
  after a MUL (OW multiplies shorts as U4). +2. Batch **2,016**.
- A function the lifter gives up on at its first attempt gets the nearest choice points
  flipped one at a time (a register kept across a call, say), then the usual search. +3.
  Batch **2,019**.
- `mov dx,[eax+6]; mov eax,[p]; dec word [eax+6]` is a post-decrement used as a value even
  with the pointer reloaded in between. +3. `cmp word [x],-1` compares with a short
  constant (`x == (short)-1`; OW widens a compare with a negative int). +3. The compiler
  no longer narrows `and word [ebp-x],0xfffe` of a stack variable or local array to a byte
  (FALL.EXE has none). Batch **2,025**.
- A value is 16 bits if any operand of the whole-register arithmetic producing it is (a
  small recursive walk back from the store): `mov ax,[x]; inc eax; mov edx,[l]; add edx,eax;
  mov [m],edx` stores a short. +3. Batch **2,028**.
- Two post-increments in one expression (`*q++ = *p++`: `mov eax,[p]; inc [p]; mov edx,[q];
  inc [q]; mov al,[eax]; mov [edx],al`): the first one's old value waits in its register
  instead of being flushed as a statement. +6. Batch **2,034**.
- **Ternaries in call arguments**: `f(a, c ? x : y, g, 2)` compiles the `?:` into a temp
  through a branch diamond, and makes Watcom evaluate the arguments to its right into temps
  first (right to left). The lifter turns `if (c) goto A; t = x; goto B; A: t = y; B:
  f(.., t, u, w)` with single-use slots back into the call (only when arguments were
  spilled: an if/else on a variable of its own compiles the same diamond). +9.
  Batch **2,043**.
- `mov [eax+4],edx; neg dword [eax+4]` with the address not reloaded is `p->f = -y`
  (Watcom stores, then negates in memory). +5. Batch **2,048**. (The compiler also gained
  a `DAGGER_DUMPINS=1` dump of the instruction stream before register allocation, for
  studying allocation differences.)
- **Pointer plus an offset, back to int**: `(int)(*(char **)p + 2048) - q` costs a register
  move for the conversion (`mov ebx,eax`) that the int sum doesn't: a choice point at
  `add reg,imm` on a loaded dword. +9 (one of them 6 KB). `and`/`or`/`xor` join the
  16-bit chains. +1. Batch **2,058**.
- **Which operand is the pointer**: in `mov edx,[g]; mov eax,[p]; mov eax,[eax]; add eax,edx`
  both operands are dword reads; OW evaluates a pointer sum's pointer side last, so which
  one was the `char *` decides the register order. A choice point when both are reads. +14.
  Batch **2,072**.
- **Switch table marks mid-block**: a `__dagger_tbl` mark after a statement that falls into
  a label was taken before that statement's code existed (OW builds a block before it
  generates it), so the table landed in front of the statement. The front end now emits a
  label before the mark, ending the block first. +4 (three of them 1.7–2.8 KB).
  Batch **2,076**.
- Locals deeper in the frame than a switch's selector temp were declared in a block opened
  inside the switch (a choice point when the frame has no parameters to tell). +2.
  Batch **2,078**.
- **Register pins (last resort)**: `#pragma dagger reg <function> <k> <register> ...` makes
  the k-th register choice of the function's allocator take that register (when it is free
  to). At a difference that is only a register name, the search compiles once with
  `DAGGER_REGLOG=1` to list the choices, then pins a choice that took our register to the
  original's (or one that took the original's to ours, or swaps a pair). Like the slot
  pins, this is a hint to the compiler, not a change to the C. +9 (and 2 more seen in
  testing). Batch **2,087**.
- Register pins are tried at any first difference with differing registers, not only pure
  renames. +7. Batch **2,094**.
- More register pins: a pin with the next choice held at its old register, pins at
  differences whose registers don't line up (`mov edx,[g]` against `mov edx,eax`), and an
  offline wide search (`LIFT_PINWIDE=1`, ~8 min) over nearby pairs of choices whose
  results the flip cache keeps. A compare chain jumping back to a loop's continue is ifs,
  not a switch (no switch case precedes its dispatch). Unsupported first attempts flip the
  choice points nearest where the lifter stopped. `DAGGER_RIGHTPREF` knob. +20.
  Batch **2,106**.
- `add eax,eax` twice on an index is a short-array subscript (`((short *)p)[i * 2 + 1]`; OW
  folds a written `(i * 2) * 2` into a shift). +4. The winning choice-point flips of the
  matched functions are committed as `config/lift_flips.json` (the search is greedy and
  some finds came from slow offline runs), and lift_all starts from them. Batch **2,110**.
- `p->arr[i]` is now spelt with the field offset next to the pointer by default
  (`*(short *)(*(char **)p + 48 + i * 2)`: the pointer side is evaluated first), the old
  spelling a function-level choice point (cached flips migrated to keep it). +6.
  Batch **2,116**.
- Functions that never return (an endless loop, no epilogue: OW omits it too) lift; a
  void function's "result" is never an argument; C that fails to compile gets its call
  arity choice points flipped. Taking every choice point of one kind the other way is a
  function-level choice for three kinds (a sweep inverting each kind's default globally
  found no better default, but four functions that need one inverted). +3.
  Batch **2,119**.
- A parameter in a 2-byte parameter's frame position that is only ever read whole is an
  int whose slot was given early (an address-taken one): typed int with its slots pinned.
  +1. Batch **2,120**.
- `lea eax,[eax+eax*2]; add eax,eax` on an index into shorts is a subscript of `x * 3` (OW
  turns a written `* 6` into imul). +2. The compiler no longer merges adjacent constant
  byte/word stores (`buf[0] = '0'; buf[1] = 'x'` stays two stores, as in FALL.EXE;
  `DAGGER_MERGESTORES` restores OW's merge). Batch **2,122**.
- The last argument register is not an argument when its value was already used (an index
  added into the first argument's register): a choice point, by default not. A pointer
  post-increment whose old value is a call argument (`f(a, b, p++)`) is recognised. +1.
  Batch **2,123**.
- `test al,0xff` on a call's int result is `(f() & 255)`; an int variable masked
  (`mov eax,[l]; and eax,0xff`) is `l & 255`. The search also backtracks (an earlier flip
  taken back, with a nearby new one) and its budget is settable (`LIFT_BUDGET`; a larger
  one finds nothing more). +1. Batch **2,124**.
- `DAGGER_NODEMOTE` (keep arithmetic wide when a result is narrowed) no longer applies to
  compound assignments: `p->flags |= 2` stays a byte `or` (`DAGGER_NODEMOTERMW` for the
  old behaviour). +2. Batch **2,126**.
- An in-memory `dec`/`inc` while a computed value waits in a register, read right after, is
  `--x` inside the expression (`x = --x % (a + b)`: the divisor is evaluated first). +1
  (2 KB). Batch **2,127**.
- A branch diamond assigning a slot that lies in the compiler's temp region of the frame
  (below every declared variable) is a `?:` even outside call arguments. +2 (one 6.4 KB).
  Batch **2,129**.
- Ternary recovery also takes compound conditions: a chain of `if (c) goto` to the two
  arms becomes `(a || b)` / `!a && b` (put back if no ternary comes of it). No change in
  matches yet. Batch **2,129**.
- **x87 floating point**: the lifter keeps a stack of double expressions (`fld`/`fild`,
  `fadd`/`fsub`/`fmul`/`fdiv` and their reversed and popping forms, `fcomp` + `fnstsw` +
  `sahf` compares, `fstp`/`fistp`), with ints converted through a temp (`mov [t],eax;
  fild [t]`), `__CHP` truncation for `(int)` conversions (mapped to 0xA167C for the build),
  library functions taking and returning doubles on the stack
  (`#pragma aux f parm routine [] value [8087]`), and `fld y; fsubr [x]; fstp [x]` as
  `x -= y`. +5. Batch **2,134**.
- FP refinements: a `fistp` temp read once later, a word `fild` of a dword-stored temp
  (`(short)x`), double constants pushed as two dwords, no phantom local in a double's upper
  half. (3D2B6 and 9830F still differ in where the double and the conversion temps go.)
- **Short division**: Watcom 10 divided two shorts at 16 bits (`mov eax,[l]; cwd; idiv bx`);
  the compiler now demotes a division whose operands are only widened shorts or small
  constants (`DAGGER_NODIVDEMOTE` keeps OW's), and the lifter reads `cwd`/`idiv r16`. An
  address-taken variable read as a word is a short (a choice point). +2. Batch **2,136**.
- **Far pointers**: `mov ebx,ds` marks the selector half of a far pointer: a register pair
  (offset, `ds`) passed to a call is `(void __far *)(void *)p` (the callee declared without
  a prototype), and a stored selector is `_FP_SEG((void *)p)` (OW's i86.h inline). +1.
  Batch **2,137**.
- The two DPMI helpers at 0x99648/0x99662 were assembly in the original (no frame, `int
  31h`): written by hand in src/int.c as `__declspec(naked)` functions with `_asm` bodies.
  Build **2,139** functions.
- Struct assignments: `lea edi,[dst]; mov esi,src; movsd ...` is `*(struct sN *)dst =
  *(struct sN *)src` (a local initialised from data kept in the code segment). A store
  ending a diamond arm is never a comma expression, and comma stores folded into a
  diamond's condition come out before it, so ternary call arguments with pushed stack
  arguments are found. +2. Batch **2,139**.
- Watcom 10 pushed a compiler temp (a `?:` result) straight from memory (`push dword
  [ebp-x]`) and only declared variables through a register; the compiler now does too
  (`DAGGER_PUSHTEMPREG` for the old). Register pins are tried nearest the difference's
  share of the function first. And the search now **starts from a previous run's flips**
  when they get further (before, they were only a fallback), so finds accumulate across
  runs. +15. Batch **2,154**.
- The flip cache keeps each function's furthest progress (a later first difference, in
  build/lift/flips_at.json), so repeated runs deepen the search; a few more runs and a wide
  pin pass converge. +4. Batch **2,158**.
- A register used as an address since it was loaded (`mov edx,[g]; sub eax,[edx+0xb]`)
  has been consumed: not a further call argument. +5. Batch **2,163**.
- Stack-parameter decrements without a dead load are pre-decrements (`--a5`). +2.
- **Allocation-order windows (last resort)**: `#pragma dagger confwin <function> <k1>
  <k2>` allocates the conflicts at sorted positions k1..k2 latest-starting first (Watcom
  10's order for e.g. `(f(42)->x + f(108)->x) / 2`: the sum in the second operand's
  register). The search tries windows around the difference's share of the function
  before register pins (`confsort` lines in `DAGGER_REGLOG`). +3. Batch **2,167**.
- Unsupported down to the two asm helpers (written by hand): high byte registers are byte
  values of their own (`mov ah,[x]; shl ah,7; add al,ah`); a small hand-written callee's
  arity is the argument registers it reads before writing (`imul edx` leaf: two); a call
  with fewer loaded registers than the callee's parameters is unprototyped. An offline
  deep search (`LIFT_DEEP=1`: every allocation window and every single register pin,
  ~16 min over the failing functions) found nothing more. +4. Batch **2,171**.
- A short variable whose address is taken or whose single bytes are read keeps 16-bit
  stores (`mov word [l],0`): the compiler doesn't widen those (`DAGGER_WIDENADDR`). +1.
  Batch **2,172**.
- More switches on variables: case ranges (the root compares different constants, the
  tree's dead `jmp default` stubs give it away), stack-parameter selectors, and two-case
  trees with stubs. +4. Batch **2,176**.
- A range case landing on a dead `jmp default` inside the compare run is the default, not
  a case of its own. `?:` arguments whose `&&` chain falls through a `goto` trampoline to
  the else arm. Sums of two calls are ordered like other commutative operands. +3.
- Compound assignments through the spilled address temp (`*(T *)t = *(T *)(t = p + k, t)
  | x`) are `p->f |= x`, so `x` can be a ternary; `xor bl,bl` on a word keeps its high
  byte; `used_as_base` reads an instruction's memory operand before its destination. +2.
- Sign hints count only for reads of the variable's own size (a zero-extended word read
  no longer makes an int unsigned); the search also tries pairs among the seven nearest
  choices, closest first. +1.
- **Zero-extended reads spelled `*(unsigned short *)p`** (a choice point at each `and
  eax,0xffff`): the `(int)(unsigned short)*(short *)p` cast adds a tree node, and TNBinary
  evaluates the bigger operand first, so the spelling decides operand order in compares
  and sums. +10. Batch **2,194** (88.7%).
- A stack parameter some callers push through a register and others as an immediate is
  narrow (the immediate pushers have no prototype in scope).
- **OW: a commutative op stored to memory** (`g = p + x`, x computed before the global p
  was read) takes x's register in Watcom 10; `rUSEREGISTER` now swaps the operands then
  (`DAGGER_NOUSEOLD` for the old). +4. Batch **2,198** (88.8%).
- Short locals: `add dword [x],imm` is `x += k` on a short too; a 2-byte local below a
  4-byte one always pins the slots; an int stored to a short whose address isn't taken
  can be a plain assignment (a choice). `f(g(), x, y)` by default. Bit-field stores
  (`and byte [p+7],0xbf` + `or dword [p+4],(v&1)<<30` is `p->f = v`). +3.
- **OW: an address operand is loaded after the other operand** (`&g + ((i<<2) + 4)` adds
  the 4 before `mov reg,offset g`; BGBinary generated the left operand first because a
  clang-built compiler evaluates `MakeBinary( GenIns( left ), GenIns( rite ) )` left to
  right). Compares nothing branches on are empty ifs (`if (x == 63) {}`). +4.
- The search's backtracking may drop register pins found against an older compiler; a
  search from scratch (`LIFT_SCRATCH=1`) found one more. +3.
- **Two-dimensional global arrays**: a sum used as the base of `[reg + g]` is an int index
  by default, and `g + (x*48 + y*4)` can be spelt `(g + x*48) + y*4` (the left-associated
  `g[x][y]` evaluates the first index first). +4. Batch **2,212** (90.1%).
- A caller widening an argument right before pushing it (`xor ah,ah; cwde; push eax`)
  passes a short. Assignment chains through a reloaded pointer (`p->a = (p->b = v)`, the
  word re-read then widened or stored on). +5. Batch **2,217** (90.6%).
- x87: floats (slots the FPU stores and loads as 4 bytes; `fstp m32; fld m32` of a temp
  rounds to float; `(int)(float)((float)x + y)` stored back to x is `x += y`), two filds
  combined are `(double)a / b`, `fistp qword` read as a dword is the unsigned conversion.
- **OW: per-function knobs with names over 15 characters never worked** (DaggerFuncOpt kept
  15 characters of the name): DAGGER_WORDSTORE, DAGGER_CHARPARMBIG, DAGGER_CONFPOSREV and
  five more now switch on per function. The widened short store also follows int sums
  and int variables (`s = s + i / j`, `s = i`). +6.
- Selector-temp switch trees treat jumps in their own run as stubs; explicit empty cases
  (values a compare sends straight to the default's code shape Watcom's tree) and extra
  register arguments to a callee taking fewer are choice points. OW: widened byte/word
  compares against out-of-range constants are no longer folded (`(uchar)x == -1`), and
  DAGGER_CALLFIRST (a knob) generates an operand making a call first. +6. Batch **2,227**
  (91.5%).
- Bit-field tests (`test byte [g],K`, a contiguous mask: only a bit-field test makes it),
  `p + i` / `i + p` address order and global arrays of short pairs as choices. Batch
  **2,242**.

## The real compiler: Watcom C32 10.0a

- The remaining functions are not hand-written assembly. Compiling their lifts with the
  real Watcom C32 10.0a (`tools/wcc10.py`: the 1995 compiler under DOSBox-X) shows plain C
  code generation throughout, so the gap is in our emulation of 10.0a, not in the source
  language.
- **FALL.EXE was compiled with `-d2`.** Debug info explains the dead `mov eax,[i]` before
  `inc dword [i]` (the value of `i++` kept for the debugger), the address of `*(int
  *)0x46c` going through a stack temp, and the frame order: with `-d2` the return value's
  slot is on top, then locals last to first, then spilled parameters last to first.
  Without `-d2` the order is first to first (`LIFT_DECLTOP=1`).
- The probes also settled: a far pointer second argument goes in ebx:ecx with the next int
  in edx (a lifter choice); `==` of two converted values stays wide (OW patch); an
  argument evaluated before a call is spilled to a temp of the compiler's own.
- `DAGGER_CC=w10 DAGGER_W10EXTRA=-d2` makes `tools/match.py` and `tools/lift_all.py` use
  the real compiler; in that mode the lifter drops the compiler's own temps (`(t = a, t)`
  is `a`; `*(int *)1132` needs no variable).
- **`src/w10/`**: functions only 10.0a reproduces (OW does not place its temps the same
  way). The build compiles them with the real compiler, `-d2` added, when it is installed
  and skips them otherwise (the original bytes stay, the checksum still matches). 4D402,
  7193D, 99922. Build **2,248** (93.77%).
- The operand computed first can be padded with a no-op when it looks smaller
  (`g & 128 & 255`; a lifter choice). DOSBox-X at `cycles=max` makes a 10.0a compile
  0.7 s (0.08 s each in a batch), so searching against the real compiler is cheap.
  `tools/w10_try.py` tries many variants of one function in one DOSBox run;
  `tools/wcc10.py` also runs the build's relocation check.

## 100%: the last 48 functions with the real compiler

The last 48 functions were matched with Watcom 10.0a by six agents working in parallel,
each from the lifter's attempt with the real compiler as the judge. Most were rewritten as
ordinary C (structs, typed prototypes, `for` and `switch`); all are in `src/w10/`. Build
**2,297 / 2,297** (100%), FALL.EXE byte-identical. What they found about 10.0a, most of it
the reason patched OW could not get these:

- **Register allocation and the stack frame both go through Watcom's unstable ShellSort**
  (`cgsrtlst.c`). The conflicts of a block are sorted by savings, so swapping the operands
  of a `+` or `*` gives the same instructions but can move esi/edi anywhere in the block,
  earlier statements included; one more or one fewer temp (an `(int)` cast on a pointer,
  `*(int *)D + 28` against `*(char **)D + 28`) does the same.
- **Frame layout** (`tools/w10_frame.py`): register parameters in order, then the locals in
  declaration order, then the return slot; shell-sorted by size, largest first; allocated
  from the bottom up, the last element nearest ebp. Inner-block locals and compiler temps go
  below. Unused locals still take slots: some matched files keep one or two.
- **Short locals are stored as dwords** (`mov dword [s],0`, `inc dword [s]`) and read with
  `movsx word`: such a slot is a plain `short`, not an int with casts.
- **Evaluation order**: the operand with more tree nodes first. A struct member at a
  nonzero offset, an array index other than 0, a cast or a mask (`lo & 0xffff`) add nodes;
  `p->arr[i]` and `*(p + K + i)` are different trees. The result lands in the left operand's
  register.
- **Types steer allocation**: callees need real prototypes (`char *` strings, `short` and
  `unsigned char` parameters: a constant for an `unsigned char` stack parameter is pushed
  through a register); arrays of pointers typed as pointers; far pointers with `MK_FP`.
- **Control flow is never threaded at -od**: a jump to the loop increment is `continue`,
  `if (a) if (b)` and `if (a && b)` differ, `A || (B || C)` and `(A || B) || C` differ, a
  switch selector in a user slot is `l = e; switch (l)`, explicit empty cases shape the tree.
- **Missed arguments**: a remainder left in edx by `idiv` is not an argument, but an edx
  load or a push just before a call usually is (three were missing in the lifts).
- 684E9 multiplies by a literal double: 10.0a puts it in the module's own constants, which
  the build cannot accept (every reference must resolve to FALL.EXE's), and with the
  constant as an extern it emits `fld; fmulp`. `(float)(x * K)` stored to a double gives
  the original `fmul [K]` bytes (the conversion emits no code there); the file says so.

## The real compiler only (branch w10-only)

- The patched Open Watcom is gone (tools/owpatch, tools/build_ow.sh, the lifter's compiler
  knobs and register pins): everything is compiled by Watcom C32 10.0a under DOSBox-X, `-d2`
  now in config/cflags.txt. A full build is one DOSBox-X run, about 40 s. The two assembly
  helpers are src/int.asm, assembled by 10.0a's WASM (which encodes reg-to-reg moves as
  `89 /r`; the original's assembler wrote `8b /r`, so those five are spelt as bytes).
- Of the 2,246 functions matched under OW, 1,994 matched 10.0a as they were; re-lifting the
  rest for 10.0a matched 75 more. src/w10/ is now src/hand/.
- Watcom 10.0a's code for a function depends on its unit's declarations (a callee with or
  without a prototype), so tools/settle_lifted.py promotes, builds, and moves a function
  that fails inside its unit file to a file of its own (config/lift_alone.txt); six matched
  only in one unit context and went back on the list. Build **2,063** (77.46%).
- The lifter now picks the declaration order that Watcom 10.0a's frame sort turns into the
  original's slots (tools/lift.py `frame_order`, unused locals for gaps): +27; an
  address-taken slot inside a bigger array is one of its elements, and a slot difference
  searches the whole frame's choices: +1. Build **2,091**.
- The other 206 went to eight agents (src/hand/). Rules they established, beyond the first
  round's:
  - **Switch tables are aligned to 4 from the start of the object's code segment** (padding
    `nop` / `mov eax,eax` / `lea eax,[eax]`), so a function with a jump table or a `repne
    scas` value table matches only at its original offset mod 4. Such functions are
    compiled together with their real predecessors: `src/hand/run_<last>.c` holds the
    unit's functions in address order from the nearest one at the unit start's alignment
    (20 runs, 82 functions). Two hand-written units (keys.c, generate.c) lost their only
    function to a run.
  - **Stack parameters take part in the frame sort** (two entries each, right after the
    register parameters, neither given a slot); the **return slot is created at the first
    `return`** and `*(int *)0x46c` address temps where they are first used, both in the
    sorted list; ?: and switch temps and inner-block locals go below, in creation order;
    unused inner-block locals take no slot (unused function-level ones do).
  - Short locals ever read as bytes (`isalnum(s)`) are stored with word moves; taking a
    short's address does the same.
  - `while (n--)` is `dec [n]; cmp [n],-1`; `--n` and `n -= 1` are `add [n],-1`; `s += e`
    on a short differs from `s = s + e`; `x /= 6` from `x = x / 6`.
  - A `!` around a whole condition is free, one nested inside `&&`/`||` makes a 0/1 temp;
    `if (A && B)` always has the jump-over-jump stub, nested ifs jump straight to the end.
  - Mask tests on ints are narrowed to the byte or word they touch (`test byte [m+1],0x20`),
    on unsigned char/short fields they are not; a narrowing cast as a condition gives
    `test byte` too.
  - Library callees identified: A0F5C sprintf, A1023 memcpy, A0E0D memmove, A0040 memset,
    A0AD9 strncpy, A0DD9 itoa, A0DF4 strlen, A1079 memchr, A00AF malloc, A0024 free,
    A13DA/A13F7 _dos_findfirst/_dos_findnext, A1004 unlink, 6CDAB open, 6CE0D creat,
    A00CB read, A0B42 write, A006E lseek, 9DEA7 close, 9DEAC abs; D_00178630 is `_IsTable`.
  Build **2,295** (99.56%).
- The last two:
  - **8D497**: `l_30 = a1->top * a1->h2 / a1->count;` with `short l_30`. The I4→I2 convert
    before the (dword) store adds one conflict to the block, which reorders 10.0a's unstable
    sort of the block's conflicts: `top` then gets edx and coalesces with the product. A
    destination's type is a register-allocation lever even when the code is the same.
  - **77B9E**: case 16 is `if (func_0009DC25() % 1) func_0006974E(c ? A : B); else
    func_0006974E(c ? A : B);`, a random pick from one choice. The front end keeps the `if`
    (the call has side effects), the code generator folds `% 1` to 0 and drops the dead
    branch, but the dead branch's `?:` temp keeps its slot: a frame slot no instruction
    touches. (The dead branch's own strings left no trace; it is written as a copy.)
  - Temps get their slots when code is flushed at front-end control flow (if/else, labels,
    goto/break, case labels), newest first before the first `switch`, in instruction order
    after it; an unused inner-block local gets a slot only before the first switch body.
- **Build 2,297 / 2,297 (100%), every function compiled by Watcom C32 10.0a**; merged into
  main.


## 2026-10-03/04: reverse engineering — every game function named

The decomp is byte-identical. This phase is about what the code does. The headless game
(tools/fallemu.py on a patched Unicorn, about 70x stock) drives a loop: reverse
engineering finds game state, the state makes playing and calling the code more useful, and
that gives more evidence.

**Running the code:**
- fallplay.py plays a session step by step.
- fallfuzz.py plays at random and stopped helping after its first run.
- fallcall.py calls any function in a live game from a safe point between frames. Its sweep
  ran all 1282 functions that play never reached; 89 of them drew their screen, among them
  the death video, the court and the bank deals.
- fallassets.py ties every file read to the code that made it.

**Evidence:** fallevidence.py builds a page for each function covering:
- what makes it run, and what a direct call does;
- its button, its messages and sounds, the assets it reads;
- its strings, callers and callees.

**Naming:**
- Eleven agents took the code one system at a time, over three rounds. They proposed
  names in config/names.csv, each with a confidence and its evidence, and confirmed the
  doubtful ones by experiment: write tests, direct calls, replays.
- All 2297 game functions are named: 440 confirmed, 1274 strong, 583 candidate. With
  globals and record fields, 3707 names.

**Findings** are in docs/state.md:
- the 71-byte record header (Daggerfall Unity's RecordRoot) and the records built on it:
  character, creature, item, spell, quest, faction, bank, region, location;
- the main loop, game modes, key map, message boxes and text macros;
- the spell effect and quest opcode tables, AI states and formulas;
- save files. Most tables match Daggerfall Unity value for value. A few original bugs
  turned up, and two places where Daggerfall Unity differs.

**Infrastructure found along the way:**
- A memory leak in parallel emulator runs took the machine down; tools/memwatch.py and
  per-worker guards now prevent it.
- An emulator file-search bug had hidden the quest files from the game.
- About 40 functions are filed under the wrong source unit (docs/state.md lists them).

**Next:**
- Name the library functions by matching them against the Watcom 10.0a runtime.
- Audit the candidate names.
- Put the names and record structs into the source, with the matching build checking each
  step.
- XnGine.

**Names into the source, and src/lifted/ frozen** (2026-10-04):
- apply_names.py wrote 2508 confirmed and strong names into the C. config/symbols.txt
  carries them for the build, and FALL.EXE stays byte-identical.
- libmatch.py named 233 library functions against Watcom 10.0a's own libraries.
- src/lifted/ is now ordinary source rather than lifter output. Record structs need edits
  a regenerator would overwrite, and every function is already matched. promote_lifted.py
  refuses to overwrite it. The lifted originals are in git history (before 9bd35ae).
- A pilot rewrote disease_infect's record accesses with include/records.h's
  `struct record` and compiled to the same bytes.

## 2026-10-04/05: names quality, structured C, record structs

**Names quality (phase 2):**
- A consistency pass made 127 decisions and set a naming convention (docs/naming.md).
- Four audits re-examined every candidate, mostly by experiment: direct calls, write tests,
  watches, surveys of the 18 saves, Daggerfall Unity's tables and formulas, and the game's
  own help texts.
- Game functions went from 440 confirmed / 1274 strong / 583 candidate to 530 / 1637 / 130.
- Several names said the opposite of the code: floor/ceiling, clear/occupied, and a "lock"
  that never locks.
- XnGine's 719 functions are named (docs/xngine_map.md). config/names.csv holds about 5200
  names; 4147 of them are in the source.

**Record structs:**
- include/records.h covers 25 record types plus unions on the header fields.
- Phases A to C converted the code: offset casts went from about 7900 to 1530.

**Natural C:**
- tools/structure.py rewrote the lifter's gotos as if/while/for (11969 -> 71).
- tools/type_globals.py typed 1273 globals.

Every change compiles byte-identical with Watcom 10.0a, so the build checks all of it.


## 2026-10-05: natural C (phase 3) and XnGine in C (phase 7)

**Natural C (phase 3)** is done. Every change compiled byte-identical with Watcom 10.0a, and
the matching build checked all of it (docs/natural_c.md, docs/structs.md):
- Six agents named every lifter placeholder local and parameter (29,964 `l_XX`/`aN` to 0)
  and retyped the ones holding records and strings.
- Two struct rounds replaced file-local copies and the remaining offset casts with shared
  structs:
  - include/structs.h (new): file formats, UI, sound, collision, templates;
  - include/records.h gained the logbook, the RMB and RDB block files, and `struct region`.
  - Offset casts went from 1530 to 2, and file-local struct definitions from 305 to 50.
- tools/protos.py made each file's declarations agree with the function's definition where
  the code allows: 959 disagreements down to 309. The rest are load-bearing: the callers'
  code was compiled against other types.
- Left: 71 gotos (jumps into shared code, two-level exits), the 2 casts through XnGine's
  allocation header, and a few spellings kept because they match.

A build risk the last round found: `rdb_model_id_from_name` (src/hand/func_000369A0.c)
compiled once with ecx and edx swapped in the full build. It matched alone, and matches now
in every order tried. Watcom 10.0a seems to read memory left over from the previous compile in
the DOSBox session. If a header change flips it again, padding or reordering the headers is
the fix.

**XnGine in C (phase 7)** is done (docs/xngine.md "phase 5, literal C"):
- Names: all 719 functions are named; the engine map is docs/xngine_map.md.
- Record corpus: tools/xn_record.py has 8,204 recorded calls of 620 functions.
- tools/xn_c.py translates every function to literal C on a register struct. It compiles with
  Watcom 10.0a.
- Replay: all 8,204 records pass with one function at a time in C, and with all of XnGine in
  C at once.
- Differential tests: 4,033 cover functions the records never reach.
- The game runs with XnGine in C: 8 saves × 2,000 ticks, and a still scene is
  pixel-identical to the asm.

**Next:** readable XnGine C (locals instead of the register struct, structs, loops for the
unrolled spans), and retyping the callers behind the 309 disagreeing declarations.

## 2026-10-05: XnGine as readable C

All of XnGine is now readable C, in `src/engine/` (docs/xngine.md "phase 6", the guide
docs/xngine_readable.md).
- **Calling convention.** The C is written as a person would write it and compiled by Watcom
  10.0a. It runs natively on the game's own stack: the asm calls it through `#pragma aux`
  register interfaces.
- **Interfaces.** `tools/xn_abi.py` derives the interfaces from liveness over both objects.
  The corpus checks them: every claimed clobber scrambled at 9M returns, every non-input at
  entry.
- **Who did what.** An infrastructure agent built the tools and the vec/mat/math pilot. Two
  analysis agents wrote the engine's data structures (64 structs) and designs for its
  self-modifying, unrolled and generated code (docs/engine/). Six agents then converted a
  group of subsystems each.
- **Coverage.** 648 of 719 functions run as C. The other 71 are blocks inside functions (their
  function's C covers them) or data.
- **Records.** With all of it routed at once, every one of the 8,204 records passes, compared
  by each function's interface.
- **Lockstep frames.** The new `xn_rc.py frames` runs the game a frame at a time, asm and C
  from the same machine with the same timer interrupts, and compares all of memory. Every
  frame matched on all 18 saves: still, walking and turning.
- **Kept on purpose.** The original bugs and quirks are kept and commented (the first piece of
  every billboard is never drawn, the serial receive returns the ring's first byte, ...). So
  are the values callers see: leftover registers, scratch globals, patched code bytes and the
  generated code's bytes.

**Next:** with no asm left in the engine, a pass can drop what only the asm needed:
- the register interfaces and the `_r` adapters;
- leftovers no game code reads;
- code patching and generation (the texture mapper and the light shaders as plain C);
- data at fixed addresses, which can become C definitions.
It would be tested against the game's calls into the engine and lockstep frames.

## 2026-10-06: XnGine as canonical C, shown equivalent at its boundary

The readable C of phase 6 still answered to the asm: register interfaces, `_r` adapters,
patched code bytes, generated code, leftover registers. This pass made it canonical C, plain
modules that keep only what the game can see (docs/xngine_canonical.md, the overview
docs/engine/architecture.md).

**How it was built:**
- An infrastructure agent mapped the engine's boundary with the game (config/xngine_boundary.csv:
  181 entries, the exits, and the memory the game reads, found by running it). It also built
  boundary comparisons, 39 lockstep scenarios with input inside the frames, asm coverage, and test
  shims that keep per-function equivalence once interfaces change. It piloted all of this on
  vec/mat/math.
- Five groups converted the rest, and were merged by the coordinator.
- A data agent moved the engine's data into C (src/engine_data/, docs/engine/data.md): computed
  tables by code; initial data loaded from the user's own FALL.EXE, so no game bytes are committed.

**Result:**
- **The code:** 648 functions, all canonical. Register files remain only at the DOS/BIOS boundary
  and in 8 catalogued game-facing adapters. Every function and module is documented, and every
  declaration agrees with its definition.
- **Quirks:** 149 original quirks are kept or dropped and catalogued (docs/engine/quirks.md), with
  one deviation (D-VID-01, DPMI page unlocks the game cannot see).

**Equivalence:**
- 8,880 recorded calls replay through shims.
- The game's calls: 2,819 / 2,819.
- Scenarios: 1,675 / 1,675 frames, identical on the screen, game memory and I/O.
- Pure helpers: 140 specs and 217 million samples (89 million exhaustive), no difference.
- Coverage: 98.1% of the asm's reachable blocks are exercised, the rest justified one by one.
- The data module equals FALL.EXE's load image symbol by symbol.

Two things found on the way:
- The game copies 30 bytes of the compiled light shaders through a stale collision pointer every
  frame (Q-LIGHT-07). So the C still writes the shaders' image, as data.
- Unicorn keeps code it has translated, so a stop address or rewritten code needs a hook, not
  emu_start's `until`.

**Next:** an SDL port starts from docs/sdl_handoff.md.
