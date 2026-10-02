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
