/* matched by the real Watcom C32 10.0a (-d2): a run of links.c from 0x00064589 to 0x0006480F, kept together for its switch table's alignment */
#include "records.h"

extern struct link links[];
extern struct link *active_links[];
extern int link_count;
extern int active_link_count;
extern void link_start(struct link *);
extern int xn_str_find_u32(struct link **, struct link *, int);
extern char D_00175962[];
extern int link_step(struct link *);
extern int mc_memcpy();
extern int screen_buffer;
extern int D_00147954;
extern char D_0017596A[];
extern signed char D_001940DA;
extern signed char quest_global_states[];
extern int D_00195798;
extern signed char D_001957CD;
extern char D_001957E9[];
extern char frame_counter[];
extern struct record *player_entity;
extern struct record *player_object;
extern int frame_ticks;
extern struct character *player_character;
extern signed char D_0019621B;
extern int D_00196222;
extern int D_00196226;
extern int D_0019622A;
extern unsigned char interaction_mode;
extern signed char D_0019628C;
extern signed char D_001A3A80;
extern signed char D_001A3A81;
extern int damage_apply(struct record *, int, int);
extern void msgbox_show_rsc(int, int);
extern int cast_creature_spell(struct record *, struct record *, int);
extern void link_show_text(int);
extern int link_answer_matches(int, int);
extern void link_hurt_player(int, int);
extern void links_set_reverse(short, unsigned char);
extern struct spell *link_find_spell(int);
extern void disease_infect(struct record *, int, int, int);
extern void poison_apply(struct record *, int, int);
extern int sound_play(int, int, int);
extern int hud_message_add(int);
extern void hud_messages_draw(void);
extern int rand_range(int, int);
extern void inpstr_begin_text(int, short);
extern int inpstr_update(void);
extern void object_set_position(struct record *, int, int, int, int, int, int);
extern int door_start_swing(int, int);
extern int xn_gfx_present_inclusive();
extern int xn_kbd_flush();

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
        if (links[i].trigger != 0 && links[i].object_id == id) {
            if (links[i].trigger < 8 || links[i].trigger > 9) {
                if (links[i].trigger != a2) return;
            } else if (links[i].trigger == 8) {
                if (a2 != 2 && a2 != 3 && a2 != 5 && a2 != 6)
                    return;
            } else {
                if (a2 != 2 && a2 != 3)
                    return;
            }
            p = &links[i];
            if (xn_str_find_u32(active_links, p, active_link_count) != 0) return;
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

    if (((int)(unsigned char)(a1->flags & 1)) != 0) return 0;
    if ((a1->action == 18 || a1->action == 20) && ((int)(unsigned char)(a1->axis & 8)) != 0 && ((int)(unsigned char)(a1->flags & 2)) != 0) {
        return 0;
    }
    a1->delta[0] = (a1->delta[1] = (a1->delta[2] = 0));
    if (((unsigned)(*(int *)((char *)1132) - a1->start_tick)) > a1->duration) {
        a1->flags |= 1;
        a1->start_tick = *(int *)((char *)1132) - a1->duration;
        a1->flags ^= 2;
        links_set_reverse((int)(short)a1->object_id, a1->flags);
    }
    if (((int)(unsigned char)(a1->flags & 4)) == 0) {
        if (a1->param != 0 && a1->object != 0) sound_play(a1->param, (int)a1->object, 110);
        a1->flags |= 4;
    }
    if (a1->object != 0 && a1->object->type != 32) a1->object->move_frame = *(int *)frame_counter;
    {
        int l_54;
        int l_50;
        switch (a1->action) {
        case 129:
            a1->combination &= D_001A3A81 | 240;
            a1->combination |= D_001A3A80;
            if (((int)(unsigned char)(a1->combination & 15)) != (a1->combination >> 4)) break;
        case 1:
            l_40 = ((*(int *)((char *)1132) - a1->start_tick) * a1->speed) >> 16;
            switch ((unsigned char)(a1->axis - 1)) {
            case 0:
                l_38 = l_40 + a1->start;
                a1->delta[0] = l_38 - (short)a1->object->x;
                a1->object->x = l_38;
                break;
            case 1:
                l_38 = a1->start - l_40;
                a1->delta[0] = l_38 - (short)a1->object->x;
                a1->object->x = l_38;
                break;
            case 2:
                l_38 = l_40 + a1->start;
                a1->delta[1] = l_38 - (short)a1->object->y;
                a1->object->y = l_38;
                break;
            case 3:
                l_38 = a1->start - l_40;
                a1->delta[1] = l_38 - (short)a1->object->y;
                a1->object->y = l_38;
                break;
            case 4:
                l_38 = l_40 + a1->start;
                a1->delta[2] = l_38 - (short)a1->object->z;
                a1->object->z = l_38;
                break;
            case 5:
                l_38 = a1->start - l_40;
                a1->delta[2] = l_38 - (short)a1->object->z;
                a1->object->z = l_38;
            }
            if (a1->object != 0) {
                if (a1->object->twin != 0) {
                    l_2C = (int)a1->object->twin->children;
                    if (l_2C != 0) {
                        if (*(int *)((char *)l_2C + 51) != 0) {
                            mc_memcpy(*(int *)((char *)l_2C + 51) + 7, (int)(signed char *)&a1->object->x, 12, (int)D_00175962, 264, 4);
                        }
                    }
                }
            }
            break;
        case 130:
            a1->combination &= D_001A3A81 | 240;
            a1->combination |= D_001A3A80;
            if (((int)(unsigned char)(a1->combination & 15)) != (a1->combination >> 4)) break;
        case 8:
            l_40 = ((*(int *)((char *)1132) - a1->start_tick) * a1->speed) >> 16;
            l_50 = a1->axis - 1;
            switch (l_50) {
            case 0:
                a1->object->angle_x = (l_40 + (short)a1->start) & 2047;
                break;
            case 1:
                a1->object->angle_x = (short)((short)a1->start - l_40) & 2047;
                break;
            case 2:
                a1->object->yaw = (l_40 + (short)a1->start) & 2047;
                break;
            case 3:
                a1->object->yaw = (short)((short)a1->start - l_40) & 2047;
                break;
            case 4:
                a1->object->angle_z = (l_40 + (short)a1->start) & 2047;
                break;
            case 5:
                a1->object->angle_z = (short)((short)a1->start - l_40) & 2047;
            }
            break;
        case 9:
            D_00195798 -= frame_ticks;
            if (D_00195798 <= 0) {
                D_00195798 = 1000;
                D_001957CD = player_character->level;
                for (l_40 = 0; l_40 < 35; l_40++) {
                    *(short *)(D_001957E9 + (l_40 * 6)) = 50;
                }
                D_00196222 = a1->object->x;
                D_00196226 = a1->object->y - 40;
                D_0019622A = a1->object->z;
                if (link_find_spell(a1->param)->target == 0) {
                    cast_creature_spell(player_entity, player_entity, a1->param);
                } else {
                    cast_creature_spell((struct record *)&D_0019621B, player_entity, a1->param);
                }
            }
            break;
        case 10:
            break;
        case 11:
            msgbox_show_rsc((int)(short)(((unsigned short)a1->param) + 8600), 1);
            break;
        case 12:
            mc_memcpy(D_00147954, 655360, 64000, (int)D_00175962, 315, 4);
            D_001940DA |= 1;
            link_show_text(a1->param + 5400);
            l_30 = hud_message_add((int)D_0017596A);
            *(signed char *)((char *)l_30 + 3) = 0;
            xn_kbd_flush();
            inpstr_begin_text(l_30 + 2, 16);
            while (inpstr_update() == 0) {
                mc_memcpy(screen_buffer, D_00147954, 64000, (int)D_00175962, 324, 4);
                hud_messages_draw();
                xn_gfx_present_inclusive(1);
            }
            D_001940DA &= 254;
            if (link_answer_matches(a1->param + 5656, l_30 + 2) == 0) a1->flags |= 16;
            break;
        case 13:
            break;
        case 14:
            object_set_position(player_object, *(int *)(*(char **)((char *)a1 + 74) + 7), *(int *)(*(char **)((char *)a1 + 74) + 11), *(int *)(*(char **)((char *)a1 + 74) + 15), player_object->angle_x, player_object->yaw, player_object->angle_z);
            break;
        case 15:
            a1->object->lock_level = (unsigned short)a1->axis;
            break;
        case 16:
            if (((int)(unsigned short)(a1->object->flags & 64)) != 0 && door_start_swing((int)a1->object, 0) != 0) {
                a1->object->flags |= 0x100;
            }
            break;
        case 17:
            a1->object->flags |= 64;
            break;
        case 18:
            if (door_start_swing((int)a1->object, 0) != 0) a1->object->flags |= 320;
            break;
        case 19:
            if (((int)(unsigned short)(a1->object->flags & 256)) != 0 && door_start_swing((int)a1->object, 1) != 0) {
                a1->object->flags &= ~0x100;
            }
            break;
        case 20:
            if (((int)(unsigned short)(a1->object->flags & 256)) != 0 && door_start_swing((int)a1->object, 1) != 0) {
                a1->object->flags &= ~0x100;
            }
            a1->object->flags &= ~0x40;
            break;
        case 21:
            D_00195798 -= frame_ticks;
            if (D_00195798 <= 0) {
                D_00195798 = 1000;
                l_34 = rand_range(a1->param, a1->axis) * player_character->level;
                if (l_34 == 0) l_34 = player_character->level;
                damage_apply(player_entity, l_34, 0);
            }
            break;
        case 22:
            link_hurt_player(3, a1->axis);
            break;
        case 23:
            link_hurt_player(0, a1->axis);
            break;
        case 24:
            link_hurt_player(1, a1->axis);
            break;
        case 25:
            link_hurt_player(2, a1->axis);
            break;
        case 26:
            D_00195798 -= frame_ticks;
            if (D_00195798 <= 0) {
                D_00195798 = 1000;
                poison_apply(player_entity, (int)&*(signed char *)((char *)rand_range(0, 11) + 128), 0);
            }
            break;
        case 27:
            D_00195798 -= frame_ticks;
            if (D_00195798 <= 0) {
                D_00195798 = 1000;
                disease_infect(player_entity, 0, rand_range(0, 16), 0);
            }
            break;
        case 28:
            if (a1->axis != 0) {
                player_character->magicka -= (unsigned short)a1->axis;
            } else {
                player_character->magicka--;
            }
            break;
        case 29:
            break;
        case 30:
            if (a1->param != 0) sound_play(a1->param, (int)a1->object, 110);
            break;
        case 31:
            quest_global_states[a1->axis] = 1;
            break;
        case 99:
            if (((int)interaction_mode) == 1) {
                if (a1->param != 0) link_show_text(a1->param + 7700);
            } else {
                D_0019628C = a1->axis;
            }
        }
        l_3C = a1->axis >> 4;
        if (l_3C != 0) {
            D_001A3A81 = ~(*(signed char *)&l_3C);
            if (((int)(unsigned char)(a1->flags & 2)) != 0) {
                l_54 = 0;
            } else {
                l_54 = l_3C;
            }
            D_001A3A80 = *(signed char *)&l_54;
        }
        return 1;
    }
}
