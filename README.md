# Daggerfall decompilation

A matching decompilation of *The Elder Scrolls II: Daggerfall* (DOS, 1996), starting with the game
executable `FALL.EXE` (version 1.07.213), built with Watcom C and run under the CauseWay DOS
extender.

This repo contains no game code or data. You supply the executables; the tools check their SHA-1.

- [docs/head_start.md](docs/head_start.md): the research plan this project started from
- [docs/progress.md](docs/progress.md): a log of what has been found and done, newest last

## Getting the game files

Bethesda gives Daggerfall away free. Its `DFInstall.zip` contains the 1996 CD files
(`DFCD/DAGGER/FALL.EXE`) and the 1.07.213 patch (`DAGGER/DAG213.EXE`). Put the extracted files
under `orig/` (gitignored).
