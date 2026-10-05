/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000804C8 */
#include "structs.h"
struct bits { unsigned char b0:1; unsigned char b1:1; unsigned char b2:1; };
extern short mouse_x;
extern short mouse_y;
extern struct bits D_001940D4;
extern struct bits D_001940D5;
extern unsigned char mouse_control_mode;
extern char view_cursor_active;
extern char D_00196272;
extern char *cursor_region_images;
extern void cursor_draw_arrow(void);
extern void xn_draw_image_transparent(int, int, int, int, char *);

void cursor_draw(short region)
{
    struct image *p;
    int unused1, unused2, unused3, unused4, unused5;   /* unused, but they have slots */
    char unused6;

    if (D_001940D5.b2) return;
    if (D_00196272 || D_001940D4.b2) {
        cursor_draw_arrow();
        return;
    }
    if (mouse_control_mode == 1 && view_cursor_active == 0) return;
    if (mouse_control_mode == 1 && view_cursor_active != 0) {
        cursor_draw_arrow();
        return;
    }
    p = (struct image *)cursor_region_images;
    while (region-- != 0)
        p = (struct image *)((char *)p + p->data_size + 12);
    xn_draw_image_transparent(mouse_x, mouse_y, p->width, p->height, p->pixels);
}
