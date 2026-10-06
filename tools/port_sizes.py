#!/usr/bin/env python3
"""The struct sizes and offsets the game once wrote as numbers (docs/port.md, phase 3): check
that each expression that replaced a number is still that number in the 32-bit layout, and
show what it is natively; check the layouts the game shares with the engine; list numbers
that may still stand for a size or an offset.

  port_sizes.py check     the expressions: 32-bit value (must be the old number) and native;
                          the game's views of engine structs against the engine's headers
  port_sizes.py scan [F]  numbers in the game's C that look like a record header, a grown
                          struct's size or a pointer-table stride (a list to review, not proof)

Nothing is built: clang compiles small probes with -fsyntax-only (the 32-bit layout with an
i386 target, as Watcom lays out the packed structs) or into a scratch binary (native values).
"""
import argparse
import glob
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# (expression, the number it replaced). The expression is valid C with records.h included.
EXPRESSIONS = [
    ("RECORD_HEADER_SIZE", 71),
    ("RECORD_LINKS_OFFSET", 55),
    ("RECORD_LINKS_SIZE", 16),
    ("MEM_BLOCK_HEADER_SIZE", 18),
    ("MODEL_INSTANCE_DATA_SIZE", 62),
    ("REC_SIZEOF(struct character)", 634),
    ("REC_OFFSETOF(struct character, career)", 560),
    ("REC_OFFSETOF(struct character, equipped)", 367),
    ("REC_OFFSETOF(struct character, ship_owned)", 120),
    ("REC_SIZEOF(struct monster)", 659),
    ("REC_SIZEOF(struct location)", 48),
    ("REC_SIZEOF(struct faction)", 92),
    ("REC_SIZEOF(struct loaded_location)", 20),
    ("REC_OFFSETOF(struct loaded_location, data)", 16),
    ("REC_OFFSETOF(struct record, data.item.item_flags)", 113),
    ("REC_OFFSETOF(struct record, children)", 63),
    ("REC_OFFSETOF(struct record, next)", 55),
    ("REC_OFFSETOF(struct model_instance, angles)", 12),
    ("REC_OFFSETOF(struct tex_cache_entry, image)", 12),
    ("REC_SIZEOF(struct tex_block)", 22),
    ("REC_OFFSETOF(struct tex_block, size)", 8),
    ("REC_SIZEOF(struct flc_player)", 44),
    ("REC_SIZEOF(struct talk_place_topic)", 19),
    ("REC_SIZEOF(struct pick_result)", 18),
    ("REC_SIZEOF(struct sos_sample)", 240),
    ("REC_SIZEOF(struct sos_sample) + REC_SIZEOF(struct wav_header)", 284),
    ("REC_SIZEOF(struct sos_song)", 32),
    ("4 * REC_SIZEOF(struct sound_channel) + 4096", 5168),
    ("REC_OFFSETOF(struct qbn_op, args[0].record)", 7),
    ("REC_SIZEOF(struct qbn_arg)", 15),
    ("REC_SIZEOF(struct qbn_item)", 19),
    ("REC_SIZEOF(struct qbn_person)", 20),
    ("REC_SIZEOF(struct qbn_place)", 24),
    ("REC_SIZEOF(struct qbn_timer)", 33),
    ("REC_SIZEOF(struct qbn_foe)", 14),
    ("REC_SIZEOF(struct qbn_op)", 87),
    ("REC_SIZEOF(struct qbn_state)", 8),
    ("27 * REC_SIZEOF(iptr)", 108),
    ("128 * REC_SIZEOF(struct record *)", 512),
    ("REC_SIZEOF(struct mem_pool)", 16),
    ("NATIVE_HEAP_BYTES(1280000)", 1280000),
]

# the game's view of an engine struct: (game expr, engine expr) pairs that must be equal natively
ENGINE_PAIRS = [
    ("sizeof(struct model_instance)", "sizeof(struct xn_model_handle)"),
    ("offsetof(struct model_instance, lights)", "offsetof(struct xn_model_handle, lights)"),
    ("offsetof(struct model_instance, matrix)", "offsetof(struct xn_model_handle, matrix)"),
    ("offsetof(struct model_instance, angles)", "offsetof(struct xn_model_handle, pad_0c)"),
    ("offsetof(struct model_instance, x)", "offsetof(struct xn_model_handle, x)"),
    ("offsetof(struct model_instance, missile_angles)", "offsetof(struct xn_model_handle, angle_x)"),
    ("offsetof(struct model_instance, frame)", "offsetof(struct xn_model_handle, frame)"),
    ("offsetof(struct block_model, x) - offsetof(struct block_model, model)",
     "offsetof(struct xn_model_handle, x)"),
    ("offsetof(struct block_model, yaw) - offsetof(struct block_model, model)",
     "offsetof(struct xn_model_handle, yaw)"),
    ("offsetof(struct tex_cache_entry, header)", "offsetof(struct xn_tex_entry, image)"),
    ("offsetof(struct tex_cache_entry, image)", "offsetof(struct xn_tex_entry, current)"),
    ("sizeof(struct tex_block)", "sizeof(struct xn_tex_block)"),
    ("offsetof(struct tex_block, size)", "offsetof(struct xn_tex_block, size)"),
    ("offsetof(struct xn_pick_hit, model)", "offsetof(struct xn_poly, handle)"),
    ("offsetof(struct monster_anim, anim_script)", "offsetof(struct xn_anim, script)"),
    ("offsetof(struct monster_anim, anim_script_pos)", "offsetof(struct xn_anim, pos)"),
    ("offsetof(struct monster_anim, frame_count)", "offsetof(struct xn_anim, tick_divisor)"),
    ("offsetof(struct monster_anim, anim_request)", "offsetof(struct xn_anim, request)"),
]

NATIVE_FLAGS = ["-std=gnu89", "-funsigned-char", "-DDAGGER_PORT", "-include",
                "port/include/port.h", "-Iport/include", "-Iinclude", "-w"]
I386_FLAGS = ["-target", "i386-unknown-none", "-fsyntax-only", "-std=gnu89",
              "-funsigned-char", "-Iinclude", "-Wno-gnu-folding-constant"]


def native_values(exprs, prelude):
    """Compile and run a probe that prints each expression natively."""
    with tempfile.TemporaryDirectory() as tmp:
        src = os.path.join(tmp, "probe.c")
        exe = os.path.join(tmp, "probe")
        with open(src, "w") as f:
            f.write("#include <stdio.h>\n#include <stddef.h>\n" + prelude)
            f.write("int main(void) {\n")
            for e in exprs:
                f.write('    printf("%%ld\\n", (long)(%s));\n' % e)
            f.write("    return 0;\n}\n")
        r = subprocess.run(["clang"] + NATIVE_FLAGS + ["-o", exe, src], cwd=ROOT,
                           capture_output=True, text=True)
        if r.returncode != 0:
            sys.exit("native probe failed:\n" + r.stderr)
        out = subprocess.run([exe], capture_output=True, text=True).stdout.split()
    return [int(v) for v in out]


def i386_mismatches(pairs):
    """The expressions whose 32-bit value is not the old number (static asserts, i386)."""
    with tempfile.TemporaryDirectory() as tmp:
        src = os.path.join(tmp, "probe32.c")
        with open(src, "w") as f:
            f.write('#include "records.h"\n')
            for i, (e, n) in enumerate(pairs):
                f.write("typedef char check_%d[((%s) == %d) ? 1 : -1];\n" % (i, e, n))
        r = subprocess.run(["clang"] + I386_FLAGS + [src], cwd=ROOT, capture_output=True,
                           text=True)
    bad = set()
    for m in re.finditer(r"check_(\d+)", r.stderr):
        bad.add(int(m.group(1)))
    if r.returncode != 0 and not bad:
        sys.exit("32-bit probe failed:\n" + r.stderr)
    return bad


def check():
    exprs = [e for e, _ in EXPRESSIONS]
    native = native_values(exprs, '#include "records.h"\n')
    bad32 = i386_mismatches(EXPRESSIONS)
    print("%-62s %8s %8s" % ("expression", "32-bit", "native"))
    for i, (e, n) in enumerate(EXPRESSIONS):
        flag = "  NOT THE OLD NUMBER" if i in bad32 else ""
        print("%-62s %8d %8d%s" % (e, n, native[i], flag))
    prelude = ('#define RECORD_SIZE(tag, n) typedef char tag##_size_unused\n'
               '#define RECORD_OFFSET(tag, m, n) typedef char tag##_##m##_offset_unused\n'
               '#include "../src/engine/xnstruct.h"\n'
               '#undef RECORD_SIZE\n#undef RECORD_OFFSET\n#include "records.h"\n')
    flat = [x for pair in ENGINE_PAIRS for x in pair]
    vals = native_values(flat, prelude)
    print("\nthe game's views of engine structs, natively:")
    wrong = 0
    for i, (g, e) in enumerate(ENGINE_PAIRS):
        a, b = vals[2 * i], vals[2 * i + 1]
        mark = "ok" if a == b else "DIFFERENT"
        wrong += a != b
        print("  %-4s %-66s %4d   %s %d" % (mark, g, a, e, b))
    return 1 if (bad32 or wrong) else 0


# numbers worth a second look: header offsets after its pointers, grown data sizes, 4-byte
# strides and pointer-array byte counts
SCAN = re.compile(
    r"(?<![\w.])(?:[-+] 71\b|0x47\b|[+] 5[59]\b|, 55,|[+] 6[37]\b|0x3[3bBfF7]\b|0x43\b|"
    r"\b634\b|\b659\b|\b560\b|\b367\b|\b371\b|, 62\)|[-+] 18\)|\[18\]|"
    r"xn_str_find_u32\(|<< 2\)\)\)? = \(iptr\)|\(iptr \*\)\(\w+ \+ \(.*<< 2\))")


def scan(files):
    if not files:
        files = sorted(glob.glob(os.path.join(ROOT, "src/lifted/*.c")) +
                       glob.glob(os.path.join(ROOT, "src/hand/*.c")) +
                       glob.glob(os.path.join(ROOT, "src/*.c")))
    n = 0
    for path in files:
        rel = os.path.relpath(path, ROOT)
        for i, line in enumerate(open(path, encoding="latin1"), 1):
            if "mc_set_location" in line or line.lstrip().startswith(("/*", "*")):
                continue
            if SCAN.search(line):
                n += 1
                print("%s:%d: %s" % (rel, i, line.strip()[:160]))
    print("%d lines" % n, file=sys.stderr)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("check")
    s = sub.add_parser("scan")
    s.add_argument("files", nargs="*")
    a = ap.parse_args()
    if a.cmd == "check":
        sys.exit(check())
    scan([os.path.abspath(f) for f in a.files])


if __name__ == "__main__":
    main()
