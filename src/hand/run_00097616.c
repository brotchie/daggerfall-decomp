/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00097488 to 0x00097616, kept together for its switch table's alignment */
#pragma pack(1)
struct img {
    unsigned short f0;
    unsigned short f2;
    unsigned short h;
    unsigned short f6;
    unsigned short f8;
    unsigned short f10;
    char data[1];
};
#pragma pack()
extern char *player_entity;
extern unsigned char *found_object;
extern struct img *D_001AA42C;
extern struct img *D_001AA430;
extern int inv_left_scroll;
extern int inv_right_scroll;
extern short inv_right_count;
extern short inv_left_count;
extern int object_delete(unsigned char *);
extern int inv_match_arrows(int);
extern int object_find(int, int (*)(int));
extern void xn_draw_image_transparent(int, int, int, int, char *);
void inv_draw_scroll_arrow(struct img *image, int arrow);

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

void inv_draw_scroll_arrow(struct img *image, int arrow)
{
    switch (arrow) {
    case 0:
        xn_draw_image_transparent(163, 48, image->h, 20, image->data);
        break;
    case 1:
        xn_draw_image_transparent(261, 48, image->h, 20, image->data);
        break;
    case 2:
        xn_draw_image_transparent(163, image->f2 + image->f6 - 20, image->h, 20, image->data + image->f10 - image->h * 20);
        break;
    case 3:
        xn_draw_image_transparent(261, image->f2 + image->f6 - 20, image->h, 20, image->data + image->f10 - image->h * 20);
        break;
    }
}
