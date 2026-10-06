/* dos.h for the native build (docs/port.md): the parts of Watcom 10.0a's <dos.h> the game
   uses. struct find_t keeps Watcom's packed 43-byte layout (the DTA of DOS's find first and
   find next), with `unsigned long size` as a 32-bit int. */
#ifndef PORT_DOS_H
#define PORT_DOS_H

#include "i86.h"

#pragma pack(push, 1)
struct find_t {
    char reserved[21];          /* the native build keeps its search state here */
    char attrib;                /* _A_SUBDIR 0x10, _A_ARCH 0x20 ... */
    unsigned short wr_time;     /* DOS time: hour << 11 | minute << 5 | second / 2 */
    unsigned short wr_date;     /* DOS date: (year - 1980) << 9 | month << 5 | day */
    unsigned int size;
    char name[13];              /* 8.3, upper case */
};
#pragma pack(pop)

#define _A_NORMAL 0x00
#define _A_RDONLY 0x01
#define _A_HIDDEN 0x02
#define _A_SYSTEM 0x04
#define _A_VOLID 0x08
#define _A_SUBDIR 0x10
#define _A_ARCH 0x20

#endif
