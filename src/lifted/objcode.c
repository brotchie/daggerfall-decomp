/* objcode.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00177358[];
extern char D_00177364[];
extern char D_00177394[];
extern char D_001773A1[];
extern char D_001773AE[];
extern char D_001773BB[];
extern char D_001773C8[];
extern char D_001773D5[];
extern char D_001773E2[];
extern char D_001773EF[];
extern char D_001773FC[];
extern char D_00177409[];
extern char D_00177416[];
extern char D_00177423[];
extern char D_00177430[];
extern unsigned char player_environment;
extern char travel_options[];
extern int D_0018507B;
extern signed char D_00187CA8;
extern int scratch_190cac;
extern signed char scratch_190ce4[];
extern signed char scratch_190ce5;
extern signed char scratch_190ce7;
extern signed char D_001940D5;
extern char location_grid[];
extern struct record *player_object;
extern struct record *location_object;
extern struct record *found_object;
extern int creature_count;
extern char D_00195B5C[];
extern char D_00195B84[];
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern char scratch_buffer[];
extern int location_grid_x;
extern int location_grid_z;
extern char found_marker[];
extern int D_00195F71;
extern int D_00195F75;
extern int D_00195F79;
extern short D_00195F81;
extern short D_00195F85;
extern int D_00195F89;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char is_daytime;
extern int marker_best;
extern int marker_kind;
extern char D_001AA64C[];
extern char D_001AA650[];
extern char D_001AA654[];
extern char D_001AA658[];
extern char D_001AA65C[];
extern int D_001AA660;
extern char D_001AA668[];
extern int D_001AA66C;
extern char D_001AA670[];
extern int D_001AA6A0;
extern signed char D_001AA6A4;
extern signed char D_001AA6A5;
extern signed char D_001AA6A6;

extern int disk_read_file(int, int);
extern int gold_can_afford(int);
extern int object_find_open(struct record *, int);
extern int travel_trip_cost(void);
extern int rand();
extern int mc_memset();
extern int mc_memcpy();
extern int xn_pal_set_all_8bit();
extern int xn_math_isqrt();
extern void msgbox_show_string(int, int);
extern void object_foreach_open(struct record *, int);
int marker_match_cb(struct record *);
struct record *marker_find_random(struct record *, int);
struct record *marker_find_nearest(struct record *, int);
int location_cell_at(int, int);
void marker_nearest_cb(struct record *);

int marker_match_cb(struct record *object)
{
    struct block *block;
    struct block_flat *flat;
    struct block_model *models;
    int unused;
    int i;

    switch (object->type) {
    case 34:
        if ((object->image >> 7) == 199 && ((object->image & 31) - 2) == marker_kind) {
            if (*(int *)D_00195B84 == 0 && marker_best < 0) {
                mc_memcpy((int)found_marker, object, 55, (int)D_00177358, 191, 4);
                return 1;
            }
            *(int *)D_00195B84 += marker_best;
        }
        break;
    case 43:
        block = &object->data.block;
        flat = block->flats;
        for (i = 0; block->flat_count > i; i++, flat++) {
            if ((flat->image >> 7) == 199) {
                if ((((int)(unsigned short)(flat->image & 31)) - 2) == marker_kind) {
                    if (*(int *)D_00195B84 == 0 && marker_best < 0) {
                        D_00195F71 = flat->x;
                        D_00195F79 = flat->z;
                        D_00195F75 = flat->y;
                        D_00195F85 = flat->image;
                        D_00195F89 = object->id;
                        *(signed char *)found_marker = 34;
                        return 1;
                    }
                    *(int *)D_00195B84 += marker_best;
                }
            }
        }
        break;
    case 56:
        models = (struct block_model *)RECORD_DATA(object);
        flat = (struct block_flat *)(models + object->model_count);
        for (i = 0; object->flat_count > i; i++, flat++) {
            if ((flat->image >> 7) == 199) {
                if ((((int)(unsigned short)(flat->image & 31)) - 2) == marker_kind) {
                    if (*(int *)D_00195B84 == 0 && marker_best < 0) {
                        D_00195F71 = flat->x;
                        D_00195F79 = flat->z;
                        D_00195F75 = flat->y;
                        D_00195F85 = flat->image;
                        D_00195F89 = object->id;
                        *(signed char *)found_marker = 34;
                        return 1;
                    }
                    *(int *)D_00195B84 += marker_best;
                }
            }
        }
    }
    return 0;
}

struct record *marker_find_first(struct record *root, int kind)
{
    marker_kind = kind;
    *(int *)D_00195B84 = 0;
    marker_best = -1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 261, 4);
    object_find_open(root, (int)marker_match_cb);
    if (*(signed char *)found_marker != 0) return (struct record *)found_marker;
    return 0;
}

struct record *marker_find_nth(struct record *root, int kind, int n)
{
    int cell;

    marker_kind = kind;
    *(int *)D_00195B84 = n;
    marker_best = -1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 287, 4);
    object_find_open(root, (int)marker_match_cb);
    cell = location_cell_at(D_00195F71, D_00195F79);
    D_00195F81 = n;
    if (*(signed char *)found_marker != 0) return (struct record *)found_marker;
    return 0;
}

struct record *marker_find_random(struct record *root, int kind)
{
    int n;

    marker_kind = kind;
    *(int *)D_00195B84 = 0;
    marker_best = 1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 317, 4);
    object_find_open(root, (int)marker_match_cb);
    if (*(int *)D_00195B84 == 0) return 0;
    n = (*(int *)D_00195B84 = rand() % *(int *)D_00195B84);
    marker_best = -1;
    object_foreach_open(root, (int)marker_match_cb);
    D_00195F81 = n;
    return (struct record *)found_marker;
}

void marker_nearest_cb(struct record *object)
{
    struct block *block;
    struct block_flat *flat;
    struct block_model *models;
    int dist;
    int i;

    switch (object->type) {
    case 34:
        if ((object->image >> 7) == 199 && ((object->image & 31) - 2) != marker_kind) return;
        dist = (object->x - player_object->x) * (object->x - player_object->x);
        dist += ((object->y - player_object->y) * (object->y - player_object->y)) * 2;
        dist += (object->z - player_object->z) * (object->z - player_object->z);
        if (dist != 0) dist = xn_math_isqrt(dist);
        if (dist < marker_best) {
            found_object = object;
            marker_best = dist;
            mc_memcpy((int)found_marker, object, 55, (int)D_00177358, 363, 4);
        }
        return;
    case 43:
        block = &object->data.block;
        flat = block->flats;
        for (i = 0; block->flat_count > i; i++, flat++) {
            if ((flat->image >> 7) == 199 && (((int)(unsigned short)(flat->image & 31)) - 2) == marker_kind) {
                dist = (flat->x - player_object->x) * (flat->x - player_object->x);
                dist += ((flat->y - player_object->y) * (flat->y - player_object->y)) * 2;
                dist += (flat->z - player_object->z) * (flat->z - player_object->z);
                if (dist != 0) dist = xn_math_isqrt(dist);
                if (dist < marker_best) {
                    marker_best = dist;
                    *(signed char *)found_marker = 34;
                    D_00195F71 = flat->x;
                    D_00195F79 = flat->z;
                    D_00195F75 = flat->y;
                    D_00195F85 = flat->image;
                    D_00195F89 = object->id;
                }
            }
        }
        return;
    case 56:
        models = (struct block_model *)RECORD_DATA(object);
        flat = (struct block_flat *)(models + object->model_count);
        for (i = 0; object->flat_count > i; i++, flat++) {
            if ((flat->image >> 7) == 199 && (((int)(unsigned short)(flat->image & 31)) - 2) == marker_kind) {
                dist = (flat->x - player_object->x) * (flat->x - player_object->x);
                dist += ((flat->y - player_object->y) * (flat->y - player_object->y)) * 2;
                dist += (flat->z - player_object->z) * (flat->z - player_object->z);
                if (dist != 0) dist = xn_math_isqrt(dist);
                if (dist < marker_best) {
                    marker_best = dist;
                    *(signed char *)found_marker = 34;
                    D_00195F71 = flat->x;
                    D_00195F79 = flat->z;
                    D_00195F75 = flat->y;
                    D_00195F85 = flat->image;
                    D_00195F89 = object->id;
                }
            }
        }
    default:;
    }
}

struct record *marker_find_nearest(struct record *root, int kind)
{
    marker_kind = kind;
    marker_best = 500000;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 432, 4);
    object_foreach_open(root, (int)marker_nearest_cb);
    if (*(signed char *)found_marker != 0) return (struct record *)found_marker;
    return 0;
}

int marker_count(struct record *root, int kind)
{
    marker_kind = kind;
    *(int *)D_00195B84 = 0;
    marker_best = 1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 456, 4);
    object_find_open(root, (int)marker_match_cb);
    return *(int *)D_00195B84;
}

int player_to_nearest_marker(struct record *root, int kind)
{
    struct record *marker;

    marker = marker_find_nearest(root, kind);
    if (marker != 0) {
        player_object->x = D_00195F71;
        player_object->y = D_00195F75;
        player_object->z = D_00195F79;
        D_001940D5 |= 2;
        return 1;
    }
    return 0;
}

int player_to_random_marker(struct record *root, int kind)
{
    struct record *marker;

    marker = marker_find_random(root, kind);
    if (marker != 0) {
        player_object->x = D_00195F71;
        player_object->y = D_00195F75;
        player_object->z = D_00195F79;
        D_001940D5 |= 2;
        return 1;
    }
    return 0;
}

int location_cell_at(int x, int z)
{
    int cell;

    x = ((x - location_object->x) + location_grid_x) / 1024;
    z = ((z - location_object->z) + location_grid_z) / 1024;
    cell = x + (z << 5);
    return *(int *)(location_grid + (cell << 2));
}

int travel_map_open(int mode)
{
    short ticks_addr;

    if (((int)D_0019626F) == 19 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (((int)player_environment) != 1) return 0;
    if (mode != 0) {
        if ((player_character->race == 8 || (player_class->flags & 16) != 0) && is_daytime != 0) {
            msgbox_show_string((int)D_00177364, 1);
            return 0;
        }
        if (creature_count != 0) {
            msgbox_show_string(D_0018507B, 1);
            return 0;
        }
        if (((int)(unsigned short)(*(short *)travel_options & 16)) != 0 && gold_can_afford(travel_trip_cost()) == 0) {
            *(signed char *)travel_options ^= 48;
        }
        if (((int)(unsigned short)(*(short *)travel_options & 8)) != 0 && player_character->ship_owned == 0 && gold_can_afford(travel_trip_cost()) == 0) {
            *(signed char *)travel_options ^= 12;
        }
        D_001AA6A6 = *(signed char *)&mode;
        window_image = disk_read_file((int)D_00177394, 0);
        if (mode == 100) {
            D_001AA6A0 = disk_read_file((int)D_001773A1, 0);
        } else {
            D_001AA6A0 = disk_read_file((int)D_001773AE, 0);
        }
        D_001AA66C = disk_read_file((int)D_001773BB, 0);
        *(int *)D_001AA65C = disk_read_file((int)D_001773C8, 0);
        D_001AA660 = disk_read_file((int)D_001773D5, 0);
        *(int *)D_001AA670 = disk_read_file((int)D_001773E2, 0);
        *(int *)D_001AA64C = disk_read_file((int)D_001773EF, 0);
        *(int *)D_001AA650 = disk_read_file((int)D_001773FC, 0);
        *(int *)D_001AA654 = disk_read_file((int)D_00177409, 0);
        *(int *)D_001AA658 = disk_read_file((int)D_00177416, 0);
        *(int *)D_00195B5C = disk_read_file((int)D_00177423, 0);
        disk_read_file((int)D_00177430, *(int *)scratch_buffer);
        xn_pal_set_all_8bit(*(int *)scratch_buffer + 8);
        *(int *)&ticks_addr = 1132;
        scratch_190cac = *(int *)(*(char **)&ticks_addr);
        D_001AA6A4 = (D_001AA6A5 = 0);
        scratch_190ce5 = (scratch_190ce4[0] = 0);
        *(int *)D_001AA668 = 0;
        game_mode = 19;
        D_00196272 = 1;
        D_00187CA8 = 0;
        scratch_190ce7 = 0;
    }
    return ((((int)(unsigned char)game_mode) == 19) ? 1 : 0);
}
