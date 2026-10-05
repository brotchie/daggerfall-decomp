# Record structs in the source

The lifted C reads the game's records through offset casts, `*(short *)((char *)l_C + 27)`.
include/records.h defines the records as structs, and the converted code reads them by
member, `l_C->days_left`, with every function still byte-identical. This page is the brief
for converting more: the scheme, the workflow, the rules, the Watcom 10.0a behaviour found on
the way, the decisions taken, and what is still unknown.

## Status

- **Phase A** (pilot units: disease, sheet, monster, inven) built records.h and
  tools/offset_casts.py.
- **Phase B** (six agents, by unit) converted the rest of the game: about 7900 casts to 2419.
  The agents could not edit records.h or the tool; their proposals were in
  build/structs/*.md.
- **Phase C** (one owner) merged those proposals into records.h and the tool, fixed the tool,
  and converted what they unlocked: **2419 casts to 1530**, every function byte-identical,
  `tools/build-and-verify.sh` BUILD OK. `offset_casts.py count` also counts casts through
  pointers that are not records (image headers, text cursors, the lifter's frame arrays,
  BIOS tick reads): most of the 1530 are those.
- **Shared structs** (2026-10-05, one owner): one definition for each data structure that
  files had copied locally. Record data went in records.h (logbook, model instance, quest
  NPC, the record +0x1B/+0x2B overlays, house and ship for sale...). Everything else went in
  the new **include/structs.h** (profile, image/CFA/texture headers, BSA and TEXT.RSC
  entries, flc_player, rect, pick_result, notebook entries, collision probes and hits, item,
  magic and monster templates, the SOS sound structs...). The lifter's bit-test helpers went
  in **include/bitfield.h**. Headers need 8.3 names because the compiler runs under DOSBox.
  Result: **574 casts to 231** (the local-naming pass had already taken 1530 to 574 by
  retyping locals), file-local struct definitions 305 to 72.
- **Final round** (2026-10-05, one owner):
  - the RMB and RDB block files in records.h (rmb_file, rdb_file, rdb_object, rdb_model,
    rdb_action, rdb_light...), named after Daggerfall Unity's DFBlock and checked against
    the code;
  - `struct region` (80 bytes) for `regions[]` and current_region_data, in place of the
    per-field globals;
  - the arch3d model headers and planes, and the talk window's tables;
  - the lifter's frame arrays, as local structs with the same layout.

  Result: **231 casts to 2**, local struct definitions 72 to 50.
- **include/records.h**: the structs below, packed, each size checked at compile time
  (`RECORD_SIZE`); `tools/offset_casts.py check` checks every `/* +0xNN */` comment against
  the computed layout.
- **tools/offset_casts.py**: lists a file's offset casts and rewrites them as member
  accesses, compiling every function with Watcom 10.0a and keeping only what matches.

## The scheme

**Every game object is a `struct record *`**: the 71-byte header (type, angles, x/y/z, flags,
id, the tree links) and then `data`, a union of the data structs by record type:

```c
r->type, r->x, r->children, r->next          /* the header */
r->data.item.group                           /* an item (type 2) */
&r->data.character                           /* an entity's character record (3, 18) */
r->data.monster.anim.anim_request            /* a creature's animation struct (18) */
r->data.person.faction_id                    /* an NPC (8) */
```

A pointer to the data itself has the data struct's type: `struct item *l_30 =
&inv_selected_item->data.item;`, `struct disease *l_C = &l_10->data.disease;`. Globals that
hold data pointers have it too (`player_character` is a `struct character *`, `player_class`
a `struct career *`). `sizeof(struct record)` is meaningless (the union); `RECORD_DATA(r)` is
`(char *)r + 0x47` for the rare place that needs bytes.

**The structs** (bytes):
- record header 0x47 + `union record_data`: character 634 (with `career` 74 at +0x230 and
  `character_skill` 6 ×35 at +0x9D), monster 659 (character + `monster_anim` 25), item 107
  (`enchantment` 4 ×10), spell 89 (`spell_effect`, `spell_range`, `spell_magnitude`), disease
  47, potion_recipe 109, membership 13, career (a saved class), location 48, quest 60 (the QBN
  header), settings 6, bank_account 13 ×62, person 3 (type 8), blessing 7 (30), building 26
  (40 quest places, 41 quest NPCs, 64 stored buildings), block 17 (43, the RMB subrecord's
  lists), automap 10240 (51);
- other records and parts: faction 92, map_location 17 (with bitfields), building 26,
  picklist 59 (`picklist_rect`, `picklist_entry` 44), the QBN resources qbn_arg 15, qbn_op 87,
  qbn_state 8, qbn_timer 33, qbn_item 19, qbn_person 20, qbn_place 24, qbn_foe 14,
  qbn_text_var 27 (section 10), the RMB block's block_model 66, block_flat 17 (also the
  people), block_door 19;
- not records: rumor 34 (RUMOR.DAT header), loaded_location 20 (`loaded_location` and the
  quest code's `D_001970C8`), mem_block 18 and mem_pool 16 (jmem.c), link 39 (links.c, an RDB
  action), model_node 20 (the model cache), move_request 30 (the collision code).
- Index enums: `ATTR_*` (attributes), `SKILL_*` (DFU skill order, checked: see below),
  `EQUIP_*` (DFU equip slots).

The class record is `struct career` (names.csv says `class+`; the original source had a
career.c, and `class` reads badly in C). Hand files that define their own `struct item`,
`struct spell`, `struct faction`, `struct career`, `struct link`... clash with records.h once
they include it: replace those structs when converting the file.

**Unions.** One offset with several meanings is an anonymous union:
- **Creature and player meanings** of the same character offset: the player's name first,
  which is the default: `level_skill_sum_start`/`floor_y` (+0x58), `special_infection`/
  `knockback_angle` (+0x6C), `reputation[0]`/`loot_table` (+0x91), `house`/`attack_timer`
  (+0x74), `last_kill_time`/`give_up_timer` (+0x1FD), the `detour_*` (+0x201..+0x209),
  `vampire_clan`/`nav_blocked` ... (+0x21D–0x220), `career_id`/`spawn_seed` (+0x225).
- **Header fields by record type** (+0x13, +0x17, +0x19, +0x1B, +0x1D, +0x24, +0x2B, +0x2F):
  each union starts with a `padNN` member, which means **no default**: the tool names the
  field only under `--prefer` (`--prefer l_14=light_radius`, `--prefer
  cast_fire_missile:a1->children=light_radius`). +0x01 keeps `angle_x` as its default
  (`draw_frame` for drawn flats). Where phase B's old defaults (`owner`,
  `lockpick_skill_tried`, `image`) had landed on another type's field (lights, arrows,
  missiles, type-56 blocks, doors, pedestrians, markers, loot piles), phase C renamed them by
  meaning (object_draw_cb, objcode.c, args.c, maploads.c, maplogic.c, steal.c, talk.c, char.c,
  click.c, text.c, mplace.c, support.c, inven.c, objlib.c, loadsave.c, career.c).
- `monster_anim` +0x10 is also a word (`anim_bits`); `map_location` +0x04/+0x08 are also
  bitfields (`x`, `type`, `discovered`, `hidden`; `z`, `width`, `height`), which
  location_here_contains (src/hand/func_00086560.c) now uses; the lifted code's shift forms
  (`(x_type_flags << 2) >> 27`) stay, as they match.

## Workflow for a unit

A unit is `src/lifted/UNIT.c`, its lift-alone files `src/lifted/UNIT_XXXXXXXX.c`, and the
unit's files in src/hand (by address range, config/units.csv and docs/state.md's
corrections).

1. **Baseline.** `.venv/bin/python tools/wcc10.py src/lifted/UNIT*.c -q` must say all OK.
2. **Globals pass (automatic).** For each file:
   `.venv/bin/python tools/offset_casts.py rewrite --retype src/lifted/FILE.c`.
   It adds `#include "records.h"`, retypes the record globals the file declares as
   `extern char g[];` (the table `GLOBAL_TYPES` in the tool: player_character, player_class,
   player_entity, player_object, current_location, current_building, current_quest,
   guild_membership, selected_spell, spell_records, game_settings, the inventory containers,
   the creature list `D_00190504[]`, ...), and rewrites every cast through them, each function
   verified. Every file has had this pass with the current table; run it again after adding
   globals to `GLOBAL_TYPES`.
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

- **Signed and unsigned reads of one field both exist.** qbn_timer flags are movzx in qcom.c
  and movsx in the timer hand code; quest_init_person's `|=` on the person flags loads with
  movsx (and keeps an address temporary in the frame): `*(short *)&r->flags |= ...` matches,
  `r->flags = (short)r->flags | ...` changes the frame. Keep the field as most code reads it
  and cast at the odd site.
- Same-typed union members compile to the same code: renaming `owner` to `light_radius`, or
  `pad19` to `npc_flags`, never needs a check beyond the compile (it was verified anyway).
- A struct global in memory and its members match the symbol+offset relocations of the
  separate symbols the lifter used: `D_001A9AB8.selected` for `D_001A9AE1` (phase B, the pick
  list), `model_cache_nodes[i].key` for `D_001A5C38 + i * 20` (objlib.c). The build's
  relocation check resolves symbol + addend.

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
- A file that does not compile stops the tool (it prints the first errors). The baseline is
  every cast raw: when a parameter becomes a pointer, write its raw arguments so that they
  compile (`f(a3, a2, (struct model_node *)*(int *)((char *)a3))`); the tool converts the
  inner cast; then write `X` for the `(struct T *)(int)X` round trips it leaves (phase C did
  this over the whole tree, each file verified).
- Walking variable-length records (rumors, memory blocks) keeps the byte step:
  `p = (struct rumor *)(((int)p + p->text_length) + 34);`.
- Functions with switch tables depend on the code size of everything before them in the
  file (docs/progress.md); a failing function can make later ones fail too. The tool tests
  each failing function alone with the rest at a known-good state.

## Phase C: what changed, and the decisions

**records.h, additions:** union members for every header field (above); `struct person`,
`struct blessing`, `struct automap`, `struct block` / `block_model` / `block_flat` /
`block_door`, `struct building` in the data union, `struct rumor`, `struct loaded_location`,
`struct mem_block`, `struct mem_pool`, `struct link`, `struct model_node`, `struct
move_request`, `struct qbn_text_var`; the pads split into named fields: character +0x223
`codeword`, +0x224 `reputation_mod`, +0x225 `career_id`/`spawn_seed`; career +0x08 (still
`pad08`, commented); item +0x30 `magicka_bonus`, +0x41 `variants`, +0x42 `draw_order`;
monster_anim +0x0C `frame_count`; qbn_item `group`, `index`, `symbol`; qbn_person `symbol`;
qbn_place `scope`, `p1`, `p2`, `p3`, `symbol`; qbn_foe `killed`, `symbol`; link +0x0B
`combination`.

**Retypes** (each checked over every file that uses the field, full build after each batch):

| field | was | now | evidence |
|---|---|---|---|
| spell_range, spell_magnitude members | unsigned char | signed char | every read is movsx; the `(signed char)` casts on them were dropped (12 files) |
| potion_recipe.ingredient_groups | unsigned char | signed char | compared with movsx (3 sites) |
| item +0x3F `message` | u32 | u16, then `variants`, `draw_order` | every access is 16-bit; +0x41/+0x42 are template bytes 41/42 |
| settings +0x00 | `view_flags` u8 + `detail_level` u8 | one u16 `view_flags` | every read is a word (34 sites, 12 files), no byte read anywhere; `view_flags & 2` as a byte does not match (head_bob_update). Bit 2: set by load_game when DAGGER.GRD exists (the content filter: FLATS.CFG flag-2 flats hidden, the save-name X/Y filter, the encounter reroll) |
| qbn_person +0x02..+0x03 | pad + u8 `flags` at +3 | u16 `flags` at +2 | quest_init_person, quest_find_site_for_building and talk read +2 as a word; the byte ops at +3 became `0x2000/0x4000/0x8000` and still compile to byte ops |
| qbn_foe +0x04 | short `count` | u8 `count`, u8 `killed` | quest_op09 loops `i < byte[+4]`; qcond_op02 counts byte +5 up |
| qbn_timer `link1`, `link2` | int | `struct record *` | they hold the objects after quest_relink_after_load |
| qbn_timer `delay` | int | unsigned int | the expiry test is unsigned |
| quest | 0x3A, `section_offsets[11]` | 0x3C, `section_offsets[10]`, `int text_offset` | the game reads +0x38 as an int (quest_init_resources, quest_relink_after_load, quest_debug_overlay); DFU's QBN header has a zero u16 (Null1) after section 10's offset; msgbox_show_qrc_text's fake quest is 60 bytes |
| header +0x13 | `char pad13[2]` | union of u16 (`pad13`, `mobile_id`, `anim_time`) and u8 `block_special` | written and read as a word everywhere except sky_update's byte |

**Conflicts between the proposals, and how they were settled:**
- settings +0x00: one u16 (ui) against a union keeping the two bytes (world). One u16: no
  code reads a byte.
- qbn_person flags: u8 at +3 (records.h) against u16 at +2 (objects, econ): u16, by the word
  reads. The timer code and quest_init_person read their flags signed in places: kept with a
  cast there (`(short)t->flags & 64`, `*(short *)&r->flags |= ...`), the field stays unsigned
  (qcom.c's reads are movzx).
- qbn_timer flags: signed in run_0002C5ED/func_0002CB34, unsigned in qcom.c: stays u16 with
  `(short)` casts in the two hand files.
- qbn_foe count: short (names.csv, UESP MobCount) against byte + byte (world): two bytes, by
  the byte reads; a count of 3 stored as a short reads the same.
- qbn_place +0x03..+0x09: `type` + `id_hi`/`id_lo` (quests_text) against `tries`/`kind`/
  `type`/`mode` (objects): both are right, by the value of +0x03. Named after DFU's Places.txt
  `p1 p2 p3`: `scope` (+0x03: 0 a fixed place, then 10; > 0 another location, counted down;
  -1 here), `p1` (+0x04), `p2` (+0x06), `p3` (+0x08); a fixed place keeps its id in p1:p2
  (quest_unlink_for_save checks it when scope is 10).
- spell_range/spell_magnitude and ingredient_groups signedness: signed, by movsx everywhere.
- qbn_arg.record as `void *` (quests_text): not done; the relink code does byte arithmetic on
  it (`record += (int)quest`), which `void *` does not allow.
- Header +0x19 of type 53: bit 14 is female for npc_talk_record_build and is tested and set by
  func_000763C6 before pickpocket_attempt; both are in the `npc_flags` comment.
- Type-43 data: the RMB subrecord's lists (objects), not a building (world's "type 43 place
  data" is the quest place, type 40).
- The skill order (trade.c's haggling reads +0x11B): **the enum is right**. skills[] starts at
  +0x9D, 6 bytes each, DFU's DFCareer order: player_frame_update scales the speed by
  skills[21] (running); the lycanthropy bonus adds 30 to 3, 21, 16, 34, 18, 30 (and 17):
  jumping, running, stealth, critical strike, climbing, hand-to-hand (swimming); lockpick_door
  reads 13; inventory prices read 14 (mercantile, +0xF1); the career popup's hand-to-hand
  damage reads 30 (+0x151). The unused haggling code (func_000684E9, func_0006899B) really
  reads running (+0x11B) next to PER and the merchants' reputation: the original's code.
- `text_macro_fa` is not in GLOBAL_TYPES: the class maker uses it as an image header.
- `D_00190EE4` (a picklist's string list), `D_001995D4` (a 2×2 array) are not in GLOBAL_TYPES.

**tools/offset_casts.py:**
- `retype_globals` no longer rewrites member names equal to a global's name
  (`->data.bank_accounts`) nor the struct tag of a global named like its tag
  (`struct bank_account *bank_account`).
- The records.h parser takes bitfields (`unsigned x:25;`, packed in units of their type); they
  are left out of the member search and shown by `layout`.
- `member_at`: a union whose first member is a `padNN` has no default; among union members
  of different widths, the one the access's width matches.
- `--prefer VAR=NAME` takes a base expression too (`a1->children=light_radius`), before the
  variable's own preference.
- A raw `*(T *)g` of a global the file already declares as a pointer is a load through it: it
  is no longer "fixed" to `*(T *)&g` (that broke sky.c and two hand files' baselines). Only
  `*(T *)&g` is the global's own value.
- Arrays of record pointers: `*(struct X **)((char *)g + (i << 2))` loads, nested index
  expressions and stores (`g[i] = a1`) are rewritten too.
- GLOBAL_TYPES: 25 entries to 99: quest_root `D_00195A00`, `D_00199780` (quest),
  `D_00195D00`, `D_00199768`, quest_event_object(2), quest_prompt_op/quest (qbn_op/quest),
  the inventory containers `D_001959DC..F8`, options_object, logbook_object, bank_accounts,
  bank_account, tavern_building, `D_00196ABC` (building), factions and the faction slots
  `D_0019670C..28`, the talk_npc_* globals, `D_00195A84`, `D_001A3AE0` (character),
  itemmaker_item(_object), guild_npc_object, spell_ready_missile/touch, `D_00199D64` (spell),
  the collision globals `D_00195C48`, `D_00195CB8`, `D_00195C70`, detect_target,
  `D_00195A88`, `D_001A4FE0`, `D_00195CE8`, `D_001970D4`/`D_001970D8`, `D_001A9B14`, the menu
  NPCs, `D_00195AEC`, location_here and `D_00196A9C` (map_location), loaded_location_object/
  data, kludge.c's `D_00199714..24`, nearest_fire, nearest_creature, the automap globals,
  `D_001995EC`/`D_001995E4` (link), model_cache_root (model_node); arrays ai_entities,
  ai_characters, potion_ingredients, potion_cauldron, people_list, collide_candidates.

**Converted in phase C** (casts before → after; the rest of the 2419 → 1530 came from the
globals pass over every file):

| file | before | after | what did it |
|---|---|---|---|
| src/hand/run_0006480F.c | 132 | 10 | `struct link` (links_trigger, link_step) |
| src/lifted/objlib.c | 117 | 25 | the model cache (`model_node`, `model_cache_nodes[]`), `struct block` in rmb_add_subrecord |
| src/lifted/colstuff.c | 140 | 54 | `struct move_request` (func_0002294E, func_00023FA5, func_00023DE0), block walks, header unions |
| src/lifted/links.c | 53 | 9 | `struct link` |
| src/lifted/qinit.c | 49 | 9 | QBN fields, text variables |
| src/lifted/objcode.c | 40 | 0 | block flats in the marker callbacks |
| src/lifted/fs2df.c | 91 | 55 | the links being built (`D_001995EC`) |
| src/lifted/faction.c | 149 | 115 | `struct rumor` (the rest is the FACTION.TXT parser) |
| src/lifted/jmem.c | 32 | 0 | `struct mem_block`, `struct mem_pool` |
| src/lifted/args.c | 56 | 29 | block people and doors, draw handles |
| src/lifted/maploads.c, maplogic.c | 82, 38 | 57, 13 | `struct loaded_location`, region_locations |
| src/hand/run_0006DC35.c | 25 | 0 | `struct person` |
| src/lifted/bank.c | 26 | 2 | block models |
| src/lifted/click.c | 67 | 44 | person data, shelf owner |
| src/lifted/support.c, init.c, qkey.c, automap.c | 49, 33, 14, 33 | 31, 15, 0, 19 | header unions, containers |
| src/hand/func_000830C7.c | 14 | 0 | draw handles, header unions |
| **all files** | **2419** | **1530** | |

Hand files moved onto records.h types: run_0002C5ED (`struct task` → qbn_timer),
func_0002B5EF (`struct Ent` → qbn_text_var), func_00031B8A (`struct Req` → qbn_person, the
NPC_SITE macro → `data.building`), func_00032A05 (`struct T` → qbn_place), func_0001CF3E,
func_0001D766, func_0001D54B (rumor structs), func_0002257C, func_000234EB (move structs),
func_00087F76 (loaded_location), run_0006480F (link).

## Field corrections for config/names.csv

(Not edited; for whoever updates it.)

- settings+0x00 `view_flags`: a u16: bits 0/1 full screen, head bobbing; bit 2 the content
  filter (DAGGER.GRD exists, load_game); bits 8-15 the detail level. Drop settings+0x01
  `detail_level` (it is the high byte).
- item+0x3F `message`: a u16; item+0x41 `variants` (template byte 41: picture variants;
  potions compare it), item+0x42 `draw_order` (template byte 42: the paperdoll order);
  item+0x30 `magicka_bonus` (enchantment 3; taken off magicka when unequipped).
- qbn_person+0x02 `flags` (u16; was +0x03 u8): +0x03 bit 7 is bit 15. qbn_person+0x08
  `symbol`, qbn_item+0x03 `group`, +0x05 `index`, +0x07 `symbol`, qbn_place+0x03 `scope`,
  +0x04 `p1`, +0x06 `p2`, +0x08 `p3`, +0x0C `symbol`, qbn_foe+0x04 `count` (u8; was short),
  +0x05 `killed`, +0x06 `symbol`. qbn_timer+0x15/+0x19: object pointers after the relink.
- quest: 60 bytes; quest+0x38 `text_offset` (int) to the 27-byte text variables
  (qbn_text_var: name[20], section u8 +0x14, index +0x15, record +0x17).
- character+0x223 `codeword` (macro_dbp_codeword: two nibbles), +0x224 `reputation_mod`
  (signed; biography "RR"), +0x225 `career_id` (biography persons) / `spawn_seed`
  (creatures), +0x91 `loot_table` for creatures (1-based).
- monster_anim+0x0C `frame_count`. link+0x0B `combination` (action 129).
- person+0x00 `faction_id` (u16), person+0x02 `flags`: bit 3 shopkeeper, bit 4 female, bit 7
  quest giver. blessing+0x06 `region`.
- object (header) fields by type: +0x13 `mobile_id` (markers, corpses), `anim_time` (animated
  flats), `block_special` (block quarters, a byte), 8000/32768 otherwise; +0x17 per type (see
  records.h: `missile_texture`, `light_radius`, `missile_yaw`, `flat_count`, `trigger_range`,
  `light_level`, `building_type`, `detect_distance`, `anim_frame`, `home_region`,
  `home_building`); +0x19 `faction_id` (quest NPCs), `region` (quest places), `npc_flags`
  (53), `water_level` (47), `spawn_seed` (markers, corpses), `from_player` (arrows, a short);
  +0x1B `soul_creature`, `trap_chance`, `model_count`, `building_index`, `block_number`,
  `shelf_owner`, `location_index`, `seen_count`; +0x1D `trap_duration`; +0x24 `door_angle`;
  +0x2B `expire_minutes`, `name_seed`, `door_swing`, `building_id`, `created_minutes`,
  `move_frame`, `move_remainder`; +0x2F `draw_handle`, `home_id`; +0x01 `draw_frame`.
- Globals: `D_001995EC`/`D_001995E4` the link being built and its chain's first;
  `D_001970C8` a second loaded_location (quest places), so `D_001970D4`/`D_001970D8` are its
  object and data; `model_cache_nodes` is `struct model_node[512]` (`D_001A5C30..40` are its
  fields and node 1).
- From phase A, still to apply: character+0x2CC `effects_list` lies past the record (drop);
  character+0x1F8 is set to 1 for the player by disease_start_cure_quest; disease +0x1B/+0x1D
  are a day range in the templates; poisons use +0x17 as the delay; `D_00195AA8` is a scratch
  "current object", `D_00195AD8` the spell a creature casts.

## Unknown

- character +0x083 (the player's is 1 from a new game), +0x1F1 (creatures 255), +0x22A
  (creatures, from the monster table); career +0x08, +0x2C (8 bytes), +0x35; item +0x28;
  building +0x06, +0x09..+0x0E, +0x10; location +0x23, +0x2F; link +0x15; mem_pool +0x00,
  +0x08; block_model +0x08..+0x23, +0x30, +0x38; the header's +0x13 value 8000.
- Header +0x2B on a creature's arrow (weapon_monster_arrow sets 1) and +0x17 on 3D objects.
- The type-41 quest NPC's data is a building copy, but npc code sets bit 4 of data+2 (the
  person flags' place) for its gender.
- All of these are structs now: notebook elements, the logbook, the RMB and RDB files, the
  monster table, the pick result, image headers and region records (structs.h, records.h).
  XnGine's allocation header is the one layout still read raw.

## What is left

- **2 offset casts**, in kludge.c's kludge_show_memory. It reads XnGine's allocation header,
  22 bytes before each texture archive (the size is at +8), and nothing documents that
  layout.
- **crime.c's court_frame** keeps `extern char region_legal_reputation[]` for one line. With
  any `regions[]` spelling, Watcom evaluates the two sides of the add in the other order.
- **Local copies that stay local** because the shared struct changes the frame:
  - func_0002F824's 60-byte move request view and func_00063FCF's 44-byte one; the shared
    struct moves the parameter spill slots;
  - the two 34-byte rumor copies (a `text[]` member would enlarge `struct rumor`);
  - func_000699D8 and func_00077639's 72-byte record header copies on the stack.
- **Spellings kept because they match**:
  - the loot chance table read as `table[category]` for categories 2 to 14;
  - the notebook page walkers stepping a `char *`;
  - `(*(signed char *)&skill->value)++` in generate.c;
  - byte steps in the arch3d plane walks;
  - the flat `unsigned char *color_remap_tables`;
  - signedness casts at a few sites.
- **Scratch globals** (scratch_190df0, scratch_buffer) are typed per file, because each
  file uses them for something else.
- Unknown member meanings are `unknown_XX`/`pad_XX`. The RDB light's +0x03 and +0x04
  (`action`, `next_object_offset`) are named from the code; DFU calls them Unknown1/2.

## Phase A pilot units

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

