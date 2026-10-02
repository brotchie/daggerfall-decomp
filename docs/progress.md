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
