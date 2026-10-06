/* matched by the real Watcom C32 10.0a (-d2): a run of links.c from 0x00064589 to 0x0006480F, kept together for its switch table's alignment */
#include "records.h"

extern struct link links[];
extern struct link *active_links[];
extern int link_count;
extern int active_link_count;
extern void link_start(struct link *);
extern iptr xn_str_find_u32(struct link **, struct link *, int);
extern char D_00175962[];
extern int link_step(struct link *);
extern int mc_memcpy();
extern iptr screen_buffer;
extern iptr D_00147954;
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
extern int damage_apply(struct record *, int, struct record *);
extern void msgbox_show_rsc(int, int);
extern int cast_creature_spell(struct record *, struct record *, int);
extern void link_show_text(int);
extern int link_answer_matches(int, iptr);
extern void link_hurt_player(int, int);
extern void links_set_reverse(short, int);
extern struct spell *link_find_spell(int);
extern void disease_infect(struct record *, unsigned char *, int, int);
extern void poison_apply(struct record *, int, int);
extern int sound_play(int, iptr, int);
extern iptr hud_message_add(char *);
extern void hud_messages_draw(void);
extern int rand_range(int, int);
extern void inpstr_begin_text(iptr, short);
extern int inpstr_update(void);
extern void object_set_position(struct record *, int, int, int, int, int, int);
extern int door_start_swing(iptr, int);
extern int xn_gfx_present_inclusive();
extern int xn_kbd_flush();

void links_trigger(struct record *object, int trigger)
{
    int i;
    int j;
    int n;
    struct link *p;
    short id;

    i = 0;
    if (link_count == 0) return;
    id = object->id;
    while (i < link_count) {
        if (links[i].trigger != 0 && links[i].object_id == id) {
            if (links[i].trigger < 8 || links[i].trigger > 9) {
                if (links[i].trigger != trigger) return;
            } else if (links[i].trigger == 8) {
                if (trigger != 2 && trigger != 3 && trigger != 5 && trigger != 6)
                    return;
            } else {
                if (trigger != 2 && trigger != 3)
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

int link_step(struct link *link)
{
    int offset;
    int bits;
    int pos;
    int damage;
    iptr message;
    struct record *child;

    if (((int)(unsigned char)(link->flags & 1)) != 0) return 0;
    if ((link->action == 18 || link->action == 20) && ((int)(unsigned char)(link->axis & 8)) != 0 && ((int)(unsigned char)(link->flags & 2)) != 0) {
        return 0;
    }
    link->delta[0] = (link->delta[1] = (link->delta[2] = 0));
    if (((unsigned)(BIOS_TICKS - link->start_tick)) > link->duration) {
        link->flags |= 1;
        link->start_tick = BIOS_TICKS - link->duration;
        link->flags ^= 2;
        links_set_reverse((int)(short)link->object_id, link->flags);
    }
    if (((int)(unsigned char)(link->flags & 4)) == 0) {
        if (link->param != 0 && link->object != 0) sound_play(link->param, (iptr)link->object, 110);
        link->flags |= 4;
    }
    if (link->object != 0 && link->object->type != 32) link->object->move_frame = *(int *)frame_counter;
    {
        int value;
        int axis;
        switch (link->action) {
        case 129:
            link->combination &= D_001A3A81 | 240;
            link->combination |= D_001A3A80;
            if (((int)(unsigned char)(link->combination & 15)) != (link->combination >> 4)) break;
        case 1:
            offset = ((BIOS_TICKS - link->start_tick) * link->speed) >> 16;
            switch ((unsigned char)(link->axis - 1)) {
            case 0:
                pos = offset + link->start;
                link->delta[0] = pos - (short)link->object->x;
                link->object->x = pos;
                break;
            case 1:
                pos = link->start - offset;
                link->delta[0] = pos - (short)link->object->x;
                link->object->x = pos;
                break;
            case 2:
                pos = offset + link->start;
                link->delta[1] = pos - (short)link->object->y;
                link->object->y = pos;
                break;
            case 3:
                pos = link->start - offset;
                link->delta[1] = pos - (short)link->object->y;
                link->object->y = pos;
                break;
            case 4:
                pos = offset + link->start;
                link->delta[2] = pos - (short)link->object->z;
                link->object->z = pos;
                break;
            case 5:
                pos = link->start - offset;
                link->delta[2] = pos - (short)link->object->z;
                link->object->z = pos;
            }
            if (link->object != 0) {
                if (link->object->twin != 0) {
                    child = link->object->twin->children;
                    if (child != 0) {
                        if (child->twin != 0) {
                            mc_memcpy(&child->twin->x, &link->object->x, 12, (iptr)D_00175962, 264, 4);
                        }
                    }
                }
            }
            break;
        case 130:
            link->combination &= D_001A3A81 | 240;
            link->combination |= D_001A3A80;
            if (((int)(unsigned char)(link->combination & 15)) != (link->combination >> 4)) break;
        case 8:
            offset = ((BIOS_TICKS - link->start_tick) * link->speed) >> 16;
            axis = link->axis - 1;
            switch (axis) {
            case 0:
                link->object->angle_x = (offset + (short)link->start) & 2047;
                break;
            case 1:
                link->object->angle_x = (short)((short)link->start - offset) & 2047;
                break;
            case 2:
                link->object->yaw = (offset + (short)link->start) & 2047;
                break;
            case 3:
                link->object->yaw = (short)((short)link->start - offset) & 2047;
                break;
            case 4:
                link->object->angle_z = (offset + (short)link->start) & 2047;
                break;
            case 5:
                link->object->angle_z = (short)((short)link->start - offset) & 2047;
            }
            break;
        case 9:
            D_00195798 -= frame_ticks;
            if (D_00195798 <= 0) {
                D_00195798 = 1000;
                D_001957CD = player_character->level;
                for (offset = 0; offset < 35; offset++) {
                    *(short *)(D_001957E9 + (offset * 6)) = 50;
                }
                D_00196222 = link->object->x;
                D_00196226 = link->object->y - 40;
                D_0019622A = link->object->z;
                if (link_find_spell(link->param)->target == 0) {
                    cast_creature_spell(player_entity, player_entity, link->param);
                } else {
                    cast_creature_spell((struct record *)&D_0019621B, player_entity, link->param);
                }
            }
            break;
        case 10:
            break;
        case 11:
            msgbox_show_rsc((int)(short)(((unsigned short)link->param) + 8600), 1);
            break;
        case 12:
            mc_memcpy(D_00147954, 655360, 64000, (iptr)D_00175962, 315, 4);
            D_001940DA |= 1;
            link_show_text(link->param + 5400);
            message = hud_message_add(D_0017596A);
            ((char *)message)[3] = 0;
            xn_kbd_flush();
            inpstr_begin_text(message + 2, 16);
            while (inpstr_update() == 0) {
                mc_memcpy(screen_buffer, D_00147954, 64000, (iptr)D_00175962, 324, 4);
                hud_messages_draw();
                xn_gfx_present_inclusive(1);
            }
            D_001940DA &= 254;
            if (link_answer_matches(link->param + 5656, message + 2) == 0) link->flags |= 16;
            break;
        case 13:
            break;
        case 14:
            object_set_position(player_object, link[1].object->x, link[1].object->y, link[1].object->z, player_object->angle_x, player_object->yaw, player_object->angle_z);
            break;
        case 15:
            link->object->lock_level = (unsigned short)link->axis;
            break;
        case 16:
            if (((int)(unsigned short)(link->object->flags & 64)) != 0 && door_start_swing((iptr)link->object, 0) != 0) {
                link->object->flags |= 0x100;
            }
            break;
        case 17:
            link->object->flags |= 64;
            break;
        case 18:
            if (door_start_swing((iptr)link->object, 0) != 0) link->object->flags |= 320;
            break;
        case 19:
            if (((int)(unsigned short)(link->object->flags & 256)) != 0 && door_start_swing((iptr)link->object, 1) != 0) {
                link->object->flags &= ~0x100;
            }
            break;
        case 20:
            if (((int)(unsigned short)(link->object->flags & 256)) != 0 && door_start_swing((iptr)link->object, 1) != 0) {
                link->object->flags &= ~0x100;
            }
            link->object->flags &= ~0x40;
            break;
        case 21:
            D_00195798 -= frame_ticks;
            if (D_00195798 <= 0) {
                D_00195798 = 1000;
                damage = rand_range(link->param, link->axis) * player_character->level;
                if (damage == 0) damage = player_character->level;
                damage_apply(player_entity, damage, 0);
            }
            break;
        case 22:
            link_hurt_player(3, link->axis);
            break;
        case 23:
            link_hurt_player(0, link->axis);
            break;
        case 24:
            link_hurt_player(1, link->axis);
            break;
        case 25:
            link_hurt_player(2, link->axis);
            break;
        case 26:
            D_00195798 -= frame_ticks;
            if (D_00195798 <= 0) {
                D_00195798 = 1000;
                poison_apply(player_entity, (int)(iptr)&*(signed char *)((char *)(iptr)rand_range(0, 11) + 128), 0);
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
            if (link->axis != 0) {
                player_character->magicka -= (unsigned short)link->axis;
            } else {
                player_character->magicka--;
            }
            break;
        case 29:
            break;
        case 30:
            if (link->param != 0) sound_play(link->param, (iptr)link->object, 110);
            break;
        case 31:
            quest_global_states[link->axis] = 1;
            break;
        case 99:
            if (((int)interaction_mode) == 1) {
                if (link->param != 0) link_show_text(link->param + 7700);
            } else {
                D_0019628C = link->axis;
            }
        }
        bits = link->axis >> 4;
        if (bits != 0) {
            D_001A3A81 = ~(*(signed char *)&bits);
            if (((int)(unsigned char)(link->flags & 2)) != 0) {
                value = 0;
            } else {
                value = bits;
            }
            D_001A3A80 = *(signed char *)&value;
        }
        return 1;
    }
}
