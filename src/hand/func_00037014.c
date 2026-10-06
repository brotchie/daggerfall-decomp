/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00037014 */
#include "records.h"
#include "clib.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern iptr screen_buffer;
extern char D_00170B13[];
extern char D_00170B1C[];
extern struct spell *selected_spell;
extern char spell_effect_settings[];
extern char spell_effect_cost_formula[];
extern char spell_target_cost_factor[];
extern struct rect spellmaker_buttons[];
extern char *spell_effect_names[];
extern char *spell_effect_subtype_names[][12];
extern char *D_001845D0[];
extern signed char text_buffer[];
extern struct record *player_object;
extern iptr magic_window_image;
extern iptr list_popup_callback;
extern struct character *player_character;
extern iptr window_image;
extern char *scratch_buffer;
extern short spell_effect_slot;
extern unsigned char D_00196271;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern short D_0019962C;
extern char spellmaker_settings_kind[];
extern struct { unsigned char a:2; unsigned char f:1; } D_001940D4;
extern int spellmaker_open(int);
extern int spellmaker_close(void);
extern void spellmaker_settings_update(void);
extern void spellmaker_effect_rows(void);
extern int list_popup_poll(void);
extern int spell_cost(struct spell *, struct character *);
extern void text_draw_centred(char *, int, int);
extern int sound_play(int, struct record *, int);
extern void text_draw_coloured(char *, int, int, int, unsigned char);
extern void buttons_draw_hover_label(short, short, int, struct rect *, char *);
extern int gold_total_alias(void);
extern short spell_cost_formula_dispatch(int);
extern void xn_draw_fullscreen_overlay_shaded(iptr);
extern void xn_draw_spell_icon(int, int, int);
extern void xn_draw_copy_rect_stride_bytes(char *, char *, int, int, int);
extern void xn_font_select(int);


void spellmaker_update(void)
{
    int unused;
    short i;
    short unused2;
    short v;

    if (spellmaker_open(0) == 0) return;
    xn_draw_fullscreen_overlay_shaded(window_image);
    xn_font_select(4);
    v = selected_spell->effect_costs[0] + selected_spell->effect_costs[1] + selected_spell->effect_costs[2];
    v = v * ((short *)spell_target_cost_factor)[selected_spell->target] >> 1;
    text_draw_coloured(itoa(player_character->magicka, ((char *)text_buffer), 10), 43, 149, 145, 156);
    text_draw_coloured(itoa(gold_total_alias(), ((char *)text_buffer), 10), 40, 158, 145, 156);
    text_draw_coloured(itoa(v << 2, ((char *)text_buffer), 10), 59, 167, 145, 156);
    text_draw_coloured(itoa(spell_cost(selected_spell, player_character), ((char *)text_buffer), 10), 70, 176, 145, 156);
    text_draw_coloured(selected_spell->name, 60, 185, 145, 156);
    xn_draw_copy_rect_stride_bytes(*(char **)&magic_window_image + selected_spell->element * 640 + 24, *(char **)&screen_buffer + selected_spell->element * 5120 + 36779, 16, 16, 40);
    xn_draw_copy_rect_stride_bytes(*(char **)&magic_window_image + selected_spell->target * 640, *(char **)&screen_buffer + selected_spell->target * 5120 + 36755, 24, 16, 40);
    xn_draw_spell_icon(288, 94, selected_spell->icon);
    xn_font_select(1);
    *((char *)&D_0012B508) = 146;
    for (i = 0; i < 3; i++) {
        if (selected_spell->effects[i].type == 255)
            continue;
        mc_strncpy(((char *)text_buffer), spell_effect_names[selected_spell->effects[i].type], 160, D_00170B13, 641);
        if (selected_spell->effects[i].subtype != 255 && (iptr)spell_effect_subtype_names[selected_spell->effects[i].type][selected_spell->effects[i].subtype] != 0) {
            func_000A1054(((char *)text_buffer), D_00170B1C, D_00170B13, 644, 160);
            func_000A1054(((char *)text_buffer), spell_effect_subtype_names[selected_spell->effects[i].type][selected_spell->effects[i].subtype], D_00170B13, 645, 160);
        }
        text_draw_centred(((char *)text_buffer), 160, (i << 5) + 30);
    }
    xn_font_select(4);
    if (!*spellmaker_settings_kind && !D_001940D4.f)
        spellmaker_effect_rows();
    if (D_001940D4.f && (i = list_popup_poll()) > -1)
        (*(void (**)(int))((char *)&list_popup_callback))(((unsigned char *)scratch_buffer)[i + 32000]);
    spellmaker_settings_update();
    if (D_0019962C > -1 && *((char *)&D_00196271)) {
        if (D_00196271 == 1) {
            i = selected_spell->effects[spell_effect_slot = D_0019962C].subtype;
            if (i == 255)
                i = 0;
            if ((*spellmaker_settings_kind = ((char (*)[12])spell_effect_settings)[selected_spell->effects[spell_effect_slot].type][i]) == 0)
                selected_spell->effect_costs[spell_effect_slot] = spell_cost_formula_dispatch(*(unsigned char *)(spell_effect_cost_formula + selected_spell->effects[spell_effect_slot].type) - 1);
        } else {
            selected_spell->effects[D_0019962C].type = selected_spell->effects[D_0019962C].subtype = 255;
            mc_memset(&selected_spell->durations[D_0019962C], 1, 3, D_00170B13, 681, 3);
            mc_memset(&selected_spell->chances[D_0019962C], 1, 3, D_00170B13, 682, 3);
            mc_memset(&selected_spell->magnitudes[D_0019962C], 1, 5, D_00170B13, 683, 5);
            selected_spell->effect_costs[D_0019962C] = 0;
        }
        D_0019962C = -1;
    }
    if (D_0019962C > -1 && (unsigned char)game_mode != 8)
        D_0019962C = -1;
    if (!*spellmaker_settings_kind && !D_001940D4.f)
        buttons_draw_hover_label(5, 22, 18, spellmaker_buttons, (char *)D_001845D0);
    if (*((char *)&key_down_esc) && !*spellmaker_settings_kind)
        spellmaker_close();
    if (!*((char *)&mouse_buttons) || *((char *)&mouse_buttons) && *((char *)&mouse_buttons_prev))
        return;
    if ((unsigned char)game_mode == 2 && !*spellmaker_settings_kind && !D_001940D4.f) {
        for (i = 0; i < 18; i++) {
            if (mouse_x > spellmaker_buttons[i].x0 && mouse_x < spellmaker_buttons[i].x1 && mouse_y > spellmaker_buttons[i].y0 && mouse_y < spellmaker_buttons[i].y1) {
                sound_play(203, player_object, 110);
                spellmaker_buttons[i].handler();
            }
        }
    }
}
