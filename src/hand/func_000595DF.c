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

void paperdoll_draw_item(struct item *item, int x, int y, int mask_value)
{
    char *mask;
    char *image;
    char *texture;
    int unused4;
    int unused3;
    int unused2;
    int unused1;
    short mask_x;
    short mask_y;

    D_00195B80 = (int)(*(char **)color_remap_tables + (item->color << 8));
    texture = (char *)xn_tex_cache_lookup(item->inventory_image >> 7, (int)(unsigned short)(item->inventory_image & 127), -1);
    if (texture == 0) {
        xn_tex_cache_flush();
        texture = (char *)xn_tex_cache_lookup(item->inventory_image >> 7, (int)(unsigned short)(item->inventory_image & 127), -1);
    }
    image = *(char **)(texture + 12);
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 6) {
        xn_draw_image_scaled(((int)(short)*(short *)image) + x, ((int)(short)*(short *)(image + 2)) + y, (int)(unsigned short)*(short *)(image + 4), (int)(unsigned short)*(short *)(image + 6), (int)(unsigned short)*(short *)(image + 4), (int)(unsigned short)*(short *)(image + 6), (int)(unsigned short)(*(short *)(image + 8) | 32768), image + *(int *)(image + 14));
    } else {
        xn_draw_paperdoll_item(((int)(short)*(short *)image) + x, ((int)(short)*(short *)(image + 2)) + y, (int)(unsigned short)*(short *)(image + 4), (int)(unsigned short)*(short *)(image + 6), image + *(int *)(image + 14));
    }
    mask_x = *(short *)image - 192;
    mask_y = *(short *)(image + 2) - 1;
    mask = (char *)paperdoll_mask + ((((int)(short)mask_y) * 125) + ((int)(short)mask_x));
    xn_draw_paperdoll_mask(mask, (int)(unsigned short)*(short *)(image + 4), (int)(unsigned short)*(short *)(image + 6), *(int *)scratch_buffer, mask_value);
}
