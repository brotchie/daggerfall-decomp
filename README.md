# Daggerfall decompilation

A matching decompilation of *The Elder Scrolls II: Daggerfall* (DOS, 1996), starting with the game
executable `FALL.EXE` (version 1.07.213), built with Watcom C 10.0 and run under the CauseWay DOS
extender.

This repo contains no game code or data. You supply the executables; the tools check their SHA-1.

- [docs/head_start.md](docs/head_start.md): the research plan this project started from
- [docs/progress.md](docs/progress.md): a log of what has been found and done, newest last

## Status

- `FALL.EXE` 1.07.213 reproduced from Bethesda's free release (`c49a2ceb…`).
- Format: uncompressed LE behind the CauseWay stub. 3 objects, 37,520 fixups.
- 3,315 functions found in object 1 (98.4% of its bytes decoded).
- Compiler: Watcom 10.0/10.0a. Game code is unoptimised: `-od -s -of+ -4r`.
- Toolchain: KKND-Decomp's patched Open Watcom `wcc386` plus three `-od` patches of our own,
  built natively on macOS.
- **26 functions match byte for byte** (`src/`), and the rebuilt executable is
  identical: `tools/build-and-verify.sh` prints `build/FALL.EXE: OK`.

## Quick start

```sh
# 1. Game files: Bethesda's DFInstall.zip in orig/, then run the 1.07.213 patcher
#    headless in DOSBox-X (brew install dosbox-x)
tools/patch_213.sh orig/DFInstall.zip

# 2. Python tools
python3 -m venv .venv && .venv/bin/pip install -r requirements.txt

# 3. Patched compiler (clones open-watcom-v2 into third_party/, about 3 minutes)
tools/build_ow.sh

# 4. Find functions, match a C file against the original, then build and verify
.venv/bin/python tools/find_functions.py
.venv/bin/python tools/match.py src/leaf_probes.c
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
| `tools/build-and-verify.sh` | splice all of `src/` into `FALL.EXE` and check the SHA-1 (`build_fall.py`) |

Several tools are adapted from [KKND-Decomp](https://github.com/Wyrelade/KKND-Decomp) (CC0).
