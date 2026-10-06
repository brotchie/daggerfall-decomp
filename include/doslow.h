/* doslow.h: real-mode memory the game reads by address: the BIOS data area (the tick count
   at 0x46C) and VGA memory at 0xA0000. Under Watcom, DOS maps them at those flat addresses;
   in the native build (docs/port.md) they are the virtual PC's low memory
   (port/include/port_vpc.h). Written by tools/port_lowmem.py. */
#ifndef DOSLOW_H
#define DOSLOW_H

#ifdef DAGGER_PORT
extern unsigned char *port_low_memory;
#define DOS_LOW(addr) ((void *)(port_low_memory + (addr)))
#else
#define DOS_LOW(addr) ((void *)(addr))
#endif

#endif
