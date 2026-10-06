/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004DA4A */
#include "structs.h"
extern short mouse_x;
extern short mouse_y;
extern void note_text_box(iptr, struct rect *);

int note_text_hit_cb(int entry)
{
    struct rect r;

    note_text_box(entry, &r);
    return mouse_x > r.x0 && mouse_x < r.x1 && mouse_y > r.y0 && mouse_y < r.y1;
}
