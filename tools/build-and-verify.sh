#!/bin/sh
# Build FALL.EXE from src/ and verify it byte for byte against the original.
#
# usage: tools/build-and-verify.sh [-v]
#   -v  print instruction diffs for any C function that does not match
#
# Success = "build/FALL.EXE: OK" and exit code 0. Anything else (a non-matching function, a
# relocation that resolves elsewhere, a SHA-1 mismatch) is a failure: never commit on red.
set -eu
cd "$(dirname "$0")/.."
PYTHON=python3
[ -x .venv/bin/python ] && PYTHON=.venv/bin/python
if "$PYTHON" tools/build_fall.py "$@"; then
    "$PYTHON" tools/update_readme_progress.py
    echo "BUILD OK: every C function matches and FALL.EXE is byte-identical."
else
    echo "BUILD FAILED: a non-matching function or checksum is never acceptable."
    exit 1
fi
