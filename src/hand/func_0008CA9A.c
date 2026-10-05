/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CA9A */
#include "records.h"

extern char D_00176E38[];
extern void picklist_save_background(struct picklist *);
extern char *mc_malloc(int, char *, int);

void picklist_init(struct picklist *a1, short a2, short a3, short a4, short a5, short a6, short a7, short a8, short a9, short a10, short a11, short a12, short a13, short a14, short a15, short a16, short a17, unsigned char a18, unsigned char a19, unsigned char a20, unsigned char a21, unsigned char a22)
{
    a1->framed = a22;
    a1->colour_normal = a18;
    a1->colour_flagged = a19;
    a1->colour_selected = a20;
    a1->colour_thumb = a21;
    a1->list_rect.x = a2;
    a1->list_rect.y = a3;
    a1->list_rect.w = a4;
    a1->list_rect.h = a5;
    a1->up_rect.x = a6;
    a1->up_rect.y = a7;
    a1->up_rect.w = a8;
    a1->up_rect.h = a9;
    a1->down_rect.x = a10;
    a1->down_rect.y = a11;
    a1->down_rect.w = a12;
    a1->down_rect.h = a13;
    a1->bar_rect.x = a14;
    a1->bar_rect.y = a15;
    a1->bar_rect.w = a16;
    a1->bar_rect.h = a17;
    a1->count = 0;
    a1->top = 0;
    a1->selected = 0;
    a1->thumb_height = a1->bar_rect.h - 1;
    a1->visible_rows = 0;
    a1->entries = (struct picklist_entry *)mc_malloc(35200, D_00176E38, 40);
    if (a1->framed == 0) return;
    a1->list_background = mc_malloc(a1->list_rect.w * a1->list_rect.h, D_00176E38, 44);
    a1->bar_background = mc_malloc(a1->bar_rect.w * a1->bar_rect.h, D_00176E38, 45);
    picklist_save_background(a1);
}
