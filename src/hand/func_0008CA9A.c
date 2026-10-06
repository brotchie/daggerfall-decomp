/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CA9A */
#include "records.h"
#include "clib.h"

extern char D_00176E38[];
extern void picklist_save_background(struct picklist *);

void picklist_init(struct picklist *list, short list_x, short list_y, short list_w, short list_h, short up_x, short up_y, short up_w, short up_h, short down_x, short down_y, short down_w, short down_h, short bar_x, short bar_y, short bar_w, short bar_h, unsigned char colour_normal, unsigned char colour_flagged, unsigned char colour_selected, unsigned char colour_thumb, unsigned char framed)
{
    list->framed = framed;
    list->colour_normal = colour_normal;
    list->colour_flagged = colour_flagged;
    list->colour_selected = colour_selected;
    list->colour_thumb = colour_thumb;
    list->list_rect.x = list_x;
    list->list_rect.y = list_y;
    list->list_rect.w = list_w;
    list->list_rect.h = list_h;
    list->up_rect.x = up_x;
    list->up_rect.y = up_y;
    list->up_rect.w = up_w;
    list->up_rect.h = up_h;
    list->down_rect.x = down_x;
    list->down_rect.y = down_y;
    list->down_rect.w = down_w;
    list->down_rect.h = down_h;
    list->bar_rect.x = bar_x;
    list->bar_rect.y = bar_y;
    list->bar_rect.w = bar_w;
    list->bar_rect.h = bar_h;
    list->count = 0;
    list->top = 0;
    list->selected = 0;
    list->thumb_height = list->bar_rect.h - 1;
    list->visible_rows = 0;
    list->entries = (struct picklist_entry *)mc_malloc(35200, D_00176E38, 40);
    if (list->framed == 0) return;
    list->list_background = mc_malloc(list->list_rect.w * list->list_rect.h, D_00176E38, 44);
    list->bar_background = mc_malloc(list->bar_rect.w * list->bar_rect.h, D_00176E38, 45);
    picklist_save_background(list);
}
