/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000830C7 */
#include "records.h"
#include "doslow.h"

#pragma pack(1)
struct stat15 { unsigned short f:15; };
#pragma pack()
extern int D_000C5404;
extern int xn_anim_ticks;
extern struct monster_template monster_table[];
extern short frame_counter;
extern unsigned char current_climate;
extern int daylight;
extern char cfg_show_markers;
extern struct model_instance D_001A945E;
extern unsigned char D_001A949C;
extern char model_cache_flush_count;
extern void spell_area_effect(struct record *);
extern void weapon_missile_orient(struct record *);
extern int weapon_arrow_update(struct record *);
extern void object_free_later(struct record *);
extern int flat_anim_finished(struct record *);
extern void flat_anim_step(struct record *);
extern iptr model_get(int, int, int);
extern void flat_animal_sound(int, int, int, int, int);
extern void xn_anim_update(void *);
extern int xn_rand_noise_2d(unsigned, unsigned);
extern void xn_model_compose_angles(short *, int, int, int);
extern void xn_model_set_angles_yaw_offset(short *, int);
extern void *xn_tex_cache_lookup_image(int, int);
extern int xn_light_add(int, int, int, int, int, int);
extern void xn_model_submit(void *, int);
extern iptr xn_flat_add(int, int, int, unsigned, int, unsigned, unsigned);

int object_draw_cb(struct record *object)
{
    struct model_instance *instance;
    struct block *block;
    struct block_flat *flat;
    struct block_model *model;
    struct texture_header *missile_image;
    struct monster_anim *anim;
    int n;
    int glow;
    int unused;
    int intensity;
    struct character *character;
    struct texture_header *image;

    if (model_cache_flush_count != 0)
        return 1;
    if (object->flags & 512)
        return 0;
    if (object->type != 9)
        object->draw_handle = 0;
    D_000C5404 = 0;
    switch (object->type) {
    case 18:
        object->angle_x = frame_counter;
        anim = &object->data.monster.anim;
        xn_anim_update(anim);
        object->image = (object->image & -128) | (anim->anim_record + anim->anim_facing);
        image = (struct texture_header *)xn_tex_cache_lookup_image(object->image >> 7, object->image & 127);
        anim->frame_count = image->frame_time;
        n = object->image >> 7;
        if (n == 280 || n == 281) {
            xn_light_add(object->x, object->y, object->z, 31, 256, 0);
            glow = 4194304;
        } else {
            glow = 0;
        }
        if ((object->image >> 7) == 268 && anim->anim_frame >= 4)
            n = 1;
        character = &object->data.character;
        if ((character->conditions & 4) == 0) {
            if ((monster_table[character->race].flags & 1) && character->race != 29 && object->y - 90 > character->ceiling_y)
                object->draw_handle = xn_flat_add(object->x, object->y - 30, object->z, object->image, anim->anim_frame, (anim->anim_bits >> 10) & 32 | 4, glow + 256);
            else
                object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, anim->anim_frame, (anim->anim_bits >> 10) & 32 | 4, glow + 256);
        }
        break;
    case 42:
        if (flat_anim_finished(object) != 0) {
            object_free_later(object);
            break;
        }
        object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, object->anim_frame, 1, 256);
        flat_anim_step(object);
        break;
    case 53:
        object->angle_x = frame_counter;
        if (object->flags & 16384)
            n = 36;
        else
            n = 4;
        if ((object->image & 127) >= 5)
            object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, (unsigned)(*(int *)DOS_LOW(0x46C) & 32) >> 5, n, 256);
        else
            object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, -1, n, 256);
        break;
    case 8:
        object->angle_x = frame_counter;
        if (object->image == 0 || object->image == 65535) {
            n = 1;
            break;
        }
        object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, -1, 4, 256);
        break;
    case 2:
        if (object->image2 == 998 && object->image == 0) {
            instance = &D_001A945E;
            instance->model = (char *)model_get(object->image2, object->image, (current_climate << 2) + D_001A949C);
            if (instance->model != 0) {
                instance->x = object->x;
                instance->y = object->y;
                instance->z = object->z;
                if (weapon_arrow_update(object) == 0)
                    break;
                weapon_missile_orient(object);
                instance->missile_angles[0] = object->missile_yaw;
                instance->missile_angles[1] = object->angle_z;
                instance->missile_angles[2] = 0;
                xn_model_submit(instance, 0);
                break;
            }
        }
        object->angle_x = frame_counter;
        if (object->image == 0 || object->image == 65535 || object->image == 200)
            break;
        if ((object->image >> 7) == 210)
            glow = 63;
        else
            glow = 0;
        object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, -1, 4, (glow << 16) + 256);
        break;
    case 34:
        if (cfg_show_markers == 0)
            break;
    case 33:
    case 44:
        object->angle_x = frame_counter;
        if (object->image == 0 || object->image == 65535 || object->image == 200)
            break;
        if ((object->image >> 7) == 210)
            glow = 63;
        else
            glow = 0;
        object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, -1, 4, (glow << 16) + 256);
        break;
    case 9:
        if (object->flags & 8192) {
            if (object->image2 & 32768) {
                if (object->image2 == 32768)
                    spell_area_effect(object);
                missile_image = (struct texture_header *)xn_tex_cache_lookup_image(object->missile_texture >> 7, object->missile_texture & 127);
                if ((int)((struct stat15 *)&object->image2)->f >= missile_image->frame_count) {
                    object->image2 = 36863;
                } else {
                    xn_flat_add(object->x, object->y, object->z, object->missile_texture, ((struct stat15 *)&object->image2)->f, 1, 4129024);
                    object->image2++;
                    object->children->light_radius >>= 1;
                }
            } else {
                xn_flat_add(object->x, object->y, object->z, object->missile_texture, -1, 1, 256);
            }
        }
        break;
    case 7:
        if (object->parent->type == 4) {
            object->draw_handle = xn_light_add(object->x, object->y, object->z, object->image, 255, 0);
        } else {
            intensity = xn_rand_noise_2d(object->x ^ object->z, (xn_anim_ticks / 40) << 7);
            intensity >>= 3;
            intensity = 256 - intensity;
            intensity = (object->light_radius * intensity) >> 8;
            object->draw_handle = xn_light_add(object->x, object->y, object->z, object->image, intensity, 0);
        }
        break;
    case 43:
        block = &object->data.block;
        model = block->models;
        flat = block->flats;
        for (n = 0; block->model_count > n; n++, model++) {
            model->model = (char *)model_get(model->id, model->variant, (current_climate << 2) + D_001A949C);
            if (model->model != 0)
                xn_model_submit(&model->model, 0);
        }
        for (n = 0; block->flat_count > n; n++, flat++) {
            if (flat->image == 0 || flat->image == 65535)
                continue;
            if ((flat->image >> 7) == 199 && cfg_show_markers == 0)
                continue;
            if ((flat->image >> 7) == 210) {
                xn_light_add(flat->x, flat->y - 16, flat->z, flat->faction_id & 255, flat->faction_id >> 8, 0);
                glow = 63;
            } else {
                glow = 0;
            }
            xn_flat_add(flat->x, flat->y, flat->z, flat->image, -1, 4, (glow << 16) + 256);
        }
        break;
    case 6:
    case 32:
        if (object->image2 == 0)
            break;
        instance = &object->data.instance;
        instance->model = (char *)model_get(object->image2, object->image, (current_climate << 2) + D_001A949C);
        if (instance->model != 0) {
            instance->x = object->x;
            instance->y = object->y;
            instance->z = object->z;
            if (object->image2 == 998) {
                if (weapon_arrow_update(object) == 0)
                    break;
                weapon_missile_orient(object);
                instance->missile_angles[0] = object->missile_yaw;
                instance->missile_angles[1] = object->angle_z;
                instance->missile_angles[2] = 0;
            } else if (object->link_flag != 255) {
                xn_model_set_angles_yaw_offset(instance->angles, object->wait_state);
            } else {
                if (object->image2 == 610 && object->image == 32)
                    D_000C5404 = 3;
                xn_model_compose_angles(instance->angles, object->angle_x, object->yaw + object->wait_state, object->angle_z);
            }
            xn_model_submit(instance, 0);
        }
        break;
    case 56:
        model = (struct block_model *)&object->data;
        flat = (struct block_flat *)(model + object->model_count);
        for (n = 0; object->model_count > n; n++, model++) {
            model->model = (char *)model_get(model->id, model->variant, (current_climate << 2) + D_001A949C);
            if (model->model != 0)
                xn_model_submit(&model->model, 0);
        }
        for (n = 0; object->flat_count > n; n++, flat++) {
            if (flat->image == 0 || flat->image == 65535)
                continue;
            if ((flat->image >> 7) == 199 && cfg_show_markers == 0)
                continue;
            if (flat->faction_id == 0) {
                if ((flat->image >> 7) == 210 && daylight == 0) {
                    intensity = xn_rand_noise_2d(object->x ^ object->z, (xn_anim_ticks / 40) << 7);
                    intensity >>= 3;
                    intensity = 256 - intensity;
                    glow = (intensity * 225) >> 8;
                    intensity = glow;
                    xn_light_add(flat->x, flat->y - flat->flags * 3, flat->z, 64, intensity, 0);
                } else {
                    glow = 0;
                }
                xn_flat_add(flat->x, flat->y, flat->z, flat->image, -1, 4, (glow << 16) + 256);
                flat_animal_sound(flat->x, flat->y, flat->z, flat->image >> 7, flat->image & 127);
            }
        }
        break;
    }
    return 0;
}
