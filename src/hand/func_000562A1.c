/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000562A1 */
struct pair { short a; short b; };
extern unsigned char D_0012B508;
extern short D_0012DA44;
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
extern char text_buffer[];
extern signed char itemmaker_slot_kinds[];
extern char D_00190CEE[];
extern char D_00190CEF[];
extern char D_00190CF0[];
extern char D_00190D02[];
extern char D_00190D03[];
extern char D_00190D04[];
extern char *spellshop_icons;
extern struct pair itemmaker_slots[];
extern char *itemmaker_item;
extern int itemmaker_item_object;
extern char D_00199910[];
extern unsigned char inv_tab;
extern char *spell_name_by_id(unsigned char);
extern int itemmaker_points_used(void);
extern int itemmaker_gold_cost(void);
extern void text_draw_colored(char *, short, short, short, unsigned char);
extern int gold_total_alias(void);
extern int inv_draw_item_cell(int, int, char *);
extern void inv_draw_left_list(char *);
extern char *func_000A0DD9(int, char *, int);
extern void func_0012DB50(int);
extern void func_00144F68(int, int, int, int, char *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern void mc_sprintf(char *, char *, ...);

void itemmaker_draw(void)
{
    short l_20;
    short l_1C;
    short l_18;

    func_00144F68(175, inv_tab * 9 + 6, 81, 9, spellshop_icons + inv_tab * 729);
    text_draw_colored(func_000A0DD9(gold_total_alias(), text_buffer, 10), 70, 15, 145, 156);
    if (itemmaker_item != 0)
        text_draw_colored(itemmaker_item, 51, 3, 145, 156);
    if (itemmaker_item != 0)
        text_draw_colored(func_000A0DD9(itemmaker_gold_cost(), text_buffer, 10), 63, 27, 145, 156);
    if (itemmaker_item != 0) {
        func_000A0ED9(161, D_001756A3);
        mc_sprintf(text_buffer, D_001756AE, itemmaker_points_used(), *(unsigned short *)(itemmaker_item + 61));
        text_draw_colored(text_buffer, 96, 39, 145, 156);
    }
    inv_draw_left_list(D_00185B54);
    if (itemmaker_item != 0)
        inv_draw_item_cell(itemmaker_item_object, 0, D_00185B30);
    func_0012DB50(3);
    for (l_1C = l_18 = l_20 = 0; l_20 < 10; l_20++) {
        if (itemmaker_slot_kinds[l_20] == -1) continue;
        D_0012B508 = 146;
        if (D_00199910[l_20] != 0)
            D_0012B508 = 193;
        if (itemmaker_slot_kinds[l_20] == 0) {
            D_00190CEE[l_1C] = l_20;
            D_00190CEF[l_1C] = 255;
            func_000A0ED9(179, D_001756A3);
            mc_sprintf(text_buffer, D_001756B6, enchant_power_names[itemmaker_slots[l_20].a]);
            text_draw_colored(text_buffer, 10, l_1C * D_0012DA44 + 60, D_0012B508, 156);
            l_1C++;
            if (itemmaker_slots[l_20].b == -1) {
                l_1C++;
            } else {
                D_00190CEF[l_1C] = l_20;
                D_00190CF0[l_1C] = 255;
                if (itemmaker_slots[l_20].a < 3) {
                    func_000A0ED9(189, D_001756A3);
                    mc_sprintf(text_buffer, D_001756B9, spell_name_by_id(enchant_spell_lists[itemmaker_slots[l_20].a][itemmaker_slots[l_20].b]));
                } else {
                    func_000A0ED9(191, D_001756A3);
                    mc_sprintf(text_buffer, D_001756B9, enchant_power_params[itemmaker_slots[l_20].a][itemmaker_slots[l_20].b]);
                }
                text_draw_colored(text_buffer, 10, l_1C * D_0012DA44 + 60, D_0012B508, 156);
                l_1C += 2;
            }
        } else {
            D_00190D02[l_18] = l_20;
            D_00190D03[l_18] = 255;
            func_000A0ED9(200, D_001756A3);
            mc_sprintf(text_buffer, D_001756B6, enchant_side_effect_names[itemmaker_slots[l_20].a]);
            text_draw_colored(text_buffer, 108, l_18 * D_0012DA44 + 60, D_0012B508, 156);
            l_18++;
            if (itemmaker_slots[l_20].b == -1) {
                l_18++;
            } else {
                D_00190D03[l_18] = l_20;
                D_00190D04[l_18] = 255;
                if (itemmaker_slots[l_20].a == 0) {
                    func_000A0ED9(210, D_001756A3);
                    mc_sprintf(text_buffer, D_001756B9, monster_names[itemmaker_slots[l_20].b]);
                } else {
                    func_000A0ED9(212, D_001756A3);
                    mc_sprintf(text_buffer, D_001756B9, enchant_side_effect_params[itemmaker_slots[l_20].a][itemmaker_slots[l_20].b]);
                }
                text_draw_colored(text_buffer, 108, l_18 * D_0012DA44 + 60, D_0012B508, 156);
                l_18 += 2;
            }
        }
    }
}
