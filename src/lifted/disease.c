/* disease.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

#include "records.h"

extern char D_0012B508[];
extern char D_00175970[];
extern char D_0017597A[];
extern char D_0017599B[];
extern char D_001759C5[];
extern char D_001759EB[];
extern char D_001759F8[];
extern char D_0018320A[];
extern char monster_category[];
extern char D_00185083[];
extern struct disease D_00186A64[];
extern char D_00186D83[];
extern char D_00186D85[];
extern char D_00186D87[];
extern char D_00186D89[];
extern char lycanthrope_attributes[];
extern char bio_modifiers[];
extern char D_0018DDE0[];
extern struct record *D_00190504[];
extern char itemmaker_slot_kinds[];
extern char D_00190D63[];
extern char D_001940D8[];
extern struct record *player_entity;
extern struct record *D_00195AA8;
extern struct spell *spell_records;
extern char D_00195B08[];
extern char D_00195B0C[];
extern char creature_count[];
extern char calendar_month[];
extern char D_00195B44[];
extern char D_00195B84[];
extern char inpstr_result[];
extern struct character *player_character;
extern struct career *player_class;
extern char game_minutes[];
extern char D_00195F24[];
extern char D_00195F25[];
extern char D_001961F5[];
extern char D_0019626F[];
extern char D_00196271[];
extern char game_mode[];
extern char in_dungeon_water[];
extern char player_ailment_flags[];
extern char D_001962A0[];
extern char D_001A3AA4[];
extern char D_001A3AA8[];
extern char trade_haggle_asking[];

extern int damage_apply(struct record *, int, int);
extern int player_in_daylight(void);
extern int player_in_temple(void);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern int func_00068845(int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int spfx_resist_roll(int, int, struct character *, struct career *, int, int);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_find_item(struct record *, int, int);
extern int object_new_id(int);
extern int mc_strncpy();
extern int func_000A0DF4();
extern int mc_memcpy();
extern int func_000C7FD9();
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
void func_000686DA(int);

void disease_infect(struct record *a1, int a2, int a3, int a4)
{
    struct character *l_1C;
    struct career *l_18;
    int l_14;
    struct record *l_10;
    struct disease *l_C;

    l_1C = &a1->data.character;
    l_18 = &l_1C->career;
    if (l_1C != player_character) goto L65980;
    if ((player_class->immunity_flags & 64) != 0) goto L65982;
L65980:;
    goto L65987;
L65982:;
    return;
L65987:;
    if (a4 != 0) goto L659B3;
    if (spfx_resist_roll(2, 64, l_1C, l_18, 4, *(int *)bio_modifiers) == 100) return;
L659B3:;
    if (l_1C->level == 1) return;
    l_10 = object_create_child(a1, 0, 47);
    l_10->type = 11;
    l_10->flags = 32771;
    l_C = &l_10->data.disease;
    if (a2 == 0) goto L65A31;
    l_14 = 0;
L65A01:;
    if (((int)(unsigned char)*(signed char *)((char *)(l_14++ + a2))) != 255) goto L65A01;
    a3 = (int)(unsigned char)*(signed char *)((char *)(int)((char *)a2 + rand_range(0, l_14 - 2)));
L65A31:;
    mc_memcpy(l_C, &D_00186A64[a3], 47, (int)D_00175970, 83, 4);
    if (l_C->days_left == 255) goto L65A7C;
    l_C->days_left = rand_range(l_C->days_left, l_C->stage);
L65A7C:;
    l_C->stage = 0;
}

void func_00065A8C(struct record *a1, int a2, int a3)
{
    struct character *l_20;
    struct career *l_1C;
    int l_18;
    struct record *l_14;
    struct disease *l_10;

    l_20 = &a1->data.character;
    l_1C = &l_20->career;
    if (l_20 != player_character) goto L65AD3;
    if ((player_class->immunity_flags & 4) != 0) goto L65AD5;
L65AD3:;
    goto L65ADA;
L65AD5:;
    return;
L65ADA:;
    if (a3 != 0) goto L65B06;
    if (spfx_resist_roll(2, 4, l_20, l_1C, 4, *(int *)D_0018DDE0) == 100) return;
L65B06:;
    if (l_20->level == 1) return;
    l_14 = object_create_child(a1, 0, 47);
    l_14->type = 11;
    l_14->flags = 32771;
    l_10 = &l_14->data.disease;
    l_10->id = *(signed char *)&a2;
    a2 = (a2 << 2) - 512;
    l_10->days_left = rand_range((int)(short)*(short *)(D_00186D87 + (a2 * 2)), (int)(short)*(short *)(D_00186D89 + (a2 * 2)));
    l_10->stage = 0;
    l_10->damage_min = rand_range((int)(short)*(short *)(D_00186D83 + (a2 * 2)), (int)(short)*(short *)(D_00186D85 + (a2 * 2)));
}

void poison_init_record(struct disease *a1, int a2)
{
    a1->id = *(signed char *)&a2;
    a2 = (a2 << 2) - 512;
    a1->days_left = rand_range((int)(short)*(short *)(D_00186D87 + (a2 * 2)), (int)(short)*(short *)(D_00186D89 + (a2 * 2)));
    a1->damage_min = rand_range((int)(short)*(short *)(D_00186D83 + (a2 * 2)), (int)(short)*(short *)(D_00186D85 + (a2 * 2)));
}

int poison_tick(struct disease *a1)
{
    int l_28;
    int l_24;
    int l_20;
    struct character *l_1C;

    if (a1->id >= 128) goto L65C66;
    return 1;
L65C66:;
    if (a1->damage_min == 0) goto L65C83;
    a1->damage_min--;
    return 1;
L65C83:;
    if (a1->stage != 0) goto L65CCB;
    a1->stage = 1;
__dagger_tbl65C9B:;
L65CCB:;
    switch ((unsigned char)(a1->id - 128)) {
case 0:
    damage_apply(D_00195AA8, rand_range(2, 12), 0);
    goto L662AD;
case 1:
    damage_apply(D_00195AA8, 2, 0);
    a1->stat_flags[4] = 1;
    a1->drained[4]++;
    player_character->attributes[4]--;
    if (player_character->attributes[4] >= 1) goto L65D5D;
    player_character->attributes[4] = 1;
    a1->drained[4]--;
L65D5D:;
    a1->stage = 2;
    goto L662AD;
case 2:
    damage_apply(D_00195AA8, rand_range(1, 10), 0);
    goto L662AD;
case 3:
    l_28 = rand_range(5, 10);
    a1->stat_flags[0] = 1;
    a1->drained[0] += l_28;
    player_character->attributes[0] -= l_28;
    if (player_character->attributes[0] >= 1) goto L65DF0;
    player_character->attributes[0] = 1;
    a1->drained[0] -= 1 - player_character->attributes[0];
L65DF0:;
    l_28 = rand_range(1, 5);
    a1->stat_flags[3] = 1;
    a1->drained[3] += l_28;
    player_character->attributes[3] -= l_28;
    if (player_character->attributes[3] >= 1) goto L65E50;
    player_character->attributes[3] = 1;
    a1->drained[3] -= 1 - player_character->attributes[3];
L65E50:;
    l_28 = rand_range(1, 5);
    a1->stat_flags[6] = 1;
    a1->drained[6] += l_28;
    player_character->attributes[6] -= l_28;
    if (player_character->attributes[6] >= 1) goto L65EB0;
    player_character->attributes[6] = 1;
    a1->drained[6] -= 1 - player_character->attributes[6];
L65EB0:;
    a1->stage = 2;
    goto L662AD;
case 4:
    fatigue_add(-rand_range(10, 100));
    goto L662AD;
case 5:
    damage_apply(D_00195AA8, rand_range(1, 30), 0);
    goto L662AD;
case 6:
    l_28 = rand_range(1, 5);
    a1->stat_flags[2] = 1;
    a1->drained[2] += l_28;
    player_character->attributes[2] -= l_28;
    if (player_character->attributes[2] >= 1) goto L65F5E;
    player_character->attributes[2] = 1;
    a1->drained[2] -= 1 - player_character->attributes[2];
L65F5E:;
    l_1C = &D_00195AA8->data.character;
    l_1C->magicka -= rand_range(5, 15);
    if (l_1C->magicka >= 0) goto L65F9D;
    l_1C->magicka = 0;
L65F9D:;
    a1->stage = 2;
    goto L662AD;
case 7:
    l_28 = rand_range(5, 20);
    a1->stat_flags[2] = 1;
    a1->drained[2] += l_28;
    player_character->attributes[2] -= l_28;
    if (player_character->attributes[2] >= 1) goto L6600B;
    player_character->attributes[2] = 1;
    a1->drained[2] -= 1 - player_character->attributes[2];
L6600B:;
    l_28 = rand_range(10, 20);
    a1->stat_flags[5] = 1;
    a1->drained[5] += l_28;
    player_character->attributes[5] -= l_28;
    if (player_character->attributes[5] >= 1) goto L6606B;
    player_character->attributes[5] = 1;
    a1->drained[5] -= 1 - player_character->attributes[5];
L6606B:;
    a1->stage = 2;
    goto L662AD;
case 8:
    fatigue_add(-rand_range(10, 100));
    l_28 = rand_range(4, 10);
    a1->stat_flags[7] = 1;
    a1->drained[7] -= l_28;
    player_character->attributes[7] += l_28;
    a1->stage = 2;
    goto L662AD;
case 9:
    l_28 = rand_range(10, 30);
    a1->stat_flags[1] = 1;
    a1->drained[1] += l_28;
    player_character->attributes[1] -= l_28;
    if (player_character->attributes[1] >= 1) goto L6612F;
    player_character->attributes[1] = 1;
    a1->drained[1] -= 1 - player_character->attributes[1];
L6612F:;
    l_28 = rand_range(5, 20);
    a1->stat_flags[0] = 1;
    a1->drained[0] -= l_28;
    player_character->attributes[0] += l_28;
    a1->stage = 2;
    goto L662AD;
case 10:
    fatigue_add(rand_range(5, 10) << 6);
    l_28 = rand_range(1, 4);
    a1->stat_flags[2] = 1;
    a1->drained[2] += l_28;
    player_character->attributes[2] -= l_28;
    if (player_character->attributes[2] >= 1) goto L661E6;
    player_character->attributes[2] = 1;
    a1->drained[2] -= 1 - player_character->attributes[2];
L661E6:;
    a1->stage = 2;
    goto L662AD;
case 11:
    l_1C = &D_00195AA8->data.character;
    l_1C->magicka += rand_range(5, 10);
    if (l_1C->magicka <= l_1C->max_magicka) goto L66244;
    l_1C->magicka = l_1C->max_magicka;
L66244:;
    l_28 = rand_range(1, 5);
    a1->stat_flags[4] = 1;
    a1->drained[4] += l_28;
    player_character->attributes[4] -= l_28;
    if (player_character->attributes[4] >= 1) goto L662A4;
    player_character->attributes[4] = 1;
    a1->drained[4] -= 1 - player_character->attributes[4];
L662A4:;
    a1->stage = 2;
default:
L662AD:;
    *(signed char *)player_ailment_flags |= 1;
    hud_message_add(*(int *)D_00185083);
    if (a1->days_left == 0) goto L662D1;
    a1->days_left--;
    goto L662F7;
L662D1:;
    if (a1->stage != 2) goto L662EE;
    a1->days_left = 254;
    a1->id = 0;
    goto L662F7;
L662EE:;
    return 0;
L662F7:;
    return 1;
}
}

void func_00066853(struct record *a1, int a2)
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
L668AD:;
    if (spell_records[l_14].name[0] == 0) goto L668D7;
    if (spell_records[l_14].id == a2) goto L668DF;
L668D7:;
    l_14++;
    goto L668AD;
L668DF:;
    mc_memcpy(l_18, &spell_records[l_14], 89, (int)D_00175970, 540, 4);
    l_18->name[func_000A0DF4(l_18->name) + 1] = 36;
}

void disease_cure_vampirism(void)
{
    int l_2C;
    struct record *l_28;
    struct record *l_24;
    struct record *l_20;
    struct record *l_1C;
    struct disease *l_18;

    if (player_character->special_infection_time == 0) goto L66947;
    if (player_character->special_infection == 0) goto L66949;
L66947:;
    goto L66963;
L66949:;
    player_character->special_infection_time = 0;
    player_character->special_infection = 0;
L66963:;
    if (player_character->race != 8) return;
    player_character->flags &= ~0x4;
    l_1C = player_entity->children;
L6698D:;
    if (l_1C == 0) goto L669E9;
    if (l_1C->type != 28) goto L669A8;
    l_24 = l_1C;
L669A8:;
    if (l_1C->type != 11) goto L669DE;
    l_18 = &l_1C->data.disease;
    if (l_18->id != 100) goto L669DE;
    l_28 = l_1C;
    l_18 = &l_28->data.disease;
L669DE:;
    l_1C = l_1C->next;
    goto L6698D;
L669E9:;
    l_2C = 0;
L669F0:;
    if (l_2C < 8) goto L66A00;
    goto L66A3B;
L669F8:;
    l_2C++;
    goto L669F0;
L66A00:;
    player_character->attributes[l_2C] -= l_18->drained[l_2C];
    player_character->base_attributes[l_2C] -= l_18->drained[l_2C];
    goto L669F8;
L66A3B:;
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
L66B03:;
    if (l_1C == 0) goto L66B3D;
    l_20 = l_1C->next;
    if (l_1C->data.spell.name[func_000A0DF4(l_1C->data.spell.name) + 1] != 36) goto L66B35;
    object_delete(l_1C);
L66B35:;
    l_1C = l_20;
    goto L66B03;
L66B3D:;
    object_delete(l_24);
    object_delete(l_28);
    player_character->max_health = (short)player_character->max_health_base;
    *(signed char *)D_001940D8 |= 8;
    paperdoll_draw(0, 0);
}

void disease_remove_skill_bonuses(void)
{
    int l_18;

    inv_unequip_all_saved();
    if (player_character->race != 8) goto L66BED;
    player_character->skills[3].value -= 30;
    player_character->skills[21].value -= 30;
    player_character->skills[16].value -= 30;
    player_character->skills[34].value -= 30;
    player_character->skills[18].value -= 30;
    player_character->skills[30].value -= 30;
L66BED:;
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
    if (player_character->race != 8) goto L66CCE;
    player_character->skills[3].value += 30;
    player_character->skills[21].value += 30;
    player_character->skills[16].value += 30;
    player_character->skills[34].value += 30;
    player_character->skills[18].value += 30;
    player_character->skills[30].value += 30;
L66CCE:;
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

    if (player_character->level == 1) goto L66D6B;
    if (disease_is_lycanthrope() == 0) goto L66D6D;
L66D6B:;
    goto L66D7F;
L66D6D:;
    if (player_character->race <= 7) goto L66D84;
L66D7F:;
    return;
L66D84:;
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
    l_1C = 0;
L66E29:;
    if (l_1C < 4) goto L66E3C;
    goto L66ED4;
L66E34:;
    l_1C++;
    goto L66E29;
L66E3C:;
    l_24 = (int)(unsigned char)*(signed char *)(lycanthrope_attributes + l_1C);
    l_18->drained[l_24] = 40;
    player_character->attributes[l_24] += 40;
    player_character->base_attributes[l_24] += 40;
    l_20 = player_character->base_attributes[l_24] - 100;
    if (l_20 <= 0) goto L66ECF;
    player_character->base_attributes[l_24] -= l_20;
    player_character->attributes[l_24] -= l_20;
    l_18->drained[l_24] -= l_20;
L66ECF:;
    goto L66E34;
L66ED4:;
    player_class->immunity_flags |= 64;
    l_30 = object_create_child(l_34, 0, 89);
    l_30->type = 9;
    l_30->flags = 3;
    l_30->id = object_new_id(l_34->id >> 16);
    l_28 = &l_30->data.spell;
    l_1C = 0;
L66F24:;
    if (spell_records[l_1C].name[0] == 0) goto L66F4E;
    if (spell_records[l_1C].id == 92) goto L66F56;
L66F4E:;
    l_1C++;
    goto L66F24;
L66F56:;
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
    player_character->lycanthrope_kill_time = *(int *)game_minutes;
}
}

void disease_lycanthrope_shapechange(int a1)
{
    struct record *l_1C;
    struct disease *l_18;

    l_18 = 0;
    if (a1 != 0) goto L67057;
    if (player_character->race < 8) goto L67059;
L67057:;
    goto L67071;
L67059:;
    if (((unsigned)(*(int *)game_minutes - player_character->last_shapechange_time)) < 1200) goto L67073;
L67071:;
    goto L67095;
L67073:;
    if (item_artifact_equipped(3) != 0) goto L67095;
    msgbox_show_string((int)D_0017597A, 1);
    return;
L67095:;
    *(signed char *)D_001940D8 |= 8;
    player_character->last_shapechange_time = *(int *)game_minutes;
    l_1C = player_entity->children;
L670B8:;
    if (l_1C == 0) goto L67108;
    if (l_1C->type != 11) goto L670FD;
    l_18 = &l_1C->data.disease;
    if (l_18->id == 101) goto L670F4;
    if (l_18->id != 102) goto L670F6;
L670F4:;
    goto L67108;
L670F6:;
    l_18 = 0;
L670FD:;
    l_1C = l_1C->next;
    goto L670B8;
L67108:;
    if (l_18 == 0) return;
    if (player_character->race >= 8) goto L671E0;
    if (l_18->id != 101) goto L67142;
    player_character->race = 9;
    goto L6714B;
L67142:;
    player_character->race = 10;
L6714B:;
    player_character->reputation[0] -= 100;
    player_character->reputation[1] -= 100;
    player_character->reputation[2] -= 100;
    player_character->reputation[3] -= 100;
    player_character->reputation[4] -= 100;
    player_character->min_metal_to_hit = 2;
    player_character->health = player_character->max_health;
    *(signed char *)D_001940D8 |= 8;
    paperdoll_draw(0, 0);
    player_character->equipped[19] = 0;
    player_character->equipped[21] = 0;
    weapon_reload_hand_sprites();
    return;
L671E0:;
    player_character->race = player_character->original_race;
    player_character->reputation[0] += 100;
    player_character->reputation[1] += 100;
    player_character->reputation[2] += 100;
    player_character->reputation[3] += 100;
    player_character->reputation[4] += 100;
    player_character->min_metal_to_hit = 0;
    player_character->health = player_character->max_health;
    *(signed char *)D_001940D8 |= 8;
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
L67290:;
    if (l_20 == 0) goto L672E0;
    if (l_20->type != 11) goto L672D5;
    l_1C = &l_20->data.disease;
    if (l_1C->id == 101) goto L672CC;
    if (l_1C->id != 102) goto L672CE;
L672CC:;
    goto L672E0;
L672CE:;
    l_1C = 0;
L672D5:;
    l_20 = l_20->next;
    goto L67290;
L672E0:;
    if (l_1C == 0) goto L672EF;
    l_24 = 1;
    goto L672F6;
L672EF:;
    l_24 = 0;
L672F6:;
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

    if (player_character->special_infection_time == 0) goto L67331;
    if (player_character->special_infection != 0) goto L67333;
L67331:;
    goto L6734D;
L67333:;
    player_character->special_infection_time = 0;
    player_character->special_infection = 0;
L6734D:;
    if (disease_is_lycanthrope() == 0) return;
    player_character->flags &= ~0x4;
    l_20 = player_entity->children;
L6736E:;
    if (l_20 == 0) goto L673D9;
    if (l_20->type != 28) goto L67389;
    l_28 = l_20;
L67389:;
    if (l_20->type != 11) goto L673CE;
    l_18 = &l_20->data.disease;
    if (l_18->id == 101) goto L673BF;
    if (l_18->id != 102) goto L673CE;
L673BF:;
    l_2C = l_20;
    l_1C = &l_2C->data.disease;
L673CE:;
    l_20 = l_20->next;
    goto L6736E;
L673D9:;
    l_30 = 0;
L673E0:;
    if (l_30 < 8) goto L673F0;
    goto L6742B;
L673E8:;
    l_30++;
    goto L673E0;
L673F0:;
    player_character->attributes[l_30] -= l_1C->drained[l_30];
    player_character->base_attributes[l_30] -= l_1C->drained[l_30];
    goto L673E8;
L6742B:;
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
L674F1:;
    if (l_20 == 0) goto L67523;
    l_24 = l_20->next;
    if (l_20->data.spell.id != 92) goto L6751B;
    object_delete(l_20);
L6751B:;
    l_20 = l_24;
    goto L674F1;
L67523:;
    *(signed char *)D_001940D8 |= 8;
    paperdoll_draw(0, 0);
    player_character->max_health = (short)player_character->max_health_base;
}

void disease_lycanthrope_tick(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = (((unsigned)*(int *)game_minutes) / 1440) & 31;
    l_1C = (((unsigned)(*(int *)game_minutes + 5760)) / 1440) & 31;
    if (disease_is_lycanthrope() == 0) return;
    if (item_artifact_equipped(3) != 0) return;
    if (((unsigned)(*(int *)game_minutes - player_character->lycanthrope_kill_time)) <= 20160) goto L67668;
    l_18 = (*(int *)game_minutes - player_character->lycanthrope_kill_time) - 20160;
    if (l_18 <= 20160) goto L675EB;
    l_18 = 4;
    goto L67627;
L675EB:;
    l_18 = (((l_18 << 8) / 20160) * player_character->max_health) / 256;
    if (l_18 >= 4) goto L67627;
    l_18 = 4;
L67627:;
    if (player_character->max_health == l_18) goto L6763F;
    hud_message_add((int)D_0017599B);
L6763F:;
    player_character->max_health = l_18;
    if (player_character->health <= l_18) goto L67668;
    player_character->health = l_18;
L67668:;
    if (player_character->race > 8) return;
    if (l_20 == 0) goto L67686;
    if (l_1C != 0) return;
L67686:;
    hud_message_add((int)D_001759C5);
    disease_lycanthrope_shapechange(1);
}

void disease_special_infection_tick(void)
{
    if (player_character->special_infection_time == 0) return;
    if (((unsigned)*(int *)game_minutes) <= player_character->special_infection_time) return;
    player_character->special_infection_time = 0;
    if (player_character->special_infection == 0) goto L676FF;
    disease_become_lycanthrope(player_character->special_infection - 1);
    return;
L676FF:;
    disease_become_vampire();
}

void disease_start_cure_quest(int a1)
{
    if (player_character->race != 8) goto L67826;
    if (a1 == 0) goto L6774F;
    if (rand_range(10, 100) < 30) goto L67751;
L6774F:;
    goto L67774;
L67751:;
    mc_strncpy((int)D_001961F5, (int)D_001759EB, 13, (int)D_00175970, 952);
    return;
L67774:;
    if (a1 != 0) goto L67788;
    if (player_character->action != 0) goto L6778A;
L67788:;
    goto L6779E;
L6778A:;
    if (rand_range(1, 100) < 50) goto L677A0;
L6779E:;
    goto L677CC;
L677A0:;
    quest_pick_file(80, 0, 48, 66, player_character->level);
    return;
L677CC:;
    if (a1 != 0) goto L677E6;
    if (rand_range(1, 100) < 50) goto L677E8;
L677E6:;
    goto L67824;
L677E8:;
    quest_pick_file(80, 0, 48, 65, player_character->level);
    if (*(signed char *)D_001961F5 == 0) goto L67824;
    player_character->action = 1;
L67824:;
    return;
L67826:;
    if (disease_is_lycanthrope() == 0) goto L67835;
    if (a1 != 0) goto L67837;
L67835:;
    goto L6784B;
L67837:;
    if (rand_range(1, 100) < 30) goto L6784D;
L6784B:;
    return;
L6784D:;
    mc_strncpy((int)D_001961F5, (int)D_001759F8, 13, (int)D_00175970, 971);
}

void func_00067875(struct record *a1)
{
    struct item *l_1C;
    int l_18;

    if (a1->type != 2) return;
    l_1C = &a1->data.item;
    if (l_1C->enchantments[0].type == (-1)) return;
    l_18 = 0;
L678B9:;
    if (l_18 < 10) goto L678CC;
    return;
L678C4:;
    l_18++;
    goto L678B9;
L678CC:;
    if (l_1C->enchantments[l_18].type == (-1)) return;
    if (l_1C->enchantments[l_18].type != 14) goto L67906;
    if (l_1C->enchantments[l_18].param == 5) goto L67908;
L67906:;
    goto L67914;
L67908:;
    *(int *)D_00195B84 += 10;
    goto L679AC;
L67914:;
    if (l_1C->enchantments[l_18].type != 25) goto L67938;
    if (l_1C->enchantments[l_18].param == 5) goto L6793A;
L67938:;
    goto L67946;
L6793A:;
    *(int *)D_00195B84 -= 10;
    goto L679AC;
L67946:;
    if (l_1C->enchantments[l_18].type != 14) goto L6796F;
    if ((short)*(signed char *)itemmaker_slot_kinds == l_1C->enchantments[l_18].param) goto L67971;
L6796F:;
    goto L6797A;
L67971:;
    *(int *)D_00195B84 += 10;
    goto L679AC;
L6797A:;
    if (l_1C->enchantments[l_18].type != 25) goto L679A3;
    if ((short)*(signed char *)itemmaker_slot_kinds == l_1C->enchantments[l_18].param) goto L679A5;
L679A3:;
    goto L679AC;
L679A5:;
    *(int *)D_00195B84 -= 10;
L679AC:;
    goto L678C4;
}

void enchant_extra_spell_points(int a1, int a2)
{
    *(int *)D_001A3AA4 += enchant_spell_points_condition(a2);
}

int enchant_spell_points_condition(int a1)
{
    struct character *l_24;
    int l_20;
    int l_1C;

    switch ((unsigned)a1) {
    goto L67BF0;
case 0:
case 1:
case 2:
case 3:
    if (((int)(unsigned char)*(signed char *)(D_0018320A + *(int *)calendar_month)) != a1) goto L67AB9;
    return 75;
L67AB9:;
    goto L67BF0;
case 4:
    if (*(signed char *)D_00195F24 == 0) goto L67AD0;
    if (*(signed char *)D_00195F25 != 0) goto L67ADC;
L67AD0:;
    return 75;
L67ADC:;
    goto L67BF0;
case 5:
    if (((int)(unsigned char)*(signed char *)D_00195F24) == 8) goto L67AF9;
    if (((int)(unsigned char)*(signed char *)D_00195F25) != 8) goto L67B05;
L67AF9:;
    return 75;
L67B05:;
    if (((int)(unsigned char)*(signed char *)D_00195F24) == 24) goto L67B1D;
    if (((int)(unsigned char)*(signed char *)D_00195F25) != 24) goto L67B29;
L67B1D:;
    return 75;
L67B29:;
    goto L67BF0;
case 6:
    a1 += -4;
    if (((int)(unsigned char)*(signed char *)D_00195F24) == 16) goto L67B4A;
    if (((int)(unsigned char)*(signed char *)D_00195F25) != 16) goto L67B56;
L67B4A:;
    return 75;
L67B56:;
    goto L67BF0;
case 7:
case 8:
case 9:
case 10:
    a1 += -7;
    l_1C = 0;
L67B66:;
    if (l_1C < *(int *)creature_count) goto L67B7E;
    goto L67BF0;
L67B76:;
    l_1C++;
    goto L67B66;
L67B7E:;
    if (func_000C7FD9(player_entity->x, player_entity->z, D_00190504[l_1C]->x, D_00190504[l_1C]->z) >= 1024) goto L67BEE;
    l_24 = &D_00190504[l_1C]->data.character;
    if (((int)(unsigned char)*(signed char *)(monster_category + l_24->race)) != a1) goto L67BEE;
    return 75;
L67BEE:;
    goto L67B76;
default:
L67BF0:;
    return 0;
}
}

int item_artifact_equipped(int a1)
{
    int l_20;
    struct item *l_1C;

    l_20 = 0;
L67C1C:;
    if (l_20 < 27) goto L67C2F;
    goto L67C90;
L67C27:;
    l_20++;
    goto L67C1C;
L67C2F:;
    if (player_character->equipped[l_20] == 0) goto L67C27;
    l_1C = &player_character->equipped[l_20]->data.item;
    if (l_1C->enchantments[0].type == (-1)) goto L67C27;
    if (l_1C->enchantments[0].type != 26) goto L67C83;
    if (l_1C->enchantments[0].param == a1) goto L67C85;
L67C83:;
    goto L67C8E;
L67C85:;
    return 1;
L67C8E:;
    goto L67C27;
L67C90:;
    return 0;
}

void func_00067CA4(int a1, int a2)
{
    int l_18;
    struct item *l_14;

    l_18 = 0;
L67CBE:;
    if (l_18 < 27) goto L67CD1;
    return;
L67CC9:;
    l_18++;
    goto L67CBE;
L67CD1:;
    if (player_character->equipped[l_18] == 0) goto L67CC9;
    l_14 = &player_character->equipped[l_18]->data.item;
    if (l_14->enchantments[0].type == (-1)) goto L67CC9;
    if (l_14->enchantments[0].type != 26) goto L67D25;
    if (l_14->enchantments[0].param == a1) goto L67D27;
L67D25:;
    goto L67D44;
L67D27:;
    item_damage(player_character->equipped[l_18], a2);
    return;
L67D44:;
    goto L67CC9;
}

void effects_tick(void)
{
    int l_24;
    int l_20;
    int l_1C;
    struct item *l_18;

    *(int *)D_001A3AA8 = 0;
    l_24 = (*(int *)D_00195B08 = *(int *)game_minutes - *(int *)D_00195B44);
    if (*(int *)D_00195B08 <= 100) goto L67DA4;
    l_24 = (*(int *)D_00195B08 = 0);
    *(int *)D_00195B44 = *(int *)game_minutes;
L67DA4:;
    *(int *)D_00195B08 >>= 2;
    if (*(int *)D_00195B08 == 0) goto L67DBE;
    *(int *)D_00195B44 = *(int *)game_minutes;
L67DBE:;
    D_00195AA8 = 0;
    if (*(int *)D_00195B08 != 0) goto L67DDB;
    if (l_24 == 0) goto L67F29;
L67DDB:;
    if (player_class->regeneration_flags == 0) goto L67DEF;
    if (*(int *)D_00195B08 != 0) goto L67DF4;
L67DEF:;
    goto L67EAA;
L67DF4:;
    if (((int)(unsigned char)(player_class->regeneration_flags & 4)) == 0) goto L67E1B;
    if (*(signed char *)in_dungeon_water != 0) goto L67E19;
    if (*(signed char *)D_001962A0 == 0) goto L67E1B;
L67E19:;
    goto L67E1D;
L67E1B:;
    goto L67E2B;
L67E1D:;
    item_enchantment_tick(0, 5, 0);
L67E2B:;
    if (((int)(unsigned char)(player_class->regeneration_flags & 8)) == 0) goto L67E4C;
    item_enchantment_tick(0, 5, 0);
L67E4C:;
    if (((int)(unsigned char)(player_class->regeneration_flags & 1)) == 0) goto L67E68;
    if (player_in_daylight() != 0) goto L67E6A;
L67E68:;
    goto L67E7B;
L67E6A:;
    item_enchantment_tick(0, 5, 1);
L67E7B:;
    if (((int)(unsigned char)(player_class->regeneration_flags & 2)) == 0) goto L67E97;
    if (player_in_daylight() == 0) goto L67E99;
L67E97:;
    goto L67EAA;
L67E99:;
    item_enchantment_tick(0, 5, 2);
L67EAA:;
    l_20 = *(int *)D_00195B08;
    *(int *)D_00195B08 = *(int *)D_00195B08 * 12;
    if (((int)(unsigned short)(player_class->flags & 16)) == 0) goto L67EDE;
    if (player_in_daylight() != 0) goto L67EE0;
L67EDE:;
    goto L67EEE;
L67EE0:;
    item_enchantment_tick(0, 17, 0);
L67EEE:;
    if (((int)(unsigned short)(player_class->flags & 32)) == 0) goto L67F0E;
    if (player_in_temple() != 0) goto L67F10;
L67F0E:;
    goto L67F21;
L67F10:;
    item_enchantment_tick(0, 17, 1);
L67F21:;
    *(int *)D_00195B08 = l_20;
L67F29:;
    l_24 = 0;
L67F30:;
    if (l_24 < 27) goto L67F43;
    goto L67FEB;
L67F3B:;
    l_24++;
    goto L67F30;
L67F43:;
    if (player_character->equipped[l_24] == 0) goto L67FE6;
    l_18 = &player_character->equipped[l_24]->data.item;
    if (l_18->enchantments[0].type == (-1)) goto L67F3B;
    D_00195AA8 = player_character->equipped[l_24];
    l_20 = 0;
L67FA2:;
    if (l_20 >= 10) goto L67FBA;
    if (l_18->enchantments[l_20].type != (-1)) goto L67FBC;
L67FBA:;
    goto L67FE6;
L67FBC:;
    item_enchantment_tick(l_18, l_18->enchantments[l_20].type, l_18->enchantments[l_20].param);
    l_20++;
    goto L67FA2;
L67FE6:;
    goto L67F3B;
L67FEB:;
    if (((int)(unsigned short)(player_class->flags & 128)) == 0) goto L6800B;
    if (player_in_daylight() == 0) goto L6800D;
L6800B:;
    goto L68026;
L6800D:;
    *(int *)D_001A3AA8 = -(player_character->max_magicka >> 1);
L68026:;
    if (((int)(unsigned short)(player_class->flags & 64)) == 0) goto L68046;
    if (player_in_daylight() == 0) goto L68048;
L68046:;
    goto L6805F;
L68048:;
    *(int *)D_001A3AA8 = -player_character->max_magicka;
L6805F:;
    if (((int)(unsigned short)(player_class->flags & 512)) == 0) goto L6807F;
    if (player_in_daylight() != 0) goto L68081;
L6807F:;
    goto L6809A;
L68081:;
    *(int *)D_001A3AA8 = -(player_character->max_magicka >> 1);
L6809A:;
    if (((int)(unsigned short)(player_class->flags & 256)) == 0) goto L680BA;
    if (player_in_daylight() != 0) goto L680BC;
L680BA:;
    return;
L680BC:;
    *(int *)D_001A3AA8 = -player_character->max_magicka;
}

void func_000685EC(void)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)game_mode) == 13) goto L68612;
    if (((int)(unsigned char)*(signed char *)D_0019626F) != 13) goto L68614;
L68612:;
    goto L68619;
L68614:;
    return;
L68619:;
    *(signed char *)D_0012B508 = 146;
    if (*(signed char *)D_00190D63 != 0) goto L6868C;
    if (*(signed char *)D_00196271 == 0) return;
    if (((int)(unsigned char)*(signed char *)D_00196271) != 1) goto L6865D;
    func_000686DA((int)*(double *)trade_haggle_asking);
    return;
L6865D:;
    if (((int)(unsigned char)*(signed char *)D_00196271) != 2) goto L68672;
    func_000686DA(0);
    return;
L68672:;
    if (((int)(unsigned char)*(signed char *)D_00196271) != 3) goto L6868A;
    *(signed char *)D_00190D63 = 1;
    trade_counter_offer();
L6868A:;
    return;
L6868C:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 13) return;
    l_18 = func_00068845(*(int *)inpstr_result);
    if (l_18 != (-1)) goto L686B4;
    func_000686DA(0);
    return;
L686B4:;
    if (l_18 == 0) goto L686C4;
    func_000686DA(l_18);
    return;
L686C4:;
    *(signed char *)D_00190D63 = 0;
    trade_haggle_show_offer();
}

void func_000686DA(int a1)
{
    if (a1 <= 0) goto L68702;
    if (((unsigned)a1) > player_character->gold) goto L68704;
L68702:;
    goto L6871A;
L68704:;
    msgbox_show_rsc(454, 1);
    a1 = 0;
L6871A:;
    *(int *)D_00195B0C = a1;
    mode_pop();
}
