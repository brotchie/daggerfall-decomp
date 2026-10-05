/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000562A1 */
#include "records.h"

struct pair { short a; short b; };
extern unsigned char D_0012B508;
extern short font_height;
extern char D_001756A3[];
extern char D_001756AE[];
extern char D_001756B6[];
extern char D_001756B9[];
extern char *enchant_power_names[];
extern char *enchant_side_effect_names[];
extern char *monster_names[];
extern char **enchant_power_params[];
extern unsigned char *enchant_spell_lists[];
extern char **enchant_side_effect_params[];
extern char D_00185B30[];
extern char D_00185B54[];
extern signed char text_buffer[];
extern signed char scratch_190ce4[];
extern signed char D_00190CEE[];
extern char D_00190CEF[];
extern char D_00190CF0[];
extern signed char D_00190D02[];
extern char D_00190D03[];
extern char D_00190D04[];
extern char *magic_window_image;
extern struct pair itemmaker_slots[];
extern struct item *itemmaker_item;
extern struct record *itemmaker_item_object;
extern signed char D_00199910[];
extern unsigned char inv_tab;
extern char *spell_name_by_id(unsigned char);
extern int itemmaker_points_used(void);
extern int itemmaker_gold_cost(void);
extern void text_draw_coloured(char *, short, short, short, unsigned char);
extern int gold_total_alias(void);
extern int inv_draw_item_cell(struct record *, int, char *);
extern void inv_draw_left_list(char *);
extern char *itoa(int, char *, int);
extern void xn_font_select(int);
extern void xn_draw_image(int, int, int, int, char *);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern void mc_sprintf(char *, char *, ...);

void itemmaker_draw(void)
{
    short l_20;
    short l_1C;
    short l_18;

    xn_draw_image(175, inv_tab * 9 + 6, 81, 9, magic_window_image + inv_tab * 729);
    text_draw_coloured(itoa(gold_total_alias(), ((char *)text_buffer), 10), 70, 15, 145, 156);
    if (itemmaker_item != 0)
        text_draw_coloured(itemmaker_item->name, 51, 3, 145, 156);
    if (itemmaker_item != 0)
        text_draw_coloured(itoa(itemmaker_gold_cost(), ((char *)text_buffer), 10), 63, 27, 145, 156);
    if (itemmaker_item != 0) {
        mc_set_location(161, D_001756A3);
        mc_sprintf(((char *)text_buffer), D_001756AE, itemmaker_points_used(), itemmaker_item->enchant_points);
        text_draw_coloured(((char *)text_buffer), 96, 39, 145, 156);
    }
    inv_draw_left_list(D_00185B54);
    if (itemmaker_item != 0)
        inv_draw_item_cell(itemmaker_item_object, 0, D_00185B30);
    xn_font_select(3);
    for (l_1C = l_18 = l_20 = 0; l_20 < 10; l_20++) {
        if (scratch_190ce4[l_20] == -1) continue;
        D_0012B508 = 146;
        if (((char *)D_00199910)[l_20] != 0)
            D_0012B508 = 193;
        if (scratch_190ce4[l_20] == 0) {
            ((char *)D_00190CEE)[l_1C] = l_20;
            D_00190CEF[l_1C] = 255;
            mc_set_location(179, D_001756A3);
            mc_sprintf(((char *)text_buffer), D_001756B6, enchant_power_names[itemmaker_slots[l_20].a]);
            text_draw_coloured(((char *)text_buffer), 10, l_1C * font_height + 60, D_0012B508, 156);
            l_1C++;
            if (itemmaker_slots[l_20].b == -1) {
                l_1C++;
            } else {
                D_00190CEF[l_1C] = l_20;
                D_00190CF0[l_1C] = 255;
                if (itemmaker_slots[l_20].a < 3) {
                    mc_set_location(189, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, spell_name_by_id(enchant_spell_lists[itemmaker_slots[l_20].a][itemmaker_slots[l_20].b]));
                } else {
                    mc_set_location(191, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, enchant_power_params[itemmaker_slots[l_20].a][itemmaker_slots[l_20].b]);
                }
                text_draw_coloured(((char *)text_buffer), 10, l_1C * font_height + 60, D_0012B508, 156);
                l_1C += 2;
            }
        } else {
            ((char *)D_00190D02)[l_18] = l_20;
            D_00190D03[l_18] = 255;
            mc_set_location(200, D_001756A3);
            mc_sprintf(((char *)text_buffer), D_001756B6, enchant_side_effect_names[itemmaker_slots[l_20].a]);
            text_draw_coloured(((char *)text_buffer), 108, l_18 * font_height + 60, D_0012B508, 156);
            l_18++;
            if (itemmaker_slots[l_20].b == -1) {
                l_18++;
            } else {
                D_00190D03[l_18] = l_20;
                D_00190D04[l_18] = 255;
                if (itemmaker_slots[l_20].a == 0) {
                    mc_set_location(210, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, monster_names[itemmaker_slots[l_20].b]);
                } else {
                    mc_set_location(212, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, enchant_side_effect_params[itemmaker_slots[l_20].a][itemmaker_slots[l_20].b]);
                }
                text_draw_coloured(((char *)text_buffer), 108, l_18 * font_height + 60, D_0012B508, 156);
                l_18 += 2;
            }
        }
    }
}
