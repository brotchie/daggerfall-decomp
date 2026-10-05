/* disease.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

#include "records.h"

extern signed char D_0012B508;
extern char D_00175970[];
extern char D_0017597A[];
extern char D_0017599B[];
extern char D_001759C5[];
extern char D_001759EB[];
extern char D_001759F8[];
extern char D_0018320A[];
extern char monster_category[];
extern int D_00185083;
extern struct disease disease_table[];
extern short poison_table[];
extern short D_00186D85[];
extern short D_00186D87[];
extern short D_00186D89[];
extern signed char lycanthrope_attributes[];
extern char bio_modifiers[];
extern int D_0018DDE0;
extern struct record *creature_list[];
extern signed char scratch_190ce4[];
extern signed char D_00190D63;
extern signed char D_001940D8;
extern struct record *player_entity;
extern struct record *scratch_current_object;
extern struct spell *spell_records;
extern char D_00195B08[];
extern int trade_haggle_result;
extern int creature_count;
extern int calendar_month;
extern int D_00195B44;
extern char D_00195B84[];
extern char inpstr_result[];
extern struct character *player_character;
extern struct career *player_class;
extern int game_minutes;
extern signed char D_00195F24;
extern signed char D_00195F25;
extern char D_001961F5[];
extern unsigned char D_0019626F;
extern unsigned char D_00196271;
extern signed char game_mode;
extern signed char in_dungeon_water;
extern signed char player_ailment_flags;
extern signed char D_001962A0;
extern char extra_spell_points[];
extern int D_001A3AA8;
extern double trade_haggle_asking;

extern int damage_apply(struct record *, int, int);
extern int player_in_daylight(void);
extern int player_in_temple(void);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern int trade_haggle_counter(int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int spfx_resist_roll(int, int, struct character *, struct career *, int, int);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_find_item(struct record *, int, int);
extern int object_new_id(int);
extern int mc_strncpy();
extern int strlen();
extern int mc_memcpy();
extern int xn_math_approx_dist2d();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void paperdoll_draw(int, int);
extern void item_damage(struct record *, int);
extern void disease_toggle_memberships_cb(int);
extern void disease_become_vampire(void);
extern void item_enchantment_tick(struct item *, short, int);
extern void trade_haggle_show_offer(void);
extern void trade_counter_offer(void);
extern void fatigue_add(int);
extern void weapon_reload_hand_sprites(void);
extern void mode_pop(void);
extern void object_foreach(struct record *, int);
extern void inv_unequip_all_saved(void);
extern void inv_reequip_saved(void);
int disease_is_lycanthrope(void);
int enchant_spell_points_condition(int);
int item_artifact_equipped(int);
void disease_become_lycanthrope(int);
void disease_lycanthrope_shapechange(int);
void trade_haggle_close(int);

void disease_infect(struct record *target, unsigned char *disease_list, int disease_id, int no_resist)
{
    struct character *target_char;
    struct career *target_class;
    int count;
    struct record *disease;
    struct disease *disease_data;

    target_char = &target->data.character;
    target_class = &target_char->career;
    if (target_char == player_character && (player_class->immunity_flags & 64) != 0) return;
    if (no_resist == 0) if (spfx_resist_roll(2, 64, target_char, target_class, 4, *(int *)bio_modifiers) == 100) return;
    if (target_char->level == 1) return;
    disease = object_create_child(target, 0, 47);
    disease->type = 11;
    disease->flags = 32771;
    disease_data = &disease->data.disease;
    if (disease_list != 0) {
        count = 0;
        while (disease_list[count++] != 255);
        disease_id = disease_list[rand_range(0, count - 2)];
    }
    mc_memcpy(disease_data, &disease_table[disease_id], 47, (int)D_00175970, 83, 4);
    if (disease_data->days_left != 255) disease_data->days_left = rand_range(disease_data->days_left, disease_data->stage);
    disease_data->stage = 0;
}

void poison_apply(struct record *target, int poison_id, int no_resist)
{
    struct character *target_char;
    struct career *target_class;
    int unused;
    struct record *poison;
    struct disease *poison_data;

    target_char = &target->data.character;
    target_class = &target_char->career;
    if (target_char == player_character && (player_class->immunity_flags & 4) != 0) return;
    if (no_resist == 0) if (spfx_resist_roll(2, 4, target_char, target_class, 4, D_0018DDE0) == 100) return;
    if (target_char->level == 1) return;
    poison = object_create_child(target, 0, 47);
    poison->type = 11;
    poison->flags = 32771;
    poison_data = &poison->data.disease;
    poison_data->id = *(signed char *)&poison_id;
    poison_id = (poison_id << 2) - 512;
    poison_data->days_left = rand_range((int)(short)D_00186D87[poison_id], (int)(short)D_00186D89[poison_id]);
    poison_data->stage = 0;
    poison_data->damage_min = rand_range((int)(short)poison_table[poison_id], (int)(short)D_00186D85[poison_id]);
}

void poison_init_record(struct disease *poison, int poison_id)
{
    poison->id = *(signed char *)&poison_id;
    poison_id = (poison_id << 2) - 512;
    poison->days_left = rand_range((int)(short)D_00186D87[poison_id], (int)(short)D_00186D89[poison_id]);
    poison->damage_min = rand_range((int)(short)poison_table[poison_id], (int)(short)D_00186D85[poison_id]);
}

int poison_tick(struct disease *poison)
{
    int amount;
    int unused;
    int unused2;
    struct character *victim;

    if (poison->id < 128) return 1;
    if (poison->damage_min != 0) {
        poison->damage_min--;
        return 1;
    }
    if (poison->stage == 0) poison->stage = 1;
    switch ((unsigned char)(poison->id - 128)) {
    case 0:
        damage_apply(scratch_current_object, rand_range(2, 12), 0);
        break;
    case 1:
        damage_apply(scratch_current_object, 2, 0);
        poison->stat_flags[4] = 1;
        poison->drained[4]++;
        player_character->attributes[4]--;
        if (player_character->attributes[4] < 1) {
            player_character->attributes[4] = 1;
            poison->drained[4]--;
        }
        poison->stage = 2;
        break;
    case 2:
        damage_apply(scratch_current_object, rand_range(1, 10), 0);
        break;
    case 3:
        amount = rand_range(5, 10);
        poison->stat_flags[0] = 1;
        poison->drained[0] += amount;
        player_character->attributes[0] -= amount;
        if (player_character->attributes[0] < 1) {
            player_character->attributes[0] = 1;
            poison->drained[0] -= 1 - player_character->attributes[0];
        }
        amount = rand_range(1, 5);
        poison->stat_flags[3] = 1;
        poison->drained[3] += amount;
        player_character->attributes[3] -= amount;
        if (player_character->attributes[3] < 1) {
            player_character->attributes[3] = 1;
            poison->drained[3] -= 1 - player_character->attributes[3];
        }
        amount = rand_range(1, 5);
        poison->stat_flags[6] = 1;
        poison->drained[6] += amount;
        player_character->attributes[6] -= amount;
        if (player_character->attributes[6] < 1) {
            player_character->attributes[6] = 1;
            poison->drained[6] -= 1 - player_character->attributes[6];
        }
        poison->stage = 2;
        break;
    case 4:
        fatigue_add(-rand_range(10, 100));
        break;
    case 5:
        damage_apply(scratch_current_object, rand_range(1, 30), 0);
        break;
    case 6:
        amount = rand_range(1, 5);
        poison->stat_flags[2] = 1;
        poison->drained[2] += amount;
        player_character->attributes[2] -= amount;
        if (player_character->attributes[2] < 1) {
            player_character->attributes[2] = 1;
            poison->drained[2] -= 1 - player_character->attributes[2];
        }
        victim = &scratch_current_object->data.character;
        victim->magicka -= rand_range(5, 15);
        if (victim->magicka < 0) victim->magicka = 0;
        poison->stage = 2;
        break;
    case 7:
        amount = rand_range(5, 20);
        poison->stat_flags[2] = 1;
        poison->drained[2] += amount;
        player_character->attributes[2] -= amount;
        if (player_character->attributes[2] < 1) {
            player_character->attributes[2] = 1;
            poison->drained[2] -= 1 - player_character->attributes[2];
        }
        amount = rand_range(10, 20);
        poison->stat_flags[5] = 1;
        poison->drained[5] += amount;
        player_character->attributes[5] -= amount;
        if (player_character->attributes[5] < 1) {
            player_character->attributes[5] = 1;
            poison->drained[5] -= 1 - player_character->attributes[5];
        }
        poison->stage = 2;
        break;
    case 8:
        fatigue_add(-rand_range(10, 100));
        amount = rand_range(4, 10);
        poison->stat_flags[7] = 1;
        poison->drained[7] -= amount;
        player_character->attributes[7] += amount;
        poison->stage = 2;
        break;
    case 9:
        amount = rand_range(10, 30);
        poison->stat_flags[1] = 1;
        poison->drained[1] += amount;
        player_character->attributes[1] -= amount;
        if (player_character->attributes[1] < 1) {
            player_character->attributes[1] = 1;
            poison->drained[1] -= 1 - player_character->attributes[1];
        }
        amount = rand_range(5, 20);
        poison->stat_flags[0] = 1;
        poison->drained[0] -= amount;
        player_character->attributes[0] += amount;
        poison->stage = 2;
        break;
    case 10:
        fatigue_add(rand_range(5, 10) << 6);
        amount = rand_range(1, 4);
        poison->stat_flags[2] = 1;
        poison->drained[2] += amount;
        player_character->attributes[2] -= amount;
        if (player_character->attributes[2] < 1) {
            player_character->attributes[2] = 1;
            poison->drained[2] -= 1 - player_character->attributes[2];
        }
        poison->stage = 2;
        break;
    case 11:
        victim = &scratch_current_object->data.character;
        victim->magicka += rand_range(5, 10);
        if (victim->magicka > victim->max_magicka) victim->magicka = victim->max_magicka;
        amount = rand_range(1, 5);
        poison->stat_flags[4] = 1;
        poison->drained[4] += amount;
        player_character->attributes[4] -= amount;
        if (player_character->attributes[4] < 1) {
            player_character->attributes[4] = 1;
            poison->drained[4] -= 1 - player_character->attributes[4];
        }
        poison->stage = 2;
    }
    player_ailment_flags |= 1;
    hud_message_add(D_00185083);
    if (poison->days_left != 0) {
        poison->days_left--;
    } else if (poison->stage == 2) {
        poison->days_left = 254;
        poison->id = 0;
    } else {
        return 0;
    }
    return 1;
}

void disease_add_vampire_spell(struct record *spellbook, int spell_id)
{
    struct record *spell;
    struct spell *spell_data;
    int i;

    spell = object_create_child(spellbook, 0, 89);
    spell->type = 9;
    spell->flags = 3;
    spell->id = object_new_id(spellbook->id >> 16);
    spell_data = &spell->data.spell;
    i = 0;
    while (spell_records[i].name[0] == 0 || spell_records[i].id != spell_id) i++;
    mc_memcpy(spell_data, &spell_records[i], 89, (int)D_00175970, 540, 4);
    spell_data->name[strlen(spell_data->name) + 1] = 36;
}

void disease_cure_vampirism(void)
{
    int i;
    struct record *vampirism;
    struct record *saved_class;
    struct record *next;
    struct record *child;
    struct disease *disease;

    if (player_character->special_infection_time != 0 && player_character->special_infection == 0) {
        player_character->special_infection_time = 0;
        player_character->special_infection = 0;
    }
    if (player_character->race != 8) return;
    player_character->flags &= ~0x4;
    child = player_entity->children;
    while (child != 0) {
        if (child->type == 28) saved_class = child;
        if (child->type == 11) {
            disease = &child->data.disease;
            if (disease->id == 100) {
                vampirism = child;
                disease = &vampirism->data.disease;
            }
        }
        child = child->next;
    }
    for (i = 0; i < 8; i++) {
        player_character->attributes[i] -= disease->drained[i];
        player_character->base_attributes[i] -= disease->drained[i];
    }
    player_character->skills[3].value -= 30;
    player_character->skills[21].value -= 30;
    player_character->skills[16].value -= 30;
    player_character->skills[34].value -= 30;
    player_character->skills[18].value -= 30;
    player_character->skills[30].value -= 30;
    mc_memcpy(player_class, &saved_class->data.career, 74, (int)D_00175970, 590, 4);
    object_foreach(player_entity->children, (int)disease_toggle_memberships_cb);
    player_character->race = player_character->original_race;
    player_character->min_metal_to_hit = 0;
    child = object_find_item(player_entity->children, 27, 0);
    if (child == 0) return;
    child = child->children;
    while (child != 0) {
        next = child->next;
        if (child->data.spell.name[strlen(child->data.spell.name) + 1] == 36) {
            object_delete(child);
        }
        child = next;
    }
    object_delete(saved_class);
    object_delete(vampirism);
    player_character->max_health = (short)player_character->max_health_base;
    D_001940D8 |= 8;
    paperdoll_draw(0, 0);
}

void disease_remove_skill_bonuses(void)
{
    int unused;

    inv_unequip_all_saved();
    if (player_character->race == 8) {
        player_character->skills[3].value -= 30;
        player_character->skills[21].value -= 30;
        player_character->skills[16].value -= 30;
        player_character->skills[34].value -= 30;
        player_character->skills[18].value -= 30;
        player_character->skills[30].value -= 30;
    }
    if (disease_is_lycanthrope() == 0) return;
    player_character->skills[17].value -= 30;
    player_character->skills[18].value -= 30;
    player_character->skills[3].value -= 30;
    player_character->skills[16].value -= 30;
    player_character->skills[21].value -= 30;
    player_character->skills[34].value -= 30;
    player_character->skills[30].value -= 30;
}

void disease_restore_skill_bonuses(void)
{
    inv_reequip_saved();
    if (player_character->race == 8) {
        player_character->skills[3].value += 30;
        player_character->skills[21].value += 30;
        player_character->skills[16].value += 30;
        player_character->skills[34].value += 30;
        player_character->skills[18].value += 30;
        player_character->skills[30].value += 30;
    }
    if (disease_is_lycanthrope() == 0) return;
    player_character->skills[17].value += 30;
    player_character->skills[18].value += 30;
    player_character->skills[3].value += 30;
    player_character->skills[16].value += 30;
    player_character->skills[21].value += 30;
    player_character->skills[34].value += 30;
    player_character->skills[30].value += 30;
}

void disease_become_lycanthrope(int kind)
{
    struct record *saved_class;
    struct record *spell;
    struct record *lycanthropy;
    struct spell *spell_data;
    int attribute;
    int excess;
    int i;
    struct disease *lycanthropy_data;
    {
        int unused;
        int unused2;
        int unused3;
        int unused4;
        int unused5;
        int unused6;
        int unused7;
        int unused8;
        int unused9;
        int unused10;
        int unused11;

        if (player_character->level == 1 || disease_is_lycanthrope() != 0 || player_character->race > 7) {
            return;
        }
        player_character->special_infection_time = 0;
        player_character->special_infection = 0;
        player_character->flags |= 20;
        lycanthropy = object_create_child(player_entity, 0, 47);
        lycanthropy->type = 11;
        lycanthropy->flags = 32771;
        saved_class = object_create_child(player_entity, 0, 74);
        saved_class->type = 28;
        saved_class->flags = 3;
        lycanthropy_data = &lycanthropy->data.disease;
        lycanthropy_data->id = *(signed char *)&kind + 101;
        mc_memcpy(&saved_class->data.career, player_class, 74, (int)D_00175970, 697, 4);
        for (i = 0; i < 4; i++) {
            attribute = (int)(unsigned char)lycanthrope_attributes[i];
            lycanthropy_data->drained[attribute] = 40;
            player_character->attributes[attribute] += 40;
            player_character->base_attributes[attribute] += 40;
            excess = player_character->base_attributes[attribute] - 100;
            if (excess > 0) {
                player_character->base_attributes[attribute] -= excess;
                player_character->attributes[attribute] -= excess;
                lycanthropy_data->drained[attribute] -= excess;
            }
        }
        player_class->immunity_flags |= 64;
        spell = object_create_child(saved_class, 0, 89);
        spell->type = 9;
        spell->flags = 3;
        spell->id = object_new_id(saved_class->id >> 16);
        spell_data = &spell->data.spell;
        i = 0;
        while (spell_records[i].name[0] == 0 || spell_records[i].id != 92) i++;
        mc_memcpy(spell_data, &spell_records[i], 89, (int)D_00175970, 725, 4);
        spell_data->effect_costs[0] = (spell_data->effect_costs[1] = (spell_data->effect_costs[2] = 0));
        player_character->skills[17].value += 30;
        player_character->skills[18].value += 30;
        player_character->skills[3].value += 30;
        player_character->skills[16].value += 30;
        player_character->skills[21].value += 30;
        player_character->skills[34].value += 30;
        player_character->skills[30].value += 30;
        player_character->original_race = player_character->race;
        player_character->lycanthrope_kill_time = game_minutes;
    }
}

void disease_lycanthrope_shapechange(int forced)
{
    struct record *child;
    struct disease *disease;

    disease = 0;
    if (forced == 0 && player_character->race < 8 && ((unsigned)(game_minutes - player_character->last_shapechange_time)) < 1200) {
        if (item_artifact_equipped(3) == 0) {
            msgbox_show_string((int)D_0017597A, 1);
            return;
        }
    }
    D_001940D8 |= 8;
    player_character->last_shapechange_time = game_minutes;
    child = player_entity->children;
    while (child != 0) {
        if (child->type == 11) {
            disease = &child->data.disease;
            if (disease->id == 101 || disease->id == 102) break;
            disease = 0;
        }
        child = child->next;
    }
    if (disease == 0) return;
    if (player_character->race < 8) {
        if (disease->id == 101) {
            player_character->race = 9;
        } else {
            player_character->race = 10;
        }
        player_character->reputation[0] -= 100;
        player_character->reputation[1] -= 100;
        player_character->reputation[2] -= 100;
        player_character->reputation[3] -= 100;
        player_character->reputation[4] -= 100;
        player_character->min_metal_to_hit = 2;
        player_character->health = player_character->max_health;
        D_001940D8 |= 8;
        paperdoll_draw(0, 0);
        player_character->equipped[19] = 0;
        player_character->equipped[21] = 0;
        weapon_reload_hand_sprites();
        return;
    }
    player_character->race = player_character->original_race;
    player_character->reputation[0] += 100;
    player_character->reputation[1] += 100;
    player_character->reputation[2] += 100;
    player_character->reputation[3] += 100;
    player_character->reputation[4] += 100;
    player_character->min_metal_to_hit = 0;
    player_character->health = player_character->max_health;
    D_001940D8 |= 8;
    paperdoll_draw(0, 0);
    weapon_reload_hand_sprites();
}

int disease_is_lycanthrope(void)
{
    int result;
    struct record *child;
    struct disease *disease;

    disease = 0;
    child = player_entity->children;
    while (child != 0) {
        if (child->type == 11) {
            disease = &child->data.disease;
            if (disease->id == 101 || disease->id == 102) break;
            disease = 0;
        }
        child = child->next;
    }
    if (disease != 0) {
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

void disease_cure_lycanthropy(void)
{
    int i;
    struct record *lycanthropy;
    struct record *saved_class;
    struct record *next;
    struct record *child;
    struct disease *lycanthropy_data;
    struct disease *disease;

    if (player_character->special_infection_time != 0 && player_character->special_infection != 0) {
        player_character->special_infection_time = 0;
        player_character->special_infection = 0;
    }
    if (disease_is_lycanthrope() == 0) return;
    player_character->flags &= ~0x4;
    child = player_entity->children;
    while (child != 0) {
        if (child->type == 28) saved_class = child;
        if (child->type == 11) {
            disease = &child->data.disease;
            if (disease->id == 101 || disease->id == 102) {
                lycanthropy = child;
                lycanthropy_data = &lycanthropy->data.disease;
            }
        }
        child = child->next;
    }
    for (i = 0; i < 8; i++) {
        player_character->attributes[i] -= lycanthropy_data->drained[i];
        player_character->base_attributes[i] -= lycanthropy_data->drained[i];
    }
    player_character->skills[17].value -= 30;
    player_character->skills[18].value -= 30;
    player_character->skills[3].value -= 30;
    player_character->skills[16].value -= 30;
    player_character->skills[21].value -= 30;
    player_character->skills[34].value -= 30;
    player_character->skills[30].value -= 30;
    mc_memcpy(player_class, &saved_class->data.career, 74, (int)D_00175970, 874, 4);
    object_delete(saved_class);
    object_delete(lycanthropy);
    player_character->race = player_character->original_race;
    child = object_find_item(player_entity->children, 27, 0);
    if (child == 0) return;
    child = child->children;
    while (child != 0) {
        next = child->next;
        if (child->data.spell.id == 92) object_delete(child);
        child = next;
    }
    D_001940D8 |= 8;
    paperdoll_draw(0, 0);
    player_character->max_health = (short)player_character->max_health_base;
}

void disease_lycanthrope_tick(void)
{
    int moon_day;
    int moon_day_ahead;
    int new_max_health;

    moon_day = (((unsigned)game_minutes) / 1440) & 31;
    moon_day_ahead = (((unsigned)(game_minutes + 5760)) / 1440) & 31;
    if (disease_is_lycanthrope() == 0) return;
    if (item_artifact_equipped(3) != 0) return;
    if (((unsigned)(game_minutes - player_character->lycanthrope_kill_time)) > 20160) {
        new_max_health = (game_minutes - player_character->lycanthrope_kill_time) - 20160;
        if (new_max_health > 20160) {
            new_max_health = 4;
        } else {
            new_max_health = (((new_max_health << 8) / 20160) * player_character->max_health) / 256;
            if (new_max_health < 4) new_max_health = 4;
        }
        if (player_character->max_health != new_max_health) hud_message_add((int)D_0017599B);
        player_character->max_health = new_max_health;
        if (player_character->health > new_max_health) player_character->health = new_max_health;
    }
    if (player_character->race > 8) return;
    if (moon_day != 0) if (moon_day_ahead != 0) return;
    hud_message_add((int)D_001759C5);
    disease_lycanthrope_shapechange(1);
}

void disease_special_infection_tick(void)
{
    if (player_character->special_infection_time == 0) return;
    if (((unsigned)game_minutes) <= player_character->special_infection_time) return;
    player_character->special_infection_time = 0;
    if (player_character->special_infection != 0) {
        disease_become_lycanthrope(player_character->special_infection - 1);
        return;
    }
    disease_become_vampire();
}

void disease_start_cure_quest(int offer_cure)
{
    if (player_character->race == 8) {
        if (offer_cure != 0 && rand_range(10, 100) < 30) {
            mc_strncpy((int)D_001961F5, (int)D_001759EB, 13, (int)D_00175970, 952);
            return;
        }
        if (offer_cure == 0 && player_character->action != 0 && rand_range(1, 100) < 50) {
            quest_pick_file(80, 0, 48, 66, player_character->level);
            return;
        }
        if (offer_cure == 0 && rand_range(1, 100) < 50) {
            quest_pick_file(80, 0, 48, 65, player_character->level);
            if (*(signed char *)D_001961F5 != 0) player_character->action = 1;
        }
        return;
    }
    if (disease_is_lycanthrope() == 0 || offer_cure == 0 || rand_range(1, 100) >= 30) return;
    mc_strncpy((int)D_001961F5, (int)D_001759F8, 13, (int)D_00175970, 971);
}

void reaction_mod_item_cb(struct record *item)
{
    struct item *item_data;
    int i;

    if (item->type != 2) return;
    item_data = &item->data.item;
    if (item_data->enchantments[0].type == (-1)) return;
    for (i = 0; i < 10; i++) {
        if (item_data->enchantments[i].type == (-1)) return;
        if (item_data->enchantments[i].type == 14 && item_data->enchantments[i].param == 5) {
            *(int *)D_00195B84 += 10;
        } else if (item_data->enchantments[i].type == 25 && item_data->enchantments[i].param == 5) {
            *(int *)D_00195B84 -= 10;
        } else if (item_data->enchantments[i].type == 14 && (short)scratch_190ce4[0] == item_data->enchantments[i].param) {
            *(int *)D_00195B84 += 10;
        } else if (item_data->enchantments[i].type == 25 && (short)scratch_190ce4[0] == item_data->enchantments[i].param) {
            *(int *)D_00195B84 -= 10;
        }
    }
}

void enchant_extra_spell_points(struct item *item, int condition)
{
    *(int *)extra_spell_points += enchant_spell_points_condition(condition);
}

int enchant_spell_points_condition(int condition)
{
    struct character *creature;
    int unused;
    int i;

    switch ((unsigned)condition) {
    case 0:
    case 1:
    case 2:
    case 3:
        if (((int)(unsigned char)*(signed char *)(D_0018320A + calendar_month)) == condition) {
            return 75;
        }
        break;
    case 4:
        if (D_00195F24 == 0 || D_00195F25 == 0) return 75;
        break;
    case 5:
        if (((int)(unsigned char)D_00195F24) == 8 || ((int)(unsigned char)D_00195F25) == 8) {
            return 75;
        }
        if (((int)(unsigned char)D_00195F24) == 24 || ((int)(unsigned char)D_00195F25) == 24) {
            return 75;
        }
        break;
    case 6:
        condition += -4;
        if (((int)(unsigned char)D_00195F24) == 16 || ((int)(unsigned char)D_00195F25) == 16) {
            return 75;
        }
        break;
    case 7:
    case 8:
    case 9:
    case 10:
        condition += -7;
        for (i = 0; i < creature_count; i++) {
            if (xn_math_approx_dist2d(player_entity->x, player_entity->z, creature_list[i]->x, creature_list[i]->z) < 1024) {
                creature = &creature_list[i]->data.character;
                if (((int)(unsigned char)*(signed char *)(monster_category + creature->race)) == condition) {
                    return 75;
                }
            }
        }
    }
    return 0;
}

int item_artifact_equipped(int artifact_id)
{
    int slot;
    struct item *item;

    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == 0) continue;
        item = &player_character->equipped[slot]->data.item;
        if (item->enchantments[0].type == (-1)) continue;
        if (item->enchantments[0].type == 26 && item->enchantments[0].param == artifact_id) return 1;
    }
    return 0;
}

void func_00067CA4(int artifact_id, int damage)
{
    int slot;
    struct item *item;

    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == 0) continue;
        item = &player_character->equipped[slot]->data.item;
        if (item->enchantments[0].type == (-1)) continue;
        if (item->enchantments[0].type == 26 && item->enchantments[0].param == artifact_id) {
            item_damage(player_character->equipped[slot], damage);
            return;
        }
    }
}

void effects_tick(void)
{
    int i;                          /* first the elapsed minutes, then the equipment slot */
    int j;                          /* first the saved tick minutes, then the enchantment */
    int unused;
    struct item *item;

    D_001A3AA8 = 0;
    i = (*(int *)D_00195B08 = game_minutes - D_00195B44);
    if (*(int *)D_00195B08 > 100) {
        i = (*(int *)D_00195B08 = 0);
        D_00195B44 = game_minutes;
    }
    *(int *)D_00195B08 >>= 2;
    if (*(int *)D_00195B08 != 0) D_00195B44 = game_minutes;
    scratch_current_object = 0;
    if (*(int *)D_00195B08 != 0 || i != 0) {
        if (player_class->regeneration_flags != 0 && *(int *)D_00195B08 != 0) {
            if (((int)(unsigned char)(player_class->regeneration_flags & 4)) != 0 && (in_dungeon_water != 0 || D_001962A0 != 0)) {
                item_enchantment_tick(0, 5, 0);
            }
            if (((int)(unsigned char)(player_class->regeneration_flags & 8)) != 0) {
                item_enchantment_tick(0, 5, 0);
            }
            if (((int)(unsigned char)(player_class->regeneration_flags & 1)) != 0 && player_in_daylight() != 0) {
                item_enchantment_tick(0, 5, 1);
            }
            if (((int)(unsigned char)(player_class->regeneration_flags & 2)) != 0 && player_in_daylight() == 0) {
                item_enchantment_tick(0, 5, 2);
            }
        }
        j = *(int *)D_00195B08;
        *(int *)D_00195B08 = *(int *)D_00195B08 * 12;
        if (((int)(unsigned short)(player_class->flags & 16)) != 0 && player_in_daylight() != 0) {
            item_enchantment_tick(0, 17, 0);
        }
        if (((int)(unsigned short)(player_class->flags & 32)) != 0 && player_in_temple() != 0) {
            item_enchantment_tick(0, 17, 1);
        }
        *(int *)D_00195B08 = j;
    }
    for (i = 0; i < 27; i++) {
        if (player_character->equipped[i] != 0) {
            item = &player_character->equipped[i]->data.item;
            if (item->enchantments[0].type == (-1)) continue;
            scratch_current_object = player_character->equipped[i];
            j = 0;
            while (j < 10 && item->enchantments[j].type != (-1)) {
                item_enchantment_tick(item, item->enchantments[j].type, item->enchantments[j].param);
                j++;
            }
        }
    }
    if (((int)(unsigned short)(player_class->flags & 128)) != 0 && player_in_daylight() == 0) {
        D_001A3AA8 = -(player_character->max_magicka >> 1);
    }
    if (((int)(unsigned short)(player_class->flags & 64)) != 0 && player_in_daylight() == 0) {
        D_001A3AA8 = -player_character->max_magicka;
    }
    if (((int)(unsigned short)(player_class->flags & 512)) != 0 && player_in_daylight() != 0) {
        D_001A3AA8 = -(player_character->max_magicka >> 1);
    }
    if (((int)(unsigned short)(player_class->flags & 256)) == 0 || player_in_daylight() == 0) {
        return;
    }
    D_001A3AA8 = -player_character->max_magicka;
}

void trade_haggle_frame(void)
{
    int agreed_price;

    if (((int)(unsigned char)game_mode) != 13 && ((int)D_0019626F) != 13) {
        return;
    }
    D_0012B508 = 146;
    if (D_00190D63 == 0) {
        if ((signed char)D_00196271 == 0) return;
        if (((int)D_00196271) == 1) {
            trade_haggle_close((int)trade_haggle_asking);
            return;
        }
        if (((int)D_00196271) == 2) {
            trade_haggle_close(0);
            return;
        }
        if (((int)D_00196271) == 3) {
            D_00190D63 = 1;
            trade_counter_offer();
        }
        return;
    }
    if (((int)(unsigned char)game_mode) != 13) return;
    agreed_price = trade_haggle_counter(*(int *)inpstr_result);
    if (agreed_price == (-1)) {
        trade_haggle_close(0);
        return;
    }
    if (agreed_price != 0) {
        trade_haggle_close(agreed_price);
        return;
    }
    D_00190D63 = 0;
    trade_haggle_show_offer();
}

void trade_haggle_close(int price)
{
    if (price > 0 && ((unsigned)price) > player_character->gold) {
        msgbox_show_rsc(454, 1);
        price = 0;
    }
    trade_haggle_result = price;
    mode_pop();
}
