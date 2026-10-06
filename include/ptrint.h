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
#endif
