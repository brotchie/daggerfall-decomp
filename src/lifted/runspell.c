/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern short D_000CEA30;
extern short D_000CEA34;
extern short mouse_x;
extern short mouse_y;
extern signed char D_00153408;
extern int D_0015340D;
extern short D_00153411;
extern char D_001757F4[];
extern char D_001757FF[];
extern char D_00175820[];
extern char D_00175837[];
extern char D_00175858[];
extern char D_0017586C[];
extern char D_00175881[];
extern short cast_anim_state;
extern signed char spell_effect_school[];
extern char spell_effect_settings[];
extern int D_0018509B;
extern short spell_last_cast_id;
extern char spell_missile_textures[];
extern char spell_cast_sounds[];
extern char spell_impact_sounds[];
extern signed char magic_school_skills[];
extern signed char spell_element_class_bits[];
extern short D_00185CEC[];
extern struct record *D_00190504[];
extern int view_look_pitch;
extern int D_001959BC;
extern int D_001959FC;
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char cheat_flags[];
extern struct spell *spell_records;
extern int creature_count;
extern struct record *spell_ready_missile;
extern struct record *spell_ready_touch;
extern struct record *guild_npc_object;
extern char *hud_bar_image;
extern struct character *player_character;
extern struct settings *game_settings;
extern struct record *D_00195C48;
extern int spell_cast_anim_fire[];
extern char D_00195F28[];
extern short spell_effect_slot;
extern short D_00195F62;
extern signed char D_00196291;
extern signed char D_00196292;
extern struct spell *D_00199D64;
extern int D_00199D6C;
extern signed char D_00199D71;

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
    while (spell_records[l_1C].name[0] == 0 || spell_records[l_1C].id != a1) l_1C++;
    l_18->type = 9;
    l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    mc_memcpy(&l_18->data.spell, &spell_records[l_1C], 89, (int)D_001757F4, 121, 4);
    l_18->data.spell.icon = 250;
    l_1C = spell_cost(&l_18->data.spell, player_character);
    if (cast_item_spell_at(l_18, a2) != 0) object_delete(l_18);
    return l_1C;
}

int cast_creature_spell(struct record *a1, struct record *a2, int a3)
{
    int l_18;
    struct record *l_14;

    l_18 = 0;
    l_14 = object_create_child(D_00195AC4, 0, 89);
    while (spell_records[l_18].name[0] == 0 || spell_records[l_18].id != a3) l_18++;
    l_14->type = 9;
    l_14->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_14->caster = a1;
    mc_memcpy(&l_14->data.spell, &spell_records[l_18], 89, (int)D_001757F4, 144, 4);
    if (D_00196292 != 0) l_14->data.spell.icon = 250;
    l_18 = spell_cost((struct spell *)((char *)l_14 + 89), &a1->data.character);
    if (cast_creature_spell_at(l_14, a1, a2) != 0) object_delete(l_14);
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
    if (a1->caster != player_entity && ((struct bf8_7_1 *)&cheat_flags)->f != 0) return;
    if (a1->caster == player_entity && a2 == player_entity && a1->data.spell.target == 3) return;
    *(short *)D_00195F28 = 512;
    l_10 = &a1->data.spell;
    if (l_10->target != 2 && l_10->target != 4) {
        sound_play((int)(short)*(short *)(spell_cast_sounds + (l_10->element * 2)), player_object, 110);
        if (a1->caster == player_entity && D_00196291 == 0) {
            cast_anim_start(l_10->element);
        } else {
            damage_spawn_splash(a1->caster, 3, 3);
        }
    }
    if (a3 == 0) {
        l_14 = spell_resist_check(a1, &a2);
        if (l_14 == 0) {
            hud_message_add(D_0018509B);
            return;
        }
    } else {
        l_14 = 100;
    }
    l_24 = object_clone(a1);
    l_24->caster = a1->caster;
    l_24->id = object_new_id(801);
    l_10 = &l_24->data.spell;
    spell_compute_values(l_10, l_24->caster->data.character.level, l_14);
    l_20 = 0;
    l_1C = l_20;
    for (; l_20 < 3; l_20++) {
        if (l_10->effects[l_20].type == 255) continue;
        if ((l_10->icon < 200 || l_10->icon == 250) && spell_extend_duration(a2, l_10, l_20) != 0) {
            l_18++;
        } else {
            spell_apply_effect(l_24, l_20, a2);
        }
        if (l_10->effects[l_20].type != 255 && ((int)(unsigned char)(*(signed char *)(spell_effect_settings + (l_10->effects[l_20].type * 12)) & 1)) != 0) {
            l_1C++;
        }
    }
    l_1C -= l_18;
    if (l_1C == 0) {
        object_delete(l_24);
        return;
    }
    object_reparent(a2, l_24);
}

void spellbook_find_last_cast_cb(struct record *a1)
{
    if (a1->type != 9) return;
    if ((short)((unsigned short)a1->data.spell.id) != spell_last_cast_id) return;
    guild_npc_object = a1;
}

int cast_recast_last(void)
{
    struct record *l_28;
    struct record *l_24;
    struct spell *l_20;
    int l_1C;

    if ((int)spell_ready_missile != 0 || (int)spell_ready_touch != 0) {
        hud_message_add((int)D_001757FF);
        return 0;
    }
    if (((int)(short)spell_last_cast_id) == (-1)) return 0;
    l_28 = object_find_item(player_entity->children, 27, 0);
    if (l_28 == 0) {
        hud_message_add((int)D_00175820);
        return 0;
    }
    guild_npc_object = 0;
    object_foreach(l_28->children, (int)spellbook_find_last_cast_cb);
    l_28 = guild_npc_object;
    l_20 = &l_28->data.spell;
    l_1C = (int)(short)D_00195F62;
    if ((player_character->magicka + D_001959FC) < l_1C) {
        hud_message_add((int)D_00175837);
        return 0;
    }
    spell_add_skill_uses(l_20, 1);
    if (D_001959FC != 0) {
        if (l_1C > D_001959FC) {
            l_1C -= D_001959FC;
            D_001959FC = 0;
        } else {
            D_001959FC -= l_1C;
            D_00195F62 = 0;
        }
    }
    player_character->magicka -= l_1C;
    l_24 = object_create_child(player_object->parent, 0, 89);
    l_24->type = 9;
    l_24->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    mc_memcpy(&l_24->data.spell, &l_28->data.spell, 89, (int)D_001757F4, 443, 4);
    if (cast_player_spell(l_24) != 0) object_delete(l_24);
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
    if (l_18->target == 0) return 100;
    if (a1->caster == *a2 && l_18->target == 3) return 100;
    l_2C = &(*a2)->data.character;
    l_28 = &l_2C->career;
    if (((int)(unsigned char)(l_28->spell_absorption_flags & 7)) != 0 || (l_2C->conditions & 0x200) != 0) {
        if ((l_2C->conditions & 0x200) != 0 || ((int)(unsigned char)(l_28->spell_absorption_flags & 4)) != 0 || (((int)(unsigned char)(l_28->spell_absorption_flags & 2)) != 0 && player_in_daylight() == 0) || (((int)(unsigned char)(l_28->spell_absorption_flags & 1)) != 0 && player_in_daylight() != 0)) {
            if ((l_2C->conditions & 0x200) != 0) {
                if (spell_find_active_effect(*a2, 20, (int)&l_1C, 0) == 0) {
                    l_1C = l_2C->level + 25;
                    if (rand_range(1, 100) > l_1C) goto L5B5A7;
                } else {
                    if (rand_range(1, 100) > l_1C) goto L5B5A7;
                }
            }
            l_20 = spell_cost(l_18, l_2C);
            if ((l_20 + l_2C->magicka) <= l_2C->max_magicka) {
                l_2C->magicka += l_20;
                hud_message_add((int)D_00175858);
                return 0;
            }
        }
    }
L5B5A7:;
    if ((l_2C->conditions & 0x400) != 0) {
        spell_find_active_effect(*a2, 21, (int)&l_1C, 0);
        if (rand_range(1, 100) <= l_1C) {
            *a2 = a1->caster;
            l_2C = &(*a2)->data.character;
            l_28 = &l_2C->career;
            hud_message_add((int)D_0017586C);
        }
    }
    if ((l_2C->conditions & 0x800) != 0) {
        spell_find_active_effect(*a2, 22, (int)&l_1C, 0);
        if (rand_range(1, 100) <= l_1C) {
            hud_message_add((int)D_00175881);
            return 0;
        }
    }
    return spfx_resist_roll(l_18->element, (int)(unsigned char)spell_element_class_bits[l_18->element], l_2C, l_28, 2, (-(a1->caster->data.character.level - l_2C->level)) * 5);
}

int func_0005B6AE(struct spell *a1, struct character *a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = 0;
    l_14 = l_1C;
    for (; l_1C < 3; l_1C++) {
        if (a1->effects[l_1C].type == 255) continue;
        l_18 = a1->effect_costs[l_1C];
        l_18 = ((110 - a2->skills[(int)(unsigned char)magic_school_skills[(int)(unsigned char)spell_effect_school[a1->effects[l_1C].type]]].value) * l_18) / 100;
        l_14 += l_18;
    }
    return l_14;
}

struct spell *spell_find_active_effect(struct record *a1, int a2, int a3, int a4)
{
    struct spell *l_14;
    int l_10;

    a1 = a1->children;
    while (a1 != 0) {
        if (a1->type == 9) {
            l_14 = &a1->data.spell;
            for (l_10 = 0; l_10 < 3; l_10++) {
                if (l_14->effects[l_10].type == a2) {
                    if (a3 != 0) *(int *)((char *)a3) = l_14->cast_chances[l_10];
                    if (a4 != 0) *(int *)((char *)a4) = l_14->cast_magnitudes[l_10];
                    spell_effect_slot = l_10;
                    guild_npc_object = a1;
                    return l_14;
                }
            }
        }
        a1 = a1->next;
    }
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

    if (a1->icon >= 200) *(int *)&a2 = 8;
    for (l_18 = 0; l_18 < 3; l_18++) {
        if (a1->effects[l_18].type == 255) continue;
        if (((int)a1->durations[l_18].base) != (-1)) {
            if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (a1->effects[l_18].type * 12)) & 1)) != 0) {
                a1->cast_durations[l_18] = ((((int)a1->durations[l_18].base) + (((int)a1->durations[l_18].plus) * (((int)(unsigned short)a2) / ((int)a1->durations[l_18].per_level)))) * a3) / 100;
            } else {
                a1->cast_durations[l_18] = 0;
            }
        } else {
            a1->cast_durations[l_18] = 65535;
        }
        if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (a1->effects[l_18].type * 12)) & 2)) != 0) {
            a1->cast_chances[l_18] = ((int)a1->chances[l_18].base) + (((int)a1->chances[l_18].plus) * (((int)(unsigned short)a2) / ((int)a1->chances[l_18].per_level)));
        }
        if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (a1->effects[l_18].type * 12)) & 4)) != 0) {
            a1->cast_magnitudes[l_18] = (a3 * (rand_range((int)a1->magnitudes[l_18].base_min, (int)a1->magnitudes[l_18].base_max) + ((((int)(unsigned short)a2) / ((int)a1->magnitudes[l_18].per_level)) * rand_range((int)a1->magnitudes[l_18].plus_min, (int)a1->magnitudes[l_18].plus_max)))) / 100;
            *(short *)&a1->magnitudes[l_18].plus_min = 0;
        }
    }
}

void spell_remove_effect_type(struct record *a1, int a2)
{
    struct spell *l_18;
    int l_14;

    a1 = a1->children;
    while (a1 != 0) {
        if (a1->type == 9) {
            l_18 = &a1->data.spell;
            l_14 = spell_find_effect_type(l_18, a2);
            if (l_14 != 0) {
                l_18->effects[l_14].type = 255;
                if (func_000CE4E0(l_18) != 0) {
                    object_delete(a1);
                    return;
                }
            }
        }
        a1 = a1->next;
    }
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
        if (D_00153408 != 0) {
            a1->angle_x = ((camera_object->angle_x + ((((((int)(short)mouse_y) + 6) - ((int)(short)D_000CEA34)) * 307) >> 8)) + D_0015340D) & 2047;
            a1->yaw = (((((mouse_x + 6) - D_000CEA30) * 2) + camera_object->yaw) + D_00153411) & 2047;
        } else {
            a1->angle_x = ((((((((int)(short)mouse_y) + 6) - ((int)(short)D_000CEA34)) * 150) / 100) + camera_object->angle_x) + view_look_pitch) & 2047;
            a1->yaw = ((camera_object->yaw + ((((((int)(short)mouse_x) + 6) - ((int)(short)D_000CEA30)) * 160) / 100)) + D_001959BC) & 2047;
        }
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
        if (func_00023EC2(player_object, (int)l_34, 65) != 0) {
            sound_play((int)(short)*(short *)(spell_impact_sounds + (a1->data.spell.element * 2)), a1, 110);
            a1->missile_texture |= 1;
            a1->image2 = 32768;
            a1->children->light_radius <<= 2;
            func_0005C856(a1, D_00195C48);
            if (l_1C->target == 2 || l_1C->target == 4) cast_anim_start(l_1C->element);
            return;
        }
        spell_missile_update(a1, 1);
        if (l_1C->target == 2 || l_1C->target == 4) cast_anim_start(l_1C->element);
        a1->y += 40;
        if (collide_line_of_sight(player_object, a1) == 0) {
            a1->y -= 40;
            sound_play((int)(short)*(short *)(spell_impact_sounds + (l_1C->element * 2)), a1, 110);
            a1->missile_texture |= 1;
            a1->image2 = 32768;
            a1->children->light_radius <<= 2;
            if (l_1C->target == 2 || l_1C->target == 4) cast_anim_start(l_1C->element);
            return;
        }
        a1->y -= 40;
        spell_missile_update(a1, 1);
        if (l_1C->target == 2 || l_1C->target == 4) cast_anim_start(l_1C->element);
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
    if (func_00023EC2(a2, (int)l_3C, 65) != 0) {
        sound_play((int)(short)*(short *)(spell_impact_sounds + (a1->data.spell.element * 2)), a1, 110);
        a1->missile_texture |= 1;
        a1->image2 = 32768;
        a1->children->light_radius <<= 2;
        func_0005C856(a1, D_00195C48);
        return;
    }
    spell_missile_update(a1, 1);
    sound_play((int)(short)*(short *)(spell_cast_sounds + (l_10->element * 2)), a1, 110);
}

void spell_area_effect(struct record *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1->data.spell.target == 2) return;
    if (a1->data.spell.icon >= 250) {
        l_1C = 168;
    } else {
        l_1C = (a1->caster->data.character.level << 2) + 64;
    }
    l_1C = l_1C * 3;
    for (l_20 = 0; l_20 < creature_count; l_20++) {
        if (func_000C7FF4(a1->y - D_00190504[l_20]->y, func_000C7FD9(a1->x, a1->z, D_00190504[l_20]->x, D_00190504[l_20]->z)) < l_1C) {
            if (collide_line_of_sight(a1, D_00190504[l_20]) == 0) continue;
            damage_knockback(D_00190504[l_20], l_1C * 30, func_000C808D(a1->x, a1->z, D_00190504[l_20]->x, D_00190504[l_20]->z), l_1C);
            func_0005C856(a1, D_00190504[l_20]);
        }
    }
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
    if (((int)(short)cast_anim_state) > (-1)) return;
    cast_anim_state = a1 << 4;
}

void cast_anim_update(void)
{
    int l_1C;
    int l_18;

    if (cast_anim_state < 0) return;
    if (((int)(short)(cast_anim_state & 15)) == 6) {
        if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
            l_18 = 0;
        } else {
            l_18 = -((int)(unsigned short)*(short *)(hud_bar_image + 6));
        }
        func_000CDB7A(spell_cast_anim_fire[(((int)(short)cast_anim_state) >> 4)], 0, l_18);
        cast_anim_state = 65535;
        return;
    }
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        l_1C = 0;
    } else {
        l_1C = -((int)(unsigned short)*(short *)(hud_bar_image + 6));
    }
    func_000CDB7A(spell_cast_anim_fire[(((int)(short)cast_anim_state) >> 4)], (int)(short)(cast_anim_state & 15), l_1C);
    (cast_anim_state)++;
}

int func_0005C9BF(unsigned char a1)
{
    struct record *l_20;

    l_20 = player_entity->children;
    while (l_20 != 0) {
        if (l_20->type == 9 && l_20->data.spell.id == a1) return 1;
        l_20 = l_20->next;
    }
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
    while ((signed char)spell_records[l_1C].name[0] == 0 || ((signed char)spell_records[l_1C].id != (signed char)a1->id && l_1C < 128)) {
        l_1C++;
    }
    if (l_1C >= 128) {
        l_18 = object_find_item(player_entity->children, 27, 0);
        D_00199D64 = a1;
        object_foreach(l_18->children, (int)func_0005CA28);
        return;
    }
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

    D_00199D6C = 0;
    if (player_entity->children == 0) return;
    l_3C = player_entity->children;
    D_00199D71 = 1;
    while (l_3C != 0) {
        if (l_3C->type == 9) {
            if ((int)l_3C->caster != (int)player_entity) {
                l_40 = 1;
            } else {
                l_40 = 0;
            }
            l_20 = l_40;
            if (l_20 != 0) D_00199D71 = 1;
            l_34 = &l_3C->data.spell;
            l_2C = 1;
            for (l_30 = 0; l_30 < 3; l_30++) {
                if (l_34->effects[l_30].type != 255 && l_34->cast_durations[l_30] > 1) {
                    l_2C = 0;
                    break;
                }
            }
            if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0 && l_20 != 0) {
                l_1C = 11;
            } else {
                l_1C = 12;
            }
            l_28 = (int)(unsigned short)D_00185CEC[(D_00199D6C % l_1C)];
            if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) l_28 += 35;
            l_24 = ((D_00199D6C / 12) * 24) + 16;
            l_18 = 1132;
            if ((l_2C != 0 && ((struct bf8_3_1 *)((char *)l_18))->f != 0) || l_2C == 0) {
                if (l_20 != 0) {
                    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
                        l_44 = 177;
                    } else {
                        l_44 = ((int)(unsigned short)*(short *)(hud_bar_image + 2)) - 22;
                    }
                    l_24 = l_44;
                } else if (l_34->effects[0].type == 35 && l_34->effects[1].type == 255 && player_character->shield_points == 0) {
                    l_38 = l_3C->next;
                    spell_end(l_3C);
                    l_3C = l_38;
                    continue;
                }
                if (l_34->icon >= 200) {
                    l_48 = 10;
                } else {
                    l_48 = l_34->icon;
                }
                func_000CD20E(l_28, l_24, l_48);
            }
            (D_00199D6C)++;
        }
        l_3C = l_3C->next;
    }
    if (D_00199D6C != 0) return;
    for (l_30 = 0; l_30 < 8; l_30++) {
        if (player_character->attributes[l_30] > player_character->base_attributes[l_30]) {
            player_character->attributes[l_30] = player_character->base_attributes[l_30];
        }
    }
}
