/* engsupp.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
extern char D_001702D4[];
extern unsigned char player_environment;
extern short D_00179954[];
extern char D_0017995C[];
extern short D_00179966[];
extern signed char climate_texture_sets[];
extern signed char scratch_190ce4[];
extern signed char scratch_190ce5;
extern struct record *player_object;
extern struct record *location_object;
extern char hud_bar_image[];
extern struct settings *game_settings;
extern char D_00195C88[];
extern int grid_visit_func;
extern char D_00195D3C[];
extern char picked_model_index[];
extern char click_face_texture[];
extern char D_00196120[];
extern signed char D_0019629F;
extern int D_00196478;
extern int D_0019647C;
extern char pick_result[];
extern int climate_index;

extern int pick_sprite_cb(int);
extern int texture_archive_for_climate(int, unsigned short);
extern int climate_category(void);
extern int rand_range(int, int);
extern int object_find(struct record *, int);
extern int rand();
extern int srand();
extern int mc_memset();
extern int mc_memcpy();
extern int xn_render_pick();
extern void shop_generate_stock(int, int, int, int, int);
extern void func_0007E815(struct record *, int);
extern void town_grid_visit_near(struct record *, int);
extern void object_foreach_open(int, int);
int arch3d_plane_at(int, short);
int pick_model_cb(struct record *);
int arch3d_plane_index(int, int);
int furniture_is_container(int);
void world_for_each_object(int);

int engine_pick_object(int x, int y, int result)
{
    int view_bottom;

    view_bottom = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2));
    if (y > view_bottom) return 0;
    mc_memset(result, 0, 18, (int)D_001702D4, 38, 4);
    *(int *)pick_result = result;
    if (*(int *)((char *)(*(int *)D_00195C88 = xn_render_pick(x, y)) + 4) == 1) return 0;
    if (*(int *)(*(char **)D_00195C88 + 4) != 0) {
        D_0019647C = *(int *)(*(char **)D_00195C88 + 4);
        *(int *)click_face_texture = *(int *)(*(char **)D_00195C88);
        world_for_each_object((int)pick_model_cb);
    } else {
        D_00196478 = *(int *)D_00195C88;
        world_for_each_object((int)pick_sprite_cb);
    }
    return *(int *)(*(char **)pick_result) & 1;
}

int arch3d_plane_point_at(int plane, short i)
{
    short point;

    *(int *)&point = plane + 8;
    if ((short)((unsigned short)(unsigned char)*(signed char *)((char *)plane)) <= i) return 0;
    *(int *)&point += ((int)(short)i) << 3;
    return *(int *)&point;
}

int arch3d_plane_at(int model, short plane_index)
{
    int arch3d;
    int plane;
    short i;

    arch3d = *(int *)((char *)model);
    *(int *)&i = 0;
    if (((int)(short)plane_index) >= *(int *)((char *)arch3d + 8)) return 0;
    plane = arch3d + *(int *)((char *)arch3d + 60);
    while ((short)(short)*(int *)&i < plane_index) {
        plane += (((int)(unsigned char)*(signed char *)((char *)plane)) << 3) + 8;
        (*(int *)&i)++;
    }
    return plane;
}

int pick_model_cb(struct record *object)
{
    int model;
    struct block *block;
    struct block_model *block_model;
    int i;

    if (((struct bf8_0_1 *)(*(char **)pick_result))->f != 0) return 0;
    switch (object->type) {
    case 43:
        block = &object->data.block;
        block_model = block->models;
        for (i = 0; block->model_count > i; i++, block_model++) {
            if ((int)&block_model->model == D_0019647C) {
                *(int *)picked_model_index = i;
                *(int *)D_00195D3C = (int)block_model;
                *(signed char *)(*(char **)pick_result) |= 13;
                *(int *)(*(char **)pick_result + 4) = (int)object;
                *(int *)(*(char **)pick_result + 8) = (int)&*(signed char *)((char *)(i << 8) + arch3d_plane_index(D_0019647C, *(int *)click_face_texture));
                *(short *)(*(char **)pick_result + 12) = i;
                return 1;
            }
        }
        return 0;
    case 56:
        block_model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, block_model++) {
            if ((int)&block_model->model == D_0019647C) {
                *(signed char *)(*(char **)pick_result) |= 5;
                *(int *)(*(char **)pick_result + 4) = (int)object;
                *(int *)(*(char **)pick_result + 8) = arch3d_plane_index(D_0019647C, *(int *)click_face_texture);
                *(short *)(*(char **)pick_result + 14) = block_model->id;
                *(short *)(*(char **)pick_result + 16) = block_model->variant;
                return 1;
            }
        }
        return 0;
    case 6:
    case 32:
        model = (int)RECORD_DATA(object);
        if (D_0019647C == model) {
            *(signed char *)(*(char **)pick_result) |= 5;
            *(int *)(*(char **)pick_result + 4) = (int)object;
            *(int *)(*(char **)pick_result + 8) = arch3d_plane_index(model, *(int *)click_face_texture);
            return 1;
        }
    }
    return 0;
}

int arch3d_plane_index(int model, int plane)
{
    int candidate;
    int i;

    for (i = 0; i < *(int *)(*(char **)((char *)model) + 8); i++) {
        candidate = arch3d_plane_at(model, (int)(short)*(short *)&i);
        if (candidate == plane) return i;
    }
    return -1;
}

void arch3d_apply_climate_textures(int arch3d)
{
    int plane;
    int cursor;
    int i;

    scratch_190ce5 = climate_category();
    scratch_190ce4[0] = climate_texture_sets[climate_index];
    cursor = arch3d + *(int *)((char *)arch3d + 60);
    for (i = 0; i < *(int *)((char *)arch3d + 8); i++) {
        plane = cursor;
        *(short *)((char *)plane + 2) = (*(short *)((char *)plane + 2) & 127) | (texture_archive_for_climate(((int)(unsigned short)*(short *)((char *)plane + 2)) >> 7, (int)(unsigned short)(*(short *)((char *)plane + 2) & 127)) << 7);
        cursor += (((int)(unsigned char)*(signed char *)((char *)cursor)) << 3) + 8;
    }
}

void dungeon_choose_textures(void)
{
    int i;
    int archive;
    int texture_set;
    int saved_seed;

    saved_seed = rand();
    srand(((unsigned)location_object->id) >> 16);
    texture_set = (int)(unsigned char)climate_texture_sets[climate_category()];
    if (texture_set == 1) return;
    mc_memcpy((int)D_00179966, (int)D_0017995C, 10, (int)D_001702D4, 279, 10);
    for (i = 0; i < 5; i++) {
        archive = rand_range(0, 4);
        if (archive == 2) archive += 2;
        archive += (int)(short)D_00179954[texture_set];
        D_00179966[i] = archive;
    }
    srand(saved_seed);
}

void interior_stock_shelves(struct record *interior, struct building *building)
{
    struct block *block;
    struct block_model *model;
    int i;

    block = &interior->data.block;
    model = block->models;
    for (i = 0; block->model_count > i; i++, model++) {
        if (model->id == 418 || (model->id == 410 && furniture_is_container(model->variant) != 0)) {
            shop_generate_stock((int)D_00196120, model->variant, building->quality, building->type, i);
        }
    }
}

int furniture_is_container(int variant)
{
    {
        int is_container;

        if (variant == 3 || variant == 4 || variant == 7 || variant == 8 || variant == 50 || variant == 51 || (variant >= 32 && variant <= 38)) {
            is_container = 1;
        } else {
            is_container = 0;
        }
        return is_container;
    }
}

void world_for_each_object(int callback)
{
    struct record *object;
    struct record *next;
    struct record *children;
    unsigned short flags;

    grid_visit_func = (int)object_find;
    if (((int)player_environment) < 3) {
        if (player_object->parent->type != 1) {
            object_find(player_object->parent->children, callback);
        } else {
            town_grid_visit_near(player_object, callback);
        }
        object = location_object->children;
        while (object != 0) {
            next = object->next;
            children = object->children;
            flags = object->flags;
            if (object->type != 38) {
                ((int (*)())(callback))(object);
                if (((int)(unsigned short)(*(int *)&flags & 1)) == 0) object_find(children, callback);
            }
            object = next;
        }
    } else {
        if (D_0019629F != 0) {
            object_find(location_object, callback);
        } else {
            func_0007E815(player_object, callback);
        }
        object = location_object->children;
        while (object != 0) {
            next = object->next;
            children = object->children;
            flags = object->flags;
            if (object->type != 47) {
                ((int (*)())(callback))(object);
                if (((int)(unsigned short)(*(int *)&flags & 1)) == 0) object_find(children, callback);
            }
            object = next;
        }
    }
    grid_visit_func = (int)object_foreach_open;
}
