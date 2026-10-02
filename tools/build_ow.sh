#!/bin/sh
# Build the patched Open Watcom v2 toolchain natively (macOS/Linux, clang or gcc).
#
# tools/owpatch/daggerfall-wcc386.patch is KKND-Decomp's kknd-wcc386.patch (CC0; it puts
# Watcom 10.x code-generator choices back into Open Watcom v2, see
# third_party/KKND-Decomp/doc/compiler_patch.md) plus a fix so it compiles off Windows:
# its KKND_CONSTREG experiment in regalloc.c is guarded by _TARGET_INTEL and uses HW_Ovlap.
#
# Daggerfall changes (each has an environment variable that restores stock OW):
#   cg generate.c FlushBlocks(): no peephole flush after every block at -od (DAGGER_FLUSH=1)
#   cc cgen.c: no return-value slot for void functions (DAGGER_VOIDRET=1)
#
# Output: third_party/open-watcom-v2/build/binbuild/{bwcc386,bwlink,bwasm,bwlib}
set -eu
cd "$(dirname "$0")/.."
OW=third_party/open-watcom-v2
BASE=91922eabf98fdfbc38de8ddd4ff3e7f5e769c64c
[ -d "$OW" ] || git clone https://github.com/open-watcom/open-watcom-v2.git "$OW"
(
    cd "$OW"
    set +u  # OW's own scripts read unset variables
    # A clean tree gets the saved patch. A modified tree is assumed to be work in progress on
    # the patch: build it as is (save it with `git diff > tools/owpatch/daggerfall-wcc386.patch`).
    if git diff --quiet; then
        git checkout -q "$BASE"
        git apply ../../tools/owpatch/daggerfall-wcc386.patch
    else
        git diff | cmp -s - ../../tools/owpatch/daggerfall-wcc386.patch ||
            echo "note: building an OW tree that differs from tools/owpatch/daggerfall-wcc386.patch"
    fi
    export OWROOT=$PWD OWDOCBUILD=0 OWDISTRBUILD=0 OWGUINOBUILD=1
    case $(uname) in Darwin) export OWTOOLS=CLANG ;; *) export OWTOOLS=GCC ;; esac
    . ./cmnvars.sh
    cd "$OWROOT"
    ./build.sh boot > build/boot.out 2>&1 || { tail -30 build/boot.out; exit 1; }
)
ls -l "$OW/build/binbuild/bwcc386"
