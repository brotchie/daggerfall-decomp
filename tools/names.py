#!/usr/bin/env python3
"""Names for FALL.EXE's functions, globals and record fields, with the evidence for each.

config/names.csv is the record: kind (func, global, field), address (0xXXXXXXXX, or
RECORD+0xOFF for a field), name, confidence (confirmed: changing it does what the name says,
or it holds in every case checked; strong: it fits every experiment; candidate: it fits),
evidence (what shows it). The sources still say func_XXXXXXXX / D_XXXXXXXX; names are applied
to them once they settle (config/symbols.txt lets the build check named functions).

usage:
  names.py check                   names are identifiers, unique, at known addresses
  names.py merge FILE.csv ...      add proposals (same columns); a clash keeps the more
                                   certain name and is reported
  names.py audit FILE.csv ...      apply audit decisions (the columns plus `action`):
                                   upgrade/rename/apply-field replace the row at that
                                   address, drop deletes it, keep updates its evidence
  names.py show NAME|ADDRESS       look one up
  names.py annotate FILE.c         print a source file with names after func_/D_ tokens
"""
import csv
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
NAMES = os.path.join(ROOT, "config", "names.csv")
FIELDS = ["kind", "address", "name", "confidence", "evidence"]
RANK = {"candidate": 0, "strong": 1, "confirmed": 2}


def load(path=NAMES):
    if not os.path.exists(path):
        return []
    with open(path, newline="") as f:
        return [dict((k, (r.get(k) or "").strip()) for k in FIELDS) for r in csv.DictReader(f)]


def save(rows, path=NAMES):
    rows = sorted(rows, key=lambda r: ({"global": 0, "field": 1, "func": 2}.get(r["kind"], 3), r["address"]))
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=FIELDS, lineterminator="\n")
        w.writeheader()
        w.writerows(rows)


def by_address(rows=None):
    """{"func_XXXXXXXX" / "D_XXXXXXXX": name} for the functions and globals."""
    out = {}
    for r in rows if rows is not None else load():
        if r["kind"] in ("func", "global"):
            a = int(r["address"], 16)
            out[("func_%08X" if r["kind"] == "func" else "D_%08X") % a] = r["name"]
    return out


_BACK = None


def canonical(text):
    """Source text with the names tools/apply_names.py applied (config/symbols.txt) put back
    in their address form (func_XXXXXXXX, D_XXXXXXXX), for tools that read sources by address."""
    global _BACK
    if _BACK is None:
        _BACK = {}
        p = os.path.join(ROOT, "config", "symbols.txt")
        if os.path.exists(p):
            for line in open(p):
                m = re.match(r"\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+)\s*;\s*(func|global)?", line)
                if m:
                    _BACK[m.group(1)] = ("D_%08X" if m.group(3) == "global" else "func_%08X") % int(m.group(2), 16)
    if not _BACK:
        return text
    return re.sub(r"\b[A-Za-z_][A-Za-z0-9_]*\b", lambda m: _BACK.get(m.group(0), m.group(0)), text)


def check(rows):
    funcs = set()
    for name in ("functions.csv", "xngine_functions.csv"):    # the game's, and object 2's
        with open(os.path.join(ROOT, "config", name), newline="") as f:
            funcs |= {int(r["va"], 16) for r in csv.DictReader(f)}
    errs, seen = [], {}
    for r in rows:
        if not re.fullmatch(r"[a-z_][a-z0-9_]*", r["name"]) and not (
                "Watcom" in r["evidence"] and re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", r["name"])):
            errs.append("%s: not a lower_case identifier (library names keep their case)" % r["name"])
        if r["confidence"] not in RANK:
            errs.append("%s: confidence %r" % (r["name"], r["confidence"]))
        if r["kind"] == "func" and int(r["address"], 16) not in funcs:
            errs.append("%s: no function at %s" % (r["name"], r["address"]))
        if r["kind"] == "field" and not re.fullmatch(r"[a-z_][a-z0-9_]*\+0x[0-9A-Fa-f]+", r["address"]):
            errs.append("%s: field address %r is not record+0xOFF" % (r["name"], r["address"]))
        key = (r["kind"] == "field" and r["address"].split("+")[0], r["name"])
        if key in seen:
            errs.append("%s: also used for %s" % (r["name"], seen[key]))
        seen[key] = r["address"]
    addr = {}
    for r in rows:
        k = (r["kind"], r["address"].upper())
        if k in addr:
            errs.append("%s %s named twice: %s, %s" % (r["kind"], r["address"], addr[k], r["name"]))
        addr[k] = r["name"]
    return errs


def merge(rows, new):
    idx = {(r["kind"], r["address"].upper()): r for r in rows}
    added = clashes = 0
    for n in new:
        k = (n["kind"], n["address"].upper())
        if k not in idx:
            rows.append(n)
            idx[k] = n
            added += 1
        elif idx[k]["name"] == n["name"]:
            if RANK.get(n["confidence"], 0) > RANK.get(idx[k]["confidence"], 0):
                idx[k].update(n)                    # the same name, now more certain
        elif idx[k]["name"] != n["name"]:
            old = idx[k]
            keep = n if RANK.get(n["confidence"], 0) > RANK.get(old["confidence"], 0) else old
            print("clash at %s %s: %s (%s) vs %s (%s): keeping %s" % (
                n["kind"], n["address"], old["name"], old["confidence"], n["name"],
                n["confidence"], keep["name"]))
            idx[k].update(keep)
            clashes += 1
    return added, clashes


def audit(rows, decisions):
    """Apply audit rows to rows in place: {action: count}, and problems."""
    idx = {(r["kind"], r["address"].upper()): r for r in rows}
    done, problems = {}, []
    for d in decisions:
        act = (d.get("action") or "").strip()
        k = (d["kind"], d["address"].upper())
        row = {f: (d.get(f) or "").strip() for f in FIELDS}
        if act in ("upgrade", "rename", "apply-field", "apply"):
            if k in idx:
                idx[k].update(row)
            else:
                rows.append(row)
                idx[k] = row
        elif act == "drop":
            if k in idx:
                rows.remove(idx.pop(k))
            else:
                problems.append("drop: no row %s %s" % k)
        elif act == "keep":
            if k in idx and row["evidence"]:
                idx[k]["evidence"] = row["evidence"]
        else:
            problems.append("unknown action %r at %s %s" % (act, d["kind"], d["address"]))
            continue
        done[act] = done.get(act, 0) + 1
    return done, problems


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    cmd, args = sys.argv[1], sys.argv[2:]
    rows = load()
    if cmd == "check":
        errs = check(rows)
        print("\n".join(errs) or "%d names OK" % len(rows))
        sys.exit(1 if errs else 0)
    elif cmd == "merge":
        for p in args:
            added, clashes = merge(rows, load(p))
            print("%s: %d added, %d clashes" % (p, added, clashes))
        errs = check(rows)
        if errs:
            raise SystemExit("not saved:\n" + "\n".join(errs))
        save(rows)
    elif cmd == "audit":
        for p in args:
            with open(p, newline="") as f:
                done, problems = audit(rows, list(csv.DictReader(f)))
            print("%s: %s" % (p, ", ".join("%d %s" % (n, a) for a, n in sorted(done.items()))))
            for x in problems[:20]:
                print("  " + x)
        errs = check(rows)
        if errs:
            raise SystemExit("not saved:\n" + "\n".join(errs[:40]))
        save(rows)
    elif cmd == "show":
        q = args[0].lower()
        for r in rows:
            if q in (r["name"], r["address"].lower()) or q.replace("func_", "0x").replace("d_", "0x") == r["address"].lower():
                print("%(kind)s %(address)s %(name)s [%(confidence)s]: %(evidence)s" % r)
    elif cmd == "annotate":
        names = by_address(rows)
        for line in open(args[0], errors="replace"):
            sys.stdout.write(re.sub(r"\b(func|D)_([0-9A-F]{8})\b",
                                    lambda m: m.group(0) + ("/*%s*/" % names[m.group(0)] if m.group(0) in names else ""),
                                    line))
    else:
        raise SystemExit(__doc__)


if __name__ == "__main__":
    main()
