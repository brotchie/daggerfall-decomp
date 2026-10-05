# Record structs in the source

The lifted C reads the game's records through offset casts, `*(short *)((char *)l_C + 27)`.
include/records.h now defines the records as structs, and the pilot units read them by member,
`l_C->days_left`, with every function still byte-identical. This page is the brief for
converting the rest (phase B): the scheme, the workflow, the rules, the Watcom 10.0a
behaviour found on the way, and what is still unknown.

## Status (phase A)

- **include/records.h**: the structs below, packed, each size checked at compile time
  (`RECORD_SIZE`); `tools/offset_casts.py check` checks every `/* +0xNN */` comment against
  the computed layout.
- **tools/offset_casts.py**: lists a file's offset casts and rewrites them as member
  accesses, compiling every function with Watcom 10.0a and keeping only what matches.
- **Pilot units**, all functions byte-identical, `tools/build-and-verify.sh` BUILD OK. Casts
  left are counted by `offset_casts.py count` (which also counts casts through pointers that
  are not records):

| file | casts before | after | what is left |
|---|---|---|---|
| src/lifted/disease.c | 334 | 0 | |
| src/lifted/disease_000679BB.c, disease_000680DD.c | 25 | 0 | |
| src/hand/run_00066352.c | own structs | records.h | (disease_become_vampire) |
| src/lifted/sheet.c | 63 | 8 | text and image pointers, not records |
| src/lifted/sheet_0003B1F3.c | 10 | 4 | `&level_skill_sum` lifter arithmetic |
| src/hand/func_0003B6CF.c, 0003BDB0.c, 0003C610.c | own structs / casts | records.h | |
| src/lifted/monster.c | 175 | 33 | the lifter's frame arrays (move requests), one raw read |
| src/lifted/monster_0006328F.c | 3 | 0 | |
| src/hand/func_0006310D.c, 000631DD.c, 000633BC.c, 00063846.c, 00063FCF.c | own structs | records.h | local move-request structs kept |
| src/lifted/inven.c | 368 | 40 | image headers; 6 functions not typed (below) |
| src/lifted/inven_00094324.c, 000946D4.c, 000973B3.c | 70 | 0 | |
| src/lifted/inven_000945C0.c | 6 | 4 | inv_click_right_item not typed |
| src/hand/func_00095B81.c | own structs | records.h | |

Not converted in the pilot units: inven.c's func_00097764, func_000984E0, func_00099155,
func_00099391, func_000993CB and the `D_00195DA8` uses in inventory_close; inv_click_right_item;
the inven.c hand files other than func_00095B81 (run_0009401E, run_00095F82 and the drawing
ones keep their own `struct obj`/`struct item`/`struct pc`). They are a good first task for
the inventory agent.

## The scheme

**Every game object is a `struct record *`**: the 71-byte header (type, angles, x/y/z, flags,
id, the tree links) and then `data`, a union of the data structs by record type:

```c
r->type, r->x, r->children, r->next          /* the header */
r->data.item.group                           /* an item (type 2) */
&r->data.character                           /* an entity's character record (3, 18) */
r->data.monster.anim.anim_request            /* a creature's animation struct (18) */
```

A pointer to the data itself has the data struct's type: `struct item *l_30 =
&inv_selected_item->data.item;`, `struct disease *l_C = &l_10->data.disease;`. Globals that
hold data pointers have it too (`player_character` is a `struct character *`, `player_class`
a `struct career *`). `sizeof(struct record)` is meaningless (the union); `RECORD_DATA(r)` is
`(char *)r + 0x47` for the rare place that needs bytes.

**The structs** (bytes): record header 0x47 + `union record_data`; character 634 (with
`career` 74 at +0x230 and `character_skill` 6 ×35 at +0x9D); monster 659 (character +
`monster_anim` 25); item 107 (`enchantment` 4 ×10); spell 89 (`spell_effect`, `spell_range`,
`spell_magnitude`); disease 47; potion_recipe 109; membership 13; faction 92; bank_account 13;
map_location 17; location 48; building 26; settings 6; picklist 59 (`picklist_rect`,
`picklist_entry` 44); quest 58 (the QBN header); qbn_arg 15, qbn_op 87, qbn_state 8,
qbn_timer 33, qbn_item 19, qbn_person 20, qbn_place 24, qbn_foe 14. Index enums: `ATTR_*`
(attributes), `SKILL_*` (DFU skill order), `EQUIP_*` (DFU equip slots).

The class record is `struct career` (names.csv says `class+`; the original source had a
career.c, and `class` reads badly in C). Hand files that define their own `struct item`,
`struct spell`, `struct faction`, `struct career` or `struct quest` (19 files, `grep -l
'^struct item {' src/hand/*.c`) clash with records.h once they include it: replace those
structs when converting the file.

**Creature and player meanings** of the same character offset are anonymous unions, the
player's name first: `level_skill_sum_start`/`floor_y` (+0x58), `special_infection`/
`knockback_angle` (+0x6C), `house`/`attack_timer` (+0x74), `last_kill_time`/`give_up_timer`
(+0x1FD), `vampire_clan`/`nav_blocked` ... (+0x21D–0x220), and the new `detour_*` (+0x201,
+0x205, +0x209). The header has `owner`/`lock_level` (+0x17) and `pad19`/`lockpick_skill_tried`
(+0x19) for doors. In creature code ask for the creature names with `--prefer` (below).

## Workflow for a unit

A unit is `src/lifted/UNIT.c`, its lift-alone files `src/lifted/UNIT_XXXXXXXX.c`, and the
unit's files in src/hand (by address range, config/units.csv and docs/state.md's
corrections). Keep the files you own; records.h changes are additions (below).

1. **Baseline.** `.venv/bin/python tools/wcc10.py src/lifted/UNIT*.c -q` must say all OK.
2. **Globals pass (automatic).** For each file:
   `.venv/bin/python tools/offset_casts.py rewrite --retype src/lifted/FILE.c`.
   It adds `#include "records.h"`, retypes the record globals the file declares as
   `extern char g[];` (the table `GLOBAL_TYPES` in the tool: player_character, player_class,
   player_entity, player_object, current_location, current_building, current_quest,
   guild_membership, selected_spell, spell_records, game_settings, the inventory containers,
   the creature list `D_00190504[]`, ...), and rewrites every cast through them, each function
   verified. Over the 150 other lifted files this converts the casts listed under "Survey".
3. **Type the locals and parameters, by hand**, one function at a time:
   - `offset_casts.py scan FILE.c --all --func NAME` lists each base and its offsets. Offsets
     on both sides of 0x47 (0x00 type, 0x37 next, 0x3F children, 0x43 parent, then 0x47+) mean
     an object: `struct record *`. Data offsets only (an item's 0x20 group, 0x2C condition,
     0x43 enchantments...) mean a data pointer: `struct item *`.
   - Change the declaration, then every use that is not a cast: `l_30 = l_28 + 71` becomes
     `l_30 = &l_28->data.item`; `l_1C = a1 + 71` for an entity becomes
     `&a1->data.character`; `(int)l_28 == (int)inv_right_container` becomes
     `l_28 == inv_right_container`; a call passes the pointer (see prototypes below).
     Do not touch the casts themselves; the tool does those.
   - Compile: `wcc10.py FILE.c -q` with no errors and no `W102: Type mismatch` warnings
     (`grep W102 <work dir>/N0000.ERR`; the work dir is printed). W102 is a pointer/int mix
     that still compiles. The functions must still match at this point: if one does not, an
     arithmetic use of a retyped variable changed meaning (see pitfalls).
   - `offset_casts.py rewrite FILE.c --prefer item` (or `--prefer l_24=character`, see
     below). It reports the casts before and after, and the casts it could not place.
   - Clean up by hand what reads badly (`(int)` casts on pointers in comparisons, `&*` forms),
     and recompile.
4. **Hand files** (src/hand): they declare their own structs (`struct obj`, `struct pc`,
   `struct item` that is really a disease, `struct mob`...). Include records.h, delete the
   local structs a records.h type replaces (a tag clash is a compile error), rename the fields
   (their offsets are in the old struct's comments or pads), verify. Local structs that are not
   records (`struct pick`, the move requests, image headers) stay.
5. **Full build**: `tools/build-and-verify.sh` must end `BUILD OK`.

## Rules

- `#include "records.h"` right after the file's opening comment.
- Objects are `struct record *` everywhere: parameters, locals, globals, return values. Use a
  data struct type only for a pointer to the data. Do not invent per-type object structs.
- A local the lifter keeps for a pointer-sized value stays `int` if it is not a record (string
  and image pointers, `l_60 + 36` slots in the lifter's frame arrays).
- Keep the names `l_1C`, `a1`: they are frame slots and renaming is a separate pass.
- **Prototypes.** Each file declares its callees. When an argument is an object, change that
  extern prototype to `struct record *` (or the data struct). Watcom then rejects every call
  in the file that passes an `int` (`E1071 Type of parameter N does not agree`): type the
  argument, or write `(struct record *)x` where x stays an int. Unprototyped callees
  (`extern int mc_memcpy();`) take pointers as they are. A prototype for a function defined
  in the same file must agree with its definition. Different files may still disagree with
  each other (the build links by name); that is fine for now.
- Storing an int into a pointer member: `p->equipped[i] = (struct record *)a1;` (the tool
  writes the cast); storing 0 needs none.
- **Globals**: `extern struct X *g;` for a pointer, `extern struct record *g[];` for an array
  of record pointers, `extern struct disease D_00186A64[];` for a table of records. After
  retyping, every lifted use must be rewritten: `*(int *)g` is now `(int)g`, `*(char **)g + N`
  is `g->member`, `g` alone (its address) is `(char *)&g`. `--retype` does this; add new
  globals to `GLOBAL_TYPES` in the tool rather than retyping by hand.
- **Unions.** `--prefer` chooses: `--prefer item` (for record data, `r->data.item`),
  `--prefer floor_y,nav_blocked` (creature names), `--prefer l_24=character` (only for casts
  through `l_24`), `--prefer inv_use_item:l_24=character` (only in that function). Casts
  through a record into its data are left alone until a preference names the variant.
- Bit tests: the lifter's `((struct bf8_3_1 *)(p + 137))->f == 0` becomes
  `(p->conditions & 0x8) == 0` (the tool does it). Byte read-modify-writes on a wider field
  become word or dword operations with the shifted constant (`p->flags &= ~0x4`,
  `flags |= 0x200`); Watcom narrows them back to the byte.
- `skills[30]` may be written `skills[SKILL_HAND_TO_HAND]` (enum constants are plain ints; the
  tool writes numbers).

## What Watcom 10.0a does with member accesses

Found by compiling every variant against FALL.EXE:

- **Member accesses compile to the same code as the offset casts.** Of about 970 casts
  rewritten in the pilot, all but a handful matched in the natural form, with the lifter's
  conversion casts dropped: `(int)(unsigned char)*(signed char *)((char *)l_1C + 129) == 1`
  is `l_1C->level == 1`, `(int)(short)*(short *)(... + 27)` is `l_C->days_left`. Signedness
  of the field decides movsx/movzx exactly as the casts did, so the field types in records.h
  are the ones the code reads. One spot needed its cast kept: skill_raised_recently returns
  `(1 << a1) & (int)player_character->skills_raised_lo` (an int expression, the field is
  unsigned).
- Index arithmetic is the same too: `*(short *)((char *)((l_2C * 2) + l_18) + 31)` is
  `l_18->drained[l_2C]`; `*(short *)(p + 157 + (X * 6))` is `p->skills[X].value`;
  `spell_records[l_14].id` for `*(char **)spell_records + (l_14 * 89) + 73`;
  `D_00190504[i]->x` for the creature list.
- Typing a local as a struct pointer instead of `int`, a parameter as `struct record *`, a
  prototype's parameter or return as a pointer, and `short *l_34 = p->base_attributes;
  l_34[i]` for `*(short *)((char *)(i * 2) + l_34)`: no change in the code.
- Bit tests on a `u32` field are narrowed to the byte (`test byte [eax+0x8a], 1` for
  `conditions & 0x100`), and `|=`/`&=`/`^=` of a constant on a `u16`/`int` field to a byte
  operation, as the lifter's byte casts had it.
- Reading the low half of a wider field: `p->max_health = (short)p->max_health_base` matches
  the original word read.
- **A change can move registers in a later statement.** In monster_apply_gravity,
  `mc_memcpy(.., &D_00190504[i]->x, ..)` matched everywhere except a register choice two
  statements later; `(int)D_00190504[i] + 7` matches. Watcom's register and slot sorts are
  unstable (docs/progress.md), so one more or one fewer tree node anywhere in a block can do
  this. When a function fails, try the other spelling of each spot you edited by hand.
- **Partial conversions can fail where the whole one matches.** In sheet.c's func_0003D0A8,
  `player_character->skills[(int)(unsigned char)*(signed char *)((char *)(*(int *)&player_class
  + l_1C) + 16)].value` did not match, `player_character->skills[player_class->skills[l_1C]]
  .value` does. Convert inner casts first (the tool does innermost first).
- Not converted: the settings record's first word read as a `u16` and tested with `& 4`
  (inv_draw_container_icon; names.csv has bytes there), and a creature's `y` read as a
  `char *` and added to (monster_apply_gravity): no member form matched, they stay raw.

## Pitfalls

- **Retyping changes arithmetic.** A local that becomes a pointer scales `+`: the lifter's
  `(l_2C * 2) + l_18` silently becomes `l_18 + 2*47*l_2C`. Before retyping, look for every
  `+`/`-` on the variable that is not inside `(char *)v + N`. The tool's raw fallback keeps
  byte arithmetic (`+ (int)v`) for the address forms it knows, but code it does not touch is
  yours. The compiler says nothing; the match check does.
- **Retyping a global** from `char g[]` to a pointer turns `*(int *)g` into a load through
  the pointer. Use `--retype`, which rewrites these first.
- An array of record pointers declared `struct record *g[]` scales `g + (i << 2)` by 4.
  `--retype` writes `(char *)g` for those uses.
- E1071 is an error, W102 only a warning: a pointer passed to an `int` prototype is caught,
  an int assigned to a pointer is not.
- A file that does not compile stops the tool (it prints the first errors).
- Functions with switch tables depend on the code size of everything before them in the
  file (docs/progress.md); a failing function can make later ones fail too. The tool tests
  each failing function alone with the rest at a known-good state.

## Unknown, and proposed corrections

For config/names.csv and docs/state.md (not edited here):

- `character+0x2CC effects_list` (candidate) lies past the 634-byte character record (the
  player entity is created with 634 bytes, a creature with 659); drop it.
- `character+0x1F8` for the player: set to 1 by disease_start_cure_quest when the vampire
  cure quest is offered. A creature's is the AI action.
- New creature fields at character+0x201/+0x205/+0x209 (func_000622EB's detour: heading,
  steps left with 1000 for none, and a side bit): named `detour_yaw`, `detour_steps`,
  `detour_side` here, candidates.
- `character+0x8A` bit 0 (conditions bit 8, silence) stops creatures casting (monster.c).
- func_000631DD's hand struct called character+0x89 `flags`: it is `conditions` (invisible,
  chameleon, shadow: bits 2, 12, 13).
- func_0003BDB0's hand struct called character+0x151 `level`: it is
  `skills[SKILL_HAND_TO_HAND].value` (the career-skill popup shows hand-to-hand damage).
- run_00066352.c's `struct item` was a disease record (`drained` at +0x1F) and its
  `dis->kind` at faction+0x23 is `vampire_clan`.
- Disease templates (D_00186A64, now `struct disease[]`): +0x1B and +0x1D are a range of days
  (days_left = rand_range(+0x1B, +0x1D)), then +0x1D is reset and used as the stage.
- Poisons use disease+0x17 (`damage_min`) as the delay before they start.
- Header +0x19 is a short; a new loot pile gets 1 (inventory_open).
- The settings record's first word is read as a u16 (`& 4`) by inv_draw_container_icon:
  `view_flags` and `detail_level` may be one u16 of flags, or bit 2 is something else.
- `D_00195AA8` is a scratch "current object" (the creature for poison damage, the item for
  enchantment ticks, the wagon): a `struct record *`. `D_00195AD8` is the spell object a
  creature casts. `D_00190504` is the creature list (creature_count record pointers).
- `item+0x34 dropped_image` is confirmed by inv_click_paperdoll: a dropped item's world
  image is set from it.
- Unnamed so far: character +0x083 (short, init sets 1), +0x1F1, +0x223, +0x224 (chargen RR),
  +0x225, +0x22A; career +0x08, +0x2C (8 bytes), +0x35; item +0x28, +0x30; the header's +0x13;
  building +0x06, +0x09..+0x0E, +0x10; location +0x23, +0x2F; most of the QBN resources.
- Not yet structs: the RMB block, link records (39 bytes), rumors (34-byte header), the
  logbook, automap, notebook elements, the move request the collision code takes (x, y, z,
  angles, a table pointer, flags: 30 bytes, now a local struct in two hand files and char
  arrays in the lifted ones), the monster table (29 bytes), building lists (`struct building`
  exists).

## Survey: the globals pass over the other 150 lifted files

`offset_casts.py rewrite --retype` on a scratch copy of every other src/lifted file (not
applied to src; a few seconds to a minute per file):

- every file still matched everywhere after the rewrite (no failures, no compile errors);
- 1290 of 6770 casts became member accesses, in 67 files, with no hand work (parse.c 89,
  click.c 84, support.c 67, equip.c 62, intrface.c 56, custom.c 56, text.c 47, ...);
- what is left: 2556 casts through parameters and 2468 through locals (step 3 of the
  workflow), and 437 through pointer globals the table does not know yet. The most used:
  `D_001995EC` 29, `rmb_block` 25, `D_00196D48` 21, `bank_account` 20 (a `struct
  bank_account *` by its name), `D_00195A00` 16 (`quest_root`, a record), `D_00199780`,
  `D_0019671C`, `D_00196484`, `D_00195D28`, `tavern_building` 11 (a building),
  `location_here` 10, `quest_event_object` 9 (a record), `itemmaker_item`, `talk_npc_faction`
  (a faction). Add the ones whose struct is known to `GLOBAL_TYPES` before the pass.

## Phase B

- Agents take whole units (the files of step "Workflow"), so no two agents edit the same file.
  Start each unit with the globals pass, then the hand typing; the biggest units by casts are
  equip.c, click.c, support.c, runspell.c, parse.c, qkey.c, spfx.c.
- records.h is shared. Adding a field by splitting a `padNN` (or adding a union member) is
  safe for everyone: the size checks and `offset_casts.py check` catch a mistake. Renaming or
  retyping an existing field touches every converted file: say so in the report instead, and
  let one agent do it with a full build. A new struct goes at the end of its section, before
  `union record_data` if it is a record's data (then add it to the union).
- `GLOBAL_TYPES` in the tool is shared the same way: add entries, run `--retype` on your own
  files only.
- Report per unit: casts before and after (`offset_casts.py count`), the functions left untyped
  and why, field corrections for names.csv.
