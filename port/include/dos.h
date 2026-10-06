/* dos.h for the native build (docs/port.md): the parts of Watcom 10.0a's <dos.h> the game
   uses. struct find_t keeps Watcom's layout, with `unsigned long size` as a 32-bit int. */
#ifndef PORT_DOS_H
#define PORT_DOS_H

#include "i86.h"

struct find_t {
    char reserved[21];          /* the native build keeps its search state here */
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned int size;
    char name[13];
};

#endif
