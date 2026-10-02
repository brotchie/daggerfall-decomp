# Daggerfall decompilation

<!-- progress:start -->
![decompiled](https://img.shields.io/badge/decompiled-90.41%25-green) ![functions](https://img.shields.io/badge/functions-2215%20of%202297-green) ![FALL.EXE](https://img.shields.io/badge/FALL.EXE-matching-brightgreen)

90.41% of the game's own code (524460 of 580076 bytes, 2215 of 2297 functions) is matched C; the rebuilt `FALL.EXE` is byte-identical to 1.07.213.
<!-- progress:end -->

A matching decompilation of *The Elder Scrolls II: Daggerfall* (DOS, 1996), starting with the game
executable `FALL.EXE` (version 1.07.213), built with Watcom C 10.0 and run under the CauseWay DOS
extender.

This repo contains no game files (executables or data). You supply the executables; the tools
check their SHA-1.

- [docs/head_start.md](docs/head_start.md): the research plan this project started from
- [docs/progress.md](docs/progress.md): a log of what has been found and done, newest last

## Status

- `FALL.EXE` 1.07.213 reproduced from Bethesda's free release (`c49a2ceb…`).
- Format: uncompressed LE behind the CauseWay stub. 3 objects, 37,520 fixups.
- 3,315 functions found in object 1 (98.4% of its bytes decoded): 2,297 of them are the
  game's own C in 84 original source units (names recovered from `__FILE__` strings), the
  rest are libraries (MemCheck, Watcom runtime). Object 2 is the XnGine engine in asm.
- Compiler: Watcom 10.0/10.0a. Game code is unoptimised: `-od -s -of+ -4r`.
- Toolchain: KKND-Decomp's patched Open Watcom `wcc386` plus our own `-od` patches (`tools/owpatch/`),
  built natively on macOS.
- Matched game functions and the share of game code: see the badges at the top (updated by
  every successful `tools/build-and-verify.sh`). Most functions are lifted automatically
  (`src/lifted/`, from `tools/lift.py`); the rebuilt executable is byte-identical.

## Quick start

```sh
# 1. Game files: Bethesda's DFInstall.zip in orig/, then run the 1.07.213 patcher
#    headless in DOSBox-X (brew install dosbox-x)
tools/patch_213.sh orig/DFInstall.zip

# 2. Python tools
python3 -m venv .venv && .venv/bin/pip install -r requirements.txt

# 3. Patched compiler (clones open-watcom-v2 into third_party/, about 3 minutes)
tools/build_ow.sh

# 4. Find functions and units, match a C file, then build and verify
.venv/bin/python tools/find_functions.py
.venv/bin/python tools/find_units.py
.venv/bin/python tools/split.py                # asm listings in asm/nonmatchings/
.venv/bin/python tools/lift_all.py               # batch: lift, compile, check
.venv/bin/python tools/promote_lifted.py         # matched → src/lifted/
tools/build-and-verify.sh
```

## Tools

| Tool | Purpose |
|---|---|
| `tools/patch_213.sh` | build the 1.07.213 `FALL.EXE` from `DFInstall.zip` |
| `tools/build_ow.sh` | build the patched Open Watcom toolchain (`tools/owpatch/`) |
| `tools/le.py` | LE loader: objects, pages, fixups (from KKND-Decomp) |
| `tools/find_functions.py` | function discovery → `config/functions.csv` |
| `tools/split.py` | one symbolic asm listing per function → `asm/nonmatchings/` (gitignored) |
| `tools/omf.py` | OMF object reader (from KKND-Decomp) |
| `tools/match.py` | compile a C file and compare its functions with `FALL.EXE` |
| `tools/cc_dis.py` | disassemble what the compiler makes of a C file |
| `tools/find_units.py`, `tools/units.py` | recover the original source units; map an address to its unit |
| `tools/lift.py`, `tools/lift_all.py` | lift `-od` code to C; batch lift, compile and check every game function (2 s) |
| `tools/promote_lifted.py` | write matched lifted functions into `src/lifted/<unit>.c` |
| `tools/update_readme_progress.py` | refresh the README badges from `build/progress.json` |
| `tools/build-and-verify.sh` | splice all of `src/` into `FALL.EXE` and check the SHA-1 (`build_fall.py`) |

Several tools are adapted from [KKND-Decomp](https://github.com/Wyrelade/KKND-Decomp) (CC0).
