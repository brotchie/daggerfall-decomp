# XnGine (object 2)

Object 2 of `FALL.EXE` is the XnGine engine: hand-written asm, shared with Battlespire and
Redguard. This file is the plan and running log for it, kept apart from `docs/progress.md` (the
game C) while the work lives on the `xngine` branch.

## Plan

Two layers in one source tree:

1. **Matching asm** (`src/xngine/*.asm`): modules that reassemble with Open Watcom's wasm to
   the original bytes and fixups, with every address a symbol. This is the reference for the
   byte-identical build.
2. **Equivalent C**: each function rewritten in documented C and proved equal to its asm, by
   differential testing against an emulator first and then, where it pays, by a solver. This is
   what the native port and fuzzing build on.

A function counts at five levels: matching asm, documented, equivalent C (tested), proved,
ported. Phases:

| Phase | Work | Gate |
|---|---|---|
| 0 | Map object 2: code, data, modules, entry points | every byte is code, data or a named table |
| 1 | Matching asm: `tools/xn_disasm.py`, `tools/xn_link.py`, build splice | `FALL.EXE` byte-identical with object 2 from asm |
| 2 | Name the self-modifying patch fields | every patch write targets a named label |
| 3 | Test harness: DOSBox-X scene dumps, Unicorn frame replay | the asm replays itself; no unknown writes to code |
| 4 | Document: names, header comments, shared types with the game C | no `func_`/`D_` names left |
| 5 | Equivalent C, by batch: literal C for every function, then readable C | all functions pass replay and fuzz tests |
| 6 | Proofs: code-integrity frame, patch removal, per-function equivalence | each function proved or its gap listed |
| 7 | Native port hand-off | captured scenes render the same natively |

## What object 2 holds

Measured with `tools/xn_disasm.py` on 1.07.213:

| Fact | Value |
|---|---|
| Range | 0xC0000-0x161568 (660,840 bytes, 79% zero) |
| Functions | 363: the 348 in `config/functions.csv`, 10 more reached by calls, 5 found by gap filling |
| Code | 20,857 instructions, 69,569 bytes |
| Calls from game C into XnGine | 952 sites, 174 entry points |
| Calls from XnGine into game C | 66 sites, 14 targets |
| Self-modifying writes | 87 sites in 14 writer functions, patching 75 instructions in 17 functions; 77 fill the placeholder `0x186A0` |

Error strings name the subsystems: `DOS:` file loading (0xC0B00), `WORLD:` (0xC2300),
`FONT:` (0x12DA00), `SET:` texture sets (0x132F00), `ENGINE:` shaders (0x136500), the 3D object
loader (0x13F700), `GFX:` mode setting (0x142900), `SYSTEM:` memory (0x147900), and a precision
timer whose messages match Michael Abrash's Zen timer (0x15F60C). The first 0x30 bytes of the
object are a table of 12 message pointers; the rest of that page is zero (the "zero page" the
engine checks for wild jumps).

## 2026-10-02: phase 1, matching asm

All of object 2 is now built from source, and `FALL.EXE` is still byte-identical.

- **`tools/xn_disasm.py`** writes 113 modules to `src/xngine/`, cut at 0x100-aligned addresses
  after zero padding (`config/xngine_modules.csv`). Code comes from recursive descent; a call
  into another object in bytes nothing reaches marks a gap as code (5 functions), and an
  instruction that would cut a fixup field in two is data, not code.
- **Every address is a symbol.** Each LE fixup becomes `func_X`, `L_X` (code) or `D_X` (data);
  a target inside an instruction is `label+k` (the patch fields). Each module declares
  `public` the 1,120 labels other modules use, so the set links with a real linker too.
- **No game bytes in the repo.** Plain data is written `B_<address>_<length>`; at build time
  `tools/xn_link.py` defines those macros from your `FALL.EXE` (`build/xngine/*.inc`). Zero runs
  are `db n dup (0)`.
- **`tools/xn_link.py`** assembles each module with `bwasm`, writes its fixups as the linker and
  LE loader would, and requires identical bytes and the same target for every LE fixup in the
  range (16-bit selector fixups excepted: the LE table keeps them). `build_fall.py` calls it and
  splices the modules in; the `--blank` self-test and two negative tests (a changed constant, a
  pointer to the wrong table) behave.
- **Encodings.** A probe pass assembles each module with an `org` back to the original offset
  after every instruction, so one mis-encoded instruction cannot shift the rest; those are
  written as `db` (symbol fields stay `dd` expressions). 45 instructions need it, and they say
  something about the original assembler:

  | Instruction | Count | Original | wasm |
  |---|---|---|---|
  | `shr reg,1` | 20 | `C1 /5 01` (imm8 form) | `D1 /5`. Typical of a shift count that was a forward-referenced constant |
  | `sbb reg,reg` | 13 | `1B /r` | `19 /r` |
  | `cmp ax,imm` | 9 | `66 3D iw` (accumulator form) | `66 83 /7 ib` |
  | others | 3 | data decoded as code (an error string after a call that does not return) | |

Next: phase 2 (name the 87 patch fields) and phase 3 (the replay harness).

Run: `.venv/bin/python tools/xn_disasm.py` regenerates `src/xngine/` (4 s);
`.venv/bin/python tools/xn_link.py [module]` checks modules; `tools/build-and-verify.sh` builds.
