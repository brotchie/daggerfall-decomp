/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004DA4A */
extern short mouse_x;
extern short mouse_y;

#pragma pack(1)
struct rect { short x0; short y0; short x1; short y1; };
#pragma pack()

extern void note_text_box(int, struct rect *);

int note_text_hit_cb(int entry)
{
    int unused;
    struct rect r;

    note_text_box(entry, &r);
    return mouse_x > r.x0 && mouse_x < r.x1 && mouse_y > r.y0 && mouse_y < r.y1;
}
