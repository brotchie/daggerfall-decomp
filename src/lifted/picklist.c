/* picklist.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern short font_height;
extern char D_00176E38[];

extern int font_text_width(int);
extern int mc_free();
extern int strlen();

void picklist_update_thumb(struct picklist *a1)
{
    a1->visible_rows = a1->list_rect.h / (((int)(short)font_height) + 1);
    if (a1->count == 0 || a1->count <= a1->visible_rows) {
        a1->thumb_height = a1->bar_rect.h - 1;
        return;
    }
    a1->thumb_height = ((a1->bar_rect.h - 2) * a1->visible_rows) / a1->count;
}

void picklist_free(struct picklist *a1)
{
    if (a1->framed != 0) {
        if (a1->list_background != 0 && (int)a1->list_background != (-1751672937)) {
            mc_free((int)a1->list_background, (int)D_00176E38, 110);
            a1->list_background = (char *)-1751672937;
        }
        if (a1->bar_background != 0 && (int)a1->bar_background != (-1751672937)) {
            mc_free((int)a1->bar_background, (int)D_00176E38, 111);
            a1->bar_background = (char *)-1751672937;
        }
    }
    if (a1->entries != 0 && (int)a1->entries != (-1751672937)) {
        mc_free((int)a1->entries, (int)D_00176E38, 114);
        a1->entries = (struct picklist_entry *)-1751672937;
    }
    a1->framed = 0;
    a1->count = 0;
}

void picklist_clip_text(int a1, short a2)
{
    while (font_text_width(a1) > ((int)(short)a2)) {
        *(signed char *)((char *)(strlen(a1) + a1) - 1) = 0;
    }
}

void swap_shorts(int a1, int a2)
{
    *(short *)((char *)a1) ^= *(short *)((char *)a2);
    *(short *)((char *)a2) ^= *(short *)((char *)a1);
    *(short *)((char *)a1) ^= *(short *)((char *)a2);
}
