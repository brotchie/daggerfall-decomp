/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039E5F */
#include "records.h"
#include "clib.h"

extern char D_00170B13[];
extern char D_00170B69[];
extern char *scratch_buffer;
extern iptr disk_read_file(char *, char *);

char **spells_std_names_for_ids(char *keys)
{
    char **list;
    char *str;
    struct spell *tbl;
    short i;
    int j;
    short n;
    short cnt;

    n = memchr(keys, 255, 1000) - keys;
    list = (char **)(scratch_buffer + 20000);
    str = scratch_buffer + 21000;
    tbl = (struct spell *)scratch_buffer;
    disk_read_file(D_00170B69, scratch_buffer);
    for (cnt = i = 0; i < n; i++) {
        j = 0;
        while (j < 128) {
            if (tbl[j].name[0] != 0 && (char)tbl[j].id == keys[i])
                break;
            j++;
        }
        mc_strncpy(str, tbl[j].name, 4, D_00170B13, 1625);
        list[cnt++] = str;
        str += strlen(str) + 1;
    }
    list[cnt] = 0;
    if (cnt == 0)
        return 0;
    return list;
}
