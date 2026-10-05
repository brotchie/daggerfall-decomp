/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00027717 */
#include "records.h"

extern char D_001707AE[];
extern char *D_00196DB4;
extern int func_000A0DF4(char *);
extern void mc_strncpy(char *, char *, int, char *, int);

void func_00027717(struct record *a1, char *a2)
{
    char *e;

    e = D_00196DB4;
    while (e[2] != 0)
        e += func_000A0DF4(e + 2) + 3;
    *(short *)e = a1->id & 65535;
    mc_strncpy(e + 2, a2, 4, D_001707AE, 506);
}
