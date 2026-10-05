/* spfx.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char D_0012B508;
extern char D_00176D55[];
extern char D_00176D5C[];
extern char D_00176D7D[];
extern unsigned char player_environment;
extern struct spell *selected_spell;
extern signed char D_001841E3[];
extern int D_00184620;
extern int D_00184624;
extern int D_00184628;
extern int D_0018462C;
extern int D_00184630;
extern int D_00184634;
extern int D_00184638;
extern int D_0018463C;
extern int D_00184640;
extern int D_00185083;
extern int D_00185097;
extern signed char undead_daedra_ids[];
extern char spell_resist_flags[];
extern int D_0018DDD8;
extern char saved_positions[];
extern int D_0018DE20;
extern struct record *creature_list[];
extern char scratch_190de4[];
extern char scratch_190ee4[];
extern signed char D_001940D4;
extern signed char D_001940D6;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern int creature_count;
extern int spfx_popup_handler;
extern char D_00195B84[];
extern struct character *player_character;
extern int game_minutes;
extern char scratch_buffer[];
extern int free_later_count;
extern short spell_ready_cost;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char game_mode;
extern signed char player_ailment_flags;
extern int D_001A99F4;
extern int recall_anchor_location;
extern int recall_anchor_region;
extern int D_001A9A00;
extern int recall_anchor_environment;
extern char D_001AA458[];

extern struct faction *faction_find(short);
extern int damage_apply(struct record *, int, int);
extern int list_popup_poll(void);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int spfx_damage(struct record *, int, struct record *);
extern int name_generate(unsigned char, unsigned char);
extern struct record *object_free_single(struct record *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int inventory_open(int, int, int);
extern int rand();
extern int srand();
extern int mc_memset();
extern int mc_strncpy();
extern int strlen();
extern int spell_find_effect_type();
extern void damage_creature_death(struct record *);
extern void msgbox_show_rsc(int, int);
extern void spell_remove_effect_type(struct record *, int);
extern void fatigue_add(int);
extern void weapon_reload_hand_sprites(void);
extern void list_popup_open(int);
extern void msgbox_choice_rsc(short, unsigned char, unsigned char, int, unsigned char, unsigned char, unsigned char);
extern void object_free_later(struct record *);
extern void object_free_pending(void);
extern void player_position_save(int);
extern void player_position_restore(int);
extern void diminution_stub(void);
extern void player_horse_sounds_stop(void);
extern void map_goto_location(int, int, int, int);
extern void spfx_dispel_magic_cb(int);
extern void object_foreach(struct record *, int);
extern void inv_unequip_all_saved(void);
extern void inv_reequip_saved(void);
extern void transport_choose(int);
int spfx_drain(struct record *, int, struct record *);
int name_generate_seeded(unsigned char, unsigned char, int);
void spfx_dispel_creatures(int, int);
void spfx_heal(struct record *, int, struct record *);
void spfx_show_choice_list(int, int);
void spfx_created_item_expire_cb(struct record *);

void spfx_dispel(struct record *spell, int slot, struct record *target)
{
    struct spell *spell_data;
    struct spell *active_data;
    struct record *active;
    int text;
    int count;

    spell_data = &spell->data.spell;
    switch (spell_data->effects[slot].subtype) {
    case 0:
        count = 0;
        text = *(int *)scratch_buffer;
        active = player_entity->children;
        while (active != 0) {
            if (active->type == 9) {
                active_data = &active->data.spell;
                mc_strncpy(text, active_data->name, 4, (int)D_00176D55, 284);
                *(int *)(scratch_190ee4 + (count << 2)) = (int)active;
                *(int *)(scratch_190de4 + (count++ << 2)) = text;
                text += strlen(active_data->name) + 1;
            }
            active = active->next;
        }
        *(int *)(scratch_190de4 + (count << 2)) = 0;
        spfx_show_choice_list((int)scratch_190de4, (int)spfx_dispel_magic_cb);
        selected_spell = spell_data;
        D_001A99F4 = slot;
        return;
    case 1:
        spfx_dispel_creatures(spell_data->cast_chances[slot], 0);
        return;
    case 2:
        spfx_dispel_creatures(spell_data->cast_chances[slot], 1);
    default:;
    }
}

void spfx_dispel_creatures(int base_chance, int kind)
{
    int i;
    int j;
    int match_count;
    int chance;
    struct character *monster;

    for (i = 0; i < creature_count; i++) {
        monster = &creature_list[i]->data.character;
        j = 0;
        match_count = j;
        for (; j < 7; j++) {
            if ((signed char)monster->mobile_id == undead_daedra_ids[(kind * 7) + j]) {
                match_count++;
            }
        }
        if (match_count == 0) continue;
        chance = base_chance + ((player_character->level - monster->level) * 5);
        if (chance < 5) {
            chance = 5;
        } else if (chance > 95) {
            chance = 95;
        }
        if (rand_range(1, 100) > chance) continue;
        object_delete(creature_list[i]);
    }
}

int spfx_drain(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;
    int new_value;
    int stat;
    int fatigue;

    target_char = &target->data.character;
    spell_data = &spell->data.spell;
    stat = spell_data->effects[slot].subtype;
    switch ((unsigned)stat) {
    case 8:
        damage_apply(target, spell_data->cast_magnitudes[slot], 0);
        break;
    case 9:
        fatigue = target_char->fatigue;
        fatigue -= spell_data->cast_magnitudes[slot];
        if (fatigue < 1) fatigue = 1;
        target_char->fatigue = fatigue;
        break;
    default:
        new_value = target_char->attributes[stat] - spell_data->cast_magnitudes[slot];
        if (new_value < 1) {
            target_char->attributes[stat] = 1;
        } else {
            target_char->attributes[stat] -= spell_data->cast_magnitudes[slot];
        }
    }
    if (target_char == player_character) hud_message_add(D_00184620);
    return 1;
}

int spfx_elemental_resistance(struct record *spell, int slot, struct record *target)
{
    struct spell *spell_data;
    struct character *target_char;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    target_char->conditions |= *(int *)(spell_resist_flags + (spell_data->effects[slot].subtype << 2));
    target_char->resist_chances[spell_data->effects[slot].subtype] = spell_data->cast_chances[slot];
    return 1;
}

int spfx_fortify_attribute(struct record *spell, int slot, struct record *target)
{
    struct spell *spell_data;
    struct character *target_char;
    int new_value;
    int attribute;

    target_char = &target->data.character;
    spell_data = &spell->data.spell;
    attribute = spell_data->effects[slot].subtype;
    new_value = target_char->attributes[attribute] + spell_data->cast_magnitudes[slot];
    if (new_value > 100) spell_data->cast_magnitudes[slot] -= new_value - 100;
    target_char->attributes[attribute] += spell_data->cast_magnitudes[slot];
    if (target_char == player_character) hud_message_add(D_00184624);
    return 1;
}

void spfx_heal(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;
    int stat;

    target_char = &target->data.character;
    spell_data = &spell->data.spell;
    stat = spell_data->effects[slot].subtype;
    switch ((unsigned)stat) {
        return;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        target_char->attributes[stat] += spell_data->cast_magnitudes[slot];
        if (target_char->attributes[stat] > target_char->base_attributes[stat]) {
            target_char->attributes[stat] = target_char->base_attributes[stat];
        }
        return;
    case 8:
        target_char->health += spell_data->cast_magnitudes[slot];
        if (target_char->health > target_char->max_health) target_char->health = target_char->max_health;
        return;
    case 9:
        target_char->fatigue += spell_data->cast_magnitudes[slot] << 6;
        stat = (target_char->attributes[0] + target_char->attributes[4]) << 6;
        if (target_char->fatigue > stat) target_char->fatigue = stat;
        return;
    case 10:
        target_char->magicka += spell_data->cast_magnitudes[slot];
        if (target_char->magicka <= target_char->max_magicka) return;
        target_char->magicka = target_char->max_magicka;
    default:;
    }
}

void spfx_transfer(struct record *spell, int slot, struct record *target)
{
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    spell_data->cast_magnitudes[slot] >>= 1;
    spfx_drain(spell, slot, target);
    spfx_heal(spell, slot, player_entity);
}

int spfx_soul_trap(struct record *spell, int slot, struct record *target)
{
    struct record *trap;
    struct spell *spell_data;

    if (target->data.character.mobile_id >= 128) {
        hud_message_add((int)D_00176D5C);
        return 0;
    }
    hud_message_add((int)D_00176D7D);
    spell_data = &spell->data.spell;
    trap = object_create_child(target, 0, 0);
    trap->type = 19;
    trap->flags = 3;
    trap->trap_duration = spell_data->cast_durations[slot];
    trap->trap_chance = (unsigned short)spell_data->cast_chances[slot];
    spell_data->effects[slot].type = 255;
    return 1;
}

int spfx_invisibility(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 4;
    if (target_char == player_character) hud_message_add(D_00184628);
    return 1;
}

int spfx_levitate(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 8;
    if (target_char == player_character) {
        transport_choose(0);
        hud_message_add(D_0018462C);
        player_character->flags &= ~0x600;
        player_horse_sounds_stop();
    }
    return 1;
}

int spfx_light(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 16;
    return 1;
}

int spfx_lock(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    target_char->conditions |= 32;
    player_character->lock_open_chance = spell_data->cast_chances[slot];
    return 1;
}

int spfx_open(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    target_char->conditions |= 64;
    player_character->lock_open_chance = spell_data->cast_chances[slot];
    return 1;
}

int spfx_regenerate(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 128;
    if (target_char == player_character) hud_message_add(D_00184630);
    return 1;
}

int spfx_silence(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    if (rand_range(1, 100) > spell_data->cast_chances[slot]) {
        hud_message_add(D_00185097);
        return 0;
    }
    target_char = &target->data.character;
    target_char->conditions |= 0x100;
    if (target_char == player_character) hud_message_add(D_00184634);
    return 1;
}

int spfx_spell_absorption(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x200;
    return 1;
}

int spfx_spell_reflection(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x400;
    return 1;
}

int spfx_spell_resistance(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x800;
    return 1;
}

int spfx_chameleon(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x1000;
    if (target_char == player_character) hud_message_add(D_00184638);
    return 1;
}

int spfx_shadow(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x2000;
    if (target_char == player_character) hud_message_add(D_0018463C);
    return 1;
}

int spfx_slowfall(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x4000;
    if (target_char == player_character) hud_message_add(D_00184640);
    return 1;
}

int spfx_free_action(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    target_char = &target->data.character;
    target_char->conditions |= 0x8000;
    if ((target_char->conditions & 0x1) == 0) return 0;
    spell_data = &spell->data.spell;
    spell_remove_effect_type(target, 0);
    target_char->conditions &= ~0x1;
    return 1;
}

int spfx_jumping(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x10000;
    return 1;
}

int spfx_climbing(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x20000;
    return 1;
}

int spfx_morph_self(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    if ((target_char->conditions & 0x40000) != 0) {
        hud_message_add(D_00185097);
        return 0;
    }
    target_char->conditions |= 0x40000;
    target_char->shapechange_form = spell_data->effects[slot].subtype;
    return 1;
}

int spfx_water_breathing(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x80000;
    return 1;
}

int spfx_water_walking(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x100000;
    return 1;
}

int spfx_diminution(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x200000;
    diminution_stub();
    return 1;
}

int spfx_pacify(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    if (target->data.character.mobile_id >= 128) return 0;
    if (rand_range(1, 100) > spell_data->cast_chances[slot]) {
        hud_message_add(D_00185097);
        return 0;
    }
    target_char->flags |= 0x8000;
    return 1;
}

int spfx_charm(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    if (target->data.character.mobile_id < 128) return 0;
    if (rand_range(1, 100) > spell_data->cast_chances[slot]) {
        hud_message_add(D_00185097);
        return 0;
    }
    target_char->flags |= 0x8000;
    return 1;
}

int spfx_telekinesis(struct record *spell, int slot, struct record *target)
{
    return 1;
}

int spfx_astral_travel(struct record *spell, int slot, struct record *target)
{
    return 1;
}

int spfx_etherealness(struct record *spell, int slot, struct record *target)
{
    return 1;
}

int spfx_detect(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x800000;
    return 1;
}

int spfx_identify(struct record *spell, int slot, struct record *target)
{
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    player_character->lock_open_chance = spell_data->cast_chances[slot];
    player_character->magicka += spell_ready_cost;
    if (player_character->magicka > player_character->max_magicka) {
        player_character->magicka = player_character->max_magicka;
    }
    *(int *)D_001AA458 = (int)(short)spell_ready_cost;
    inventory_open(1, 4, 8);
    return 0;
}

int spfx_wizard_sight(struct record *spell, int slot, struct record *target)
{
    return 1;
}

int spfx_darkness(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x1000000;
    return 1;
}

int spell_recall_prompt(struct record *spell, int slot, struct record *target)
{
    msgbox_choice_rsc(4000, 20, 21, 0, 97, 116, 0);
    spfx_popup_handler = 1;
    return 0;
}

int spfx_comprehend_languages(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x2000000;
    return 1;
}

int spfx_intensify_fire(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x4000000;
    return 1;
}

int spfx_diminish_fire(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;

    target_char = &target->data.character;
    target_char->conditions |= 0x8000000;
    return 1;
}

int spfx_wall_of_stone(struct record *spell, int slot, struct record *target)
{
    return 0;
}

int spfx_wall_of_fire(struct record *spell, int slot, struct record *target)
{
    return 0;
}

int func_0008A858(struct record *object, int effect_type, int with_roll)
{
    struct spell *spell_data;
    int found_slot;

    object = object->children;
    while (object != 0) {
        if (object->type == 9) {
            spell_data = &object->data.spell;
            found_slot = spell_find_effect_type(spell_data, effect_type);
            if (found_slot != 0) {
                if (with_roll != 0) {
                    /* compares with the address of cast_chances, not a chance: an original bug */
                    return ((((unsigned)rand_range(1, 100)) < ((int)spell_data->cast_chances)) ? 1 : 0);
                }
                return 1;
            }
        }
        object = object->next;
    }
    return 0;
}

void spfx_effect_tick(struct record *spell, struct record *target, int slot)
{
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    switch (spell_data->effects[slot].type) {
case 1:
    spfx_damage(spell, slot, target);
    return;
case 18:
    spell_data->effects[slot].subtype = 8;
    spfx_heal(spell, slot, target);
    return;
case 39:
    D_001940D6 |= 16;
    player_character->detect_kind = spell_data->effects[slot].subtype;
default:;
}
}

void spfx_walk_effect_records(struct record *object, int (*callback)())
{
    struct record *next;

    while (object != 0) {
        next = object->next;
        if (object->type == 11 && ((int)(unsigned short)(object->flags & 32768)) != 0) {
            if (callback(&object->data.disease) == 0) object_free_single(object);
        }
        object = next;
    }
}

int spfx_disease_daily(struct disease *disease)
{
    int stat;
    int unused;
    int damage;

    if (disease->id > 99) return 1;
    damage = rand_range(disease->damage_min, disease->damage_max);
    if (disease->days_left == 254) return 1;
    if (disease->days_left != 255) {
        disease->days_left--;
        if (disease->days_left == 0) {
            disease->days_left = 254;
            return 1;
        }
    }
    player_ailment_flags |= 2;
    disease->stage = 1;
    for (stat = 0; stat < 11; stat++) {
        if (disease->stat_flags[stat] == 0) continue;
        switch ((unsigned)stat) {
        case 8:
            player_character->health -= damage;
            if (player_character->health <= 0) damage_creature_death(player_entity);
            break;
        case 9:
            fatigue_add(-damage);
            break;
        case 10:
            player_character->magicka -= damage;
            if (player_character->magicka < 0) player_character->magicka = 0;
            break;
        default:
            player_character->attributes[stat] -= damage;
            if (player_character->attributes[stat] <= 0) damage_creature_death(player_entity);
            disease->drained[stat] += damage;
            if (player_character->attributes[stat] < 1) {
                player_character->attributes[stat] = 1;
                disease->drained[stat] -= 1 - player_character->attributes[stat];
            }
        }
    }
    hud_message_add(D_00185083);
    return 1;
}

int func_0008AC0E(struct record *spell, struct record *target, int slot)
{
    struct spell *spell_data;
    struct character *target_char;
    int chance;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    chance = spell_data->cast_chances[slot] + ((spell->caster->data.character.level - target_char->level) * 5);
    return ((rand_range(0, 100) < chance) ? 1 : 0);
}

int spfx_disease_recover(struct disease *disease)
{
    int i;
    int changed;

    if (disease->days_left != 254) return 1;
    i = 0;
    changed = i;
    for (; i < 8; i++) {
        if (disease->stat_flags[i] != 0) {
            if (disease->stat_flags[i] < 0 && player_character->attributes[i] > player_character->base_attributes[i]) {
                changed = 1;
                player_character->attributes[i]--;
                disease->drained[i]++;
            } else if (disease->drained[i] != 0 && player_character->attributes[i] < player_character->base_attributes[i]) {
                changed = 1;
                player_character->attributes[i]++;
                disease->drained[i]--;
            }
        }
    }
    return changed;
}

int spfx_resist_roll(int element, int element_bit, struct character *target, struct career *target_class, int unused, int modifier)
{
    int chance;
    int roll;

    if ((target->conditions & *(int *)(spell_resist_flags + (element << 2))) != 0 && rand_range(1, 100) <= target->resist_chances[element]) {
        return 0;
    }
    chance = 50;
    if (target_class != 0) {
        if ((target_class->immunity_flags & element_bit) != 0) return 0;
        if ((target_class->critical_weakness_flags & element_bit) != 0) return 100;
        if ((target_class->low_tolerance_flags & element_bit) != 0) chance >>= 1;
        if ((target_class->resistance_flags & element_bit) != 0) chance += chance >> 1;
    }
    chance += modifier;
    chance += D_0018DDD8;
    if (element == 1 && target->race == 2) chance += 30;
    if (element == 4 && target->race == 0) chance += 30;
    if (chance < 5) {
        chance = 5;
    } else if (chance > 95) {
        chance = 95;
    }
    roll = rand_range(1, 100);
    if (roll > chance) return 100;
    if ((chance - 20) > roll) return 0;
    roll -= chance;
    return (-roll) * 5;
}

void spfx_cure_disease(struct record *object, struct character *target_char)
{
    struct disease *disease;
    int i;

    if (target_char == player_character) inv_unequip_all_saved();
    object = object->children;
    while (object != 0) {
        if (object->type == 11) {
            disease = &object->data.disease;
            if (disease->id >= 100) goto L8B013;
            for (i = 0; i < 8; i++) {
                target_char->attributes[i] += disease->drained[i];
                if (target_char->attributes[i] > target_char->base_attributes[i]) {
                    target_char->attributes[i] = target_char->base_attributes[i];
                }
            }
            object = object_free_single(object);
        } else {
L8B013:;
            object = object->next;
        }
    }
    player_character->special_infection_time = 0;
    player_character->special_infection = 0;
    if (target_char != player_character) return;
    inv_reequip_saved();
}

void spfx_show_choice_list(int strings, int callback)
{
    list_popup_open(strings);
    spfx_popup_handler = callback;
}

void spfx_popup_update(void)
{
    int choice;

    if (spfx_popup_handler == 0) return;
    D_0012B508 = 146;
    if (spfx_popup_handler == 1 && ((int)(unsigned char)game_mode) != 8) {
        if (((int)D_00196271) == 1) {
            recall_anchor_environment = (int)player_environment;
            recall_anchor_location = location_object->image;
            recall_anchor_region = (int)(unsigned char)current_region;
            if (((int)player_environment) == 2) {
                D_001A9A00 = player_object->parent->image;
            } else {
                D_001A9A00 = 0;
            }
            player_position_save(1);
        } else if (D_0018DE20 == 0) {
            msgbox_show_rsc(4001, 1);
        } else {
            map_goto_location(recall_anchor_region, recall_anchor_environment, recall_anchor_location, D_001A9A00);
            player_position_restore(1);
            mc_memset((int)saved_positions, 0, 48, (int)D_00176D55, 1342, 48);
        }
        spfx_popup_handler = 0;
        return;
    }
    if (((struct bf8_2_1 *)&D_001940D4)->f == 0 || (choice = list_popup_poll()) <= (-1)) return;
    ((int (*)())(spfx_popup_handler))(choice);
    spfx_popup_handler = 0;
}

int spell_extend_duration(struct record *target, struct spell *spell, int slot)
{
    struct record *active;
    struct spell *active_data;
    int i;

    active = target->children;
    while (active != 0) {
        if (active->type == 9) {
            active_data = &active->data.spell;
            for (i = 0; i < 3; i++) {
                if (active_data->effects[i].type == spell->effects[slot].type && active_data->effects[i].subtype == spell->effects[slot].subtype) {
                    active_data->cast_durations[i] += spell->cast_durations[slot];
                    return 1;
                }
            }
        }
        active = active->next;
    }
    return 0;
}

int spell_active_chance(struct record *target, unsigned char effect_type, unsigned char subtype)
{
    struct record *active;
    struct spell *active_data;
    int i;

    active = target->children;
    while (active != 0) {
        if (active->type == 9) {
            active_data = &active->data.spell;
            for (i = 0; i < 3; i++) {
                if (active_data->effects[i].type == effect_type && active_data->effects[i].subtype == subtype) {
                    return active_data->cast_chances[i];
                }
            }
        }
        active = active->next;
    }
    return 0;
}

void spfx_created_item_expire_cb(struct record *item)
{
    int slot;

    if (((int)(unsigned short)(item->flags & 4096)) == 0 || item->type != 2 || ((unsigned)item->expire_minutes) >= game_minutes) {
        return;
    }
    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == item) {
            player_character->equipped[slot] = 0;
            (*(int *)D_00195B84)++;
        }
    }
    object_free_later(item);
}

void spfx_expire_created_items(void)
{
    *(int *)D_00195B84 = 0;
    free_later_count = 0;
    object_foreach(player_object->children, (int)spfx_created_item_expire_cb);
    object_free_pending();
    if (*(int *)D_00195B84 == 0) return;
    weapon_reload_hand_sprites();
}

int name_generate_seeded(unsigned char bank, unsigned char female, int seed)
{
    int saved_seed;
    int name;

    saved_seed = rand();
    srand(seed);
    name = name_generate((int)(unsigned char)bank, (int)(unsigned char)female);
    srand(saved_seed);
    return name;
}

int npc_display_name(struct record *npc)
{
    struct person *npc_data;
    struct faction *faction;

    if (npc->type != 8 && npc->type != 53) return 0;
    npc_data = &npc->data.person;
    if (npc_data->faction_id != 0) {
        faction = faction_find((short)npc_data->faction_id);
        if (faction->type == 4) return (int)faction->name;
    }
    if (npc->twin != 0) {
        return name_generate_seeded((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)((signed char)npc->flags & 4), npc->name_seed);
    }
    return name_generate_seeded((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], npc_data->flags & 16, npc->id);
}
