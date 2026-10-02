# Daggerfall decompilation: head start

Research notes (2026-10-01) on what a matching decompilation of *The Elder Scrolls II: Daggerfall*
(DOS, 1996) needs before work starts. None of this has been checked against the executable yet:
there's no copy of the game on this machine.

## Summary

- **No decomp of the game's executable exists.** DecompFall stopped early on the 16-bit launcher.
  Daggerfall Unity is a full recreation, not a decompilation. DaggerXL's author reconstructed the
  executable in C, but never published it.
- **The target is FALL.EXE**, built with Watcom C and run under the **CauseWay** DOS extender (not
  DOS/4GW).
- **There is a precedent with the same toolchain:** KKND-Decomp (Watcom 10.x, DOS, 1997) gets
  byte-identical rebuilds with a patched Open Watcom `wcc386`. Its compiler patch and tools are CC0.
- **Three unknowns decide the effort**, and each takes about a day to settle once the executable is
  here:
  1. FALL.EXE's format: CauseWay's LE or 3P format, compressed or not.
  2. The exact Watcom version, and whether KKND's patch covers it.
  3. How much handwritten asm there is.

## 1. The game's executables

| File | What it is | Priority |
|---|---|---|
| `FALL.EXE` | The game, 32-bit protected mode, Watcom C, with CauseWay embedded | The target |
| `DAGGER.EXE` | Launcher: checks the system, sets up the mouse and a static swap file, starts FALL.EXE. 16-bit (DecompFall's Ghidra output shows `__cdecl16near`) | Later |
| `ARENA2\REPORT.EXE`, setup tools | Helpers | Ignore |

- **Version:** use **1.07.213**, the final patch (often called "2.13"). UESP's offsets refer to it.
- **Where to get it:** Bethesda has given it away free since 2009, and it's also on Steam.
- **Distribution:** the decomp repo must not contain the executable or any game data. Use KKND's
  model: you supply the executable, and the build checks its SHA-1.

## 2. Step one: unwrap FALL.EXE

CauseWay can run Watcom's LE format, or its own **3P** format (converted by `LE23P.EXE`),
optionally compressed with **CWC**.

- **If it's LE inside a CauseWay stub:** load it with
  [ghidra-lx-loader](https://github.com/yetmorecode/ghidra-lx-loader). It handles LE and LX with full
  fixups, and was tested on Redguard, which uses the same engine (XnGine).
- **If it's 3P or compressed:** write a converter. CauseWay's source is public
  ([amindlost/cw](https://github.com/amindlost/cw), and `contrib/extender/causeway` in Open Watcom
  v2), so its loader and decompressor are documented by their code.
- **The likely case:** probably uncompressed. People hex-edit tables in FALL.EXE at fixed file
  offsets (the item list at 0x1B682A in 1.07.213), which wouldn't work on a compressed file.
- **Output:** a small loader script (like KKND's `le.py`) that yields the code and data objects,
  their base addresses and the relocations.

## 3. Step two: identify the compiler version and flags

The game shipped in 1996, which makes Watcom **10.0** (1994), **10.5** (1995) or **10.6** (1996)
likely; 11.0 came in 1997.

1. **Runtime strings.** Watcom's runtime library embeds "WATCOM C/C++32 Run-Time system …
   Copyright … 1988-19xx", and the year range moves with each release:
   `strings FALL.EXE | grep -i -e watcom -e causeway`.
2. **Library fingerprints.** Compile runtime functions (`memcpy`, `printf`, the startup code) with
   each candidate version and byte-compare them. IDA's FLIRT signatures for Watcom do the same per
   version.
3. **Code generation**, which pins down the flags:
   - register (`eax/edx/ebx/ecx`) vs stack calling convention (`-5r` / `-5s`)
   - the floating-point option (`-fp`)
   - optimisation idioms
   - function alignment, and padding at loop heads

   Start from KKND's flags: `wcc386 -s -of+ -5r -omilert -zm -zp1`.
4. **Handwritten asm**, probably in the renderer. Mark it early and keep it as asm, as in Blast
   Corps.

## 4. The compiler

- **Open Watcom** (1.9, and the active 2.0 fork) is free and open source. It descends from Watcom
  11.0c, so its output is close to 11.0's, but not to 10.x's.
- **KKND's patched `wcc386`** puts the Watcom 10.x code generator's choices back into Open Watcom
  v2 ([doc](https://github.com/Wyrelade/KKND-Decomp/blob/main/doc/compiler_patch.md),
  `tools/owpatch/kknd-wcc386.patch` against open-watcom-v2 `91922ea`). Each change can be switched
  off with an environment variable. The changes:
  - EBX before ECX in register preference
  - the 386/486 multiply cost, so struct indexing becomes shifts and adds
  - the factoring order for multiplies by constants
  - switch tables read through `cs:`, with a lower table threshold and a pure binary search
  - no alignment padding
  - no jump threading on loop entry
  - loops entered at the bottom test
- **The original 10.x compilers** are commercial and not legally available. You'd only need one if
  the patched Open Watcom can't reach a match. The DOS-hosted compiler runs in DOSBox; the
  Windows-hosted binaries (`binnt/`) run under Wine and are much faster to script.
- **Hosting:** KKND runs on Windows (`binnt`). Here, build Open Watcom v2 with the patch natively
  on macOS or Linux. No emulator is needed to compile.

## 5. Build and verify: fork KKND's tools

[KKND-Decomp](https://github.com/Wyrelade/KKND-Decomp) (CC0, started 2026-09-24, 492 of 3,713
functions matched) has:

| Tool | Purpose |
|---|---|
| `le.py`, `omf.py` | readers for the LE executable and OMF object formats |
| `split.py`, `find_functions.py` | split the executable into per-function asm |
| `match.py` | compile one function and compare it |
| `build_kknd.py`, `build-and-verify.sh` | splice matched C into the retail executable and check the SHA-1 |
| `lib_match.py` | identify library functions |
| `score_functions.py`, `difficult_functions` | rank functions by difficulty |
| `name_evidence.py`, `learn.py` | naming evidence; search the learnings |
| `make_report.py`, `update_readme_progress.py` | progress reporting |

Rule from KKND: **a function counts as matched only when the full rebuild reproduces the SHA-1**
and every relocation resolves. A match in isolation isn't enough: `match.py` can't see argument
push order under relocation masks, or code placed after jump tables.

Read
[`DECOMPILATION_LEARNINGS.md`](https://github.com/Wyrelade/KKND-Decomp/blob/main/DECOMPILATION_LEARNINGS.md)
first. It lists the C source tricks that make Watcom output match, for example:
- `#pragma aux … modify exact`
- declaration order to choose stack slots
- comma expressions for store order
- a single shared exit instead of early returns

## 6. Separate out the library code

Mark these as libraries early, so progress measures the game's own code:
- Watcom's C runtime
- CauseWay
- licensed middleware, probably a sound library (KKND found StratosWare MemCheck in its
  executable)

## 7. Sources for names and meaning

These play the role the names ledger plays for Blast Corps: evidence for naming functions and data.

- **[Daggerfall Unity](https://github.com/Interkarma/daggerfall-unity)** (MIT): its file-format
  readers (the DaggerfallConnect library), and the classic formulas researched from the original
  game by Allofich (skills, levelling, the save format;
  [DFWorkshop](https://www.dfworkshop.net/the-fighters-update/)).
- **UESP:**
  - [file formats](https://en.uesp.net/wiki/Daggerfall:Files)
  - [save-game offsets](https://en.uesp.net/wiki/Daggerfall_Mod:Save_Game_Offsets)
  - [SAVEGAME.DAT](https://en.uesp.net/wiki/Daggerfall_Mod:SAVEGAME.DAT_description)
  - [item tables in FALL.EXE](https://en.uesp.net/wiki/Daggerfall_Mod:Hacking_Items), with 288
    records of 48 bytes at 0x1B682A
  - [the hacking guide](https://en.uesp.net/dagger/daghack.shtml)
- **Strings in FALL.EXE**, including debug-menu text (around 0x1AA57C in 1.07.213).
- ***Daggerfall Chronicles***, the official guide, for formulas.
- **Sibling games on the same engine (XnGine):** Battlespire, Redguard and The Terminator: Future
  Shock may share code, which helps with naming.

## 8. Prior attempts

| Project | Status | Useful? |
|---|---|---|
| [DecompFall](https://github.com/DecompFall/) | "10%" of DAGGER.EXE: Ghidra pseudocode and strings. FALL.EXE untouched. Last push 2024-03-14 | No |
| DaggerXL / XL Engine (luciusDXL) | Reconstructed **all of FALL.EXE in C** as a test build called "DaggerfallDOS" ([UESP](https://en.uesp.net/wiki/Daggerfall_Mod:DaggerXL), [DFWorkshop](https://www.dfworkshop.net/daggerxl/)). Project ended in 2020; the [XL-Engine repo](https://github.com/luciusDXL/XL-Engine) has only the engine shell | **Biggest possible shortcut, if Lucius shares notes or a symbol map** (he now leads The Force Engine). Not matching code, but function boundaries and names would be gold |
| [Daggerfall Unity](https://github.com/Interkarma/daggerfall-unity) | Complete recreation | For names, formats and checking behaviour |
| [OpenDF](https://github.com/kcat/opendf) | Older engine recreation | Minor |

## 9. Other precedents

| Project | Toolchain | Takeaway |
|---|---|---|
| [KKND-Decomp](https://github.com/Wyrelade/KKND-Decomp) | Watcom 10.x, DOS/4GW, LE | The template (above) |
| [rebrew](https://github.com/maci0/rebrew) | General | A workbench that rewrites C (keeping what it does) and recompiles until the output matches |
| [homm1-decomp](https://github.com/sushi-shi/homm1-decomp) | Windows | Matching decomp of a game from the same era |
| [Duke2Reconstructed](https://github.com/lethal-guitar/Duke2Reconstructed), [reconstruction-of-zzt](https://github.com/asiekierka/reconstruction-of-zzt) | 16-bit Borland/Turbo | Mature byte-identical DOS reconstructions; the method, not the toolchain |
| [LineWars II](https://github.com/fredangstadt-lang/LineWarsII-DOS) | Open Watcom, DOSBox-X | Behavioural reconstruction, checked against the original machine code running in an emulator |
| [Duke4.net EXE restoration](https://forums.duke4.net/topic/10026-restoration-of-a-few-games-exes-versions/) | Various | Restoring exact versions of other games' executables |

decomp.me reportedly supports Watcom (unconfirmed). It would allow collaborative matching of
individual functions.

## 10. Later: testing behaviour

A matching decomp only needs the SHA-1. For a port later on:
- the DOSBox-X debugger and deterministic input replay would play the role the port's diff mode
  plays in Blast Corps
- Daggerfall Unity would be the reference for behaviour

## First steps

1. Get FALL.EXE 1.07.213 here and record its SHA-1.
2. Work out its format (section 2) and get the code and relocations into Ghidra.
3. Fingerprint the compiler (section 3).
4. Build Open Watcom v2 with KKND's patch, and try matching 5–10 small leaf functions.
5. If those match: fork KKND's splitter and build-and-verify for FALL.EXE's layout, then set up the
   repo (you supply the executable; the build checks the SHA-1).
6. In parallel: contact Lucius about the DaggerfallDOS notes.
