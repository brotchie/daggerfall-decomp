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
