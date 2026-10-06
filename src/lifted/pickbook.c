/* pickbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern iptr screen_buffer;
extern char D_00170DE4[];
extern char D_00170DEF[];
extern char D_00170E11[];
extern char D_00170E17[];
extern struct spell *selected_spell;
extern struct rect spellbook_buttons[];
extern char spell_effect_names[];
extern char spell_effect_subtype_names[];
extern char D_0018320A[];
extern int D_00184634;
extern char monster_category[];
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern struct record *creature_list[];
extern char scratch_190d64[];
extern char scratch_190de4[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D8;
extern int spell_points_bonus;
extern struct record *player_entity;
extern struct record *player_object;
extern int creature_count;
extern struct record *spell_ready_missile;
extern struct record *spell_ready_touch;
extern iptr magic_window_image;
extern struct character *player_character;
extern iptr window_image;
extern int game_minutes;
extern int trade_mode;
extern short spell_effect_slot;
extern short spell_ready_cost;
extern signed char msgbox_kind;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char inv_right_icon;
extern iptr spellbook_saved_screen;
extern struct picklist shared_picklist;

extern int spell_effect_text_index(short);
extern int spell_cost(struct spell *, struct character *);
extern int sheet_open(short);
extern int spellbook_open(short);
extern int cast_player_spell(struct record *);
extern int sound_play(int, struct record *, int);
extern iptr hud_message_add(iptr);
extern int picklist_frame(struct picklist *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_find_item(struct record *, short, short);
extern int object_new_id(int);
extern int inventory_open(int, int, int);
extern int mc_free();
extern int mc_strncpy();
extern int strlen();
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);
extern int mc_memcpy();
extern int xn_draw_spell_icon();
extern int xn_draw_copy_rect_stride_bytes();
extern int xn_mouse_poll_clamped();
extern int xn_font_select();
extern int xn_draw_image_transparent();
extern void spell_add_skill_uses(struct spell *, int);
extern void msgbox_show_rsc(int, int);
extern void text_draw_coloured(iptr, int, int, int, unsigned char);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
extern void picklist_init(struct picklist *, short, short, short, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(struct picklist *, char *, short);
extern void picklist_free(struct picklist *);
extern void object_swap_siblings(struct record *, struct record *);
extern void object_foreach(struct record *, void (*)());
int spellbook_close(void);
int spellbook_build_list(void);
int spell_cost_item_percent(void);
void spellbook_add_spell_cb(struct record *);
void spellbook_draw_spell(struct spell *);
void spellbook_effect_help(int);
#pragma aux mc_set_location parm routine [];

void spellbook_add_spell_cb(struct record *object)
{
    struct spell *spell;
    int cost;

    if (object->type != 9) return;
    spell = &object->data.spell;
    if (((int)(unsigned char)spell->name[0]) == 33 || ((int)(unsigned char)spell->name[strlen(spell->name) + 1]) == 36) {
        cost = spell_cost(spell, player_character);
        if (((int)(unsigned char)spell->name[strlen(spell->name) + 1]) == 36) {
            cost >>= 2;
            mc_set_location(62, (iptr)D_00170DE4);
            mc_sprintf((iptr)text_buffer, (iptr)D_00170DEF, cost, spell->name);
        } else {
            mc_set_location(65, (iptr)D_00170DE4);
            mc_sprintf((iptr)text_buffer, (iptr)D_00170DEF, cost, &spell->name[1]);
        }
    } else {
        mc_set_location(68, (iptr)D_00170DE4);
        mc_sprintf((iptr)text_buffer, (iptr)D_00170DEF, spell_cost(spell, player_character), spell->name);
    }
    picklist_add(&shared_picklist, text_buffer, 0);
    *(iptr *)(scratch_190de4 + (((int)(short)(*(short *)scratch_190d64)++) << 2)) = (iptr)object;
}

void spellbook_frame(void)
{
    struct record *spell_object;
    struct record *new_spell;
    short *image;
    int cost;
    int unused;
    short picked;
    short button;

    if (spellbook_open(0) == 0) return;
    mc_memcpy(screen_buffer, spellbook_saved_screen, 64000, (iptr)D_00170DE4, 123, 4);
    image = (short *)window_image;
    xn_draw_image_transparent((unsigned short)image[0], (unsigned short)image[1], (unsigned short)image[2], (unsigned short)image[3], (char *)image + 12);
    mc_set_location(129, (iptr)D_00170DE4);
    mc_sprintf((iptr)text_buffer, (iptr)D_00170E11, player_character->magicka, player_character->max_magicka);
    text_draw_coloured((iptr)text_buffer, 238, 20, 145, 141);
    xn_font_select(4);
    *(int *)&picked = picklist_frame(&shared_picklist);
    if (((int)(short)picked) > (-1)) {
        spell_object = (struct record *)*(iptr *)(scratch_190de4 + (((int)(unsigned short)shared_picklist.selected) << 2));
        spellbook_close();
        if ((player_character->conditions & 0x100) != 0) {
            hud_message_add(D_00184634);
        } else if (((struct bf8_7_1 *)&D_001940D4)->f != 0) {
            cost = (spell_cost(&spell_object->data.spell, player_character) * spell_cost_item_percent()) / 100;
            if (spell_object->data.spell.id == 92) cost = 0;
            if (((int)(unsigned char)spell_object->data.spell.name[strlen(spell_object->data.spell.name) + 1]) == 36) {
                cost >>= 2;
            }
            if ((player_character->magicka + spell_points_bonus) < cost) {
                hud_message_add((iptr)D_00170E17);
                return;
            }
            spell_ready_cost = cost;
            *(int *)&spell_ready_touch = (*(int *)&spell_ready_missile = 0);
            if (spell_points_bonus != 0) {
                if (cost > spell_points_bonus) {
                    cost -= spell_points_bonus;
                    spell_points_bonus = 0;
                } else {
                    spell_points_bonus -= cost;
                    spell_ready_cost = 0;
                }
            }
            spell_add_skill_uses(&spell_object->data.spell, 1);
            player_character->magicka -= cost;
            new_spell = object_create_child(player_object->parent, 0, 89);
            new_spell->type = 9;
            new_spell->id = object_new_id(100);
            mc_memcpy(&new_spell->data.spell, &spell_object->data.spell, 89, (iptr)D_00170DE4, 181, 4);
            if (cast_player_spell(new_spell) != 0) object_delete(new_spell);
        }
        return;
    }
    spellbook_draw_spell((selected_spell = (struct spell *)(*(iptr *)(scratch_190de4 + (((int)(unsigned short)shared_picklist.selected) << 2)) + 71)));
    if (((int)(unsigned char)msgbox_kind) == 2) {
        mc_strncpy(shared_picklist.entries[shared_picklist.selected].text, selected_spell->name, 40, (iptr)D_00170DE4, 193);
    }
    if (key_down_esc != 0 || ((int)(short)picked) == (-2)) spellbook_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    *(int *)&button = 0;
    for (; ((int)(short)button) < 9; (*(int *)&button)++) {
        if (mouse_x > spellbook_buttons[(int)(short)button].x0 && mouse_x < spellbook_buttons[(int)(short)button].x1 && mouse_y > spellbook_buttons[(int)(short)button].y0 && mouse_y < spellbook_buttons[(int)(short)button].y1) {
            sound_play(205, player_object, 100);
            spellbook_buttons[(int)(short)button].handler();
        }
    }
}

int spellbook_close(void)
{
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0) {
        D_001940D4 &= 251;
        picklist_free(&shared_picklist);
    }
    while (key_down_esc != 0);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    D_001940D8 &= 253;
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (iptr)D_00170DE4, 222);
        window_image = -1751672937;
    }
    if (magic_window_image != 0 && magic_window_image != (-1751672937)) {
        mc_free(magic_window_image, (iptr)D_00170DE4, 223);
        magic_window_image = -1751672937;
    }
    if (spellbook_saved_screen != 0 && spellbook_saved_screen != (-1751672937)) {
        mc_free(spellbook_saved_screen, (iptr)D_00170DE4, 224);
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

void spellbook_draw_spell(struct spell *spell)
{
    short i;

    D_0012B508 = 145;
    xn_draw_spell_icon(172, 32, spell->icon);
    xn_draw_copy_rect_stride_bytes((iptr)(*(char **)&magic_window_image + (spell->element * 640)) + 24, (iptr)(*(char **)&screen_buffer + 10486), 16, 16, 40);
    xn_draw_copy_rect_stride_bytes((iptr)(*(char **)&magic_window_image + (spell->target * 640)), (iptr)&*(signed char *)(*(char **)&screen_buffer + 10445), 24, 16, 40);
    text_draw_coloured((iptr)spell->name, 148, 20, 145, 141);
    *(int *)&i = 0;
    for (; ((int)(short)i) < 3; (*(int *)&i)++) {
        if ((spell->effects[(int)(short)i].type) == 255) continue;
        text_draw_centred_coloured(*(int *)(spell_effect_names + ((spell->effects[(int)(short)i].type) << 2)), 219, (int)(short)((*(int *)&i * 38) + 63), 145, 141);
        if ((spell->effects[(int)(short)i].subtype) != 255 && *(int *)(spell_effect_subtype_names + ((spell->effects[(int)(short)i].type) * 48) + ((spell->effects[(int)(short)i].subtype) << 2)) != 0) {
            text_draw_centred_coloured(*(int *)(spell_effect_subtype_names + ((spell->effects[(int)(short)i].type) * 48) + ((spell->effects[(int)(short)i].subtype) << 2)), 219, (int)(short)((*(int *)&i * 38) + 75), 145, 141);
        }
    }
}

void spellbook_effect_help(int slot)
{
{
    int unused;

    if (selected_spell->effects[(int)(short)*(short *)&slot].type == 255) return;
    spell_effect_slot = slot;
    D_001940D5 |= 1;
    msgbox_show_rsc((int)(short)(spell_effect_text_index((int)(short)*(short *)&slot) + 1200), 1);
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
    struct record *object;

    *(short *)scratch_190d64 = 0;
    object = object_find_item(player_entity, 27, 0);
    if (object == 0) return 0;
    if (object->children == 0 && ((int)(unsigned short)(player_character->flags & 4)) == 0) return 0;
    picklist_init(&shared_picklist, 27, 30, 111, 131, 144, 29, 8, 15, 144, 150, 8, 15, 144, 45, 9, 104, 146, 146, 244, 114, 0);
    object_foreach(object->children, spellbook_add_spell_cb);
    if (((int)(unsigned short)(player_character->flags & 4)) != 0) {
        object = player_entity->children;
        while (object != 0) {
            if (object->type == 28) {
                object_foreach(object->children, spellbook_add_spell_cb);
                break;
            }
            object = object->next;
        }
    }
    if (*(short *)scratch_190d64 == 0) {
        picklist_free(&shared_picklist);
        msgbox_show_rsc(12, 1);
        return 0;
    }
    D_001940D8 &= 254;
    D_001940D4 |= 4;
    return 1;
}

void spellbook_delete_button(void)
{
    object_delete((struct record *)((iptr)selected_spell - 71));
    picklist_free(&shared_picklist);
    if (spellbook_build_list() != 0) return;
    spellbook_close();
}

void spellbook_up_button(void)
{
    struct record *object;

    object = (struct record *)((iptr)selected_spell - 71);
    if (object->prev == 0) return;
    object_swap_siblings(object, object->prev);
    picklist_free(&shared_picklist);
    spellbook_build_list();
}

void spellbook_down_button(void)
{
    struct record *object;

    object = (struct record *)((iptr)selected_spell - 71);
    if (object->next == 0) return;
    object_swap_siblings(object, object->next);
    picklist_free(&shared_picklist);
    spellbook_build_list();
}

int spell_cost_item_percent(void)
{
    struct item *item;
    int slot;
    int i;
    int percent;
    int j;
    int day;
    int day_ahead;

    percent = 100;
    day = (((unsigned)game_minutes) / 1440) & 31;
    day_ahead = (((unsigned)(game_minutes + 5760)) / 1440) & 31;
    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == 0) continue;
        item = (struct item *)((char *)player_character + 371);
        if (item->enchantments[0].type == -1) continue;
        for (i = 0; i < 10; i++) {
            if (item->enchantments[i].type == (-1)) break;
            if (item->enchantments[i].type == 3) {
                switch ((unsigned short)item->enchantments[i].param) {
                case 0:
                case 1:
                case 2:
                case 3:
                    if ((short)*(unsigned char *)(D_0018320A + (((unsigned)game_minutes) / 43200)) == item->enchantments[i].param) {
                        percent = 75;
                    }
                    break;
                case 4:
                    if (day == 0 || day_ahead == 0) percent = 75;
                    break;
                case 5:
                    if (day == 8 || day == 24 || day_ahead == 8 || day_ahead == 24) percent = 75;
                    break;
                case 6:
                    if (day == 16 || day_ahead == 16) percent = 75;
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                    for (j = 0; j < creature_count; ) {
                        if (((int)(unsigned char)*(signed char *)(monster_category + creature_list[j]->data.character.race)) == (((int)(short)item->enchantments[i].param) - 7)) {
                            percent = 75;
                        }
                    }
                }
            }
        }
    }
    return percent;
}
