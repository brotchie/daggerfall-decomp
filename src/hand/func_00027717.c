/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00027717 */
#include "records.h"

extern char D_001707AE[];
extern char *automap_notes;
extern int strlen(char *);
extern void mc_strncpy(char *, char *, int, char *, int);

void automap_add_note(struct record *a1, char *a2)
{
    char *e;

    e = automap_notes;
    while (e[2] != 0)
        e += strlen(e + 2) + 3;
    *(short *)e = a1->id & 65535;
    mc_strncpy(e + 2, a2, 4, D_001707AE, 506);
}
