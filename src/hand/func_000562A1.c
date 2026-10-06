/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000562A1 */
#include "records.h"
#include "clib.h"

extern struct rect itemmaker_buttons[];
extern unsigned char D_0012B508;
extern short font_height;
extern char D_001756A3[];
extern char D_001756AE[];
extern char D_001756B6[];
extern char D_001756B9[];
extern char *enchant_power_names[];
extern char *enchant_side_effect_names[];
extern char *monster_names[];
extern iptr enchant_power_params[];
extern unsigned char *enchant_spell_lists[];
extern iptr enchant_side_effect_params[];
extern signed char text_buffer[];
extern signed char scratch_190ce4[];
extern signed char scratch_190cee[];
extern char D_00190CEF[];
extern char D_00190CF0[];
extern signed char D_00190D02[];
extern char D_00190D03[];
extern char D_00190D04[];
extern char *magic_window_image;
extern struct enchantment itemmaker_slots[];
extern struct item *itemmaker_item;
extern struct record *itemmaker_item_object;
extern signed char D_00199910[];
extern unsigned char inv_tab;
extern char *spell_name_by_id(unsigned char);
extern int itemmaker_points_used(void);
extern int itemmaker_gold_cost(void);
extern void text_draw_coloured(char *, short, short, short, unsigned char);
extern int gold_total_alias(void);
extern int inv_draw_item_cell(struct record *, int, struct rect *);
extern void inv_draw_left_list(struct rect *);
extern void xn_font_select(int);
extern void xn_draw_image(int, int, int, int, char *);
#pragma aux mc_set_location parm routine [];

void itemmaker_draw(void)
{
    short slot;
    short power_row;
    short side_row;

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
    inv_draw_left_list(&itemmaker_buttons[8]);
    if (itemmaker_item != 0)
        inv_draw_item_cell(itemmaker_item_object, 0, &itemmaker_buttons[5]);
    xn_font_select(3);
    for (power_row = side_row = slot = 0; slot < 10; slot++) {
        if (scratch_190ce4[slot] == -1) continue;
        D_0012B508 = 146;
        if (((char *)D_00199910)[slot] != 0)
            D_0012B508 = 193;
        if (scratch_190ce4[slot] == 0) {
            ((char *)scratch_190cee)[power_row] = slot;
            D_00190CEF[power_row] = 255;
            mc_set_location(179, D_001756A3);
            mc_sprintf(((char *)text_buffer), D_001756B6, enchant_power_names[itemmaker_slots[slot].type]);
            text_draw_coloured(((char *)text_buffer), 10, power_row * font_height + 60, D_0012B508, 156);
            power_row++;
            if (itemmaker_slots[slot].param == -1) {
                power_row++;
            } else {
                D_00190CEF[power_row] = slot;
                D_00190CF0[power_row] = 255;
                if (itemmaker_slots[slot].type < 3) {
                    mc_set_location(189, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, spell_name_by_id(enchant_spell_lists[itemmaker_slots[slot].type][itemmaker_slots[slot].param]));
                } else {
                    mc_set_location(191, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, ((char **)enchant_power_params[itemmaker_slots[slot].type])[itemmaker_slots[slot].param]);
                }
                text_draw_coloured(((char *)text_buffer), 10, power_row * font_height + 60, D_0012B508, 156);
                power_row += 2;
            }
        } else {
            ((char *)D_00190D02)[side_row] = slot;
            D_00190D03[side_row] = 255;
            mc_set_location(200, D_001756A3);
            mc_sprintf(((char *)text_buffer), D_001756B6, enchant_side_effect_names[itemmaker_slots[slot].type]);
            text_draw_coloured(((char *)text_buffer), 108, side_row * font_height + 60, D_0012B508, 156);
            side_row++;
            if (itemmaker_slots[slot].param == -1) {
                side_row++;
            } else {
                D_00190D03[side_row] = slot;
                D_00190D04[side_row] = 255;
                if (itemmaker_slots[slot].type == 0) {
                    mc_set_location(210, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, monster_names[itemmaker_slots[slot].param]);
                } else {
                    mc_set_location(212, D_001756A3);
                    mc_sprintf(((char *)text_buffer), D_001756B9, ((char **)enchant_side_effect_params[itemmaker_slots[slot].type])[itemmaker_slots[slot].param]);
                }
                text_draw_coloured(((char *)text_buffer), 108, side_row * font_height + 60, D_0012B508, 156);
                side_row += 2;
            }
        }
    }
}
