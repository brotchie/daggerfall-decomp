# Naming convention

How names in config/names.csv (and so in the source) are formed. This came out of the
consistency pass over the first ~4000 names; new names follow it.

**Form**
- Names are lower_case identifiers.
- Functions are `<subsystem>_<verb>_<object>`; globals are `<subsystem>_<noun>`.
- The subsystem is:
  - the original unit's subject (`inv_`, `talk_`, `spfx_`, `kludge_`, `init_`, `load_` for
    loadsave.c's load path), or
  - a cross-unit concept (`object_`, `item_`, `spell_`, `gold_`, `text_draw_`, `msgbox_`,
    `list_popup_`).
- Library functions keep their own names.

**One word per concept**

| concept | word | not |
|---|---|---|
| a node of the object tree (`struct record`) | object (`object_find`, `found_object`, `scratch_object`, `*_object`) | record, entity (except `player_entity`, type 3) |
| a record of a file format or table (BSA, MAPS, QBN section, SAVETREE chunk, CFG) | record (`archive_find_record`, `quest_record`, `savetree_*_record`, `location_read_record`) | |
| a search or walk result | `found_<kind>` | `<kind>_match`, `*_result` for new names |
| type 2 | item | thing |
| type 8, `struct person` | person; the QBN resource is `qbn_person`, the biography's is `bio_person` | |
| type 53, people.c | pedestrian (one), people (the list: `people_list`, `people_count`) | person |
| anyone you can talk to (8, 53, creatures in talk) | npc (`npc_talk`, `npc_load_face`) | |
| type 18 | monster or creature (synonyms; prefer monster in new names: monster.c, `struct monster`) | |
| QBN resources | foe, qbn_item, qbn_person, qbn_place | |
| the class record (CLASSnn.CFG) | class (`player_class`, `class+` rows, `classmaker_`); the C type is `struct career`, and the member is `career` | |
| career.c's biography code | `career_` | |
| a number of things | `_count` (an array of them `_counts`); `_index`, `id`, `x/y/z`, `yaw/angle_x/angle_z` | num, n |
| spelling | British: colour, centred | (the color.c unit prefix `color_` and the records.h member `item.color` stay) |

**Verbs**
- **`X_open(n)` / `X_show(n)`.** n != 0 opens. `X_open(0)` is called first by the screen's
  per-frame function: it opens on the hotkey, if there is one, and returns whether the screen is
  open. `status_show(0)` is the same for the STATUS key.
- **`X_update` / `X_frame`.** The screen's per-frame function. Both are in use; prefer `_update`
  for new names. A function that returns an animation frame is `*_anim_frame`.
- **`_tick`.** A game-time step: per minute or per round, or a spawner run each frame.
- **`_poll`.** Input only.
- **`_draw`.** Draws to the screen. **`_render`** composes into an image or the 3D view.
- **`_close`.** Leaves the screen (usually the EXIT handler). **`_run`, `_screen`, `_loop`** are
  modal loops that return when done.
- **`find` / `get` / `count`.** `find` returns a match, `get` takes a key or index, `count`
  counts.
- **`make` / `create` / `init` / `load` / `free`.** `make`/`create` allocate an object, `init`
  fills a new one, `load`/`save` deal with files, `free` releases.
- **Callbacks** end in `_cb`.
- **Button tables** are `<screen>_buttons`. Handlers are `<screen>_button_<label>` (or the
  existing `<screen>_<label>_button`).
- **Quest opcode handlers** are `qaction_opNN_<what>` for actions and `qcond_opNN_<what>` for
  conditions and events, named after the event (`qcond_op21_foe_hurt`). Handlers shared by several
  opcodes drop the number (`qaction_place_foe`).
- **Debug menu handlers** are `kludge_*`.

**Shared storage**
- A global used by unrelated units with different meanings gets a neutral name: `shared_scratch`
  (the int at 0x195B84), `scratch_object` (0x195B50), or `scratch_<address>` for the slots of the
  screen scratch block 0x190BE4–0x190E1F.
- A slot that only one unit uses keeps that unit's name. Same meaning in two units is not
  sharing: `chargen_selected_attribute`, `court_prison_days` with %dip.
- A global with a dominant meaning and incidental reuse keeps the dominant name, and its evidence
  lists the reuses (`text_rsc_buffer`, `inpstr_saved_text`).
- `build/names/scratch/audit_cons/shared_globals.py` finds the candidates: globals used by three or more units.

**Fields**
- A field row's name is the records.h member name at that offset.
- The prefix names the record kind. It is the struct tag, with four exceptions:
  - `object+` is `struct record`'s header (fallcall reads the prefix);
  - `class+` is `struct career`;
  - `monster+` is a type-18 creature's record, offsets from the record start. It is used only for
    the creature meanings of character union offsets and for `struct monster_anim`; plain
    character members go under `character+`;
  - `door+` holds the door meanings of header union offsets.
- One name per prefix and address. A union's other members live in records.h only.
- A row may name an element or byte inside a member when the game reads it on its own
  (`right_hand`, `transport_flags`, `equip_effect_flags`, `skill_uses`). records.h mentions it in
  the member's comment.
- Element structs are named in records.h only: character_skill, enchantment,
  spell_effect/range/magnitude, picklist_rect/entry, block_model/flat/door, qbn_text_var,
  move_request, mem_pool, loaded_location.

**Renames**
- A rename keeps the old confidence unless there is new evidence. The evidence ends with "was
  <old name>".
- Rename when the old name misleads: it names one of several uses, it clashes with another name,
  or it is one letter from another function. Do not rename for style alone.
