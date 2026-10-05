/* matched by the real Watcom C32 10.0a (-d2): a run of links.c from 0x00064589 to 0x0006480F, kept together for its switch table's alignment */
#include "records.h"

extern struct link D_00199D78[];
extern struct link *active_links[];
extern int link_count;
extern int active_link_count;
extern void link_start(struct link *);
extern int func_000CE44C(struct link **, struct link *, int);
extern char D_00175962[];
extern int link_step(struct link *);
extern int mc_memcpy();
extern char screen_buffer[];
extern char D_00147954[];
extern char D_0017596A[];
extern char D_001940DA[];
extern char quest_global_states[];
extern char D_00195798[];
extern char D_001957CD[];
extern char D_001957E9[];
extern char frame_counter[];
extern struct record *player_entity;
extern struct record *player_object;
extern char D_00195AB0[];
extern struct character *player_character;
extern char D_0019621B[];
extern char D_00196222[];
extern char D_00196226[];
extern char D_0019622A[];
extern char interaction_mode[];
extern char D_0019628C[];
extern char D_001A3A80[];
extern char D_001A3A81[];
extern int damage_apply(struct record *, int, int);
extern void msgbox_show_rsc(int, int);
extern int cast_creature_spell(struct record *, struct record *, int);
extern void link_show_text(int);
extern int link_answer_matches(int, int);
extern void link_hurt_player(int, int);
extern void func_00065748(short, unsigned char);
extern struct spell *func_00065864(int);
extern void disease_infect(struct record *, int, int, int);
extern void func_00065A8C(struct record *, int, int);
extern int sound_play(int, int, int);
extern int hud_message_add(int);
extern void hud_messages_draw(void);
extern int rand_range(int, int);
extern void inpstr_begin_text(int, short);
extern int inpstr_update(void);
extern void object_set_position(struct record *, int, int, int, int, int, int);
extern int door_start_swing(int, int);
extern int func_000CDD81();
extern int func_00142790();

void links_trigger(struct record *a1, int a2)
{
    int i;
    int j;
    int n;
    struct link *p;
    short id;

    i = 0;
    if (link_count == 0) return;
    id = a1->id;
    while (i < link_count) {
        if (D_00199D78[i].trigger != 0 && D_00199D78[i].object_id == id) {
            if (D_00199D78[i].trigger < 8 || D_00199D78[i].trigger > 9) {
                if (D_00199D78[i].trigger != a2) return;
            } else if (D_00199D78[i].trigger == 8) {
                if (a2 != 2 && a2 != 3 && a2 != 5 && a2 != 6)
                    return;
            } else {
                if (a2 != 2 && a2 != 3)
                    return;
            }
            p = &D_00199D78[i];
            if (func_000CE44C(active_links, p, active_link_count) != 0) return;
            active_links[active_link_count++] = p;
            n = p->chain_count + 1;
            for (j = 0; j < n; j++, p++)
                link_start(p);
        }
        i++;
    }
}

void links_update(void)
{
    int i;
    int n;
    int j;
    int sum;
    struct link *p;

    for (i = 0; i < active_link_count; i++) {
        p = active_links[i];
        n = p->chain_count + 1;
        sum = j = 0;
        for (; j < n; j++, p++) {
            sum += link_step(p);
            if ((p->flags & 16) != 0) {
                sum = 0;
                p->flags &= 239;
                break;
            }
        }
        if (sum == 0) {
            if (active_link_count - 1 > i)
                mc_memcpy(&active_links[i], &active_links[i + 1], (active_link_count - i) * 4 - 4, D_00175962, 187, 4);
            active_link_count--;
            i--;
        }
    }
}

int link_step(struct link *a1)
{
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;

    if (((int)(unsigned char)(a1->flags & 1)) == 0) goto L6483D;
    return 0;
L6483D:;
    if (a1->action == 18) goto L6485D;
    if (a1->action != 20) goto L6486E;
L6485D:;
    if (((int)(unsigned char)(a1->axis & 8)) != 0) goto L64870;
L6486E:;
    goto L64881;
L64870:;
    if (((int)(unsigned char)(a1->flags & 2)) != 0) goto L64883;
L64881:;
    goto L6488F;
L64883:;
    return 0;
L6488F:;
    a1->delta[0] = (a1->delta[1] = (a1->delta[2] = 0));
    if (((unsigned)(*(int *)((char *)1132) - a1->start_tick)) <= a1->duration) goto L6490F;
    a1->flags |= 1;
    a1->start_tick = *(int *)((char *)1132) - a1->duration;
    a1->flags ^= 2;
    func_00065748((int)(short)a1->object_id, a1->flags);
L6490F:;
    if (((int)(unsigned char)(a1->flags & 4)) != 0) goto L64956;
    if (a1->param == 0) goto L64932;
    if (a1->object != 0) goto L64934;
L64932:;
    goto L6494F;
L64934:;
    sound_play(a1->param, (int)a1->object, 110);
L6494F:;
    a1->flags |= 4;
L64956:;
    if (a1->object == 0) goto L64971;
    if (a1->object->type != 32) goto L64976;
L64971:;
    goto L64A1D;
L64976:;
    a1->object->move_frame = *(int *)frame_counter;
L64A1D:;
    switch (a1->action) {
case 129:
    a1->combination &= *(signed char *)D_001A3A81 | 240;
    a1->combination |= *(signed char *)D_001A3A80;
    if (((int)(unsigned char)(a1->combination & 15)) != (a1->combination >> 4)) goto L652AB;
case 1:
    l_40 = ((*(int *)((char *)1132) - a1->start_tick) * a1->speed) >> 16;
    switch ((unsigned char)(a1->axis - 1)) {
case 0:
    l_38 = l_40 + a1->start;
    a1->delta[0] = l_38 - (short)a1->object->x;
    a1->object->x = l_38;
    goto L64C01;
case 1:
    l_38 = a1->start - l_40;
    a1->delta[0] = l_38 - (short)a1->object->x;
    a1->object->x = l_38;
    goto L64C01;
case 2:
    l_38 = l_40 + a1->start;
    a1->delta[1] = l_38 - (short)a1->object->y;
    a1->object->y = l_38;
    goto L64C01;
case 3:
    l_38 = a1->start - l_40;
    a1->delta[1] = l_38 - (short)a1->object->y;
    a1->object->y = l_38;
    goto L64C01;
case 4:
    l_38 = l_40 + a1->start;
    a1->delta[2] = l_38 - (short)a1->object->z;
    a1->object->z = l_38;
    goto L64C01;
case 5:
    l_38 = a1->start - l_40;
    a1->delta[2] = l_38 - (short)a1->object->z;
    a1->object->z = l_38;
default:
L64C01:;
    if (a1->object == 0) goto L64C5C;
    if (a1->object->twin == 0) goto L64C5C;
    l_2C = (int)a1->object->twin->children;
    if (l_2C == 0) goto L64C5C;
    if (*(int *)((char *)l_2C + 51) == 0) goto L64C5C;
    mc_memcpy(*(int *)((char *)l_2C + 51) + 7, (int)(signed char *)&a1->object->x, 12, (int)D_00175962, 264, 4);
L64C5C:;
    goto L652AB;
}
case 130:
    a1->combination &= *(signed char *)D_001A3A81 | 240;
    a1->combination |= *(signed char *)D_001A3A80;
    if (((int)(unsigned char)(a1->combination & 15)) != (a1->combination >> 4)) goto L652AB;
case 8:
    l_40 = ((*(int *)((char *)1132) - a1->start_tick) * a1->speed) >> 16;
{
    int l_54;
    int l_50;
    l_50 = a1->axis - 1;
    switch (l_50) {
case 0:
    a1->object->angle_x = (l_40 + (short)a1->start) & 2047;
    goto L64D97;
case 1:
    a1->object->angle_x = (short)((short)a1->start - l_40) & 2047;
    goto L64D97;
case 2:
    a1->object->yaw = (l_40 + (short)a1->start) & 2047;
    goto L64D97;
case 3:
    a1->object->yaw = (short)((short)a1->start - l_40) & 2047;
    goto L64D97;
case 4:
    a1->object->angle_z = (l_40 + (short)a1->start) & 2047;
    goto L64D97;
case 5:
    a1->object->angle_z = (short)((short)a1->start - l_40) & 2047;
default:
L64D97:;
    goto L652AB;
}
case 9:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L64E69;
    *(int *)D_00195798 = 1000;
    *(signed char *)D_001957CD = player_character->level;
    l_40 = 0;
L64DD5:;
    if (l_40 < 35) goto L64DE5;
    goto L64DF4;
L64DDD:;
    l_40++;
    goto L64DD5;
L64DE5:;
    *(short *)(D_001957E9 + (l_40 * 6)) = 50;
    goto L64DDD;
L64DF4:;
    *(int *)D_00196222 = a1->object->x;
    *(int *)D_00196226 = a1->object->y - 40;
    *(int *)D_0019622A = a1->object->z;
    if (func_00065864(a1->param)->target != 0) goto L64E51;
    cast_creature_spell(player_entity, player_entity, a1->param);
    goto L64E69;
L64E51:;
    cast_creature_spell((struct record *)D_0019621B, player_entity, a1->param);
L64E69:;
    goto L652AB;
case 10:
    goto L652AB;
case 11:
    msgbox_show_rsc((int)(short)(((unsigned short)a1->param) + 8600), 1);
    goto L652AB;
case 12:
    mc_memcpy(*(int *)D_00147954, 655360, 64000, (int)D_00175962, 315, 4);
    *(signed char *)D_001940DA |= 1;
    link_show_text(a1->param + 5400);
    l_30 = hud_message_add((int)D_0017596A);
    *(signed char *)((char *)l_30 + 3) = 0;
    func_00142790();
    inpstr_begin_text(l_30 + 2, 16);
L64EF5:;
    if (inpstr_update() != 0) goto L64F30;
    mc_memcpy(*(int *)screen_buffer, *(int *)D_00147954, 64000, (int)D_00175962, 324, 4);
    hud_messages_draw();
    func_000CDD81(1);
    goto L64EF5;
L64F30:;
    *(signed char *)D_001940DA &= 254;
    if (link_answer_matches(a1->param + 5656, l_30 + 2) != 0) goto L64F5D;
    a1->flags |= 16;
L64F5D:;
    goto L652AB;
case 13:
    goto L652AB;
case 14:
    object_set_position(player_object, *(int *)(*(char **)((char *)a1 + 74) + 7), *(int *)(*(char **)((char *)a1 + 74) + 11), *(int *)(*(char **)((char *)a1 + 74) + 15), player_object->angle_x, player_object->yaw, player_object->angle_z);
    goto L652AB;
case 15:
    a1->object->lock_level = (unsigned short)a1->axis;
    goto L652AB;
case 16:
    if (((int)(unsigned short)(a1->object->flags & 64)) == 0) goto L64FEF;
    if (door_start_swing((int)a1->object, 0) != 0) goto L64FF1;
L64FEF:;
    goto L64FFB;
L64FF1:;
    a1->object->flags |= 0x100;
L64FFB:;
    goto L652AB;
case 17:
    a1->object->flags |= 64;
    goto L652AB;
case 18:
    if (door_start_swing((int)a1->object, 0) == 0) goto L6502C;
    a1->object->flags |= 320;
L6502C:;
    goto L652AB;
case 19:
    if (((int)(unsigned short)(a1->object->flags & 256)) == 0) goto L6505D;
    if (door_start_swing((int)a1->object, 1) != 0) goto L6505F;
L6505D:;
    goto L65069;
L6505F:;
    a1->object->flags &= ~0x100;
L65069:;
    goto L652AB;
case 20:
    if (((int)(unsigned short)(a1->object->flags & 256)) == 0) goto L6509A;
    if (door_start_swing((int)a1->object, 1) != 0) goto L6509C;
L6509A:;
    goto L650A6;
L6509C:;
    a1->object->flags &= ~0x100;
L650A6:;
    a1->object->flags &= ~0x40;
    goto L652AB;
case 21:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L65124;
    *(int *)D_00195798 = 1000;
    l_34 = rand_range(a1->param, a1->axis) * player_character->level;
    if (l_34 != 0) goto L65115;
    l_34 = player_character->level;
L65115:;
    damage_apply(player_entity, l_34, 0);
L65124:;
    goto L652AB;
case 22:
    link_hurt_player(3, a1->axis);
    goto L652AB;
case 23:
    link_hurt_player(0, a1->axis);
    goto L652AB;
case 24:
    link_hurt_player(1, a1->axis);
    goto L652AB;
case 25:
    link_hurt_player(2, a1->axis);
    goto L652AB;
case 26:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L651BE;
    *(int *)D_00195798 = 1000;
    func_00065A8C(player_entity, (int)&*(signed char *)((char *)rand_range(0, 11) + 128), 0);
L651BE:;
    goto L652AB;
case 27:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L65200;
    *(int *)D_00195798 = 1000;
    disease_infect(player_entity, 0, rand_range(0, 16), 0);
L65200:;
    goto L652AB;
case 28:
    if (a1->axis == 0) goto L65224;
    player_character->magicka -= (unsigned short)a1->axis;
    goto L65230;
L65224:;
    player_character->magicka--;
L65230:;
    goto L652AB;
case 29:
    goto L652AB;
case 30:
    if (a1->param == 0) goto L6525E;
    sound_play(a1->param, (int)a1->object, 110);
L6525E:;
    goto L652AB;
case 31:
    *(signed char *)(quest_global_states + a1->axis) = 1;
    goto L652AB;
case 99:
    if (((int)(unsigned char)*(signed char *)interaction_mode) != 1) goto L652A0;
    if (a1->param == 0) goto L6529E;
    link_show_text(a1->param + 7700);
L6529E:;
    goto L652AB;
L652A0:;
    *(signed char *)D_0019628C = a1->axis;
default:
L652AB:;
    l_3C = a1->axis >> 4;
    if (l_3C == 0) goto L652F8;
    *(signed char *)D_001A3A81 = ~(*(signed char *)&l_3C);
    if (((int)(unsigned char)(a1->flags & 2)) == 0) goto L652EA;
    l_54 = 0;
    goto L652F0;
L652EA:;
    l_54 = l_3C;
L652F0:;
    *(signed char *)D_001A3A80 = *(signed char *)&l_54;
L652F8:;
    return 1;
}
}
}
