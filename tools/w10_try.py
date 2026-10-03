#!/usr/bin/env python3
"""Try C variants of one function with Watcom C32 10.0a, all in one DOSBox-X run.

usage: w10_try.py file.c func_XXXXXXXX "old text" "new text" ["new text" ...]
  each variant is file.c with `old text` replaced by one `new text`; a new text of the form
  @path uses that whole file instead. Prints, per variant: differing bytes, OK, the size
  difference and the first differing instruction (ours | target).
"""
import sys, os, tempfile
sys.path.insert(0, 'tools')
import wcc10, match, lift_all, capstone
from omf import OMF
lift_all.init_worker()
lift_all.W["md"] = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
src = open(sys.argv[1]).read(); func = sys.argv[2]; old = sys.argv[3]
assert old in src, "old not found"
td = tempfile.mkdtemp(prefix="try10_")
paths = []
for k, new in enumerate(sys.argv[4:]):
    p = os.path.join(td, "v%d.c" % k)
    open(p, "w").write(open(new[1:]).read() if new.startswith("@") else src.replace(old, new))
    paths.append((p, new))
objs, wd = wcc10.compile_many([p for p, _ in paths], match.default_flags())
tgt = match.Target(); va, size = match.symbol_map()[func]
for p, new in paths:
    obj = objs[p]
    if obj is None:
        err = open(os.path.join(wd, "N%04d.ERR" % [q for q, _ in paths].index(p)), errors="replace").read().strip().splitlines()
        print("ERR", [l for l in err if "rror" in l][:2], "|", new[:100]); continue
    o = OMF(obj)
    ok, nd = match.compare(tgt, o, func, va, size, quiet=True)
    fns = {x.strip('_'): (si, off, sz) for x, si, off, sz in o.functions()}
    si, off, csz = fns[func]
    ours = bytes(o.data[si][off:off + csz])
    mask = {fx.offset - off + j for fx in o.fixups if fx.seg == si and off <= fx.offset < off + csz for j in range(fx.size)}
    tmask = {k for k in range(size) if va + k in tgt.fix}
    desc = "" if ok else lift_all.first_diff(ours, tgt.bytes_at(va, size), va, mask, tmask)[0]
    print("%3d %s %s %s | %s" % (nd, "OK" if ok else "  ", csz - size, desc[:60], new.replace("\n", " ")[:110]))
