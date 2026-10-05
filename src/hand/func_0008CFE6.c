/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CFE6 */
#include "records.h"

extern signed char mouse_buttons;
extern signed char mouse_double_click;
extern short mouse_x;
extern short mouse_y;
extern short font_height;
extern signed char key_down_enter;
extern signed char key_down_up;
extern signed char key_down_down;
extern int picklist_repeat_clock;
extern int point_in_rect(short, short, short, short, short, short);

short picklist_poll(struct picklist *l)
{
    short ratio;
    short pos;

    if (*((char *)&mouse_buttons) || *((char *)&key_down_up) || *((char *)&key_down_down) || *((char *)&key_down_enter)) {
        if (l->count == 0)
            ratio = 0;
        else
            ratio = l->top * l->bar_rect.h / l->count;
        if (*((char *)&key_down_enter) || point_in_rect(mouse_x, mouse_y, l->list_rect.x, l->list_rect.y, l->list_rect.x + l->list_rect.w - 1, l->list_rect.y + l->list_rect.h - 1)) {
            if (*((char *)&key_down_enter))
                pos = l->selected;
            else
                pos = l->top + (mouse_y - l->list_rect.y) / (font_height + 1);
            if (pos < l->count) {
                l->selected = pos;
                if ((*((char *)&key_down_enter) || *((char *)&mouse_double_click)) && (l->entries[l->selected].flags & 128) == 0)
                    return l->entries[l->selected].index + 1;
            }
            return -5;
        }
        if ((*((char *)&key_down_down) || point_in_rect(mouse_x, mouse_y, l->down_rect.x, l->down_rect.y, l->down_rect.x + l->down_rect.w, l->down_rect.y + l->down_rect.h)) && (unsigned)(*(int *)0x46c - picklist_repeat_clock) > 3) {
            picklist_repeat_clock = *(int *)0x46c;
            if (l->selected < l->count - 1) {
                l->selected++;
                if (l->count > l->visible_rows)
                    if (l->selected >= l->top + l->visible_rows && l->top < l->count - l->visible_rows)
                        l->top++;
                return -3;
            }
        }
        if ((*((char *)&key_down_up) || point_in_rect(mouse_x, mouse_y, l->up_rect.x, l->up_rect.y, l->up_rect.x + l->up_rect.w, l->up_rect.y + l->up_rect.h)) && (unsigned)(*(int *)0x46c - picklist_repeat_clock) > 3) {
            picklist_repeat_clock = *(int *)0x46c;
            if (l->selected != 0) {
                l->selected--;
                if (l->count > l->visible_rows)
                    if (l->selected < l->top && l->top != 0)
                        l->top--;
                return -2;
            }
        }
        if (point_in_rect(mouse_x, mouse_y, l->bar_rect.x, l->bar_rect.y, l->bar_rect.x + l->bar_rect.w, l->bar_rect.y + l->bar_rect.h - 1))
            if (l->count > l->visible_rows) {
                pos = l->count * (mouse_y - l->bar_rect.y) / l->bar_rect.h;
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
