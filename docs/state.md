# Game state map

Where FALL.EXE keeps its state in memory, found by experiment in the headless game
(`tools/fallstate.py`, `tools/fallplay.py`), with the evidence for each finding and how far
it can be trusted.

- **Addresses** are the game's own, as in the decomp's `D_xxxxxxxx` names. In the emulator,
  add `fallemu.LOAD`.
- **Pointers** stored in memory already include LOAD.
- **Offsets** are from the start of a record.
- **Confidence:**
  - *confirmed*: changing the value changes the game the way the name says, or the value
    matches every save checked;
  - *strong*: it tracks the change in every experiment;
  - *candidate*: it fits, but is unproven.

## Globals

| address | what | confidence | evidence | uses in the decomp |
|---|---|---|---|---|
| `D_00195AA4` | pointer to the player object | confirmed | moving the object's x/z (+7/+15) teleports the player (`fallplay tp`) | 790 in 104 files |
| `D_00195AA0` | pointer to the player's creature entity: the character record is at +0x47, and its parent (+0x43) is the world object | confirmed | in 2 saves; the disease code tests `entity + 71 == player character` | |
| `D_00195BE0` | pointer to the player character record (below), which is the entity + 0x47. It is a separate allocation from the world object: their distance differs between saves (0x197, 0x1CF) | confirmed | it points at the name; the stats, gold and so on follow it in every save | 1228 in 118 files |
| `D_001789FA` (byte) | where the player is: 1 outside, 2 inside a building, 3 in a dungeon (palaces too) | confirmed | constant in 4 outside, 3 building and 9 dungeon/palace states | 182 in 50 files |
| `D_00196274` (byte) | the game mode, i.e. which screen has the input: 0 world, 3 character sheet, 4 inventory, 5 spellbook, 7 options, 14 logbook, 16 rest, 19 travel map | strong | the same value from 3 saves for each screen, and different between screens; an earlier hand-matched file already calls it `MODE` | 219 in 59 files |
| `D_00196276` (byte) | interaction mode: 0 grab (F2), 1 info (F3), 2 steal (F1), 3 talk (F4) | strong | the same value from 3 saves for each key | 26 in 5 files |
| `D_00195EB0` | key map: action → scan code (action 27 travel map = W, 12 rest = R, 24 logbook = L, 26 automap = M, ...) | strong | read by the travel play agent, and it matches Options > CONTROLS | 27 in 4 files |
| `D_001959AC` (dword) | the frame counter | confirmed | main()'s loop does `D_001959AC++` once per iteration (src/hand/func_00010010.c), about 119 per 400 timer ticks. It isn't a clock: moving it didn't change the sky | 41 in 15 files |
| `D_00195BF4` (dword) | game time in minutes: the day is `/1440`, trips end at 06:00 or 18:00 (360 / 1080); save_blades reads day 671, 22:10 | strong | travel and sheet code (naming agent); `fallplay` shows it | |
| `D_00195BEC` | pointer to the player's class record (= character + 0x230): class name at +0x1C ("Wizard"), the 12 skill ids at +0x10, HP per level at +0x34, the advancement multiplier at +0x36, and forbidden-equipment masks at +0x0B/+0x0E. Its immunity bits are tested by the disease code | strong | naming agents (inventory, sheet) | |
| `D_0018DBFC` | where the frame is: main()'s loop sets 0–3 around its calls, and the per-frame update `func_0001025B` sets 98–107 between steps | strong | the code | |

`D_00196270`–`D_0019627E` are a run of state bytes that the decomp uses heavily (mode
`…74`; `…79` 160 uses, `…72` 121, `…71` 76, `…77` 60), probably one game-state struct. The
mode and interaction-mode bytes sit in it.

## Player character record (`*D_00195BE0`)

The record is packed: fields sit at odd offsets.

| offset | what | confidence | evidence |
|---|---|---|---|
| +0x00 | name, 32 bytes | confirmed | "Dafydd gen orbo" (save_blades) |
| +0x20 | attributes, 8 × u16: STR INT WIL AGI END PER SPD LUC | confirmed | 100 100 100 98 100 92 100 97, as on the sheet |
| +0x30 | base attributes (8 × u16); the sheet shows an attribute in red when it is below its base | strong | sheet code |
| +0x43 (byte) | race, or current form: 8 vampire, 9 werewolf. Above 8 blocks the inventory | confirmed | writing 2 shows "Nord"; F6 in save_kralwolf refuses |
| +0x58 | the skill sum at the start: level = (skill sum − this + 28) / 15 | strong | the formula matches Daggerfall Unity's |
| +0x5C (i32) | max health base: level-up adds the HP gain here and to +0x7E | strong | sheet and level-up code |
| +0x7C (u16) | current health | confirmed | writing 123 made the sheet show 123/489 |
| +0x7E (u16) | max health | strong | drawn after the slash; 356 (blades), 489 (keophex) |
| +0x9B (u16) | fatigue × 64. The sheet shows +0x9B >> 6 out of STR + END, which is not stored | confirmed | writing 77×64 made it 77/187; writing STR 50 made the max 150 |
| +0x9D | skills, 6 bytes each (Mercantile at +0xF1, Pickpocket at +0xF7) | strong | trade code (inventory agent) |
| +0x16F | equipped items, 27 slots (right hand 19, left 21) | confirmed | inventory experiments |
| +0x81 (byte) | level | confirmed | 21, 13, 20 in three saves, as on their sheets |
| +0x85 (i32) | gold coins carried | confirmed | +5000 per gold cheat (Ctrl+F9) in two saves. Writing 12345 shows gold 112345 and drops encumbrance by exactly the coins' weight (400 to the kg). The sheet's GOLD adds something else, 100000 in blades and 90400 in morthag1, probably letters of credit |
| +0x8D, +0x8F (u16) | magicka, current and max | strong | 28 / 160 in morthag1, as on its sheet |

An earlier draft named +0x7C max health. That was wrong: the saves checked were all at full
health, and a write test settled it.

## Objects, saves and the heap

These come from the objects and data naming agent; details are in build/names/objects_data.md.

**The 71-byte header** is Daggerfall Unity's RecordRoot.
- +0x1D: image2. For a 3D object the model id is image2 × 100 + image.
- +0x27: the parent id, used only in the file.
- +0x3B: the previous sibling. Daggerfall Unity calls it ChildObject, which is wrong.
- Record types use the classic names: 1 World, 4 Move (the player object), 5 Eye (the
  camera), 23 Options, 24 Logbook, 39 NonWorld.

**Two trees.** The location's objects, and `nonworld_root` (0x1959A8, type 39, id 700),
which holds quest objects and tavern rooms. `object_find_by_id` searches the location first.

**Heaps:**
- Objects live in a pool of first-fit blocks with 18-byte headers (magic 'iiii'), checked by
  `mem_check_heap`.
- ARCH3D models have their own pool, cached in a 512-node binary tree.
- Bug: `object_heap_free` drifts by one byte on each odd-size allocation.

**Save files:**
- SAVETREE.DAT, version 294: position, location id, environment, building records,
  location records, nonworld records, links.
- SAVEVARS.DAT is mapped field by field to Daggerfall Unity's SaveVars. Daggerfall Unity
  reads the ship price one byte early: it is a u32 at 0x1751.
- `save_game(5, ...)` wrote all of SAVE5 when called directly.

**Other:**
- links.c runs a dungeon's action links: RDB action records of 39 bytes, with the same
  action and trigger types as Daggerfall Unity.
- profile.c is HMI's INI library, and sosez.c is the HMI SOS sound wrapper.
- args.c reads the Z.CFG keys into 22 `cfg_*` globals.

## The library region

The library region (0x9DA1C–0xBB27F, 1018 functions) holds several libraries:
- **Watcom C32 10.0a's runtime** (clib3r, math387r, emu387). tools/libmatch.py names 233 of
  them: 218 byte-identical once relocations are masked, 14 static functions inside matched
  modules, and `_cstart` as a near match.
- **StratosWare MemCheck** (0xA8D26–0xA93EA and around). It replaces memcpy, memset,
  memmove, strncpy, malloc, free and sprintf with checked versions that take the caller's
  `__FILE__` and `__LINE__`. Those calls are where the original source file names and line
  numbers come from (config/units.csv).
- **HMI SOS:** the MIDI song player (0x9E18C–0xA0AD9), the digital driver loader
  (0xA45AF–0xA89E8, HMIMDRV.386) and hardware detection (around 0xADE87, hmidet.386).
- **Rational's DOS/4G interface** (0xB3A76–0xB6C36), and an exception-dump handler
  (0xA1A16–0xA2A2B, "XXDEF.C" with register dumps).

None of the non-Watcom libraries are available to match against; naming them would need
their APIs (HMI SOS headers, the MemCheck API).

## Records in general

Every game record (item, character, spell, loot pile, container, effect) starts with a
71-byte header, worked out by the inventory naming agent:
- +0x00: type (2 item, 3 character, 9 spell, 11 effect, 33 loot pile, 52 container, 54 item
  in repair);
- +0x15: flags (0x20: unpaid shop goods);
- +0x37: next sibling; +0x3F: first child; +0x43: parent;
- +0x47: the record's own data.

The player is a tree: the world object (`D_00195AA4`) → the entity (`D_00195AA0`, type 3,
whose data at +0x47 is the character record) → 5 item containers → items.

## Factions and guilds

These come from the dialogue and guilds naming agent; details are in build/names/talk_guilds.md.

**Faction record** (92 bytes): the tree is at 0x19672C and the count at 0x196710.

| offset | field |
|---|---|
| +0x00 | type |
| +0x01 | region |
| +0x03 | name |
| +0x1D | reputation |
| +0x1F | power |
| +0x21 | faction id |
| +0x33 | face |
| +0x36 | social group |
| +0x38 | 3 allies |
| +0x44 | 3 enemies |
| +0x50 / +0x54 / +0x58 | next / child / parent |

**Guild membership** is a type-10 record holding rank, guild kind, faction and the time of
the last rank change.
- Guild kinds: 0 Dark Brotherhood, 1 Mages, 2 Fighters, 3 Thieves, 64 + k knightly orders,
  128 + k temples.
- Rank r needs skills of 22 + 8r and 4 + 4r in the guild's skills, and a rank changes at
  most every 28 days.

**Character fields used here:**
- +0x91: reputation with the 5 social groups;
- +0x211 to +0x222: Thieves Guild and Dark Brotherhood invitation times and counts.

## Magic

These come from the magic naming agent; details are in build/names/magic.md.

**Spell record** (89 bytes, the SPELLS.STD format): effects, element, target, gold costs,
the duration/chance/magnitude settings, name, icon and id. Bytes +0x4A to +0x58 are filled
in at cast time.
- An active spell is a type-9 object, a child of its target, with its caster at header +0x2F.
- Casting spell 82 (Orc Strength) through 0x5A91A raised STR from 80 to 100. `spell_end`
  (0x8A4E4) put it back.

**Effect handlers** are a 64-entry table at 0xCAF40 (51–63 are the debug menu's handlers), in the XnGine object, reached through
the thunk at 0xCAE0E.
- Entries 0–50 are the spell effects, named from FALL.EXE's own effect-name table at
  0x182686.
- 25 hand-built spells sent through `spell_apply_effect` (0x5B9DF) each did what the name
  says.

**Magicka cost** = max(5, target factor / 2 × the sum of the cost formulas × (110 − school
skill) / 100). The formulas are a 7-entry table at 0xCAF24. `spell_cost` (0x3A0C0) returned
exactly the costs the spellbook shows.

**Character fields used by magic:**
- +0x89: 32 active-effect flags, one bit per effect plus four resistance bits;
- +0x219: shield points; +0x21E: the lock and open chance; +0x22B: resistance chances.

**Crafting:** direct calls to 0x36F69, 0x55E35 and 0x8F246 open the spellmaker, the item
enchanter and the potion maker. A potion recipe is 109 bytes.

## Creatures and combat

These come from the combat naming agent; details are in build/names/combat.md.

**Monster table** `D_0018487A`: 29 bytes per creature type. It holds the HP bonus, armour,
loot table, the weapon material needed to hit, flags (+4), three damage ranges and the level.
Every value checked equals Daggerfall Unity's EnemyBasics: Rat 1d8+8 HP and 1–4 damage, Frost
Daedra 50–100. Other tables match Daggerfall Unity too:
- weights, monster categories, corpse textures and language skills;
- the 13 monster spell lists and the class spell lists;
- the Wabbajack list and the disease lists.
Daggerfall Unity's "FALL.EXE offset" comments convert to our addresses as VA = file offset −
0x39600.

**Creature object:** a 0x47-byte header, then a character record. A few character offsets
mean something else for creatures (+0x58, +0x6C, +0x74, +0x1FD, +0x21D), so those are named
`monster+`.
- +0x23E: ASCR animation record number; +0x23F: the action.
- +0x241: the mobile id (Daggerfall Unity's MobileTypes).
- +0x2C1: the animation struct. It holds the frame index, a request byte at +0x2D5 (255 =
  none), the state at +0x2D8, and event bits (bit 0 strike, bit 1 missile).

**AI states:** 0 move, 8 melee, 16 hurt, 24 bow, 32 spell, 48 idle, 56/57/59 Daedra Seducer.
- Detection and the 200-frame give-up timer match Daggerfall Unity.
- A creature attacks when rand % SPD < SPD / 8 + 6.

**Damage** (`damage_resolve_attack`, `damage_roll_to_hit`) matches Daggerfall Unity's
CalculateAttackDamage and CalculateSuccessfulHit.
- Up to 5 monster attacks; weapon, material, race and swing modifiers; STR; backstab × 3.
- The to-hit chance is clamped to 3–97.
- Classic also deals damage when a knocked-back creature hits a wall.

Direct calls confirmed:
- `damage_apply` (500 damage) killed the Frost Daedra;
- `monster_summon_near_player` (id 25) summons a Frost Daedra;
- lycanthropy infection and shapechanging back from wolf form.

## Banks, taverns, courts and character creation

These come from the economy and character-creation naming agent; details are in
build/names/economy_chargen.md. Most were confirmed by clicking through the screens in live
saves.

**Bank:**
- Accounts are a type-25 record (`bank_accounts`, `D_00195A04`): 62 regions × 13 bytes,
  holding the balance, loan owed, loan due date and a defaulted flag.
- A letter of credit costs amount / 100 + 1.
- Loans go up to level × 50000, at 10%, due in 360 days.
- Houses and ships sell back for 85% of the price. A ship costs 100000 or 200000 and needs
  a port town.

**Tavern:**
- A room costs 7 gold a day, haggled down by `trade_adjust_price`. Knights stay free, and
  Heart's Day gives a free day; the most you can book is 350 days.
- A meal heals 2 × its price, once per 240 minutes (character+0x205).
- The code that would keep rented rooms and their items between visits is dead.

**Court:**
- The crime reputation table and the fine table match Daggerfall Unity's.
- The fine is paid in 40-gold units, each a coin toss between 40 gold and 3 days of prison.
- Pleading guilty halves both. Not guilty leads to a DEBATE (Etiquette) or LIE (Streetwise)
  roll.

**Class record** (74 bytes, fully mapped). The class maker's advancement multiplier is
1.0 + advantage costs − disadvantage costs + (HP per level − 8) × 0.05.

**Character creation** runs province → race → gender → class (from the list, the 10
questions, or the class maker) → background (random, or 12 questions) → name, face,
attributes, skills, reflexes → the intro movie.

## World, maps and time

These come from the world naming agent; details are in build/names/world.md. Direct calls
from a morthag1 snapshot confirmed:
- `calendar_format_date` → "6th of Frostfall", and `calendar_update` gives year 406;
- `building_is_open` follows an open-hours table that equals Daggerfall Unity's;
- `region_update_prices` moves prices 2% a day, as Daggerfall Unity does;
- `location_reveal` writes MAPSAVE.SAV;
- `map_goto_location` moved the player to Charmarket, and into a dungeon;
- the encounter tables fill on entering a dungeon (table 19 is underwater).

**Map location record** (17 bytes):
- +0: bits 0–19 the map pixel, bits 20–31 the MAPPITEM/MAPDITEM index;
- +0x0C: the dungeon type; +0x0D: a mask of the town's services.

**Region record:** Daggerfall Unity's 80-byte RegionDataRecord, at 0x18F044:
- +0x48: precipitation override; +0x49: punishment flags; +0x4A: legal reputation;
- +0x4C: the persecuted temple; +0x4E: the price adjustment.

**FACTION.TXT** is parsed through a 19-entry {hash, handler} keyword table. That names the
faction fields ruler (+0x02), vam (+0x23), flats (+0x2F), race (+0x35) and ggroup (+0x37).

**Weather** is rolled daily for each climate from a [season][climate][7] chance table.

**Encounters:** Daggerfall Unity's 45 encounter tables.
- A content filter (settings bit 2) rerolls Nymph, Daedra Seducer and Lamia.
- Ghosts and Wraiths are rerolled by day.
- The swamp climate falls into a dungeon table, which looks like an original bug.

**Debug menu:** kludge.c has a debug pick list ("Get rumor" … "Enter Tavern"). Its handlers
are `spell_effect_handlers[51..63]`, so that table has 64 entries. Its last labels are off
by one: "Generate Songs" runs the tavern.

## Input, movement and the interface

These come from the UI and input naming agent; details are in build/names/ui_input.md.

**Key map:** `key_map` holds the 38 actions in the order of the CONTROLS screen (0 FORWARD
… 37 INVENTORY).
- `key_action_held` (0x42F0F) tests whether a key is down; `key_action_pressed` (0x430CD)
  fires only on the first frame.
- Bindings of 200 and up are joystick and mouse buttons and joystick axes.

**Steering:** the view is a 3 × 3 grid of steering regions, with handlers in a table at
0xCAF00 (forward-left, forward, forward-right, turn left, stop/activate, turn right, slide
left, back, slide right). The movement keys use the same handlers.

**Movement:** `player_movement_update` (0x81425) does the walking, climbing, jumping and
fall damage every frame. The on-ground flag, vertical velocity, crouch and look pitch were
confirmed by experiment.

**Settings:** `game_settings` (0x195BF8) points at the settings record. Its first word (always
read as one u16) holds full screen (bit 0), head bobbing (bit 1), the content filter (bit 2,
set by load_game when DAGGER.GRD exists) and the detail level (bits 8-15); the volumes follow.

**HUD:**
- 11 buttons.
- The portrait overlays low health, poison, disease and being under someone's spell.
- Two compasses: the HUD one, and a small strip in full screen.

**More game modes** (`game_mode` values): 26 the repair menu, 27 the witches' coven menu,
28 the service menu (banking or selling), chosen by building type.

## Message boxes and support code

These come from the text and support naming agent; details are in build/names/text_support.md.

**Message boxes:**
- `text_rsc_load` reads a TEXT.RSC record and picks one of its variants at random.
- `text_expand_wrap` expands the macros and wraps the lines; `msgbox_render` draws the box.
- `msgbox_show_rsc` (0x3F09F) and its string, quest-text and QRC siblings show a box and
  wait in `msgbox_wait`.
- `msgbox_kind` (`D_00196270`) says how the box closes: 1 a click, 2 text input, 4 a flag,
  5 buttons.

**support.c is a grab-bag:**
- screen messages;
- the mode stack (`mode_push` / `mode_pop`) and `rand_range`;
- the per-frame walk over world objects: creatures, pedestrians and the Detect target;
- building access for trespassing;
- gold: `gold_total` is the sheet's GOLD (coins plus letters of credit), and carrying
  capacity is STR × 1.5;
- keeping rented rooms, house and ship contents, and items in repair across location
  changes;
- Recall's position slots;
- a stubbed-out driver for a Logitech SWIFT 3D input device.

**File formats:**
- Books are `bok%05u.txt`: a 234-byte header, then the page count and page offsets.
- The logbook holds 32 quest ids, each with 10 message ids, times and places.
- The notebook is `notebook.tde`, in 3640-byte pages. It's all dead code: `note_update` has
  no callers and nothing points at it.
- NAMEGEN.DAT's banks are Daggerfall Unity's.

## Text macros and quests

These come from the text and quests naming agent; details are in build/names/text_quests.md.

**Text macros:** 255 entries of `{name[5]; handler}` (9 bytes each), indexed by first letter
(0x17C47E pointers, 0x17C4F2 counts); `parse_expand` (0x4633F) expands them.
- Every handler was called directly in save_dorian, and the results match the game: %ra
  "Dark Elf", %dat "Middas the 23th of Evening Star", %rn "Queen Akorithi".
- 18 handlers return "BLANK".
- Two bugs in the table: "%3hn" is spelt "1hn", and "%prg" appears twice.
- Some macros that TEXT.RSC uses have no handler (%hol, %2com, %nam).

**Quests:** an active quest is a type-14 object under `quest_root`. Its data at +0x47 is the
QBN file as loaded, with its pointers relocated.
- Section record sizes are in `qbn_record_sizes` (0x199788): 19, 94, 34, 20, 24, 16, 33,
  14, 87, 8.
- An opcode record is 87 bytes: five 15-byte arguments, then the message at +0x51.
- `qbn_opcode_arg_counts` (0x195DA4) is a digit string with each opcode's argument count. It
  matches UESP for every documented opcode.
- The actions dispatch from 0x29958. Events dispatch from 0x2B26B: 1 item given, 2 kill,
  3 item found, 5 dropped, 21 foe hurt, 28 NPC clicked, 71 pay gold, 73 spell cast,
  78 faction.
- `quest_pick_file` (0x4C274) picks a guild or NPC quest by its file-name letters.

## Source units that are mislabelled

config/units.csv takes each unit's range from its first and last MemCheck file reference.
The naming agents found these functions filed under the wrong unit:
- equip.c is item creation (items from templates, artifacts, books, paintings, loot, shop
  stock), not equipping.
- 0x615E0–0x6228A, filed under equip.c, is creature AI that belongs to monster.c.
- 0x91D20–0x922F6, filed under generate.c, are inventory callbacks.
- 0x28FAD–0x298F3, filed under automap.c, are quest condition and event handlers called
  from qcom.c.
- 0x191DA–0x1B5BE, filed under talk.c, are probably faction.c's faction tree and politics.
- 0x6D91A, filed under disk.c, belongs to guilds.c; 0x71606 and 0x717EC look like rest.c.
- spells.c starts at 0x36F69 (filed under fs2df.c).
- spfx.c starts at 0x88C0E; the effect handlers 0x88CF5–0x891E6 are filed under
  maplogic.c. spfx.c ends at 0x8B3EA; 0x8B43B and 0x8B48B are names.c.
- itemmakr.c starts at 0x55E35 (filed under custom.c).
- potions.c starts at 0x8EE48 (filed under object.c).
- 0x5A72F and 0x5A7D5, filed under book.c, are probably runspell.c.
- 0x2E914 belongs to damage.c (filed under qkey.c).
- 0x2FD75–0x304C8 (filed under damage.c) are the quest-state core, which belongs to
  qmisc.c.
- 0x4BA70–0x4BCA6 (filed under tamriel.c) belong to quests.c. 0x4CF37, 0x4CFA7 and 0x4D195
  (filed under quests.c) are armour code.
- 0x4A8DB–0x4AA47 (after parse.c's last file reference) are probably tamriel.c's calendar.
- 0x2D62F–0x2EC79 (filed under qkey.c) belong to damage.c.
- 0x641CD–0x64301 (filed under monster.c) belong to links.c.
- 0x74024 and 0x7425E (filed under weapons.c) belong to click.c; 0x79A28 (moninit.c)
  belongs to loadsave.c.
- 0x7242F (`fatigue_add`) belongs to rest.c; 0x728D2 (filed under rest.c) is weapons.c.
- faction.c starts at 0x191DA. camera.c is the screenshot code (PICS\SCR%d.BMP).
- Probably filed in the wrong unit, with no file string to settle it:
  - kludge.c's 0x45A1D and 0x45AED (item helpers used by the loot code) and 0x45E45
    (location door code);
  - sound.c's 0x69E3C (DPMI memory code);
  - rumor.c's 0x13E17 and 0x13F06 (region flags).
- 0x684E9–0x686DA (filed under disease.c) are trade.c's unused haggling window.
- 0x1EE84 and 0x1EF2E (filed under maploads.c) belong to tavern.c; 0x1FE74 (tavern.c)
  belongs to region.c.
- 0x20C55 (filed under song.c) belongs to crime.c; 0x68A1D and 0x68B1B (trade.c) belong to
  sound.c.
- 0x3FC4B–0x40F7F (filed under text.c) is people.c: pedestrians and guards.
- 0x5A442–0x5A6ED (after book.c's last file reference) are text-drawing, mouse-bounds and
  clip helpers, possibly their own file. 0x8C286 (names.c) belongs to inpstr.c.
- 0x13438–0x135E6 (filed under archive.c) are steal.c's lockpicking.
- 0x6A683 (jmem.c) is logbook.c's logbook_open; 0x9A993 (objcode.c) is travel.c's
  travel_map_open; 0x11016 (main.c) is probably sosez.c's sos_init.
- 0x7C908–0x7CAEB (filed under loadsave.c) are probably support.c's text-drawing helpers.
- 0x82DF6–0x83EDF (filed under args.c) draw world and automap objects, and 0x843E0–0x849E4
  are probably objlib.c's RMB record object makers.
- 0x76AF4–0x784EE (filed under click.c, past its last file reference) has no known unit. It
  holds ambient sounds, footsteps, the position history Alt+F11 uses, the creature
  spawn-point search, head bobbing, and the music choice.
- 0x998C8–0x99D0D (filed under color.c) is a string hash, door swinging and building
  lookups.
- 0x51B3E (question.c) is pflc.c's FLIC play loop; 0x8CA25 (inpstr.c) is picklist.c.
- 0x4259C (pickbook.c) is the STATUS key handler; 0x82657–0x82750 (intrface.c) are dead
  hex-string helpers.
- 0x2586B (filed under career.c) turns killed creatures into corpses: mplace.c or the
  monster code.

## Record structs

include/records.h defines the records as C structs: the 71-byte header (`struct record`) with
a union of the data that follows it (`r->data.character`, `r->data.item`, ...), and the
character (634 bytes, with the class record `career` inside it), monster, item, spell,
disease, potion recipe, faction, membership, bank account, map location, location, building,
NPC person, blessing, automap, RMB block lists, pick list and the quest/QBN records (the QBN
header is 60 bytes), and some structures that are not records: rumors, the loaded MAPS
location, links, memory blocks and pools, the model cache nodes and the collision move
request. Every size and offset is checked at compile time and by
`tools/offset_casts.py check`. docs/structs.md has the rules for converting code to them.

## Calling functions directly

`tools/fallcall.py` calls any function in a live game, from the safe point at the entry of
the per-frame update. It reports the return value, the functions that ran, the files opened,
the globals and player fields changed, and the screen if the function drew on it.
`fallcall.py sweep` does this for many functions, each from a fresh game. Two examples:
- Calling `func_00065937(entity, 0, 3, 1)` infected the player with disease 3: a new type-11
  record was created as a child of the player's entity. An earlier reading put an effects list
  at character+0x2CC, but that lies past the 634-byte character record: the changed pointer
  belonged to the next heap object.
- Calling `func_0002FA97()` played anim0012.vid, the death cutscene.

## Names in the source

`tools/apply_names.py` writes the confirmed and strong names into src/ and include/, so the
code reads `player_character`, `object_create_child` and `rand_range` instead of
`D_00195BE0`, `func_0008DCE3` and `func_0007D6AE`. Each name goes into config/symbols.txt,
which the matching build uses to check named functions and resolve named globals. The build
stays byte-identical: the names are checked by the same test as the code.
- Candidates stay as addresses.
- Library functions stay as addresses for now; their names would meet the C library's
  headers.
- A name the sources already use for something else is left out.
- Tools that read the sources by address go through `names.canonical()`.
- After a re-lift (promote_lifted.py), run apply_names.py again.

## How to add to this

`tools/fallstate.py` runs the experiments:
- `screens` and `modes` compare the same action from several saves;
- `values` finds numbers read off the screen;
- `gold` and `time` look for values that change in step.

A finding goes in only with its evidence. Once confirmed, it becomes a sensor:
`fallplay.state()` reads it at every step, so the play agents and the fuzzer can see it.
The naming pass uses this page and `tools/fallevidence.py` (which inputs make each function
run, its strings, callers and callees) to name globals and functions.
