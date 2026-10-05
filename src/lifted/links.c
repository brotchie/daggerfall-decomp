/* links.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int D_00147954;
extern char D_00175962[];
extern signed char text_rsc_buffer[];
extern signed char D_00190FE5[];
extern struct record *player_entity;
extern struct record *D_00195AC4;
extern struct spell *spell_records;
extern struct character *player_character;
extern int D_00195C44;
extern char D_00199D78[];
extern signed char D_00199D7B[];
extern signed char D_00199D84[];
extern char D_00199D9B[];
extern int active_links[];
extern int link_count;
extern int active_link_count;

extern int func_000658CA(unsigned short, unsigned short);
extern int hud_message_add(int);
extern int spfx_damage(struct record *, int, struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_find_by_id(struct record *, int);
extern int func_000A00CB();
extern int write();
extern int func_000A0DF4();
extern int stricmp();
extern int mc_memcpy();
extern int func_000CE77F();
extern void parse_rsc_text(int, int, int);
extern void parse_rsc_text_copy(int, int);

void links_save(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (active_link_count < 0) active_link_count = 0;
    mc_memcpy(D_00147954, (int)D_00199D78, link_count * 39, (int)D_00175962, 78, 4);
    l_1C = D_00147954;
    for (l_20 = 0; l_20 < link_count; l_20++) {
        if (*(int *)((char *)((l_20 * 39) + l_1C) + 35) != 0) {
            *(int *)((char *)((l_20 * 39) + l_1C) + 35) = *(int *)(*(char **)((char *)((l_20 * 39) + l_1C) + 35) + 31);
        }
    }
    write(a1, (int)&link_count, 4);
    write(a1, l_1C, link_count * 39);
    write(a1, (int)&active_link_count, 4);
    l_18 = D_00147954;
    for (l_20 = 0; l_20 < active_link_count; l_20++) {
        *(int *)((char *)((l_20 << 2) + l_18)) = ((unsigned)(active_links[l_20] - ((int)D_00199D78))) / 39;
    }
    write(a1, l_18, active_link_count << 2);
}

void links_load(int a1)
{
    int l_18;

    func_000A00CB(a1, (int)&link_count, 4);
    func_000A00CB(a1, (int)D_00199D78, link_count * 39);
    for (l_18 = 0; l_18 < link_count; l_18++) {
        if (((int)(unsigned char)D_00199D7B[l_18 * 39]) == 108) {
            D_00199D7B[l_18 * 39] = 100;
        }
        if (*(int *)(D_00199D9B + (l_18 * 39)) != 0) {
            *(int *)(D_00199D9B + (l_18 * 39)) = (int)object_find_by_id(D_00195AC4, *(int *)(D_00199D9B + (l_18 * 39)));
        }
    }
    func_000A00CB(a1, (int)&active_link_count, 4);
    if (active_link_count < 0) active_link_count = 0;
    func_000A00CB(a1, (int)active_links, active_link_count << 2);
    for (l_18 = 0; l_18 < active_link_count; l_18++) {
        active_links[l_18] = ((int)D_00199D78) + (active_links[l_18] * 39);
    }
}

void link_show_text(int a1)
{
    int l_1C;
    int l_18;

    parse_rsc_text_copy(a1, D_00195C44);
    l_1C = D_00195C44;
    while (*(signed char *)((char *)l_1C) != 0) {
        l_18 = l_1C;
        while (*(signed char *)((char *)l_18) != 0 && ((int)(unsigned char)*(signed char *)((char *)l_18)) != 252 && ((int)(unsigned char)*(signed char *)((char *)l_18)) != 253) {
            l_18++;
        }
        *(signed char *)((char *)l_18) = 0;
        hud_message_add(l_1C);
        l_1C = l_18 + 1;
    }
}

int link_answer_matches(int a1, int a2)
{
    int l_1C;
    int l_18;

    parse_rsc_text(a1, 0, 0);
    l_18 = func_000A0DF4((int)text_rsc_buffer);
    for (l_1C = 1; l_1C < l_18; l_1C++) {
        if (((int)(unsigned char)text_rsc_buffer[l_1C]) == 44 || ((int)(unsigned char)text_rsc_buffer[l_1C]) == 34) {
            text_rsc_buffer[l_1C] = 0;
        }
    }
    D_00190FE5[l_1C] = 0;
    l_1C = 1;
    while (text_rsc_buffer[l_1C] != 0) {
        if (stricmp(((int)text_rsc_buffer) + l_1C, a2) == 0) return 1;
        l_1C += func_000A0DF4(((int)text_rsc_buffer) + l_1C) + 1;
    }
    return 0;
}

void link_hurt_player(int a1, int a2)
{
    struct record *l_18;
    struct spell *l_14;

    l_18 = object_create_child(D_00195AC4, 0, 89);
    l_14 = &l_18->data.spell;
    l_14->cast_magnitudes[0] = a2 * ((unsigned short)player_character->level);
    l_14->effects[0].subtype = 0;
    l_14->element = *(signed char *)&a1;
    spfx_damage(l_18, 0, player_entity);
    object_delete(l_18);
}

void link_start(struct link *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    a1->flags &= 250;
    switch (a1->action) {
    case 1:
        if (a1->duration != 0) {
            a1->speed = (a1->magnitude << 16) / a1->duration;
        } else {
            a1->speed = a1->magnitude << 16;
        }
        if (((int)(unsigned char)(a1->flags & 2)) != 0) a1->speed = -a1->speed;
        l_1C = 1132;
        a1->start_tick = *(int *)((char *)l_1C);
        switch (a1->axis - 1) {
            break;
        case 0:
        case 1:
            a1->start = a1->object->x;
            break;
        case 2:
        case 3:
            a1->start = a1->object->y;
            break;
        case 4:
        case 5:
            a1->start = a1->object->z;
        }
        return;
    case 8:
        if (a1->duration != 0) {
            a1->speed = (a1->magnitude << 16) / a1->duration;
        } else {
            a1->speed = a1->magnitude << 16;
        }
        if (((int)(unsigned char)(a1->flags & 2)) != 0) a1->speed = -a1->speed;
        l_18 = 1132;
        a1->start_tick = *(int *)((char *)l_18);
        if (((int)(unsigned char)(a1->flags & 32)) == 0 && (l_20 = func_000658CA(a1->object->image2, a1->object->image)) != 0) {
            a1->flags |= 32;
            if (((int)(unsigned char)(a1->axis & 1)) != 0) l_20++;
            a1->axis = *(signed char *)&l_20;
        }
        switch (a1->axis - 1) {
            break;
        case 0:
        case 1:
            a1->start = a1->object->angle_x;
            break;
        case 2:
        case 3:
            a1->start = a1->object->yaw;
            break;
        case 4:
        case 5:
            a1->start = a1->object->angle_z;
        }
        return;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        if (((int)(unsigned char)(a1->flags & 2)) == 0) return;
        a1->flags |= 1;
    default:;
    }
}

void func_00065748(short a1, int a2)
{
    int l_18;

    for (l_18 = 0; l_18 < link_count; l_18++) {
        if (*(unsigned short *)(D_00199D78 + (l_18 * 39)) == (short)a1) {
            D_00199D84[l_18 * 39] &= 253;
            D_00199D84[l_18 * 39] |= *(signed char *)&a2 & 2;
        }
    }
}

int func_000657B2(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    for (l_28 = 0; l_28 < active_link_count; l_28++) {
        l_1C = active_links[l_28];
        l_24 = ((int)(unsigned char)*(signed char *)((char *)l_1C + 10)) + 1;
        for (l_20 = 0; l_20 < l_24; l_20++, (*(char (**)[39])&l_1C)++) {
            if (*(int *)((char *)l_1C + 35) == a1 && func_000CE77F(l_1C + 29, 6) != 0) {
                return l_1C + 29;
            }
        }
    }
    return 0;
}

struct spell *func_00065864(int a1)
{
    int l_1C;

    l_1C = 0;
    while (spell_records[l_1C].name[0] == 0 || spell_records[l_1C].id != a1) l_1C++;
    return &spell_records[l_1C];
}
