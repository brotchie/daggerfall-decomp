/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003C610 */
#include "records.h"

extern char D_00195B84[];
extern char D_00195C44[];
extern signed char D_001962A2;
extern char D_001962A7;

void func_0003C610(struct record *a1)
{
    struct disease *l_18;

    if (a1->type != 11) return;
    l_18 = &a1->data.disease;
    if (l_18->id >= 128 && D_001962A7 == 0 && l_18->stage != 0) {
        D_001962A2 = 1;
        (*(char **)D_00195C44)[*(int *)D_00195B84 + 60000] = 117;
        (*(int *)D_00195B84)++;
        D_001962A7 = 1;
    } else if (l_18->id < 100) {
        D_001962A2 = 1;
        if ((a1->flags & 0x8000) && l_18->stage != 0) {
            (*(char **)D_00195C44)[*(int *)D_00195B84 + 60000] = l_18->id + 100;
            (*(int *)D_00195B84)++;
        }
    }
}
