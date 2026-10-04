# Asset tracing: from game data to the code that uses it

Investigation, 2026-10-04. The idea: every asset (a monster's animation record, a sprite archive,
a sound, a text record, a block) is read by specific code, so following it from the file,
through memory, to the code that reads it should name that code. **It works, and it is
cheap.** Every DOS read can be attributed to a clean guest call chain with about 15%
overhead. Watching the loaded bytes or a game object costs 1.25–3x. The chains, together
with community format tables (UESP, Daggerfall Unity), already name the loaders, the monster
spawn, animation, sound, damage and text paths, and settle four uncertain unit boundaries.

The experiment scripts and outputs were in `build/asset_trace/` (not committed; Stage 1 makes them a tool). Nothing in tools/, src/, docs/ or config/
was changed.

## 1. What was built for the experiments

| file | what |
|---|---|
| `atrace.py` | `FileTrace(emu)` wraps `Emu.int21`, so every open, read, seek and close is logged with: the file, the position, the guest buffer, the tick, and the **validated guest call chain**. It also has `funcs()`/`func_of()` (address to function and unit/XnGine module) and `vstack()` (the stack walk). |
| `bsa.py`, `assets.py` | Readers for the BSA directory (name and number records), DAGGER.SND, and the TEXT.RSC index. They name the record any file position falls in, and give a TEXT.RSC record's text. |
| `exp1_mord.py` | Reads during a short walk in save_mord. |
| `exp2_load.py SAVE` | Loads any of the 18 classic saves from the load screen (`lt_load.snap`), with the save's files put into this run's overlay as SAVE0. Every read is traced. This gives a full-load trace for every save. |
| `io_report.py` | Summarises an I/O log: file kind, then reads, bytes and chain. |
| `exp3_watch.py`, `exp4_life.py` | Read and write hooks on the buffers that file reads filled. Measures the cost, and follows a buffer's life from its read until it is reused. |
| `exp5_monsters.py` | Lists every monster object, from a hook on func_00078B8A during the load. |
| `exp6_fields.py` | Struct access map: for each function, the offsets it reads and writes in watched objects. |
| `exp7_poison.py`, `exp9_poisonfight.py` | Change a monster's id in the save or in memory, then compare which assets load and which code runs. |
| `exp8_fight.py`, `exp10_health.py` | Teleport next to the monsters in save_mord, then watch who reads the id and who writes health. |
| `census.py`, `census_report.py`, `text_report.py` | Replay every step of recorded play sessions through `fallplay.replay_step`, with every read attributed. This is done by wrapping `Emu.load`, so no existing tool needs editing. |

### The stack walk (the key piece)

Game code is `-of+`, so it has EBP frames, but the int 21h itself happens in library code or
XnGine asm, which have no frames. A plain scan of the stack for return addresses picks up stale
values, for example a phantom `0012DBCC[xn_12DB00]` between loadsave.c functions.
`vstack()` does this instead:

- It scans the stack for values that follow a CALL.
- It keeps one only if its call target is the function of the frame inside it, starting from
  the function at EIP. A call through a pointer, or a jmp thunk, is accepted on trust.
- This produced clean chains everywhere, for example
  `func_0006CEFC[disk] <- func_0007A983[loadsave] <- func_0007B222[loadsave] <- func_0003A3CE[intro] <- func_00010010[main]`.

### Determinism holds under the hooks

Adding read or write memory hooks does not change the run: `exp_det.py` compared 2985 reads,
and their buffers and ticks were identical. So a pass that only finds buffer addresses can be
followed by a hooked pass that watches them.

## 2. What was proved

### Q1: file level. Every read is attributed to its code

```
.venv/bin/python build/asset_trace/exp2_load.py mord 2500     # 23 s; 3911 I/O events
.venv/bin/python build/asset_trace/io_report.py build/asset_trace/load_mord_io.json 5
```

Excerpt from the full save load of mord (innermost game C first):

```
MONSTER.BSA   8 reads  00013260[archive] <- 00078B8A[moninit] <- 0008E3F7[object] <- 00078C79[moninit] <- 0007A983[loadsave]
TEXTURE.nnn   3 reads  00135EAB[xn_135D00] <- 00135DE4[xn_135D00] <- 000830C7[args] <- 0008E4A8[object] <- 0007E815[support]   [TEXTURE.280,486,489: monster sprites]
TEXTURE.nnn   6 reads  00135EAB[xn_135D00] <- 00135D00[xn_135D00] <- 00036233[fs2df] <- 00035D6D[fs2df] <- 00086794[maplogic]  [dungeon textures]
BLOCKS.BSA   20 reads  00013260[archive] <- 00035D6D[fs2df] <- 00086794[maplogic] <- 000876AD[maplogic] <- 0007A983[loadsave]
DAGGER.SND    1 read   00013260[archive] <- 00085A51[objlib] <- 00069938[sound] <- 000633BC[monster] <- 0006310D[monster]
SAVETREE.DAT 2308 reads 00079A28[moninit] <- 00079BE4[loadsave] <- 0007A983[loadsave]
FACTION.TXT   1 read   0001B69D[faction] <- 0001CA8C[faction] <- 0007B9A1[loadsave]
WEAPON##.CIF  2 reads  0006CB53[disk] <- 00072916[weapons] <- 000728D2[rest] <- 00072AA0[weapons]
```

`bsa.py` names the records that were read. For example, MONSTER.BSA @20777 is
`ASCR0139.ANC`, and @7870 is `ASCR0025.ANC`.

The census replayed **293 recorded play steps** in about 6 minutes (3 processes, with fallplay's
`replay_step` running exactly). It found:

- 1294 reads;
- **165 game functions** and 8 XnGine functions in asset chains (depth 4);
- 30 TEXT.RSC records, each with the code that showed it.

| function | shows TEXT.RSC record |
|---|---|
| `func_0007193D` (rest.c) | 27 "You cannot loiter for more than 3 hours", 349 "You have finished loitering", 353 "You wake up" |
| `func_000717EC` | 354 "There are enemies nearby." |
| `func_00097C2E` (inven.c) | 267/268 shop descriptions |
| `func_0002118B` | court sentencing (8055 "You have been found guilty…") |

The loader APIs this exposed (the generic layer every asset goes through):

| API | what |
|---|---|
| archive.c `func_00012E04` | opens a BSA |
| archive.c `func_00012FCE` | finds a record by name or number ("File %s not found in %s") |
| archive.c `func_00013260` | reads a record |
| disk.c `func_0006CB53` | reads a whole file by name |
| disk.c `func_0006CEFC` | reads from the save directory |
| text.c `func_0003D412` | reads a TEXT.RSC record |
| `func_0003E8A8` / `func_0003F09F` | message by RSC id |
| parse.c `func_0004A6B5` | dialogue text by id |
| sound.c `func_00069938` | plays a 3D sound (id, object, volume) |
| sound.c `func_0006998C`, `func_00069A62` | other sound players |
| objlib.c `func_00085A51` | loads sound records |
| objlib.c `func_000852A6` | loads ARCH3D models |
| sosez.c `func_00011870` | loads MIDI |
| moninit.c `func_00078566` | spawns a monster or NPC: ASCR, CLASSnn.CFG, texture |
| XnGine xn_135D00 `func_00135D00`/`DE4`/`EAB`, xn_C0C00 `func_000C0DC4` | loads and decodes texture archives |

### Q2: memory level. Who reads the loaded bytes

Results of `exp4_life.py` (it replays the load with hooks on each buffer from its read until
the next read that overlaps it):

- **ASCRnnnn.ANC** records stay in memory as read. Only XnGine **xn_C0100** reads them:
  - `func_000C013B`: start an animation state (a word table at record+6, indexed by the
    state at anim+0x14);
  - `func_000C019C`: advance by the timer `D_001343C0`;
  - `func_000C01EB`: the opcode loop, which dispatches through `D_000C0000`.

  So **xn_C0100 is the monster animation-script interpreter.** No game C reads the record.
- **TEXTURE.280** (the Frost Daedra's sprites) is read whole into a buffer:
  - xn_135D00 decodes it (`func_0013641D` 72k reads, `func_00136282` 315k reads) when
    `func_000830C7` asks;
  - later the renderer reads it again (`func_00154E20` xn_154D00 <- xn_12A100).

  So the buffer is a long-lived cache.
- **Pitfall:** buffers are freed and reused. Watching a buffer *after* the load only showed its
  reuse by other code (exp3), so the watch window must start at the read.

Costs (save_mord, patched Unicorn):

| what | cost |
|---|---|
| no hooks | 600 ticks in 5.5 s, 300 ticks in 2.6 s |
| FileTrace with chains (full load) | 18.7 s, up to 21–23 s: **+15%** |
| read hooks on 8 small buffers (3 KB), Python callback | **+25%** |
| one hook spanning 800 KB (Python filter) | 2x |
| **read hook over the whole heap** (64 MB, Python bisect filter) | **3x** (300 ticks: 7.9 s against 2.6 s) |
| R+W hooks on 9 buffers for a whole save load | 27.7 s against 21 s (1.3x) |
| R+W hooks over 3 monster objects × 1 KB | 1500 ticks in 15.9 s (about 1.2x) |

Full taint tracking at instruction level isn't needed. Assets are either kept in place (ANC,
textures, model records) or parsed by the code that read them, and that code shows up as the
first reader. A watch with C-side filtering would bring the 3x heap watch down to near 1x
(see Stage 4), but no experiment needed it.

### Q3: asset identity to game entity, and poisoning

Monster objects (`exp5_monsters.py`):

- They are type 18 (byte +0); func_00078B8A returns early for any other type. There are 8 in
  mord, 3 within 10 m of the start.
- The layout found:
  - **+71**: a character record with the same layout as the player's (`*D_00195BE0`): the
    name "Frost Daedra" at +71, and current/max health at +71+0x7C/+0x7E (+195/+197:
    81/81);
  - **+574**: the u16 monster id (ASCR record number; equals Daggerfall Unity's
    `MobileTypes`: 25 FrostDaedra; 128+ humanoid classes, 146 Knight_CityWatch);
  - **+577**: a byte copy of the id, used for sound;
  - **+27**: the billboard texture, packed as archive<<7 | record (0x8C00 is TEXTURE.280);
  - **+705**: the animation state (+709 points to the ASCR record).
- `D_00190704[obj+146]` holds the ASCR record pointers, and `D_00195AC8` is the MONSTER.BSA
  archive handle (both from the source of func_00078B8A, with the asset trace saying what
  they hold).
- Per id, the monster's assets are:
  - `ASCR%04d.ANC` (MONSTER.BSA);
  - **sound = 10000 + 10·id (+0, +1 bark, +2 attack)** (func_000633BC's source);
  - TEXTURE (archive+record at +27, kept in the save);
  - `CLASS%02d.CFG` for humanoids (moninit func_00078566).
- The DAGGER.SND record id maps to its directory index, and the index to Daggerfall Unity's
  `SoundClips` names. Examples: 10251 is EnemyFrostDaedraBark, 203 ButtonClick, 11461 Halt2.
- Source: func_0006310D plays 11461 ("Halt") when `id == 146` (city watch): guards shout.

Poisoning the id in the save (`exp7_poison.py NEWID`) changes which assets load:

- ASCR0006 (spider) or ASCR0009 (werewolf) is read instead of ASCR0025;
- the bark sound changes to 10061 SpiderBark or 10091 WerewolfBark, instead of 10251;
- the texture stays TEXTURE.280, because +27 is stored in the save.

So the id drives two of the three asset paths. **Coverage did not change** (660 functions with
each id over 1500 ticks; 522 each in an 800-tick fight with the id poisoned in memory). Code
that depends on the id runs only when its event happens. For "which code branches on the id",
**watching the field works better**:

- `exp8_fight.py` teleports the player next to the Frost Daedra (they are 9 m away
  horizontally, 12.8 m apart vertically, and never meet otherwise).
  - A monster's spell cast reads the id: damage.c `func_0002F490` at 0x2F5A5, which tests
    `id >= 0x80` (humanoid), via runspell `func_0005ABE6` <- monster.c `func_00062D57`.
  - The player dies within about 1000 ticks.
- `exp10_health.py` found that **the player's current health is record +0x7C**:
  - it went 292 → 215 → 142 → 123, written by `func_0002E914` (filed under qkey.c) via
    maplogic `func_00089035`;
  - docs/state.md lists current health as not found yet.
- A static check backs this up: 14 functions read `[reg+0x1fa]` (the id within the record),
  and they compare it with 0x92 (146), 0x80, 0x8b and 0x2b.

### Q4: which asset classes give the most leverage, and where they are reached

| asset class | yield for naming | reached by |
|---|---|---|
| **TEXT.RSC** (messages, dialogue) | Highest. 163 static call sites pass a constant id to `func_0003F09F` (85 functions, 148 ids), and every id has its text. The dynamic trace found the API; a static scan applies it to all code, run or not. | every session (quest1, pilot2/3 most) |
| **DAGGER.SND** (sounds) | High. 68 functions pass constant ids (79 ids), named through Daggerfall Unity's SoundClips: ButtonClick, PageTurn, OpenBook, GoldPieces, EquipPlate, CastSpell1, … Monster sounds tie monster.c to the monster id. | every session |
| **MONSTER.BSA / CLASS cfg / monster TEXTUREs** | Monster spawn, animation (XnGine xn_C0100), sprite update, the monster object layout, and AI/combat through struct access maps. | save_mord (3 monsters, reached by teleport); combat2 (random spawn ASCR0139); magic1/16 (spawn of ASCR0144 + CLASS18 via `func_00040E9D`) |
| **SAVETREE / SAVEVARS / MAPSAVE** | The object records loaded in place (memory layout = file layout), so record formats on UESP name struct fields. | any full load (exp2) |
| **BLOCKS / MAPS / ARCH3D** | Moderate. The location/dungeon loaders (fs2df, maplogic, maploads, objlib). Model ids are many but generic. | every load and travel |
| **IMG/CIF/CEL** (screens) | Moderate. Each screen's setup function (sheet, rest, spellbook, talk, travel map, char creation). The evidence table's strings already give most of this. | every session |
| **Spells** | Through sounds (CastSpell1 from runspell `func_0005ABE6`), missile textures (TEXTURE.375/378/380 loaded in the fight) and damage. SPELLS.STD wasn't seen in the sessions replayed. | magic1, the mord fight |
| **QRC/QBN** (quests) | `S%07d.QRC` read via text.c `func_0003E7D7` <- parse.c `func_0004A748`. Small so far: quest1 reads only S0000999. | quest1 |

**The "pink room" is the Mantellan Crux.**

- Asset tracing the load of orcs (`exp2_load.py orcs`) shows it reads:
  - BLOCKS.BSA `S0000000.RDB`–`S0000006.RDB`: the Mantellan Crux's blocks, the endgame
    dungeon ([forum](https://forums.dfworkshop.net/viewtopic.php?t=5969));
  - MAPTABLE.031 (51 bytes).
- blades, orcs, worms, uking, sentinel and wayrest are the **six main-quest endings** (whoever
  is given the Mantella). Those saves were made inside the Crux.
- So they load correctly. docs/play.md's "something about loading those saves is off" is
  wrong. The Crux is a real dungeon, worth tracing for ending/quest code.
- Monsters in it (Orc Warlord 24, Orc Shaman 21) are far from the start.

### Q5: how this feeds naming

Evidence rows in the style of docs/state.md, for fields and globals:

| what | name | evidence |
|---|---|---|
| obj+574 | monster id | ASCR record and sound follow a poisoned id |
| obj+195 / rec+0x7C | current health | it falls under attack; written by func_0002E914 |
| obj+27 | billboard texture | 0x8C00>>7 = 280 = the sprite file read |
| `D_00190704` | ASCR table | moninit source + trace |
| `D_00195AC8` | MONSTER.BSA handle | moninit source + trace |

For functions, one line in the evidence table per function, for example:

- `func_0007193D`: "shows 'You wake up', 'You have finished loitering'; reads REST02I0.IMG"
- `func_000633BC`: "plays 10000+10·monster id (monster bark)"

The access maps add "writes monster +612..615, +722..726 (anim state)": that is the AI.

The unit boundaries listed as uncertain are settled by this evidence:

| function | filed under | belongs to | why |
|---|---|---|---|
| `func_000615E0`, `func_00061E9B` | equip.c | **monster.c** | top readers of monster objects; callers of monster.c |
| `func_000717EC` | guilds.c | **rest.c** | "There are enemies nearby", REST00I0.IMG |
| `func_0002E914` | qkey.c | **damage.c** | writes current health |
| `func_00040E9D` | text.c | probably **people.c** | spawns a person |

## 3. Staged plan

### Stage 1: `tools/fallassets.py`, attributed asset reads for every episode (recommended first)

**What to build**

- Promote `atrace.FileTrace` and `vstack` to `tools/fallassets.py`, and `bsa.py`/`assets.py`
  to an `assets` helper.
- Commands:
  - `collect` replays every fallevidence episode and logs each read. The episodes are play
    steps and fuzz corpus states, via the same `todo()`/`replay_step` (wrapping `Emu.load`,
    or a hook argument added to `replay_step`). Each read gets its file, record (BSA / SND /
    RSC / TEXTURE archive), bytes and validated chain. Output: `build/assets/reads.jsonl`.
  - `loads` runs `exp2_load.py` for all 18 saves (full-load traces).
  - `report` writes per asset class the loader API and consumer chains, and per function the
    assets it reads, directly or within 3 frames.

**Tools to extend**

- `fallevidence.py`:
  - asset tokens as episode features (`asset:MONSTER.BSA/ASCR0025`,
    `snd:EnemyFrostDaedraBark`, `rsc:354`) for the lift scores;
  - an `assets` column in functions.csv and units/*.md.
- `docs/state.md`: the confirmed fields (current health +0x7C; monster object +0/+27/+71/
  +574/+577/+705).

**Yield**

- Asset evidence for more than 165 functions now, and more as play and fuzzing grow.
- The 15–20 loader APIs named.
- Unit-boundary checks.
- The 18 full-load traces (each about 25 s) cover the load paths of every save, including the
  Crux.

**Effort**: half a day. CPU: the replays cost what `fallevidence collect` costs now, plus 15%.

### Stage 2: static id census at the asset API call sites (pairs with Stage 1)

**What to build**

- A small script, `tools/asset_ids.py`, needs no emulator. It scans `asm/nonmatchings` (or the C)
  for the constant first argument of each API Stage 1 found:
  - TEXT.RSC: `func_0003F09F`, `func_0007DDC9`, parse `func_0004A6B5`;
  - sound: `func_00069938`, `func_0006998C`, `func_00069A62`;
  - format strings such as `ASCR%04d.ANC` and `CLASS%02d.CFG`.
- It names each id: TEXT.RSC text from `assets.rsc_text`, sounds by DAGGER.SND index from
  Daggerfall Unity's SoundClips.

**Yield**

- About 150 functions get a line such as:
  - "shows 454 'You do not have enough gold.'"
  - "shows 5100/5102 (tavern room rental)"
  - "plays EquipPlate"
- This covers code that play never reached.

**Effort**: 2–3 hours.

### Stage 3: struct access maps for the entities assets lead to

**What to build**

- `tools/fallfields.py` generalises `exp6_fields.py`.
- Given objects found through assets, it hooks reads and writes on each object while episodes
  replay. Sources of objects:
  - monster objects, from the func_00078B8A / func_00078566 hooks;
  - the player object and record;
  - item records;
  - the RDB block objects.
- It writes `function → offsets read/written`, and for id or type fields the compare constants
  in the instructions after the read (`exp8` found `cmp eax,0x80` / `0x92`).
- It needs scenarios that make the events happen (Stage 5).

**Yield**

- Field names for the creature/character record: health, animation state, texture, id,
  position, AI state at +612..+615.
- Functions grouped by the fields they write: AI, animation, damage, spells.
- The id-branch census: which monster ids are special-cased, and where.

**Effort**: 1 day. Cost about 1.2–1.3x per replay.

### Stage 4: buffer life and parse maps, with an optional C-side watch

**What to build**

- Make `exp4_life.py` general. Watch every read buffer from its read until reuse, and log the
  first readers and the offsets they read.
- Add copy-following: hook the library memcpy/memmove entries, so a copy out of a watched
  buffer watches the destination too.
- For formats parsed into structs (ENEMYnnn.CFG, CLASSnn.CFG, the BSA record headers, SAVEVARS),
  join the file offsets (from UESP and DaggerfallConnect) to the struct offsets and globals the
  parser writes.
- If wide watches are wanted: add `uc_dagger_watch(lo, hi, bitmap, ring)` to
  `tools/unicorn/dagger-unicorn.patch`. A C-side bitmap check would log (pc, addr) into a ring
  buffer without a Python call per access, bringing a 3x heap watch down to about 1.1x.

**Yield**: global and struct names for parsed tables, for example "D_xxxx = the enemy CFG table".

**Effort**: 1–2 days; the Unicorn patch is half a day of that.

### Stage 5: scenarios that reach the high-yield assets

- **Fight**: save_mord with the player teleported next to the Frost Daedra (exp8). Monsters
  cast at once, and the player dies in about 1000 ticks. Add one cheat-mode variant and one
  where the player swings the weapon toward the monster, so monster damage paths run too.
- **Spawns**: resting in the mord dungeon, and the magic1/16 town spawn.
- **The Mantellan Crux**: the six ending saves, with full-load traces and play.
- **Poisoned saves**: a SAVETREE id edit (+ +27 texture) over longer fights, for differential
  coverage of id-specific code (werewolf/vampire disease, the guard "Halt").

**Effort**: 1 day of play-agent time plus CPU. Add them as fallplay sessions so fallevidence
picks them up.

### Stage 6: naming pass

- Turn Stage 1–3 output into candidates, each with its evidence (the docs/state.md
  confidence levels):
  - function names ("rest loop", "monster bark", "apply damage", "ASCR VM");
  - field names (obj+574 monster id, rec+0x7C current health);
  - globals (`D_00190704` ASCR pointers, `D_00195AC8` MONSTER.BSA handle).
- Feed the unit-boundary fixes back into config/units.csv. That is a separate change for the
  main agent.

## 4. Recommendation

**Start with Stage 1, with Stage 2 alongside it.**

- Stage 1 promotes code that already works (`atrace.py`, `census.py`) into a tool.
- It reuses fallevidence's episodes, costs about 15% more CPU, and attaches an "assets read"
  line to every function that reads data, through any of the 15–20 loader APIs it identifies.
- Stage 2 turns those APIs into static evidence for about 150 functions in a few hours, with
  no emulator.

Together they give the naming pass:

- text-, sound- and file-based labels for roughly 300 functions (165 dynamic + about 150 static, before overlap);
- four unit-boundary corrections;
- the monster object and character-record field map found here.

Stages 3–5 then go deep into monsters, combat and spells.

## 5. Facts to carry into docs (found here, with evidence)

| fact | evidence |
|---|---|
| Player character record **+0x7C = current health** (+0x7E max) | 292 → 215 → 142 → 123 under attack in exp10; written by func_0002E914 |
| Monster object: type 18 at +0; character record at +71 (name, health +195/+197); id u16 at +574 (Daggerfall Unity's MobileTypes); id byte at +577 (sound 10000+10·id); texture at +27 (archive<<7 \| record); ASCR animation state at +705 | exp5 and exp7, the C of func_00078B8A and func_000633BC |
| XnGine xn_C0100 is the ASCR animation-script interpreter; xn_135D00 is the texture archive loader/decoder; xn_C0C00 func_000C0DC4 is its file read | exp4 |
| The six "pink room" saves are the main-quest ending saves inside the **Mantellan Crux** (BLOCKS S0000000–S0000006). The load is not broken. | exp2 on orcs |
| `D_00195BE0` (record pointer) is not always player obj+0x1CF: in save_mord the record is at obj+0x4781. Read the pointer, don't assume the offset. | exp10 |
