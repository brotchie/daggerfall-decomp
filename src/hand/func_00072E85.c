/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00072E85 */
#include "records.h"
#include "bitfield.h"

extern int xn_anim_ticks;
extern char D_00176184[];
extern short cast_anim_state;
extern char D_001875B7[];
extern signed char D_001940D6;
extern signed char D_001940DA;
extern signed char player_motion_flags;
extern int D_0019597C[];
extern struct record *player_entity;
extern struct record *player_object;
extern int frame_ticks;
extern struct record *spell_ready_missile;
extern struct image *hud_bar_image;
extern iptr D_00195B80;
extern struct character *player_character;
extern struct settings *game_settings;
extern signed char weapon_active_hand;
extern signed char D_00196272;
extern char D_001A4A30[];
extern int D_001A4A38[];
extern int D_001A4A48[];
extern int D_001A4A50[];
extern int D_001A4A54;
extern iptr weapon_hand_cif[];
extern int D_001A4A60[];
extern char D_001A4A68[];
extern int D_001A4A70[];
extern char color_remap_tables[];
extern void fatigue_add(int);
extern int weapon_start_swing(int);
extern void weapon_bow_update(void);
extern void weapon_melee_strike(struct record *);
extern void hud_status_set(char *);
extern int inv_take_arrow(int);
extern int xn_img_cif_group();
extern int xn_draw_cif_rle_frame();
extern int xn_draw_img_masked_remap();

void weapon_player_update(void)
{
    int bar_height;
    iptr cif;
    int unused;

    if (D_0019597C[((int)(unsigned char)weapon_active_hand)] != 0) return;
    if ((player_character->conditions & 0x1) != 0) return;
    if (cast_anim_state >= 0 || (iptr)spell_ready_missile != 0 || D_00196272 != 0 || ((struct bf8_6_1 *)&D_001940DA)->f != 0) {
        return;
    }
    if (weapon_hand_cif[((int)(unsigned char)weapon_active_hand)] == 0) return;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) return;
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) return;
    if (((iptr)D_001875B7) == *(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2))) {
        if (inv_take_arrow(0) == 0) {
            hud_status_set(D_00176184);
            D_001940D6 &= 191;
            return;
        }
        weapon_bow_update();
        return;
    }
    if (*(int *)(D_001A4A30 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0) {
        D_00195B80 = (iptr)(*(char **)color_remap_tables + (((int)(unsigned char)*(signed char *)(*(char **)(D_001A4A30 + (((int)(unsigned char)weapon_active_hand) << 2)) + 127)) << 8));
    }
    if (((struct bf8_6_1 *)&D_001940D6)->f != 0 && D_001A4A70[((int)(unsigned char)weapon_active_hand)] == 0) {
        cif = weapon_hand_cif[((int)(unsigned char)weapon_active_hand)];
        if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
            bar_height = 0;
        } else {
            bar_height = hud_bar_image->height;
        }
        xn_draw_img_masked_remap(cif, -bar_height);
    }
    if (D_001A4A70[((int)(unsigned char)weapon_active_hand)] == 0) {
        D_001A4A50[((int)(unsigned char)weapon_active_hand)] = 1;
        if (D_001A4A50[((int)(unsigned char)weapon_active_hand)] < 0) {
            D_001A4A50[((int)(unsigned char)weapon_active_hand)] = 0;
        }
        weapon_start_swing((int)(unsigned char)weapon_active_hand);
        D_00195B80 = *(int *)color_remap_tables;
        return;
    }
    if (D_001A4A50[0] > 0) D_001A4A50[0] -= frame_ticks;
    if (D_001A4A54 > 0) D_001A4A54 -= frame_ticks;
    if (*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0) {
        xn_draw_cif_rle_frame(xn_img_cif_group(weapon_hand_cif[((int)(unsigned char)weapon_active_hand)], D_001A4A38[((int)(unsigned char)weapon_active_hand)]), (int)(unsigned char)*(signed char *)((char *)(*(iptr *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) + D_001A4A60[((int)(unsigned char)weapon_active_hand)])), ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 0 : -hud_bar_image->height), (int)(unsigned char)weapon_active_hand);
    } else {
        xn_draw_cif_rle_frame(xn_img_cif_group(weapon_hand_cif[((int)(unsigned char)weapon_active_hand)], D_001A4A38[((int)(unsigned char)weapon_active_hand)]), 5 - D_001A4A70[((int)(unsigned char)weapon_active_hand)], ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 0 : -hud_bar_image->height), (int)(unsigned char)weapon_active_hand);
    }
    if (D_001A4A50[((int)(unsigned char)weapon_active_hand)] <= 0) {
        D_001A4A50[((int)(unsigned char)weapon_active_hand)] = (115 - player_character->attributes[6]) * 3;
        D_001A4A48[((int)(unsigned char)weapon_active_hand)] = xn_anim_ticks;
        (D_001A4A70[((int)(unsigned char)weapon_active_hand)])--;
        (D_001A4A60[((int)(unsigned char)weapon_active_hand)])++;
        if (*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0) {
            if (((int)(unsigned char)*(signed char *)((char *)(iptr)(*(char **)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) + D_001A4A60[((int)(unsigned char)weapon_active_hand)]))) == 255) {
                D_001A4A70[((int)(unsigned char)weapon_active_hand)] = 0;
            } else {
                D_001A4A70[((int)(unsigned char)weapon_active_hand)] = 2;
            }
        }
        if ((*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) != 0 && ((int)(unsigned char)*(signed char *)((char *)(iptr)(*(char **)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) + D_001A4A60[((int)(unsigned char)weapon_active_hand)]))) == 4) || (*(int *)(D_001A4A68 + (((int)(unsigned char)weapon_active_hand) << 2)) == 0 && D_001A4A70[((int)(unsigned char)weapon_active_hand)] == 3)) {
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
    D_00195B80 = *(int *)color_remap_tables;
}
