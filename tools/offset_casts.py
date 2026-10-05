#!/usr/bin/env python3
"""Record offset casts in the C sources: list them, and rewrite them as struct member accesses
with every function checked against FALL.EXE.

The lifted code reads records through casts: `*(short *)((char *)l_C + 27)`,
`*(int *)(*(char **)player_character + 133)`. include/records.h has the record structs. This
tool reads records.h's layout (it computes every offset, packed), finds the casts whose base
has a struct type, and rewrites them (docs/structs.md has the rules and the workflow).

usage:
  offset_casts.py check
        compute records.h's layouts; check every `/* +0xNN` comment against the computed
        offset; print each struct's size
  offset_casts.py layout STRUCT
        print a struct's fields with offsets, sizes and types
  offset_casts.py scan FILE.c [--as BASE=STRUCT ...] [--func NAME] [--all]
        list the offset casts of FILE.c: function, line, base+offset, the access type and
        its conversion casts, and the field. BASE is a local or parameter (l_1C, a1) or
        `*global` (a global holding a pointer). Globals in GLOBAL_TYPES are built in.
        Without --all only bases with a known struct are listed.
  offset_casts.py count FILE.c ...
        the number of offset casts left in each file (the progress measure; it also counts
        casts through pointers that are not records: image headers, the lifter's frame arrays)
  offset_casts.py rewrite FILE.c [-o OUT] [--retype] [--prefer LIST] [--dry]
        rewrite the casts through struct-typed names as member accesses: globals declared
        `extern struct X *g;` (or `extern struct X *g[];`), and parameters and locals declared
        `struct X *v`. Every function is compiled with Watcom 10.0a and compared with FALL.EXE:
        a function keeps the natural form (casts dropped) when it matches, else the form that
        keeps the conversion casts, else the casts that match one at a time; the rest stay raw.
        Repeats until nothing changes (outer casts become reachable once inner ones are done).
        --retype first turns the file's `extern char g[];` into the typed declaration for the
        globals in GLOBAL_TYPES (their loads become `*(T *)&g`, their address `(char *)&g`).
        --prefer picks among union members: NAME (everywhere), VAR=NAME or FUNC:VAR=NAME (VAR
        may be a base expression, `a1->children`); a record's data (`r->data.X`) needs it,
        --prefer item, --prefer l_28=item, and so do the header unions that start with a padNN
        (records.h), --prefer l_14=light_radius.
        --dry lists the casts and their natural form without compiling.

Address forms recognised: `(char *)V + N`, `(char *)V`, `*(char **)G + N`,
`(char *)*(int *)G + N`, `(char *)(V + (I * K))`, each with an index term after the constant;
`*(T *)G` of a typed global; `*(T *)(G + (I << 2))` of an array of record pointers; the
lifter's bit tests `((struct bf8_B_1 *)(V + N))->f == 0`.
"""
import argparse
import collections
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RECORDS_H = os.path.join(ROOT, "include", "records.h")

# globals that hold a pointer to a record, and the struct they point to. `rewrite --retype`
# turns a lifted file's `extern char g[];` into `extern struct X *g;`, and `scan` reads
# `*(char **)g + N` as a member of X.
GLOBAL_TYPES = {
    "player_character": "character",
    "player_class": "career",
    "player_entity": "record",
    "player_object": "record",
    "camera_object": "record",
    "nonworld_root": "record",
    "D_00195AC4": "record",
    "D_00195AA8": "record",
    "current_location": "location",
    "current_building": "building",
    "current_quest": "quest",
    "guild_membership": "membership",
    "selected_spell": "spell",
    "spell_records": "spell",
    "game_settings": "settings",
    "inv_left_container": "record",
    "inv_right_container": "record",
    "D_00195B34": "record",             # inv_right_container_base (candidate)
    "inv_selected_item": "record",
    "inv_temp_pile": "record",
    "wagon_container": "record",
    "D_00195AF4": "record",             # an item record (inventory, arrows)
    "D_00195A80": "item",               # the item data shown by inv_item_info
    # phase C: merged from the phase B agents' notes (build/structs/*.md)
    "D_00195A00": "record",             # quest_root: its children are the type-14 quests
    "D_00199780": "quest",              # quest_tick_data: the running quest's data
    "D_00195D00": "record",             # quest_tick_object
    "D_00199768": "record",             # the quest reward container
    "quest_event_object": "record",
    "quest_event_object2": "record",
    "quest_prompt_op": "qbn_op",        # qaction_op29_prompt; quest_prompt_answer
    "quest_prompt_quest": "quest",
    "D_001959DC": "record",             # inventory_containers[1]
    "D_001959E0": "record",             # inventory_containers[2] (letters of credit)
    "D_001959E4": "record",             # inventory_containers[3]
    "D_001959EC": "record",             # inventory_containers[5]: house_container
    "D_001959F0": "record",             # inventory_containers[6]: ship_container
    "D_001959F4": "record",             # inventory_containers[7]: room_storage_container
    "D_001959F8": "record",             # inventory_containers[8]: repair_container
    "options_object": "record",
    "logbook_object": "record",
    "bank_accounts": "record",          # the type-25 record: data.bank_accounts[62]
    "bank_account": "bank_account",     # &bank_accounts->data.bank_accounts[region] (bank_open)
    "tavern_building": "building",
    "D_00196ABC": "building",           # tavern_room_list: the rented rooms' building entries
    "factions": "faction",              # the faction tree (an array; the _r walkers' root)
    "D_0019670C": "faction",            # the %-macro / rumor-template faction slots (parse.c)
    "D_00196714": "faction",
    "D_0019671C": "faction",            # faction_match
    "D_00196720": "faction",
    "D_00196724": "faction",
    "D_00196728": "faction",
    "talk_npc_own_faction": "faction",
    "talk_npc_faction": "faction",
    "talk_npc_record": "character",
    "talk_npc_object": "record",
    "D_00195A84": "character",          # text_macro_npc: npc_talk_record_build's record
    "D_001A3AE0": "character",          # trade.c haggling: the customer (a4 + 71)
    "itemmaker_item": "item",           # &record->data.item (itemmaker_pick_item)
    "itemmaker_item_object": "record",
    "guild_npc_object": "record",       # a scratch "found object" for callbacks
    "spell_ready_missile": "record",
    "spell_ready_touch": "record",
    "D_00199D64": "spell",
    "D_00195C48": "record",             # the object a collision hit
    "D_00195CB8": "record",             # the floor object (automap_open walks its parents)
    "D_00195C70": "record",             # the moving model
    "detect_target": "record",
    "D_00195A88": "record",             # the player object's parent while riding (main.c)
    "D_001A4FE0": "record",             # the root load_fix_object_cb searches
    "D_00195CE8": "record",             # the clicked NPC (quest_init_person)
    "D_001970D4": "record",             # the other town's location object (quest places)
    "D_001970D8": "location",           # its data
    "D_001A9B14": "record",             # object_find_by_id's result
    "repair_menu_npc": "record",
    "coven_menu_npc": "record",
    "service_menu_npc": "record",
    "D_00195AEC": "record",             # the shelf whose items are shown
    "location_here": "map_location",
    "D_00196A9C": "map_location",       # region_locations: MAPTABLE, indexed x17
    "loaded_location_object": "record",
    "loaded_location_data": "location",
    "D_00199714": "record",             # kludge.c door and creature scratch
    "D_00199720": "record",
    "D_00199724": "record",
    "nearest_fire": "record",
    "nearest_creature": "record",
    "D_00196DA0": "record",             # automap_record (type 51)
    "D_00196DB0": "record",             # automap_selected_object
    "D_001995EC": "link",               # the link fs2df.c is building (in D_00199D78[])
    "D_001995E4": "link",               # the first link of its chain
    "model_cache_root": "model_node",
    # arrays of record pointers
    "inventory_containers[]": "record",
    "D_00190504[]": "record",           # the creatures (creature_count of them)
    "ai_entities[]": "record",          # ai_update_creatures fills it from D_00190504
    "ai_characters[]": "character",     # ai_entities[i] + 71
    "potion_ingredients[]": "record",
    "potion_cauldron[]": "record",
    "people_list[]": "record",          # pedestrians (people_count of them)
    "collide_candidates[]": "record",   # func_0002325A fills up to 128
}
GLOBAL_BASES = {"*" + g: t for g, t in GLOBAL_TYPES.items() if not g.endswith("[]")}

BASIC = {"char": 1, "signed char": 1, "unsigned char": 1, "short": 2, "unsigned short": 2,
         "int": 4, "unsigned int": 4, "unsigned": 4, "long": 4, "unsigned long": 4,
         "void": 1}


# ---- records.h layout ---------------------------------------------------------------------

class Field:
    def __init__(self, name, off, ctype, dims, size, elem, bits=None):
        self.name, self.off, self.ctype, self.dims, self.size, self.elem = \
            name, off, ctype, dims, size, elem   # elem: size of one element (array) or size
        self.bits = bits                         # a bitfield: (first bit, width) in its unit

    def __repr__(self):
        return "%s @%#x %s%s%s" % (self.name, self.off, self.ctype,
                                   "".join("[%d]" % d for d in self.dims),
                                   ":%d" % self.bits[1] if self.bits else "")


def strip_comments(text):
    """Keep `/* +0x..` comments as markers (`@OFF@`), drop the others."""
    def repl(m):
        c = m.group(0)
        mm = re.match(r"/\*\s*\+0x([0-9A-Fa-f]+)", c)
        return " @%s@ " % mm.group(1) if mm else " "
    text = re.sub(r"/\*.*?\*/", repl, text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def parse_records(path=RECORDS_H):
    """{tag: (size, [Field])} with offsets computed (packed), and a list of problems."""
    raw = open(path).read()
    lines = raw.splitlines()
    text = "\n".join(l for l in lines if not l.lstrip().startswith("#"))
    text = re.sub(r"RECORD_(SIZE|OFFSET)\([^)]*\);", "", text)
    text = strip_comments(text)
    toks = re.findall(r"@[0-9A-Fa-f]+@|[A-Za-z_]\w*|\d+|[{}\[\];,*():]", text)
    structs = {}
    problems = []
    pos = [0]

    def peek(k=0):
        return toks[pos[0] + k] if pos[0] + k < len(toks) else None

    def take(expect=None):
        t = toks[pos[0]]
        if expect is not None and t != expect:
            raise SyntaxError("expected %r, got %r near token %d" % (expect, t, pos[0]))
        pos[0] += 1
        return t

    def parse_type():
        words = []
        while peek() in ("unsigned", "signed", "char", "short", "int", "long", "void", "const"):
            w = take()
            if w != "const":
                words.append(w)
        if words:
            if words == ["unsigned"]:
                words = ["unsigned", "int"]
            if words == ["signed"]:
                words = ["int"]
            return " ".join(words), None
        if peek() == "struct":
            take()
            if peek() == "{":
                return None, "anon-struct"
            return "struct " + take(), None
        if peek() == "union":
            take()
            if peek() == "{":
                return None, "anon-union"
            return "struct " + take(), None     # a named union: kept with the structs
        raise SyntaxError("type expected at %r" % peek())

    def size_of(ctype):
        if ctype in BASIC:
            return BASIC[ctype]
        if ctype.startswith("struct "):
            tag = ctype[7:]
            if tag not in structs:
                raise SyntaxError("struct %s used before it is defined" % tag)
            return structs[tag][0]
        raise SyntaxError("unknown type " + ctype)

    def parse_members(base, is_union):
        """Parse members up to '}'; return (size, fields). Bitfields share a unit of their
        type's size (`unsigned x:25; unsigned type:5;` is one 4-byte unit), as Watcom packs them."""
        fields = []
        off = 0
        size = 0
        pending = []    # fields declared since the last offset marker
        unit = None     # the open bitfield unit: [offset in the struct, size, bits used]
        while peek() != "}":
            if peek().startswith("@"):
                # an offset comment belongs to the members declared just before it
                want = int(take()[1:-1], 16)
                for f in pending:
                    if f.off != want:
                        problems.append("%s: comment says +0x%X, layout gives +0x%X"
                                        % (f.name, want, f.off))
                pending = []
                continue
            ctype, anon = parse_type()
            start = base + (0 if is_union else off)
            if anon or peek(1) != ":":
                unit = None
            if anon:
                take("{")
                sub_size, sub_fields = parse_members(start, anon == "anon-union")
                take("}")
                take(";")
                fields += sub_fields
                pending = sub_fields[:1]
                msize = sub_size
            else:
                msize = 0
                pending = []
                while True:
                    ptr = 0
                    while peek() == "*":
                        take()
                        ptr += 1
                    name = take()
                    dims = []
                    while peek() == "[":
                        take()
                        dims.append(int(take()))
                        take("]")
                    elem = 4 if ptr else size_of(ctype)
                    n = 1
                    for d in dims:
                        n *= d
                    fctype = ctype + " " + "*" * ptr if ptr else ctype
                    if peek() == ":":
                        take()
                        nbits = int(take())
                        if unit is None or unit[1] != elem or unit[2] + nbits > 8 * elem:
                            unit = [base + (0 if is_union else off + msize), elem, 0]
                            msize += elem
                        f = Field(name, unit[0], fctype.strip(), [], elem, elem, (unit[2], nbits))
                        unit[2] += nbits
                        fields.append(f)
                        if not pending:
                            pending.append(f)
                        if peek() == ",":
                            take()
                            continue
                        take(";")
                        break
                    unit = None
                    fstart = base + (0 if is_union else off + msize)
                    f = Field(name, fstart, fctype.strip(), dims, elem * n, elem)
                    fields.append(f)
                    if not pending:
                        pending.append(f)   # `short x, y;  /* +0x00 */` dates the first
                    msize += elem * n
                    if peek() == ",":
                        take()
                        continue
                    take(";")
                    break
            if is_union:
                size = max(size, msize)
            else:
                off += msize
                size = off
        return size, fields

    while pos[0] < len(toks):
        t = take()
        if t in ("struct", "union") and peek(1) == "{":
            tag = take()
            take("{")
            size, fields = parse_members(0, t == "union")
            take("}")
            end_marker = None
            if peek() == ";":
                take()
            if peek() and peek().startswith("@"):
                end_marker = int(take()[1:-1], 16)
                if end_marker != size:
                    problems.append("struct %s: end comment says +0x%X, size is +0x%X"
                                    % (tag, end_marker, size))
            structs[tag] = (size, fields)
    return structs, problems


def leaves(structs, tag, base=0, prefix="", bits=False):
    """Flatten a struct into leaf fields: (offset, size, path, ctype, dims, elem). Bitfields
    are left out (an offset cast never names one) unless `bits`: their ctype is then
    `unsigned int:FIRST:WIDTH`."""
    out = []
    for f in structs[tag][1]:
        p = prefix + f.name
        if f.bits:
            if bits:
                out.append((base + f.off, f.size, p, "%s:%d:%d" % ((f.ctype,) + f.bits), [], f.elem, None))
            continue
        if f.ctype.startswith("struct ") and not f.ctype.endswith("*"):
            sub = f.ctype[7:]
            if f.dims:
                out.append((base + f.off, f.size, p, f.ctype, f.dims, f.elem, sub))
            else:
                out += leaves(structs, sub, base + f.off, p + ".", bits)
        else:
            out.append((base + f.off, f.size, p, f.ctype, f.dims, f.elem, None))
    return out


def field_at(structs, tag, off, width):
    """The member path at byte offset `off` of struct `tag` (an access of `width` bytes)."""
    best = None
    for (o, size, path, ctype, dims, elem, sub) in leaves(structs, tag):
        if not (o <= off < o + size):
            continue
        rel = off - o
        if dims:
            # index into the array (all dimensions flattened row-major)
            k, r = divmod(rel, elem)
            idx = []
            for d in reversed(dims[1:]):
                idx.append(k % d)
                k //= d
            idx.append(k)
            p = path + "".join("[%d]" % i for i in reversed(idx))
            if sub:
                inner = field_at(structs, sub, r, width)
                return (p + "." + inner[0], inner[1], inner[2]) if inner else (p, ctype, elem)
            if r == 0:
                return (p, ctype, elem)
            return ("%s+%d" % (p, r), ctype, elem)
        if rel == 0:
            best = (path, ctype, size)
            if not path.split(".")[-1].startswith("pad"):
                return best
        elif best is None:
            best = ("%s+%d" % (path, rel), ctype, size)
    return best


# ---- scanning the C ---------------------------------------------------------------------

TYPES = (r"(?:signed char|unsigned char|char|unsigned short|short|unsigned int|unsigned|int|"
         r"char \*\*|char \*|int \*|short \*|unsigned short \*|unsigned char \*)")
DEREF = re.compile(r"\*\((" + TYPES + r") ?\*\)\(")
CONV = re.compile(r"((?:\((?:int|unsigned char|short|unsigned short|unsigned|unsigned int|"
                  r"signed char)\))+)\s*$")
FUNC = re.compile(r"^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*$")
WIDTH = {"signed char": 1, "unsigned char": 1, "char": 1, "short": 2, "unsigned short": 2}


def matching_paren(s, i):
    d = 1
    while d:
        c = s[i]
        d += (c == "(") - (c == ")")
        i += 1
    return i - 1


def split_terms(e):
    terms, d, cur = [], 0, ""
    for c in e:
        d += (c == "(") - (c == ")")
        if c == "+" and d == 0:
            terms.append(cur.strip())
            cur = ""
        else:
            cur += c
    terms.append(cur.strip())
    return terms


BASE_FORMS = [
    (re.compile(r"^\*\(char \*\*\)(\w+)$"), lambda m: "*" + m.group(1)),
    (re.compile(r"^\(char \*\)\*\(int \*\)(\w+)$"), lambda m: "*" + m.group(1)),
    (re.compile(r"^\(char \*\)(\w+)$"), lambda m: m.group(1)),
]


def parse_addr(addr):
    base, off, idx = None, 0, []
    for t in split_terms(addr):
        if re.fullmatch(r"\d+", t):
            off += int(t)
            continue
        if base is None:
            for rx, f in BASE_FORMS:
                m = rx.match(t)
                if m:
                    base = f(m)
                    break
            else:
                return None, 0, []
        else:
            idx.append(t)
    return base, off, idx


def scan(path):
    """[(func, line, base, offset, access type, conversion casts, index terms, text)]"""
    return scan_text(open(path).read())


def scan_text(s):
    line_starts = [0] + [m.end() for m in re.finditer(r"\n", s)]
    funcs = []
    for m in re.finditer(r"(?m)^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{)]*\)\s*\n\{", s):
        funcs.append((m.start(), m.group(1)))
    out = []
    import bisect
    for m in DEREF.finditer(s):
        j = matching_paren(s, m.end())
        base, off, idx = parse_addr(s[m.end():j])
        if base is None:
            continue
        pre = s[max(0, m.start() - 60):m.start()]
        cm = CONV.search(pre)
        conv = cm.group(1) if cm else ""
        ln = bisect.bisect_right(line_starts, m.start())
        k = bisect.bisect_right([f[0] for f in funcs], m.start()) - 1
        fn = funcs[k][1] if k >= 0 else "?"
        out.append((fn, ln, base, off, m.group(1), conv, idx, s[m.start():j + 1]))
    return out


# ---- rewriting offset casts as member accesses ---------------------------------------------

ASSIGN_OPS = ("<<=", ">>=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "++", "--")
CAST_LIST = re.compile(r"\(([a-z ]+)\)")
UNARY_AMP = re.compile(r"(?:^|[(,=]|\((?:int|char \*|unsigned)\))\s*&$")


def find_functions(s):
    """[(name, start, body_open, body_end, params)] for each function definition."""
    out = []
    for m in re.finditer(r"(?m)^[A-Za-z_][\w \*]*?\b(\w+)\s*\(([^;{)]*)\)\s*\n\{", s):
        i = m.end() - 1
        d, j = 0, i
        while True:
            c = s[j]
            if c == "{":
                d += 1
            elif c == "}":
                d -= 1
                if d == 0:
                    break
            j += 1
        out.append((m.group(1), m.start(), i, j + 1, m.group(2)))
    return out


def typed_names(s, funcs):
    """Globals declared `extern struct X *g;`, and each function's struct-pointer parameters
    and locals: ({name: tag}, {func: {name: tag}})."""
    glob = {m.group(2): m.group(1) for m in
            re.finditer(r"(?m)^extern\s+struct\s+(\w+)\s*\*\s*(\w+)\s*;", s)}
    # arrays of record pointers, `extern struct X *g[];`, are keyed "g[]"
    glob.update({m.group(2) + "[]": m.group(1) for m in
                 re.finditer(r"(?m)^extern\s+struct\s+(\w+)\s*\*\s*(\w+)\s*\[\d*\]\s*;", s)})
    per = {}
    for name, _st, bo, be, params in funcs:
        t = {m.group(2): m.group(1) for m in re.finditer(r"struct\s+(\w+)\s*\*\s*(\w+)", params)}
        for m in re.finditer(r"(?m)^\s+struct\s+(\w+)\s*\*\s*(\w+)\s*;", s[bo:be]):
            t[m.group(2)] = m.group(1)
        per[name] = t
    return glob, per


def find_field(structs, tag, name):
    for f in structs[tag][1]:
        if f.name == name:
            return f
    return None


CHAIN = re.compile(r"^(\w+(?:\[[^\[\]]+\])?)((?:(?:->|\.)\w+(?:\[[^\[\]]+\])*)*)$")


def chain_tag(structs, expr, names, glob):
    """The struct tag a member chain (`a1->hdr.parent`) points to, or None."""
    m = CHAIN.match(expr)
    if not m:
        return None
    root = m.group(1)
    if "[" in root:
        tag = glob.get(root[:root.index("[")] + "[]")
    else:
        tag = names.get(root) or glob.get(root)
    if tag is None or tag not in structs:
        return None
    ptr = True
    for op, fname, subs in re.findall(r"(->|\.)(\w+)((?:\[[^\[\]]+\])*)", m.group(2)):
        if (op == "->") != ptr:
            return None
        f = find_field(structs, tag, fname)
        if f is None or subs.count("[") != len(f.dims):
            return None
        ct = f.ctype
        if ct.startswith("struct ") and ct.endswith("*"):
            tag, ptr = ct[7:-1].strip(), True
        elif ct.startswith("struct "):
            tag, ptr = ct[7:], False
        else:
            return None
        if tag not in structs:
            return None
    return tag if ptr else None


def leaf_width(ctype):
    return 4 if ctype.endswith("*") else BASIC.get(ctype)


def member_at(structs, tag, off, prefer=(), width=None):
    """(path, ctype) of the scalar member that starts exactly at `off`, or None. Among union
    members: a --prefer name, else the first one (of the access's `width` when one is), but
    none when the union's first member is a padNN (records.h: no default meaning)."""
    cands = []
    for (o, size, path, ctype, dims, elem, sub) in leaves(structs, tag):
        if not (o <= off < o + size):
            continue
        rel = off - o
        if dims:
            k, r = divmod(rel, elem)
            idx = []
            for d in reversed(dims[1:]):
                idx.append(k % d)
                k //= d
            idx.append(k)
            p = path + "".join("[%d]" % i for i in reversed(idx))
            if sub:
                inner = member_at(structs, sub, r, prefer)
                if inner:
                    cands.append((p + "." + inner[0], inner[1]))
            elif r == 0:
                cands.append((p, ctype))
        elif rel == 0 and not (ctype.startswith("struct ") and not ctype.endswith("*")):
            cands.append((path, ctype))
    if not cands:
        return None

    def parts(path):
        return [q.split("[")[0] for q in path.split(".")]
    for want in prefer:                 # in order: the first preference that matches wins
        for c in cands:
            if want in parts(c[0]):
                return c
    real = [c for c in cands if not parts(c[0])[-1].startswith("pad")]
    # a record's data is a union by record type: which one must be said (--prefer item)
    variants = {parts(c[0])[1] for c in real if parts(c[0])[0] == "data" and len(parts(c[0])) > 1}
    if len(variants) > 1:
        return None
    if real and parts(cands[0][0])[-1].startswith("pad"):
        return None             # a union without a default: --prefer says which
    if real and width:
        same = [c for c in real if leaf_width(c[1]) == width]
        if same:
            return same[0]
    return real[0] if real else cands[0]


def index_term(t):
    """An address term `(X * E)`, `(E * X)`, `(X << k)` or `X`: (X, E)."""
    t = t.strip()
    if t.startswith("(") and matching_paren(t, 1) == len(t) - 1:
        inner = t[1:-1].strip()
    else:
        inner = t
    m = re.fullmatch(r"(.+?)\s*\*\s*(\d+)", inner)
    if m and _balanced(m.group(1)):
        return _strip(m.group(1)), int(m.group(2))
    m = re.fullmatch(r"(\d+)\s*\*\s*(.+)", inner)
    if m and _balanced(m.group(2)):
        return _strip(m.group(2)), int(m.group(1))
    m = re.fullmatch(r"(.+?)\s*<<\s*(\d+)", inner)
    if m and _balanced(m.group(1)):
        return _strip(m.group(1)), 1 << int(m.group(2))
    return _strip(t), 1


def _balanced(e):
    d = 0
    for c in e:
        d += (c == "(") - (c == ")")
        if d < 0:
            return False
    return d == 0


def _strip(e):
    e = e.strip()
    while e.startswith("(") and matching_paren(e, 1) == len(e) - 1 and \
            not re.fullmatch(r"\([a-z ]+\*?\)", e[:e.index(")") + 1]):
        e = e[1:-1].strip()
    return e


def indexed_member(structs, tag, off, x, stride, prefer=()):
    """(path, ctype) for `base + off + x * stride` as an array element, or None."""
    for (o, size, path, ctype, dims, elem, sub) in leaves(structs, tag):
        if not dims or not (o <= off < o + size):
            continue
        row = elem
        for d in dims[1:]:
            row *= d
        if row != stride:
            continue
        k, r = divmod(off - o, row)
        ix = x if k == 0 else "%s + %d" % (x, k)
        p = "%s[%s]" % (path, ix)
        if len(dims) == 2 and not sub:
            c, rr = divmod(r, elem)
            if rr:
                return None
            return ("%s[%d]" % (p, c), ctype)
        if len(dims) > 2:
            return None
        if sub:
            inner = member_at(structs, sub, r, prefer)
            return (p + "." + inner[0], inner[1]) if inner else None
        return (p, ctype) if r == 0 else None
    return None


class Site:
    """One offset cast and what it can become: alternatives 'n' (natural), 'c' (keeps the
    conversion casts), 'r' (raw: the original text, or `&g` for a retyped global)."""

    def __init__(self, start, end, alts, func, line, note=""):
        self.start, self.end, self.alts, self.func, self.line, self.note = \
            start, end, alts, func, line, note


def member_cover(structs, tag, off, prefer=()):
    """(path, ctype, rel): the scalar member that covers byte `off`, `rel` bytes into it."""
    for k in range(0, 4):
        if off - k < 0:
            break
        m = member_at(structs, tag, off - k, prefer)
        if m:
            w = leaf_width(m[1]) or 0
            return (m[0], m[1], k) if k < w else None
    return None


def resolve_base(e, names, glob, structs):
    """(base expression, tag, kind) for `(char *)E`'s E: a typed name or a member chain."""
    e = e.strip()
    if e in names:
        return e, names[e], "local"
    ct = chain_tag(structs, e, names, glob)
    if ct:
        return e, ct, "chain"
    return None


def parse_site_addr(addr, names, glob, structs):
    """The address of a cast: (base, tag, kind, const offset, [index terms]) or None."""
    terms = split_terms(addr)
    first, rest = terms[0], terms[1:]
    got = None
    fix = None
    mm = re.match(r"^(?:\*\(char \*\*\)|\(char \*\)\*\(int \*\))&?(\w+)$", first)
    if mm and mm.group(1) in glob:
        got = (mm.group(1), glob[mm.group(1)], "global")
    else:
        mm = re.match(r"^\(char \*\)(?:\(int\))?(.+)$", first)
        if mm:
            e = mm.group(1).strip()
            got = resolve_base(e, names, glob, structs)
            if got is None and e.startswith("(") and matching_paren(e, 1) == len(e) - 1:
                # (char *)(A + B): a base and an index term
                two = split_terms(e[1:-1])
                if len(two) == 2:
                    for x, y in (two, two[::-1]):
                        g = re.match(r"^(?:\*\(char \*\*\)&?|\*\(int \*\)&?|\(int\))(\w+)$", x)
                        if g and g.group(1) in names and x.startswith("(int)"):
                            got = (g.group(1), names[g.group(1)], "local")
                        elif g and g.group(1) in glob and not x.startswith("(int)"):
                            got = (g.group(1), glob[g.group(1)], "global")
                        else:
                            got = resolve_base(_strip(x), names, glob, structs)
                            if got:
                                # raw, `(i * 2) + p` must stay byte arithmetic: `+ (int)p`
                                fix = (e, e.replace(x, "(int)" + x, 1))
                        if got:
                            rest = [y] + rest
                            break
    if not got:
        return None
    off, idx = 0, []
    for tt in rest:
        if re.fullmatch(r"\d+", tt):
            off += int(tt)
        else:
            idx.append(tt)
    return got + (off, idx, fix)


def _store_rhs(s, eq):
    """For `lhs = RHS;` with s[eq] the '=': (RHS text, index of the ';'), at depth 0."""
    d, k = 0, eq + 1
    while k < len(s):
        c = s[k]
        if c in "([":
            d += 1
        elif c in ")]":
            d -= 1
            if d < 0:
                return None
        elif c == ";" and d == 0:
            return s[eq + 1:k].strip(), k
        k += 1
    return None


def _cast_to(ctype, e):
    """`(ctype)e`, with parens around e unless it is a plain operand."""
    if e == "0":
        return "0"
    if re.fullmatch(r"[\w.\[\]>-]+|\*\([\w ]+\*\)\w+|\w+\([^;]*\)", e) and _balanced(e):
        return "(%s)%s" % (ctype, e)
    return "(%s)(%s)" % (ctype, e)


def _postfix(e):
    """True when `e` is a plain postfix expression (a member chain), safe without parens."""
    while True:
        e2 = re.sub(r"\[[^\[\]]*\]", "", e)
        if e2 == e:
            break
        e = e2
    return re.fullmatch(r"[\w.]+(?:->[\w.]+)*", e) is not None


def site_prefer(prefer_all, func, base):
    """The member names preferred for a cast through `base` in `func`: `--prefer` items are
    NAME (everywhere), VAR=NAME (casts through VAR, or through the base expression VAR:
    `a1->children=light_radius`) or FUNC:VAR=NAME; the whole base expression first, then the
    variable, then the plain names."""
    root = re.match(r"\w+", base).group(0)
    exact, scoped, plain = [], [], []
    for p in prefer_all:
        if "=" in p:
            lhs, name = p.split("=", 1)
            f, _c, var = lhs.rpartition(":")
            if f and f != func:
                continue
            if var == base and var != root:
                exact.append(name)
            elif var == root:
                scoped.append(name)
        else:
            plain.append(p)
    return tuple(exact + scoped + plain)


def collect_sites(s, structs, prefer=()):
    """Sites of every function in `s`, innermost casts only; and skipped casts with reasons."""
    prefer_all = prefer
    funcs = find_functions(s)
    glob, per = typed_names(s, funcs)
    line_starts = [0] + [m.end() for m in re.finditer(r"\n", s)]
    import bisect
    sites, skipped = [], []
    # `*(T *)&g`: the value of a retyped global (`*(T *)g` without the & is a load through a
    # global already declared as a pointer: left alone)
    bare = re.compile(r"(&)?\*\((" + TYPES + r") ?\*\)&(\w+)\b(?!\s*\()")
    for fname, _st, bo, be, _params in funcs:
        names = per[fname]
        body = s[bo:be]
        cand = []
        # `*(T *)((char *)g + (x << 2))` of an array of record pointers: g[x]
        for m in re.finditer(r"\*\((int|char \*|struct \w+ \*) ?\*\)(\()(?:\(char \*\))?(\w+)\b", body):
            g = m.group(3)
            if g + "[]" not in glob:
                continue
            tag = glob[g + "[]"]
            a = bo + m.start()
            j = matching_paren(s, bo + m.end(2))
            inner = s[bo + m.end(2):j]
            terms = split_terms(inner)
            if _strip(terms[0]) not in ("(char *)" + g, g) or len(terms) > 2:
                continue
            if len(terms) == 1:
                x, e = "0", 4
            elif re.fullmatch(r"\d+", terms[1]):
                if int(terms[1]) % 4:
                    continue
                x, e = str(int(terms[1]) // 4), 4
            else:
                x, e = index_term(terms[1])
            if e != 4:
                continue
            b = j + 1
            t = m.group(1)
            rawt = s[a:b]
            elem = "%s[%s]" % (g, x)
            after = s[b:b + 16].lstrip()
            if after.startswith("=") and not after.startswith("=="):
                st = _store_rhs(s, s.index("=", b))
                if not st:
                    cand.append((a, b, {"r": rawt}, ""))
                    continue
                rhs, semi = st
                mm = re.fullmatch(r"\(int\)(\w+)", rhs)
                if t == "struct %s *" % tag or (mm and names.get(mm.group(1)) == tag) or rhs == "0":
                    val = mm.group(1) if mm and names.get(mm.group(1)) == tag else rhs
                else:
                    val = _cast_to("struct %s *" % tag, rhs)
                cand.append((a, semi, {"n": "%s = %s" % (elem, val), "r": s[a:semi]}, ""))
            elif after.startswith(ASSIGN_OPS):
                cand.append((a, b, {"r": rawt}, ""))
            elif t == "struct %s *" % tag:
                cand.append((a, b, {"n": elem, "r": rawt}, ""))
            else:
                cand.append((a, b, {"n": "(%s)%s" % (t, elem), "r": rawt}, ""))
        for m in DEREF.finditer(body):
            t = m.group(1)
            ds = bo + m.start()
            j = matching_paren(s, bo + m.end())
            got = parse_site_addr(s[bo + m.end():j], names, glob, structs)
            if got is None or got[1] not in structs:
                continue
            base, tag, kind, off, idx, fix = got
            prefer = site_prefer(prefer_all, fname, base)
            ln = bisect.bisect_right(line_starts, ds)
            raw = s[ds:j + 1]
            if fix:
                raw = raw.replace(fix[0], fix[1], 1)

            def skip(why):
                skipped.append((fname, ln, why))
                if raw != s[ds:j + 1]:
                    cand.append((ds, j + 1, {"r": raw}, why))
            if len(idx) > 1:
                skip("two index terms")
                continue
            size = structs[tag][0]
            btext = base
            width = WIDTH.get(t, 4)
            if idx:
                x, stride = index_term(idx[0])
                if stride == size:
                    # an array of records: base[x]
                    k, off = divmod(off, size)
                    btext = "%s[%s]" % (base, x if k == 0 else "%s + %d" % (x, k))
                    mem = member_at(structs, tag, off, prefer, width)
                else:
                    mem = indexed_member(structs, tag, off, x, stride, prefer)
            else:
                mem = member_at(structs, tag, off, prefer, width)
            sep = "." if btext != base else "->"
            # context
            before = s[:ds]
            after = s[j + 1:j + 16].lstrip()
            lvalue = before.rstrip().endswith(("++", "--")) or \
                (after.startswith(ASSIGN_OPS) or (after.startswith("=") and not after.startswith("==")))
            wrapped = False
            if not lvalue and before.rstrip().endswith("(") and after.startswith(")"):
                a2 = after[1:].lstrip()
                lvalue = a2.startswith(ASSIGN_OPS) or (a2.startswith("=") and not a2.startswith("=="))
                wrapped = lvalue
            amp = UNARY_AMP.search(before.rstrip()[-12:]) is not None
            if amp and not lvalue:
                # `&*(T *)(p + N)`: the address of the member there, whatever its width
                if mem is None:
                    cov = None if idx else member_cover(structs, tag, off, prefer)
                    if cov and cov[2] == 0:
                        mem = (cov[0], cov[1])
                if mem is None:
                    skip("no member at %s+0x%X%s" % (tag, off, " [i]" if idx else ""))
                    continue
                member = "%s%s%s" % (btext, sep, mem[0])
                start = len(before.rstrip()) - 1
                cand.append((start, j + 1, {"n": ("&" + member) if t == mem[1] else "(%s *)&%s" % (t, member),
                                            "r": "&" + raw}, ""))
                continue
            if mem is None or leaf_width(mem[1]) != width:
                # a narrower access to a wider field
                cov = None if idx else member_cover(structs, tag, off, prefer)
                if cov and lvalue and not wrapped and (leaf_width(cov[1]) or 0) > width:
                    mm = re.match(r"(\|=|&=|\^=)\s*(\d+)\s*;", after)
                    if mm:
                        op, c = mm.group(1), int(mm.group(2))
                        sh = 8 * cov[2]
                        if op == "&=":
                            newc = "~0x%X" % (((~c) & ((1 << (8 * width)) - 1)) << sh)
                        else:
                            newc = "0x%X" % (c << sh) if sh else str(c)
                        end = j + 1 + s[j + 1:].index(";")
                        cand.append((ds, end, {"n": "%s%s%s %s %s" % (btext, sep, cov[0], op, newc),
                                               "r": raw + s[j + 1:end]}, ""))
                        continue
                if cov and not lvalue and cov[2] == 0 and not cov[1].endswith("*"):
                    cand.append((ds, j + 1, {"n": "(%s)%s%s%s" % (t, btext, sep, cov[0]), "r": raw}, ""))
                    continue
                if mem is None:
                    skip("no member at %s+0x%X%s%s" % (tag, off, " [i]" if idx else "",
                                                       " (record data: say which, --prefer item|character|...)"
                                                       if tag == "record" and off >= 0x47 else ""))
                else:
                    skip("%s->%s is %s, read as %s" % (tag, mem[0], mem[1], t))
                continue
            path, F = mem
            member = "%s%s%s" % (btext, sep, path)
            start, end = ds, j + 1
            ptrF = F.endswith("*")
            if lvalue:
                if ptrF and t != F:
                    st = _store_rhs(s, s.index("=", j + 1)) if after.startswith("=") and not wrapped else None
                    if st:
                        rhs, semi = st
                        cand.append((start, semi, {"n": "%s = %s" % (member, _cast_to(F, rhs)),
                                                   "r": raw + s[j + 1:semi]}, ""))
                        continue
                    skip("store of %s into pointer %s->%s" % (t, tag, path))
                    continue
                else:
                    alts = {"n": member, "r": raw}
            else:
                cm = CONV.search(before[-60:])
                chain = cm.group(1) if cm else ""
                chain_txt = ""
                if chain:
                    start = ds - len(before[-60:]) + cm.start(1)
                    chain_txt = s[start:ds]
                casts = CAST_LIST.findall(chain)
                conservative = chain_txt + ("(%s)" % t if t != F else "") + member
                if ptrF:
                    natural = conservative
                    # a pointer member needs no cast where a pointer is wanted: `p = member;`
                    # into a struct pointer, or a test against 0
                    ma = re.search(r"(?<![=!<>])\b(\w+)\s*=\s*$", before[-80:])
                    dest = None
                    if ma and not chain_txt and after.startswith(";"):
                        dest = names.get(ma.group(1)) or glob.get(ma.group(1))
                    if dest:
                        ftag = F[7:-1].strip() if F.startswith("struct ") else None
                        natural = member if ftag == dest else "(struct %s *)%s" % (dest, member)
                    elif not chain_txt and re.match(r"(==|!=)\s*0\b", after):
                        natural = member
                else:
                    cs = list(casts)
                    while cs and cs[-1] == F:
                        cs.pop()
                    if cs == ["int"]:
                        cs = []
                    natural = "".join("(%s)" % c for c in cs) + member
                alts = {"n": natural, "c": conservative, "r": chain_txt + raw}
                if alts["n"] == alts["c"]:
                    del alts["c"]
            # `(member)` needs no parens: take them into the site
            nat = alts.get("n")
            if nat and _postfix(nat) and s[start - 1] == "(" and s[end] == ")":
                pb = s[:start - 1].rstrip()
                if pb and not (pb[-1].isalnum() or pb[-1] in "_)]"):
                    alts = {k: (v if k == "n" else "(" + v + ")") for k, v in alts.items()}
                    start, end = start - 1, end + 1
            cand.append((start, end, alts, ""))
        # the lifter's bit tests, `((struct bf8_B_1 *)(p + N))->f == 0`: `(p->member & MASK) == 0`
        for m in re.finditer(r"\(\(struct bf8_(\d)_1 \*\)\(", body):
            a = bo + m.start()
            j = matching_paren(s, bo + m.end())          # the address's closing paren
            if not s.startswith(")->f", j + 1):
                continue
            b = j + 5
            if not re.match(r"\s*[!=]=\s*0\b", s[b:b + 8]):
                continue
            got = parse_site_addr(s[bo + m.end():j], names, glob, structs)
            if got is None or got[1] not in structs or got[4]:
                continue
            base, tag, kind, off, idx, fix = got
            cov = member_cover(structs, tag, off, site_prefer(prefer_all, fname, base))
            if not cov or cov[1].endswith("*"):
                continue
            mask = 1 << (8 * cov[2] + int(m.group(1)))
            raw = s[a:b]
            if fix:
                raw = raw.replace(fix[0], fix[1], 1)
            cand.append((a, b, {"n": "(%s->%s & 0x%X)" % (base, cov[0], mask), "r": raw}, ""))
        # bare `*(T *)g` of a retyped global: its value (not inside a cast handled above)
        spans = [(a0, b0) for a0, b0, _x, _y in cand]
        for m in bare.finditer(body):
            g = m.group(3)
            a, b = bo + m.start(), bo + m.end()
            if g not in glob or any(a0 <= a and b <= b0 for a0, b0 in spans):
                continue
            t = m.group(2)
            tt = t + ("" if t.endswith("*") else " ")
            after = s[b:b + 16].lstrip()
            if m.group(1):
                cand.append((a, b, {"c": "(%s*)&%s" % (tt, g), "r": "&*(%s*)&%s" % (tt, g)}, ""))
            elif after.startswith(ASSIGN_OPS) or (after.startswith("=") and not after.startswith("==")):
                st = _store_rhs(s, s.index("=", b)) if after.startswith("=") else None
                if st:
                    rhs, semi = st
                    cand.append((a, semi, {"n": "%s = %s" % (g, _cast_to("struct %s *" % glob[g], rhs)),
                                           "r": "*(%s*)&%s = %s" % (tt, g, rhs)}, ""))
                else:
                    cand.append((a, b, {"r": "*(%s*)&%s" % (tt, g)}, ""))
            else:
                cand.append((a, b, {"n": "(%s)%s" % (t, g), "r": "*(%s*)&%s" % (tt, g)}, ""))
        # innermost only: drop a candidate that contains another
        cand.sort()
        for k, (a, b, alts, note) in enumerate(cand):
            if any(a <= a2 and b2 <= b and (a2, b2) != (a, b) for a2, b2, _x, _y in cand):
                continue
            sites.append(Site(a, b, alts, fname, bisect.bisect_right(line_starts, a), note))
    return sites, skipped


def apply(s, sites, choice):
    """`s` with each site replaced by its alternative choice[i] ('r' leaves raw)."""
    out = []
    pos = 0
    for i in sorted(range(len(sites)), key=lambda k: sites[k].start):
        st = sites[i]
        if st.start < pos:
            continue
        out.append(s[pos:st.start])
        out.append(st.alts.get(choice[i], st.alts["r"]))
        pos = st.end
    out.append(s[pos:])
    return "".join(out)


class Verifier:
    """Compile many variants of a file in one DOSBox-X run; {variant: {func: ok}}."""

    def __init__(self):
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        import build_fall
        import match
        from le import LE
        self.match, self.build_fall = match, build_fall
        self.tgt = match.Target()
        self.syms = match.symbol_map()
        self.le = LE(match.EXE)
        self.fix_at = {f.src_va: f for f in self.le.fixups()}
        self.gsyms = build_fall.load_symbols()
        self.flags = match.default_flags()
        self.compiles = 0
        self.errors = []

    def run(self, texts, tmp):
        import shutil
        import wcc10
        from omf import OMF
        paths = []
        for k, t in enumerate(texts):
            p = os.path.join(tmp, "v%05d.c" % k)
            with open(p, "w") as f:
                f.write(t)
            paths.append(p)
        res = []
        for lo in range(0, len(paths), 400):
            chunk = paths[lo:lo + 400]
            objs, td = wcc10.compile_many(chunk, self.flags)
            self.compiles += len(chunk)
            for k, p in enumerate(chunk):
                obj = objs[p]
                if obj is None:
                    try:
                        err = open(os.path.join(td, "N%04d.ERR" % k), errors="replace").read()
                    except OSError:
                        err = ""
                    self.errors = [l for l in err.splitlines() if "Error!" in l][:8]
                    res.append(None)
                    continue
                o = OMF(obj)
                fns = o.functions()
                r = {}
                for pub, si, off, fsize in fns:
                    name = self.build_fall.c_name(pub)
                    if name not in self.syms:
                        continue
                    va, size = self.syms[name]
                    ok, _d = self.match.compare(self.tgt, o, name, va, size, quiet=True)
                    if ok:
                        lo2 = max([q + z for _p, s2, q, z in fns if s2 == si and q < off] + [0])
                        bad = self.build_fall.check_relocs(o, si, lo2, off, fsize, va, self.tgt,
                                                           self.fix_at, self.syms, self.gsyms,
                                                           self.le, {})
                        if bad is None and lo2 < off and self.tgt.bytes_at(
                                va - (off - lo2), off - lo2) != bytes(o.data[si][lo2:off]):
                            bad = "table"
                        ok = bad is None
                    r[name] = ok
                res.append(r)
            shutil.rmtree(td, ignore_errors=True)
        return res


def settle(s, sites, ver, tmp, log):
    """Choose an alternative for every site so that each function still matches."""
    funcs = sorted({st.func for st in sites})
    by_func = collections.defaultdict(list)
    for i, st in enumerate(sites):
        by_func[st.func].append(i)
    raw = ["r"] * len(sites)
    choice = list(raw)
    base, = ver.run([apply(s, sites, raw)], tmp)
    if base is None:
        raise SystemExit("the file does not compile with every cast raw (check the declarations):\n  "
                         + "\n  ".join(getattr(ver, "errors", [])))
    broken = [f for f, ok in base.items() if not ok]
    if broken:
        log("baseline (every cast raw) already fails: %s" % ", ".join(broken))

    def combine(over=None):
        """The settled choice, with function f's sites from `over` = (f, choice)."""
        c = list(choice)
        if over:
            f, oc = over
            for i in by_func[f]:
                c[i] = oc[i]
        return c

    todo = [f for f in funcs if f not in broken]
    # 1. natural everywhere, 2. conservative everywhere
    for mode in ("n", "c"):
        if not todo:
            break
        trial = list(choice)
        for f in todo:
            for i in by_func[f]:
                trial[i] = mode if mode in sites[i].alts else ("c" if "c" in sites[i].alts else "n")
                if trial[i] not in sites[i].alts:
                    trial[i] = "r"
        r, = ver.run([apply(s, sites, trial)], tmp)
        if r is None:
            log("mode %s: compile error" % mode)
            continue
        nxt = []
        for f in todo:
            if r.get(f):
                for i in by_func[f]:
                    choice[i] = trial[i]
            else:
                nxt.append(f)
        log("mode %s: %d of %d functions match" % (mode, len(todo) - len(nxt), len(todo)))
        todo = nxt
    # 3. one site at a time, each function alone against the settled rest
    for f in todo:
        idx = by_func[f]
        variants, keys = [], []
        for i in idx:
            for mode in ("n", "c"):
                if mode not in sites[i].alts:
                    continue
                oc = list(raw)
                oc[i] = mode
                variants.append(apply(s, sites, combine((f, oc))))
                keys.append((i, mode))
        res = ver.run(variants, tmp)
        best = {}
        for (i, mode), r in zip(keys, res):
            if r and r.get(f) and i not in best:
                best[i] = mode
        cand = [i for i in idx if i in best]
        # 4. grow the set: the longest passing prefix, drop the site after it, go on
        acc = []
        rest = list(cand)
        while rest:
            variants = []
            for k in range(1, len(rest) + 1):
                oc = list(raw)
                for i in acc + rest[:k]:
                    oc[i] = best[i]
                variants.append(apply(s, sites, combine((f, oc))))
            res = ver.run(variants, tmp)
            k = 0
            while k < len(rest) and res[k] and res[k].get(f):
                k += 1
            acc += rest[:k]
            rest = rest[k + 1:]
        for i in acc:
            choice[i] = best[i]
        log("%s: %d of %d sites by search" % (f, len(acc), len(idx)))
    return choice


def retype_globals(s, names=None):
    """A lifted file's `extern char g[];` for the globals in GLOBAL_TYPES become
    `extern struct X *g;`. Uses that are not a load through the pointer (`*(T *)g`,
    `*(char **)g + N`), i.e. the global's own address, become `(char *)&g`; the loads are
    left for the rewrite (whose raw form is `*(T *)&g`). Adds the #include. Returns the text
    and the globals retyped."""
    done = []
    for g, tag in GLOBAL_TYPES.items():
        if names and g not in names:
            continue
        if g.endswith("[]"):
            # an array of record pointers: every use keeps its byte arithmetic, (char *)g
            g = g[:-2]
            decl = "extern char %s[];" % g
            if decl not in s:
                continue
            s = s.replace(decl, "extern struct %s *%s[];" % (tag, g))
            out, pos = [], 0
            for m in re.finditer(r"(?<![.>])\b%s\b" % re.escape(g), s):
                if re.search(r"(?:struct \w+ \*|\bstruct\s+)$", s[max(0, m.start() - 60):m.start()]):
                    continue
                out.append(s[pos:m.start()])
                deref = re.search(r"\*\((?:" + TYPES + r") ?\*\)$", s[max(0, m.start() - 30):m.start()])
                out.append("(char *)%s" % g if re.match(r"\s*\+", s[m.end():]) and not deref
                           else "((char *)%s)" % g)
                pos = m.end()
            out.append(s[pos:])
            s = "".join(out)
            done.append(g + "[]")
            continue
        decl = "extern char %s[];" % g
        if decl not in s:
            continue
        s = s.replace(decl, "extern struct %s *%s;" % (tag, g))
        # every other use of g: a deref through it stays, the address becomes (char *)&g
        out, pos = [], 0
        # (not a member of the same name, `->data.bank_accounts`, nor a struct tag of the same
        # name, `struct bank_account *bank_account;`)
        for m in re.finditer(r"(?<![.>])\b%s\b" % re.escape(g), s):
            if m.start() < pos:
                continue
            pre = s[max(0, m.start() - 60):m.start()]
            if re.search(r"(?:struct \w+ \*|\bstruct\s+)$", pre):
                continue
            out.append(s[pos:m.start()])
            if re.search(r"\*\((?:" + TYPES + r") ?\*\)$", pre):
                out.append("&" + g)              # a load through g: *(T *)&g, the same value
            else:
                out.append("((char *)&%s)" % g)  # g's own address
            pos = m.end()
        out.append(s[pos:])
        s = "".join(out)
        done.append(g)
    if done and '#include "records.h"' not in s:
        # after the file's opening comment
        m = re.match(r"\s*(/\*.*?\*/[ \t]*\n)?", s, re.S)
        k = m.end()
        s = s[:k] + '#include "records.h"\n' + ("" if s[k:k + 1] == "\n" else "\n") + s[k:]
    return s, done


def rewrite(path, out, prefer, log, keep_tmp=False, retype=False):
    import tempfile
    structs, problems = parse_records()
    if problems:
        raise SystemExit("records.h: " + "; ".join(problems))
    s = open(path).read()
    if retype:
        s, done = retype_globals(s)
        if done:
            log("retyped: " + ", ".join(done))
    ver = Verifier()
    tmp = tempfile.mkdtemp(prefix="offcasts_")
    total = collections.Counter()
    all_skipped = []
    for gen in range(4):
        sites, skipped = collect_sites(s, structs, prefer)
        if gen == 0:
            all_skipped = skipped
        if not sites:
            break
        log("pass %d: %d casts to rewrite" % (gen + 1, len(sites)))
        choice = settle(s, sites, ver, tmp, log)
        for c in choice:
            total[c] += 1
        # raw sites stay as they were (with `&g` for a retyped global)
        s2 = apply(s, sites, choice)
        if s2 == s:
            break
        s = s2
        if all(c == "r" for c in choice):
            break
    # the result must still match everywhere it matched
    final, = ver.run([s], tmp)
    with open(out, "w") as f:
        f.write(s)
    if not keep_tmp:
        import shutil
        shutil.rmtree(tmp, ignore_errors=True)
    log("casts: %d before, %d after (%d natural, %d with the conversion casts kept); %d compiles"
        % (len(scan_text(open(path).read())), len(scan_text(s)), total["n"], total["c"],
           ver.compiles))
    funcs = find_functions(s)
    glob, per = typed_names(s, funcs)
    bad = [f for f, ok in (final or {}).items() if not ok]
    if final is None:
        log("FINAL COMPILE FAILED")
    elif bad:
        log("FAILING after the rewrite: " + ", ".join(bad))
    for fn, ln, why in all_skipped:
        log("  skipped %s:%d %s" % (fn, ln, why))
    return final is not None and not bad


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd")
    sub.add_parser("check")
    lp = sub.add_parser("layout")
    lp.add_argument("struct")
    sp = sub.add_parser("scan")
    sp.add_argument("file")
    sp.add_argument("--as", dest="as_", action="append", default=[])
    sp.add_argument("--func")
    sp.add_argument("--all", action="store_true")
    cp = sub.add_parser("count")
    cp.add_argument("files", nargs="+")
    rp = sub.add_parser("rewrite")
    rp.add_argument("file")
    rp.add_argument("-o", dest="out")
    rp.add_argument("--prefer", default="")
    rp.add_argument("--dry", action="store_true", help="list the sites, don't compile")
    rp.add_argument("--retype", action="store_true",
                    help="first retype the file's record globals (GLOBAL_TYPES)")
    a = ap.parse_args()
    if a.cmd == "rewrite":
        structs, _p = parse_records()
        prefer = tuple(x for x in a.prefer.split(",") if x)
        if a.dry:
            s = open(a.file).read()
            if a.retype:
                s, _d = retype_globals(s)
            sites, skipped = collect_sites(s, structs, prefer)
            for st in sites:
                print("%-28s %5d  %-40s -> %s" % (st.func[:28], st.line,
                                                  s[st.start:st.end][:40], st.alts.get("n", st.alts.get("c"))))
            for fn, ln, why in skipped:
                print("skipped %s:%d %s" % (fn, ln, why))
            return
        ok = rewrite(a.file, a.out or a.file, prefer, lambda m: print(m, flush=True),
                     retype=a.retype)
        sys.exit(0 if ok else 1)
    if a.cmd == "count":
        tot = 0
        for f in a.files:
            n = len(scan(f))
            tot += n
            print("%5d  %s" % (n, f))
        if len(a.files) > 1:
            print("%5d  total" % tot)
        return
    structs, problems = parse_records()
    if a.cmd == "check":
        for tag, (size, _f) in structs.items():
            print("struct %-18s %4d bytes (0x%X)" % (tag, size, size))
        for p in problems:
            print("PROBLEM:", p)
        sys.exit(1 if problems else 0)
    if a.cmd == "layout":
        for (o, size, path, ctype, dims, elem, _sub) in leaves(structs, a.struct, bits=True):
            print("+0x%03X %4d  %-3d %-18s %s%s" % (o, o, size, ctype, path,
                                                 "".join("[%d]" % d for d in dims)))
        return
    if a.cmd == "scan":
        bases = dict(GLOBAL_BASES)
        for kv in a.as_:
            k, v = kv.split("=")
            bases[k] = v
        rows = scan(a.file)
        for fn, ln, base, off, t, conv, idx, text in rows:
            if a.func and fn != a.func:
                continue
            tag = bases.get(base)
            if tag is None and not a.all:
                continue
            field = ""
            if tag:
                width = WIDTH.get(t, 4)
                fa = field_at(structs, tag, off, width)
                if fa:
                    path, ctype, fsize = fa
                    note = ""
                    fw = BASIC.get(ctype, 4)
                    if "+" in path or (fw != width and not ctype.startswith("struct")):
                        note = "  <- %s, %d-byte access" % (ctype, width)
                    field = "%s->%s%s%s" % (tag, path, "[+idx]" if idx else "", note)
                else:
                    field = "%s: past the end" % tag
            print("%-28s %5d  %-14s +0x%03X %-13s %-28s %s" % (
                fn[:28], ln, base, off, t, conv, field))
        return
    ap.print_help()


if __name__ == "__main__":
    main()
