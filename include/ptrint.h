/* ptrint.h: integers that hold addresses. The game keeps many pointers in ints. Watcom's
   int is as wide as its pointers; the native build's pointers are 64 bits (docs/port.md). */
#ifndef PTRINT_H
#define PTRINT_H
#ifdef DAGGER_PORT
typedef long iptr;            /* an int that holds an address */
typedef unsigned long uptr;   /* an unsigned that holds an address */
#else
typedef int iptr;
typedef unsigned uptr;
#endif

/* Frame slots (tools/port_slots.py). Watcom gives every local and parameter a 4-byte frame
   slot, and the game reaches some narrow ones through a wider type: `*(int *)&x = v` on a
   short, `*(char **)&x` on a short that holds an address. The matching build keeps the
   declared type (Watcom's frame layout follows the declarations); natively a slot type is
   as wide as the widest access. A direct read of one is cast to its Watcom type,
   `(short)x`: a no-op under Watcom, natively the low bits a word load gives. A 4-byte
   access that holds an address is `*(iptr *)&x`. */
#ifdef DAGGER_PORT
typedef int slot16;           /* a short reached as an int */
typedef unsigned uslot16;     /* an unsigned short reached as an int */
typedef int slot8;            /* a signed char reached as an int */
typedef unsigned uslot8;      /* an unsigned char reached as an int */
typedef iptr pslot16;         /* a short that holds an address (`*(char **)&x`) */
typedef uptr upslot16;        /* an unsigned short that holds an address */
#else
typedef short slot16;
typedef unsigned short uslot16;
typedef signed char slot8;
typedef unsigned char uslot8;
typedef short pslot16;
typedef unsigned short upslot16;
#endif

/* Three int locals that one 12-byte access fills as a struct vec3 (sky_update): Watcom's
   frame keeps them side by side (tools/w10_frame.py), clang's need not, so natively each is
   stored on its own. */
#ifdef DAGGER_PORT
#define VEC3_LOCALS_SET(x, y, z, src) \
    ((x) = ((int *)(src))[0], (y) = ((int *)(src))[1], (z) = ((int *)(src))[2])
#define VEC3_LOCALS_MEMCPY(x, y, z, src, file, line) VEC3_LOCALS_SET(x, y, z, src)
#else
#define VEC3_LOCALS_SET(x, y, z, src) (*(struct vec3 *)&(x) = *(struct vec3 *)(src))
#define VEC3_LOCALS_MEMCPY(x, y, z, src, file, line) mc_memcpy(&(x), (src), 12, (file), (line), 4)
#endif
#endif
