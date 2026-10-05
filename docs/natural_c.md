# Natural C

The lifter wrote every function's control flow as gotos (`if (c) goto L;`, `goto L;`, `L:;`),
every global as `extern char g[];` read through casts (`*(int *)g`), every local and parameter
by its frame slot (`l_1C`, `a1`) and every record access as an offset cast. Phase 3 turned
that into ordinary C. Every function still compiles with Watcom C32 10.0a to FALL.EXE's bytes:

- **tools/structure.py** rewrites the gotos as if / else / `&&` / `||` / while / do-while /
  for / `for (;;)` / break / continue, and ends switches where they end.
- **tools/type_globals.py** declares the globals with their types (`extern int g;`,
  `extern short g[];`) and drops the casts.
- **Local and parameter names**: six agents named every `l_XX` and `aN` by what it holds, and
  retyped the ones holding records or strings (below).
- **Record structs**: include/records.h, include/structs.h and tools/offset_casts.py
  (docs/structs.md).
- **tools/protos.py** makes each file's declarations of a function agree with its definition
  (below).

## Phase 3 in numbers

| | before phase 3 | now |
|---|---|---|
| `goto`s / labels | 11,969 / 10,611 | 71 / 34 |
| `l_XX` locals and `aN` parameters | 29,964 | 0 |
| record offset casts (`offset_casts.py count`) | ~7,900 | 2 |
| file-local struct definitions | 305 (struct pass start) | 50 |
| `extern char g[];` declarations (mostly strings and buffers now) | 4,861 | 1,992 |
| declarations that disagree with the definition | 959 | 309 |

## Structuring and typed globals

Measured on src/lifted (164 files) and src/hand (249 files) as of commit 2f19d5b, with
`tools/build-and-verify.sh` BUILD OK on the result:

| | before | after |
|---|---|---|
| `goto`s, src/lifted | 11,135 | 45 |
| labels, src/lifted | 9,975 | 29 |
| `goto`s, src/hand | 834 | 26 |
| labels, src/hand | 636 | 16 |
| `if` / `else` | 9,443 / 335 | 6,596 / 1,200 |
| `while` / `do` / `for` | 130 / 5 / 186 | 580 / 21 / 700 |
| `break` / `continue` | 321 / 50 | 803 / 179 |
| `extern char g[];` declarations | 4,861 | 1,987 |
| typed `extern` declarations | 2,096 | 4,970 |
| `*(T *)g` casts on untyped globals | 8,678 | 1,607 |

- structure.py changed 1,264 functions, and every one matched fully structured on its
  first compile: the search below never had to give anything back. It skips the 240
  src/hand functions already written as structured C; 11 of them have 20 gotos between them.
- type_globals.py typed 1,273 globals: 944 scalars, 258 arrays, and 71 read through several
  types. That is 2,874 declarations, consistent across the files. One global was put back in
  one file (`region_event_groups` in rumor.c). In 31 files, uses through another type keep
  their cast as `*(T *)&g`.
- Time with `-j 8` over the 413 files: structure.py 51 s (1.0 s a file, inven.c the longest
  at 4.3 s; 640 compiles); type_globals.py 52 s (1.8 s a file, support.c 10.4 s; 689 compiles).

What is left:

- **gotos (71)**:
  - jumps into code shared with another branch or case (colstuff.c, talk.c `case 3:` into
    `case 2:`, weapons.c, spfx.c);
  - exits from two loops at once (itemmakr.c);
  - a switch the lifter wrote as an if-chain (talk.c's quest topics);
  - a `?:` inside a condition (main.c);
  - the hand functions above.
- **globals still `char g[]` (1,320)**:
  - 913 used only by address (strings and buffers, for which `char[]` is right);
  - 250 arrays read at byte offsets that are not whole elements: arrays of records, for
    records.h;
  - 79 read through types of different sizes;
  - 38 record pointers read at offsets (`*(int *)(*(char **)g + 18)`), left for
    offset_casts.py;
  - 15 already in offset_casts.py's GLOBAL_TYPES;
  - 18 with more address uses than reads;
  - 4 unused.

## Applying the tools

Both tools edit files in place only with `--in-place`, and verify every file they change:
each function is compiled with Watcom 10.0a (tools/wcc10.py, many variants per DOSBox-X run)
and compared with FALL.EXE, relocations included (the same check as tools/build_fall.py).
They keep only what matches.

```sh
# from the lifter's goto form (src/lifted and src/hand as of 2f19d5b)
tools/structure.py run --in-place src/lifted/*.c src/hand/*.c -j 8 --report struct.csv
tools/type_globals.py run --in-place src/lifted/*.c src/hand/*.c --arrays --mixed -j 8 --report types.csv
tools/build-and-verify.sh
```

Order matters:

- structure.py reads the lifter's form: a statement per line, gotos and labels, switches,
  blocks of inner-block locals. A function written as structured C, its own output
  included, is reported "not parsed" and left alone. To re-run it after a change, start
  from the goto form in git.
- type_globals.py works on any C, and decides each global over all the files it is given,
  so give it the whole set.
- `STRUCT_TMP=dir` puts the compile scratch there.

Other commands:

- `structure.py show FILE.c [--func NAME]`: print the structured form, without compiling.
- `structure.py count FILE.c ...`: count gotos and labels.
- `structure.py selftest FILE.c ...`: check that the parser gives back every function as it
  was with nothing structured.
- `type_globals.py census FILES --arrays --mixed --list`: show each global's decision and
  the reason for the ones left alone.
- `STRUCT_SABOTAGE=while structure.py run ...`: write every while condition wrong. It tests
  the search: the bad whiles must come back as gotos, and the file must still match.

## How structure.py works

**Items and addresses.** A function body becomes a list of items:

- `stmt`;
- `cj` (`if (c) goto L;`) and `goto`;
- `label`;
- `case`;
- `switch` and `block`, each holding its own item list;
- `ret` (in a void function `return;` is a jump to the epilogue, the address after the last
  statement, and `if (c) return;` a conditional one);
- `mark` (a label nothing jumps to: the lifter's `__dagger_tbl` switch-table marks).

Every code-bearing item gets a sequence number, its address. A label's address is that of the
next code-bearing item. Two constructs ending at the same place therefore end at the same
address, as they do in the binary, where the lifter wrote one label. Every comparison uses
addresses (a body ends where its end label's address is), not label names. `selftest`
renders a function with nothing structured and checks that it is the original text.

**Conditions** are read with the inverse of Watcom's code shapes (next section), as an exact
chart parse over spans of the control items (`sE`/`sJT` in the code).

**Statements** come from templates tried at each item:

- a `cj` starts an if, or an if-else when the then-part ends in a jump past the else-part;
- a label that is jumped to from below starts a loop:
  - `for`, when the shape has the increment block;
  - else `while`, when a condition follows the label and the loop ends in `goto` back;
  - else `do-while`, when it ends in a conditional jump back;
  - else `for (;;)` or `while (1)`;
- a forward `goto` over increment statements starts a `for (;; i++)`.

In a loop body, jumps to the exit and to the continue point become `break` and `continue`; in
a switch, jumps to its end become `break`.

The parse is checked as a whole. A construct is dropped (`forbidden`) and the function parsed
again when:

- a label inside a condition or a loop header is still jumped to from elsewhere;
- code outside a construct jumps into it.

**Choices among equal code.**

- When several conditions fit, the parse takes the one covering the most jumps.
- A void function's tail does not become an if's body when an early `return` fits.
- An if's body ends at the first place where it can end without taking in a `case` label.

**Moves of the lifter's own scaffolding** are made first. Each is kept only where it leaves
fewer gotos.

- **`swend`.** The lifter runs a switch body to the end of the function, so the code after
  the switch sits after its `default:`. The switch now ends at the label its breaks jump to,
  or, when nothing jumps there, right after an empty `default:`. Everything from there on
  moves after the switch, and the empty `default:` goes.
- **`swlift`.** A block of inner-block locals opened in one case and running over the later
  ones opens before the switch instead.
- **`hoist`.** A block of inner-block locals opened in the middle of an if or a loop opens at
  the start of its case or container.
- **`shrink`.** The items after the last use of a block's locals move after the block.
- **`swlead`.** The lifter's jump before a switch's first case goes. It is the dispatch's own
  jump, which Watcom emits anyway.
- **`mark`.** The `__dagger_tbl` marks are dropped. In structured code Watcom puts the
  table where the original has it without them.

**Verification and the search.** Every construct and move has a key. In each file every
function is first compiled fully structured, in one compile, together with the original
for reference. A function that does not match then goes through a search, one DOSBox-X run
per step, with the other functions at their settled form:

1. Every variant with one key left out. The first that matches wins.
2. Otherwise, every key alone. The keys that match alone are added one at a time while the
   function still matches.
3. A function with nothing found keeps its lifted text.

A last compile of the whole file must match every function that matched before; anything
that does not is put back.

## Watcom C32 10.0a `-od`: the shapes of control flow

Watcom at `-od` emits each statement's jumps in a fixed way and never threads a jump to a
jump. One peephole folds a conditional jump over an unconditional one. This is the model
structure.py inverts. Each shape was measured on small test functions
(build/p3_work/exp/t1.c to t3.c) and confirmed on the 1,264 functions.

Notation:

- `jcc(c, L)` is a jump to L when c holds; the lifter writes it `if (c) goto L;`.
- `E(c, F)` is the code for "fall through when c is true, jump to F when false" (what an
  `if` needs).
- `J(c, T)` is "jump to T when c is true, fall through when false".

Conditions:

- A comparison: `E(a < b, F)` = `jcc(a >= b, F)`, and `J(a < b, T)` = `jcc(a < b, T)`.
  Flipping the comparison costs nothing; the lifter's conditions are all one comparison.
- `E(A || B, F)` = `J(A, T); E(B, F); T:`. This is the natural form.
- `J(A && B, T)` = `E(A, X); J(B, T); X:`. This is the natural form too.
- `E(A && B, F)` = `J(A && B, T); jmp F; T:`. This is the **jump-over-jump stub**: an `if`
  on `&&` jumps to a stub `jmp F` when the left side is false, not to F. So
  `if (a) if (b) S` (straight to the end) and `if (a && b) S` (through the stub) are
  different code, and the gotos say which one the source had.
- `J(A || B, T)` = `E(A || B, X); jmp T; X:`, the mirror of the stub. It shows in
  `do ... while (a || b)` and in `(a || b) || c`.
- The tree shape shows: `(a || b) || c` and `a || (b || c)` differ, and so do
  `(a && b) || c` and `a && (b || c)`. Parentheses follow the tree exactly.
- `!` around a whole condition is free: `E(!x, F)` = `J(x, F)`, so `!(a && b)` and
  `!a || !b` are the same code; the parse prefers the second. A `!` around an `&&` or `||`
  nested inside another one makes a 0/1 temp: it is not a jump shape.
- An assignment inside a condition: `if (a && (p = f()) != 0 && p->x > 5)`. The lifter
  writes it as `p = f();` before the test, and that is the same code. The comma form
  `(p = f(), p != 0)` is not: Watcom evaluates it into a 0/1 temp. The tool takes in the
  assignment only after the condition's first test, or as a while's condition, and only
  when the variable is the left operand's one use.

Statements:

- `if (c) S` = `E(c, F); S; F:`.
- `if (c) S1 else S2` = `E(c, X); S1; jmp F; X: S2; F:`. After `return`, `break`,
  `continue` or `goto` the `jmp F` is dead and is not emitted: an if whose then-part ends in
  one of those reads as an if without else, and is written so. An `if (c) {}` emits only the
  compare.
- `while (c) S` = `T: E(c, E); S; C: jmp T; E:`. `continue` jumps to C, the `jmp T` at the
  bottom, not to T.
- `do S while (c)` = `T: S; C: J(c, T); E:`. `continue` jumps to C, the condition.
- `for (I; c; N) S` = `I; T: J(c, B); jmp E; C: N; jmp T; B: S; jmp C; E:`. The increment
  comes before the body; `continue` jumps to C. Without a condition the shape is
  `I; jmp B; C: N; B: S; jmp C`, and without an increment `C:` holds just `jmp T`. An
  initialiser before T is the statement before the loop; the tool takes it in when it sets
  the loop variable. Several increments are written `i++, p++`, the same code.
- `for (;;) S` and `while (1) S` = `T: S; jmp T; E:`, but `continue` goes to T in the first
  and to the `jmp T` in the second.
- **The one peephole**: `jcc(c, X); jmp L; X:` with nothing jumping to the `jmp`, becomes
  `jcc(!c, L)`. This is how `if (x) break;` as the last statement of a loop becomes the
  loop's own back jump: `for (;;) { S; if (x) break; }` is the code of
  `do S while (!x)`. And `while (c);` with an empty body is the code of `do {} while (c);`,
  so an empty-bodied loop on one comparison is written `while (c);`. The `&&` stub never
  folds, because its `jmp` is a jump target.
- A switch: the dispatch (a jump table or a compare tree) ends in a jump to `default:`, or
  to the end of the switch when there is no default. `break` is `jmp` to the end. A
  `default:` that is empty and last, and no `default:` at all, give the same code. A
  statement before the first case is emitted (as a jump) although nothing reaches it; the
  lifter used one to reproduce the dispatch's last jump, and the switch emits it without.
- Table placement:
  - A `__dagger_tbl` mark at the start of a `for` body puts the jump table after the
    mark's label, with a `jmp` over it. Without the mark the table lands where the
    original has it (quest_run_opcodes).
  - Opening a block of inner-block locals somewhere else can move its locals' frame slots.
    In 3 functions an earlier block opening broke the match (func_0001E928,
    func_00026081, link_step), which is why the block moves are made only where they
    remove gotos.

## How type_globals.py works

It scans each file's `extern char g[];` globals and classifies every use:

- `*(T *)g` (a read or write as T);
- `*(T *)(g + off)` (an element);
- `&g`;
- a cast of the address, `(int)g`;
- a bare use, which is an address;
- the address with an offset, `g + 4`.

It decides each global over all the files given, so that every file declares it the same way:

- **scalar**: one type T, no element or offset use. Declared `extern T g;`; `*(T *)g`
  becomes `g`, `(int)g` becomes `(int)&g`, a bare use `((char *)&g)`, and `(g)++` loses its
  parentheses. It is not typed when it has more address uses than reads (a buffer that is
  sometimes read as an int).
- **array** (`--arrays`): one element type T, every offset a multiple of `sizeof(T)`:
  `(i << 2)` or `(i * 4)` for an int, a constant, or anything for a byte. Declared
  `extern T g[];`, with `g[i]`. Indexes that contain other converted uses are rewritten
  inside out.
- **mixed** (`--mixed`): types of one size (`signed char` and `unsigned char`, `int` and
  `char *`). It is declared with the type that needs the fewest casts:
  - `(Y)*(X *)g` becomes `(Y)g` (or `g` when Y is the declared type);
  - a plain store, `++`/`--` or `+=`-style update between integer types becomes `g`;
  - any other read becomes `(X)g`.

  When that does not match, those uses are kept as `*(X *)&g` in that file: types steer
  Watcom's register allocation. `(char *)g + 10486` on an `int` global is not
  `*(char **)g + 10486`.

Left alone:

- the record globals in offset_casts.py's GLOBAL_TYPES, so as not to fight it;
- pointers read at offsets (`*(int *)(*(char **)g + 18)`), records for offset_casts.py and
  records.h;
- structs by value;
- globals read through types of different sizes.

Each file is compiled with all its typings. If a function fails:

1. For mixed globals, the `*(X *)&g` form is tried.
2. Then each global is left out in turn.
3. Then each is tried alone.

A global that breaks a function is put back in that file only.

Hand files declare some globals with types of their own (`extern unsigned char game_mode;`
in a hand file, `signed char` from the lifted uses). `census --list` shows these as "also
declared"; two translation units disagreeing on a byte's signedness changes no code.

## Local and parameter names

Six agents (by subsystem: magic and combat, economy and society, quests and text, objects,
UI, world) renamed every lifter placeholder with a function-scoped rename tool. The tool
refuses a name that is a global, a function, a typedef or already used in the function, and
never touches members, strings or comments. Each file was then checked with tools/wcc10.py.
Renames never change code; type changes can, so each retyping was compiled and kept only
when it matched:

- locals and parameters that hold records became `struct record *` and the like;
- strings became `char *`;
- the casts that made unnecessary were dropped.

Locals stay in their declared order (Watcom's frame layout follows it). Unused ones keep their
slots as `unused`, `unused2`... Two kinds of placeholder were kept on purpose:
- `*(int *)&short_local` reads, which the code depends on;
- pointers still held in an int because no struct exists for them yet.

## Declarations: tools/protos.py

Each file declares the functions it calls, as they were typed when lifted
(`extern int faction_find(short);`). Once the definition is typed, the old declarations
disagree. Every file compiles on its own, so nothing breaks, but the C says two things.

- `protos.py census` lists the declarations that disagree with their definition.
- `protos.py run --in-place -j 8` rewrites each one as the definition's prototype:
  - A pointer the declaration has and the definition lacks (where the definition still says
    `int`) is kept.
  - At call sites, `(int)` casts on single-name arguments at the positions that became
    pointers are dropped: `disk_read_file((int)D_00170794, 0)` becomes
    `disk_read_file(D_00170794, 0)`.
  - A file keeps a change only when Watcom 10.0a still compiles all its functions to
    FALL.EXE's bytes, with no more warnings than before. More warnings mean a caller passes
    another type.
- `protos.py unify --in-place` handles the functions still disagreeing. It searches the types
  their definition and declarations use, position by position, for one prototype under which
  every file matches, the definition's file included:
  - pointer-typed candidates are tried first;
  - a definition's pointer is never turned back into an integer.

On 2026-10-05: 959 disagreeing declarations; `run` fixed 627 and `unify` 10 functions, and
the last struct round fixed a few by hand; 309 are left. Those are load-bearing: the callers' code was compiled against other types than the
definition's. text_draw_coloured is the common case: its callers' declarations say
`unsigned char` for the colour, and with that prototype Watcom loads the constant through a
register before pushing it (`mov eax, 0x9c; push eax`). With the definition's `int` it would
push the constant directly (`push 0x9c`). The original source evidently had disagreeing
declarations too. `protos.py census` lists them.
