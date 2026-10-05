/* picklist.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern short font_height;
extern char D_00176E38[];

extern int font_text_width(char *);
extern int mc_free();
extern int strlen(char *);

void picklist_update_thumb(struct picklist *list)
{
    list->visible_rows = list->list_rect.h / (((int)(short)font_height) + 1);
    if (list->count == 0 || list->count <= list->visible_rows) {
        list->thumb_height = list->bar_rect.h - 1;
        return;
    }
    list->thumb_height = ((list->bar_rect.h - 2) * list->visible_rows) / list->count;
}

void picklist_free(struct picklist *list)
{
    if (list->framed != 0) {
        if (list->list_background != 0 && (int)list->list_background != (-1751672937)) {
            mc_free((int)list->list_background, (int)D_00176E38, 110);
            list->list_background = (char *)-1751672937;
        }
        if (list->bar_background != 0 && (int)list->bar_background != (-1751672937)) {
            mc_free((int)list->bar_background, (int)D_00176E38, 111);
            list->bar_background = (char *)-1751672937;
        }
    }
    if (list->entries != 0 && (int)list->entries != (-1751672937)) {
        mc_free((int)list->entries, (int)D_00176E38, 114);
        list->entries = (struct picklist_entry *)-1751672937;
    }
    list->framed = 0;
    list->count = 0;
}

void picklist_clip_text(char *text, short max_width)
{
    while (font_text_width(text) > max_width) {
        text[strlen(text) - 1] = 0;
    }
}

void swap_shorts(short *a, short *b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
