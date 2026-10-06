/* spells.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"
#include "doslow.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern short font_height;
extern signed char key_down_esc;
extern iptr screen_buffer;
extern char D_00170B13[];
extern char D_00170B1E[];
extern char D_00170B4C[];
extern char D_00170B69[];
extern struct spell *selected_spell;
extern signed char spell_effect_school[];
extern signed char spell_effect_target_class[];
extern char spell_effect_settings[];
extern char spell_effect_costs[];
extern char spell_effect_cost_index[];
extern char spell_effect_cost_formula[];
extern char spell_target_cost_factor[];
extern iptr D_0017D1F6;
extern signed char magic_school_skills[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern struct record *player_entity;
extern struct record *player_object;
extern char cheat_flags[];
extern struct image *list_popup_image;
extern struct spell *spell_records;
extern struct record *scratch_object;
extern iptr magic_window_image;
extern iptr list_popup_callback;
extern iptr window_image;
extern char *scratch_buffer;
extern short spell_effect_slot;
extern short D_00195F3C;
extern short D_00195F3E;
extern short D_00195F40;
extern short D_00195F42;
extern char list_popup_picklist[];
extern char spellmaker_spell[];
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern char spell_effect_cost_current[];
extern iptr spellmaker_settings_image;
extern short D_00199628;
extern short D_0019962C;
extern char spellmaker_settings_kind[];
extern signed char D_0019962F;
extern signed char D_00199630;

extern int func_00037AB7(void);
extern int spell_effect_text_index(short);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern int disk_write_arena2_file(char *, iptr, int);
extern int list_popup_update(void);
extern int gold_can_afford(int);
extern int picklist_poll(iptr);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_find_item(struct record *, short, short);
extern int object_count_type(struct record *, short);
extern int object_new_id(int);
extern int spell_cost_formula_dispatch(int);
extern void xn_mouse_poll_clamped(void);
extern void xn_mouse_cursor_move(int, int);
extern int xn_font_select(int);
extern void xn_draw_image(int, int, int, int, char *);
extern void spellmaker_adjust_value(iptr, short, int, int);
extern void skill_add_uses(int, int);
extern void msgbox_show_string(iptr, int);
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void list_popup_open(iptr);
extern void msgbox_choice_rsc(short, short, short, short, unsigned char, unsigned char, unsigned char);
extern void gold_spend(int);
extern void inpstr_begin_text(iptr, short);
extern void picklist_free(iptr);
extern void picklist_draw(iptr, int);
extern void object_foreach(struct record *, void (*)());
int spellmaker_allowed_targets(void);
int spellmaker_new(void);
int spellbook_has_spell_id(unsigned char);
struct spell *spells_pick_list(void);
iptr spells_std_name_list(iptr);
void spellmaker_allowed_targets_cb(unsigned char);
void spellmaker_pick_subtype_cb(int);
void spell_assign_new_id(void);
void spellbook_find_id_cb(struct record *);
void spells_std_delete(void);
void spells_std_edit(void);
void spells_std_append(void);
#pragma aux mc_set_location parm routine [];

int spellmaker_close(void)
{
    while (key_down_esc != 0);
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        if (((struct bf8_0_1 *)&cheat_flags)->f != 0) {
            spells_std_delete();
            return 0;
        }
        D_001940D5 |= 1;
        msgbox_show_rsc(1814, 1);
        return 0;
    }
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free((void *)window_image, D_00170B13, 732);
        window_image = -1751672937;
    }
    if (magic_window_image != 0 && magic_window_image != (-1751672937)) {
        mc_free((void *)magic_window_image, D_00170B13, 733);
        magic_window_image = -1751672937;
    }
    if (spellmaker_settings_image != 0 && spellmaker_settings_image != (-1751672937)) {
        mc_free((void *)spellmaker_settings_image, D_00170B13, 734);
        spellmaker_settings_image = -1751672937;
    }
    D_00196272 = 0;
    return 1;
}

void spellmaker_enter_name(void)
{
    iptr prompt;

    prompt = (iptr)scratch_buffer + 55000;
    mc_set_location(744, D_00170B13);
    mc_sprintf((char *)prompt, D_00170B1E, D_0017D1F6);
    *(signed char *)((char *)(strlen((char *)prompt) + prompt) + 1) = 0;
    inpstr_begin_text((iptr)selected_spell + 47, 24);
    msgbox_show_string(prompt, 2);
}

int spellmaker_element_fire(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1801, 1);
        return 0;
    }
    if (func_00037AB7() > 0) selected_spell->element = 0;
    return 0;
}

int spellmaker_element_frost(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1802, 1);
        return 0;
    }
    if (func_00037AB7() > 0) selected_spell->element = 1;
    return 0;
}

int spellmaker_element_poison(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1803, 1);
        return 0;
    }
    if (func_00037AB7() > 0) selected_spell->element = 2;
    return 0;
}

int spellmaker_element_shock(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1804, 1);
        return 0;
    }
    if (func_00037AB7() > 0) selected_spell->element = 3;
    return 0;
}

int spellmaker_element_magic(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1805, 1);
        return 0;
    }
    selected_spell->element = 4;
    return 0;
}

int spellmaker_target_caster(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1806, 1);
        return 0;
    }
    if (spellmaker_allowed_targets() != 1) selected_spell->target = 0;
    return 0;
}

int spellmaker_target_touch(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1807, 1);
        return 0;
    }
    if (spellmaker_allowed_targets() != 0) selected_spell->target = 1;
    return 0;
}

int spellmaker_target_distance(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1808, 1);
        return 0;
    }
    if (spellmaker_allowed_targets() != 0) selected_spell->target = 2;
    return 0;
}

int spellmaker_target_area(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1809, 1);
        return 0;
    }
    if (spellmaker_allowed_targets() != 0) selected_spell->target = 3;
    return 0;
}

int spellmaker_target_area_at_distance(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_001940D5 |= 1;
        msgbox_show_rsc(1810, 1);
        return 0;
    }
    if (spellmaker_allowed_targets() != 0) selected_spell->target = 4;
    return 0;
}

int spellmaker_allowed_targets(void)
{
    D_0019962F = 2;
    spellmaker_allowed_targets_cb(selected_spell->effects[0].type);
    spellmaker_allowed_targets_cb(selected_spell->effects[1].type);
    spellmaker_allowed_targets_cb(selected_spell->effects[2].type);
    return (int)(unsigned char)D_0019962F;
}

void spellmaker_allowed_targets_cb(unsigned char effect_type)
{
    if (((int)(unsigned char)effect_type) == 255) return;
    if (((int)(unsigned char)spell_effect_target_class[(int)(unsigned char)effect_type]) == 2) return;
    D_0019962F = spell_effect_target_class[(int)(unsigned char)effect_type];
}

int spellmaker_buy(void)
{
    struct record *object;
    int price;

    if (selected_spell->effects[0].type == 255 && selected_spell->effects[1].type == 255 && selected_spell->effects[2].type == 255) {
        return 0;
    }
    if (selected_spell->name[0] == 0) {
        msgbox_show_rsc(1704, 1);
        return 0;
    }
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        if (((struct bf8_0_1 *)&cheat_flags)->f != 0) {
            spells_std_append();
            return 0;
        }
        D_001940D5 |= 1;
        msgbox_show_rsc(1812, 1);
        return 0;
    }
    price = (selected_spell->effect_costs[0] + selected_spell->effect_costs[1]) + selected_spell->effect_costs[2];
    price = (((int)(short)*(short *)(spell_target_cost_factor + (selected_spell->target * 2))) * price) * 2;
    if (gold_can_afford(price) == 0) {
        msgbox_show_rsc(1702, 1);
        return 0;
    }
    object = object_find_item(player_entity->children, 27, 0);
    if (object == 0) {
        msgbox_show_string((iptr)D_00170B4C, 1);
        return 0;
    }
    if (object_count_type(object->children, 9) > 128) {
        msgbox_show_rsc(1709, 1);
        return 0;
    }
    if (object == 0) {
        msgbox_show_rsc(1703, 1);
        return 0;
    }
    gold_spend(price);
    spell_assign_new_id();
    object = object_create_child(object, 0, 89);
    object->type = 9;
    object->id = object_new_id(100);
    mc_memcpy(&object->data.spell, selected_spell, 89, D_00170B13, 981, 4);
    sound_play(206, player_object, 110);
    spellmaker_new();
    msgbox_show_rsc(1705, 1);
    return 0;
}

int spellmaker_new(void)
{
    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        if (((struct bf8_0_1 *)&cheat_flags)->f != 0) {
            spells_std_edit();
            return 0;
        }
        D_001940D5 |= 1;
        msgbox_show_rsc(1813, 1);
        return 0;
    }
    selected_spell = (struct spell *)spellmaker_spell;
    mc_memset(selected_spell, 1, 89, D_00170B13, 1009, 4);
    mc_memset((void *)((iptr)selected_spell + 47), 0, 24, D_00170B13, 1010, 25);
    selected_spell->target = 0;
    selected_spell->element = 4;
    mc_memset(selected_spell, 255, 6, D_00170B13, 1015, 6);
    selected_spell->effect_costs[0] = (selected_spell->effect_costs[1] = (selected_spell->effect_costs[2] = 0));
    D_0019962C = 65535;
    return 0;
}

void spellmaker_pick_subtype_cb(int subtype)
{
    short effect_type;

    effect_type = selected_spell->effects[(int)(short)spell_effect_slot].type;
    mc_memcpy(spell_effect_cost_current, (void *)(((iptr)spell_effect_costs) + (((int)(unsigned char)*(signed char *)(spell_effect_cost_index + ((((int)(short)effect_type) * 12) + ((int)(short)*(short *)&subtype)))) << 3)), 8, D_00170B13, 1030, 8);
    selected_spell->effects[(int)(short)spell_effect_slot].subtype = *(signed char *)&subtype;
    *(signed char *)spellmaker_settings_kind = *(signed char *)(spell_effect_settings + ((((int)(short)effect_type) * 12) + ((int)(short)*(short *)&subtype)));
    D_00199628 = 0;
}

int spellmaker_find_effect(short effect_type)
{
    slot16 i;

    *(int *)&i = 0;
    for (; ((int)(short)i) < 3; (*(int *)&i)++) {
        if ((short)((unsigned short)selected_spell->effects[(int)(short)i].type) == effect_type) {
            return (int)(short)i;
        }
    }
    return -1;
}

void spellmaker_duration_base(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 14 + (((int)(short)spell_effect_slot) * 3)), (int)(short)delta, 60, 0);
}

void spellmaker_duration_plus(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 14 + (((int)(short)spell_effect_slot) * 3)) + 1, (int)(short)delta, 60, 0);
}

void spellmaker_duration_per_level(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 14 + (((int)(short)spell_effect_slot) * 3)) + 2, (int)(short)delta, 20, 0);
}

void spellmaker_chance_base(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 23 + (((int)(short)spell_effect_slot) * 3)), (int)(short)delta, 100, 0);
}

void spellmaker_chance_plus(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 23 + (((int)(short)spell_effect_slot) * 3)) + 1, (int)(short)delta, 100, 0);
}

void spellmaker_chance_per_level(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 23 + (((int)(short)spell_effect_slot) * 3)) + 2, (int)(short)delta, 20, 0);
}

void spellmaker_magnitude_base_min(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 32 + (((int)(short)spell_effect_slot) * 5)), (int)(short)delta, 100, 1);
}

void spellmaker_magnitude_base_max(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 32 + (((int)(short)spell_effect_slot) * 5)) + 1, (int)(short)delta, 100, -1);
}

void spellmaker_magnitude_plus_min(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 32 + (((int)(short)spell_effect_slot) * 5)) + 2, (int)(short)delta, 100, 1);
}

void spellmaker_magnitude_plus_max(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 32 + (((int)(short)spell_effect_slot) * 5)) + 3, (int)(short)delta, 100, -1);
}

void spellmaker_magnitude_per_level(short delta)
{
    spellmaker_adjust_value((iptr)((char *)selected_spell + 32 + (((int)(short)spell_effect_slot) * 5)) + 4, (int)(short)delta, 20, 0);
}

void spellmaker_settings_close(void)
{
    D_001940D5 |= 32;
    *(signed char *)spellmaker_settings_kind = 0;
}

void spellmaker_effect_rows(void)
{
    slot16 row;

    *(int *)&row = 0;
    for (; ((int)(short)row) < 3; (*(int *)&row)++) {
        if (selected_spell->effects[(int)(short)row].type == 255) continue;
        if (((int)(unsigned char)(mouse_buttons & 2)) != 0 && mouse_buttons_prev != mouse_buttons && ((int)(short)mouse_y) > ((((int)(short)row) << 5) + 30) && ((int)(short)mouse_y) < (((((int)(short)row) << 5) + 30) + ((int)(short)font_height))) {
            D_001940D5 |= 1;
            msgbox_show_rsc((int)(short)(spell_effect_text_index((int)(short)row) + 1200), 1);
        }
        if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && mouse_buttons_prev != mouse_buttons && ((int)(short)mouse_y) > ((((int)(short)row) << 5) + 30) && ((int)(short)mouse_y) < (((((int)(short)row) << 5) + 30) + ((int)(short)font_height))) {
            D_0019962C = *(int *)&row;
            msgbox_choice_rsc(1708, 11, 10, 0, 101, 100, 0);
        }
    }
}

int list_popup_poll(void)
{
    slot16 choice;

    xn_draw_image((int)(short)D_00195F40, (int)(short)D_00195F3E, (int)(short)D_00195F42, (int)(short)D_00195F3C, (char *)((iptr)list_popup_image + 12));
    if (key_down_esc != 0) {
        if (((iptr)spellmaker_pick_subtype_cb) == list_popup_callback) {
            selected_spell->effects[(int)(short)spell_effect_slot].type = 255;
        }
        D_001940D4 &= 251;
        picklist_free((iptr)list_popup_picklist);
        return -2;
    }
    *(int *)&choice = picklist_poll((iptr)list_popup_picklist) - 1;
    if (((int)(short)choice) > (-1)) {
        D_001940D4 &= 251;
        picklist_free((iptr)list_popup_picklist);
        return (int)(short)choice;
    }
    picklist_draw((iptr)list_popup_picklist, 0);
    return -1;
}

int spell_icon_cycle(void)
{
    slot16 step;

    if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        *(int *)&step = -1;
    } else {
        *(int *)&step = 1;
    }
    selected_spell->icon = (selected_spell->icon + ((int)(short)step)) % 69;
    return 0;
}

int spell_icon_next(void)
{
    selected_spell->icon = (selected_spell->icon + 1) % 69;
    return 0;
}

int spell_icon_prev(void)
{
    selected_spell->icon = (selected_spell->icon - 1) % 69;
    return 0;
}

void spell_assign_new_id(void)
{
    struct spell *spells;
    int i;
    unsigned char id;
    slot16 is_free;

    *(int *)&is_free = 0;
    id = 0;
    spells = spell_records;
    while ((short)is_free == 0) {
        *(int *)&is_free = 1;
        for (i = 0; ((int)(short)*(short *)&i) < 128; i++) {
            if (spells[*(short *)&i].name[0] == 0) continue;
            if (spells[*(short *)&i].id == id) {
                id++;
                *(int *)&is_free = 0;
                break;
            }
        }
        if ((short)is_free != 0 && spellbook_has_spell_id((int)(unsigned char)id) != 0) {
            id++;
            *(int *)&is_free = 0;
        }
    }
    selected_spell->id = id;
}

void spellbook_find_id_cb(struct record *object)
{
    if (((int)(unsigned char)(signed char)object->type) != 9) return;
    if ((signed char)object->data.spell.id != D_00199630) return;
    *(iptr *)&scratch_object = (iptr)object;
}

int spellbook_has_spell_id(unsigned char id)
{
    struct record *spellbook;
    int found;

    *(iptr *)&scratch_object = 0;
    D_00199630 = id;
    spellbook = object_find_item(player_entity->children, 27, 0);
    object_foreach(spellbook->children, spellbook_find_id_cb);
    if ((iptr)scratch_object != 0) {
        found = 1;
    } else {
        found = 0;
    }
    return found;
}

struct spell *spells_pick_list(void)
{
    slot16 choice;
    struct spell *spells;
    slot16 i;
    iptr names;
    short unused;

    names = spells_std_name_list(0);
    if (names == 0) return 0;
    xn_font_select(4);
    list_popup_open(names);
    spells = (struct spell *)scratch_buffer;
    D_001940D4 |= 1;
    for (;;) {
        keys_world_actions();
        xn_mouse_poll_clamped();
        *(int *)&choice = list_popup_update();
        if (((int)(short)choice) > (-1)) {
            *(int *)&i = 0;
            while (spells[(int)(short)i].name[0] == 0) (*(int *)&i)++;
            while ((short)choice != 0) {
                (*(int *)&i)++;
                while (spells[(int)(short)i].name[0] == 0) (*(int *)&i)++;
                (*(int *)&choice)--;
            }
            return &spells[(int)(short)i];
        }
        if (((int)(short)choice) == (-2)) return 0;
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        mc_memcpy((void *)DOS_LOW(0xA0000), (void *)screen_buffer, 64000, D_00170B13, 1560, 4);
    }
}

iptr spells_std_name_list(iptr ids)
{
    iptr names;
    pslot16 id_count;
    struct spell *spells;
    slot16 i;
    iptr text;
    slot16 count;

    if (ids != 0) *(iptr *)&id_count = (iptr)(memchr((char *)ids, 255, 1000) - ids);
    names = (iptr)scratch_buffer + 20000;
    text = (iptr)scratch_buffer + 21000;
    spells = (struct spell *)scratch_buffer;
    disk_read_file(D_00170B69, (iptr)scratch_buffer);
    *(int *)&i = 0;
    *(int *)&count = *(int *)&i;
    for (; ((int)(short)i) < 128; (*(int *)&i)++) {
        if (spells[(int)(short)i].name[0] == 0) continue;
        if (ids != 0) if (memchr((char *)ids, spells[(int)(short)i].id, (int)(short)id_count) == 0) continue;
        mc_strncpy((char *)text, spells[(int)(short)i].name, 4, D_00170B13, 1587);
        *(iptr *)(((char *)names + (((int)(short)(*(int *)&count)++) << PTR_SHIFT))) = text;
        text += strlen((char *)text) + 1;
    }
    *(iptr *)((char *)((((int)(short)count) << PTR_SHIFT) + names)) = 0;
    if ((short)count == 0) return 0;
    return names;
}

void spells_std_delete(void)
{
    struct spell *spell;

    spell = spells_pick_list();
    if (spell == 0) return;
    spell->name[0] = 0;
    disk_write_arena2_file(D_00170B69, (iptr)scratch_buffer, 11392);
}

void spells_std_edit(void)
{
    struct spell *spell;

    spell = spells_pick_list();
    if (spell == 0) return;
    mc_memcpy(selected_spell, spell, 89, D_00170B13, 1654, 4);
    spell->name[0] = 0;
    disk_write_arena2_file(D_00170B69, (iptr)scratch_buffer, 11392);
}

void spells_std_append(void)
{
    struct spell *spell;

    spell_assign_new_id();
    spell = (struct spell *)scratch_buffer;
    disk_read_file(D_00170B69, (iptr)scratch_buffer);
    while (spell->name[0] != 0) spell++;
    mc_memcpy(spell, selected_spell, 89, D_00170B13, 1667, 4);
    disk_write_arena2_file(D_00170B69, (iptr)scratch_buffer, 11392);
    msgbox_show_rsc(1706, 1);
}

int spell_cost(struct spell *spell, struct character *caster)
{
    int i;
    int effect_cost;
    int total;
    int saved_slot;

    total = 0;
    saved_slot = (int)(short)spell_effect_slot;
    selected_spell = spell;
    for (i = 0; i < 3; i++) {
        if (spell->effects[i].type == 255) continue;
        spell_effect_slot = i;
        if (spell->effects[i].subtype != 255) {
            mc_memcpy(spell_effect_cost_current, (void *)(((iptr)spell_effect_costs) + (((int)(unsigned char)*(signed char *)(spell_effect_cost_index + ((spell->effects[i].type * 12) + spell->effects[i].subtype))) << 3)), 8, D_00170B13, 1684, 8);
        } else {
            mc_memcpy(spell_effect_cost_current, (void *)(((iptr)spell_effect_costs) + (((int)(unsigned char)*(signed char *)(spell_effect_cost_index + (spell->effects[i].type * 12))) << 3)), 8, D_00170B13, 1686, 8);
        }
        if (spell->effects[i].type >= 51) total++;
        effect_cost = spell_cost_formula_dispatch(((int)(unsigned char)*(signed char *)(spell_effect_cost_formula + spell->effects[i].type)) - 1);
        effect_cost = ((110 - caster->skills[(int)(unsigned char)magic_school_skills[(int)(unsigned char)spell_effect_school[spell->effects[i].type]]].value) * effect_cost) / 100;
        total += effect_cost;
    }
    spell_effect_slot = saved_slot;
    i = (((int)(short)*(short *)(spell_target_cost_factor + (spell->target * 2))) * total) >> 1;
    if (i < 5) i = 5;
    return i;
}

void spell_add_skill_uses(struct spell *spell, int uses)
{
    int i;

    for (i = 0; i < 3; i++) {
        if (spell->effects[i].type != 255) {
            skill_add_uses((int)(unsigned char)magic_school_skills[(int)(unsigned char)spell_effect_school[spell->effects[i].type]], uses);
        }
    }
}
