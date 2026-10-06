/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00078566 */
#include "records.h"

extern char D_00176844[];
extern char D_0017684E[];
extern char D_0017685B[];
extern char D_00176868[];
extern char D_00176876[];
extern struct monster_template monster_table[];
extern signed char class_creature_types[];
extern signed char text_buffer[];
extern char D_00190704[];
extern int monster_bsa_handle;
extern struct character *player_character;
extern signed char D_00196293;
extern int archive_find_record(int, char *, int);
extern iptr archive_read_record(int, int, iptr);
extern void loot_generate(int, struct record *, int, int);
extern int monster_set_action(struct record *, int, int);
extern int monster_alloc_anim_slot(void);
extern iptr disk_read_file(char *, iptr);
extern int monster_roll_d8_health(int, int);
extern int monster_roll_class_health(int, int, int);
extern void monster_init_gear(struct record *);
extern void monster_maybe_give_map(struct record *, int);
extern int rand_range(int, int);
extern int rand();
extern int mc_memset();
extern int mc_strncpy();
extern int mc_memcpy();
extern int xn_anim_reset();
extern int xn_tex_archive_set_translucent();
extern struct tex_cache_entry *xn_tex_cache_lookup(int, int, int);
extern int xn_tex_cache_flush();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);

void monster_init(struct record *monster, int monster_type)
{
    struct character *monster_char;
    struct career *career;
    struct monster_anim *anim;
    struct monster_template *table_row;
    struct tex_cache_entry *texture;
    int i;
    int skill_value;
    int texture_base;
    int class_index;
    int type_147;
    int record_index;
    {
        int ascr;

        monster_char = &monster->data.character;
        career = &monster_char->career;
        anim = (struct monster_anim *)((char *)career + 74);
        type_147 = 0;
        if (monster_type == 99 || monster_type == 98) monster_type = 0;
        monster_char->mobile_id = *(signed char *)&monster_type;
        if (monster_type >= 128) {
            if (monster_type == 147) {
                type_147 = 1;
                monster_type = 144;
                class_index = monster_type & 127;
            } else {
                class_index = monster_type & 127;
                monster_type = (int)(unsigned char)class_creature_types[class_index];
            }
        }
        if (monster_type >= 43) {
            if (type_147 != 0) {
                texture_base = rand_range(0, 4) + 491;
                monster_type = 0;
            } else if (class_index == 18) {
                texture_base = 340;
            } else {
                if (D_00196293 == 0) {
                    monster_type += rand() & 1;
                } else if (((int)(unsigned char)D_00196293) == 2) {
                    monster_type++;
                }
                texture_base = 432;
            }
        } else {
            texture_base = 255;
        }
        mc_set_location(73, (iptr)D_00176844);
        mc_sprintf((iptr)text_buffer, (iptr)D_0017684E, monster_type + texture_base);
        monster_char->anim_slot = monster_alloc_anim_slot();
        monster_char->action = 0;
        xn_anim_reset(anim);
        if (type_147 != 0) {
            monster_char->ascr_record = 145;
        } else {
            if (monster_type >= 43) {
                ascr = monster_type + 85;
            } else {
                ascr = monster_type;
            }
            monster_char->ascr_record = *(signed char *)&ascr;
        }
        mc_set_location(91, (iptr)D_00176844);
        mc_sprintf((iptr)text_buffer, (iptr)D_0017685B, monster_char->ascr_record);
        record_index = archive_find_record(monster_bsa_handle, text_buffer, 8);
        *(iptr *)(D_00190704 + (monster_char->anim_slot << 2)) = (iptr)(anim->anim_script = (char *)archive_read_record(monster_bsa_handle, record_index, 0));
        anim->anim_request = 0;
        do {
            texture = xn_tex_cache_lookup(texture_base + monster_type, 5, 0);
            if (texture == 0) xn_tex_cache_flush();
        } while (texture == 0);
        anim->frame_count = texture->image->frame_time;
        monster_char->magicka = (monster_char->max_magicka = 0);
        if (monster_type == 23 || monster_type == 18) xn_tex_archive_set_translucent(texture_base + monster_type);
        table_row = &monster_table[monster_type];
        if (monster_type >= 43) {
            mc_set_location(112, (iptr)D_00176844);
            mc_sprintf((iptr)text_buffer, (iptr)D_00176868, class_index);
            disk_read_file(text_buffer, (iptr)career);
        } else {
            mc_set_location(117, (iptr)D_00176844);
            mc_sprintf((iptr)text_buffer, (iptr)D_00176876, monster_type);
            record_index = archive_find_record(monster_bsa_handle, text_buffer, 8);
            archive_read_record(monster_bsa_handle, record_index, (iptr)career);
        }
        if (type_147 != 0) {
            monster_char->flags |= 0x2000;
            monster_char->race = 55;
        } else {
            monster_char->flags &= ~0x2000;
            monster_char->race = *(signed char *)&monster_type;
        }
        monster_char->race = *(signed char *)&monster_type;
        if (monster_type >= 43) {
            monster_char->level = player_character->level;
            if (monster_char->mobile_id == 146) monster_char->level += rand_range(3, 6);
            monster_char->health = (monster_char->max_health = monster_roll_class_health(career->hp_per_level, table_row->hp_bonus, monster_char->level));
        } else {
            monster_char->level = table_row->level;
            monster_char->health = (monster_char->max_health = monster_roll_d8_health(career->hp_per_level, table_row->hp_bonus));
        }
        mc_memset(monster_char->armor_values, table_row->armor * 5, 7, (iptr)D_00176844, 145, 7);
        monster_char->loot_table = table_row->loot_table;
        monster_char->table_flags = table_row->flags;
        mc_memcpy(monster_char->attack_damage, table_row->attack_damage, 10, (iptr)D_00176844, 148, 20);
        monster_char->min_metal_to_hit = table_row->min_metal_to_hit;
        mc_memcpy(monster_char->attributes, career->attributes, 16, (iptr)D_00176844, 150, 16);
        if (table_row->range_min == table_row->range_max) {
            monster_char->pad22A = table_row->range_min;
        } else {
            monster_char->pad22A = rand_range((unsigned char)table_row->range_min, (unsigned char)table_row->range_max);
        }
        monster_char->nav_direction = 2;
        monster_char->nav_blocked = 0;
        monster_char->nav_turn_count = 12;
        monster_char->nav_stuck_count = 0;
        monster_char->pad1F1 = 255;
        monster_char->give_up_timer = 0;
        skill_value = (monster_char->level * 5) + 30;
        if (skill_value > 100) skill_value = 100;
        for (i = 0; i < 35; i++) {
            monster_char->skills[i].value = skill_value;
        }
        mc_strncpy(monster_char->name, career->name, 32, (iptr)D_00176844, 169);
        monster_init_gear(monster);
        monster_maybe_give_map(monster, monster_char->mobile_id);
        monster->image = (texture_base + monster_type) << 7;
        if (table_row->loot_table != 0) {
            loot_generate(table_row->loot_table - 1, monster, player_character->level, (int)(unsigned short)(player_character->flags & 1));
        }
        monster_char->fall_velocity = 0;
        monster_char->faction_id = 0;
        monster_set_action(monster, 0, 48);
    }
}
