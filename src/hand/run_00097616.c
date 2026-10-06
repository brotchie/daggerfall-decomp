/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00097488 to 0x00097616, kept together for its switch table's alignment */
#include "structs.h"
extern char *player_entity;
extern unsigned char *found_object;
extern struct image *D_001AA42C;
extern struct image *D_001AA430;
extern int inv_left_scroll;
extern int inv_right_scroll;
extern short inv_right_count;
extern short inv_left_count;
extern iptr object_delete(unsigned char *);
extern iptr inv_match_arrows(iptr);
extern int object_find(iptr, iptr (*)());
extern void xn_draw_image_transparent(int, int, int, int, char *);
void inv_draw_scroll_arrow(struct image *image, int arrow);

int inv_take_arrow(int consume)
{
    unsigned char *item;

    found_object = 0;
    object_find(*(int *)(player_entity + 63), inv_match_arrows);
    if (found_object == 0) return 0;
    if (consume == 0) return 1;
    item = found_object + 71;
    if (item[49] == 1) {
        object_delete(found_object);
        return 1;
    }
    item[49]--;
    return 1;
}

void inv_draw_scroll_arrows(void)
{
    int last;

    inv_draw_scroll_arrow(inv_left_scroll != 0 ? D_001AA42C : D_001AA430, 0);
    inv_draw_scroll_arrow(inv_right_scroll != 0 ? D_001AA42C : D_001AA430, 1);
    last = inv_left_count - 4;
    inv_draw_scroll_arrow(last > 0 && inv_left_scroll < last ? D_001AA42C : D_001AA430, 2);
    last = inv_right_count - 4;
    inv_draw_scroll_arrow(last > 0 && inv_right_scroll < last ? D_001AA42C : D_001AA430, 3);
}

void inv_draw_scroll_arrow(struct image *image, int arrow)
{
    switch (arrow) {
    case 0:
        xn_draw_image_transparent(163, 48, image->width, 20, image->pixels);
        break;
    case 1:
        xn_draw_image_transparent(261, 48, image->width, 20, image->pixels);
        break;
    case 2:
        xn_draw_image_transparent(163, image->y + image->height - 20, image->width, 20, image->pixels + image->data_size - image->width * 20);
        break;
    case 3:
        xn_draw_image_transparent(261, image->y + image->height - 20, image->width, 20, image->pixels + image->data_size - image->width * 20);
        break;
    }
}
