/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00095B81 */
#include "records.h"

extern int D_001940D6;
extern int D_001940D8;
extern struct record *wagon_container;
extern struct record *player_object;
extern struct character *player_character;
extern int picked_model_index;
extern struct record *inv_right_rows[];
extern int inv_right_scroll;
extern short D_001AA588;
extern void inv_draw_item_cell(struct record *, short, int);
extern int xn_math_approx_dist2d(int, int, int, int);
extern int xn_math_approx_hypot(int, int);
extern int xn_str_find_u32(char *, struct record *, int);

void inv_list_right_item(struct record *a1, int a2)
{
    struct item *s;

    if (a1->type != 2 && a1->type != 54 || (a1->flags & 2))
        return;
    if (a1->flags & 0x200)
        return;
    if ((D_001940D6 & 4) && a1->owner != picked_model_index)
        return;
    if ((!(D_001940D6 & 4) && a1->parent != wagon_container ? 1 : 0) && xn_math_approx_hypot(a1->z - player_object->z, xn_math_approx_dist2d(a1->x, a1->y, player_object->x, player_object->y)) > 160)
        return;
    if (D_001AA588 >= inv_right_scroll && D_001AA588 < inv_right_scroll + 4) {
        if (!(D_001940D8 & 4) && xn_str_find_u32((char *)player_character->equipped, a1, 27) != 0) {
            s = &a1->data.item;
            if (s->group != 1)
                return;
        }
        inv_right_rows[D_001AA588 - inv_right_scroll] = a1;
        inv_draw_item_cell(a1, D_001AA588 - inv_right_scroll, a2);
    }
    D_001AA588++;
}
