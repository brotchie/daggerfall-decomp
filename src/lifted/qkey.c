/* qkey.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
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
extern struct record *D_00195AA8;
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
extern int flats_cfg_find(int);
extern int sound_play(int, struct record *, int);
extern int disk_open_data(int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int spfx_resist_roll(int, int, struct character *, struct career *, int, int);
extern int name_generate_seeded(unsigned char, unsigned char, int);
extern int building_name(struct building *);
extern int object_free_single(struct record *);
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
extern void parse_expand(int, int);
extern void quest_raise_event(int, struct record *, struct record *);
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

int quest_symbol_text(int a1, int a2, int a3)
{
    struct faction *l_50;
    struct qbn_foe *l_4C;
    struct qbn_place *l_48;
    struct qbn_place *l_44;
    struct qbn_item *l_40;
    struct qbn_person *l_3C;
    struct qbn_timer *l_38;
    struct qbn_timer *l_34;
    struct record *l_30;
    struct building *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct character *l_18;
    int l_14;
    {
        int l_60;

        if (current_quest != 0) {
            l_40 = quest_section(current_quest, 0);
            for (l_24 = 0; current_quest->section_counts[0] > l_24; l_24++, l_40++) {
                if (a1 == l_40->symbol) break;
            }
            if (current_quest->section_counts[0] > l_24) {
                l_40->flags &= 127;
                l_30 = l_40->object;
                if (l_30 != 0) {
                    if ((text_macro_item = &l_30->data.item)->group == 28 && text_macro_item->index == 0) {
                        return itoa(text_macro_item->value, (int)text_rsc_buffer, 10);
                    }
                    if (text_macro_item->group == 7) {
                        l_14 = (int)(*(char **)&D_00147954 + 90000);
                        mc_set_location(115, (int)D_001708F0);
                        mc_sprintf((int)text_buffer, (int)D_001708F7, (int)(unsigned short)(short)text_macro_item->message);
                        l_1C = disk_open_data((int)text_buffer);
                        read(l_1C, l_14, 234);
                        close(l_1C);
                        mc_strncpy((int)text_rsc_buffer, l_14, 2048, (int)D_001708F0, 119);
                        return (int)text_rsc_buffer;
                    }
                    parse_expand((int)D_00170909, (int)D_00190B44);
                    return (int)D_00190B44;
                }
            }
            l_48 = quest_section(current_quest, 4);
            l_44 = 0;
            for (l_24 = 0; current_quest->section_counts[4] > l_24; l_24++, l_48++) {
                if (a1 == l_48->symbol) {
                    l_44 = l_48;
                    break;
                }
            }
            if (l_44 != 0) {
                l_48->flags &= 127;
                l_30 = l_48->object;
                if (l_30 != 0) {
                    if ((a2 & 240) == 0) {
                        l_2C = (struct building *)RECORD_DATA(l_30);
                        return building_name(l_2C);
                    }
                    if ((a2 & 240) <= 32) return (int)RECORD_DATA(l_30) + 26;
                    if ((a2 & 240) != 0) {
                        return *(int *)(region_names + (((int)(unsigned short)l_30->region) << 2));
                    }
                }
            }
            l_3C = quest_section(current_quest, 3);
            for (l_24 = 0; current_quest->section_counts[3] > l_24; l_24++, l_3C++) {
                if (a1 == l_3C->symbol) break;
            }
            if (current_quest->section_counts[3] <= l_24) goto L2D3DB;
            l_3C->flags &= ~0x8000;
            l_30 = l_3C->object;
            if (l_30 == 0) goto L2D3DB;
            if (l_30->type == 65) return (int)faction_find(l_30->faction_id)->name;
            if (((int)(unsigned short)(l_30->flags & 4)) != 0) {
                l_60 = 1;
            } else {
                l_60 = 0;
            }
            text_macro_gender = *(signed char *)&l_60;
            if ((a2 & 15) == 1) return flats_cfg_find(l_30->image) + 9;
            if ((a2 & 15) > 1) {
                if (l_30->faction_id != 0) return (int)faction_find(l_30->faction_id)->name;
                return (int)D_0017090D;
            }
            switch (a2 & 240) {
            case 0:
                l_50 = faction_find((int)(short)l_30->data.building.faction_id);
                if (l_50 != 0 && l_50->type == 4) return (int)l_50->name;
                if (l_3C->kind == (-1)) {
                    l_50 = faction_find(l_3C->faction_id);
                    if (l_50->type == 4) return (int)l_50->name;
                }
                return name_generate_seeded((int)(unsigned char)D_001841E3[l_30->home_region], (int)(unsigned char)((signed char)l_30->flags & 4), l_30->name_seed);
            case 16:
                if ((l_30->id >> 16) == 50015) return (int)D_00170918;
                if ((l_30->id >> 16) == 50027) return (int)D_00170928;
                if ((l_30->id >> 16) == 50029) return (int)D_0017093A;
                if ((l_30->id >> 16) == 50033) return (int)D_00170949;
                if ((l_30->id >> 16) == 50041) return (int)D_00170959;
                return building_name((struct building *)RECORD_DATA(l_30));
            case 32:
                return (int)RECORD_DATA(l_30) + 26;
            case 48:
                return *(int *)(region_names + (l_30->home_region << 2));
            default:
L2D3DB:;
                l_4C = quest_section(current_quest, 7);
                for (l_24 = 0; current_quest->section_counts[7] > l_24; l_24++, l_4C++) {
                    if (a1 == l_4C->symbol) break;
                }
                if (current_quest->section_counts[7] > l_24) {
                    l_30 = l_4C->object;
                    if (l_30 != 0) {
                        l_18 = &l_30->data.character;
                        if ((a2 & 15) == 1) {
                            if (l_18->mobile_id >= 128) {
                                return name_generate_seeded((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)((signed char)l_18->flags & 1), l_30->name_seed);
                            }
                            return name_generate_seeded((int)(unsigned char)D_0017A144[current_quest->id % 3], 0, l_30->name_seed);
                        }
                        if (l_18->mobile_id >= 128) {
                            return *(int *)(D_0017CAFA + (l_18->mobile_id << 2));
                        }
                        return *(int *)(monster_names + (l_18->race << 2));
                    }
                }
            }
            l_38 = quest_section(current_quest, 6);
            for (l_24 = 0; current_quest->section_counts[6] > l_24; l_24++, l_38++) {
                if (a1 == l_38->state_hash) break;
            }
            l_20 = l_38->delay;
            if (a3 != 0) {
                l_34 = quest_section(current_quest, 6);
                for (l_24 = 0; current_quest->section_counts[6] > l_24; l_24++, l_34++) {
                    if (a3 == l_34->state_hash) break;
                }
                l_20 += l_34->delay;
            }
            if (current_quest->section_counts[6] > l_24) {
                if ((a2 & 15) != 0) {
                    return itoa((l_20 + 1439) / 1440, (int)text_rsc_buffer, 10);
                }
                return itoa(l_20, (int)text_rsc_buffer, 10);
            }
        }
        return text_blank;
    }
}

void damage_resolve_attack(struct record *a1, struct record *a2, int a3)
{
    struct character *l_50;
    struct character *l_4C;
    struct item *l_48;
    struct disease *l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    struct monster_anim *l_10;

    if (((int)(unsigned char)game_mode) == 21) return;
    if (a2 != player_entity) {
        for (l_30 = 0; l_30 < creature_count; l_30++) {
            if (creature_list[l_30] == a2) {
                l_24 = l_30;
                break;
            }
        }
    }
    if (a1 == player_entity && rand_range(1, 100) <= 20) {
        if (player_character->race == 8) {
            sound_play(((((int)(unsigned short)(player_character->flags & 1)) != 0) ? (rand() & 1) + 10281 : (rand() & 1) + 10301), player_object, 100);
        } else if (player_character->race == 9) {
            sound_play((rand() & 1) + 10091, player_object, 100);
        } else if (player_character->race == 10) {
            sound_play((rand() & 1) + 10141, player_object, 100);
        }
    }
    l_40 = (int)(unsigned char)struck_body_part_table[rand_range(0, 19)];
    l_50 = &a1->data.character;
    l_4C = &a2->data.character;
    D_00195AA8 = l_50->equipped[a3];
    l_48 = &l_50->equipped[a3]->data.item;
    if (l_50->equipped[a3] != 0 && l_48->group == 3 && player_character->race < 9) {
        l_3C = (int)(unsigned char)weapon_skills[l_48->index];
    } else {
        l_3C = 30;
    }
    l_34 = l_50->skills[l_3C].value;
    if (l_3C != 30 && l_50 == player_character && (((int)(short)*(short *)(weapon_proficiency_bits + (l_48->index * 2))) & player_class->expert_weapons) != 0) {
        l_34 += player_character->level;
    }
    l_18 = 0;
    if (a1 == player_entity) {
        l_10 = &a2->data.monster.anim;
        if (l_10->anim_request == 57) return;
        if (l_10->anim_facing > 2) {
            l_34 += player_character->skills[SKILL_BACKSTABBING].value;
            skill_add_uses(19, 1);
            l_18 = player_character->skills[SKILL_BACKSTABBING].value;
        }
    }
    if (a1 == player_entity) skill_add_uses(l_3C, 1);
    if (l_3C == 30) {
        if (a1 == player_entity) {
            if (damage_roll_to_hit(l_50, l_4C, l_40, l_34, -1, 0) != 0) {
                l_28 = rand_range((player_character->skills[SKILL_HAND_TO_HAND].value / 10) + 1, (int)&*(signed char *)((char *)(player_character->skills[SKILL_HAND_TO_HAND].value / 5) + 1));
                if (l_18 != 0) {
                    if (rand_range(1, 100) <= l_18) {
                        l_28 = l_28 * 3;
                        hud_message_add(D_0018506F);
                    }
                }
                l_38 = damage_apply(a2, l_28, 0) + ((int)(short)swing_damage);
                spell_break_concealment(a1);
                if (l_38 != 0) {
                    l_2C += l_38;
                    damage_knockback(a2, l_38 * 10, D_00195ABC, l_38 * 2);
                }
            }
        } else {
            l_30 = 0;
            l_2C = l_30;
            for (; l_30 < 5; l_30++) {
                if ((rand() % 100) < (50 - ((player_character->reflexes - 2) * 10)) && l_50->attack_damage[l_30][0] != 0 && damage_roll_to_hit(l_50, l_4C, l_40, l_34, -1, 0) != 0) {
                    l_28 = rand_range(l_50->attack_damage[l_30][0], l_50->attack_damage[l_30][1]);
                    if (l_18 != 0) {
                        if (rand_range(1, 100) <= l_18) {
                            l_28 = l_28 * 3;
                            hud_message_add(D_0018506F);
                        }
                    }
                    l_38 = damage_apply(a2, l_28, 0);
                    if (a1 == player_entity) l_38 += (int)(short)swing_damage;
                    l_2C += l_38;
                    if (a1 == player_entity && l_38 != 0) {
                        damage_knockback(a2, l_38 * 10, D_00195ABC, l_38 * 2);
                    }
                    if (l_38 != 0) damage_monster_hit_effects(a1, a2);
                    if (a1->type == 18) damage_namira_reflect(a1, a2, l_38);
                }
            }
        }
        if (l_2C != 0) {
            sound_play((rand() & 1) + 380, a2, 110);
            spell_break_concealment(a1);
        } else {
            sound_play(damage_miss_sound(0, l_4C->mobile_id), a2, 110);
        }
        return;
    }
    if (l_4C->min_metal_to_hit > l_48->material && a1 == player_entity) {
        hud_message_add((int)D_00170968);
    }
    if (l_4C->min_metal_to_hit <= l_48->material && damage_roll_to_hit(l_50, l_4C, l_40, l_34, l_48->index, (int)(short)*(short *)(material_to_hit + (l_48->material * 2))) != 0) {
        l_28 = rand_range((int)(short)*(short *)(weapon_damage_min + (l_48->index << 2)), (int)(short)*(short *)(weapon_damage_max + (l_48->index << 2)));
        if (l_50->race == 1 && l_48->index < 16) l_28 += l_50->level / 3;
        if (l_50->race == 5 && l_48->index >= 16) l_28 += l_50->level / 3;
        if (l_50->race == 3) l_28 += l_50->level / 4;
        if (a1 == player_entity) {
            l_28 += (int)(short)swing_damage;
            if ((((int)(short)*(short *)(weapon_proficiency_bits + (l_48->index * 2))) & player_class->expert_weapons) != 0) {
                l_28 += (player_character->level / 3) + 1;
            }
        }
        if (((int)(unsigned short)(player_character->flags & 512)) != 0 && D_0019628E != 0) {
            l_28 += (l_28 << 6) / 256;
        }
        if (D_0019628E != 0) l_28 += (l_28 << 5) / 256;
        if (l_4C->race == 15) {
            if (l_48->material == 2) l_28 <<= 1;
            if (((int)(unsigned short)(l_48->item_flags & 16)) == 0) l_28 >>= 1;
        }
        l_28 += ((int)(short)*(short *)(material_to_hit + (l_48->material * 2))) / 10;
        l_28 += ((int)&*(signed char *)((char *)(l_50->attributes[ATTR_STR] + *(int *)D_00195A08) - 50)) / 5;
        if (l_28 < 1) l_28 = 0;
        l_28 += damage_bonus_vs_target(l_48, l_50, l_4C);
        if (l_18 != 0) {
            if (rand_range(1, 100) <= l_18) {
                l_28 = l_28 * 3;
                hud_message_add(D_0018506F);
            }
        }
        l_38 = damage_apply(a2, l_28, l_50->equipped[a3]);
        if (D_0019629C != 0) return;
        if (a1 == player_entity && l_38 != 0) {
            damage_knockback(a2, l_38 * 10, D_00195ABC, l_38 * 2);
        }
        if (l_38 != 0 && l_50->equipped[a3]->children != 0) {
            l_44 = &l_50->equipped[a3]->children->data.disease;
            poison_apply(a2, l_44->id, 0);
            object_free_single(l_50->equipped[a3]->children);
        }
        if (l_38 != 0) {
            spell_break_concealment(a1);
            damage_weapon_strike_effects(l_48, a1, a2, l_38);
            if (a1->type == 18) damage_namira_reflect(a1, a2, l_38);
            sound_play(rand_range(0, 4) + 378, a2, 110);
            if (l_50->equipped[a3] != 0) item_wear_from_hit(l_50->equipped[a3], l_38);
            if (l_4C->equipped[(int)(unsigned char)body_part_armor_slots[l_40]] != 0) {
                item_wear_from_hit(l_4C->equipped[(int)(unsigned char)body_part_armor_slots[l_40]], l_38);
            }
        }
        return;
    }
    sound_play(damage_miss_sound(l_48, l_4C->mobile_id), a2, 110);
}

int damage_roll_to_hit(struct character *a1, struct character *a2, int a3, int a4, int a5, int a6)
{
    struct career *l_10;

    if (a1->health < (a1->max_health >> 3)) {
        l_10 = &a1->career;
        if (((int)(unsigned short)(l_10->flags & 4)) != 0) a4 += 5;
    }
    if (a2->health < (a2->max_health >> 3)) {
        l_10 = &a2->career;
        if (((int)(unsigned short)(l_10->flags & 4)) != 0) a4 += -5;
    }
    if (a1 == player_character) a4 += (int)(short)swing_to_hit;
    if (a2 == player_character) a4 -= D_0018DDDC;
    a4 += a6;
    a4 += (a1->attributes[ATTR_AGI] - a2->attributes[ATTR_AGI]) / 10;
    a4 += (a1->attributes[ATTR_LUC] - a2->attributes[ATTR_LUC]) / 10;
    a4 += a2->armor_values[a3];
    if (a2->race < 43) a4 += 40;
    a4 += a1->to_hit_bonus;
    if (a1->race == 1 && a5 < 16) a4 += a1->level / 3;
    if (a1->race == 5 && a5 >= 16) a4 += a1->level / 3;
    if (a1->race == 3) a4 += a1->level / 4;
    a4 -= a1->skills[SKILL_DODGING].value / 4;
    if (rand_range(1, 100) < a1->skills[SKILL_CRITICAL_STRIKE].value) {
        a4 += a1->skills[SKILL_CRITICAL_STRIKE].value / 10;
    }
    if (a1 == player_character) skill_add_uses(34, 1);
    if (a2 == player_character) skill_add_uses(20, 1);
    a4 += -50;
    if (a4 < 3) {
        a4 = 3;
    } else if (a4 > 97) {
        a4 = 97;
    }
    return ((rand_range(0, 100) <= a4) ? 1 : 0);
}

int damage_bonus_vs_target(struct item *a1, struct character *a2, struct character *a3)
{
    int l_20;
    int l_1C;
    int l_18;
    struct career *l_14;

    l_18 = 0;
    l_14 = &a2->career;
    if (l_14->attack_modifier_flags != 0) {
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 1)) != 0 && *(signed char *)(monster_category + a3->race) == 0) {
            l_18 += a2->level;
        }
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 2)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + a3->race)) == 1) {
            l_18 += a2->level;
        }
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 4)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + a3->race)) == 2) {
            l_18 += a2->level;
        }
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 8)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + a3->race)) == 3) {
            l_18 += a2->level;
        }
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 16)) != 0 && *(signed char *)(monster_category + a3->race) == 0) {
            l_18 -= a2->level;
        }
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 32)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + a3->race)) == 1) {
            l_18 -= a2->level;
        }
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 64)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + a3->race)) == 2) {
            l_18 -= a2->level;
        }
        if (((int)(unsigned char)(l_14->attack_modifier_flags & 128)) != 0 && ((int)(unsigned char)*(signed char *)(monster_category + a3->race)) == 3) {
            l_18 -= a2->level;
        }
    }
    if (a1->enchantments[0].type == (-1)) return 0;
    l_1C = a3->race;
    for (l_20 = 0; l_20 < 10; l_20++) {
        if (a1->enchantments[l_20].type == (-1)) return 0;
        if (a1->enchantments[l_20].type == 4 && (short)((unsigned short)(unsigned char)*(signed char *)(monster_category + l_1C)) == a1->enchantments[l_20].param) {
            l_18 += a2->level;
        }
        if (a1->enchantments[l_20].type == 20 && (short)((unsigned short)(unsigned char)*(signed char *)(monster_category + l_1C)) == a1->enchantments[l_20].param) {
            l_18 -= a2->level;
        }
        if (a1->enchantments[l_20].type == 21 && a1->enchantments[l_20].param == 0) {
            damage_apply((struct record *)((char *)a2 - 71), (int)&*(signed char *)((char *)(a2->level >> 2) + 1), 0);
        }
    }
    return l_18;
}

int damage_heal(struct character *a1, int a2)
{
    int l_18;

    l_18 = a2;
    a1->health += a2;
    if (a1->health > a1->max_health) {
        l_18 -= a1->health - a1->max_health;
        a1->health = a1->max_health;
    }
    return l_18;
}

int damage_apply(struct record *a1, int a2, struct record *a3)
{
    struct character *l_1C;
    int l_18;
    struct item *l_14;

    if (a1 == 0) return 0;
    if (a1 == player_entity && ((struct bf8_6_1 *)&cheat_flags)->f != 0) return 0;
    if (D_0019628D == 0 && a1 == player_entity && guards_are_present() != 0) {
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
    if (a1->wait_state == 99) monster_wake_all();
    if (a2 <= 0) return 0;
    l_1C = &a1->data.character;
    if (a1 != player_entity) {
        l_1C->give_up_timer = 60;
    } else {
        if ((l_1C->conditions & 0x400000) != 0) {
            l_1C->shield_points -= a2;
            if (l_1C->shield_points > 3000000) {
                a2 = -l_1C->shield_points;
                l_1C->shield_points = 0;
            } else {
                a2 = 0;
            }
        }
        if (a2 == 0) return 0;
    }
    if (a1 != player_entity) {
        a1->data.character.flags &= ~0x8000;
        damage_spawn_splash(a1, 0, 2);
    } else {
        damage_player_hurt(a2);
    }
    if (a3 != 0) {
        l_14 = &a3->data.item;
        if (l_14->enchantments[0].type == 26 && l_14->enchantments[0].param == 6) {
            monster_wabbajack(a3, a1);
        } else if (l_14->enchantments[0].type == 26 && l_14->enchantments[0].param == 1) {
            if (spfx_resist_roll(4, 2, l_1C, &l_1C->career, 2, 0) != 100) {
                damage_creature_death(a1);
                item_damage(a3, (int)&*(signed char *)((char *)(l_1C->health / 8) + 1));
                quest_raise_event(21, a1, 0);
                return l_1C->health;
            }
        }
    }
    l_1C->health -= a2;
    if (l_1C->health < 1) {
        if (guards_are_present() != 0 && a1 == player_entity) {
            crime_guards_or_court(0);
        } else {
            damage_creature_death(a1);
        }
    } else if (a2 != 0) {
        quest_raise_event(21, a1, 0);
    }
    D_001940D7 |= 8;
    return a2;
}

void damage_find_empty_soul_trap_cb(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->enchantments[0].type == 26 && l_18->enchantments[0].param == 9 && a1->children == 0) {
        scratch_object = a1;
        return;
    }
    if (l_18->group != 27 || l_18->index != 1) return;
    if (a1->children != 0) return;
    scratch_object = a1;
}

void damage_drop_at_death_cb(struct record *a1)
{
    a1->x = found_object->x;
    a1->y = found_object->y;
    a1->z = found_object->z;
}
