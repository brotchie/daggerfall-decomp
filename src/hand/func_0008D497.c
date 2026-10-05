/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008D497 */
#include "records.h"

extern unsigned char D_0012B508;
extern unsigned char font_height;
extern char D_00176E38[];
extern char D_001A9AF3;
extern void text_draw(char *, int, int);
extern void picklist_clip_text(char *, short);
extern void mc_strncpy(char *, char *, int, char *, int);
extern void xn_draw_line_text_colour(int, int, int, int);
extern void xn_draw_fill_rect(int, int, int, int);
extern void xn_draw_put_rect(int, int, int, int, char *, int);

void picklist_draw(struct picklist *a1, int a2)
{
    short l_30;
    short l_2C;
    char l_88[80];
    short l_28;
    short l_24;
    short l_20;
    short l_1C;
    short l_18;
    unsigned char l_14;

    l_28 = D_0012B508;
    l_30 = 0;
    l_14 = font_height;
    if (a1->count == 0)
        l_30 = 0;
    else
        l_30 = a1->top * a1->bar_rect.h / a1->count;
    if (a1->framed) {
        xn_draw_put_rect(a1->list_rect.x, a1->list_rect.y, a1->list_rect.w, a1->list_rect.h, a1->list_background, 0);
        xn_draw_put_rect(a1->bar_rect.x, a1->bar_rect.y, a1->bar_rect.w, a1->bar_rect.h, a1->bar_background, 0);
    }
    D_0012B508 = a1->colour_thumb;
    l_2C = a1->bar_rect.x + 2;
    l_18 = l_30 + (a1->bar_rect.y + 1);
    l_20 = a1->bar_rect.w - 4;
    l_1C = a1->thumb_height - 1;
    if (a1->bar_rect.x != 0)
        xn_draw_fill_rect(l_2C, l_18, l_20, l_1C);
    D_0012B508 = 123;
    xn_draw_line_text_colour(l_2C, l_18, l_2C, l_18 + l_1C - 1);
    xn_draw_line_text_colour(l_2C, l_18 + l_1C - 1, l_2C + l_20 - 1, l_18 + l_1C - 1);
    D_0012B508 = 112;
    xn_draw_line_text_colour(l_2C + l_20 - 1, l_18, l_2C + l_20 - 1, l_18 + l_1C - 2);
    xn_draw_line_text_colour(l_2C + 1, l_18, l_2C + l_20 - 1, l_18);
    l_24 = a1->top;
    l_18 = 1;
    while (l_24 < a1->count && l_18 < a1->list_rect.h - l_14) {
        mc_strncpy(l_88, a1->entries[l_24].text, 80, D_00176E38, 236);
        picklist_clip_text(l_88, a1->list_rect.w);
        if (l_24 != a1->selected || D_001A9AF3 != 0) {
            D_0012B508 = l_24 != a1->selected ? 156 : 0;
            text_draw(l_88, a1->list_rect.x + 2, a1->list_rect.y + l_18 + 1);
        }
        D_0012B508 = (a1->entries[l_24].flags & 1) ? a1->colour_flagged : a1->colour_normal;
        if (l_24 == a1->selected)
            D_0012B508 = a1->colour_selected;
        text_draw(l_88, a1->list_rect.x + 1, a1->list_rect.y + l_18);
        l_24++;
        l_18 += l_14 + 1;
    }
    D_0012B508 = l_28;
}
