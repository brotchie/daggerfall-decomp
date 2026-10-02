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
| Functions | 657 (`config/xngine_functions.csv`): 348 in `config/functions.csv`, 58 more reached by calls, 100 through pointers in data (dispatch tables, callbacks), 151 nothing references directly |
| Code | 38,380 instructions, 133,169 bytes |
| Calls from game C into XnGine | 952 sites, 174 entry points |
| Calls from XnGine into game C | 66 sites, 14 targets |
| Self-modifying writes | 303 from 46 functions into 256 instructions of 56 functions (`config/xngine_patches.csv`) |

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

## 2026-10-02: phase 2, the self-modifying code

Every write into code now targets a named field, and `config/xngine_patches.csv` lists all 303
(field, patched instruction, original value, kind, writer). The census needed a better code
map first: the first one stopped at 20,857 instructions because a lot of the engine is reached
only through dispatch tables, callback variables or nothing at all.

- **Code discovery**, in order: calls; an undecoded call into game C marks its gap as code;
  pointers in data or `offset` immediates whose target decodes as code (a clean path to `ret`
  or `jmp`, no string-like instructions, no fixup cut in two); and code after a `ret` past
  alignment padding (`nop`, `xchg ebx,ebx`). An address instructions read or write as memory is
  never a function entry; discovery restarts without any it took for one. This found 309
  functions beyond the 348 listed, and code went from 69 KB to 133 KB.
- **Patch fields**: an operand the code rewrites is `patch_<address>`, defined next to its
  instruction (`patch_1584AD equ L_1584AB+2   ; rewritten at run time`); a module that writes
  another module's field defines the name from that instruction's public label, since wasm
  cannot export an equate. Writers read like a setup list:

  ```asm
  func_0012A2D0:
      pushad
      mov dword ptr [D_000CEA28], eax
      ...
      mov dword ptr [patch_1584AD], eax
      mov dword ptr [patch_158608], eax
  ```

- **Kinds** (193 of the 303 writes patch a different function than the writer):

  | Kind | Writes | What |
  |---|---|---|
  | placeholder | 202 | Immediates assembled as round numbers and overwritten: 100000 (193), 10000 (4), 100 (4), 1000 (1). In `add`, `mov`, `cmp`, `imul`, `sub`, `shl` |
  | address | 63 | A memory operand's address rewritten (`mov`, `add`, `mul dword ptr [x]`, `lea`) |
  | opcode | 36 | Opcode bytes of 15 unrolled loops: `0xC3` (`ret`) planted at a computed entry, the original opcode (`8B`, `88`, or a saved word) put back after the call |
  | operand | 2 | A shift count (18) and a step (320, the screen width) |

The busiest writers are setup routines: `0x12A2D0` (29 writes), `0x12A100` (25), `0x13E8C8`
(20). What the static census cannot see (writes through registers, generated code) is phase
3's job: replay flags every write into code.

## 2026-10-02: phase 3, a headless FALL.EXE

Phase 3 needs XnGine's real inputs, so instead of dumping memory from DOSBox-X, the game now
runs headless in `tools/fallemu.py`: Unicorn for the CPU, with the extender, DOS, BIOS and PC
hardware the game touches written in Python. It boots `FALL.EXE z.cfg` to the main menu in
about a billion instructions (50 s, 20 MIPS) and saves the screen as a PNG.

- **Memory as CauseWay sets it up.** The game is zero-based and flat: it reads the BIOS tick
  count at 0x46C and video memory at 0xA0000 directly, so its objects load above 1 MB and are
  relocated through the LE fixups. They load at 0x01000000 + their preferred address, so
  `func_00012345` runs at 0x01012345. The six selector fixups in object 2 get the flat data
  selector.
- **Services:** DOS files over `build/game` (a clone of the patched install) with writes going
  to `build/emu/overlay`; DOS memory, dates and directory search; about 25 DPMI functions;
  CauseWay's `FF25h`/`FF26h`/`FF30h`; BIOS video, keyboard and mouse; VGA palette and retrace
  ports. The Watcom startup takes the DOS/4G path (`int 21h ax=FF00h`).
- **Deterministic:** the timer interrupt fires every 400,000 instructions while interrupts
  are enabled, and the date is fixed.
- **`Z.CFG`:** the game needs exactly one argument, its config file (`main` checks
  `argc == 2`, the "Please run DAGGER.EXE" message). The overlay holds one with `path`,
  `pathcd`, `maps` and `mapfile maps.bsa` (12 characters at most).
- Two things DOS does that mattered: handles are the lowest free number (Watcom's runtime
  only tracks 20), and a seek with an invalid mode fails without moving.

**A latent bug, found on the way:** XnGine's `func_000C31AE` seeks with `mov ah,42h / int 21h`
and never sets AL, so the mode is the low byte of whatever EAX held (a pointer, 0x1C, in this
run). DOS rejects it and does not move, and the 0x400-byte read that follows only works if the
file position is already right. The emulator logs it as a note each run.

## 2026-10-02: phase 3, input, a scripted new game, and call records

- **Input.** Keys go through the game's own int 9 handler (XnGine's, at `0x142840`). The
  mouse driver behaves as the game uses it: XnGine polls absolute position (function 3) with
  its own range, while some screens move the cursor from motion counters (function `0Bh`).
  XnGine's mouse code (`0x12B196`) pairs two presses within 8 BIOS ticks and 4 pixels into a
  double-click, so scripted clicks are 80+ timer ticks apart, and lists select with a
  double-click.
- **Snapshots** hold the whole machine and what the game wrote to its files, so each step of
  a long scenario resumes in seconds. `tools/scenarios/newgame.txt` boots, creates a Breton
  battlemage and skips the intro; it replays identically from boot.
- **The install's missing files.** Bethesda's free release ships the CD's `ARENA2` without
  `ARCH3D.BSA` and `DAGGER.SND`; the installer unpacks them from `PACKED.DAT` (`DAG_HUGE.LST`
  says so). That file is 256 KB blocks of PKWARE DCL "implode" data, each with a 36-byte
  header, and a directory of names and sizes at the end. `tools/blast.py` (after zlib's
  blast.c) unpacks both, 27,143,532 and 7,661,766 bytes, into the run's overlay.
- **Records** (`tools/xn_record.py`). An entry hook stops the CPU before a function's first
  instruction (by moving EIP to a `hlt`), and the call then runs alone with memory hooks: the
  record holds every byte it read before writing, the XnGine code bytes that differ from the
  file (the self-modified state), registers and descriptors, and its writes, exit registers,
  port I/O, and interrupts with what each service returned. Replay loads `FALL.EXE` fresh,
  puts that input back, answers services from the record, and compares. **All 13 XnGine
  functions that run at the main menu replay exactly.**
- **Tracer** (`tools/xn_trace.py`): at the menu, 13 of 657 functions run, the planted-`ret`
  loops make 4,400 opcode writes per 100 ticks, and nothing writes into code outside the known
  patch fields.

**The divide fault entering the world was by design.** The first 3D frame divides by zero in
`func_0015BC42` (`idiv` computing 2^32 / `[edi+5Ch]`, a texture gradient that is 0 for
axis-aligned walls seen straight on), whether or not the intro is skipped. XnGine expects it:
it installs a DPMI divide-error handler (`0x149FC8`) that reads the faulting `idiv`'s ModRM
byte, steps EIP over the instruction (2, 3 or 6 bytes) and returns with EAX = EDX = 0. A frame
takes about 10 of them. Two emulator changes made it work:

- CPU exceptions go to the game's DPMI 0203h handlers with a DPMI 0.9 frame (return CS:EIP,
  error code, faulting CS:EIP, EFLAGS, SS:ESP), and resume from the frame after its `retf`.
- Unicorn keeps the last exception in `env->old_exception` and clears it only when delivering
  through an IDT, so a second handled #DE came back as a double fault and the CPU stalled. The
  emulator finds that field in Unicorn's context blob at start-up (a scratch #DE) and resets it
  inside the hook after each handled exception.

With that, the headless game reaches the 3D world (a snowy exterior after the fast-start
character), at about 4.5 timer ticks a second. The handler was not in the code map (it is
reached only through DPMI and ends in `retf`); `tools/xn_trace.py` now writes code it sees run
outside the map, and handlers the game registers, to `config/xngine_seeds.csv`, which
`tools/xn_disasm.py` starts from.

Records now also replay port reads, and carry the exception handlers: all 110 records from the
menu, the province map, character creation and the intro video replay exactly.

Run: `.venv/bin/python tools/xn_disasm.py` regenerates `src/xngine/` (4 s);
`.venv/bin/python tools/xn_link.py [module]` checks modules; `tools/build-and-verify.sh` builds.
