/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char D_000CEA30[];
extern char D_000CEA34[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_00153408[];
extern char D_0015340D[];
extern char D_00153411[];
extern char D_001757F4[];
extern char D_001757FF[];
extern char D_00175820[];
extern char D_00175837[];
extern char D_00175858[];
extern char D_0017586C[];
extern char D_00175881[];
extern char cast_anim_state[];
extern char spell_effect_school[];
extern char spell_effect_settings[];
extern char D_0018509B[];
extern char spell_last_cast_id[];
extern char spell_missile_textures[];
extern char spell_cast_sounds[];
extern char spell_impact_sounds[];
extern char magic_school_skills[];
extern char spell_element_class_bits[];
extern char D_00185CEC[];
extern struct record *D_00190504[];
extern char view_look_pitch[];
extern char D_001959BC[];
extern char D_001959FC[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char cheat_flags[];
extern struct spell *spell_records;
extern char creature_count[];
extern struct record *spell_ready_missile;
extern struct record *spell_ready_touch;
extern struct record *guild_npc_object;
extern char hud_bar_image[];
extern struct character *player_character;
extern struct settings *game_settings;
extern struct record *D_00195C48;
extern char spell_cast_anim_fire[];
extern char D_00195F28[];
extern char spell_effect_slot[];
extern char D_00195F62[];
extern char D_00196291[];
extern char D_00196292[];
extern struct spell *D_00199D64;
extern char D_00199D6C[];
extern char D_00199D71[];

extern int collide_line_of_sight(struct record *, struct record *);
extern int func_00023EC2(struct record *, int, int);
extern int spell_cost(struct spell *, struct character *);
extern int player_in_daylight(void);
extern int cast_player_spell(struct record *);
extern int cast_item_spell_at(struct record *, struct record *);
extern int cast_creature_spell_at(struct record *, struct record *, struct record *);
extern int spell_missile_update(struct record *, int);
extern int sound_play(int, struct record *, int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int spfx_resist_roll(int, int, struct character *, struct career *, int, int);
extern int spell_extend_duration(struct record *, struct spell *, int);
extern int object_delete(struct record *);
extern struct record *object_clone(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *object_find_item(struct record *, int, int);
extern int object_new_id(int);
extern int mc_memset();
extern int mc_strncpy();
extern int mc_memcpy();
extern int func_000C2000();
extern int func_000C2043();
extern int func_000C7FD9();
extern int func_000C7FF4();
extern int func_000C808D();
extern int spell_effect_dispatch();
extern int func_000CD20E();
extern int func_000CDB7A();
extern int spell_find_effect_type();
extern int func_000CE4E0();
extern int func_000CE70D();
extern void damage_spawn_splash(struct record *, int, int);
extern void damage_knockback(struct record *, int, int, int);
extern void spell_add_skill_uses(struct spell *, int);
extern void func_0007D774(struct record *, struct record *);
extern void spell_end(struct record *);
extern void object_foreach(struct record *, int);
int spell_resist_check(struct record *, struct record **);
struct spell *spell_find_active_effect(struct record *, int, int, int);
int spell_apply_effect(struct record *, int, struct record *);
void spellbook_find_last_cast_cb(struct record *);
void spell_compute_values(struct spell *, unsigned short, int);
void func_0005C856(struct record *, struct record *);
void cast_anim_start(int);
void func_0005CA28(struct record *);
void func_0005CA87(struct spell *);

int cast_item_strike_spell(int a1, struct record *a2)
{
    int l_1C;
    struct record *l_18;

    l_1C = 0;
    l_18 = object_create_child(player_object->parent, 0, 89);
L5AA29:;
    if (spell_records[l_1C].name[0] == 0) goto L5AA53;
    if (spell_records[l_1C].id == a1) goto L5AA5B;
L5AA53:;
    l_1C++;
    goto L5AA29;
L5AA5B:;
    l_18->type = 9;
    l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    mc_memcpy(&l_18->data.spell, &spell_records[l_1C], 89, (int)D_001757F4, 121, 4);
    l_18->data.spell.icon = 250;
    l_1C = spell_cost(&l_18->data.spell, player_character);
    if (cast_item_spell_at(l_18, a2) == 0) goto L5AAD2;
    object_delete(l_18);
L5AAD2:;
    return l_1C;
}

int cast_creature_spell(struct record *a1, struct record *a2, int a3)
{
    int l_18;
    struct record *l_14;

    l_18 = 0;
    l_14 = object_create_child(D_00195AC4, 0, 89);
L5AB14:;
    if (spell_records[l_18].name[0] == 0) goto L5AB3E;
    if (spell_records[l_18].id == a3) goto L5AB46;
L5AB3E:;
    l_18++;
    goto L5AB14;
L5AB46:;
    l_14->type = 9;
    l_14->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_14->caster = a1;
    mc_memcpy(&l_14->data.spell, &spell_records[l_18], 89, (int)D_001757F4, 144, 4);
    if (*(signed char *)D_00196292 == 0) goto L5ABA7;
    l_14->data.spell.icon = 250;
L5ABA7:;
    l_18 = spell_cost((struct spell *)((char *)l_14 + 89), &a1->data.character);
    if (cast_creature_spell_at(l_14, a1, a2) == 0) goto L5ABD5;
    object_delete(l_14);
L5ABD5:;
    return l_18;
}

void cast_spell_on(struct record *a1, struct record *a2, int a3)
{
    struct record *l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    struct spell *l_10;

    l_18 = 0;
    if (a1->caster == player_entity) goto L5AC19;
    if (((struct bf8_7_1 *)&cheat_flags)->f != 0) goto L5AC1B;
L5AC19:;
    goto L5AC20;
L5AC1B:;
    return;
L5AC20:;
    if (a1->caster != player_entity) goto L5AC39;
    if (a2 == player_entity) goto L5AC3B;
L5AC39:;
    goto L5AC4B;
L5AC3B:;
    if (a1->data.spell.target == 3) goto L5AC4D;
L5AC4B:;
    goto L5AC52;
L5AC4D:;
    return;
L5AC52:;
    *(short *)D_00195F28 = 512;
    l_10 = &a1->data.spell;
    if (l_10->target == 2) goto L5AC84;
    if (l_10->target != 4) goto L5AC86;
L5AC84:;
    goto L5ACEA;
L5AC86:;
    sound_play((int)(short)*(short *)(spell_cast_sounds + (l_10->element * 2)), player_object, 110);
    if (a1->caster != player_entity) goto L5ACC1;
    if (*(signed char *)D_00196291 == 0) goto L5ACC3;
L5ACC1:;
    goto L5ACD5;
L5ACC3:;
    cast_anim_start(l_10->element);
    goto L5ACEA;
L5ACD5:;
    damage_spawn_splash(a1->caster, 3, 3);
L5ACEA:;
    if (a3 != 0) goto L5AD15;
    l_14 = spell_resist_check(a1, &a2);
    if (l_14 != 0) goto L5AD13;
    hud_message_add(*(int *)D_0018509B);
    return;
L5AD13:;
    goto L5AD1C;
L5AD15:;
    l_14 = 100;
L5AD1C:;
    l_24 = object_clone(a1);
    l_24->caster = a1->caster;
    l_24->id = object_new_id(801);
    l_10 = &l_24->data.spell;
    spell_compute_values(l_10, l_24->caster->data.character.level, l_14);
    l_20 = 0;
    l_1C = l_20;
L5AD79:;
    if (l_20 < 3) goto L5AD8C;
    goto L5AE36;
L5AD84:;
    l_20++;
    goto L5AD79;
L5AD8C:;
    if (l_10->effects[l_20].type == 255) goto L5AD84;
    if (l_10->icon < 200) goto L5ADC6;
    if (l_10->icon != 250) goto L5ADD8;
L5ADC6:;
    if (spell_extend_duration(a2, l_10, l_20) != 0) goto L5ADDA;
L5ADD8:;
    goto L5ADE2;
L5ADDA:;
    l_18++;
    goto L5ADF0;
L5ADE2:;
    spell_apply_effect(l_24, l_20, a2);
L5ADF0:;
    if (l_10->effects[l_20].type == 255) goto L5AE29;
    if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (l_10->effects[l_20].type * 12)) & 1)) != 0) goto L5AE2B;
L5AE29:;
    goto L5AE31;
L5AE2B:;
    l_1C++;
L5AE31:;
    goto L5AD84;
L5AE36:;
    l_1C -= l_18;
    if (l_1C != 0) goto L5AE4C;
    object_delete(l_24);
    return;
L5AE4C:;
    object_reparent(a2, l_24);
}

void spellbook_find_last_cast_cb(struct record *a1)
{
    if (a1->type != 9) return;
    if ((short)((unsigned short)a1->data.spell.id) != *(short *)spell_last_cast_id) return;
    guild_npc_object = a1;
}

int cast_recast_last(void)
{
    struct record *l_28;
    struct record *l_24;
    struct spell *l_20;
    int l_1C;

    if ((int)spell_ready_missile != 0) goto L5B26C;
    if ((int)spell_ready_touch == 0) goto L5B282;
L5B26C:;
    hud_message_add((int)D_001757FF);
    return 0;
L5B282:;
    if (((int)(short)*(short *)spell_last_cast_id) != (-1)) goto L5B29A;
    return 0;
L5B29A:;
    l_28 = object_find_item(player_entity->children, 27, 0);
    if (l_28 != 0) goto L5B2CD;
    hud_message_add((int)D_00175820);
    return 0;
L5B2CD:;
    guild_npc_object = 0;
    object_foreach(l_28->children, (int)spellbook_find_last_cast_cb);
    l_28 = guild_npc_object;
    l_20 = &l_28->data.spell;
    l_1C = (int)(short)*(short *)D_00195F62;
    if ((player_character->magicka + *(int *)D_001959FC) >= l_1C) goto L5B32F;
    hud_message_add((int)D_00175837);
    return 0;
L5B32F:;
    spell_add_skill_uses(l_20, 1);
    if (*(int *)D_001959FC == 0) goto L5B376;
    if (l_1C <= *(int *)D_001959FC) goto L5B364;
    l_1C -= *(int *)D_001959FC;
    *(int *)D_001959FC = 0;
    goto L5B376;
L5B364:;
    *(int *)D_001959FC -= l_1C;
    *(short *)D_00195F62 = 0;
L5B376:;
    player_character->magicka -= l_1C;
    l_24 = object_create_child(player_object->parent, 0, 89);
    l_24->type = 9;
    l_24->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    mc_memcpy(&l_24->data.spell, &l_28->data.spell, 89, (int)D_001757F4, 443, 4);
    if (cast_player_spell(l_24) == 0) goto L5B3F4;
    object_delete(l_24);
L5B3F4:;
    return 1;
}

int spell_resist_check(struct record *a1, struct record **a2)
{
    struct character *l_2C;
    struct career *l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct spell *l_18;

    l_18 = &a1->data.spell;
    if (l_18->target != 0) goto L5B439;
    return 100;
L5B439:;
    if (a1->caster != *a2) goto L5B456;
    if (l_18->target == 3) goto L5B458;
L5B456:;
    goto L5B464;
L5B458:;
    return 100;
L5B464:;
    l_2C = &(*a2)->data.character;
    l_28 = &l_2C->career;
    if (((int)(unsigned char)(l_28->spell_absorption_flags & 7)) != 0) goto L5B49B;
    if ((l_2C->conditions & 0x200) == 0) goto L5B5A7;
L5B49B:;
    if ((l_2C->conditions & 0x200) != 0) goto L5B4B8;
    if (((int)(unsigned char)(l_28->spell_absorption_flags & 4)) == 0) goto L5B4BA;
L5B4B8:;
    goto L5B4D6;
L5B4BA:;
    if (((int)(unsigned char)(l_28->spell_absorption_flags & 2)) == 0) goto L5B4D4;
    if (player_in_daylight() == 0) goto L5B4D6;
L5B4D4:;
    goto L5B4D8;
L5B4D6:;
    goto L5B4F7;
L5B4D8:;
    if (((int)(unsigned char)(l_28->spell_absorption_flags & 1)) == 0) goto L5B4F2;
    if (player_in_daylight() != 0) goto L5B4F7;
L5B4F2:;
    goto L5B5A7;
L5B4F7:;
    if ((l_2C->conditions & 0x200) == 0) goto L5B559;
    if (spell_find_active_effect(*a2, 20, (int)&l_1C, 0) != 0) goto L5B545;
    l_1C = l_2C->level + 25;
    if (rand_range(1, 100) > l_1C) goto L5B5A7;
    goto L5B559;
L5B545:;
    if (rand_range(1, 100) > l_1C) goto L5B5A7;
L5B559:;
    l_20 = spell_cost(l_18, l_2C);
    if ((l_20 + l_2C->magicka) > l_2C->max_magicka) goto L5B5A7;
    l_2C->magicka += l_20;
    hud_message_add((int)D_00175858);
    return 0;
L5B5A7:;
    if ((l_2C->conditions & 0x400) == 0) goto L5B606;
    spell_find_active_effect(*a2, 21, (int)&l_1C, 0);
    if (rand_range(1, 100) > l_1C) goto L5B606;
    *a2 = a1->caster;
    l_2C = &(*a2)->data.character;
    l_28 = &l_2C->career;
    hud_message_add((int)D_0017586C);
L5B606:;
    if ((l_2C->conditions & 0x800) == 0) goto L5B64D;
    spell_find_active_effect(*a2, 22, (int)&l_1C, 0);
    if (rand_range(1, 100) > l_1C) goto L5B64D;
    hud_message_add((int)D_00175881);
    return 0;
L5B64D:;
    return spfx_resist_roll(l_18->element, (int)(unsigned char)*(signed char *)(spell_element_class_bits + l_18->element), l_2C, l_28, 2, (-(a1->caster->data.character.level - l_2C->level)) * 5);
}

int func_0005B6AE(struct spell *a1, struct character *a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = 0;
    l_14 = l_1C;
L5B6D0:;
    if (l_1C < 3) goto L5B6E3;
    goto L5B761;
L5B6DB:;
    l_1C++;
    goto L5B6D0;
L5B6E3:;
    if (a1->effects[l_1C].type == 255) goto L5B6DB;
    l_18 = a1->effect_costs[l_1C];
    l_18 = ((110 - a2->skills[(int)(unsigned char)*(signed char *)(magic_school_skills + ((int)(unsigned char)*(signed char *)(spell_effect_school + a1->effects[l_1C].type)))].value) * l_18) / 100;
    l_14 += l_18;
    goto L5B6DB;
L5B761:;
    return l_14;
}

struct spell *spell_find_active_effect(struct record *a1, int a2, int a3, int a4)
{
    struct spell *l_14;
    int l_10;

    a1 = a1->children;
L5B925:;
    if (a1 == 0) goto L5B9CE;
    if (a1->type != 9) goto L5B9C0;
    l_14 = &a1->data.spell;
    l_10 = 0;
L5B952:;
    if (l_10 < 3) goto L5B962;
    goto L5B9C0;
L5B95A:;
    l_10++;
    goto L5B952;
L5B962:;
    if (l_14->effects[l_10].type != a2) goto L5B9BE;
    if (a3 == 0) goto L5B98C;
    *(int *)((char *)a3) = l_14->cast_chances[l_10];
L5B98C:;
    if (a4 == 0) goto L5B9A5;
    *(int *)((char *)a4) = l_14->cast_magnitudes[l_10];
L5B9A5:;
    *(short *)spell_effect_slot = l_10;
    guild_npc_object = a1;
    return l_14;
L5B9BE:;
    goto L5B95A;
L5B9C0:;
    a1 = a1->next;
    goto L5B925;
L5B9CE:;
    return 0;
}

int spell_apply_effect(struct record *a1, int a2, struct record *a3)
{
    struct spell *l_14;

    l_14 = &a1->data.spell;
    return spell_effect_dispatch(l_14->effects[a2].type, a1, a2, a3);
}

void spell_compute_values(struct spell *a1, unsigned short a2, int a3)
{
    int l_18;

    if (a1->icon < 200) goto L5BA56;
    *(int *)&a2 = 8;
L5BA56:;
    l_18 = 0;
L5BA5D:;
    if (l_18 < 3) goto L5BA70;
    return;
L5BA68:;
    l_18++;
    goto L5BA5D;
L5BA70:;
    if (a1->effects[l_18].type == 255) goto L5BA68;
    if (((int)a1->durations[l_18].base) == (-1)) goto L5BB26;
    if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (a1->effects[l_18].type * 12)) & 1)) == 0) goto L5BB16;
    a1->cast_durations[l_18] = ((((int)a1->durations[l_18].base) + (((int)a1->durations[l_18].plus) * (((int)(unsigned short)a2) / ((int)a1->durations[l_18].per_level)))) * a3) / 100;
    goto L5BB24;
L5BB16:;
    a1->cast_durations[l_18] = 0;
L5BB24:;
    goto L5BB34;
L5BB26:;
    a1->cast_durations[l_18] = 65535;
L5BB34:;
    if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (a1->effects[l_18].type * 12)) & 2)) == 0) goto L5BB99;
    a1->cast_chances[l_18] = ((int)a1->chances[l_18].base) + (((int)a1->chances[l_18].plus) * (((int)(unsigned short)a2) / ((int)a1->chances[l_18].per_level)));
L5BB99:;
    if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (a1->effects[l_18].type * 12)) & 4)) == 0) goto L5BC4E;
    a1->cast_magnitudes[l_18] = (a3 * (rand_range((int)a1->magnitudes[l_18].base_min, (int)a1->magnitudes[l_18].base_max) + ((((int)(unsigned short)a2) / ((int)a1->magnitudes[l_18].per_level)) * rand_range((int)a1->magnitudes[l_18].plus_min, (int)a1->magnitudes[l_18].plus_max)))) / 100;
    *(short *)&a1->magnitudes[l_18].plus_min = 0;
L5BC4E:;
    goto L5BA68;
}

void spell_remove_effect_type(struct record *a1, int a2)
{
    struct spell *l_18;
    int l_14;

    a1 = a1->children;
L5BC77:;
    if (a1 == 0) return;
    if (a1->type != 9) goto L5BCCA;
    l_18 = &a1->data.spell;
    l_14 = spell_find_effect_type(l_18, a2);
    if (l_14 == 0) goto L5BCCA;
    l_18->effects[l_14].type = 255;
    if (func_000CE4E0(l_18) == 0) goto L5BCCA;
    object_delete(a1);
    return;
L5BCCA:;
    a1 = a1->next;
    goto L5BC77;
}

void cast_fire_missile(struct record *a1)
{
    int l_20;
    struct spell *l_1C;
    int l_18;
{
    char l_40[12];
    char l_34[16];

    object_reparent(D_00195AC4, a1);
    *(int *)((char *)l_34 + 12) = (int)object_create_child(a1, 0, 0);
    *(signed char *)(*(char **)((char *)l_34 + 12)) = 7;
    *(short *)(*(char **)((char *)l_34 + 12) + 27) = 48;
    *(short *)(*(char **)((char *)l_34 + 12) + 23) = 64;
    a1->x = player_object->x;
    *(int *)(*(char **)((char *)l_34 + 12) + 7) = a1->x;
    a1->y = player_object->y - 50;
    *(int *)(*(char **)((char *)l_34 + 12) + 11) = a1->y;
    a1->z = player_object->z;
    *(int *)(*(char **)((char *)l_34 + 12) + 15) = a1->z;
    *(short *)(*(char **)((char *)l_34 + 12) + 1) = 0;
    *(short *)(*(char **)((char *)l_34 + 12) + 3) = 0;
    if (*(signed char *)D_00153408 == 0) goto L5BE03;
    a1->angle_x = ((camera_object->angle_x + ((((((int)(short)*(short *)mouse_y) + 6) - ((int)(short)*(short *)D_000CEA34)) * 307) >> 8)) + *(int *)D_0015340D) & 2047;
    a1->yaw = (((((*(short *)mouse_x + 6) - *(short *)D_000CEA30) * 2) + camera_object->yaw) + *(short *)D_00153411) & 2047;
    goto L5BE89;
L5BE03:;
    a1->angle_x = ((((((((int)(short)*(short *)mouse_y) + 6) - ((int)(short)*(short *)D_000CEA34)) * 150) / 100) + camera_object->angle_x) + *(int *)view_look_pitch) & 2047;
    a1->yaw = ((camera_object->yaw + ((((((int)(short)*(short *)mouse_x) + 6) - ((int)(short)*(short *)D_000CEA30)) * 160) / 100)) + *(int *)D_001959BC) & 2047;
L5BE89:;
    *(short *)(*(char **)((char *)l_34 + 12) + 5) = (a1->angle_z = 0);
    mc_memset((int)l_40, 0, 12, (int)D_001757F4, 711, 4);
    func_000CE70D(a1->angle_x, a1->yaw, 1024, (int)l_40);
    *(int *)l_40 += a1->x;
    *(int *)((char *)l_40 + 4) += a1->y;
    *(int *)((char *)l_40 + 8) += a1->z;
    mc_memset((int)l_34, 0, 12, (int)D_001757F4, 717, 4);
    func_000C2000(&a1->x, (int)l_40, (char *)a1 + 118);
    func_000C2043((char *)a1 + 118, 110, (int)l_34);
    a1->x += *(int *)l_34;
    a1->y += *(int *)((char *)l_34 + 4);
    a1->z += *(int *)((char *)l_34 + 8);
    *(int *)(*(char **)((char *)l_34 + 12) + 7) = a1->x;
    *(int *)(*(char **)((char *)l_34 + 12) + 11) = a1->y;
    *(int *)(*(char **)((char *)l_34 + 12) + 15) = a1->z;
    l_1C = &a1->data.spell;
    a1->missile_texture = *(short *)(spell_missile_textures + (l_1C->element * 2));
    a1->flags |= 0x2000;
    a1->image2 = 0;
    *(int *)l_34 = (player_object->x + a1->x) / 2;
    *(int *)((char *)l_34 + 4) = (player_object->y + a1->y) / 2;
    *(int *)((char *)l_34 + 8) = (player_object->z + a1->z) / 2;
    if (func_00023EC2(player_object, (int)l_34, 65) == 0) goto L5C08D;
    sound_play((int)(short)*(short *)(spell_impact_sounds + (a1->data.spell.element * 2)), a1, 110);
    a1->missile_texture |= 1;
    a1->image2 = 32768;
    a1->children->light_radius <<= 2;
    func_0005C856(a1, D_00195C48);
    if (l_1C->target == 2) goto L5C078;
    if (l_1C->target != 4) goto L5C088;
L5C078:;
    cast_anim_start(l_1C->element);
L5C088:;
    return;
L5C08D:;
    spell_missile_update(a1, 1);
    if (l_1C->target == 2) goto L5C0BA;
    if (l_1C->target != 4) goto L5C0CA;
L5C0BA:;
    cast_anim_start(l_1C->element);
L5C0CA:;
    a1->y += 40;
    if (collide_line_of_sight(player_object, a1) != 0) goto L5C15B;
    a1->y -= 40;
    sound_play((int)(short)*(short *)(spell_impact_sounds + (l_1C->element * 2)), a1, 110);
    a1->missile_texture |= 1;
    a1->image2 = 32768;
    a1->children->light_radius <<= 2;
    if (l_1C->target == 2) goto L5C149;
    if (l_1C->target != 4) goto L5C159;
L5C149:;
    cast_anim_start(l_1C->element);
L5C159:;
    return;
L5C15B:;
    a1->y -= 40;
    spell_missile_update(a1, 1);
    if (l_1C->target == 2) goto L5C18F;
    if (l_1C->target != 4) goto L5C19F;
L5C18F:;
    cast_anim_start(l_1C->element);
L5C19F:;
    sound_play((int)(short)*(short *)(spell_cast_sounds + (l_1C->element * 2)), a1, 110);
}
}

void cast_creature_missile(struct record *a1, struct record *a2, struct record *a3)
{
    struct record *l_28;
    struct spell *l_10;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    char l_3C[12];

    object_reparent(D_00195AC4, a1);
    l_28 = object_create_child(a1, 0, 0);
    l_28->type = 7;
    l_28->image = 48;
    l_28->light_radius = 64;
    a1->x = a2->x;
    l_28->x = a1->x;
    a1->y = a2->y - 50;
    l_28->y = a1->y;
    a1->z = a2->z;
    l_28->z = a1->z;
    l_28->angle_x = 0;
    a1->angle_x = func_000C808D(a2->y, a2->z, a3->y, a3->z);
    l_28->yaw = 0;
    a1->yaw = func_000C808D(a2->x, a2->z, a3->x, a3->z);
    l_28->angle_z = (a1->angle_z = 0);
    func_000CE70D(a1->angle_x, a1->yaw, 110, &a1->x);
    l_28->x = a1->x;
    l_28->y = a1->y;
    l_28->z = a1->z;
    a3->y -= 50;
    func_000C2000(&a1->x, &a3->x, (char *)a1 + 118);
    a3->y += 50;
    l_10 = &a1->data.spell;
    a1->missile_texture = *(short *)(spell_missile_textures + (l_10->element * 2));
    a1->flags |= 0x2000;
    a1->image2 = 0;
    *(int *)l_3C = (a2->x + a1->x) / 2;
    *(int *)((char *)l_3C + 4) = (a2->y + a1->y) / 2;
    *(int *)((char *)l_3C + 8) = (a2->z + a1->z) / 2;
    if (func_00023EC2(a2, (int)l_3C, 65) == 0) goto L5C412;
    sound_play((int)(short)*(short *)(spell_impact_sounds + (a1->data.spell.element * 2)), a1, 110);
    a1->missile_texture |= 1;
    a1->image2 = 32768;
    a1->children->light_radius <<= 2;
    func_0005C856(a1, D_00195C48);
    return;
L5C412:;
    spell_missile_update(a1, 1);
    sound_play((int)(short)*(short *)(spell_cast_sounds + (l_10->element * 2)), a1, 110);
}

void spell_area_effect(struct record *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1->data.spell.target == 2) return;
    if (a1->data.spell.icon < 250) goto L5C6E5;
    l_1C = 168;
    goto L5C6FF;
L5C6E5:;
    l_1C = (a1->caster->data.character.level << 2) + 64;
L5C6FF:;
    l_1C = l_1C * 3;
    l_20 = 0;
L5C70F:;
    if (l_20 < *(int *)creature_count) goto L5C727;
    goto L5C7F5;
L5C71F:;
    l_20++;
    goto L5C70F;
L5C727:;
    if (func_000C7FF4(a1->y - D_00190504[l_20]->y, func_000C7FD9(a1->x, a1->z, D_00190504[l_20]->x, D_00190504[l_20]->z)) >= l_1C) goto L5C7F0;
    if (collide_line_of_sight(a1, D_00190504[l_20]) == 0) goto L5C71F;
    damage_knockback(D_00190504[l_20], l_1C * 30, func_000C808D(a1->x, a1->z, D_00190504[l_20]->x, D_00190504[l_20]->z), l_1C);
    func_0005C856(a1, D_00190504[l_20]);
L5C7F0:;
    goto L5C71F;
L5C7F5:;
    if (func_000C7FF4(a1->y - player_object->y, func_000C7FD9(a1->x, a1->z, player_object->x, player_object->z)) >= l_1C) return;
    func_0005CA87(&a1->data.spell);
    func_0007D774(a1, player_entity);
}

void func_0005C856(struct record *a1, struct record *a2)
{
    func_0005CA87(&a1->data.spell);
    if (a2->type != 18) return;
    func_0007D774(a1, a2);
}

void cast_anim_start(int a1)
{
    if (((int)(short)*(short *)cast_anim_state) > (-1)) return;
    *(short *)cast_anim_state = a1 << 4;
}

void cast_anim_update(void)
{
    int l_1C;
    int l_18;

    if (*(short *)cast_anim_state < 0) return;
    if (((int)(short)(*(short *)cast_anim_state & 15)) != 6) goto L5C952;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) == 0) goto L5C916;
    l_18 = 0;
    goto L5C92A;
L5C916:;
    l_18 = -((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 6));
L5C92A:;
    func_000CDB7A(*(int *)(spell_cast_anim_fire + ((((int)(short)*(short *)cast_anim_state) >> 4) << 2)), 0, l_18);
    *(short *)cast_anim_state = 65535;
    return;
L5C952:;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) == 0) goto L5C971;
    l_1C = 0;
    goto L5C985;
L5C971:;
    l_1C = -((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 6));
L5C985:;
    func_000CDB7A(*(int *)(spell_cast_anim_fire + ((((int)(short)*(short *)cast_anim_state) >> 4) << 2)), (int)(short)(*(short *)cast_anim_state & 15), l_1C);
    (*(short *)cast_anim_state)++;
}

int func_0005C9BF(unsigned char a1)
{
    struct record *l_20;

    l_20 = player_entity->children;
L5C9DB:;
    if (l_20 == 0) goto L5CA14;
    if (l_20->type != 9) goto L5C9FE;
    if (l_20->data.spell.id == a1) goto L5CA00;
L5C9FE:;
    goto L5CA09;
L5CA00:;
    return 1;
L5CA09:;
    l_20 = l_20->next;
    goto L5C9DB;
L5CA14:;
    return 0;
}

void func_0005CA28(struct record *a1)
{
    if (a1->type != 9) return;
    if ((signed char)D_00199D64->id != (signed char)a1->data.spell.id) return;
    mc_strncpy((int)D_00199D64 + 47, a1->data.spell.name, 25, (int)D_001757F4, 958);
}

void func_0005CA87(struct spell *a1)
{
    int l_1C;
    struct record *l_18;

    l_1C = 0;
L5CA9F:;
    if ((signed char)spell_records[l_1C].name[0] == 0) goto L5CAD2;
    if ((signed char)spell_records[l_1C].id == (signed char)a1->id) goto L5CAD0;
    if (l_1C < 128) goto L5CAD2;
L5CAD0:;
    goto L5CADA;
L5CAD2:;
    l_1C++;
    goto L5CA9F;
L5CADA:;
    if (l_1C < 128) goto L5CB14;
    l_18 = object_find_item(player_entity->children, 27, 0);
    D_00199D64 = a1;
    object_foreach(l_18->children, (int)func_0005CA28);
    return;
L5CB14:;
    mc_strncpy(a1->name, (int)(signed char *)&spell_records[l_1C].name[0], 25, (int)D_001757F4, 974);
}

void spell_hud_draw_icons(void)
{
    int l_48;
    int l_44;
    int l_40;
    struct record *l_3C;
    struct record *l_38;
    struct spell *l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    *(int *)D_00199D6C = 0;
    if (player_entity->children == 0) return;
    l_3C = player_entity->children;
    *(signed char *)D_00199D71 = 1;
L5CB80:;
    if (l_3C == 0) goto L5CDA7;
    if (l_3C->type != 9) goto L5CD99;
    if ((int)l_3C->caster == (int)player_entity) goto L5CBB4;
    l_40 = 1;
    goto L5CBBB;
L5CBB4:;
    l_40 = 0;
L5CBBB:;
    l_20 = l_40;
    if (l_20 == 0) goto L5CBCE;
    *(signed char *)D_00199D71 = 1;
L5CBCE:;
    l_34 = &l_3C->data.spell;
    l_2C = 1;
    l_30 = 0;
L5CBE5:;
    if (l_30 < 3) goto L5CBF5;
    goto L5CC2E;
L5CBED:;
    l_30++;
    goto L5CBE5;
L5CBF5:;
    if (l_34->effects[l_30].type == 255) goto L5CC21;
    if (l_34->cast_durations[l_30] > 1) goto L5CC23;
L5CC21:;
    goto L5CC2C;
L5CC23:;
    l_2C = 0;
    goto L5CC2E;
L5CC2C:;
    goto L5CBED;
L5CC2E:;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) == 0) goto L5CC4A;
    if (l_20 != 0) goto L5CC4C;
L5CC4A:;
    goto L5CC55;
L5CC4C:;
    l_1C = 11;
    goto L5CC5C;
L5CC55:;
    l_1C = 12;
L5CC5C:;
    l_28 = (int)(unsigned short)*(short *)(D_00185CEC + ((*(int *)D_00199D6C % l_1C) * 2));
    if (((int)(unsigned short)(game_settings->view_flags & 1)) == 0) goto L5CC95;
    l_28 += 35;
L5CC95:;
    l_24 = ((*(int *)D_00199D6C / 12) * 24) + 16;
    l_18 = 1132;
    if (l_2C == 0) goto L5CCC8;
    if (((struct bf8_3_1 *)((char *)l_18))->f != 0) goto L5CCD2;
L5CCC8:;
    if (l_2C != 0) goto L5CD93;
L5CCD2:;
    if (l_20 == 0) goto L5CD13;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) == 0) goto L5CCF7;
    l_44 = 177;
    goto L5CD0B;
L5CCF7:;
    l_44 = ((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2)) - 22;
L5CD0B:;
    l_24 = l_44;
    goto L5CD5F;
L5CD13:;
    if (l_34->effects[0].type != 35) goto L5CD34;
    if (l_34->effects[1].type == 255) goto L5CD36;
L5CD34:;
    goto L5CD44;
L5CD36:;
    if (player_character->shield_points == 0) goto L5CD46;
L5CD44:;
    goto L5CD5F;
L5CD46:;
    l_38 = l_3C->next;
    spell_end(l_3C);
    l_3C = l_38;
    goto L5CDA2;
L5CD5F:;
    if (l_34->icon < 200) goto L5CD7A;
    l_48 = 10;
    goto L5CD85;
L5CD7A:;
    l_48 = l_34->icon;
L5CD85:;
    func_000CD20E(l_28, l_24, l_48);
L5CD93:;
    (*(int *)D_00199D6C)++;
L5CD99:;
    l_3C = l_3C->next;
L5CDA2:;
    goto L5CB80;
L5CDA7:;
    if (*(int *)D_00199D6C != 0) return;
    l_30 = 0;
L5CDB7:;
    if (l_30 < 8) goto L5CDC7;
    return;
L5CDBF:;
    l_30++;
    goto L5CDB7;
L5CDC7:;
    if (player_character->attributes[l_30] <= player_character->base_attributes[l_30]) goto L5CE0B;
    player_character->attributes[l_30] = player_character->base_attributes[l_30];
L5CE0B:;
    goto L5CDBF;
}
