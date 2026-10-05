/* picklist.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_0012DA44[];
extern char D_00176E38[];

extern int font_text_width(int);
extern int mc_free();
extern int func_000A0DF4();

void picklist_update_thumb(struct picklist *a1)
{
    a1->visible_rows = a1->list_rect.h / (((int)(short)*(short *)D_0012DA44) + 1);
    if (a1->count == 0) goto L8CC7B;
    if (a1->count > a1->visible_rows) goto L8CC8C;
L8CC7B:;
    a1->thumb_height = a1->bar_rect.h - 1;
    return;
L8CC8C:;
    a1->thumb_height = ((a1->bar_rect.h - 2) * a1->visible_rows) / a1->count;
}

void picklist_free(struct picklist *a1)
{
    if (a1->framed == 0) goto L8CF97;
    if (a1->list_background == 0) goto L8CF40;
    if ((int)a1->list_background != (-1751672937)) goto L8CF42;
L8CF40:;
    goto L8CF61;
L8CF42:;
    mc_free((int)a1->list_background, (int)D_00176E38, 110);
    a1->list_background = (char *)-1751672937;
L8CF61:;
    if (a1->bar_background == 0) goto L8CF76;
    if ((int)a1->bar_background != (-1751672937)) goto L8CF78;
L8CF76:;
    goto L8CF97;
L8CF78:;
    mc_free((int)a1->bar_background, (int)D_00176E38, 111);
    a1->bar_background = (char *)-1751672937;
L8CF97:;
    if (a1->entries == 0) goto L8CFAC;
    if ((int)a1->entries != (-1751672937)) goto L8CFAE;
L8CFAC:;
    goto L8CFCD;
L8CFAE:;
    mc_free((int)a1->entries, (int)D_00176E38, 114);
    a1->entries = (struct picklist_entry *)-1751672937;
L8CFCD:;
    a1->framed = 0;
    a1->count = 0;
}

void picklist_clip_text(int a1, short a2)
{
L8D46D:;
    if (font_text_width(a1) <= ((int)(short)a2)) return;
    *(signed char *)((char *)(func_000A0DF4(a1) + a1) - 1) = 0;
    goto L8D46D;
}

void swap_shorts(int a1, int a2)
{
    *(short *)((char *)a1) ^= *(short *)((char *)a2);
    *(short *)((char *)a2) ^= *(short *)((char *)a1);
    *(short *)((char *)a1) ^= *(short *)((char *)a2);
}
