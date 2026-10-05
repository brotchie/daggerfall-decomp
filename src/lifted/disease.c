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

void disease_infect(struct record *a1, int a2, int a3, int a4)
{
    struct character *l_1C;
    struct career *l_18;
    int l_14;
    struct record *l_10;
    struct disease *l_C;

    l_1C = &a1->data.character;
    l_18 = &l_1C->career;
    if (l_1C == player_character && (player_class->immunity_flags & 64) != 0) return;
    if (a4 == 0) if (spfx_resist_roll(2, 64, l_1C, l_18, 4, *(int *)bio_modifiers) == 100) return;
    if (l_1C->level == 1) return;
    l_10 = object_create_child(a1, 0, 47);
    l_10->type = 11;
    l_10->flags = 32771;
    l_C = &l_10->data.disease;
    if (a2 != 0) {
        l_14 = 0;
        while (((int)(unsigned char)*(signed char *)((char *)(l_14++ + a2))) != 255);
        a3 = (int)(unsigned char)*(signed char *)((char *)(int)((char *)a2 + rand_range(0, l_14 - 2)));
    }
    mc_memcpy(l_C, &disease_table[a3], 47, (int)D_00175970, 83, 4);
    if (l_C->days_left != 255) l_C->days_left = rand_range(l_C->days_left, l_C->stage);
    l_C->stage = 0;
}

void poison_apply(struct record *a1, int a2, int a3)
{
    struct character *l_20;
    struct career *l_1C;
    int l_18;
    struct record *l_14;
    struct disease *l_10;

    l_20 = &a1->data.character;
    l_1C = &l_20->career;
    if (l_20 == player_character && (player_class->immunity_flags & 4) != 0) return;
    if (a3 == 0) if (spfx_resist_roll(2, 4, l_20, l_1C, 4, D_0018DDE0) == 100) return;
    if (l_20->level == 1) return;
    l_14 = object_create_child(a1, 0, 47);
    l_14->type = 11;
    l_14->flags = 32771;
    l_10 = &l_14->data.disease;
    l_10->id = *(signed char *)&a2;
    a2 = (a2 << 2) - 512;
    l_10->days_left = rand_range((int)(short)D_00186D87[a2], (int)(short)D_00186D89[a2]);
    l_10->stage = 0;
    l_10->damage_min = rand_range((int)(short)poison_table[a2], (int)(short)D_00186D85[a2]);
}

void poison_init_record(struct disease *a1, int a2)
{
    a1->id = *(signed char *)&a2;
    a2 = (a2 << 2) - 512;
    a1->days_left = rand_range((int)(short)D_00186D87[a2], (int)(short)D_00186D89[a2]);
    a1->damage_min = rand_range((int)(short)poison_table[a2], (int)(short)D_00186D85[a2]);
}

int poison_tick(struct disease *a1)
{
    int l_28;
    int l_24;
    int l_20;
    struct character *l_1C;

    if (a1->id < 128) return 1;
    if (a1->damage_min != 0) {
        a1->damage_min--;
        return 1;
    }
    if (a1->stage == 0) a1->stage = 1;
    switch ((unsigned char)(a1->id - 128)) {
    case 0:
        damage_apply(scratch_current_object, rand_range(2, 12), 0);
        break;
    case 1:
        damage_apply(scratch_current_object, 2, 0);
        a1->stat_flags[4] = 1;
        a1->drained[4]++;
        player_character->attributes[4]--;
        if (player_character->attributes[4] < 1) {
            player_character->attributes[4] = 1;
            a1->drained[4]--;
        }
        a1->stage = 2;
        break;
    case 2:
        damage_apply(scratch_current_object, rand_range(1, 10), 0);
        break;
    case 3:
        l_28 = rand_range(5, 10);
        a1->stat_flags[0] = 1;
        a1->drained[0] += l_28;
        player_character->attributes[0] -= l_28;
        if (player_character->attributes[0] < 1) {
            player_character->attributes[0] = 1;
            a1->drained[0] -= 1 - player_character->attributes[0];
        }
        l_28 = rand_range(1, 5);
        a1->stat_flags[3] = 1;
        a1->drained[3] += l_28;
        player_character->attributes[3] -= l_28;
        if (player_character->attributes[3] < 1) {
            player_character->attributes[3] = 1;
            a1->drained[3] -= 1 - player_character->attributes[3];
        }
        l_28 = rand_range(1, 5);
        a1->stat_flags[6] = 1;
        a1->drained[6] += l_28;
        player_character->attributes[6] -= l_28;
        if (player_character->attributes[6] < 1) {
            player_character->attributes[6] = 1;
            a1->drained[6] -= 1 - player_character->attributes[6];
        }
        a1->stage = 2;
        break;
    case 4:
        fatigue_add(-rand_range(10, 100));
        break;
    case 5:
        damage_apply(scratch_current_object, rand_range(1, 30), 0);
        break;
    case 6:
        l_28 = rand_range(1, 5);
        a1->stat_flags[2] = 1;
        a1->drained[2] += l_28;
        player_character->attributes[2] -= l_28;
        if (player_character->attributes[2] < 1) {
            player_character->attributes[2] = 1;
            a1->drained[2] -= 1 - player_character->attributes[2];
        }
        l_1C = &scratch_current_object->data.character;
        l_1C->magicka -= rand_range(5, 15);
        if (l_1C->magicka < 0) l_1C->magicka = 0;
        a1->stage = 2;
        break;
    case 7:
        l_28 = rand_range(5, 20);
        a1->stat_flags[2] = 1;
        a1->drained[2] += l_28;
        player_character->attributes[2] -= l_28;
        if (player_character->attributes[2] < 1) {
            player_character->attributes[2] = 1;
            a1->drained[2] -= 1 - player_character->attributes[2];
        }
        l_28 = rand_range(10, 20);
        a1->stat_flags[5] = 1;
        a1->drained[5] += l_28;
        player_character->attributes[5] -= l_28;
        if (player_character->attributes[5] < 1) {
            player_character->attributes[5] = 1;
            a1->drained[5] -= 1 - player_character->attributes[5];
        }
        a1->stage = 2;
        break;
    case 8:
        fatigue_add(-rand_range(10, 100));
        l_28 = rand_range(4, 10);
        a1->stat_flags[7] = 1;
        a1->drained[7] -= l_28;
        player_character->attributes[7] += l_28;
        a1->stage = 2;
        break;
    case 9:
        l_28 = rand_range(10, 30);
        a1->stat_flags[1] = 1;
        a1->drained[1] += l_28;
        player_character->attributes[1] -= l_28;
        if (player_character->attributes[1] < 1) {
            player_character->attributes[1] = 1;
            a1->drained[1] -= 1 - player_character->attributes[1];
        }
        l_28 = rand_range(5, 20);
        a1->stat_flags[0] = 1;
        a1->drained[0] -= l_28;
        player_character->attributes[0] += l_28;
        a1->stage = 2;
        break;
    case 10:
        fatigue_add(rand_range(5, 10) << 6);
        l_28 = rand_range(1, 4);
        a1->stat_flags[2] = 1;
        a1->drained[2] += l_28;
        player_character->attributes[2] -= l_28;
        if (player_character->attributes[2] < 1) {
            player_character->attributes[2] = 1;
            a1->drained[2] -= 1 - player_character->attributes[2];
        }
        a1->stage = 2;
        break;
    case 11:
        l_1C = &scratch_current_object->data.character;
        l_1C->magicka += rand_range(5, 10);
        if (l_1C->magicka > l_1C->max_magicka) l_1C->magicka = l_1C->max_magicka;
        l_28 = rand_range(1, 5);
        a1->stat_flags[4] = 1;
        a1->drained[4] += l_28;
        player_character->attributes[4] -= l_28;
        if (player_character->attributes[4] < 1) {
            player_character->attributes[4] = 1;
            a1->drained[4] -= 1 - player_character->attributes[4];
        }
        a1->stage = 2;
    }
    player_ailment_flags |= 1;
    hud_message_add(D_00185083);
    if (a1->days_left != 0) {
        a1->days_left--;
    } else if (a1->stage == 2) {
        a1->days_left = 254;
        a1->id = 0;
    } else {
        return 0;
    }
    return 1;
}

void disease_add_vampire_spell(struct record *a1, int a2)
{
    struct record *l_1C;
    struct spell *l_18;
    int l_14;

    l_1C = object_create_child(a1, 0, 89);
    l_1C->type = 9;
    l_1C->flags = 3;
    l_1C->id = object_new_id(a1->id >> 16);
    l_18 = &l_1C->data.spell;
    l_14 = 0;
    while (spell_records[l_14].name[0] == 0 || spell_records[l_14].id != a2) l_14++;
    mc_memcpy(l_18, &spell_records[l_14], 89, (int)D_00175970, 540, 4);
    l_18->name[strlen(l_18->name) + 1] = 36;
}

void disease_cure_vampirism(void)
{
    int l_2C;
    struct record *l_28;
    struct record *l_24;
    struct record *l_20;
    struct record *l_1C;
    struct disease *l_18;

    if (player_character->special_infection_time != 0 && player_character->special_infection == 0) {
        player_character->special_infection_time = 0;
        player_character->special_infection = 0;
    }
    if (player_character->race != 8) return;
    player_character->flags &= ~0x4;
    l_1C = player_entity->children;
    while (l_1C != 0) {
        if (l_1C->type == 28) l_24 = l_1C;
        if (l_1C->type == 11) {
            l_18 = &l_1C->data.disease;
            if (l_18->id == 100) {
                l_28 = l_1C;
                l_18 = &l_28->data.disease;
            }
        }
        l_1C = l_1C->next;
    }
    for (l_2C = 0; l_2C < 8; l_2C++) {
        player_character->attributes[l_2C] -= l_18->drained[l_2C];
        player_character->base_attributes[l_2C] -= l_18->drained[l_2C];
    }
    player_character->skills[3].value -= 30;
    player_character->skills[21].value -= 30;
    player_character->skills[16].value -= 30;
    player_character->skills[34].value -= 30;
    player_character->skills[18].value -= 30;
    player_character->skills[30].value -= 30;
    mc_memcpy(player_class, &l_24->data.career, 74, (int)D_00175970, 590, 4);
    object_foreach(player_entity->children, (int)disease_toggle_memberships_cb);
    player_character->race = player_character->original_race;
    player_character->min_metal_to_hit = 0;
    l_1C = object_find_item(player_entity->children, 27, 0);
    if (l_1C == 0) return;
    l_1C = l_1C->children;
    while (l_1C != 0) {
        l_20 = l_1C->next;
        if (l_1C->data.spell.name[strlen(l_1C->data.spell.name) + 1] == 36) {
            object_delete(l_1C);
        }
        l_1C = l_20;
    }
    object_delete(l_24);
    object_delete(l_28);
    player_character->max_health = (short)player_character->max_health_base;
    D_001940D8 |= 8;
    paperdoll_draw(0, 0);
}

void disease_remove_skill_bonuses(void)
{
    int l_18;

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

void disease_become_lycanthrope(int a1)
{
    struct record *l_34;
    struct record *l_30;
    struct record *l_2C;
    struct spell *l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct disease *l_18;
    {
        int l_64;
        int l_60;
        int l_5C;
        int l_58;
        int l_54;
        int l_50;
        int l_4C;
        int l_48;
        int l_44;
        int l_40;
        int l_3C;

        if (player_character->level == 1 || disease_is_lycanthrope() != 0 || player_character->race > 7) {
            return;
        }
        player_character->special_infection_time = 0;
        player_character->special_infection = 0;
        player_character->flags |= 20;
        l_2C = object_create_child(player_entity, 0, 47);
        l_2C->type = 11;
        l_2C->flags = 32771;
        l_34 = object_create_child(player_entity, 0, 74);
        l_34->type = 28;
        l_34->flags = 3;
        l_18 = &l_2C->data.disease;
        l_18->id = *(signed char *)&a1 + 101;
        mc_memcpy(&l_34->data.career, player_class, 74, (int)D_00175970, 697, 4);
        for (l_1C = 0; l_1C < 4; l_1C++) {
            l_24 = (int)(unsigned char)lycanthrope_attributes[l_1C];
            l_18->drained[l_24] = 40;
            player_character->attributes[l_24] += 40;
            player_character->base_attributes[l_24] += 40;
            l_20 = player_character->base_attributes[l_24] - 100;
            if (l_20 > 0) {
                player_character->base_attributes[l_24] -= l_20;
                player_character->attributes[l_24] -= l_20;
                l_18->drained[l_24] -= l_20;
            }
        }
        player_class->immunity_flags |= 64;
        l_30 = object_create_child(l_34, 0, 89);
        l_30->type = 9;
        l_30->flags = 3;
        l_30->id = object_new_id(l_34->id >> 16);
        l_28 = &l_30->data.spell;
        l_1C = 0;
        while (spell_records[l_1C].name[0] == 0 || spell_records[l_1C].id != 92) l_1C++;
        mc_memcpy(l_28, &spell_records[l_1C], 89, (int)D_00175970, 725, 4);
        l_28->effect_costs[0] = (l_28->effect_costs[1] = (l_28->effect_costs[2] = 0));
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

void disease_lycanthrope_shapechange(int a1)
{
    struct record *l_1C;
    struct disease *l_18;

    l_18 = 0;
    if (a1 == 0 && player_character->race < 8 && ((unsigned)(game_minutes - player_character->last_shapechange_time)) < 1200) {
        if (item_artifact_equipped(3) == 0) {
            msgbox_show_string((int)D_0017597A, 1);
            return;
        }
    }
    D_001940D8 |= 8;
    player_character->last_shapechange_time = game_minutes;
    l_1C = player_entity->children;
    while (l_1C != 0) {
        if (l_1C->type == 11) {
            l_18 = &l_1C->data.disease;
            if (l_18->id == 101 || l_18->id == 102) break;
            l_18 = 0;
        }
        l_1C = l_1C->next;
    }
    if (l_18 == 0) return;
    if (player_character->race < 8) {
        if (l_18->id == 101) {
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
    int l_24;
    struct record *l_20;
    struct disease *l_1C;

    l_1C = 0;
    l_20 = player_entity->children;
    while (l_20 != 0) {
        if (l_20->type == 11) {
            l_1C = &l_20->data.disease;
            if (l_1C->id == 101 || l_1C->id == 102) break;
            l_1C = 0;
        }
        l_20 = l_20->next;
    }
    if (l_1C != 0) {
        l_24 = 1;
    } else {
        l_24 = 0;
    }
    return l_24;
}

void disease_cure_lycanthropy(void)
{
    int l_30;
    struct record *l_2C;
    struct record *l_28;
    struct record *l_24;
    struct record *l_20;
    struct disease *l_1C;
    struct disease *l_18;

    if (player_character->special_infection_time != 0 && player_character->special_infection != 0) {
        player_character->special_infection_time = 0;
        player_character->special_infection = 0;
    }
    if (disease_is_lycanthrope() == 0) return;
    player_character->flags &= ~0x4;
    l_20 = player_entity->children;
    while (l_20 != 0) {
        if (l_20->type == 28) l_28 = l_20;
        if (l_20->type == 11) {
            l_18 = &l_20->data.disease;
            if (l_18->id == 101 || l_18->id == 102) {
                l_2C = l_20;
                l_1C = &l_2C->data.disease;
            }
        }
        l_20 = l_20->next;
    }
    for (l_30 = 0; l_30 < 8; l_30++) {
        player_character->attributes[l_30] -= l_1C->drained[l_30];
        player_character->base_attributes[l_30] -= l_1C->drained[l_30];
    }
    player_character->skills[17].value -= 30;
    player_character->skills[18].value -= 30;
    player_character->skills[3].value -= 30;
    player_character->skills[16].value -= 30;
    player_character->skills[21].value -= 30;
    player_character->skills[34].value -= 30;
    player_character->skills[30].value -= 30;
    mc_memcpy(player_class, &l_28->data.career, 74, (int)D_00175970, 874, 4);
    object_delete(l_28);
    object_delete(l_2C);
    player_character->race = player_character->original_race;
    l_20 = object_find_item(player_entity->children, 27, 0);
    if (l_20 == 0) return;
    l_20 = l_20->children;
    while (l_20 != 0) {
        l_24 = l_20->next;
        if (l_20->data.spell.id == 92) object_delete(l_20);
        l_20 = l_24;
    }
    D_001940D8 |= 8;
    paperdoll_draw(0, 0);
    player_character->max_health = (short)player_character->max_health_base;
}

void disease_lycanthrope_tick(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = (((unsigned)game_minutes) / 1440) & 31;
    l_1C = (((unsigned)(game_minutes + 5760)) / 1440) & 31;
    if (disease_is_lycanthrope() == 0) return;
    if (item_artifact_equipped(3) != 0) return;
    if (((unsigned)(game_minutes - player_character->lycanthrope_kill_time)) > 20160) {
        l_18 = (game_minutes - player_character->lycanthrope_kill_time) - 20160;
        if (l_18 > 20160) {
            l_18 = 4;
        } else {
            l_18 = (((l_18 << 8) / 20160) * player_character->max_health) / 256;
            if (l_18 < 4) l_18 = 4;
        }
        if (player_character->max_health != l_18) hud_message_add((int)D_0017599B);
        player_character->max_health = l_18;
        if (player_character->health > l_18) player_character->health = l_18;
    }
    if (player_character->race > 8) return;
    if (l_20 != 0) if (l_1C != 0) return;
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

void disease_start_cure_quest(int a1)
{
    if (player_character->race == 8) {
        if (a1 != 0 && rand_range(10, 100) < 30) {
            mc_strncpy((int)D_001961F5, (int)D_001759EB, 13, (int)D_00175970, 952);
            return;
        }
        if (a1 == 0 && player_character->action != 0 && rand_range(1, 100) < 50) {
            quest_pick_file(80, 0, 48, 66, player_character->level);
            return;
        }
        if (a1 == 0 && rand_range(1, 100) < 50) {
            quest_pick_file(80, 0, 48, 65, player_character->level);
            if (*(signed char *)D_001961F5 != 0) player_character->action = 1;
        }
        return;
    }
    if (disease_is_lycanthrope() == 0 || a1 == 0 || rand_range(1, 100) >= 30) return;
    mc_strncpy((int)D_001961F5, (int)D_001759F8, 13, (int)D_00175970, 971);
}

void reaction_mod_item_cb(struct record *a1)
{
    struct item *l_1C;
    int l_18;

    if (a1->type != 2) return;
    l_1C = &a1->data.item;
    if (l_1C->enchantments[0].type == (-1)) return;
    for (l_18 = 0; l_18 < 10; l_18++) {
        if (l_1C->enchantments[l_18].type == (-1)) return;
        if (l_1C->enchantments[l_18].type == 14 && l_1C->enchantments[l_18].param == 5) {
            *(int *)D_00195B84 += 10;
        } else if (l_1C->enchantments[l_18].type == 25 && l_1C->enchantments[l_18].param == 5) {
            *(int *)D_00195B84 -= 10;
        } else if (l_1C->enchantments[l_18].type == 14 && (short)scratch_190ce4[0] == l_1C->enchantments[l_18].param) {
            *(int *)D_00195B84 += 10;
        } else if (l_1C->enchantments[l_18].type == 25 && (short)scratch_190ce4[0] == l_1C->enchantments[l_18].param) {
            *(int *)D_00195B84 -= 10;
        }
    }
}

void enchant_extra_spell_points(int a1, int a2)
{
    *(int *)extra_spell_points += enchant_spell_points_condition(a2);
}

int enchant_spell_points_condition(int a1)
{
    struct character *l_24;
    int l_20;
    int l_1C;

    switch ((unsigned)a1) {
    case 0:
    case 1:
    case 2:
    case 3:
        if (((int)(unsigned char)*(signed char *)(D_0018320A + calendar_month)) == a1) {
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
        a1 += -4;
        if (((int)(unsigned char)D_00195F24) == 16 || ((int)(unsigned char)D_00195F25) == 16) {
            return 75;
        }
        break;
    case 7:
    case 8:
    case 9:
    case 10:
        a1 += -7;
        for (l_1C = 0; l_1C < creature_count; l_1C++) {
            if (xn_math_approx_dist2d(player_entity->x, player_entity->z, creature_list[l_1C]->x, creature_list[l_1C]->z) < 1024) {
                l_24 = &creature_list[l_1C]->data.character;
                if (((int)(unsigned char)*(signed char *)(monster_category + l_24->race)) == a1) {
                    return 75;
                }
            }
        }
    }
    return 0;
}

int item_artifact_equipped(int a1)
{
    int l_20;
    struct item *l_1C;

    for (l_20 = 0; l_20 < 27; l_20++) {
        if (player_character->equipped[l_20] == 0) continue;
        l_1C = &player_character->equipped[l_20]->data.item;
        if (l_1C->enchantments[0].type == (-1)) continue;
        if (l_1C->enchantments[0].type == 26 && l_1C->enchantments[0].param == a1) return 1;
    }
    return 0;
}

void func_00067CA4(int a1, int a2)
{
    int l_18;
    struct item *l_14;

    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] == 0) continue;
        l_14 = &player_character->equipped[l_18]->data.item;
        if (l_14->enchantments[0].type == (-1)) continue;
        if (l_14->enchantments[0].type == 26 && l_14->enchantments[0].param == a1) {
            item_damage(player_character->equipped[l_18], a2);
            return;
        }
    }
}

void effects_tick(void)
{
    int l_24;
    int l_20;
    int l_1C;
    struct item *l_18;

    D_001A3AA8 = 0;
    l_24 = (*(int *)D_00195B08 = game_minutes - D_00195B44);
    if (*(int *)D_00195B08 > 100) {
        l_24 = (*(int *)D_00195B08 = 0);
        D_00195B44 = game_minutes;
    }
    *(int *)D_00195B08 >>= 2;
    if (*(int *)D_00195B08 != 0) D_00195B44 = game_minutes;
    scratch_current_object = 0;
    if (*(int *)D_00195B08 != 0 || l_24 != 0) {
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
        l_20 = *(int *)D_00195B08;
        *(int *)D_00195B08 = *(int *)D_00195B08 * 12;
        if (((int)(unsigned short)(player_class->flags & 16)) != 0 && player_in_daylight() != 0) {
            item_enchantment_tick(0, 17, 0);
        }
        if (((int)(unsigned short)(player_class->flags & 32)) != 0 && player_in_temple() != 0) {
            item_enchantment_tick(0, 17, 1);
        }
        *(int *)D_00195B08 = l_20;
    }
    for (l_24 = 0; l_24 < 27; l_24++) {
        if (player_character->equipped[l_24] != 0) {
            l_18 = &player_character->equipped[l_24]->data.item;
            if (l_18->enchantments[0].type == (-1)) continue;
            scratch_current_object = player_character->equipped[l_24];
            l_20 = 0;
            while (l_20 < 10 && l_18->enchantments[l_20].type != (-1)) {
                item_enchantment_tick(l_18, l_18->enchantments[l_20].type, l_18->enchantments[l_20].param);
                l_20++;
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
    int l_18;

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
    l_18 = trade_haggle_counter(*(int *)inpstr_result);
    if (l_18 == (-1)) {
        trade_haggle_close(0);
        return;
    }
    if (l_18 != 0) {
        trade_haggle_close(l_18);
        return;
    }
    D_00190D63 = 0;
    trade_haggle_show_offer();
}

void trade_haggle_close(int a1)
{
    if (a1 > 0 && ((unsigned)a1) > player_character->gold) {
        msgbox_show_rsc(454, 1);
        a1 = 0;
    }
    trade_haggle_result = a1;
    mode_pop();
}
