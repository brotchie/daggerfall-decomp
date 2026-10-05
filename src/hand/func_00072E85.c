/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00072E85 */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern int D_001343C0;
extern char D_00176184[];
extern short cast_anim_state;
extern char D_001875B7[];
extern signed char D_001940D6;
extern signed char D_001940DA;
extern signed char player_motion_flags;
extern int D_0019597C[];
extern struct record *player_entity;
extern struct record *player_object;
extern int D_00195AB0;
extern struct record *spell_ready_missile;
extern char hud_bar_image[];
extern int D_00195B80;
extern struct character *player_character;
extern struct settings *game_settings;
extern signed char weapon_active_hand;
extern signed char D_00196272;
extern char D_001A4A30[];
extern int D_001A4A38[];
extern int D_001A4A48[];
extern int D_001A4A50[];
extern int D_001A4A54;
extern int weapon_hand_cif[];
extern int D_001A4A60[];
extern char D_001A4A68[];
extern int D_001A4A70[];
extern char D_001AA600[];
extern void fatigue_add(int);
extern int weapon_start_swing(int);
extern void weapon_bow_update(void);
extern void weapon_melee_strike(struct record *);
extern void hud_status_set(int);
extern int inv_take_arrow(int);
extern int func_000CB381();
extern int func_000CB39A();
extern int func_000CB473();

void weapon_player_update(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (D_0019597C[((int)(unsigned char)weapon_active_hand)] != 0) return;
    if ((player_character->conditions & 0x1) != 0) return;
    if (cast_anim_state >= 0 || (int)spell_ready_missile != 0 || D_00196272 != 0 || ((struct bf8_6_1 *)&D_001940DA)->f != 0) {
        return;
    }
    if (weapon_hand_cif[((int)(unsigned char)weapon_active_hand)] == 0) return;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) return;
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) return;
    if (((int)D_001875B7) == *(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2))) {
        if (inv_take_arrow(0) == 0) {
            hud_status_set((int)D_00176184);
            D_001940D6 &= 191;
            return;
        }
        weapon_bow_update();
        return;
    }
    if (*(int *)(D_001A4A30 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0) {
        D_00195B80 = (int)(*(char **)D_001AA600 + (((int)(unsigned char)*(signed char *)(*(char **)(D_001A4A30 + (((int)(unsigned char)weapon_active_hand) << 2)) + 127)) << 8));
    }
    if (((struct bf8_6_1 *)&D_001940D6)->f != 0 && D_001A4A70[((int)(unsigned char)weapon_active_hand)] == 0) {
        l_1C = weapon_hand_cif[((int)(unsigned char)weapon_active_hand)];
        if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
            l_20 = 0;
        } else {
            l_20 = (int)(unsigned short)*(short *)(*(char **)hud_bar_image + 6);
        }
        func_000CB473(l_1C, -l_20);
    }
    if (D_001A4A70[((int)(unsigned char)weapon_active_hand)] == 0) {
        D_001A4A50[((int)(unsigned char)weapon_active_hand)] = 1;
        if (D_001A4A50[((int)(unsigned char)weapon_active_hand)] < 0) {
            D_001A4A50[((int)(unsigned char)weapon_active_hand)] = 0;
        }
        weapon_start_swing((int)(unsigned char)weapon_active_hand);
        D_00195B80 = *(int *)D_001AA600;
        return;
    }
    if (D_001A4A50[0] > 0) D_001A4A50[0] -= D_00195AB0;
    if (D_001A4A54 > 0) D_001A4A54 -= D_00195AB0;
    if (*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0) {
        func_000CB39A(func_000CB381(weapon_hand_cif[((int)(unsigned char)weapon_active_hand)], D_001A4A38[((int)(unsigned char)weapon_active_hand)]), (int)(unsigned char)*(signed char *)((char *)(*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) + D_001A4A60[((int)(unsigned char)weapon_active_hand)])), ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 0 : -((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 6))), (int)(unsigned char)weapon_active_hand);
    } else {
        func_000CB39A(func_000CB381(weapon_hand_cif[((int)(unsigned char)weapon_active_hand)], D_001A4A38[((int)(unsigned char)weapon_active_hand)]), 5 - D_001A4A70[((int)(unsigned char)weapon_active_hand)], ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 0 : -((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 6))), (int)(unsigned char)weapon_active_hand);
    }
    if (D_001A4A50[((int)(unsigned char)weapon_active_hand)] <= 0) {
        D_001A4A50[((int)(unsigned char)weapon_active_hand)] = (115 - player_character->attributes[6]) * 3;
        D_001A4A48[((int)(unsigned char)weapon_active_hand)] = D_001343C0;
        (D_001A4A70[((int)(unsigned char)weapon_active_hand)])--;
        (D_001A4A60[((int)(unsigned char)weapon_active_hand)])++;
        if (*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0) {
            if (((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) + D_001A4A60[((int)(unsigned char)weapon_active_hand)]))) == 255) {
                D_001A4A70[((int)(unsigned char)weapon_active_hand)] = 0;
            } else {
                D_001A4A70[((int)(unsigned char)weapon_active_hand)] = 2;
            }
        }
        if ((*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0 && ((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) + D_001A4A60[((int)(unsigned char)weapon_active_hand)]))) == 4) || (*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) == 0 && D_001A4A70[((int)(unsigned char)weapon_active_hand)] == 3)) {
            player_entity->x = player_object->x;
            player_entity->y = player_object->y;
            player_entity->z = player_object->z;
            player_entity->angle_x = player_object->angle_x;
            player_entity->yaw = player_object->yaw;
            player_entity->angle_z = player_object->angle_z;
            weapon_melee_strike(player_entity);
            fatigue_add(-11);
        }
    }
    D_00195B80 = *(int *)D_001AA600;
}
