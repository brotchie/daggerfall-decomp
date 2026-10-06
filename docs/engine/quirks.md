# XnGine's quirks: the original bugs the canonical C keeps

Canonical C (docs/xngine_canonical.md) is equivalent to the asm at the engine's boundary, so
it keeps every behaviour of the asm that the game can see, bugs included. This catalogue
lists them: each with the asm that shows it, what it does, whether the game can see it, and
the C that keeps it. The C marks each one with a comment `Quirk Q-...`, and each function's
header comment names the quirks it has.

## Format

One entry per quirk:

```
### Q-<GROUP>-<nn>: one line saying what the asm does

- **Where:** the function(s) (name, address) and the canonical C file.
- **Asm:** the instructions that show it, with their addresses.
- **Behaviour:** what callers get, in terms of the game (inputs that trigger it, results).
- **Visible:** who can see it: the game (a boundary entry, its sites, the records or
  scenarios that show it), engine callers only, or no caller (dead code). Evidence.
- **Kept by:** the C that reproduces it (a function, an expression, a boundary adapter), and
  the test that checks it (a record, `tools/xn_equiv.py` spec, a scenario).
- **Status:** kept | kept at the boundary only | dropped (with why the game cannot see it).
```

`<GROUP>` is the module (SYS, MATH, MAT, VEC, DRAW...). Numbers are never reused. A quirk the
canonical C does not keep is still listed, with **Status: dropped** and the evidence that no
caller can observe it (the boundary map, coverage, a dead function).

Not quirks, and not listed here: the asm's private bookkeeping that canonical C drops
(scratch globals, patch fields, leftover registers no caller reads); those are in
`config/xngine_dropped.csv` with their reasons.

## System

### Q-SYS-01: a division that overflows gives 0

- **Where:** every `div`/`idiv` that can overflow or meet a divisor of 0; XnGine's
  divide-error handler `xn_sys_divide_error_handler` (149FC8), installed through DPMI 0203h
  by `xn_sys_install_divide_handler` (149F4C). Canonical: `src/engine/arith.c`
  (`xn_s64_div_or0` and its siblings).
- **Asm:** the handler reads the faulting instruction's ModRM byte, steps EIP over it (2, 3
  or 6 bytes) and returns with EAX = EDX = 0. The renderer relies on it about 240 times a
  tick (`xn_light_setup_poly`'s 2^32 / gradient, 0 for axis-aligned walls).
- **Behaviour:** the quotient and the remainder of an overflowing division are both 0.
  (The handler does not decode SIB forms: a faulting divide with a SIB byte would be stepped
  over by the wrong length. No XnGine divide has one.)
- **Visible:** to the game through every result computed from such a division: angles to far
  points (`xn_math_angle_to_point`), terrain heights of steep triangles
  (`xn_math_triangle_y_at`), the lighting.
- **Kept by:** the `_or0` helpers check whether `idiv`/`div` would fault and give 0 without
  raising the exception. The CPU exception itself is engine-internal (the boundary
  comparison leaves exceptions out; `config/xngine_dropped.csv` excuses them in the record
  tests). `tools/xn_equiv.py` (`xn_math_angle_to_point`, `xn_vec_normalize_shift`) compares
  them at scale.
- **Status:** kept.

### Q-SYS-02: the divide-error handler does not know SIB forms

- **Where:** `xn_sys_divide_error_handler` (149FC8, with its run-time blocks 149FD6..14A0CF);
  `src/engine/sys.c` (`divide_length`).
- **Asm:** 149FC8 reads the ModRM byte after the faulting opcode (`mov eax, [esp+0Ch]`;
  `cmp byte ptr [eax+1], 3Dh` ...) and compares it with 35h, 3Dh, 70h..7Fh and B0h..BFh one
  value at a time; 14A0C8 steps 2 bytes for anything else, 14A0D2 6, 14A0DD 3.
- **Behaviour:** a divide with a SIB byte (ModRM 34h, 3Ch, 74h, 7Ch, B4h, BCh) would be
  stepped over by the wrong length (2), into the middle of the instruction; the opcode is
  never checked to be a divide.
- **Visible:** no: no XnGine divide has a SIB form, and canonical C divides nowhere it can
  fault (Q-SYS-01). The game's own code still runs under the handler.
- **Kept by:** `divide_length`; the handler's 28 vector records (`test --boundary sys`, kind
  isr), group A's records `grpa_sys_div_fault_disp8` / `_disp32` (a divide by 0 through
  [ebp+8] and [disp32]: the 3- and 6-byte steps), and the scenarios, where the asm modules of
  later groups still fault into it.
- **Status:** kept.

### Q-SYS-03: restoring the critical error handler leaves it marked installed

- **Where:** `xn_sys_restore_crit_error_handler` (C0580); `src/engine/sys.c`.
- **Asm:** C0582 tests `xn_sys_int24_installed` and C058B..C05AE put the vector back and
  unlock the handler, but nothing clears the flag.
- **Behaviour:** a second restore sets the saved vector again and unlocks again; an install
  after a restore does nothing.
- **Visible:** no: shutdown_video calls it once, as the game exits.
- **Kept by:** `xn_sys_restore_crit_error_handler` (the flag left alone); its record.
- **Status:** kept.

## Math (src/engine/math.c)

### Q-MATH-01: the approximate distance halves without a sign

- **Where:** `xn_math_approx_dist2d` (0C7FD9).
- **Asm:** `0C7FDD neg eax` leaves 0x80000000 negative; `0C7FED shr ebx, 1` halves the
  smaller distance as an unsigned value.
- **Behaviour:** for a coordinate difference of exactly -2^31 the "magnitude" stays negative
  and its half is 0x40000000 instead of a negative number.
- **Visible:** in principle (46 game sites); only for differences of 2^31 world units, which
  the game's coordinates never reach.
- **Kept by:** `(s32)((u32)small >> 1)`; `tools/xn_equiv.py xn_math_approx_dist2d` includes the
  edges.
- **Status:** kept.

### Q-MATH-02: the integer square root of 0 depends on the caller's ECX

- **Where:** `xn_math_isqrt` (14BC00); its callers `xn_math_angle_to_point` (0C808D) and the
  game. Canonical: `xn_math_isqrt_zero`, `xn_math_isqrt_b` (math.c).
- **Asm:** `14BC04 bsr ecx, eax`: for EAX = 0, BSR leaves ECX as the caller had it; `14BC07
  dec ecx` then picks the first trial bit from it (`shl esi, cl`), and the search accepts
  every bit whose square's low dword is not above 0.
- **Behaviour:** isqrt(0) is 0 when the caller's ECX is 1..16 or ECX - 1 > 15; when ECX - 1
  is negative with its low 5 bits >= 16 it is a large value (0xFFFF4AFB for ECX = 0,
  0x7FFF4AFB for ECX = -1).
- **Visible:** to the game. Its site 080FFC (`intrface.c`: `sine = (x << 8) /
  isqrt(x*x + z*z)`) has ECX = x*x; for x = 0x10000 and z = 0 the sum wraps to 0 and ECX is
  0, so the game divides by 0xFFFF4AFB instead of faulting on 0. The other three game sites
  test for 0 first. Inside the engine, `xn_math_angle_to_point` reaches isqrt(0) with ECX =
  the low dword of dz << 28 (for coincident points, or sums that wrap to 0).
- **Kept by:** canonical `xn_math_isqrt(v)` returns 0 for 0; `xn_math_isqrt_zero(ecx)` is
  the asm's answer; the game's calls go through the boundary adapter `xn_math_isqrt_b`
  (the game's registers), and `xn_math_angle_to_point` passes it its own `dz << 28`.
  `tools/xn_equiv.py xn_math_isqrt@0` compares every ECX in +-2^20 and random ones.
- **Status:** kept (at the boundary through the adapter).

### Q-MATH-03: the exponential series is broken

- **Where:** `xn_math_exp_series` (0C8054), dead (no caller).
- **Asm:** the series' terms are computed into EBX and dropped (`0C808B pop ebx`); what
  reaches EAX is (v*v)^2 (`0C8078 imul eax, eax` on the earlier square); the two `idiv`s
  only matter for the divide errors they can raise.
- **Behaviour:** returns (v*v)^2 (32-bit products).
- **Visible:** no caller.
- **Kept by:** `return sq * sq` (the divisions, which change nothing but could fault, are
  gone: Q-SYS-01).
- **Status:** kept (the result); the divide errors dropped.

### Q-MATH-04: squares wrap at 32 bits

- **Where:** `xn_math_angle_to_point` (0C808D), `xn_math_isqrt` (14BC00).
- **Asm:** `0C809E imul ebx, ebx` and `0C80A1 imul eax` (whose high dword is dropped), `0C80A3
  add eax, ebx`: dx*dx + dz*dz in 32 bits; isqrt compares the trial squares signed (`14BC21
  imul eax; 14BC23 cmp eax, ecx; jg`).
- **Behaviour:** for points more than about 46,340 world units apart the squared distance
  wraps; isqrt of the wrapped (possibly negative) value is not a square root, and the angle
  comes out wrong (or the division overflows: Q-SYS-01).
- **Visible:** to the game (20 sites: facing and AI) for far points.
- **Kept by:** the same s32 arithmetic in C (Watcom's 32-bit multiply wraps);
  `tools/xn_equiv.py xn_math_angle_to_point` draws coordinates up to +-4*10^7.
- **Status:** kept.

### Q-MATH-05: the sine product does not mask its angle

- **Where:** `xn_math_mul_sin` (0CE6D4).
- **Asm:** `0CE6D4 imul dword ptr [edx*4 + xn_sin_table]` with EDX as the caller gave it.
- **Behaviour:** an angle outside 0..2047 reads past the table (the cosine table follows).
- **Visible:** one game site; it passes masked angles in the records seen.
- **Kept by:** `xn_sin_table[angle]` unmasked.
- **Status:** kept.

### Q-MATH-06: the plane height of a vertical plane is its normal's x

- **Where:** `xn_math_plane_y_at` (14BFE1), dead.
- **Asm:** `14BFE1 test edx, edx; je 14C020` returns with EAX unchanged: the normal's x.
- **Behaviour:** returns n.x instead of a height when n.y is 0.
- **Visible:** no caller.
- **Kept by:** `if (n->y == 0) return n->x;`.
- **Status:** kept.

## Matrices (src/engine/mat.c)

### Q-MAT-01: the scale matrix gets its own address

- **Where:** `xn_mat_set_scale` (1373FE), reached only through `xn_mat_set_scale_thunk`
  (1373F8), which nothing calls.
- **Asm:** `13741C mov edx, [xn_cam_scale_y]; 137422 shl edx, 0Eh; 137425 mov [eax+10h], eax`:
  it computes the y scale and stores EAX (the matrix's address) instead.
- **Behaviour:** m[1][1] = the address of m; m[2][2] stays 1.0.
- **Visible:** no caller.
- **Kept by:** `m->m[1][1] = (s32)m;`.
- **Status:** kept.

## DOS (src/engine/dos.c)

### Q-DOS-01: a DOS error is reported as a failed internal check

- **Where:** the 'DOS:' fatal tail L_0C0E8C, reached from `xn_dos_open` (C0C8C) and
  `xn_dos_create` (C0D54); `src/engine/dos.c` (`dos_fatal`).
- **Asm:** C0E8C `mov [internal_check_failed], eax` with EAX the caller's (C0CB0 / C0D6A
  pop it back before the jump), then `call fatal_error`.
- **Behaviour:** fatal_error reports 'Failed internal check N.' instead of 'DOS: File not
  found: ...' whenever EAX is not 0; N is a leftover of the caller (the file name's address
  in xn_dos_load_file and the world, digits in the texture loader).
- **Visible:** to the player, on a fatal path no record or scenario reaches (a missing data
  file).
- **Kept by:** `xn_dos_open` / `xn_dos_create` pass the path's address as N, so the message
  is still replaced; the number itself was a register leftover and is not reproduced.
- **Status:** kept in kind (the number differs).

### Q-DOS-02: counts and positions are DOS's AX even after a failure

- **Where:** `xn_dos_read` (C0DC4), `xn_dos_seek` (C0DE7), `xn_dos_file_size` (C0DFC).
- **Asm:** C0DC4 `mov eax, 3F00h; int 21h; ret`: EAX is DOS's, the carry not looked at by any
  caller; C0E0F builds DX:AX after the seek without testing CF.
- **Behaviour:** after a failed read, the "count" is DOS's error code.
- **Visible:** no failure happens in the records or the scenarios (every file is there).
- **Kept by:** `xn_dos_read` returns EAX as DOS left it; `xn_dos_seek` returns the error code
  and leaves the position alone; records `test dos`.
- **Status:** kept.

### Q-DOS-03: a write checks only its last piece

- **Where:** `xn_dos_write` (C0D8C); the world editor's writes (dead).
- **Asm:** C0D8C..C0DAE loops on 32K pieces (`cmp ecx, 8000h; jle` signed) without testing
  CF; the CF returned is the last `int 21h`'s.
- **Behaviour:** a failure in an earlier piece goes unnoticed.
- **Visible:** no caller (the world editor is dead code).
- **Kept by:** `xn_dos_write`; its records.
- **Status:** kept.

### Q-DOS-04: the ARENA2 path and the file name share one count of 80 characters

- **Where:** `xn_dos_make_path` (C0ED0).
- **Asm:** C0ED8 `mov ecx, 50h` once; both copy loops (C0EE0, C0EF3) end in `loop`; C0EE8
  `cmp byte ptr [edi-1], 5Ch`.
- **Behaviour:** the name gets 80 less the path's length; a path of 80 or more leaves ECX 0,
  which the name's `loop` takes as 2^32 (no limit); an empty arena2_path tests the byte
  before xn_dos_path.
- **Visible:** no: Z.CFG's path is short ('C:\ARENA2\').
- **Kept by:** `xn_dos_make_path` (one count `n`); records (`grpa_dos_make_path_noslash`:
  a path without its backslash).
- **Status:** kept.

### Q-DOS-05: a whole-file load counts an error code as bytes read

- **Where:** `xn_dos_load_file` (C0E24).
- **Asm:** C0E62 `movzx eax, ax; add [disk_last_file_size], eax; add edx, eax` after each
  read, without looking at CF.
- **Behaviour:** a failed read adds its error code to the length and to the pointer, then
  stops (it is not FFFFh).
- **Visible:** disk_last_file_size is the game's; no load fails in the records or scenarios.
- **Kept by:** `xn_dos_load_file`; records (fonts, light.dat, the shade tables).
- **Status:** kept.

## Memory (src/engine/mem.c)

### Q-MEM-01: the reciprocal tables' last entry is never written

- **Where:** `xn_mem_init` (149E00).
- **Asm:** 149E84 / 149EA7 `cmp ecx, 3FFh; jne`: the loops fill entries 1..3FEh of the
  1024-entry tables.
- **Behaviour:** entry 3FFh keeps whatever object 2's data had there.
- **Visible:** to the rasteriser only if it indexes 3FFh.
- **Kept by:** `xn_mem_init`'s loops (`k < 0x3FF`); its records.
- **Status:** kept.

## Keyboard (src/engine/kbd.c)

### Q-KBD-01: a key comes back with the keymap's address in EAX's upper half

- **Where:** `xn_kbd_read_key` (1427A8), and `xn_kbd_wait_key` (142808, dead) through it.
- **Asm:** 1427BE..1427EC: `dec eax; add eax, [xn_kbd_keymap]` ... `mov al, [eax+ebx]; mov
  ah, [xn_kbd_last_scancode]`: the key is built in a pointer register.
- **Behaviour:** EAX = the keymap entry's address's upper 16 bits : scan code : ASCII. With
  no key, or from the BIOS, the upper half is 0.
- **Visible:** the game's three sites (07CF3B, 07CFB5, 08C470) read only AL, but the
  boundary map's liveness counts EAX as read.
- **Kept by:** the boundary adapter `xn_kbd_read_key_b`; `xn_kbd_read_key` itself returns the
  16-bit key. `test --boundary kbd` (20 game records).
- **Status:** kept at the boundary only.

### Q-KBD-02: the wait for released keys looks one byte early

- **Where:** `xn_kbd_wait_all_released` (CE92C).
- **Asm:** CE938 `mov edi, xn_kbd_last_scancode` (142307), `mov ecx, 80h`, `repne scasb` for
  1: key_down starts at 142308.
- **Behaviour:** a last key of Esc (scan code 1) counts as held until it is read or released;
  key 127's flag is never looked at.
- **Visible:** to the game's three callers (the wait after a menu choice); not in the
  records (no key held).
- **Kept by:** `xn_kbd_wait_all_released` (the scan from &xn_kbd_last_scancode); records.
- **Status:** kept.

## Mouse (src/engine/mouse.c)

### Q-MOUSE-01: the cursor's y takes the x hotspot

- **Where:** `xn_mouse_cursor_clip` (12B326).
- **Asm:** 12B37A `movsx eax, word ptr [mouse_y]; movsx edx, word ptr [xn_mouse_hotspot_x]`.
- **Behaviour:** the cursor's top is mouse_y - hotspot_x.
- **Visible:** no: the game's hotspot is (0, 0).
- **Kept by:** `xn_mouse_cursor_clip`; records.
- **Status:** kept.

### Q-MOUSE-02: double-click distances are 16-bit

- **Where:** `xn_mouse_poll` (12B196).
- **Asm:** 12B1FF `sub ax, [mouse_x]; jns; neg ax; cmp ax, 4; jge` (and the same for y and the
  right button).
- **Behaviour:** a distance of -32768 stays negative after `neg` and counts as near.
- **Visible:** no: positions are within the screen.
- **Kept by:** `near16`; records.
- **Status:** kept.

## Random numbers (src/engine/rand.c)

### Q-RAND-01: the tick seed reads the wrong address

- **Where:** `xn_rand_seed_from_ticks` (153978, dead).
- **Asm:** 153979 `mov eax, 40006Ch; mov eax, [eax]`.
- **Behaviour:** reads the flat address 40006Ch (segment 40h, offset 6Ch written as one
  number) instead of the BIOS tick count at 46Ch.
- **Visible:** no caller. In the emulator the read faults (40006Ch is not mapped), so the
  function has no record.
- **Kept by:** `xn_rand_seed_from_ticks`.
- **Status:** kept.

## Timing (src/engine/timer.c)

### Q-TIMER-01: the PIT read programs channel 2

- **Where:** `xn_timer_read_pit` (CE8B2); two game sites.
- **Asm:** CE8B2 `mov al, 0B0h; out 43h, al`, then two `in al, 40h`.
- **Behaviour:** 0B0h selects channel 2, lo/hi access, mode 0: it reprograms the speaker's
  channel instead of latching channel 0 (00h), so the two reads of channel 0 are not latched.
- **Visible:** the port I/O (compared), and the PC speaker's channel on real hardware.
- **Kept by:** `xn_timer_read_pit`; its records and `test --boundary timer`.
- **Status:** kept.

## Strings (src/engine/str.c)

### Q-STR-01: numbers of 1 or 0 digits

- **Where:** `xn_str_from_int` (153900).
- **Asm:** 153902 `lea ecx, [ebx-1]; test ecx, ecx; je`: one digit returns at once; with 0
  digits ECX = -1 and 15391B divides by `[ecx*4 + xn_pow10_table]`, the dword before the
  table (xn_rand_seed, 153800).
- **Behaviour:** 1 digit writes nothing, not even a sign; 0 digits writes the sign and one
  character, '0' + value / xn_rand_seed (a seed of 0 divides by zero: '0', Q-SYS-01).
- **Visible:** no: the callers pass 3 and 4.
- **Kept by:** `xn_str_from_int`.
- **Status:** kept.

### Q-STR-02: a count of 0 is 2^32

- **Where:** `xn_str_find_u16` (CE45E), `xn_str_fill_ascending` (CE663) and its copy CE495,
  `xn_str_count_nonzero` (CE77F), `xn_str_find_byte_pair` (CE7A7).
- **Asm:** each tests its count with `dec; jne` after handling the first element.
- **Behaviour:** a count of 0 runs 2^32 times (until a match, or a fault).
- **Visible:** no caller passes 0 in the records or scenarios.
- **Kept by:** the do-while loops; records.
- **Status:** kept.

### Q-STR-03: appending a character writes no terminator

- **Where:** `xn_str_append_char` (CE49E).
- **Asm:** CE4A6 `mov [eax], dl; ret` over the old terminator.
- **Behaviour:** the string is terminated only by what followed it.
- **Visible:** no: the game's buffer is zeroed first.
- **Kept by:** `xn_str_append_char`; its record.
- **Status:** kept.

### Q-STR-04: copying a name folds no case

- **Where:** `xn_str_copy_alnum` (CE3FD).
- **Asm:** CE423 `and bl, 0DFh` is reached only for 'A'..'Z' (CE412..CE415).
- **Behaviour:** the mask meant to fold case lands on capitals, where it changes nothing;
  lower case is copied as it is.
- **Visible:** to parse_expand's macro lookup (it matches names as typed).
- **Kept by:** `xn_str_copy_alnum`; its record.
- **Status:** kept.

### Q-STR-05: "not found" is index 0

- **Where:** `xn_str_find_char` (15377E, dead).
- **Asm:** 15378E `xor eax, eax` when the terminator is passed.
- **Behaviour:** a character that is not there gives 0, as one at index 0 does.
- **Visible:** no caller.
- **Kept by:** `xn_str_find_char`.
- **Status:** kept.

### Q-STR-06: a word copy compares signed

- **Where:** `xn_str_copy_word_max80` (153964, dead).
- **Asm:** 15396B `cmp al, 20h; jle`.
- **Behaviour:** bytes from 80h end the word, as control characters do.
- **Visible:** no caller.
- **Kept by:** `xn_str_copy_word_max80`.
- **Status:** kept.

## Spells (src/engine/spell.c)

### Q-SPELL-01: "no effects" tests slot 1 twice and slot 2 never

- **Where:** `spell_has_no_effects` (CE4E0).
- **Asm:** CE4E5 and CE4EB both `cmp byte ptr [eax+2], 0FFh`.
- **Behaviour:** a spell whose only effect is in slot 2 counts as having none.
- **Visible:** to the game's one caller (the spell maker).
- **Kept by:** `spell_has_no_effects`; its records.
- **Status:** kept.

## Joystick (src/engine/joy.c)

### Q-JOY-01: the axis mask's pairing tests do nothing

- **Where:** `xn_joy_init` (152B00).
- **Asm:** 152B3E..152B4D: `mov al, ah; and al, 0Fh; test al, 3; jne; and al, 0FCh; test al,
  0C0h; jne; and al, 3; or ah, al`.
- **Behaviour:** the tests meant to keep an axis only with its pair work on bits the `and al,
  0Fh` already cleared or kept; the OR adds bits the mask has. The mask is the bits that read
  0 all 64 times.
- **Visible:** xn_joy_axis_mask; nothing reads it.
- **Kept by:** `xn_joy_init` (the mask as computed); its record.
- **Status:** kept.

## Serial driver (src/engine/serial.c)

### Q-SERIAL-01: a received byte is always the ring's first

- **Where:** `xn_serial_rx_get` (160A9C).
- **Asm:** 160AC8 `shl esi, 7; mov al, [esi + xn_serial_rx_buffers]`: the ring's address
  without the head.
- **Behaviour:** the head moves on and the count goes down, but every byte read is ring[0].
- **Visible:** no: the head-tracker drivers are never opened.
- **Kept by:** `xn_serial_rx_get`; its records.
- **Status:** kept.

### Q-SERIAL-02: closing a port sets the break bit

- **Where:** `xn_serial_close` (1609C0).
- **Asm:** 1609DB `mov al, 40h; out dx, al` to base + 3 (the line control register).
- **Behaviour:** the line is left sending a break instead of with its line control cleared.
- **Visible:** port I/O (compared); no tracker is opened.
- **Kept by:** `xn_serial_close`; its records.
- **Status:** kept.

### Q-SERIAL-03: a modem status interrupt reads the wrong registers

- **Where:** `xn_serial_irq_com1` (160AE0).
- **Asm:** L_160B30 reads 3F8h and 3FDh (data and line status) when the IIR says modem
  status; a transmit interrupt (IIR 2) falls through to the EOI.
- **Behaviour:** the modem status register (3FEh) is never read, so a modem status
  interrupt is not cleared.
- **Visible:** no: COM1's IRQ is never unmasked in the game.
- **Kept by:** `xn_serial_irq_com1`.
- **Status:** kept.

### Q-SERIAL-04: the COM1 handler returns with a 16-bit iret

- **Where:** `xn_serial_irq_com1` (160AE0); its entry stub.
- **Asm:** 160B42 `iretw` (66 CF) in 32-bit code.
- **Behaviour:** pops 16-bit IP, CS and FLAGS: it would lose EIP's upper half and the stack
  frame's alignment, a crash on real hardware.
- **Visible:** no: the IRQ is never unmasked.
- **Kept by:** the build's interrupt stub returns with the asm's own return instruction
  (tools/xn_rc.py `handler_entry`); the C handler is plain.
- **Status:** kept (in the entry stub).

## Head trackers (src/engine/helmet.c)

### Q-HELMET-01: the smoothing sums land in the pick's scratch point

- **Where:** `xn_helmet_smooth` (1535A4).
- **Asm:** 1535DC..153640 add into 120288, 12028C, 120290 (xn_pick_view_x, xn_pick_view_y,
  pick_distance: the pick code's).
- **Behaviour:** smoothing a sample overwrites the last pick's point and distance, which the
  game's click handlers read.
- **Visible:** only if a tracker were polled (never).
- **Kept by:** `xn_helmet_smooth`; its records.
- **Status:** kept.

### Q-HELMET-02: driver A's read gives no angles

- **Where:** driver A's read (160EAC) as the front end's poll (15356C) uses it.
- **Asm:** 160EAC reads its 6-byte record into xn_helmet_a_record and returns EAX = 0 (or -1
  after a timeout, with CF the last compare: more than 6 bytes waiting clears it); 153586
  then smooths EAX, EDX and EBX: EDX and EBX are the poll's caller's.
- **Behaviour:** driver A never decodes its record; the "sample" is 0 or -1 and two leftover
  registers.
- **Visible:** no: nothing polls a tracker.
- **Kept by:** the C driver table's A read gives (0 or -1, 0, 0); the leftovers are dropped.
  Group A's record `grpa_helmet_poll_a` polls driver A with EDX = EBX = 0, where the two
  agree.
- **Status:** dropped in part (the two leftover registers: dead code).

### Q-HELMET-03: driver B's packet search goes on looking for the bad checksum

- **Where:** `xn_helmet_b_read` (16116C), `xn_helmet_b_find_packet` in the C.
- **Asm:** 1611BF `cmp al, [ebp+esi]; pop esi; jne L_161196`: after a mismatch AL holds the
  computed sum, and the search compares the same byte with it.
- **Behaviour:** after a bad checksum the search looks for bytes equal to that sum instead of
  FFh (without moving or counting down).
- **Visible:** no tracker is opened.
- **Kept by:** `xn_helmet_b_find_packet`; b_read's records (the probe's, and
  `grpa_helmet_b_read`: a good packet).
- **Status:** kept.

### Q-HELMET-04: driver A waits on port 1's ring

- **Where:** `xn_helmet_a_read` (160EAC), `xn_helmet_a_read_text` (160E6F).
- **Asm:** 160EB2 / 160E75 `cmp dword ptr [xn_serial_rx_count], ...`: port 1's count.
- **Behaviour:** on another port it waits for bytes that never arrive there.
- **Visible:** no tracker is opened (the game passes port 1).
- **Kept by:** `a_read_record`.
- **Status:** kept.

### Q-HELMET-05: driver B's version scan starts at the ring's start

- **Where:** `xn_helmet_b_parse_version` (161114).
- **Asm:** 16111C..161127: the ring's base - 1, then `inc ebx` up to base + 1FFh.
- **Behaviour:** it scans the ring from index 0, not from its head, and stops one byte short
  of its end.
- **Visible:** no tracker is opened.
- **Kept by:** `xn_helmet_b_parse_version`.
- **Status:** kept.

### Q-HELMET-06: driver C's read passes its caller's EBX to the mouse driver

- **Where:** driver C's read (1614F0).
- **Asm:** 1614F0 `mov eax, 6005h; xor ecx, ecx; xor edx, edx; call 161524`: EBX is not set.
- **Behaviour:** the tracker read gets BX as the poll's caller left it.
- **Visible:** no tracker is opened.
- **Kept by:** `xn_helmet_c_read(bx, ...)` takes it (its records pass the asm's); the front
  end passes 0.
- **Status:** kept in `xn_helmet_c_read`; dropped in the front end (dead).

## Animation (src/engine/anim.c)

### Q-ANIM-01: the speed of reset_with_rate goes to linear 0Ch

- **Where:** `xn_anim_reset_with_rate` (C0131, dead).
- **Asm:** C0131 `call xn_anim_reset` (which returns EAX = 0) then `mov word ptr [eax+0Ch],
  dx`.
- **Behaviour:** writes the speed over the real-mode int 3 vector's offset instead of
  a->tick_divisor.
- **Visible:** no caller.
- **Kept by:** `xn_anim_reset_with_rate`; its records.
- **Status:** kept.

### Q-ANIM-02: restart sets the position to the bare offset

- **Where:** opcode 5, `xn_anim_op_restart` (C026B).
- **Asm:** C026B..C0276: EDI = script + offset is computed, then `mov [esi+8], eax; mov edi,
  eax` store the offset alone.
- **Behaviour:** the script continues at the address equal to the offset (low memory).
- **Visible:** to the game only if a script uses opcode 5.
- **Kept by:** `xn_anim_op_restart`; its records.
- **Status:** kept.

### Q-ANIM-03: opcode 11 yields only when the position wraps

- **Where:** `xn_anim_op_set_record` (C02DF).
- **Asm:** C02E2 `add edi, 2` is the last flag-setting instruction before `ret`: its carry is
  the opcode's yield flag.
- **Behaviour:** the script goes on after it (a pointer never wraps).
- **Visible:** to the game (every state sets its record group with it).
- **Kept by:** `xn_anim_op_set_record`; records and scenarios (the fight).
- **Status:** kept.

### Q-ANIM-04: a random range with hi = lo - 1 gives 0

- **Where:** `xn_anim_rand_range` (C02E9).
- **Asm:** C02F7 `sub dh, dl; inc dh; div dh`: a span of 0 faults; XnGine's handler leaves
  EDX = 0, so `mov al, dl; add al, ah` gives 0.
- **Behaviour:** the loop count or the percentage roll is 0.
- **Visible:** to a script with such a loop (none in the records).
- **Kept by:** `xn_anim_rand_range` checks the span first (Q-SYS-01).
- **Status:** kept.

### Q-ANIM-05: an opcode from 12 up

- **Where:** `xn_anim_run_opcodes` (C01EB).
- **Asm:** C01F3 `call dword ptr [eax*4 + xn_anim_opcodes]`: the table has 12 handlers, two
  zeros, then other data.
- **Behaviour:** the asm calls address 0 or data: a crash.
- **Visible:** no: no record or scenario reaches one.
- **Kept by:** nothing: canonical C stops the script (`run_opcode`'s default).
- **Status:** dropped (a crash cannot be kept; unreachable in the records and scenarios).

## Palette (src/engine/pal.c)

### Q-PAL-01: a count of 0 sets 2^32 DAC entries

- **Where:** `xn_pal_set_range_8bit` (0CD33A).
- **Asm:** `0CD343 movzx ecx, bx` ... `0CD362 loop 0CD346`: a count of 0 makes the `loop` run
  2^32 times (the index wraps at 256).
- **Behaviour:** `xn_pal_set_range_8bit(rgb, first, 0)` writes 2^32 DAC entries from rgb.
- **Visible:** to the game in principle (9 sites); every recorded call passes 1 or more.
- **Kept by:** `u32 n = count; do ... while (--n != 0)`; the records of 0CD33A.
- **Status:** kept.

### Q-PAL-02: index 0 is never the nearest colour

- **Where:** `xn_pal_find_nearest` (12D40E).
- **Asm:** the search starts at the palette's second entry (`12D40E` ... `add esi, 3`) and counts
  255 entries.
- **Behaviour:** black (or whatever entry 0 holds) is never chosen.
- **Visible:** no: only the dead `xn_pal_build_ega16_map` calls it.
- **Kept by:** `nearest_in` starts at 1; `tools/xn_equiv.py xn_pal_find_nearest`.
- **Status:** kept.

### Q-PAL-03: colour differences are signed bytes

- **Where:** `xn_pal_find_nearest` (12D40E), `xn_pal_find_nearest_hsv` (12D481).
- **Asm:** each difference is a byte subtraction sign-extended before it is squared.
- **Behaviour:** a difference of 128 or more (8-bit components; hues) wraps: 200 counts as -56.
- **Visible:** no (dead callers).
- **Kept by:** `e = (s8)(r - p[0])`; `tools/xn_equiv.py xn_pal_find_nearest`.
- **Status:** kept.

### Q-PAL-04: a negative fade length runs on

- **Where:** `xn_pal_fade_to` (12D8BD).
- **Asm:** the retrace loop counts with `dec ebx; jne`.
- **Behaviour:** steps < 0 fade for 2^32 - |steps| retraces (with negative per-step deltas).
- **Visible:** one game site (`play_death_video`) passes 50.
- **Kept by:** `u32 left = steps; do ... while (--left != 0)`; the records of 12D8BD.
- **Status:** kept.

### Q-PAL-05: HSV to RGB returns astray

- **Where:** `xn_pal_hsv_to_rgb` (12D6AB), dead.
- **Asm:** for a grey (s = 0) and hue sectors 0-4 the exits at 12D6F4, 12D7A5, 12D7C5 and 12D7E5
  return before popping the three words pushed at the entry: the `ret` jumps to the address they
  make.
- **Behaviour:** only hues of sector 5 and up (140h and above) come back to the caller.
- **Visible:** no caller.
- **Kept by:** the C returns the colour on every path (a jump into data has no C form);
  `tools/xn_equiv.py xn_pal_hsv_to_rgb` compares sectors 5 and up exhaustively.
- **Status:** kept for the sectors that return; dropped for the others (dead, and no behaviour).

## Graphics modes and screen (src/engine/gfx.c)

### Q-GFX-01: the inclusive present shows one row more

- **Where:** `xn_gfx_present_inclusive` (0CDD81).
- **Asm:** `0CDD8E inc dword [xn_gfx_clip_bottom]`, present, `dec`, unless the bottom is 200.
- **Behaviour:** the row just below the clip window goes to the screen too.
- **Visible:** to the game: main and 27 UI loops; every scenario.
- **Kept by:** `xn_gfx_present_inclusive`; its 28 records, the scenarios.
- **Status:** kept.

### Q-GFX-02: waiting for 0 retraces waits for 2^32

- **Where:** `xn_gfx_wait_vretraces` (0CD32C), dead.
- **Asm:** `0CD336 dec eax; jne`.
- **Behaviour:** n = 0 waits 2^32 retraces.
- **Visible:** no caller.
- **Kept by:** `do ... while (--n != 0)`.
- **Status:** kept.

### Q-GFX-03: the linear frame buffer's address loses its low word

- **Where:** `xn_gfx_vesa_set_mode` (15FCA7).
- **Asm:** after DPMI 0800h the address is BX:CX; `15FD7B shl ebx, 10h` then copies the cleared BX
  into CX instead of CX into BX.
- **Behaviour:** the frame buffer is used from (its address & FFFF0000h).
- **Visible:** VESA only; the game never finds VBE (and the emulator has none).
- **Kept by:** `(u8 *)(r.ebx << 16)`.
- **Status:** kept.

### Q-GFX-04: the banked present's first copy runs to the end of the bank

- **Where:** `xn_gfx_vesa_present_banked` (15FEAE).
- **Asm:** the first copy is 10000h minus the offset's low word, whatever is left of the view.
- **Behaviour:** a view that ends inside its first bank copies past its end, and the remaining
  count goes negative (the last copy is about 4 GB).
- **Visible:** VESA only.
- **Kept by:** `xn_gfx_vesa_present_banked`.
- **Status:** kept.

## Fonts (src/engine/font.c)

### Q-FONT-01: loading a font leaves EBX = 4 to the game

- **Where:** `xn_font_load` (12DB28); the game's `init_game_data` (04ED36, four calls).
- **Asm:** `12DB2E mov ebx, 4` (the digit count for `xn_str_from_int`), never restored.
- **Behaviour:** the game's code after each call finds EBX = 4 where Watcom's convention keeps
  its own value; after the fourth call it calls 03F5EF with it.
- **Visible:** to the game (game_reads of 12DB28 include EBX).
- **Kept by:** the boundary adapter `xn_font_load_b`; `xn_rc.py test --boundary xn_font_load`.
- **Status:** kept at the boundary only.

### Q-FONT-02: a glyph cut on the left is shifted by the wrong amount

- **Where:** `xn_font_draw_glyph` (12DC44).
- **Asm:** `12DCB3 sub ebx, eax` (the width less the columns cut), then `12DCB7 add bl, 10h` and
  `12DCBA mov [shift], bl`: the rows are shifted by the remaining width plus 16, not by the
  columns cut plus 16, and that width (plus 16) is what is drawn.
- **Behaviour:** the wrong columns of the glyph show, and 16 blank columns further on (into the
  right clip); a remaining width of 240 or more wraps the shift below 16.
- **Visible:** to the game wherever text crosses a window's left edge.
- **Kept by:** `xn_font_draw_glyph`; the records of `build/xn_canon/group_b/records/b_text`.
- **Status:** kept.

### Q-FONT-03: a glyph's width counts in a byte

- **Where:** `xn_font_draw_glyph` (12DC44).
- **Asm:** `12DCFB mov cl, bl` ... `dec cl; jne`.
- **Behaviour:** a width of 256n draws 256 columns (0 too).
- **Visible:** no font has a glyph wider than 16.
- **Kept by:** `n = (u8)w; do ... while (--n != 0)`.
- **Status:** kept.

### Q-FONT-04: characters from 80h are skipped, and '!' measures as a space

- **Where:** `xn_font_draw_string` (12DBCC), `xn_font_glyph_width` (12DD1C).
- **Asm:** `12DBE1 sub cl, 21h; jl` (signed: 80h-FFh are below '!'); `12DD1C sub eax, 21h; jle`.
- **Behaviour:** the string drawer draws nothing for 80h-FFh (no advance); the width of '!' is
  the space's less the glyph spacing, not its glyph's.
- **Visible:** to the game (its text has no 80h-FFh; '!' widths in its centring).
- **Kept by:** `(s8)c >= 0x21`, `c <= 0x21`; `tools/xn_equiv.py xn_font_glyph_width`.
- **Status:** kept.

## Images (src/engine/img.c)

### Q-IMG-01: the EGA remap reads past its map

- **Where:** `xn_img_remap_colours` (147820), dead.
- **Asm:** the pixel indexes `xn_pal_ega16_map` (16 bytes) unmasked.
- **Behaviour:** pixels above 15 take the bytes after the map.
- **Visible:** no caller.
- **Kept by:** `xn_pal_ega16_map[*p]`.
- **Status:** kept.

## 2D drawing (src/engine/draw*.c)

### Q-DRAW-01: colour tables are indexed by replacing an address byte

- **Where:** every blit through a 256-byte table: `xn_draw_cif_rle_frame`, `xn_draw_img_masked_remap`,
  `xn_draw_darken_rect`, `xn_draw_paperdoll_item`, `xn_draw_remap_masked_rect`,
  `xn_draw_remap_rect`, `xn_draw_remap_rows_320`, `xn_draw_image_shaded`.
- **Asm:** `mov al, [eax]` with the colour loaded into AL of the table's address.
- **Behaviour:** a table that is not 256-aligned is read from the 256 bytes before its start.
- **Visible:** no: the shade rows and the dye remaps are aligned (the scaled image's `xlat` adds,
  as C indexing does).
- **Kept by:** `XN_TABLE256` (xdrawhlp.h).
- **Status:** kept.

### Q-DRAW-02: copying from the screen leaves EAX to the game

- **Where:** `xn_draw_get_rect` (144E84, through 144E9C); 7 game sites.
- **Asm:** after the copy EAX is `xn_gfx_width - w` (144EC0 onwards), after a clip that leaves
  nothing the clip's x.
- **Behaviour:** the game reads AH and EAX's upper half after its calls.
- **Visible:** to the game (game_reads `ah eax.u`).
- **Kept by:** the boundary adapter `xn_draw_get_rect_b`; `test --boundary xn_draw_get_rect`.
- **Status:** kept at the boundary only.

### Q-DRAW-03: the image blits leave their registers to the game

- **Where:** `xn_draw_image` (144F68 / 144F7C), `xn_draw_image_transparent` (144FB4 / 144FC8);
  100 game sites and a pointer the game holds to each.
- **Asm:** after a blit, EAX = `xn_gfx_width - w`, ECX = EDX = 0, EBX = w (the opaque one); EAX =
  the clipped x with the last source pixel loaded in AL, ECX = 0, EDX = `xn_gfx_width`, EBX =
  16 w (the transparent one: the planted ret's offset); after a clip that leaves nothing, the
  clip's x, y, w, h (Q-DRAW-04).
- **Visible:** to the game (game_reads `ah eax.u ecx`).
- **Kept by:** the boundary adapters `xn_draw_image_b`, `xn_draw_image_transparent_b`
  (`xn_draw_image_regs_out`, `xn_draw_image_transparent_regs_out`); `test --boundary`.
- **Status:** kept at the boundary only.

### Q-DRAW-04: a failed top clip leaves y less the window's top

- **Where:** `xn_draw_clip_rect` (144E00).
- **Asm:** `sub edx, [clip_top]` before the height test that fails.
- **Behaviour:** the rectangle's y comes back as y - clip_top.
- **Visible:** through Q-DRAW-03's registers.
- **Kept by:** `xn_draw_clip_rect`; `tools/xn_equiv.py xn_draw_clip_rect`.
- **Status:** kept.

### Q-DRAW-05: the byte copy leaves ECX = 0 to the game

- **Where:** `xn_draw_copy_rect_stride_bytes` (0CE31C); 7 game sites.
- **Asm:** the row count runs down in ECX (`0CE341 dec ecx; jne`).
- **Behaviour:** the game's code after the call finds ECX = 0.
- **Visible:** to the game (game_reads `ecx`).
- **Kept by:** the boundary adapter `xn_draw_copy_rect_stride_bytes_b`.
- **Status:** kept at the boundary only.

### Q-DRAW-06: counts of 0 run 2^32 times

- **Where:** `xn_draw_fill_rect`, `xn_draw_fill_rect_clipped` (y1 = y2), `xn_draw_copy_rect_stride`,
  `xn_draw_copy_rect_stride_bytes`, `xn_draw_darken_rect`, `xn_draw_zoom4x`, `xn_draw_mark_matching`,
  `xn_draw_paperdoll_mask`, `xn_draw_image_drop_shadow`, the dead row routines.
- **Asm:** `dec reg; jne` and `inc; cmp; jne` loops tested at the bottom.
- **Behaviour:** a width or height of 0 runs the loop 2^32 times.
- **Visible:** no game call passes 0 (records).
- **Kept by:** do-while loops on u32 counts.
- **Status:** kept.

### Q-DRAW-07: lines stop one pixel short

- **Where:** `xn_draw_hline` (15310C), `xn_draw_vline` (153188), `xn_draw_line_unclipped` (153210);
  so `xn_draw_line`, `xn_draw_line_to` and the dead `xn_draw_rect_outline`.
- **Behaviour:** the right (lower) end point is not drawn; the outline's bottom edge is not drawn
  when it is the window's bottom.
- **Visible:** to the game (the text cursor, the maps' lines).
- **Kept by:** the C's half-open ranges; the records of `b_lines`.
- **Status:** kept.

### Q-DRAW-08: the line clip's edge cases

- **Where:** `xn_draw_line_clip` (1532D4).
- **Asm:** a right cut stores `clip_right - 1`, a bottom cut `clip_bottom`; `y2 <= clip_top`
  rejects a horizontal line on the window's top row.
- **Visible:** to the game (12 + 2 sites).
- **Kept by:** `xn_draw_line_clip`; `tools/xn_equiv.py xn_draw_line_clip` (and `@window`).
- **Status:** kept.

### Q-DRAW-09: the RLE animations' off-screen tests

- **Where:** `xn_draw_cif_rle_frame` (0CB39A), `xn_draw_cast_anim_mirrored` (0CDB7A).
- **Asm:** a literal run is drawn when its first pixel is at or after `screen_buffer`, a repeat
  when after it; the mirrored animation does not move its mirror pointer over a skipped literal
  run.
- **Behaviour:** rows partly above the screen draw whole runs or none; the mirror image of a row
  after a skipped run is shifted.
- **Visible:** to the game (weapons and the casting hands, which start above the screen).
- **Kept by:** the C's tests; the records (`b_cast`).
- **Status:** kept.

### Q-DRAW-10: the masked remap's top clip draws from a negative row

- **Where:** `xn_draw_img_masked_remap` (0CB473).
- **Asm:** `0CB489 sub ebx, [clip_top]` then the row offset of that (negative) row.
- **Behaviour:** after a top clip the image is drawn from `xn_gfx_row_offset[y - clip_top]`, a
  dword before the table, instead of from clip_top.
- **Visible:** to the game (one site), when its image starts above the view.
- **Kept by:** `xn_draw_img_masked_remap`; `b_masked_remap`.
- **Status:** kept.

### Q-DRAW-11: the CEL frame counter

- **Where:** `xn_draw_cel_frame` (0CD126).
- **Asm:** `div bx` of the counter's low word by the frame count: 0 frames is a divide error
  (frame 0, Q-SYS-01); the row loop counts with `loop` on ECX, of which only CX is loaded.
- **Behaviour:** frame = (frame_counter mod 65536) mod frames; the rows run on the caller's ECX
  upper half (0 at the game's one site in every record).
- **Visible:** the frame choice to the game; the ECX upper half no (0).
- **Kept by:** `(u16)frame_counter % frames`, 0 for no frames.
- **Status:** kept; the ECX upper half dropped.

### Q-DRAW-12: the masked remap refuses rectangles that touch the edge

- **Where:** `xn_draw_remap_masked_rect` (0CDCE5), dead.
- **Asm:** `x + w >= clip_right` (and the bottom) rejects.
- **Kept by:** `>=`. **Visible:** no caller. **Status:** kept.

### Q-DRAW-13: the hurt flash's odd rows

- **Where:** `xn_draw_view_checkerboard` (0CD53C).
- **Asm:** `shr ecx, 1; jb` enters the two-row loop at its second row: the odd row uses up a
  pair.
- **Behaviour:** an odd row count draws one row fewer (3 rows draw 1); 1 row runs 2^32 pairs.
- **Visible:** to the game (the view's height is even in every record).
- **Kept by:** `xn_draw_view_checkerboard`; its 16 records.
- **Status:** kept.

### Q-DRAW-14: the dead shaded blit's shade row is never set

- **Where:** `xn_draw_image_shaded` (0C8381, 0C8395); `xn_draw_shade_row` (0C7B8A).
- **Behaviour:** the shade row is the byte's initial 0.
- **Visible:** no caller. **Kept by:** reading `xn_draw_shade_row`. **Status:** kept.

### Q-DRAW-15: a scaled image of 2 pixels or less draws nothing

- **Where:** `xn_draw_image_scaled` (0C0700).
- **Asm:** `0C0700 cmp bx, 2; jle` (before anything); `0C0792 cmp ax, 2; jle` for the height,
  after the image is unpacked.
- **Behaviour:** w <= 2 or h <= 2 (16-bit, signed) draws nothing; a row-compressed image with
  h <= 2 is still unpacked into scratch_buffer.
- **Visible:** to the game (4 sites).
- **Kept by:** the order of the C; the records.
- **Status:** kept.

### Q-DRAW-16: the scaled image's strides

- **Where:** `xn_draw_image_scaled` (0C0700).
- **Asm:** `0C08B7 add esi, 100h` (the unpacked image's stride) but `0C07FB imul ax, [src_w]` for
  the rows the top clip skips; `0C08A5 add edi, 140h`.
- **Behaviour:** a plain (not row-compressed) image is read with a stride of 100h; after a top
  clip the source starts src_w bytes per skipped row in; destination rows are 320 apart whatever
  the screen's width.
- **Visible:** to the game when an item is cut by the window's top (the inventory scrolls).
- **Kept by:** `xn_draw_image_scaled`; the records of `b_scaled`.
- **Status:** kept.

### Q-DRAW-17: the scaled row's pair loop counts ECX (dropped)

- **Where:** the row compiler `xn_draw_image_scaled_row` (0C094A).
- **Asm:** `0C096F xor cx, cx` leaves ECX's upper half as `xn_draw_image_scaled`'s h argument had
  it; `0C0A04 loop` counts all of ECX in the first column with two or more pixels.
- **Behaviour:** an h of 65536 or more would emit about 65536 times more stores.
- **Visible:** no: the game's 4 sites pass h < 65536 (records).
- **Kept by:** nothing: the C has no generated loop.
- **Status:** dropped.

### Q-DRAW-18: a scaled column of 33 pixels or more wraps its jumps (dropped)

- **Where:** the row compiler (0C094A).
- **Asm:** each column's two `je` displacements are bytes: a column of about 33 pixels or more
  (a scale of 33 or more) wraps them, and the generated code jumps into itself.
- **Visible:** no: no game image is scaled that far.
- **Kept by:** nothing: the C draws the column.
- **Status:** dropped.

### Q-DRAW-19: rows wider than the unrolled bodies (dropped)

- **Where:** `xn_draw_image_transparent` (144FC8 / 14501C, 640 steps), `xn_draw_image_shaded`
  (0C8395 / 0C83F6, 321 steps).
- **Asm:** the planter writes its `ret` at step w: a wider row plants it past the body's end.
- **Visible:** no: clipped rows are at most 320 wide.
- **Kept by:** nothing: the C loops w times.
- **Status:** dropped.

## The VID player (src/engine/vid.c)

### Q-VID-01: a long audio chunk copies over its own start

- **Where:** `xn_vid_read_audio` (0C1C4A).
- **Asm:** each pass of the copy loop starts again at the audio buffer's start.
- **Behaviour:** an audio chunk longer than what is left of the read buffer is split by a
  refill, and its second part overwrites its first.
- **Visible:** in the sound only (no shipped movie's chunk crosses: the records of
  `b_vid_synth_big` make one).
- **Kept by:** `xn_vid_read_audio`.
- **Status:** kept.

### Q-VID-02: the movie's row step keeps the screen width's upper half

- **Where:** `xn_vid_open` (0C1654).
- **Asm:** a 16-bit `sub` of the movie's width from the screen's.
- **Visible:** no (the width is 320).
- **Kept by:** `(xn_gfx_width & 0xFFFF0000) | (u16)(...)`. **Status:** kept.

### Q-VID-03: the movie file is opened unchecked

- **Where:** `xn_vid_open` (0C1654).
- **Asm:** `int 21h 3D00h` and the handle stored from AX without looking at CF.
- **Behaviour:** a file that does not open (it was found by `xn_dos_file_exists` just before)
  leaves DOS's error code as the handle.
- **Visible:** no (the file exists).
- **Kept by:** `xn_dos_open_mode` (group A), its result ignored. **Status:** kept.

### Q-VID-04: a raw frame that crosses the read buffer goes on with the file handle as its width

- **Where:** `xn_vid_decode_chunk` (0C18A3), the raw frame (type 0) at 0C1A04.
- **Asm:** `0C1A19 mov ecx, ebx` reloads each row's count from EBX; `0C1A1F call xn_vid_refill`
  leaves the file handle in BX (`mov bx, [file]` for DOS).
- **Behaviour:** the rows after a refill are as wide as the handle (5 or so): the frame is
  garbled and the chunks after it are misread.
- **Visible:** no shipped movie has a raw frame (`tools/xn_mkrec.py b`
  surveys them); a synthetic one shows it (`b_vid_synth_big`).
- **Kept by:** `next_w` in the raw frame's loop.
- **Status:** kept.

### Deviations

#### D-VID-01: the C unlocks what the VID player locked

- **Where:** `xn_vid_play` (0C1500), `xn_vid_finish` (0C1844).
- **Asm:** the player locks its data (4F1h bytes), its code (907h) and its two audio buffers
  (4000h each) with the game's `dpmi_lock_region`, but its four `dpmi_unlock_region` calls get no
  size: EDX as earlier calls left it. That is the movie's y (from `xn_vid_play`'s arguments,
  through calls that keep EDX), or after a sample was stopped what SOS's stop call left (0Ah),
  and after an unlock that was done what `int386` left (0). `dpmi_unlock_region` does nothing for
  a size of 0.
- **Behaviour (asm):** a movie at y = 0 (every game movie) never unlocks anything (a leak of DPMI
  page locks); after a stopped sample the first audio buffer is unlocked by 10 bytes. Records:
  `xn_vid_finish` newgame_0@1951 (no unlock), save_mord__fight_walk (one), `xn_vid_play`
  save_mord__fight_swing@4517 (none).
- **Why the C differs:** the size is a leftover register: behaviour that hangs on a garbage
  register has no canonical form. The C unlocks what it locked, with the real sizes.
- **Why the game cannot see it:** DPMI page locks have no effect the game can read (and none in
  the emulated machine). The only traces are the int 31h 0600h/0601h calls, which the boundary's
  I/O compare leaves out (`tools/xn_rc.py drop_page_locks`, the replay answering an unrecorded
  one with success), and MemCheck's last-call record (file and line, 0x188A9B..), which the extra
  `dpmi_unlock_region` calls set through its checked memset and which only MemCheck's own error
  reports read (`config/xngine_dropped.csv` kind memory-own, for `xn_vid_play` and
  `xn_vid_finish`).
- **Status:** deviation (decided 2026-10-05).

## Collision (src/engine/collide.c, colmodel.c)

### Q-COLL-01: the plane distance takes the plane point's y for its z term

- **Where:** `xn_collide_plane_distance` (14C25F), dead. Canonical: collide.c.
- **Asm:** `14C28F mov eax, [esi + 8]; 14C292 sub eax, [ecx + 4]`: s.z - c.y where s.z - c.z was
  meant (the x and y terms use c.x, c.y).
- **Behaviour:** the distance is n . (s - c) with c's z replaced by its y.
- **Visible:** no caller (dead).
- **Kept by:** `xn_s64_mac(&d, n->z, s->z - c->y)`; its records (group_e_collide
  `plane_distance`, probe records) and `equiv_e.py xn_collide_plane_distance` (500,000
  samples).
- **Status:** kept.

### Q-COLL-02: the vertex test returns the sphere's x on a hit

- **Where:** `xn_collide_face_test_vertices` (15CEDE), dead.
- **Asm:** the loop saves the centre (`push eax; push edx; push ebx`) around each
  `xn_collide_point_in_sphere` call and pops it back (`15CEFF pop ebx; pop edx; pop eax`)
  before testing the result's sign (`15CF02 jns 15CF0F`): on a hit EAX is the centre's x.
- **Behaviour:** returns the centre's x when a corner is inside the sphere, -1 when none is.
- **Visible:** no caller.
- **Kept by:** `return c->x;` in the hit branch; records group_e_collide
  `face_test_vertices_near/_far`.
- **Status:** kept.

### Q-COLL-03: a model without collision spheres is always hit

- **Where:** `xn_collide_segment_model` (14A300), `xn_collide_spheres_model` (14AA92): game
  entries. Canonical: colmodel.c.
- **Asm:** `14A44D mov ebp, [eax + 20h]; 14A452 jle 14A678` (14A678: `xor eax, eax`): with a
  sphere count of 0 or less the test returns 0, "a hit", after its bounding sphere (and the
  same in 14AA92). The hit list's count has been set to 0 before.
- **Behaviour:** any segment or probe that reaches the bounding sphere of a model with no
  collision spheres collides with it (the result 0, as in modes 1 and 2).
- **Visible:** to the game (9 and 7 sites), for ARCH3D models with sphere_count 0.
- **Kept by:** `if (left <= 0) return 0;` in both; records and `test --boundary`.
- **Status:** kept.

### Q-COLL-04: the model-model test never runs its detailed test

- **Where:** `xn_collide_model_model` (14A6C0), dead; the code after it, 14A710-14AA57
  (`xn_collide_model_model_detail` in the ABI table: a block, not a function).
- **Asm:** `14A6FE cmp [mode], 2; 14A705 je 14AA58; 14A70B jmp 14AA58`: both ways past the
  detailed sphere-pair and face test, which nothing else enters (it has no prologue: its
  exits pop 14A6C0's registers). That code has a bug of its own: it takes a face's plane
  point from the face record (`+8`, the first point slot), not from the model's points.
- **Behaviour:** two models collide (0) when their bounding spheres meet, in every mode.
- **Visible:** no caller.
- **Kept by:** colmodel.c returns 0 after the bounding spheres; the detailed test is not
  written (its 31 blocks are justified as unreachable in the coverage). Records:
  group_e_collide `model_model_*`.
- **Status:** kept (the result); the unreachable detailed test is not C.

### Q-COLL-05: a flat's hit normal has the flat's position added

- **Where:** `xn_collide_segment_flat` (14B1E0) and its game entry
  `xn_collide_segment_flat_stk` (14B1C7).
- **Asm:** `14B3F6..14B426`: the normal (`xn_collide_flat_normal`, rounded >> 8) gets
  `add eax, [edi]; add edx, [edi + 4]; add ebx, [edi + 8]` (edi: the flat's position), as
  the hit point above it does.
- **Behaviour:** the hit's nx, ny, nz are the normal plus the flat's world position.
- **Visible:** to the game (1 site), which receives the hit list.
- **Kept by:** `hit->nx = round8(normal.x) + pos->x` ...; records group_e_collide
  `segment_flat_hit`, `segment_flat_stk_hit`.
- **Status:** kept.

### Q-COLL-06: the probe test's "stop at the first" mode unbalances the stack

- **Where:** `xn_collide_spheres_model` (14AA92), mode 1.
- **Asm:** the mode-1 exit from the sphere loop (14AEF2 `pop eax` ...) pops two dwords more
  than the loop pushed and returns through its caller's frame.
- **Behaviour:** a crash (or a return to a wrong address) whenever mode 1 meets a sphere.
- **Visible:** no: the game passes mode 0 or 2 only (src/lifted/colstuff.c 267, 284, 311,
  501, 513, 525; color.c 123); the corpus has modes 0 (2) and 2 (29). Not recordable.
- **Kept by:** colmodel.c returns 0 there (what mode 1 means).
- **Status:** dropped (unreachable from the game).

### Q-COLL-07: a stub chains three helpers' registers

- **Where:** `xn_collide_ref_helpers` (14B1B7), dead.
- **Asm:** `14B1B7 call xn_collide_sphere_plane; call xn_collide_point_in_face; call
  xn_collide_face_test_edges; ret`: each helper gets the previous one's leftover registers.
- **Behaviour:** private scratch writes and leftover registers only.
- **Visible:** no caller; it writes only engine scratch (dropped).
- **Kept by:** canonical C does nothing (its outputs are excused: config/xngine_dropped.csv).
- **Status:** dropped.

### Q-COLL-08: the probe-probe test mixes fixed point and world units

- **Where:** `xn_collide_spheres_spheres` (14B017), dead.
- **Asm:** `14B05D sub eax, [esi] .. 14B06B shl eax, 8; shl edx, 8; shl ebx, 8`: the probes'
  offset is taken << 8 (24.8), but the spheres' positions and radii are compared in world
  units.
- **Behaviour:** only probes at the same position (or within a fraction of a unit) can
  report a pair.
- **Visible:** no caller.
- **Kept by:** colmodel.c computes the offset << 8 as the asm does; records group_e_collide
  `spheres_spheres_hit/_near/_miss`.
- **Status:** kept.

### Q-COLL-09: the bounding-sphere segment test's products are 32-bit

- **Where:** `xn_collide_segment_sphere` (14C904), used for every model's and flat's
  bounding sphere.
- **Asm:** `14C934 imul eax, eax` ... `14C98A imul eax, [xn_collide_vec_b]`: the squares and
  the dot product keep their low dwords (only the projection's square is 64-bit).
- **Behaviour:** for a segment or a centre more than about 32,000 world units from the
  segment's end the squares wrap and the test's answer is arbitrary.
- **Visible:** in principle (the game's 10 segment-test sites: xn_collide_segment_model and
  xn_collide_segment_flat_stk); the game's segments (missiles, rays) and models are far
  shorter.
- **Kept by:** the same s32 arithmetic; `equiv_e.py xn_collide_segment_sphere` (500,000
  samples, world-sized inputs) and the records.
- **Status:** kept.

## World (src/engine/world.c)

### Q-WORLD-01: the band table's seek mode is the file name's low byte

- **Where:** `xn_world_read_height_bands` (0C31AE), called by `xn_world_open` (0C310F).
- **Asm:** `0C31AE mov edx, [bands_offset]; xor ecx, ecx; shld ecx, edx, 10h; and edx, FFFFh;
  0C31C0 call xn_dos_seek` with AL never set: it is what xn_world_open left in EAX, the file
  name's address (0x17521C: AL = 1Ch). DOS refuses mode 1Ch (AX = 421Ch).
- **Behaviour:** the seek fails and the file stays where `xn_world_read_header` left it,
  which is the band table: the read is right.
- **Visible:** to the game's DOS I/O (the boundary compares a service by AX: 421Ch);
  `xn_world_open` records (newgame_0, call_4FDB8).
- **Kept by:** `xn_world_read_height_bands((u8)(u32)name)` and its seek's mode.
- **Status:** kept.

### Q-WORLD-02: the water tile fix's first store lands in the slot's first row

- **Where:** `xn_world_fix_lone_tiles` (0C3C3C).
- **Asm:** `0C3C84 movzx edx, byte [climate]; shl edx, 2; mov al, [edx + bands]; 0C3C94 mov
  [edi + edx], al`: the water line is stored at the slot's square (climate * 4, row 0) before
  the 3 x 3 squares around the water square.
- **Behaviour:** for each water square, square (climate * 4, 0) of the slot also gets the
  water line's height.
- **Visible:** to the game (the height layer: game-visible chunks; the terrain draws it).
- **Kept by:** `heights[xn_world_cell_header.climate * 4] = water;`; records of 0C3C3C and the
  outdoor scenarios.
- **Status:** kept.

### Q-WORLD-03: the non-planar marks read past the height layer

- **Where:** `xn_world_mark_nonplanar_quads` (0C3FCB).
- **Asm:** 256 x 256 squares (`mov edx, 100h` rows, `mov ecx, 100h` columns), each reading
  [esi + 1], [esi + 100h], [esi + 101h]: the last column reads the next row's first square,
  the last row reads the 256 bytes after the layer (the flat layer).
- **Behaviour:** the last row's and column's marks depend on the next row's and the flat
  layer's bytes.
- **Visible:** to the game (the height layer's bit 7 picks the terrain's triangles).
- **Kept by:** the same loop over p[0x100], p[0x101]; records and `test --boundary`.
- **Status:** kept.

### Q-WORLD-04: the tile passes' table lookups include stale register bytes

- **Where:** `xn_world_fix_lone_tiles` (0C3C3C), `xn_world_rock_slopes` (0C3D14).
- **Asm:** `0C3C6D mov al, [esi + ebx]; and al, 3Fh; mov al, [eax + tile_class]` with EAX's
  upper bytes as the caller left them until the first `0C3CCE xor eax, eax`; 0C3D14's square
  offsets are EBX with only its low half cleared.
- **Behaviour:** with a caller's EAX or EBX above 0FFh, lookups and squares far outside the
  tables and the slot.
- **Visible:** no: the only caller, `xn_world_gen_tiles`, leaves EAX = a class 0..3 and EBX
  = 8080h (upper half 0).
- **Kept by:** canonical C indexes by the byte alone.
- **Status:** dropped (unreachable).

### Q-WORLD-05: the world file's own "cannot open" message is unreachable

- **Where:** `xn_world_open` (0C310F).
- **Asm:** `0C3112 call xn_dos_open; 0C3117 jb 0C318B` (shut down, print
  'cannot open', exit): but xn_dos_open never returns with CF: a missing file goes to the
  'DOS:' fatal error and `exit` inside it.
- **Behaviour:** none.
- **Visible:** no (a fatal path no run can reach).
- **Kept by:** canonical C has no branch after `xn_dos_open`.
- **Status:** dropped.

## Terrain (src/engine/terrain.c)

### Q-TERRAIN-01: tiles turned 180 or 270 degrees ignore their u flip

- **Where:** `xn_terrain_draw_cells` (13EFD7): its u-axis routine table
  `xn_terrain_u_axis_fns` (138538).
- **Asm:** the table, by rotation * 2 + flip: 13F4CC, 13F51C, 13F550, 13F4F4, 13F51C, 13F51C,
  13F4F4, 13F4F4: rotations 2 and 3 have the same routine for both flips (the v table,
  138558, differs for every flip).
- **Behaviour:** a ground tile turned twice or three times with its u flip bit set is drawn
  unflipped in u.
- **Visible:** to the game (the screen: outdoor ground).
- **Kept by:** terrain.c's `u_axis_fns[8]`, the same entries; the outdoor scenarios.
- **Status:** kept.

### Q-TERRAIN-02: the face planes leave an edge's z in pick_distance

- **Where:** `xn_terrain_face_plane_a/_b` (13EB21, 13EC22).
- **Asm:** `13EB4E mov [xn_pick_view_y], edx; 13EB54 mov [pick_distance], ecx`: the plane's
  first edge's y and z (camera units << 4) are kept in the shared scratch vectors, and
  0x120290 is the game's pick_distance.
- **Behaviour:** after a terrain frame, pick_distance holds the last face plane's edge z.
- **Visible:** to the game: pick_distance is game-visible (13 object-1 reads); the scenarios
  compare it every frame.
- **Kept by:** `xn_scratch_vecs.a.z = a.z;` (the edge's y and the second edge, private
  scratch, are no longer written).
- **Status:** kept.

## Sky (src/engine/sky.c)

### Q-SKY-01: a rain streak far above the view draws 31 pixels of the wrong colours

- **Where:** `xn_sky_draw_rain_streak` (0C9D19) and the dead `..._bottom_clip` (0C9D63).
- **Asm:** the run is 31 unrolled steps of 9 bytes stopped by a `ret` planted at
  `0C9D89 + 9n` (`0C9D79 lea ecx, [ecx + ecx * 8]; 0C9D82 mov byte [ecx + C9D89h], C3h`);
  above the view `0C9D2B sub edx, [clip_top]; sub esi, edx; add ecx, edx` shortens n. For a
  streak more than 30 rows above the view n < 0: the `ret` lands before the run (code that
  already ran, restored after), so all 31 steps run, from the view's top, with the colour
  pointer moved back past the gradient; an n above 31 (the bottom-clip entry) likewise.
- **Behaviour:** 31 pixels from the view's top in the bytes before xn_rain_streak_colours.
- **Visible:** no game call reaches it: `xn_sky_draw_rain` picks y at most 20 rows above the
  view.
- **Kept by:** `rain_run`'s `if (n < 0 || n > 31) n = 31;`; records group_e_sky
  `sky_rain_streak_-40/-31`, `sky_rain_bottom_clip_40/-3`.
- **Status:** kept.

### Q-SKY-02: a snow flake that starts again is drawn large

- **Where:** `xn_sky_draw_snow` (0C9A89).
- **Asm:** the flake's size class is CX, the loop count (100 down to 1: `0C9AE6 cmp cx, 50h;
  0C9AEC cmp cx, 32h`); a flake that reached the bottom is placed again with `div cx`, CX
  then the clip window's width, so its class is that width (at least 50h: large).
- **Behaviour:** a respawned flake is drawn 2 x 2 and falls at the large speed this frame,
  whatever its index.
- **Visible:** to the game (the screen: the snow scenario).
- **Kept by:** `snow_respawn` returns the width as the size; scenario `snow`, records.
- **Status:** kept.

### Q-SKY-03: the bottom-clip entry's 31-pixel run leaves its planted ret

- **Where:** `xn_sky_draw_rain_streak_bottom_clip` (0C9D63), dead.
- **Asm:** for n = 31 the planted `ret` lands on the restoring instruction itself (0C9EA0):
  the run "returns" to 0C9EA0 (`push C9EA0h` at 0C9D6C), which is now `ret`, and the byte is
  never restored; every later streak then stops at its own planted `ret` and never restores
  it either.
- **Behaviour:** later rain streaks lose pixels, cumulatively.
- **Visible:** no: nothing reaches 0C9D63, and the live entry's n is at most 30.
- **Kept by:** canonical C writes no code; config/xngine_dropped.csv excuses 0C9EA0 (record
  group_e_sky `sky_rain_bottom_clip_31`).
- **Status:** dropped.

## Water (src/engine/water.c)

### Q-WATER-01: a row's x is updated to 16 bits

- **Where:** `xn_water_draw` (12F79C): the S-buffer walk.
- **Asm:** `12F930 mov bx, [esi + 4]` (a span's x_end) and `12F94C mov bx, [esi + 6]; add
  ebx, edx`: only BX is set, EBX's upper half is kept.
- **Behaviour:** none while x stays within 0..65535: x starts at the view's left edge and
  becomes span ends; only a view window whose left edge is negative would carry FFFFh into
  later runs.
- **Visible:** not with the game's view windows.
- **Kept by:** `x = (x & 0xFFFF0000) | node->x_end` and the like.
- **Status:** kept.

### Q-WATER-02: the bottom cut does not clear its outcode bit

- **Where:** `xn_water_clip_extent` (12F59C).
- **Asm:** `12F6D1 and byte [p1.outcode], EFh` after the near cut, `12F705 and ..., F7h` after
  the top cut; after the bottom cut (12F725) no `and`.
- **Behaviour:** none seen: the cut copies the intersection's own outcode, and a point on the
  bottom plane (y = z) has no 'below' bit; the line is reported unseen (CF, 12F798) only if
  an end keeps a top or bottom bit, which no record or scenario reaches (it needs a cut point
  behind the eye).
- **Visible:** no.
- **Kept by:** the same three tests in water.c (the bottom one without a clear).
- **Status:** kept.

## The rasteriser: spans (src/engine/span.c)

### Q-SPAN-01: the lit solid span's shade carries never reach the row's high half

- **Where:** `xn_span_solid_lit` (155920) and its tail `xn_span_solid_lit_tail` (155880);
  `src/engine/span.c` (`xn_span_solid_lit`, `xn_span_solid_lit_tail`).
- **Asm:** 1559B3 `mov eax, ecx` / 1559B8 `mov al, COLOUR` / 1559BA `mov ah, ch` / 1559BC
  `add ecx, ebp`: the pixel's shade-row address takes only bits 8-15 of the running shade (CH);
  its high half stays the block's start shade's. The tail (155880 `mov ah, ch`) is the same.
- **Behaviour:** when the interpolated shade crosses a 64K boundary between two 16-pixel
  samples, the pixels keep the start row's high half: a row 256 rows away (outside the shade
  table, or its first rows) for the rest of the block.
- **Visible:** to the game's screen through solid polygons lit by point lights (lighting
  kind 8). The shade table is 16K-aligned in a 32K block, so the 64 rows never cross a 64K
  boundary unless the table itself does; the records and scenarios show no crossing.
- **Kept by:** `(base & 0xFFFF00FF) | (shade & 0xFF00)` in both loops; 155920's 24 records and
  155880's 25 through their shims; the dungeon scenarios.
- **Status:** kept.

### Q-SPAN-02: the lit textured tail takes its shade at pixel n with the 1/z of pixel n - 1

- **Where:** `xn_span_tex_lit` (156A94); `src/engine/span.c`.
- **Asm:** 156C2B-156C5D: `lea eax, [ebp - 1]` (n - 1) times the polygon's d(1/z)/dx for the
  tail's end depth, while the ray is the column n's (`xn_cam_dir_x_mid[n]`) and u, v are taken
  at pixel n.
- **Behaviour:** the shade interpolated over the last n < 16 pixels ends at a point one pixel
  nearer in depth than its ray.
- **Visible:** the screen, on every lit textured span with a tail (dungeons with torches).
- **Kept by:** `z = Z_OF(inv_z + (n - 1) * poly->inv_z_dx)`; 156A94's 26 records; the scenarios.
- **Status:** kept.

### Q-SPAN-03: the S-buffer compares span ends as 16-bit values and offsets only a low word

- **Where:** `xn_span_insert` (158D04); `src/engine/span.c`.
- **Asm:** 158D0B `cmp dx, bp` and 158D1E `cmp bx, [esi+4]`: a span's end against the new
  span's start and end, as words; on a split, `sub bx, ...` changes only BX's low word before
  the 1/z offset is multiplied out with all of EBX.
- **Behaviour:** ends beyond 32767 or negative ends compare wrongly; a split's depth offset
  uses x1's high word as it was. Screen x is 0..319, so neither happens with the game's spans.
- **Visible:** no in practice (x within the screen); kept for exactness.
- **Kept by:** the `(s16)` compares and the low-word difference; 158D04's 30 records.
- **Status:** kept.

### Q-SPAN-04: a 1/z of 4000h or less gives a lit span z = 0

- **Where:** `xn_span_solid_lit` (155938, 155990: `div ecx` of 2^46 by 1/z).
- **Asm:** a 1/z at or below 4000h makes the quotient overflow: XnGine's divide-error handler
  returns 0 (Q-SYS-01).
- **Behaviour:** the shade is taken at the eye (z = 0) for points beyond 2^26 units.
- **Visible:** no: the view distance is far smaller (the far plane's 1/z is well above 4000h).
- **Kept by:** `xn_udiv64_or0` (src/engine/smc.c).
- **Status:** kept.

## Shade and fog (src/engine/shade.c)

### Q-SHADE-01: with the fog off, the fog start becomes the view distance unshifted

- **Where:** `xn_shade_set_fog` (14D23C); `src/engine/shade.c`.
- **Asm:** 14D2D7 `mov eax, [xn_cam_far_z]` / 14D2DC `mov [xn_fog_start], eax`: the far
  distance in z units, where the fog start is otherwise in z >> 8 units.
- **Behaviour:** after `xn_shade_set_fog(-1)` (or a start at or beyond the view distance)
  xn_fog_start is 256 times farther than any depth in its units.
- **Visible:** to the flats, which fog from xn_fog_start (xn_flat_span_light_setup, 155610):
  with the fog off they never fog. The game reads xn_fog_start nowhere.
- **Kept by:** `xn_fog_start = xn_cam_far_z;`; 14D23C's 29 records and their --boundary test.
- **Status:** kept.

### Q-SHADE-02: a fog start of 0 or 1 gives the fog's inverse start 0

- **Where:** `xn_shade_set_fog` (14D23C) and `xn_shade_fog_span` (150040).
- **Asm:** 14D288-14D28F `div ebx` of 2^32 by the start: it overflows for 0 and 1, and the handler
  makes it 0 (Q-SYS-01).
- **Behaviour:** every span counts as starting beyond the fog start.
- **Visible:** no: the game's starts are 4, 8 and the view distance - 512 (init.c).
- **Kept by:** `xn_udiv64_or0(1, 0, xn_fog_start)` in `xn_shade_fog_span`.
- **Status:** kept.

### Q-SHADE-03: the fogged part of a span is cut by an unwrapped add

- **Where:** `xn_shade_fog_span` (150040).
- **Asm:** `add ebp, edx; jle`: the count plus the (negative) pixels before the fog, tested
  by the flags of the add (SF != OF or ZF), not by the wrapped sum's sign.
- **Behaviour:** a span whose count and offset overflow when added is still taken as empty
  when the exact sum is not positive.
- **Visible:** no in practice (counts are at most 320).
- **Kept by:** `xn_add_lt0(n, h) || n + h == 0` (src/engine/smc.c `xn_add_lt0`, plain C).
- **Status:** kept.

## Lights (src/engine/light.c)

### Q-LIGHT-01: a point light's handle is what the camera's sphere test leaves in EAX

- **Where:** `xn_light_add` (136AD8) / `xn_light_add_regs` (136AF0); `src/engine/light.c`.
- **Asm:** 136B6F `call xn_cam_cull_sphere`: the point light's culling call leaves its last
  value in EAX, which 136AF0 returns; a directional light returns x.
- **Behaviour:** the result is x, or for a point light the sphere test's residue: not a
  handle of anything.
- **Visible:** to the game: func_000830C7 (src/hand) stores it as `object->draw_handle`
  (lines 169 and 175).
- **Kept by:** `xn_light_add` returns the residue (`xn_cam_cull_sphere`'s *residue);
  136AD8's 28 game-called records (`test --boundary`).
- **Status:** kept.

### Q-LIGHT-02: the light count goes up before the full test

- **Where:** `xn_light_add_regs` (136AF0).
- **Asm:** 136AF8 `inc [xn_light_count]` / 136AFE `cmp [xn_light_count], 20h` / 136B05 `jae`.
- **Behaviour:** the 32nd and later adds of a frame fail and still count; a culled light
  takes its count back.
- **Visible:** to the engine's light scans (the count bounds xn_light_to_view); the game
  reads xn_light_count nowhere.
- **Kept by:** `if ((u32)++xn_light_count >= XN_LIGHTS) return x;`; 136AF0's 28 records.
- **Status:** kept.

### Q-LIGHT-04: the light shaders' squares-table index is not bounded

- **Where:** the shader templates 136C30/136C84/136D10; `xn_light_shade` (light.c).
- **Asm:** each template reads `[reg*4 + SQ]`, where SQ is &xn_squares_table_mid[foot point]
  and reg the pixel's view-space coordinate >> 8 (z: z >> 14): no clamp to -4096..4095.
- **Behaviour:** a light or a pixel more than 4096 units (>> 8) from the other reads past the
  8192-entry table.
- **Visible:** the screen, if it happens; the squares table sits before the texture size
  masks and the render tables in object 2.
- **Kept by:** the plain index in `xn_light_shade`; the lit spans' records; the scenarios.
- **Status:** kept.

### Q-LIGHT-05: the third point light ends the face's light list

- **Where:** `xn_light_add_point` (15BF75), `xn_light_setup_poly` (15BC42).
- **Asm:** 15C035 `add ebp, 4` / 15C038 `cmp ebp, 0Ch` / 15C03F `pop eax; jmp
  xn_light_build_shader_3`: the handler leaves the dispatch loop for good.
- **Behaviour:** the lights after the third point light that reaches the face, directional
  ones included, do not light it.
- **Visible:** the screen (faces near several torches).
- **Kept by:** `return xn_light_build_shader(poly, row, pts, 3)` inside the loop; 15BF75's 28
  records through its shim (test-only `lightp_t.asm`: the asm's frame skip), 15BC42's records.
- **Status:** kept.

### Q-LIGHT-06: a polygon with no depth slope gets an inverse slope of 0

- **Where:** `xn_light_setup_poly` (15BC53), `xn_light_setup_terrain` (15BCA6).
- **Asm:** `idiv ebx` of 2^32 by the polygon's d(1/z)/dx: 0 faults and the handler gives 0
  (Q-SYS-01), about ten times a frame (walls facing the eye).
- **Behaviour:** +60h is 0, which the fog takes as "the same depth along the span".
- **Visible:** the screen, through the fog.
- **Kept by:** `xn_idiv64_or0(1, 0, poly->inv_z_dx)`.
- **Status:** kept.

### Q-LIGHT-07: the compiled shaders' bytes in big_buffer reach the game

- **Where:** the shader builders `xn_light_build_shader_1/2/3` (15BCF6, 15BD78, 15BE4D),
  which copy a patched template to `xn_light_code_next` (big_buffer, reset every frame);
  the game's `player_movement_update` (src/lifted/intrface.c line 648).
- **Asm:** 15BD69 / 15BF66 `rep movsd` into big_buffer. The game keeps a pointer to a
  collision hit in big_buffer (D_00195CD8, set in colstuff.c) and copies 30 bytes through it
  every frame (`mc_memcpy(D_00195E30, D_00195CD8, 30)`), after the frame's shaders have
  overwritten that part of big_buffer.
- **Behaviour:** the game's copy of its ground hit (D_00195E30) holds 30 bytes of a light
  shader's machine code.
- **Visible:** to the game: in walk_mord every frame, 30 bytes at big_buffer + 4..+21h last
  written by 15BF66 are read by the game's memcpy (build/xn_canon/group_c/tools/shader_flow.py,
  who_reads.py).
- **Kept by:** canonical C evaluates shaders from a C record, and still writes the asm's image
  of each shader (the template's bytes, a const array in light.c, with its operands) at
  xn_light_code_next as data that nothing runs (`write_asm_image`). The light builders'
  records and the scenarios compare it.
- **Status:** kept.

### Q-LIGHT-08: the terrain's base row keeps the ambient level's fraction, and a negative level leaves cells unlit

- **Where:** `xn_light_setup_terrain` (15BCAB-15BCBD); compare `xn_render_begin_frame`
  (the model faces' and flats' ambient row).
- **Asm:** 15BCAB `mov eax, [xn_light_ambient]` / 15BCB0 `cmp eax, 3F00h; jae` (unsigned) /
  15BCB7 `add eax, [xn_shade_table]`: the level as the game set it, not masked to whole rows
  or clamped as 12A4F0 does for the faces.
- **Behaviour:** a terrain cell's row has the level's low byte as its fraction; a negative
  level is "fully lit" (kind 0) for the terrain while the faces clamp it to row 0.
- **Visible:** the screen outdoors, when the game's ambient level has a fraction or is below
  0.
- **Kept by:** `xn_light_setup_terrain`; 15BC9C's 26 records; the outdoor scenarios.
- **Status:** kept.

## The renderer (src/engine/render.c, rframe.c)

### Q-RENDER-01: the outline mode marks the pixel after a span

- **Where:** `xn_render_span_mark_ends` (12A860); `src/engine/render.c`.
- **Asm:** 12A865 `mov [edi + 1], al` / 12A868 `mov [edi + ebp + 1], al`: the span's first
  pixel and the pixel after its last.
- **Behaviour:** the right mark lies one pixel right of the span (the next span overdraws it,
  or the background).
- **Visible:** in render mode 0 (outlines), and in mode 8 for texture kinds 8 and 12; the game
  selects mode 8 only.
- **Kept by:** `pix[n] = text_colour`; 12A860's 6 records.
- **Status:** kept.

### Q-RENDER-02: the draw lists' quicksort resumes its right part where its left part stopped

- **Where:** `xn_render_sort_pairs_range` (15811C); `src/engine/render.c`.
- **Asm:** 158168-15817D: the recursive calls keep ESI (the forward scan's position) as the
  callee left it: the right part `[esi, hi]` starts where the left part's own sort stopped,
  not where this partition's scan did.
- **Behaviour:** the result is still sorted, but the recursion (and so the order of equal
  keys) differs from a textbook quicksort.
- **Visible:** the order the game sees models and flats drawn in (painter's order for equal
  distances): the screen.
- **Kept by:** `i = xn_render_sort_pairs_range(pairs, lo, j)` feeding the right part;
  15811C's 28 records, the model and flat passes' records, the scenarios.
- **Status:** kept.

### Q-RENDER-03: the pick's span test counts the span's end and offsets only x's low word

- **Where:** `xn_render_pick` (12A608); `src/engine/render.c`.
- **Asm:** 12A667 `cmp ax, [esi+6]` / 12A66D `cmp [esi+4], ax`: 16-bit compares with x_end
  inclusive; 12A676 `sub ax, [esi+6]`.
- **Behaviour:** a click on a span's end pixel (which the next span draws) picks this span's
  polygon; the depth comes from x's low word.
- **Visible:** to the game (the pick's result and pick_distance, xn_pick_view_x/y).
- **Kept by:** the `(s16)` compares and the low-word offset; 12A608's 24 records (--boundary).
- **Status:** kept.

### Q-RENDER-05: the frame draws the rows symmetric about the view centre

- **Where:** `xn_render_frame` (12A8B6-12A8C3, 12A963); `src/engine/rframe.c`.
- **Asm:** the row loop runs xn_render_row_y from clip_top - centre_y up to -(that): the end
  is the row as far below the centre as the clip top is above it, not the clip bottom.
- **Behaviour:** with a clip window not centred on the view centre, rows past the clip bottom
  are drawn, or rows above it left out.
- **Visible:** the game's windows are centred (160, 100 / 160, 77 with the clip matching).
- **Kept by:** `end_row = -xn_render_row_y`; 12A870's 28 records; the scenarios.
- **Status:** kept.

### D-RENDER-01 (a deviation): a pick outside the clip window unbalances the asm's stack

- **Where:** `xn_render_pick` (12A608, 12A611..12A635 jumping to 12A6C3).
- **Asm:** outside the clip window the asm jumps to 12A6C3 `pop eax`, which pops the saved ESI
  as the flat pick's result, then pops ESI, ECX, EBX one slot off and returns through the
  caller's stack: a crash or a jump into the caller's data.
- **Behaviour:** canonical C returns 0 (no polygon).
- **Visible:** engine_pick_object (engsupp.c) checks only y > the view's bottom (199 or the
  HUD's top row), so a click on the HUD's top row with the HUD shown (y = clip bottom) reaches
  it; nothing in the records or scenarios does.
- **Kept by:** not kept: no C can return as the asm does there.
- **Status:** dropped (a crash in the asm).

## The texture mapper (src/engine/tmap.c)

### Q-TMAP-01: the mapper count goes up before the full test

- **Where:** `xn_tmap_compile` (15C274); `src/engine/tmap.c`.
- **Asm:** 15C281 `inc [xn_tmap_pool_count]` / 15C287 `cmp ..., 300h` / 15C291 `jae`.
- **Behaviour:** the 768th compile and every later one fail (xn_tex_cache_full) without the
  count coming back; slot 767 is never handed out.
- **Visible:** to the game: a full texture cache makes xn_render_frame return 1.
- **Kept by:** `slot = xn_tmap_pool_count++` before the test; 15C274's 26 records.
- **Status:** kept.

## Camera (src/engine/cam.c)

### Q-CAM-01: the dead in-place scale scales m[2][2] where m[1][2] belongs

- **Where:** `xn_cam_scale_matrix_in_place` (13742B), dead. Canonical: cam.c.
- **Asm:** the sixth product reads and writes `[esi + 20h]` (137476..13747F), m[2][2], where
  the row-1 pattern of the five before it (+0, +4, +8, +0Ch, +10h) wants +14h.
- **Behaviour:** rows 0 and 1 are scaled by the view's x and y scales except m[1][2], which
  stays; m[2][2] is scaled by the y scale.
- **Visible:** no caller.
- **Kept by:** `m->m[2][2] = xn_mulshr(m->m[2][2], xn_cam_scale_y, 14);` (marked); its
  record (`test xn_cam_scale_matrix_in_place`).
- **Status:** kept.

### Q-CAM-02: the sphere test's leftover reaches the game through xn_light_add

- **Where:** `xn_cam_cull_sphere` (15CF18) and its helpers `xn_cam_sphere_dist_x/_y`
  (15D03C, 15D05C). Canonical: cam.c.
- **Asm:** the test returns CF; EAX is whatever it computed last: the low dword of
  `2y * inv_scale_y` (15CF72) when the centre is inside the view, the near or far plane's
  difference (15CF9B, 15CFB3), or the low dword of `nz * pick_distance` from a side plane's
  distance (15D047). `xn_light_add` (136AD8) returns it in EAX when it drops a light or keeps a
  point light, and the game stores it (object->draw_handle, func_000830C7).
- **Behaviour:** the game's light handle is that leftover.
- **Visible:** to the game, through `xn_light_add` (10 game sites; `game_reads` has EAX).
- **Kept by:** the `*residue` out-parameter of `xn_cam_cull_sphere`, `xn_cam_sphere_dist_x/_y`'s
  `*low`; C's `xn_light_add` passes it on. Records (cull_sphere 28; sphere_dist 2 crafted:
  group_d_cam), `equiv_d.py xn_cam_cull_sphere` (2 million samples) and the side planes' specs.
- **Status:** kept.

## Polygons (src/engine/poly.c)

### Q-POLY-01: a model face inside the view is walked with byte-sized counters

- **Where:** `xn_poly_project_face` (158420), the path for a face whose vertices are all inside
  the view (its outcodes' OR is 0). Canonical: poly.c (`walk_projected`).
- **Asm:** 15855B `add ecx, 3FFh`: CL counts the vertices after the first (`dec cl; jne`) and
  CH steps the ring's byte offset by 4 (`add ch, 4`); 158594 `shr ecx, 0Ah` picks the ring of
  CH / 4 vertices, DL the top vertex's byte offset.
- **Behaviour:** for a face of n points the ring is that of n mod 64 points and the counters
  wrap: a face of 1 point copies 256 more, one of 0 points 255 more (CH starting at 3).
- **Visible:** no: prepare rejects faces of more than 24 points and the data has none below 3;
  the C keeps the counters as the asm has them.
- **Kept by:** `walk_projected` (the `u8` counters from `n + 3FFh`); the face records.
- **Status:** kept.

### Q-POLY-02: the edge walker counts the vertices in a signed byte

- **Where:** `xn_poly_rasterize` (15B9A0) and the flats' `xn_flat_raster` (1552A0), through
  `xn_poly_vertex_count` (15B984). Canonical: poly.c `xn_walk_left_edge/_right_edge`.
- **Asm:** `dec byte ptr [15B984h]; js` before each edge (15B9B7, 15BA5F; 1552D3, 155394).
- **Behaviour:** a polygon of 128 to 255 vertices ends its walk at once (the count is
  negative); one of 0 walks 255 edges.
- **Visible:** no: the clipper's buffers hold 32 vertices, a face is dropped from 28
  (Q-POLY-03), a terrain cell has 3 or 4.
- **Kept by:** `xn_edge_walk.count` (`s8`), set from the count the projectors pass.
- **Status:** kept.

### Q-POLY-03: a clipped face of 28 or more vertices is dropped

- **Where:** `xn_poly_project_face` (158420), the clipped path. Canonical: poly.c.
- **Asm:** 1584E9 `cmp cl, 1Ch; jae 1585B2` after the projection.
- **Behaviour:** such a face adds no spans (the projector has already written its vertices).
- **Visible:** in principle (a 24-point face clipped by several planes); not seen in the
  records or the scenarios.
- **Kept by:** `if ((u8)n >= 0x1C) return 0;` (marked).
- **Status:** kept.

## Flats (src/engine/flat.c)

### Q-FLAT-01: the first visible piece of every flat is not drawn

- **Where:** `xn_flat_draw` (154E20) installs `xn_flat_span_light_setup` (155610) as the flat's
  span routine; `xn_flat_span_emit` (15526C) calls it for the first piece. Canonical: flat.c.
- **Asm:** 154ECD `mov [edi+3Ch], 155610h`; 155610 stores the real routine in `[esi+3Ch]` and
  returns without drawing (docs/xngine_map.md, "Unsure" 3).
- **Behaviour:** each flat loses its first visible run of pixels (its top row's leftmost
  piece).
- **Visible:** to the game: the screen (every scenario with flats).
- **Kept by:** `xn_flat_span_light_setup` draws nothing; `xn_flat_span_emit` calls the flat's
  routine once a piece. The scenarios (town_crowd, night, fight...).
- **Status:** kept.

### Q-FLAT-02: a flat that is not queued returns its scale argument

- **Where:** `xn_flat_add` (154D00) / `xn_flat_add_body` (154D20). Canonical: flat.c.
- **Asm:** 154DA2 `pop ecx; pop edi` gives back the pushed scale argument in EDI, which 154D13
  `mov eax, edi` returns.
- **Behaviour:** for a flat behind the eye, beyond the far plane or past the 512th of a frame,
  the result is the scale argument (with its light byte), not 0.
- **Visible:** to the game: it stores the result as the object's draw handle (8 sites of 14).
- **Kept by:** `return scale;` (marked); `test --boundary xn_flat_add`.
- **Status:** kept.

### Q-FLAT-03: a flat of a kind without a quad draws the last flat's quad

- **Where:** `xn_flat_draw` and `xn_flat_pick` through `xn_flat_quad_table` (153C00): kinds 2-3
  and 8-16 are `xn_flat_quad_none*` (154FD4, 1550BC, 1550C0), a lone `ret`. Canonical: flat.c
  `build_quad`.
- **Asm:** 154E81 `call [ebx*4 + 153C00h]`; 154E88 tests the clipper's AND, which the last
  quad left.
- **Behaviour:** such a flat is projected with the clipper's buffer and outcodes as the last
  quad built left them (or skipped when that one was outside a plane).
- **Visible:** to the screen if the game queued such kinds; the records and scenarios show
  kinds 1 and 4 only.
- **Kept by:** `build_quad`'s default case does nothing.
- **Status:** kept.

### Q-FLAT-04: a directional light is added again and again until the shade row is the last

- **Where:** `xn_flat_span_light_setup` (155610). Canonical: flat.c.
- **Asm:** 1556DB-1556E9: `add ebp, intensity << 8; cmp ebp, last; jl 155654`, back to the
  same light's type test without stepping EDI or counting ECX down.
- **Behaviour:** one directional light makes any flat unlit (the last row: no shading at all,
  or the fog's row past the fog's start).
- **Visible:** to the screen (outdoors the sun is a directional light).
- **Kept by:** the inner `do ... while (row < last)` (marked).
- **Status:** kept.

### Q-FLAT-05: the pick reads a missing image's header at address 0

- **Where:** `xn_flat_pick` (155508). Canonical: flat.c.
- **Asm:** 155552 `call xn_tex_cache_lookup_image` then 155557.. reads `[eax+0Eh]`, `[eax+6]`,
  `[eax+4]` without testing EAX for 0 (the cache full).
- **Behaviour:** with the cache full the quad's size comes from the bytes at linear 4 and 6
  (the real-mode interrupt table).
- **Visible:** to the pick's result (rare: the cache is full only until the game flushes it).
- **Kept by:** `image->width`, `image->height` with image 0 (marked).
- **Status:** kept.

### Q-FLAT-06: kinds 17 to 31 read past the quad table

- **Where:** `xn_flat_draw`, `xn_flat_pick`: `xn_flat_quad_table` has 17 entries; `flags &
  1Fh` indexes 32.
- **Asm:** 154E7B `and ebx, 1Fh`; 154E81 `call [ebx*4 + 153C00h]`: entries 17-31 are the flat
  sort list (153C44: keys and flat addresses).
- **Behaviour:** the asm would call into data (a crash).
- **Visible:** no: the game passes kinds 1 and 4 (and 0-31 never beyond 16 in the records,
  scenarios or game code: objlib.c 1, args.c 4, func_000830C7 4/32|4/1/n with n from the
  flat's own kind).
- **Kept by:** nothing: `build_quad` builds no quad for them (as kinds 2-3, 8-16).
- **Status:** dropped.

## Models (src/engine/model.c)

### Q-MODEL-01: the dead x-z extent starts its minimum and maximum swapped

- **Where:** `xn_model_xz_extent` (0CE828), dead. Canonical: model.c.
- **Asm:** 0CE833-0CE842: the minima start at -100000 (FFFE7960h), the maxima at 100000.
- **Behaviour:** the extents are at least 200000 >> 8; points within +-100000 change nothing.
- **Visible:** no caller.
- **Kept by:** the swapped starting values (marked); record group_d_model `xz_extent_wide`.
- **Status:** kept.

### Q-MODEL-02: a count of 0 runs 2^32 times

- **Where:** the model loops `dec; jne` / `loop` over points, faces and face points
  (`xn_model_max_y`, `_xz_extent`, `_prepare`, `_calc_*`, `_transform_face_verts`,
  `_draw_faces`, `_is_occluded`'s rows). Canonical: model.c (`do ... while (--n != 0)`).
- **Behaviour:** a model with no points or faces (or a face of 0 points) runs off its data.
- **Visible:** no: ARCH3D models have points and faces.
- **Kept by:** the `do`-`while` loops.
- **Status:** kept.

### Q-MODEL-03: preparing a model leaves a face's edge in the pick's point

- **Where:** `xn_model_calc_face_uv_axes` (13FF65) and `xn_model_calc_face_normal` (140845),
  under `xn_model_prepare` (13FE15, a game entry). Canonical: model.c.
- **Asm:** 13FF7E.. and 140867.. store the first edge in `xn_pick_view_x/_y` and
  `pick_distance` (the shared scratch vector a, 120288) and work on it there.
- **Behaviour:** after a model is prepared, `pick_distance` holds the last face's edge z
  (scaled when it had texture axes).
- **Visible:** `xn_pick_view_x` and `pick_distance` are game-visible (config/xngine_boundary.csv:
  13 object-1 references to pick_distance).
- **Kept by:** the edge kept in `xn_scratch_vecs.a` (marked); the second edge and the
  scratch scalars are locals (dropped rows: private).
- **Status:** kept.

### Q-MODEL-04: the occlusion test offsets x in 16 bits

- **Where:** `xn_model_is_occluded` (140910). Canonical: model.c.
- **Asm:** 140955 `sub ax, [esi+6]; je; js`: only AX is offset by the span's start; the
  product 140960 `imul eax, [ebp+5Ch]` takes all of EAX.
- **Behaviour:** an x with a high word (never: screen x) would keep it in the depth estimate.
- **Visible:** no (screen coordinates); kept as the asm computes it.
- **Kept by:** `(x & 0xFFFF0000) | (u16)(x - node->x_start)` (marked).
- **Status:** kept.

### Q-MODEL-05: the back-face test's last add is compared without its wrap

- **Where:** `xn_model_draw_faces` (1403AF). Canonical: model.c.
- **Asm:** 140409-140424: `n.x * eye.x + plane_d` and `n.z * eye.z + n.y * eye.y` are 32-bit
  sums; `add eax, ecx; jge` tests the last add's true sign (SF = OF).
- **Behaviour:** when the last add overflows, the face is front-facing by the true sum's sign,
  not the wrapped one; the wrapped sum is the 1/z gradient's divisor.
- **Visible:** in principle (huge eye offsets); not in the records.
- **Kept by:** `sum_negative(a, b)` (marked) and the wrapped divisor.
- **Status:** kept.

### Q-MODEL-06: a model of 1024 or more vertices overwrote the vertex-flag clear's code

- **Where:** `xn_model_draw_faces` (1403AF) and the unrolled clear `xn_model_clear_vert_flags`
  (140A28, 1024 steps of 6 bytes).
- **Asm:** 1403BE plants `C3h` at 140A28 + 6n and 1403CA writes `88h` back there: for n = 1024
  that is the body's own `ret` (142228), for n > 1024 the code after it.
- **Behaviour:** the asm corrupts its own code for such a model (the next draw crashes).
- **Visible:** no: the vertex arrays hold 1024 vertices and no ARCH3D model comes near (the
  check of prepare covers "v2.5" files only).
- **Kept by:** nothing: the C clears at most 1024 flags (`xn_model_clear_vert_flags`).
- **Status:** dropped.

## Texture cache (src/engine/tex.c)

### Q-TEX-01: an eviction never reports failure

- **Where:** `xn_tex_heap_evict` (136145), `xn_tex_heap_alloc` (1360EE). Canonical: tex.c.
- **Asm:** 136170 `stc` falls into 136171 `clc; ret`; 136107 `jb 13611D` (the 'SET: Out of
  memory in find_memory.' fatal exit) is never taken.
- **Behaviour:** when nothing is left to evict, the first fit just fails and the cache is
  marked full; the fatal exit cannot run.
- **Visible:** to the game: a full cache instead of the end of the program.
- **Kept by:** `xn_tex_heap_evict` is `void`; `xn_tex_heap_alloc` has no fatal path. Records
  group_d_tex `load_no_room` (nothing evictable).
- **Status:** kept.

### Q-TEX-02: the least recently used search tests an archive's use count at a byte offset

- **Where:** `xn_tex_heap_find_lru` (136173). Canonical: tex.c.
- **Asm:** 136194 `sub ebx, xn_tex_archives; shr ebx, 2` (the archive number), then
  13619D `test word ptr [ebx + xn_tex_archive_use]`: the number as a byte offset into a table
  of words.
- **Behaviour:** archive n's eligibility reads the high byte of count n/2 and the low byte of
  count n/2 + 1 (or count n/2 for even n): an archive in use may be evicted, an unused one
  kept.
- **Visible:** to the game (which archives stay loaded; the heap is game-visible).
- **Kept by:** `XN_AT(u16, xn_tex_archive_use, archive)` (marked).
- **Status:** kept.

### Q-TEX-03: the texture path is not terminated after the file name

- **Where:** `xn_tex_load_archive` (135EAB). Canonical: tex.c.
- **Asm:** 135EF8-135F00 copies "texture.NNN" up to its 0 and does not copy the 0; a missing
  backslash is added (135EEF), reading the byte before the path when the configured path is
  empty.
- **Behaviour:** the path in `xn_tex_path` ends where an earlier, longer path left a 0.
- **Visible:** `xn_tex_path` is game-visible (the game reads 22 bytes of it).
- **Kept by:** the copy loops (marked); records group_d_tex `load_no_backslash`.
- **Status:** kept.

### Q-TEX-04: a transparent pixel in an image's last column is missed

- **Where:** `xn_tex_check_transparent` (135FCF). Canonical: tex.c.
- **Asm:** 136011 `repne scasb` over the row's width, 136013 `test ecx, ecx; jne`: a 0 in the
  last column also leaves ECX 0.
- **Behaviour:** such an image is compiled (a mapper, kind 4) although it has a colour 0.
- **Visible:** to the screen (the image is drawn opaque).
- **Kept by:** `if (k < width - 1)` (marked).
- **Status:** kept.

### Q-TEX-05: a caller's frame is not checked against the image's frames

- **Where:** `xn_tex_cache_lookup` (135D00). Canonical: tex.c.
- **Asm:** 135D76 `test ebx, ebx; jns 135D9D`: a frame of 0 or more indexes the frame table
  (135DA1 `mov esi, [esi + ebx*4]`) as it is.
- **Behaviour:** a frame past the image's last reads past its frame table.
- **Visible:** to the game for its flats' frames (they stay within the images' frames).
- **Kept by:** `image->frame_offsets[frame]` (marked).
- **Status:** kept.

### Q-TEX-06: the frames the animation clock picks share one decoded frame a game frame

- **Where:** `xn_tex_cache_lookup` (135D00) and `xn_tex_unpack_alloc` (1362CF). Canonical:
  tex.c.
- **Asm:** 135D19 keeps the caller's frame in `xn_tex_cur_frame`; the clock's frame (135D7A..)
  is computed in EBX only; 1362CF..1362EB make the key from `xn_tex_cur_frame & 0FFFFh`.
- **Behaviour:** every lookup by the clock (frame -1) keys its decode as frame 0FFFFh: if the
  clock moves on within a game frame, the later lookups get the first decode.
- **Visible:** to the screen, for a tick within a frame.
- **Kept by:** the key from the caller's frame (marked).
- **Status:** kept.
