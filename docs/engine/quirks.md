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
