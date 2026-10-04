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
| `D_00195AA0` | pointer to the player's creature entity: the character record is at +0x47, and +0x40 points to the world object | confirmed | in 2 saves; the disease code tests `entity + 71 == player character` | |
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
| +0x30 | 8 × u16, a second set of attributes (base values before bonuses?) | candidate | 85 100 75 70 80 65 80 70 in save_blades |
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
| +0x2CC | head of a list of attached effect records | candidate | `func_00065937` (disease) links its new type-11 record here when called on the player |

An earlier draft named +0x7C max health. That was wrong: the saves checked were all at full
health, and a write test settled it.

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

## Calling functions directly

`tools/fallcall.py` calls any function in a live game, from the safe point at the entry of
the per-frame update. It reports the return value, the functions that ran, the files opened,
the globals and player fields changed, and the screen if the function drew on it.
`fallcall.py sweep` does this for many functions, each from a fresh game. Two examples:
- Calling `func_00065937(entity, 0, 3, 1)` infected the player with disease 3: a new effect
  record of type 11 was linked at character+0x2CC.
- Calling `func_0002FA97()` played anim0012.vid, the death cutscene.

## How to add to this

`tools/fallstate.py` runs the experiments:
- `screens` and `modes` compare the same action from several saves;
- `values` finds numbers read off the screen;
- `gold` and `time` look for values that change in step.

A finding goes in only with its evidence. Once confirmed, it becomes a sensor:
`fallplay.state()` reads it at every step, so the play agents and the fuzzer can see it.
The naming pass uses this page and `tools/fallevidence.py` (which inputs make each function
run, its strings, callers and callees) to name globals and functions.
