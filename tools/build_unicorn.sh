#!/bin/sh
# Build the patched Unicorn 2.1.4 that tools/fallemu.py uses (third_party/unicorn/build).
#
# Stock Unicorn runs the game's 3D world ~70x slower than this build, for two reasons:
#   - uc_emu_start(count=N) counts instructions with a hook on every instruction, which makes
#     every translated block several times bigger and slower; this build counts a whole
#     translation block at a time in the check every block already makes (slices end on a
#     block boundary: still deterministic);
#   - XnGine patches its own code all the time (operands, planted `ret`s), and every patch
#     retranslates the whole block around it; this build translates code on pages the guest
#     has patched in short blocks (8 instructions, unoptimised), keeps each page's code
#     bitmap instead of discarding it whenever a block is added, and lets a store that does
#     not change memory leave translated code alone.
# It also forgets a CPU exception once an interrupt hook has handled it, so the next divide
# error is not taken for a double fault (stock Unicorn needs a context save/restore per fault),
# and records code coverage for free (uc_dagger_coverage, used by tools/fallcov.py).
# Knobs (environment): UC_EXACT_COUNT=1 (per-instruction counting), UC_SMC_TB_INSNS (block
# length on patched pages, 0 = off), UC_SMC_NOOPT=0 (optimise them), UC_MAX_TB_INSNS (cap all).
#
# usage: tools/build_unicorn.sh    (needs git, cmake, ninja; about a minute)
set -eu
cd "$(dirname "$0")/.."
SRC=third_party/unicorn
if [ ! -d "$SRC" ]; then
    git clone -q --depth 1 -b 2.1.4 https://github.com/unicorn-engine/unicorn.git "$SRC"
fi
if ! git -C "$SRC" diff --quiet; then
    echo "$SRC has local changes; reset it (git -C $SRC checkout .) to re-apply the patch"
else
    git -C "$SRC" apply "$(pwd)/tools/unicorn/dagger-unicorn.patch"
fi
cmake -S "$SRC" -B "$SRC/build" -G Ninja -DUNICORN_ARCH=x86 -DCMAKE_BUILD_TYPE=Release \
    -DUNICORN_BUILD_TESTS=OFF > /dev/null
ninja -C "$SRC/build" > /dev/null
ls "$SRC"/build/libunicorn.*
