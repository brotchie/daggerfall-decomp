/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000830C7 */
#include "records.h"

#pragma pack(1)
struct light {                  /* 17 bytes */
    int x, y, z;
    unsigned short snd;
    unsigned short f14;
    unsigned char f16;
};
struct anim {                   /* 66 bytes */
    unsigned short id;
    unsigned char rec;
    char pad3;
    int handle;
    char pad8[58];
};
struct flame {
    int handle;
    char pad4[8];
    char pos[20];
    int x, y, z;
    int f44, f48, f52;
};
struct stat15 { unsigned short f:15; };
#pragma pack()
extern int D_000C5404;
extern int xn_anim_ticks;
struct race { unsigned short flags; char pad[27]; };
extern struct race monster_table_flags[];
extern short frame_counter;
extern unsigned char current_climate;
extern int daylight;
extern char cfg_show_markers;
extern struct flame D_001A945E;
extern unsigned char D_001A949C;
extern char model_cache_flush_count;
extern void spell_area_effect(struct record *);
extern void weapon_missile_orient(struct record *);
extern int weapon_arrow_update(struct record *);
extern void object_free_later(struct record *);
extern int flat_anim_finished(struct record *);
extern void flat_anim_step(struct record *);
extern int model_get(unsigned short, int, int);
extern void flat_animal_sound(int, int, int, int, int);
extern int xn_anim_update();
extern int xn_rand_noise_2d();
extern int xn_model_compose_angles();
extern int xn_model_set_angles_yaw_offset();
extern int xn_tex_cache_lookup_image();
extern int xn_light_add();
extern int xn_model_submit();
extern int xn_flat_add();

int object_draw_cb(struct record *a1)
{
    struct flame *l_4C;
    unsigned char *l_48;
    struct light *l_44;
    struct anim *l_40;
    unsigned char *l_3C;
    struct monster_anim *l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    struct character *l_24;
    unsigned char *l_20;

    if (model_cache_flush_count != 0)
        return 1;
    if (a1->flags & 512)
        return 0;
    if (a1->type != 9)
        a1->draw_handle = 0;
    D_000C5404 = 0;
    switch (a1->type) {
    case 18:
        a1->angle_x = frame_counter;
        l_38 = &a1->data.monster.anim;
        xn_anim_update(l_38);
        a1->image = (a1->image & -128) | (l_38->anim_record + l_38->anim_facing);
        l_20 = (unsigned char *)xn_tex_cache_lookup_image(a1->image >> 7, a1->image & 127);
        l_38->frame_count = *(short *)(l_20 + 22);
        l_34 = a1->image >> 7;
        if (l_34 == 280 || l_34 == 281) {
            xn_light_add(a1->x, a1->y, a1->z, 31, 256, 0);
            l_30 = 4194304;
        } else {
            l_30 = 0;
        }
        if ((a1->image >> 7) == 268 && l_38->anim_frame >= 4)
            l_34 = 1;
        l_24 = &a1->data.character;
        if ((l_24->conditions & 4) == 0) {
            if ((monster_table_flags[l_24->race].flags & 1) && l_24->race != 29 && a1->y - 90 > l_24->ceiling_y)
                a1->draw_handle = xn_flat_add(a1->x, a1->y - 30, a1->z, a1->image, l_38->anim_frame, (l_38->anim_bits >> 10) & 32 | 4, l_30 + 256);
            else
                a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, l_38->anim_frame, (l_38->anim_bits >> 10) & 32 | 4, l_30 + 256);
        }
        break;
    case 42:
        if (flat_anim_finished(a1) != 0) {
            object_free_later(a1);
            break;
        }
        a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, a1->anim_frame, 1, 256);
        flat_anim_step(a1);
        break;
    case 53:
        a1->angle_x = frame_counter;
        if (a1->flags & 16384)
            l_34 = 36;
        else
            l_34 = 4;
        if ((a1->image & 127) >= 5)
            a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, (unsigned)(*(int *)0x46c & 32) >> 5, l_34, 256);
        else
            a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, -1, l_34, 256);
        break;
    case 8:
        a1->angle_x = frame_counter;
        if (a1->image == 0 || a1->image == 65535) {
            l_34 = 1;
            break;
        }
        a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, -1, 4, 256);
        break;
    case 2:
        if (a1->image2 == 998 && a1->image == 0) {
            l_4C = &D_001A945E;
            l_4C->handle = model_get(a1->image2, a1->image, (current_climate << 2) + D_001A949C);
            if (l_4C->handle != 0) {
                l_4C->x = a1->x;
                l_4C->y = a1->y;
                l_4C->z = a1->z;
                if (weapon_arrow_update(a1) == 0)
                    break;
                weapon_missile_orient(a1);
                l_4C->f44 = a1->missile_yaw;
                l_4C->f48 = a1->angle_z;
                l_4C->f52 = 0;
                xn_model_submit(l_4C, 0);
                break;
            }
        }
        a1->angle_x = frame_counter;
        if (a1->image == 0 || a1->image == 65535 || a1->image == 200)
            break;
        if ((a1->image >> 7) == 210)
            l_30 = 63;
        else
            l_30 = 0;
        a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, -1, 4, (l_30 << 16) + 256);
        break;
    case 34:
        if (cfg_show_markers == 0)
            break;
    case 33:
    case 44:
        a1->angle_x = frame_counter;
        if (a1->image == 0 || a1->image == 65535 || a1->image == 200)
            break;
        if ((a1->image >> 7) == 210)
            l_30 = 63;
        else
            l_30 = 0;
        a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, -1, 4, (l_30 << 16) + 256);
        break;
    case 9:
        if (a1->flags & 8192) {
            if (a1->image2 & 32768) {
                if (a1->image2 == 32768)
                    spell_area_effect(a1);
                l_3C = (unsigned char *)xn_tex_cache_lookup_image(a1->missile_texture >> 7, a1->missile_texture & 127);
                if ((int)((struct stat15 *)&a1->image2)->f >= (int)*(unsigned short *)(l_3C + 20)) {
                    a1->image2 = 36863;
                } else {
                    xn_flat_add(a1->x, a1->y, a1->z, a1->missile_texture, ((struct stat15 *)&a1->image2)->f, 1, 4129024);
                    a1->image2++;
                    a1->children->light_radius >>= 1;
                }
            } else {
                xn_flat_add(a1->x, a1->y, a1->z, a1->missile_texture, -1, 1, 256);
            }
        }
        break;
    case 7:
        if (a1->parent->type == 4) {
            a1->draw_handle = xn_light_add(a1->x, a1->y, a1->z, a1->image, 255, 0);
        } else {
            l_28 = xn_rand_noise_2d(a1->x ^ a1->z, (xn_anim_ticks / 40) << 7);
            l_28 >>= 3;
            l_28 = 256 - l_28;
            l_28 = (a1->light_radius * l_28) >> 8;
            a1->draw_handle = xn_light_add(a1->x, a1->y, a1->z, a1->image, l_28, 0);
        }
        break;
    case 43:
        l_48 = (unsigned char *)&a1->data;
        l_40 = *(struct anim **)(l_48 + 5);
        l_44 = *(struct light **)(l_48 + 9);
        for (l_34 = 0; l_48[0] > l_34; l_34++, l_40++) {
            l_40->handle = model_get(l_40->id, l_40->rec, (current_climate << 2) + D_001A949C);
            if (l_40->handle != 0)
                xn_model_submit(&l_40->handle, 0);
        }
        for (l_34 = 0; l_48[1] > l_34; l_34++, l_44++) {
            if (l_44->snd == 0 || l_44->snd == 65535)
                continue;
            if ((l_44->snd >> 7) == 199 && cfg_show_markers == 0)
                continue;
            if ((l_44->snd >> 7) == 210) {
                xn_light_add(l_44->x, l_44->y - 16, l_44->z, l_44->f14 & 255, l_44->f14 >> 8, 0);
                l_30 = 63;
            } else {
                l_30 = 0;
            }
            xn_flat_add(l_44->x, l_44->y, l_44->z, l_44->snd, -1, 4, (l_30 << 16) + 256);
        }
        break;
    case 6:
    case 32:
        if (a1->image2 == 0)
            break;
        l_4C = (struct flame *)&a1->data;
        l_4C->handle = model_get(a1->image2, a1->image, (current_climate << 2) + D_001A949C);
        if (l_4C->handle != 0) {
            l_4C->x = a1->x;
            l_4C->y = a1->y;
            l_4C->z = a1->z;
            if (a1->image2 == 998) {
                if (weapon_arrow_update(a1) == 0)
                    break;
                weapon_missile_orient(a1);
                l_4C->f44 = a1->missile_yaw;
                l_4C->f48 = a1->angle_z;
                l_4C->f52 = 0;
            } else if (a1->link_flag != 255) {
                xn_model_set_angles_yaw_offset(l_4C->pos, a1->wait_state);
            } else {
                if (a1->image2 == 610 && a1->image == 32)
                    D_000C5404 = 3;
                xn_model_compose_angles(l_4C->pos, a1->angle_x, a1->yaw + a1->wait_state, a1->angle_z);
            }
            xn_model_submit(l_4C, 0);
        }
        break;
    case 56:
        l_40 = (struct anim *)&a1->data;
        l_44 = (struct light *)(l_40 + a1->model_count);
        for (l_34 = 0; a1->model_count > l_34; l_34++, l_40++) {
            l_40->handle = model_get(l_40->id, l_40->rec, (current_climate << 2) + D_001A949C);
            if (l_40->handle != 0)
                xn_model_submit(&l_40->handle, 0);
        }
        for (l_34 = 0; a1->flat_count > l_34; l_34++, l_44++) {
            if (l_44->snd == 0 || l_44->snd == 65535)
                continue;
            if ((l_44->snd >> 7) == 199 && cfg_show_markers == 0)
                continue;
            if (l_44->f14 == 0) {
                if ((l_44->snd >> 7) == 210 && daylight == 0) {
                    l_28 = xn_rand_noise_2d(a1->x ^ a1->z, (xn_anim_ticks / 40) << 7);
                    l_28 >>= 3;
                    l_28 = 256 - l_28;
                    l_30 = (l_28 * 225) >> 8;
                    l_28 = l_30;
                    xn_light_add(l_44->x, l_44->y - l_44->f16 * 3, l_44->z, 64, l_28, 0);
                } else {
                    l_30 = 0;
                }
                xn_flat_add(l_44->x, l_44->y, l_44->z, l_44->snd, -1, 4, (l_30 << 16) + 256);
                flat_animal_sound(l_44->x, l_44->y, l_44->z, l_44->snd >> 7, l_44->snd & 127);
            }
        }
        break;
    }
    return 0;
}
