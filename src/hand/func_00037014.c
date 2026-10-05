/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00037014 */
#include "records.h"

extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00170B13[];
extern char D_00170B1C[];
extern struct spell *selected_spell;
extern char spell_effect_settings[];
extern char spell_effect_cost_formula[];
extern char spell_target_cost_factor[];
extern char spellmaker_buttons[];
extern char D_0017B241[];
extern char D_0017B243[];
extern char D_0017B245[];
extern char D_0017B247[];
extern char spell_effect_names[];
extern char spell_effect_subtype_names[];
extern char D_001845D0[];
extern char text_buffer[];
extern struct record *player_object;
extern char spellshop_icons[];
extern char list_popup_callback[];
extern struct character *player_character;
extern char window_image[];
extern char D_00195C44[];
extern char spell_effect_slot[];
extern char D_00196271[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char D_0019962C[];
extern char spellmaker_settings_kind[];
extern struct { unsigned char a:2; unsigned char f:1; } D_001940D4;
extern int spellmaker_open(int);
extern void spellmaker_exit(void);
extern void spellmaker_settings_update(void);
extern void spellmaker_effect_rows(void);
extern short spells_list_poll(void);
extern int spell_cost(struct spell *, struct character *);
extern void text_draw_centred(char *, int, int);
extern void sound_play(int, struct record *, int);
extern void text_draw_colored(char *, int, int, int, unsigned char);
extern void func_0007EC8F(short, short, int, char *, char *);
extern int gold_total_alias(void);
extern void mc_memset(char *, int, int, char *, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);
extern char *func_000A0DD9(int, char *, int);
extern void func_000A1054(char *, char *, char *, int, int);
extern short spell_cost_formula_dispatch(int);
extern void func_000CB552(int);
extern void func_000CD20E(int, int, int);
extern void func_000CE31C(char *, char *, int, int, int);
extern void func_0012DB50(int);


void spellmaker_update(void)
{
    int l_24;
    short i;
    short l_1C;
    short v;

    if (spellmaker_open(0) == 0) return;
    func_000CB552(*(int *)window_image);
    func_0012DB50(4);
    v = selected_spell->effect_costs[0] + selected_spell->effect_costs[1] + selected_spell->effect_costs[2];
    v = v * ((short *)spell_target_cost_factor)[selected_spell->target] >> 1;
    text_draw_colored(func_000A0DD9(player_character->magicka, text_buffer, 10), 43, 149, 145, 156);
    text_draw_colored(func_000A0DD9(gold_total_alias(), text_buffer, 10), 40, 158, 145, 156);
    text_draw_colored(func_000A0DD9(v << 2, text_buffer, 10), 59, 167, 145, 156);
    text_draw_colored(func_000A0DD9(spell_cost(selected_spell, player_character), text_buffer, 10), 70, 176, 145, 156);
    text_draw_colored(selected_spell->name, 60, 185, 145, 156);
    func_000CE31C(*(char **)spellshop_icons + selected_spell->element * 640 + 24, *(char **)screen_buffer + selected_spell->element * 5120 + 36779, 16, 16, 40);
    func_000CE31C(*(char **)spellshop_icons + selected_spell->target * 640, *(char **)screen_buffer + selected_spell->target * 5120 + 36755, 24, 16, 40);
    func_000CD20E(288, 94, selected_spell->icon);
    func_0012DB50(1);
    *D_0012B508 = 146;
    for (i = 0; i < 3; i++) {
        if (selected_spell->effects[i].type == 255)
            continue;
        mc_strncpy(text_buffer, *(char **)(spell_effect_names + selected_spell->effects[i].type * 4), 160, D_00170B13, 641);
        if (selected_spell->effects[i].subtype != 255 && *(int *)(spell_effect_subtype_names + selected_spell->effects[i].type * 48 + selected_spell->effects[i].subtype * 4) != 0) {
            func_000A1054(text_buffer, D_00170B1C, D_00170B13, 644, 160);
            func_000A1054(text_buffer, *(char **)(spell_effect_subtype_names + selected_spell->effects[i].type * 48 + selected_spell->effects[i].subtype * 4), D_00170B13, 645, 160);
        }
        text_draw_centred(text_buffer, 160, (i << 5) + 30);
    }
    func_0012DB50(4);
    if (!*spellmaker_settings_kind && !D_001940D4.f)
        spellmaker_effect_rows();
    if (D_001940D4.f && (i = spells_list_poll()) > -1)
        (*(void (**)(int))list_popup_callback)((*(unsigned char **)D_00195C44)[i + 32000]);
    spellmaker_settings_update();
    if (*(short *)D_0019962C > -1 && *D_00196271) {
        if (*(unsigned char *)D_00196271 == 1) {
            i = selected_spell->effects[*(short *)spell_effect_slot = *(short *)D_0019962C].subtype;
            if (i == 255)
                i = 0;
            if ((*spellmaker_settings_kind = ((char (*)[12])spell_effect_settings)[selected_spell->effects[*(short *)spell_effect_slot].type][i]) == 0)
                selected_spell->effect_costs[*(short *)spell_effect_slot] = spell_cost_formula_dispatch(*(unsigned char *)(spell_effect_cost_formula + selected_spell->effects[*(short *)spell_effect_slot].type) - 1);
        } else {
            selected_spell->effects[*(short *)D_0019962C].type = selected_spell->effects[*(short *)D_0019962C].subtype = 255;
            mc_memset(&selected_spell->durations[*(short *)D_0019962C], 1, 3, D_00170B13, 681, 3);
            mc_memset(&selected_spell->chances[*(short *)D_0019962C], 1, 3, D_00170B13, 682, 3);
            mc_memset(&selected_spell->magnitudes[*(short *)D_0019962C], 1, 5, D_00170B13, 683, 5);
            selected_spell->effect_costs[*(short *)D_0019962C] = 0;
        }
        *(short *)D_0019962C = -1;
    }
    if (*(short *)D_0019962C > -1 && *(unsigned char *)game_mode != 8)
        *(short *)D_0019962C = -1;
    if (!*spellmaker_settings_kind && !D_001940D4.f)
        func_0007EC8F(5, 22, 18, spellmaker_buttons, D_001845D0);
    if (*key_down_esc && !*spellmaker_settings_kind)
        spellmaker_exit();
    if (!*mouse_buttons || *mouse_buttons && *mouse_buttons_prev)
        return;
    if (*(unsigned char *)game_mode == 2 && !*spellmaker_settings_kind && !D_001940D4.f) {
        for (i = 0; i < 18; i++) {
            if (*(short *)mouse_x > *(short *)(spellmaker_buttons + i * 12) && *(short *)mouse_x < *(short *)(D_0017B243 + i * 12) && *(short *)mouse_y > *(short *)(D_0017B241 + i * 12) && *(short *)mouse_y < *(short *)(D_0017B245 + i * 12)) {
                sound_play(203, player_object, 110);
                (*(void (**)(void))(D_0017B247 + i * 12))();
            }
        }
    }
}
