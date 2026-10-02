#!/bin/sh
# Produce the 1.07.213 FALL.EXE from Bethesda's DFInstall.zip by running the
# official DAG213.EXE patcher headless in DOSBox-X (brew install dosbox-x).
#
#   tools/patch_213.sh [path/to/DFInstall.zip]   (default: orig/DFInstall.zip)
#
# Output: orig/1.07.213/FALL.EXE, checked against the expected SHA-1.
set -eu
cd "$(dirname "$0")/.."
ZIP=${1:-orig/DFInstall.zip}
EXPECT=c49a2ceb677239af733d0e0127ac810ec859c0ac
WORK=build/patch

rm -rf "$WORK" && mkdir -p "$WORK"
unzip -q "$ZIP" 'DFCD/DAGGER/*' 'DAGGER/DAG213.EXE' -d "$WORK"
mv "$WORK/DFCD/DAGGER" "$WORK/game"
mv "$WORK/DAGGER/DAG213.EXE" "$WORK/game/"

# The patcher asks "Are you sure you want to update now (y/n)?", then asks
# again before running FIXMAPS; AUTOTYPE answers. It never exits on its own,
# so stop DOSBox once FALL.EXE has changed and gone quiet.
cat > "$WORK/run.conf" <<'CONF'
[dosbox]
memsize=32
quit warning=false
[cpu]
cycles=max
[autoexec]
mount c "game"
c:
AUTOTYPE -w 3 -p 1 y enter enter enter
DAG213.EXE
exit
CONF
(cd "$WORK" && SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
    dosbox-x -nopromptfolder -fastlaunch -conf run.conf >dosbox.log 2>&1) &
PID=$!
for _ in $(seq 1 120); do
    sleep 2
    sum=$(shasum "$WORK/game/FALL.EXE" | cut -d' ' -f1)
    [ "$sum" = "$EXPECT" ] && break
done
sleep 5
pkill -9 -f 'dosbox-x .*-conf run.conf' 2>/dev/null || true
wait "$PID" 2>/dev/null || true

mkdir -p orig/1.07.213
cp "$WORK/game/FALL.EXE" "$WORK/game/DAGGER.EXE" orig/1.07.213/
echo "$EXPECT  orig/1.07.213/FALL.EXE" | shasum -c -
