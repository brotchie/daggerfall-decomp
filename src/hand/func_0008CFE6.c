/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CFE6 */
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

#pragma pack(1)
struct rect { short x; short y; short w; short h; };
struct item { unsigned short flags; short val; char pad[40]; };
struct list {
    char pad0[5];
    struct rect area;
    struct rect up;
    struct rect down;
    struct rect bar;
    unsigned short count;
    unsigned short top;
    unsigned short sel;
    unsigned short visible;
    short pad2d;
    struct item *items;
};
#pragma pack()

short picklist_poll(struct list *l)
{
    short ratio;
    short pos;

    if (*mouse_buttons || *key_down_up || *key_down_down || *key_down_enter) {
        if (l->count == 0)
            ratio = 0;
        else
            ratio = l->top * l->bar.h / l->count;
        if (*key_down_enter || point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->area.x, l->area.y, l->area.x + l->area.w - 1, l->area.y + l->area.h - 1)) {
            if (*key_down_enter)
                pos = l->sel;
            else
                pos = l->top + (*(short *)mouse_y - l->area.y) / (*(short *)D_0012DA44 + 1);
            if (pos < l->count) {
                l->sel = pos;
                if ((*key_down_enter || *D_0012AC02) && (l->items[l->sel].flags & 128) == 0)
                    return l->items[l->sel].val + 1;
            }
            return -5;
        }
        if ((*key_down_down || point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->down.x, l->down.y, l->down.x + l->down.w, l->down.y + l->down.h)) && (unsigned)(*(int *)0x46c - *(int *)D_001A9AB4) > 3) {
            *(int *)D_001A9AB4 = *(int *)0x46c;
            if (l->sel < l->count - 1) {
                l->sel++;
                if (l->count > l->visible)
                    if (l->sel >= l->top + l->visible && l->top < l->count - l->visible)
                        l->top++;
                return -3;
            }
        }
        if ((*key_down_up || point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->up.x, l->up.y, l->up.x + l->up.w, l->up.y + l->up.h)) && (unsigned)(*(int *)0x46c - *(int *)D_001A9AB4) > 3) {
            *(int *)D_001A9AB4 = *(int *)0x46c;
            if (l->sel != 0) {
                l->sel--;
                if (l->count > l->visible)
                    if (l->sel < l->top && l->top != 0)
                        l->top--;
                return -2;
            }
        }
        if (point_in_rect(*(short *)mouse_x, *(short *)mouse_y, l->bar.x, l->bar.y, l->bar.x + l->bar.w, l->bar.y + l->bar.h - 1))
            if (l->count > l->visible) {
                pos = l->count * (*(short *)mouse_y - l->bar.y) / l->bar.h;
                if (pos >= l->count - l->visible)
                    pos = l->count - l->visible;
                if (pos != l->top) {
                    l->sel = pos;
                    l->top = pos;
                    return -4;
                }
            }
    }
    return -1;
}
