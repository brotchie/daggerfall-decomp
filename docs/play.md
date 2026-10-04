# Playing the headless game

`tools/fallplay.py` drives FALL.EXE in the emulator a step at a time, so a model can play at
its own pace: the game only runs when told to. This page is the brief for an agent (or a
person) at the controls: the commands, where to start, and the UI facts learnt the hard
way. `fallplay.py -h` has the full command list.

## The loop

    .venv/bin/python tools/fallplay.py new S cheat_morthag1     # S: a session name
    .venv/bin/python tools/fallplay.py buildings S guild         # what's in town
    .venv/bin/python tools/fallplay.py enter S 83                # walk in
    .venv/bin/python tools/fallplay.py step S "10 press f4; 60 click 160,100"
    .venv/bin/python tools/fallplay.py rewind S 3                # undo back to step 3

Every step loads the session's last snapshot, plays, saves a new one, and prints the screen
(`build/play/S/NNN.png`, 2x with a ruler every 20 game pixels: halve what you read off it
to get game pixels), the player's position and heading, the nearest shops, and how many
functions ran for the first time. `cov S` reports the session's coverage by source unit.
Steps cost CPU, not real time: think as long as you like between them.

## Where to start

Snapshots in `build/emu/snap` (`new S NAME`): `save_*` are the 18 UESP saves loaded and in
the world, `cheat_*` the same with the game's cheat mode on (see the keys below).

| snapshot | where |
|---|---|
| cheat_tlalac_s, save_tlalac_s | Crossing: a small town, 6 taverns, 4 general stores, Mages Guild, 2 temples, bank |
| cheat_morthag1, save_morthag1 | a city: 15 taverns, Fighters Guild, Mages Guild, Knights of the Flame, temple of Mara, palace, library, bank, most shop types |
| cheat_dorian, save_dorian | a city: Mages Guild, temple, palace, library, pawn shop, bookseller, 5 weapon smiths |
| cheat_gash, save_gash | a city: Mages Guild, a second guild hall, temple, palace, library, gem stores |
| save_kralvamp, save_kralwolf | a large city, the character a vampire / a werewolf |
| save_wereboar | a city, the character a wereboar |
| cheat_shadow, save_shadow | a hamlet (3 taverns) |
| cheat_ming, save_ming | on a ship |
| save_blades, kral, keophex, mord, orcs, sentinel, uking, wayrest, worms | indoors: dungeons, palaces, buildings (no town loaded) |

`tools/scenarios/newgame.txt` (a fallemu scenario) goes from boot through character
creation into the first dungeon.

## Moving about

- `walk S M` / `back S M` walk that many metres (they stop when blocked).
- `face S DEG` / `turn S DEG` turn with the arrow keys. The 3D view follows only the
  game's own turning: a heading written to memory moves the compass but not the camera.
- `tp S EAST NORTH` jumps by metres. Run `face` afterwards so the view settles.
- `goto S N [SIDE [M]]` stands back from building N facing it, and `look4 S N` takes a
  picture from each side.
- `door S N` stands 1.5 m outside building N's door, facing it. `enter S N` also goes in.
  The doors come from the door faces of the buildings' 3D models (ARCH3D.BSA). In Crossing,
  `enter` gets into every shop, guild, temple, tavern and the bank, and about a third of
  the houses. The rest of the houses are locked.
- `where S` says whether you are outside, inside a building, or in a dungeon (palaces count
  as dungeons). `goto`, `door`, `enter` and `look4` need you outside: from indoors, leave
  by the door or `rewind`.
- Every step also prints a `state:` line read from the game's memory (docs/state.md):
  - which screen has the input (world, character sheet, inventory, spellbook, options,
    logbook, rest, travel map) and the F1–F4 interaction mode;
  - the character's name and level, gold coins, magicka, and max health.

  Trust it over guessing from the picture.

## UI facts

- **Hold keys.** A key press shorter than a frame is missed in the world: `press` holds 20
  ticks.
- **Interaction modes.** F1 steal, F2 grab, F3 info, F4 talk. Snapshots start in info
  mode, where clicking a door only describes the shop. Doors need grab mode (`enter`
  presses F2 for you), and people need talk mode.
- **Doors.** Click the screen centre (165,112) from within about 1.5 m. Farther away gives
  "You are too far away...". A shop shows a description first: click it away at (160,60),
  then wait about 200 ticks for the interior. Interior doors open the same way.
- **Cursor.** A mouse cursor left near a screen edge steers the view, so click the centre
  before walking.
- **Keys** (the game's key map table is `D_00195EB0`; Options > CONTROLS lists them):
  - Arrow keys walk and turn. W is *not* forward: it opens the travel map.
  - J jump, D crouch, P run, LShift/RShift slide, PgUp/PgDn float.
  - A readies the weapon. Right-button drags swing it (`mouse X,Y,2` moves).
  - Backspace opens the spellbook (cast), Q recasts, E aborts a spell, U uses a magic item.
  - Other screens: R rest, T transport (FOOT / HORSE / CART / SHIP), F5 character sheet,
    F6 inventory, M automap (exit (297,180); it works indoors), L logbook, I location,
    F10 large HUD, Esc options.
  - Esc closes most windows, but popups (affiliations, help) need a click.
- **Clicks.** A lone click at the very start of a step is sometimes lost. Move the mouse
  first: `5 mouse X,Y,0; 10 click X,Y`.
- **Character sheet (F5).**
  - Top: AFFILIATIONS (70,87); skills PRIMARY / MAJOR / MINOR / MISC at (80, 109 / 119 /
    129 / 139).
  - Bottom: INVENTORY (35,156), SPELLBOOK (105,156), LOG (35,170), HISTORY (105,170),
    EXIT (70,187).
  - Gold on the sheet counts letters of credit too; the coins are what weigh.
- **Inventory (F6).** Tabs across y=4. Buttons: WAGON (240,20), INFO (240,42), EQUIP
  (240,64), REMOVE (240,86), USE (240,109), GOLD (240,132), EXIT (240,189).
- **Spellbook.** The list is on the left, at x 25–140 and y 33 + 8k. Click a spell to see
  it; double-click to cast. DELETE (45,174), SORT (132,174), EXIT (260,174).
- **Options (Esc).** It takes 150–200 ticks to appear.
  - SAVE GAME (110,52), LOAD GAME (160,52), EXIT GAME (208,52).
  - Sound / music / detail sliders at y 65 / 73 / 81.
  - CONTROLS (122,108), CONTINUE (196,108).
- **Rest (R).**
  - FOR A WHILE (110,70) / UNTIL FULLY HEALED (160,70) / LOITER (210,70).
  - FOR A WHILE asks for hours: `5 press 8; 40 press enter`. It runs about 200 ticks an
    hour, with STOP at (160,80).
  - With monsters near, the game refuses: "There are enemies nearby."
- **Talking (F4, click a person).**
  - Topic tabs: LOCATION (35,30), PEOPLE (35,40), THINGS (35,50), WORK (20,60). The list
    is at x 10–100 and y 72–170, rows 7 px apart.
  - Double-click a topic, then OKAY (60,191) to ask. WORK + OKAY asks about jobs, and
    gives leads to quest givers.
  - COPY TO LOGBOOK (155,166), GOODBYE (151,187).
  - Guild officers (not every member) have JOIN GUILD / TALK / Get Quest / EXIT. Click
    them from about 2 m.
- **Logbook (L).** DIALOG NOTES (65,191), page arrows (187,191) and (215,191), EXIT
  (290,191).
- **Shopkeeper (F4, click).** REPAIR ITEM / TALK / SELL / EXIT at y = 55 / 64 / 73 / 87,
  x about 161. There's no BUY entry: to buy, click an item on a shelf.
- **Buying from a shelf.** Click a shelf item; the trade window opens. Then SELECT
  (240,64), the list item (295,66), BUY (240,140), and YES (128,108) or NO (192,108) on the
  price. EXIT is (240,189).
  - Only better shops have shelves. In Crossing, the quality-3 general stores (#10, #45)
    are bare rooms; quality 8 or more (#48, #51) has shelves. `buildings` shows quality.
- **Guild member (F4, click).** JOIN GUILD (greyed if you can't) / TALK (161,64) / Buy
  Spells (161,73) / EXIT.
  - The spell shop: the list is on the left; BUY (45,174), then YES (128,108); EXIT
    (260,174).
  - A Mages Guild member may first offer to recharge your magicka.
- **Cheat mode** (the cheat_* snapshots):
  - Ctrl+F9 gold
  - Ctrl+F1 reveals the maps
  - [ and ] cycle through quest locations
  - Alt+F11 goes back to the last position

## Start points to be wary of

Six of the "indoors" saves (blades, orcs, worms, uking, sentinel, wayrest) all start at the
same spot (x 1045 m, z 408390 m), in an odd pink-floored room with a green cube. save_mord
starts in a real dungeon corridor. Something about loading those saves is off; use them
for screens and the character, not for dungeons.

## For coverage

The point of playing is to run code that the saves alone never reach: magic, guilds,
shops, banks, quests, crime and guards, travel, dungeons, combat, resting and levelling,
character creation. Each step records the command that made it (`argv` in
`build/play/S/steps.json`). The emulator and every command are deterministic, so a
session replays exactly from its first snapshot, and a session that reaches somewhere
new is a scenario. `tools/fallcov.py report` totals the coverage of every run in
`build/cov`: play sessions, batch runs, and `tools/fallfuzz.py`, the random play that runs
alongside on spare cores.
