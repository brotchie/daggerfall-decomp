/* doslow.h: real-mode memory the game reads by address: the BIOS data area (the tick count
   at 0x46C) and VGA memory at 0xA0000. Under Watcom, DOS maps them at those flat addresses;
   in the native build (docs/port.md) they are the virtual PC's low memory
   (port/include/port_vpc.h). Written by tools/port_lowmem.py. */
#ifndef DOSLOW_H
#define DOSLOW_H

/* DOS_NULL(p): a pointer the game reads through before it is set. Under the DOS extender a
   null pointer reaches linear 0, the real-mode interrupt table (zeros in tools/fallemu.py);
   natively the virtual PC's low memory, whose first KB is zeros too. */
#ifdef DAGGER_PORT
extern unsigned char *port_low_memory;
#define DOS_LOW(addr) ((void *)(port_low_memory + (addr)))
#define DOS_NULL(p) ((p) != 0 ? (p) : (__typeof__(p))(void *)port_low_memory)
#else
#define DOS_LOW(addr) ((void *)(addr))
#define DOS_NULL(p) (p)
#endif

#endif
