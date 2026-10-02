#!/usr/bin/env python3
"""Rewrite the progress badges in README.md from build/progress.json (written by the build).

The badges are shields.io images between `<!-- progress:start -->` and `<!-- progress:end -->`.
Run by tools/build-and-verify.sh after a successful build.
"""
import json
import os
import re
from urllib.parse import quote

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def colour(pct):
    for limit, name in ((20, "red"), (40, "orange"), (60, "yellow"), (80, "yellowgreen"),
                        (100, "green")):
        if pct < limit:
            return name
    return "brightgreen"


def badge(label, message, col):
    """A shields.io static badge: `-` and `_` are doubled, the rest URL-quoted."""
    q = lambda t: quote(t.replace("-", "--").replace("_", "__"), safe="")
    return "![%s](https://img.shields.io/badge/%s-%s-%s)" % (label, q(label), q(message), col)


def main():
    p = json.load(open(os.path.join(ROOT, "build", "progress.json")))
    pct = 100.0 * p["bytes_done"] / p["bytes_total"]
    fpct = 100.0 * p["functions_done"] / p["functions_total"]
    lines = [
        badge("decompiled", "%.2f%%" % pct, colour(pct)),
        badge("functions", "%d of %d" % (p["functions_done"], p["functions_total"]), colour(fpct)),
        badge("FALL.EXE", "matching" if p["matching"] else "not matching",
              "brightgreen" if p["matching"] else "red"),
    ]
    block = "<!-- progress:start -->\n%s\n\n%.2f%% of the game's own code (%d of %d bytes, %d of %d " \
            "functions) is matched C; the rebuilt `FALL.EXE` is byte-identical to 1.07.213.\n" \
            "<!-- progress:end -->" % (" ".join(lines), pct, p["bytes_done"], p["bytes_total"],
                                      p["functions_done"], p["functions_total"])
    path = os.path.join(ROOT, "README.md")
    text = open(path).read()
    if "<!-- progress:start -->" in text:
        text = re.sub(r"<!-- progress:start -->.*?<!-- progress:end -->", block, text, flags=re.S)
    else:
        title, rest = text.split("\n", 1)
        text = "%s\n\n%s\n%s" % (title, block, rest)
    open(path, "w").write(text)
    print("README: %.2f%% decompiled, %d / %d functions" % (pct, p["functions_done"], p["functions_total"]))


if __name__ == "__main__":
    main()
