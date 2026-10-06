/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008D497 */
#include "records.h"
#include "clib.h"

extern unsigned char D_0012B508;
extern unsigned char font_height;
extern char D_00176E38[];
extern char D_001A9AF3;
extern void text_draw(char *, int, int);
extern void picklist_clip_text(char *, short);
extern void xn_draw_line_text_colour(int, int, int, int);
extern void xn_draw_fill_rect(int, int, int, int);
extern void xn_draw_put_rect(int, int, int, int, char *, int);

void picklist_draw(struct picklist *list, int unused)
{
    short thumb_y;
    short x;
    char text[80];
    short saved_colour;
    short i;
    short w;
    short h;
    short y;
    unsigned char line_height;

    saved_colour = D_0012B508;
    thumb_y = 0;
    line_height = font_height;
    if (list->count == 0)
        thumb_y = 0;
    else
        thumb_y = list->top * list->bar_rect.h / list->count;
    if (list->framed) {
        xn_draw_put_rect(list->list_rect.x, list->list_rect.y, list->list_rect.w, list->list_rect.h, list->list_background, 0);
        xn_draw_put_rect(list->bar_rect.x, list->bar_rect.y, list->bar_rect.w, list->bar_rect.h, list->bar_background, 0);
    }
    D_0012B508 = list->colour_thumb;
    x = list->bar_rect.x + 2;
    y = thumb_y + (list->bar_rect.y + 1);
    w = list->bar_rect.w - 4;
    h = list->thumb_height - 1;
    if (list->bar_rect.x != 0)
        xn_draw_fill_rect(x, y, w, h);
    D_0012B508 = 123;
    xn_draw_line_text_colour(x, y, x, y + h - 1);
    xn_draw_line_text_colour(x, y + h - 1, x + w - 1, y + h - 1);
    D_0012B508 = 112;
    xn_draw_line_text_colour(x + w - 1, y, x + w - 1, y + h - 2);
    xn_draw_line_text_colour(x + 1, y, x + w - 1, y);
    i = list->top;
    y = 1;
    while (i < list->count && y < list->list_rect.h - line_height) {
        mc_strncpy(text, list->entries[i].text, 80, D_00176E38, 236);
        picklist_clip_text(text, list->list_rect.w);
        if (i != list->selected || D_001A9AF3 != 0) {
            D_0012B508 = i != list->selected ? 156 : 0;
            text_draw(text, list->list_rect.x + 2, list->list_rect.y + y + 1);
        }
        D_0012B508 = (list->entries[i].flags & 1) ? list->colour_flagged : list->colour_normal;
        if (i == list->selected)
            D_0012B508 = list->colour_selected;
        text_draw(text, list->list_rect.x + 1, list->list_rect.y + y);
        i++;
        y += line_height + 1;
    }
    D_0012B508 = saved_colour;
}
