/* qkey.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern int D_00147954;
extern char D_001708F0[];
extern char D_001708F7[];
extern char D_00170909[];
extern char D_0017090D[];
extern char D_00170918[];
extern char D_00170928[];
extern char D_0017093A[];
extern char D_00170949[];
extern char D_00170959[];
extern char D_00170968[];
extern signed char weapon_skills[];
extern signed char D_0017A144[];
extern signed char struck_body_part_table[];
extern char D_0017CAFA[];
extern char material_to_hit[];
extern char weapon_damage_min[];
extern char weapon_damage_max[];
extern char monster_names[];
extern char region_names[];
extern signed char D_001841E3[];
extern int text_blank;
extern char monster_category[];
extern int D_0018506F;
extern signed char body_part_armor_slots[];
extern char weapon_proficiency_bits[];
extern int D_0018DDDC;
extern signed char text_buffer[];
extern struct record *creature_list[];
extern char D_00190B44[];
extern signed char text_rsc_buffer[];
extern unsigned char D_001940D7;
extern char D_00195A08[];
extern struct item *text_macro_item;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *scratch_current_object;
extern int D_00195ABC;
extern char cheat_flags[];
extern struct record *found_object;
extern int creature_count;
extern struct record *scratch_object;
extern struct character *player_character;
extern struct career *player_class;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char game_mode;
extern signed char D_0019628D;
extern signed char D_0019628E;
extern signed char text_macro_gender;
extern signed char D_0019629C;
extern signed char D_001962B2;
extern struct quest *current_quest;
extern short swing_to_hit;
extern short swing_damage;

extern struct faction *faction_find(short);
extern int damage_miss_sound(struct item *, int);
extern void *quest_section(struct quest *, int);
extern int guards_are_present(void);
extern struct flat_cfg *flats_cfg_find(int);
extern int sound_play(int, struct record *, int);
extern int disk_open_data(char *);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int spfx_resist_roll(int, int, struct character *, struct career *, int, int);
extern int name_generate_seeded(unsigned char, unsigned char, int);
extern int building_name(struct building *);
extern struct record *object_free_single(struct record *);
extern int rand();
extern int close();
extern int read();
extern int mc_strncpy();
extern int itoa();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern void crime_reputation_penalty(void);
extern void damage_weapon_strike_effects(struct item *, struct record *, struct record *, int);
extern void damage_creature_death(struct record *);
extern void damage_monster_hit_effects(struct record *, struct record *);
extern void damage_player_hurt(int);
extern void damage_spawn_splash(struct record *, int, int);
extern void damage_knockback(struct record *, int, int, int);
extern void monster_wake_all(void);
extern void damage_namira_reflect(struct record *, struct record *, int);
extern void skill_add_uses(int, int);
extern void crime_guards_or_court(int);
extern void parse_expand(unsigned char *, char *);
extern void quest_raise_event(short, struct record *, struct record *);
extern void spell_break_concealment(struct record *);
extern void item_wear_from_hit(struct record *, int);
extern void item_damage(struct record *, int);
extern void poison_apply(struct record *, int, int);
extern void monster_wabbajack(struct record *, struct record *);
extern void msgbox_yes_no_rsc(int);
int damage_roll_to_hit(struct character *, struct character *, int, int, int, int);
int damage_bonus_vs_target(struct item *, struct character *, struct character *);
int damage_apply(struct record *, int, struct record *);
#pragma aux mc_set_location parm routine [];

int quest_symbol_text(int symbol, int form, int second_symbol)
{
    struct faction *faction;
    struct qbn_foe *foe;
    struct qbn_place *qbn_place;
    struct qbn_place *found_place;
    struct qbn_item *qbn_item;
    struct qbn_person *qbn_person;
    struct qbn_timer *timer;
    struct qbn_timer *second_timer;
    struct record *object;
    struct building *building;
    int unused;
    int i;
    int minutes;
    int book_file;
    struct character *creature;
    char *book_header;
    {
        int is_female;

        if (current_quest != 0) {
            qbn_item = quest_section(current_quest, 0);
            for (i = 0; current_quest->section_counts[0] > i; i++, qbn_item++) {
                if (symbol == qbn_item->symbol) break;
            }
            if (current_quest->section_counts[0] > i) {
                qbn_item->flags &= 127;
                object = qbn_item->object;
                if (object != 0) {
                    if ((text_macro_item = &object->data.item)->group == 28 && text_macro_item->index == 0) {
                        return itoa(text_macro_item->value, (int)text_rsc_buffer, 10);
                    }
                    if (text_macro_item->group == 7) {
                        book_header = *(char **)&D_00147954 + 90000;
                        mc_set_location(115, (int)D_001708F0);
                        mc_sprintf((int)text_buffer, (int)D_001708F7, (int)(unsigned short)(short)text_macro_item->message);
                        book_file = disk_open_data(text_buffer);
                        read(book_file, book_header, 234);
                        close(book_file);
                        mc_strncpy((int)text_rsc_buffer, book_header, 2048, (int)D_001708F0, 119);
                        return (int)text_rsc_buffer;
                    }
                    parse_expand(D_00170909, D_00190B44);
                    return (int)D_00190B44;
                }
            }
            qbn_place = quest_section(current_quest, 4);
            found_place = 0;
            for (i = 0; current_quest->section_counts[4] > i; i++, qbn_place++) {
                if (symbol == qbn_place->symbol) {
                    found_place = qbn_place;
                    break;
                }
            }
            if (found_place != 0) {
                qbn_place->flags &= 127;
                object = qbn_place->object;
                if (object != 0) {
                    if ((form & 240) == 0) {
                        building = &object->data.building;
                        return building_name(building);
                    }
                    if ((form & 240) <= 32) return (int)object->data.quest_npc.location_name;
                    if ((form & 240) != 0) {
                        return *(int *)(region_names + (((int)(unsigned short)object->region) << 2));
                    }
                }
            }
            qbn_person = quest_section(current_quest, 3);
            for (i = 0; current_quest->section_counts[3] > i; i++, qbn_person++) {
                if (symbol == qbn_person->symbol) break;
            }
            if (current_quest->section_counts[3] <= i) goto L2D3DB;
            qbn_person->flags &= ~0x8000;
            object = qbn_person->object;
            if (object == 0) goto L2D3DB;
            if (object->type == 65) return (int)faction_find(object->faction_id)->name;
            if (((int)(unsigned short)(object->flags & 4)) != 0) {
                is_female = 1;
            } else {
                is_female = 0;
            }
            text_macro_gender = *(signed char *)&is_female;
            if ((form & 15) == 1) return (int)flats_cfg_find(object->image)->name;
            if ((form & 15) > 1) {
                if (object->faction_id != 0) return (int)faction_find(object->faction_id)->name;
                return (int)D_0017090D;
            }
            switch (form & 240) {
            case 0:
                faction = faction_find((int)(short)object->data.building.faction_id);
                if (faction != 0 && faction->type == 4) return (int)faction->name;
                if (qbn_person->kind == (-1)) {
                    faction = faction_find(qbn_person->faction_id);
                    if (faction->type == 4) return (int)faction->name;
                }
                return name_generate_seeded((int)(unsigned char)D_001841E3[object->home_region], (int)(unsigned char)((signed char)object->flags & 4), object->name_seed);
            case 16:
                if ((object->id >> 16) == 50015) return (int)D_00170918;
                if ((object->id >> 16) == 50027) return (int)D_00170928;
                if ((object->id >> 16) == 50029) return (int)D_0017093A;
                if ((object->id >> 16) == 50033) return (int)D_00170949;
                if ((object->id >> 16) == 50041) return (int)D_00170959;
                return building_name(&object->data.building);
            case 32:
                return (int)object->data.quest_npc.location_name;
            case 48:
                return *(int *)(region_names + (object->home_region << 2));
            default:
L2D3DB:;
                foe = quest_section(current_quest, 7);
                for (i = 0; current_quest->section_counts[7] > i; i++, foe++) {
                    if (symbol == foe->symbol) break;
                }
                if (current_quest->section_counts[7] > i) {
                    object = foe->object;
                    if (object != 0) {
                        creature = &object->data.character;
                        if ((form & 15) == 1) {
                            if (creature->mobile_id >= 128) {
                                return name_generate_seeded((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)((signed char)creature->flags & 1), object->name_seed);
                            }
                            return name_generate_seeded((int)(unsigned char)D_0017A144[current_quest->id % 3], 0, object->name_seed);
                        }
                        if (creature->mobile_id >= 128) {
                            return *(int *)(D_0017CAFA + (creature->mobile_id << 2));
                        }
                        return *(int *)(monster_names + (creature->race << 2));
                    }
                }
            }
            timer = quest_section(current_quest, 6);
            for (i = 0; current_quest->section_counts[6] > i; i++, timer++) {
                if (symbol == timer->state_hash) break;
            }
            minutes = timer->delay;
            if (second_symbol != 0) {
                second_timer = quest_section(current_quest, 6);
                for (i = 0; current_quest->section_counts[6] > i; i++, second_timer++) {
                    if (second_symbol == second_timer->state_hash) break;
                }
                minutes += second_timer->delay;
            }
            if (current_quest->section_counts[6] > i) {
                if ((form & 15) != 0) {
                    return itoa((minutes + 1439) / 1440, (int)text_rsc_buffer, 10);
                }
                return itoa(minutes, (int)text_rsc_buffer, 10);
            }
        }
        return text_blank;
    }
}

void damage_resolve_attack(struct record *attacker, struct record *target, int hand_slot)
{
    struct character *attacker_character;
    struct character *target_character;
    struct item *weapon;
    struct disease *poison;
    int body_part;
    int skill_index;
    int dealt;
    int chance;
    int i;
    int total_dealt;
    int damage;
    int creature_index;
    int unused1;
    int unused2;
    int backstab_chance;
    int unused3;
    struct monster_anim *anim;

    if (((int)(unsigned char)game_mode) == 21) return;
    if (target != player_entity) {
        for (i = 0; i < creature_count; i++) {
            if (creature_list[i] == target) {
                creature_index = i;
                break;
            }
        }
    }
    if (attacker == player_entity && rand_range(1, 100) <= 20) {
        if (player_character->race == 8) {
            sound_play(((((int)(unsigned short)(player_character->flags & 1)) != 0) ? (rand() & 1) + 10281 : (rand() & 1) + 10301), player_object, 100);
        } else if (player_character->race == 9) {
            sound_play((rand() & 1) + 10091, player_object, 100);
        } else if (player_character->race == 10) {
            sound_play((rand() & 1) + 10141, player_object, 100);
        }
    }
    body_part = (int)(unsigned char)struck_body_part_table[rand_range(0, 19)];
    attacker_character = &attacker->data.character;
    target_character = &target->data.character;
    scratch_current_object = attacker_character->equipped[hand_slot];
    weapon = &attacker_character->equipped[hand_slot]->data.item;
    if (attacker_character->equipped[hand_slot] != 0 && weapon->group == 3 && player_character->race < 9) {
        skill_index = (int)(unsigned char)weapon_skills[weapon->index];
    } else {
        skill_index = 30;
    }
    chance = attacker_character->skills[skill_index].value;
    if (skill_index != 30 && attacker_character == player_character && (((int)(short)*(short *)(weapon_proficiency_bits + (weapon->index * 2))) & player_class->expert_weapons) != 0) {
        chance += player_character->level;
    }
    backstab_chance = 0;
    if (attacker == player_entity) {
        anim = &target->data.monster.anim;
        if (anim->anim_request == 57) return;
        if (anim->anim_facing > 2) {
            chance += player_character->skills[SKILL_BACKSTABBING].value;
            skill_add_uses(19, 1);
            backstab_chance = player_character->skills[SKILL_BACKSTABBING].value;
        }
    }
    if (attacker == player_entity) skill_add_uses(skill_index, 1);
    if (skill_index == 30) {
        if (attacker == player_entity) {
            if (damage_roll_to_hit(attacker_character, target_character, body_part, chance, -1, 0) != 0) {
                damage = rand_range((player_character->skills[SKILL_HAND_TO_HAND].value / 10) + 1, (int)&*(signed char *)((char *)(player_character->skills[SKILL_HAND_TO_HAND].value / 5) + 1));
                if (backstab_chance != 0) {
                    if (rand_range(1, 100) <= backstab_chance) {
                        damage = damage * 3;
                        hud_message_add(D_0018506F);
                    }
                }
                dealt = damage_apply(target, damage, 0) + ((int)(short)swing_damage);
                spell_break_concealment(attacker);
                if (dealt != 0) {
                    total_dealt += dealt;
                    damage_knockback(target, dealt * 10, D_00195ABC, dealt * 2);
                }
            }
        } else {
            i = 0;
            total_dealt = i;
            for (; i < 5; i++) {
                if ((rand() % 100) < (50 - ((player_character->reflexes - 2) * 10)) && attacker_character->attack_damage[i][0] != 0 && damage_roll_to_hit(attacker_character, target_character, body_part, chance, -1, 0) != 0) {
                    damage = rand_range(attacker_character->attack_damage[i][0], attacker_character->attack_damage[i][1]);
                    if (backstab_chance != 0) {
                        if (rand_range(1, 100) <= backstab_chance) {
                            damage = damage * 3;
                            hud_message_add(D_0018506F);
                        }
                    }
                    dealt = damage_apply(target, damage, 0);
                    if (attacker == player_entity) dealt += (int)(short)swing_damage;
                    total_dealt += dealt;
                    if (attacker == player_entity && dealt != 0) {
                        damage_knockback(target, dealt * 10, D_00195ABC, dealt * 2);
                    }
                    if (dealt != 0) damage_monster_hit_effects(attacker, target);
                    if (attacker->type == 18) damage_namira_reflect(attacker, target, dealt);
                }
            }
        }
        if (total_dealt != 0) {
            sound_play((rand() & 1) + 380, target, 110);
            spell_break_concealment(attacker);
        } else {
            sound_play(damage_miss_sound(0, target_character->mobile_id), target, 110);
        }
        return;
    }
    if (target_character->min_metal_to_hit > weapon->material && attacker == player_entity) {
        hud_message_add((int)D_00170968);
    }
    if (target_character->min_metal_to_hit <= weapon->material && damage_roll_to_hit(attacker_character, target_character, body_part, chance, weapon->index, (int)(short)*(short *)(material_to_hit + (weapon->material * 2))) != 0) {
        damage = rand_range((int)(short)*(short *)(weapon_damage_min + (weapon->index << 2)), (int)(short)*(short *)(weapon_damage_max + (weapon->index << 2)));
        if (attacker_character->race == 1 && weapon->index < 16) damage += attacker_character->level / 3;
        if (attacker_character->race == 5 && weapon->index >= 16) damage += attacker_character->level / 3;
        if (attacker_character->race == 3) damage += attacker_character->level / 4;
        if (attacker == player_entity) {
            damage += (int)(short)swing_damage;
            if ((((int)(short)*(short *)(weapon_proficiency_bits + (weapon->index * 2))) & player_class->expert_weapons) != 0) {
                damage += (player_character->level / 3) + 1;
            }
        }
        if (((int)(unsigned short)(player_character->flags & 512)) != 0 && D_0019628E != 0) {
            damage += (damage << 6) / 256;
        }
        if (D_0019628E != 0) damage += (damage << 5) / 256;
        if (target_character->race == 15) {
            if (weapon->material == 2) damage <<= 1;
            if (((int)(unsigned short)(weapon->item_flags & 16)) == 0) damage >>= 1;
        }
        damage += ((int)(short)*(short *)(material_to_hit + (weapon->material * 2))) / 10;
        damage += ((int)&*(signed char *)((char *)(attacker_character->attributes[ATTR_STR] + *(int *)D_00195A08) - 50)) / 5;
        if (damage < 1) damage = 0;
        damage += damage_bonus_vs_target(weapon, attacker_character, target_character);
        if (backstab_chance != 0) {
            if (rand_range(1, 100) <= backstab_chance) {
                damage = damage * 3;
                hud_message_add(D_0018506F);
            }
        }
        dealt = damage_apply(target, damage, attacker_character->equipped[hand_slot]);
        if (D_0019629C != 0) return;
        if (attacker == player_entity && dealt != 0) {
            damage_knockback(target, dealt * 10, D_00195ABC, dealt * 2);
        }
        if (dealt != 0 && attacker_character->equipped[hand_slot]->children != 0) {
            poison = &attacker_character->equipped[hand_slot]->children->data.disease;
            poison_apply(target, poison->id, 0);
            object_free_single(attacker_character->equipped[hand_slot]->children);
        }
        if (dealt != 0) {
            spell_break_concealment(attacker);
            damage_weapon_strike_effects(weapon, attacker, target, dealt);
            if (attacker->type == 18) damage_namira_reflect(attacker, target, dealt);
            sound_play(rand_range(0, 4) + 378, target, 110);
            if (attacker_character->equipped[hand_slot] != 0) item_wear_from_hit(attacker_character->equipped[hand_slot], dealt);
            if (target_character->equipped[(int)(unsigned char)body_part_armor_slots[body_part]] != 0) {
                item_wear_from_hit(target_character->equipped[(int)(unsigned char)body_part_armor_slots[body_part]], dealt);
            }
        }
        return;
    }
    sound_play(damage_miss_sound(weapon, target_character->mobile_id), target, 110);
}

int damage_roll_to_hit(struct character *attacker, struct character *target, int body_part, int chance, int weapon_index, int material_bonus)
{
    struct career *career;

    if (attacker->health < (attacker->max_health >> 3)) {
        career = &attacker->career;
        if (((int)(unsigned short)(career->flags & 4)) != 0) chance += 5;
    }
    if (target->health < (target->max_health >> 3)) {
        career = &target->career;
        if (((int)(unsigned short)(career->flags & 4)) != 0) chance += -5;
    }
    if (attacker == player_character) chance += (int)(short)swing_to_hit;
    if (target == player_character) chance -= D_0018DDDC;
    chance += material_bonus;
    chance += (attacker->attributes[ATTR_AGI] - target->attributes[ATTR_AGI]) / 10;
    chance += (attacker->attributes[ATTR_LUC] - target->attributes[ATTR_LUC]) / 10;
    chance += target->armor_values[body_part];
    if (target->race < 43) chance += 40;
    chance += attacker->to_hit_bonus;
    if (attacker->race == 1 && weapon_index < 16) chance += attacker->level / 3;
    if (attacker->race == 5 && weapon_index >= 16) chance += attacker->level / 3;
    if (attacker->race == 3) chance += attacker->level / 4;
    chance -= attacker->skills[SKILL_DODGING].value / 4;
    if (rand_range(1, 100) < attacker->skills[SKILL_CRITICAL_STRIKE].value) {
        chance += attacker->skills[SKILL_CRITICAL_STRIKE].value / 10;
    }
    if (attacker == player_character) skill_add_uses(34, 1);
    if (target == player_character) skill_add_uses(20, 1);
    chance += -50;
    if (chance < 3) {
        chance = 3;
    } else if (chance > 97) {
        chance = 97;
    }
    return ((rand_range(0, 100) <= chance) ? 1 : 0);
}

int damage_bonus_vs_target(struct item *weapon, struct character *attacker, struct character *target)
{
    int i;
    int target_race;
    int bonus;
    struct career *career;

    bonus = 0;
    career = &attacker->career;
    if (career->attack_modifier_flags != 0) {
        if (((int)(unsigned char)(career->attack_modifier_flags & 1)) != 0 && *(signed char *)(monster_category + target->race) == 0) {
            bonus += attacker->level;
        }
        if (((int)(unsigned char)(career->attack_modifier_flags & 2)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + target->race)) == 1) {
            bonus += attacker->level;
        }
        if (((int)(unsigned char)(career->attack_modifier_flags & 4)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + target->race)) == 2) {
            bonus += attacker->level;
        }
        if (((int)(unsigned char)(career->attack_modifier_flags & 8)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + target->race)) == 3) {
            bonus += attacker->level;
        }
        if (((int)(unsigned char)(career->attack_modifier_flags & 16)) != 0 && *(signed char *)(monster_category + target->race) == 0) {
            bonus -= attacker->level;
        }
        if (((int)(unsigned char)(career->attack_modifier_flags & 32)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + target->race)) == 1) {
            bonus -= attacker->level;
        }
        if (((int)(unsigned char)(career->attack_modifier_flags & 64)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + target->race)) == 2) {
            bonus -= attacker->level;
        }
        if (((int)(unsigned char)(career->attack_modifier_flags & 128)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + target->race)) == 3) {
            bonus -= attacker->level;
        }
    }
    if (weapon->enchantments[0].type == (-1)) return 0;
    target_race = target->race;
    for (i = 0; i < 10; i++) {
        if (weapon->enchantments[i].type == (-1)) return 0;
        if (weapon->enchantments[i].type == 4 && (short)((unsigned short)(unsigned char)*(signed char *)(monster_category + target_race)) == weapon->enchantments[i].param) {
            bonus += attacker->level;
        }
        if (weapon->enchantments[i].type == 20 && (short)((unsigned short)(unsigned char)*(signed char *)(monster_category + target_race)) == weapon->enchantments[i].param) {
            bonus -= attacker->level;
        }
        if (weapon->enchantments[i].type == 21 && weapon->enchantments[i].param == 0) {
            damage_apply((struct record *)((char *)attacker - 71), (int)&*(signed char *)((char *)(attacker->level >> 2) + 1), 0);
        }
    }
    return bonus;
}

int damage_heal(struct character *character, int amount)
{
    int healed;

    healed = amount;
    character->health += amount;
    if (character->health > character->max_health) {
        healed -= character->health - character->max_health;
        character->health = character->max_health;
    }
    return healed;
}

int damage_apply(struct record *target, int damage, struct record *weapon)
{
    struct character *character;
    int unused;
    struct item *weapon_item;

    if (target == 0) return 0;
    if (target == player_entity && ((struct bf8_6_1 *)&cheat_flags)->f != 0) return 0;
    if (D_0019628D == 0 && target == player_entity && guards_are_present() != 0) {
        if (((int)(unsigned char)game_mode) == 21) return 0;
        crime_reputation_penalty();
        msgbox_yes_no_rsc(15);
        D_0019628D = 1;
        if (((int)D_00196271) == 1) {
            crime_guards_or_court(1);
            return 0;
        }
    }
    if (D_001962B2 != 0) return 0;
    if (target->wait_state == 99) monster_wake_all();
    if (damage <= 0) return 0;
    character = &target->data.character;
    if (target != player_entity) {
        character->give_up_timer = 60;
    } else {
        if ((character->conditions & 0x400000) != 0) {
            character->shield_points -= damage;
            if (character->shield_points > 3000000) {
                damage = -character->shield_points;
                character->shield_points = 0;
            } else {
                damage = 0;
            }
        }
        if (damage == 0) return 0;
    }
    if (target != player_entity) {
        target->data.character.flags &= ~0x8000;
        damage_spawn_splash(target, 0, 2);
    } else {
        damage_player_hurt(damage);
    }
    if (weapon != 0) {
        weapon_item = &weapon->data.item;
        if (weapon_item->enchantments[0].type == 26 && weapon_item->enchantments[0].param == 6) {
            monster_wabbajack(weapon, target);
        } else if (weapon_item->enchantments[0].type == 26 && weapon_item->enchantments[0].param == 1) {
            if (spfx_resist_roll(4, 2, character, &character->career, 2, 0) != 100) {
                damage_creature_death(target);
                item_damage(weapon, (int)&*(signed char *)((char *)(character->health / 8) + 1));
                quest_raise_event(21, target, 0);
                return character->health;
            }
        }
    }
    character->health -= damage;
    if (character->health < 1) {
        if (guards_are_present() != 0 && target == player_entity) {
            crime_guards_or_court(0);
        } else {
            damage_creature_death(target);
        }
    } else if (damage != 0) {
        quest_raise_event(21, target, 0);
    }
    D_001940D7 |= 8;
    return damage;
}

void damage_find_empty_soul_trap_cb(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 9 && object->children == 0) {
        scratch_object = object;
        return;
    }
    if (item->group != 27 || item->index != 1) return;
    if (object->children != 0) return;
    scratch_object = object;
}

void damage_drop_at_death_cb(struct record *object)
{
    object->x = found_object->x;
    object->y = found_object->y;
    object->z = found_object->z;
}
