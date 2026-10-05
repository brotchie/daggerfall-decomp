/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000595DF */
#include "records.h"

extern int paperdoll_mask;
extern int D_00195B80;
extern char scratch_buffer[];
extern char color_remap_tables[];
extern int xn_draw_image_scaled();
extern int xn_draw_paperdoll_mask();
extern int xn_draw_paperdoll_item();
extern int xn_tex_cache_lookup();
extern int xn_tex_cache_flush();

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

    D_00195B80 = (int)(*(char **)color_remap_tables + (a1->color << 8));
    l_24 = xn_tex_cache_lookup(a1->inventory_image >> 7, (int)(unsigned short)(a1->inventory_image & 127), -1);
    if (l_24 == 0) {
        xn_tex_cache_flush();
        l_24 = xn_tex_cache_lookup(a1->inventory_image >> 7, (int)(unsigned short)(a1->inventory_image & 127), -1);
    }
    l_28 = *(int *)((char *)l_24 + 12);
    if (a1->enchantments[0].type == 26 && a1->enchantments[0].param == 6) {
        xn_draw_image_scaled(((int)(short)*(short *)((char *)l_28)) + a2, ((int)(short)*(short *)((char *)l_28 + 2)) + a3, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), (int)(unsigned short)(*(short *)((char *)l_28 + 8) | 32768), l_28 + *(int *)((char *)l_28 + 14));
    } else {
        xn_draw_paperdoll_item(((int)(short)*(short *)((char *)l_28)) + a2, ((int)(short)*(short *)((char *)l_28 + 2)) + a3, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), l_28 + *(int *)((char *)l_28 + 14));
    }
    l_10 = *(short *)((char *)l_28) - 192;
    l_C = *(short *)((char *)l_28 + 2) - 1;
    l_2C = (int)(((char *)paperdoll_mask) + ((((int)(short)l_C) * 125) + ((int)(short)l_10)));
    xn_draw_paperdoll_mask(l_2C, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), *(int *)scratch_buffer, a4);
}
