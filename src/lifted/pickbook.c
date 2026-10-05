/* pickbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern int screen_buffer;
extern char D_00170DE4[];
extern char D_00170DEF[];
extern char D_00170E11[];
extern char D_00170E17[];
extern struct spell *selected_spell;
extern char spellbook_buttons[];
extern char D_0017B6D2[];
extern char D_0017B6D4[];
extern char D_0017B6D6[];
extern char D_0017B6D8[];
extern char spell_effect_names[];
extern char spell_effect_subtype_names[];
extern char D_0018320A[];
extern int D_00184634;
extern char monster_category[];
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern struct record *D_00190504[];
extern char D_00190D64[];
extern char text_macro_fpc[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D8;
extern int D_001959FC;
extern struct record *player_entity;
extern struct record *player_object;
extern int creature_count;
extern struct record *spell_ready_missile;
extern struct record *spell_ready_touch;
extern int spellshop_icons;
extern struct character *player_character;
extern int window_image;
extern int game_minutes;
extern int trade_mode;
extern short spell_effect_slot;
extern short D_00195F62;
extern signed char msgbox_kind;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char inv_right_icon;
extern int spellbook_saved_screen;
extern struct picklist D_001A9AB8;

extern int spell_effect_text_index(short);
extern int spell_cost(struct spell *, struct character *);
extern int sheet_open(int);
extern int spellbook_open(int);
extern int cast_player_spell(struct record *);
extern int sound_play(int, struct record *, int);
extern int hud_message_add(int);
extern int picklist_frame(struct picklist *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_find_item(struct record *, int, int);
extern int object_new_id(int);
extern int inventory_open(int, int, int);
extern int mc_free();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000CD20E();
extern int func_000CE31C();
extern int func_0012B136();
extern int func_0012DB50();
extern int func_00144FB4();
extern void spell_add_skill_uses(struct spell *, int);
extern void msgbox_show_rsc(int, int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void picklist_init(struct picklist *, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(struct picklist *, int, int);
extern void picklist_free(struct picklist *);
extern void func_0008E152(struct record *, struct record *);
extern void object_foreach(struct record *, int);
int spellbook_close(void);
int spellbook_build_list(void);
int func_00042380(void);
void spellbook_add_spell_cb(struct record *);
void spellbook_draw_spell(struct spell *);
void spellbook_effect_help(int);
#pragma aux func_000A0ED9 parm routine [];

void spellbook_add_spell_cb(struct record *a1)
{
    struct spell *l_1C;
    int l_18;

    if (a1->type != 9) return;
    l_1C = &a1->data.spell;
    if (((int)(unsigned char)l_1C->name[0]) == 33 || ((int)(unsigned char)l_1C->name[func_000A0DF4(l_1C->name) + 1]) == 36) {
        l_18 = spell_cost(l_1C, player_character);
        if (((int)(unsigned char)l_1C->name[func_000A0DF4(l_1C->name) + 1]) == 36) {
            l_18 >>= 2;
            func_000A0ED9(62, (int)D_00170DE4);
            mc_sprintf((int)text_buffer, (int)D_00170DEF, l_18, l_1C->name);
        } else {
            func_000A0ED9(65, (int)D_00170DE4);
            mc_sprintf((int)text_buffer, (int)D_00170DEF, l_18, &l_1C->name[1]);
        }
    } else {
        func_000A0ED9(68, (int)D_00170DE4);
        mc_sprintf((int)text_buffer, (int)D_00170DEF, spell_cost(l_1C, player_character), l_1C->name);
    }
    picklist_add(&D_001A9AB8, (int)text_buffer, 0);
    *(int *)(text_macro_fpc + (((int)(short)(*(short *)D_00190D64)++) << 2)) = (int)a1;
}

void spellbook_frame(void)
{
    struct record *l_30;
    struct record *l_2C;
    int l_28;
    int l_24;
    int l_20;
    short l_1C;
    short l_18;

    if (spellbook_open(0) == 0) return;
    mc_memcpy(screen_buffer, spellbook_saved_screen, 64000, (int)D_00170DE4, 123, 4);
    l_28 = window_image;
    func_00144FB4((int)(unsigned short)*(short *)((char *)l_28), (int)(unsigned short)*(short *)((char *)l_28 + 2), (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), l_28 + 12);
    func_000A0ED9(129, (int)D_00170DE4);
    mc_sprintf((int)text_buffer, (int)D_00170E11, player_character->magicka, player_character->max_magicka);
    text_draw_colored((int)text_buffer, 238, 20, 145, 141);
    func_0012DB50(4);
    *(int *)&l_1C = picklist_frame(&D_001A9AB8);
    if (((int)(short)l_1C) > (-1)) {
        l_30 = (struct record *)*(int *)(text_macro_fpc + (((int)(unsigned short)D_001A9AB8.selected) << 2));
        spellbook_close();
        if ((player_character->conditions & 0x100) != 0) {
            hud_message_add(D_00184634);
        } else if (((struct bf8_7_1 *)&D_001940D4)->f != 0) {
            l_24 = (spell_cost(&l_30->data.spell, player_character) * func_00042380()) / 100;
            if (l_30->data.spell.id == 92) l_24 = 0;
            if (((int)(unsigned char)l_30->data.spell.name[func_000A0DF4(l_30->data.spell.name) + 1]) == 36) {
                l_24 >>= 2;
            }
            if ((player_character->magicka + D_001959FC) < l_24) {
                hud_message_add((int)D_00170E17);
                return;
            }
            D_00195F62 = l_24;
            *(int *)&spell_ready_touch = (*(int *)&spell_ready_missile = 0);
            if (D_001959FC != 0) {
                if (l_24 > D_001959FC) {
                    l_24 -= D_001959FC;
                    D_001959FC = 0;
                } else {
                    D_001959FC -= l_24;
                    D_00195F62 = 0;
                }
            }
            spell_add_skill_uses(&l_30->data.spell, 1);
            player_character->magicka -= l_24;
            l_2C = object_create_child(player_object->parent, 0, 89);
            l_2C->type = 9;
            l_2C->id = object_new_id(100);
            mc_memcpy(&l_2C->data.spell, &l_30->data.spell, 89, (int)D_00170DE4, 181, 4);
            if (cast_player_spell(l_2C) != 0) object_delete(l_2C);
        }
        return;
    }
    spellbook_draw_spell((selected_spell = (struct spell *)(*(int *)(text_macro_fpc + (((int)(unsigned short)D_001A9AB8.selected) << 2)) + 71)));
    if (((int)(unsigned char)msgbox_kind) == 2) {
        mc_strncpy(D_001A9AB8.entries[D_001A9AB8.selected].text, selected_spell->name, 40, (int)D_00170DE4, 193);
    }
    if (key_down_esc != 0 || ((int)(short)l_1C) == (-2)) spellbook_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 9; (*(int *)&l_18)++) {
        if (mouse_x > *(short *)(spellbook_buttons + (((int)(short)l_18) * 12)) && mouse_x < *(short *)(D_0017B6D4 + (((int)(short)l_18) * 12)) && mouse_y > *(short *)(D_0017B6D2 + (((int)(short)l_18) * 12)) && mouse_y < *(short *)(D_0017B6D6 + (((int)(short)l_18) * 12))) {
            sound_play(205, player_object, 100);
            ((int (*)())(*(int *)(D_0017B6D8 + (((int)(short)l_18) * 12))))();
        }
    }
}

int spellbook_close(void)
{
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0) {
        D_001940D4 &= 251;
        picklist_free(&D_001A9AB8);
    }
    while (key_down_esc != 0);
    while (mouse_buttons != 0) func_0012B136();
    D_001940D8 &= 253;
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00170DE4, 222);
        window_image = -1751672937;
    }
    if (spellshop_icons != 0 && spellshop_icons != (-1751672937)) {
        mc_free(spellshop_icons, (int)D_00170DE4, 223);
        spellshop_icons = -1751672937;
    }
    if (spellbook_saved_screen != 0 && spellbook_saved_screen != (-1751672937)) {
        mc_free(spellbook_saved_screen, (int)D_00170DE4, 224);
        spellbook_saved_screen = -1751672937;
    }
    D_00196272 = 0;
    if (((struct bf8_5_1 *)&D_001940D8)->f != 0) {
        D_001940D8 &= 223;
        sheet_open(1);
    } else if (((struct bf8_7_1 *)&D_001940D8)->f != 0) {
        D_001940D8 &= 127;
        inventory_open(2, trade_mode, (int)(unsigned char)inv_right_icon);
    } else {
        D_00187CA8 = 1;
    }
    return 1;
}

void spellbook_draw_spell(struct spell *a1)
{
    short l_18;

    D_0012B508 = 145;
    func_000CD20E(172, 32, a1->icon);
    func_000CE31C((int)(*(char **)&spellshop_icons + (a1->element * 640)) + 24, (int)(*(char **)&screen_buffer + 10486), 16, 16, 40);
    func_000CE31C((int)(*(char **)&spellshop_icons + (a1->target * 640)), (int)&*(signed char *)(*(char **)&screen_buffer + 10445), 24, 16, 40);
    text_draw_colored((int)a1->name, 148, 20, 145, 141);
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 3; (*(int *)&l_18)++) {
        if ((a1->effects[(int)(short)l_18].type) == 255) continue;
        text_draw_centered_colored(*(int *)(spell_effect_names + ((a1->effects[(int)(short)l_18].type) << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 63), 145, 141);
        if ((a1->effects[(int)(short)l_18].subtype) != 255 && *(int *)(spell_effect_subtype_names + ((a1->effects[(int)(short)l_18].type) * 48) + ((a1->effects[(int)(short)l_18].subtype) << 2)) != 0) {
            text_draw_centered_colored(*(int *)(spell_effect_subtype_names + ((a1->effects[(int)(short)l_18].type) * 48) + ((a1->effects[(int)(short)l_18].subtype) << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 75), 145, 141);
        }
    }
}

void spellbook_effect_help(int a1)
{
{
    int l_1C;

    if (selected_spell->effects[(int)(short)*(short *)&a1].type == 255) return;
    spell_effect_slot = a1;
    D_001940D5 |= 1;
    msgbox_show_rsc((int)(short)(spell_effect_text_index((int)(short)*(short *)&a1) + 1200), 1);
}
}

void spellbook_effect1_button(void)
{
    spellbook_effect_help(0);
}

void spellbook_effect2_button(void)
{
    spellbook_effect_help(1);
}

void spellbook_effect3_button(void)
{
    spellbook_effect_help(2);
}

int spellbook_build_list(void)
{
    struct record *l_1C;

    *(short *)D_00190D64 = 0;
    l_1C = object_find_item(player_entity, 27, 0);
    if (l_1C == 0) return 0;
    if (l_1C->children == 0 && ((int)(unsigned short)(player_character->flags & 4)) == 0) return 0;
    picklist_init(&D_001A9AB8, 27, 30, 111, 131, 144, 29, 8, 15, 144, 150, 8, 15, 144, 45, 9, 104, 146, 146, 244, 114, 0);
    object_foreach(l_1C->children, (int)spellbook_add_spell_cb);
    if (((int)(unsigned short)(player_character->flags & 4)) != 0) {
        l_1C = player_entity->children;
        while (l_1C != 0) {
            if (l_1C->type == 28) {
                object_foreach(l_1C->children, (int)spellbook_add_spell_cb);
                break;
            }
            l_1C = l_1C->next;
        }
    }
    if (*(short *)D_00190D64 == 0) {
        picklist_free(&D_001A9AB8);
        msgbox_show_rsc(12, 1);
        return 0;
    }
    D_001940D8 &= 254;
    D_001940D4 |= 4;
    return 1;
}

void spellbook_delete_button(void)
{
    object_delete((struct record *)((int)selected_spell - 71));
    picklist_free(&D_001A9AB8);
    if (spellbook_build_list() != 0) return;
    spellbook_close();
}

void spellbook_up_button(void)
{
    struct record *l_18;

    l_18 = (struct record *)((int)selected_spell - 71);
    if (l_18->prev == 0) return;
    func_0008E152(l_18, l_18->prev);
    picklist_free(&D_001A9AB8);
    spellbook_build_list();
}

void spellbook_down_button(void)
{
    struct record *l_18;

    l_18 = (struct record *)((int)selected_spell - 71);
    if (l_18->next == 0) return;
    func_0008E152(l_18, l_18->next);
    picklist_free(&D_001A9AB8);
    spellbook_build_list();
}

int func_00042380(void)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = 100;
    l_20 = (((unsigned)game_minutes) / 1440) & 31;
    l_1C = (((unsigned)(game_minutes + 5760)) / 1440) & 31;
    for (l_30 = 0; l_30 < 27; l_30++) {
        if (player_character->equipped[l_30] == 0) continue;
        l_34 = (int)player_character + 371;
        if (((int)(short)*(short *)((char *)l_34 + 67)) == (-1)) continue;
        for (l_2C = 0; l_2C < 10; l_2C++) {
            if (((int)(short)*(short *)((char *)((l_2C << 2) + l_34) + 67)) == (-1)) break;
            if (((int)(short)*(short *)((char *)((l_2C << 2) + l_34) + 67)) == 3) {
                switch (*(unsigned short *)((char *)((l_2C << 2) + l_34) + 69)) {
                case 0:
                case 1:
                case 2:
                case 3:
                    if ((short)*(unsigned char *)(D_0018320A + (((unsigned)game_minutes) / 43200)) == *(short *)((char *)((l_2C << 2) + l_34) + 69)) {
                        l_28 = 75;
                    }
                    break;
                case 4:
                    if (l_20 == 0 || l_1C == 0) l_28 = 75;
                    break;
                case 5:
                    if (l_20 == 8 || l_20 == 24 || l_1C == 8 || l_1C == 24) l_28 = 75;
                    break;
                case 6:
                    if (l_20 == 16 || l_1C == 16) l_28 = 75;
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                    for (l_24 = 0; l_24 < creature_count; ) {
                        if (((int)(unsigned char)*(signed char *)(monster_category + D_00190504[l_24]->data.character.race)) == (((int)(short)*(short *)((char *)((l_2C << 2) + l_34) + 69)) - 7)) {
                            l_28 = 75;
                        }
                    }
                }
            }
        }
    }
    return l_28;
}
