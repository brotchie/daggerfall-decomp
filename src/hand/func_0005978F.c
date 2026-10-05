/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005978F */
#include "records.h"

extern char D_0017573C[];
extern struct character *player_character;
extern unsigned char *D_00199B4C;
extern unsigned char **D_00199B50;
extern int mc_memmove();

/* item +0x42 (the top byte of `message`) is the paperdoll drawing order here */
void func_0005978F(struct item *a1, int a2)
{
    int l_18;
    int l_14;

    l_18 = 0;
    if (a1->group == 3) {
        l_14 = a1->inventory_image >> 7;
        if (l_14 == 432 || l_14 == 433) {
        } else if (player_character->flags & 1) {
            a1->inventory_image -= 128;
        }
        if (a2 == 19) {
            if (a1->item_flags & 4)
                a1->inventory_image++;
        } else if (a2 == 21) {
            if (a1->item_flags & 4)
                ((unsigned char *)a1)[66] += 5;
        }
    }
    while (D_00199B50[l_18] != 0 && ((unsigned char *)a1)[66] > D_00199B50[l_18][66])
        l_18++;
    mc_memmove(&D_00199B4C[l_18 + 1], &D_00199B4C[l_18], 27, D_0017573C, 201, 4);
    mc_memmove(&D_00199B50[l_18 + 1], &D_00199B50[l_18], 108, D_0017573C, 202, 4);
    D_00199B50[l_18] = (unsigned char *)a1;
    D_00199B4C[l_18] = a2;
}
