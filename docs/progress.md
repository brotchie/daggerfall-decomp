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
