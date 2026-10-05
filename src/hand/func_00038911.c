/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00038911 */
#pragma pack(1)
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct Ent { unsigned char type; char pad; };
struct P {
    struct Ent e[4];            /* 0x00 */
    unsigned short v[3];        /* 0x08 */
    signed char a[3][3];        /* 0x0e */
    signed char b[3][3];        /* 0x17 */
    signed char c[3][5];        /* 0x20 */
};
struct Box { short x0; short y0; short x1; short y1; void (*fn)(); };
struct E6 { short s; char pad[4]; };
struct C { char pad[0x9d]; struct E6 e[1]; };
extern unsigned char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern struct P *selected_spell;
extern unsigned char spell_effect_school[];
extern unsigned char spell_effect_cost_formula[];
extern struct Box spellmaker_setting_buttons[];
extern unsigned char magic_school_skills[];
extern char text_buffer[];
extern unsigned char D_001940D5;
extern struct C *player_character;
extern short spell_effect_slot;
extern short D_00195F34;
extern short D_00195F52;
extern short D_00195F54;
extern short mouse_motion_y;
extern unsigned char msgbox_kind;
extern char game_mode;
extern unsigned char mouse_buttons_prev;
extern int spellmaker_settings_image;
extern short D_00199628;
extern short D_0019962A;
extern unsigned char spellmaker_settings_kind;
extern int spell_effect_text_index(short);
extern void msgbox_open_rsc(int, int);
extern void msgbox_update(void);
extern void text_draw_colored(char *, short, short, int, unsigned char);
extern void func_0007CAEB(short, short, short, short, short, short, short);
extern int func_0009DEAC();
extern char *func_000A0DD9(int, char *, int);
extern int spell_cost_formula_dispatch();
extern int func_000CB552();
extern int func_0012B49E();

void spellmaker_settings_update(void)
{
    int val;
    int r;
    short i;

    if (spellmaker_settings_kind == 0)
        return;
    if (msgbox_kind != 4) {
        msgbox_open_rsc(spell_effect_text_index(spell_effect_slot) + 1500, 4);
        game_mode = 2;
        D_001940D5 |= 64;
        D_00195F34 = 88;
    }
    if ((int)(unsigned char)(mouse_buttons & 1) != 0 && mouse_buttons_prev != mouse_buttons && mouse_x > 281
      && mouse_x < 304 && mouse_y > 94 && mouse_y < 109) {
        D_001940D5 |= 32;
        msgbox_update();
        spellmaker_settings_kind = 0;
        return;
    }
    func_000CB552(spellmaker_settings_image);
    msgbox_update();
    if ((int)(unsigned char)(mouse_buttons & 1) != 0 && mouse_buttons != mouse_buttons_prev) {
        for (i = 11; i < 33; i++) {
            if (mouse_x > spellmaker_setting_buttons[i].x0 && mouse_x < spellmaker_setting_buttons[i].x1
              && mouse_y > spellmaker_setting_buttons[i].y0 && mouse_y < spellmaker_setting_buttons[i].y1) {
                if (i < 22)
                    mouse_motion_y = 1;
                else
                    mouse_motion_y = -1;
                r = i % 11;
                if (r < 3 && (int)(unsigned char)(spellmaker_settings_kind & 1) != 0
                  || r < 6 && (int)(unsigned char)(spellmaker_settings_kind & 2) != 0
                  || (int)(unsigned char)(spellmaker_settings_kind & 4) != 0)
                    spellmaker_setting_buttons[i % 11].fn(mouse_motion_y);
                goto done;
            }
        }
        mouse_motion_y = i = 0;
        for (; i < 11; i++) {
            if (mouse_x > spellmaker_setting_buttons[i].x0 && mouse_x < spellmaker_setting_buttons[i].x1
              && mouse_y > spellmaker_setting_buttons[i].y0 && mouse_y < spellmaker_setting_buttons[i].y1)
                mouse_motion_y = i + 1;
        }
        if (mouse_motion_y == 0)
            goto done;
        D_0019962A = mouse_motion_y - 1;
        ((struct bf8_2_1 *)&D_001940D5)->f = 1;
        D_00195F54 = mouse_x;
        D_00195F52 = mouse_y;
        D_00199628 = 0;
    } else if (mouse_buttons == 0 && ((struct bf8_2_1 *)&D_001940D5)->f) {
        ((struct bf8_2_1 *)&D_001940D5)->f = 0;
        func_0012B49E(D_00195F54, D_00195F52);
    } else if (((struct bf8_2_1 *)&D_001940D5)->f) {
        D_00199628 -= mouse_motion_y;
        if (func_0009DEAC(D_00199628) > 30) {
            if (D_0019962A < 3 && (int)(unsigned char)(spellmaker_settings_kind & 1) != 0
              || D_0019962A < 6 && (int)(unsigned char)(spellmaker_settings_kind & 2) != 0
              || (int)(unsigned char)(spellmaker_settings_kind & 4) != 0)
                spellmaker_setting_buttons[D_0019962A].fn(D_00199628 / 30);
            D_00199628 = 0;
        }
    }
done:
    val = selected_spell->v[spell_effect_slot];
    val = (110 - player_character->e[magic_school_skills[spell_effect_school[selected_spell->e[spell_effect_slot].type]]].s) * val / 100;
    text_draw_colored(func_000A0DD9(val, text_buffer, 10), 275, 119, 145, 156);
    if ((int)(unsigned char)(spellmaker_settings_kind & 1) != 0) {
        func_0007CAEB(64, 94, 87, 109, selected_spell->a[spell_effect_slot][0], 145, 156);
        func_0007CAEB(104, 94, 127, 109, selected_spell->a[spell_effect_slot][1], 145, 156);
        func_0007CAEB(160, 94, 183, 109, selected_spell->a[spell_effect_slot][2], 145, 156);
    }
    if ((int)(unsigned char)(spellmaker_settings_kind & 2) != 0) {
        func_0007CAEB(64, 114, 87, 129, selected_spell->b[spell_effect_slot][0], 145, 156);
        func_0007CAEB(104, 114, 127, 129, selected_spell->b[spell_effect_slot][1], 145, 156);
        func_0007CAEB(160, 114, 183, 129, selected_spell->b[spell_effect_slot][2], 145, 156);
    }
    if ((int)(unsigned char)(spellmaker_settings_kind & 4) != 0) {
        func_0007CAEB(64, 134, 87, 149, selected_spell->c[spell_effect_slot][0], 145, 156);
        func_0007CAEB(104, 134, 127, 149, selected_spell->c[spell_effect_slot][1], 145, 156);
        func_0007CAEB(144, 134, 167, 149, selected_spell->c[spell_effect_slot][2], 145, 156);
        func_0007CAEB(184, 134, 207, 149, selected_spell->c[spell_effect_slot][3], 145, 156);
        func_0007CAEB(240, 134, 263, 149, selected_spell->c[spell_effect_slot][4], 145, 156);
    }
    selected_spell->v[spell_effect_slot] = spell_cost_formula_dispatch(spell_effect_cost_formula[selected_spell->e[spell_effect_slot].type] - 1);
}
