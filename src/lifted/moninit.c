/* moninit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00176844[];
extern char monster_table_flags[];
extern char monster_spells_imp[];
extern char monster_spells_ghost[];
extern char monster_spells_orc_shaman[];
extern char monster_spells_wraith[];
extern char monster_spells_frost_daedra[];
extern char monster_spells_fire_daedra[];
extern char monster_spells_daedroth[];
extern char monster_spells_vampire[];
extern char monster_spells_seducer[];
extern char monster_spells_vampire_ancient[];
extern char monster_spells_daedra_lord[];
extern char monster_spells_lich[];
extern char monster_spells_ancient_lich[];
extern char monster_class_spell_lists[];
extern char D_001879CE[];
extern char wabbajack_creatures[];
extern char monster_map_chance[];
extern char D_00190704[];
extern struct record *nonworld_root;
extern struct record *D_00195AC4;
extern struct spell *spell_records;
extern struct character *player_character;
extern char save_file_handle[];

extern struct record *monster_make_item(struct record *, unsigned short, int, int, int, int);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int func_000A00CB();
extern int mc_memcpy();
extern void character_update_armor_values(struct record *);
extern void item_make(int, int, struct item *);
extern void item_damage(struct record *, int);
extern void poison_init_record(struct disease *, int);
extern void monster_init(struct record *, int);
extern void monster_reload_anim_cb(struct record *);
extern void object_foreach(struct record *, int);
void monster_give_spells(struct record *, int);
void monster_give_equipment(struct record *, struct character *, int);
void monster_poison_weapon(struct record *);

void monster_reload_anims(void)
{
    int l_18;

    l_18 = 0;
L78C8E:;
    if (l_18 < 128) goto L78CA1;
    goto L78D00;
L78C99:;
    l_18++;
    goto L78C8E;
L78CA1:;
    if (*(int *)(D_00190704 + (l_18 << 2)) == 0) goto L78CFE;
    if (*(int *)(D_00190704 + (l_18 << 2)) == 0) goto L78CD1;
    if (*(int *)(D_00190704 + (l_18 << 2)) != (-1751672937)) goto L78CD3;
L78CD1:;
    goto L78CFE;
L78CD3:;
    mc_free(*(int *)(D_00190704 + (l_18 << 2)), (int)D_00176844, 211);
    *(int *)(D_00190704 + (l_18 << 2)) = -1751672937;
L78CFE:;
    goto L78C99;
L78D00:;
    object_foreach(D_00195AC4, (int)monster_reload_anim_cb);
    object_foreach(nonworld_root, (int)monster_reload_anim_cb);
}

int monster_roll_d8_health(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = l_1C;
L78D48:;
    if (l_1C < a1) goto L78D5A;
    goto L78D6E;
L78D52:;
    l_1C++;
    goto L78D48;
L78D5A:;
    l_18 += rand_range(1, 8);
    goto L78D52;
L78D6E:;
    return l_18 + a2;
}

int monster_roll_class_health(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_18 = 0;
    l_14 = l_18;
L78DA5:;
    if (l_18 < a3) goto L78DB7;
    goto L78DC9;
L78DAF:;
    l_18++;
    goto L78DA5;
L78DB7:;
    l_14 += rand_range(1, a1);
    goto L78DAF;
L78DC9:;
    return l_14 + a2;
}

void monster_init_gear(struct record *a1)
{
    struct character *l_1C;
    int l_18;

    l_1C = &a1->data.character;
    if (l_1C->race < 43) goto L78E87;
    monster_give_equipment(a1, l_1C, rand() & 1);
    if (((int)(unsigned short)(*(short *)(monster_table_flags + (l_1C->race * 29)) & 2)) == 0) goto L78E82;
    l_18 = l_1C->level / 3;
    if (l_18 <= 6) goto L78E6E;
    l_18 = 6;
L78E6E:;
    monster_give_spells(a1, *(int *)(monster_class_spell_lists + (l_18 << 2)));
L78E82:;
    return;
L78E87:;
    switch (l_1C->race) {
    return;
case 1:
    monster_give_spells(a1, (int)monster_spells_imp);
    return;
case 7:
    monster_give_equipment(a1, l_1C, 0);
    return;
case 8:
    monster_give_equipment(a1, l_1C, 1);
    return;
case 12:
    monster_give_equipment(a1, l_1C, 1);
    return;
case 18:
    monster_give_spells(a1, (int)monster_spells_ghost);
    return;
case 21:
    monster_give_equipment(a1, l_1C, 0);
    monster_give_spells(a1, (int)monster_spells_orc_shaman);
    return;
case 23:
    monster_give_spells(a1, (int)monster_spells_wraith);
    return;
case 24:
    monster_give_equipment(a1, l_1C, 2);
    return;
case 25:
    monster_give_spells(a1, (int)monster_spells_frost_daedra);
    return;
case 26:
    monster_give_spells(a1, (int)monster_spells_fire_daedra);
    return;
case 27:
    monster_give_spells(a1, (int)monster_spells_daedroth);
    return;
case 28:
    monster_give_spells(a1, (int)monster_spells_vampire);
    return;
case 29:
    monster_give_spells(a1, (int)monster_spells_seducer);
    return;
case 30:
    monster_give_spells(a1, (int)monster_spells_vampire_ancient);
    return;
case 31:
    monster_give_spells(a1, (int)monster_spells_daedra_lord);
    return;
case 32:
    monster_give_spells(a1, (int)monster_spells_lich);
    return;
case 33:
    monster_give_spells(a1, (int)monster_spells_ancient_lich);
default:;
}
}

void monster_give_spells(struct record *a1, int a2)
{
    int l_24;
    int l_20;
    struct record *l_1C;
    struct record *l_18;
    struct character *l_14;

    l_24 = 0;
    l_14 = &a1->data.character;
    l_14->magicka = (l_14->max_magicka = (((unsigned short)l_14->level) * 10) + 100);
    l_14->skills[22].value = 80;
    l_14->skills[23].value = 80;
    l_14->skills[24].value = 80;
    l_14->skills[25].value = 80;
    l_14->skills[26].value = 80;
    l_14->skills[27].value = 80;
    l_1C = object_create_child(a1, 0, 0);
    l_1C->type = 22;
    l_1C->flags = 3;
    l_1C->id = object_new_id(((unsigned)a1->id) >> 16);
L7916D:;
    if (((int)(unsigned char)*(signed char *)((char *)(a2 + l_24))) == 255) return;
    l_20 = 0;
L7918C:;
    if ((signed char)spell_records[l_20].id == *(signed char *)((char *)(a2 + l_24))) goto L791AD;
    l_20++;
    goto L7918C;
L791AD:;
    l_18 = object_create_child(l_1C, 0, 89);
    l_18->type = 9;
    l_18->flags = 1;
    l_18->id = object_new_id(((unsigned)a1->id) >> 16);
    mc_memcpy(&l_18->data.spell, &spell_records[l_20], 89, (int)D_00176844, 370, 4);
    l_24++;
    goto L7916D;
}

void monster_give_equipment(struct record *a1, struct character *a2, int a3)
{
    int l_14;
    int l_10;

    mc_memset(a2->equipped, 0, 108, (int)D_00176844, 400, 108);
    switch ((unsigned)a3) {
case 0:
    a2->equipped[19] = monster_make_item(a1, 3, 5, 7, -1, 100);
    a2->equipped[21] = monster_make_item(a1, 2, 7, 8, -1, 50);
    if (a2->equipped[21] != 0) goto L7939A;
    a2->equipped[21] = monster_make_item(a1, 3, 0, 3, 2, 50);
L7939A:;
    a2->equipped[12] = monster_make_item(a1, 2, 5, 5, -1, 50);
    a2->equipped[13] = monster_make_item(a1, 2, 4, 4, -1, 50);
    a2->equipped[15] = monster_make_item(a1, 2, 3, 3, -1, 50);
    a2->equipped[18] = monster_make_item(a1, 2, 0, 0, -1, 50);
    a2->equipped[23] = monster_make_item(a1, 2, 2, 2, -1, 50);
    a2->equipped[26] = monster_make_item(a1, 2, 6, 6, -1, 50);
    character_update_armor_values(a1);
    goto L7967C;
case 1:
    a2->equipped[19] = monster_make_item(a1, 3, 9, 14, -1, 100);
    a2->equipped[12] = monster_make_item(a1, 2, 5, 5, -1, 75);
    a2->equipped[13] = monster_make_item(a1, 2, 4, 4, -1, 75);
    a2->equipped[15] = monster_make_item(a1, 2, 3, 3, -1, 75);
    a2->equipped[18] = monster_make_item(a1, 2, 0, 0, -1, 75);
    a2->equipped[23] = monster_make_item(a1, 2, 2, 2, -1, 75);
    a2->equipped[26] = monster_make_item(a1, 2, 6, 6, -1, 75);
    character_update_armor_values(a1);
    goto L7967C;
case 2:
    a2->equipped[19] = monster_make_item(a1, 3, 9, 14, -1, 100);
    a2->equipped[12] = monster_make_item(a1, 2, 5, 5, -1, 90);
    a2->equipped[13] = monster_make_item(a1, 2, 4, 4, -1, 90);
    a2->equipped[15] = monster_make_item(a1, 2, 3, 3, -1, 90);
    a2->equipped[18] = monster_make_item(a1, 2, 0, 0, -1, 90);
    a2->equipped[23] = monster_make_item(a1, 2, 2, 2, -1, 90);
    a2->equipped[26] = monster_make_item(a1, 2, 6, 6, -1, 90);
    character_update_armor_values(a1);
default:
L7967C:;
    l_10 = 0;
L79683:;
    if (l_10 < 7) goto L79693;
    goto L796AE;
L7968B:;
    l_10++;
    goto L79683;
L79693:;
    if (a2->armor_values[l_10] <= 50) goto L796AC;
    a2->armor_values[l_10] = 60;
L796AC:;
    goto L7968B;
L796AE:;
    if (player_character->level < 2) return;
    if (a2->mobile_id < 128) goto L796E8;
    if (a2->equipped[19] != 0) goto L796EA;
L796E8:;
    goto L79733;
L796EA:;
    if (a2->mobile_id != 139) goto L79708;
    l_14 = 60;
    goto L7970F;
L79708:;
    l_14 = 5;
L7970F:;
    if (rand_range(1, 100) >= l_14) goto L79731;
    monster_poison_weapon(a2->equipped[19]);
L79731:;
    return;
L79733:;
}
    switch (a2->mobile_id) {
case 7:
case 8:
case 12:
    if (rand_range(1, 100) >= 5) return;
    monster_poison_weapon(a2->equipped[19]);
default:;
}
}

void monster_poison_weapon(struct record *a1)
{
    a1 = object_create_child(a1, 0, 47);
    a1->id = object_new_id(((unsigned)a1->parent->id) >> 16);
    poison_init_record(&a1->data.disease, (int)&*(signed char *)((char *)rand_range(0, 7) + 128));
}

void monster_wabbajack(struct record *a1, struct record *a2)
{
    int l_18;
    struct character *l_14;

    l_14 = &a2->data.character;
    if (((int)(unsigned short)(l_14->flags & 4096)) != 0) return;
    l_18 = l_14->max_health;
    monster_init(a2, (int)(unsigned char)*(signed char *)(wabbajack_creatures + rand_range(0, 16)));
    l_14->flags |= 0x1000;
    l_18 = l_14->max_health - l_18;
    if (l_18 >= 0) return;
    item_damage(a1, l_18);
}

void monster_maybe_give_map(struct record *a1, int a2)
{
    struct record *l_18;
    struct item *l_14;

    if (a2 >= 128) goto L798A3;
    if (rand_range(1, 100) > ((int)(unsigned char)*(signed char *)(monster_map_chance + a2))) return;
    goto L798C1;
L798A3:;
    if (rand_range(1, 100) > ((int)(unsigned char)*(signed char *)(D_001879CE + a2))) return;
L798C1:;
    l_18 = object_create_child(a1, 0, 107);
    l_18->type = 2;
    l_18->id = object_new_id(((unsigned)a1->id) >> 16);
    l_14 = &l_18->data.item;
    item_make(27, 8, l_14);
}

int func_00079A28(int a1)
{
    int l_1C;

    func_000A00CB(*(int *)save_file_handle, (int)&l_1C, 4);
    func_000A00CB(*(int *)save_file_handle, a1, l_1C);
    return l_1C;
}
