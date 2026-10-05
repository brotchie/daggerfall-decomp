/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CFE6 */
#include "records.h"

extern char mouse_buttons[];
extern char D_0012AC02[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012DA44[];
extern char key_down_enter[];
extern char key_down_up[];
extern char key_down_down[];
extern char D_001A9AB4[];
extern int point_in_rect(short, short, short, short, short, short);

short picklist_poll(struct picklist *l)
{
    short ratio;
    short pos;

    if (*mouse_buttons || *key_down_up || *key_down_down || *key_down_enter) {
        if (l->count == 0)
            ratio = 0;
        else
            ratio = l->top * l->bar_rect.h / l->count;
        if (*key_down_enter || point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->list_rect.x, l->list_rect.y, l->list_rect.x + l->list_rect.w - 1, l->list_rect.y + l->list_rect.h - 1)) {
            if (*key_down_enter)
                pos = l->selected;
            else
                pos = l->top + (*(short *)mouse_y - l->list_rect.y) / (*(short *)D_0012DA44 + 1);
            if (pos < l->count) {
                l->selected = pos;
                if ((*key_down_enter || *D_0012AC02) && (l->entries[l->selected].flags & 128) == 0)
                    return l->entries[l->selected].index + 1;
            }
            return -5;
        }
        if ((*key_down_down || point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->down_rect.x, l->down_rect.y, l->down_rect.x + l->down_rect.w, l->down_rect.y + l->down_rect.h)) && (unsigned)(*(int *)0x46c - *(int *)D_001A9AB4) > 3) {
            *(int *)D_001A9AB4 = *(int *)0x46c;
            if (l->selected < l->count - 1) {
                l->selected++;
                if (l->count > l->visible_rows)
                    if (l->selected >= l->top + l->visible_rows && l->top < l->count - l->visible_rows)
                        l->top++;
                return -3;
            }
        }
        if ((*key_down_up || point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->up_rect.x, l->up_rect.y, l->up_rect.x + l->up_rect.w, l->up_rect.y + l->up_rect.h)) && (unsigned)(*(int *)0x46c - *(int *)D_001A9AB4) > 3) {
            *(int *)D_001A9AB4 = *(int *)0x46c;
            if (l->selected != 0) {
                l->selected--;
                if (l->count > l->visible_rows)
                    if (l->selected < l->top && l->top != 0)
                        l->top--;
                return -2;
            }
        }
        if (point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->bar_rect.x, l->bar_rect.y, l->bar_rect.x + l->bar_rect.w, l->bar_rect.y + l->bar_rect.h - 1))
            if (l->count > l->visible_rows) {
                pos = l->count * (*(short *)mouse_y - l->bar_rect.y) / l->bar_rect.h;
                if (pos >= l->count - l->visible_rows)
                    pos = l->count - l->visible_rows;
                if (pos != l->top) {
                    l->selected = pos;
                    l->top = pos;
                    return -4;
                }
            }
    }
    return -1;
}
