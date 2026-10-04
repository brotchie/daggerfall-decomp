#!/usr/bin/env python3
"""Daggerfall's data files, read the way the game reads them, to name what a file read got.

  BSA archives (UESP, "BSA file format"): u16 count, u16 type, records back to back from
      offset 4, the directory at the end: type 0x0100 names (14 bytes + u32 size), else
      numbers (u16 or u32 id + u32 size). DAGGER.SND is one, by number.
  TEXT.RSC: u16 index length, then (u16 id, u32 offset) entries; a record ends at 0xFE, 0xFF
      separates the variants the game picks one of at random, 0xFC/0xFD end lines.
  TEXTURE.nnn: u16 count, the archive's name (24 bytes), 20-byte record headers (u32 offset
      at +2).
  Sounds: a DAGGER.SND record id -> its directory index -> Daggerfall Unity's SoundClips name
      (config/sound_clips.csv).

usage: assets.py FILE POS          the record FILE's byte POS falls in, and its label
       assets.py rsc ID [ID ...]   TEXT.RSC records' text
       assets.py snd ID [ID ...]   sound names of DAGGER.SND record ids
"""
import csv
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DIRS = (os.path.join(ROOT, "build", "game", "ARENA2"), os.path.join(ROOT, "build", "emu", "packed"))
MULTI = {"IMG", "CIF", "CEL", "RCI", "QRC", "QBN", "SKY", "CFG"}
_cache = {}


def path(name):
    """The game's copy of ARENA2 file `name`, any case (ARCH3D.BSA and DAGGER.SND come from
    PACKED.DAT, unpacked by fallemu)."""
    for d in DIRS:
        if os.path.isdir(d):
            for n in os.listdir(d):
                if n.upper() == name.upper():
                    return os.path.join(d, n)
    return None


def base(f):
    """A DOS or host path's file name, upper case."""
    return os.path.basename((f or "").replace("\\", "/")).upper()


def bsa_records(name):
    """[(name or id, offset, size)] of a BSA archive, in directory order; [] if unreadable."""
    key = "bsa:" + name.upper()
    if key not in _cache:
        p = path(name)
        out = []
        if p:
            d = open(p, "rb").read()
            count, typ = struct.unpack_from("<HH", d, 0)
            ent = 18 if typ == 0x0100 else 8 if _fits(d, count, 8) else 6
            dirpos, pos = len(d) - count * ent, 4
            for i in range(count):
                e = d[dirpos + i * ent: dirpos + (i + 1) * ent]
                if ent == 18:
                    rid = e[:14].split(b"\0")[0].decode("latin-1")
                else:
                    rid = struct.unpack_from("<I" if ent == 8 else "<H", e)[0]
                sz = struct.unpack_from("<I", e, ent - 4)[0]
                out.append((rid, pos, sz))
                pos += sz
        _cache[key] = out
    return _cache[key]


def _fits(d, count, ent):
    dirpos = len(d) - count * ent
    return dirpos > 4 and sum(struct.unpack_from("<I", d, dirpos + i * ent + ent - 4)[0]
                              for i in range(count)) + 4 == dirpos


def _rsc():
    if "rsc" not in _cache:
        p = path("TEXT.RSC")
        d = open(p, "rb").read() if p else b"\0\0"
        n = struct.unpack_from("<H", d, 0)[0]
        ents = sorted((off, rid) for rid, off in (struct.unpack_from("<HI", d, 2 + 6 * i)
                                                   for i in range((n - 2) // 6)) if rid != 0xFFFF)
        recs = {}
        for i, (off, rid) in enumerate(ents):
            recs[rid] = (off, ents[i + 1][0] if i + 1 < len(ents) else len(d))
        _cache["rsc"] = (d, ents, recs)
    return _cache["rsc"]


def rsc_ids():
    """The record ids TEXT.RSC has."""
    return set(_rsc()[2])


def rsc_text(rid, n=80):
    """TEXT.RSC record rid as one line (variants joined by ' / '), cut to n chars; None if
    there is no such record."""
    d, _ents, recs = _rsc()
    if rid not in recs:
        return None
    off, end = recs[rid]
    s = []
    for c in d[off:end]:
        if c == 0xFE:
            break
        s.append(" / " if c == 0xFF else chr(c) if 32 <= c < 127 else " ")
    s = " ".join("".join(s).split())
    return s if len(s) <= n else s[:n - 3] + "..."


def texture(name):
    """(archive name, [record offsets]) of a TEXTURE.nnn file."""
    key = "tex:" + name.upper()
    if key not in _cache:
        p = path(name)
        out = ("", [])
        if p:
            with open(p, "rb") as f:
                h = f.read(26)
                count = struct.unpack_from("<H", h, 0)[0]
                recs = f.read(20 * count)
            offs = [struct.unpack_from("<I", recs, 20 * i + 2)[0] for i in range(count)
                    if 20 * i + 6 <= len(recs)]
            out = (" ".join(h[2:26].split(b"\0")[0].decode("latin-1").split()), offs)
        _cache[key] = out
    return _cache[key]


def sound_clips():
    """{DAGGER.SND directory index: Daggerfall Unity's name}."""
    if "clips" not in _cache:
        with open(os.path.join(ROOT, "config", "sound_clips.csv"), newline="") as f:
            rows = csv.DictReader(line for line in f if not line.startswith("#"))
            _cache["clips"] = {int(r["index"]): r["name"] for r in rows}
    return _cache["clips"]


def sound_name(rid):
    """The name of DAGGER.SND record rid: SoundClips' name for its index, else 'index N';
    None if DAGGER.SND has no such record."""
    if "sndix" not in _cache:
        _cache["sndix"] = {r[0]: i for i, r in enumerate(bsa_records("DAGGER.SND"))}
    i = _cache["sndix"].get(rid)
    if i is None:
        return None
    return sound_clips().get(i, "index %d" % i)


def record(f, pos):
    """The record at byte pos of file f: a BSA record's name or id, a TEXT.RSC id, a
    TEXTURE record's index; None outside a record (headers, directories) or for other
    files."""
    f = base(f)
    if pos is None:
        return None
    if f == "TEXT.RSC":
        _d, ents, _recs = _rsc()
        best = None
        for off, rid in ents:
            if off > pos:
                break
            best = rid
        return best
    if f.startswith("TEXTURE."):
        ok = [(o, i) for i, o in enumerate(texture(f)[1]) if o <= pos]
        return max(ok)[1] if ok else None
    if f.endswith((".BSA", ".SND")):
        for rid, off, sz in bsa_records(f):
            if off <= pos < off + sz:
                return rid
    return None


def asset_class(f):
    """The kind of file: an archive by name, TEXTURE.nnn, *.IMG for the per-screen files."""
    f = base(f)
    stem, _, ext = f.rpartition(".")
    if f.startswith("TEXTURE.") or ext.isdigit():
        return (stem or f) + ".nnn"
    if ext in ("BSA", "SND", "RSC"):
        return f
    if ext in MULTI or any(c.isdigit() for c in stem):
        return "*." + ext
    return f


def label(f, rec):
    """A read's asset, one string: MONSTER.BSA/ASCR0025.ANC, DAGGER.SND/GoldPieces,
    TEXT.RSC/454, TEXTURE.280 (the whole archive: XnGine caches it), REST02I0.IMG."""
    f = base(f)
    if f.startswith("TEXTURE.") or rec is None:
        return f
    if f == "DAGGER.SND":
        return "DAGGER.SND/" + (sound_name(rec) or str(rec))
    return "%s/%s" % (f, rec)


def token(f, rec):
    """The episode feature for a read: rsc:454, snd:GoldPieces, asset:MONSTER.BSA/ASCR0025."""
    f = base(f)
    if f == "TEXT.RSC" and rec is not None:
        return "rsc:%d" % rec
    if f == "DAGGER.SND" and rec is not None:
        return "snd:" + (sound_name(rec) or str(rec)).replace(" ", "")
    arch, _, r = label(f, rec).partition("/")
    return "asset:" + arch + ("/" + r.rsplit(".", 1)[0] if r else "")


def main():
    a = sys.argv[1:]
    if len(a) >= 2 and a[0] == "rsc":
        for x in a[1:]:
            print(x, rsc_text(int(x), 400))
    elif len(a) >= 2 and a[0] == "snd":
        for x in a[1:]:
            print(x, sound_name(int(x)))
    elif len(a) == 2:
        rec = record(a[0], int(a[1], 0))
        print(rec, label(a[0], rec), asset_class(a[0]))
        if base(a[0]).startswith("TEXTURE."):
            print(texture(base(a[0]))[0])
    else:
        print(__doc__)


if __name__ == "__main__":
    main()
