/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000595DF */
#include "records.h"

extern int D_00195B74;
extern int D_00195B80;
extern char D_00195C44[];
extern char D_001AA600[];
extern int func_000C0700();
extern int func_000CD262();
extern int func_000CD291();
extern int func_00135D00();
extern int func_00135E39();

void paperdoll_draw_item(struct item *a1, int a2, int a3, int a4)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    short l_10;
    short l_C;

    D_00195B80 = (int)(*(char **)D_001AA600 + (a1->color << 8));
    l_24 = func_00135D00(a1->inventory_image >> 7, (int)(unsigned short)(a1->inventory_image & 127), -1);
    if (l_24 == 0) {
        func_00135E39();
        l_24 = func_00135D00(a1->inventory_image >> 7, (int)(unsigned short)(a1->inventory_image & 127), -1);
    }
    l_28 = *(int *)((char *)l_24 + 12);
    if (a1->enchantments[0].type == 26 && a1->enchantments[0].param == 6) {
        func_000C0700(((int)(short)*(short *)((char *)l_28)) + a2, ((int)(short)*(short *)((char *)l_28 + 2)) + a3, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), (int)(unsigned short)(*(short *)((char *)l_28 + 8) | 32768), l_28 + *(int *)((char *)l_28 + 14));
    } else {
        func_000CD291(((int)(short)*(short *)((char *)l_28)) + a2, ((int)(short)*(short *)((char *)l_28 + 2)) + a3, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), l_28 + *(int *)((char *)l_28 + 14));
    }
    l_10 = *(short *)((char *)l_28) - 192;
    l_C = *(short *)((char *)l_28 + 2) - 1;
    l_2C = (int)(((char *)D_00195B74) + ((((int)(short)l_C) * 125) + ((int)(short)l_10)));
    func_000CD262(l_2C, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), *(int *)D_00195C44, a4);
}
