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
extern int D_00190CAC;
extern signed char itemmaker_slot_kinds[];
extern signed char D_00190CE5;
extern signed char D_00190CE7;
extern signed char D_001940D5;
extern char D_001940E4[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct record *D_00195AF4;
extern int creature_count;
extern char D_00195B5C[];
extern char D_00195B84[];
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern char D_00195C44[];
extern int D_00195CE0;
extern int D_00195CE4;
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
extern signed char D_00196280;
extern int D_001AA644;
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
extern int func_000CD367();
extern int func_0014BC00();
extern void msgbox_show_string(int, int);
extern void object_foreach_open(struct record *, int);
int marker_match_cb(struct record *);
struct record *marker_find_random(struct record *, int);
struct record *marker_find_nearest(struct record *, int);
int location_cell_at(int, int);
void marker_nearest_cb(struct record *);

int marker_match_cb(struct record *a1)
{
    struct block *l_2C;
    struct block_flat *l_28;
    int l_24;
    int l_20;
    int l_1C;

    switch (a1->type) {
    case 34:
        if ((a1->image >> 7) == 199 && ((a1->image & 31) - 2) == marker_kind) {
            if (*(int *)D_00195B84 == 0 && D_001AA644 < 0) {
                mc_memcpy((int)found_marker, a1, 55, (int)D_00177358, 191, 4);
                return 1;
            }
            *(int *)D_00195B84 += D_001AA644;
        }
        break;
    case 43:
        l_2C = &a1->data.block;
        l_28 = l_2C->flats;
        for (l_1C = 0; l_2C->flat_count > l_1C; l_1C++, l_28++) {
            if ((l_28->image >> 7) == 199) {
                if ((((int)(unsigned short)(l_28->image & 31)) - 2) == marker_kind) {
                    if (*(int *)D_00195B84 == 0 && D_001AA644 < 0) {
                        D_00195F71 = l_28->x;
                        D_00195F79 = l_28->z;
                        D_00195F75 = l_28->y;
                        D_00195F85 = l_28->image;
                        D_00195F89 = a1->id;
                        *(signed char *)found_marker = 34;
                        return 1;
                    }
                    *(int *)D_00195B84 += D_001AA644;
                }
            }
        }
        break;
    case 56:
        l_24 = (int)RECORD_DATA(a1);
        l_28 = (struct block_flat *)(l_24 + (a1->model_count * 66));
        for (l_1C = 0; a1->flat_count > l_1C; l_1C++, l_28++) {
            if ((l_28->image >> 7) == 199) {
                if ((((int)(unsigned short)(l_28->image & 31)) - 2) == marker_kind) {
                    if (*(int *)D_00195B84 == 0 && D_001AA644 < 0) {
                        D_00195F71 = l_28->x;
                        D_00195F79 = l_28->z;
                        D_00195F75 = l_28->y;
                        D_00195F85 = l_28->image;
                        D_00195F89 = a1->id;
                        *(signed char *)found_marker = 34;
                        return 1;
                    }
                    *(int *)D_00195B84 += D_001AA644;
                }
            }
        }
    }
    return 0;
}

struct record *marker_find_first(struct record *a1, int a2)
{
    marker_kind = a2;
    *(int *)D_00195B84 = 0;
    D_001AA644 = -1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 261, 4);
    object_find_open(a1, (int)marker_match_cb);
    if (*(signed char *)found_marker != 0) return (struct record *)found_marker;
    return 0;
}

struct record *marker_find_nth(struct record *a1, int a2, int a3)
{
    int l_14;

    marker_kind = a2;
    *(int *)D_00195B84 = a3;
    D_001AA644 = -1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 287, 4);
    object_find_open(a1, (int)marker_match_cb);
    l_14 = location_cell_at(D_00195F71, D_00195F79);
    D_00195F81 = a3;
    if (*(signed char *)found_marker != 0) return (struct record *)found_marker;
    return 0;
}

struct record *marker_find_random(struct record *a1, int a2)
{
    int l_18;

    marker_kind = a2;
    *(int *)D_00195B84 = 0;
    D_001AA644 = 1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 317, 4);
    object_find_open(a1, (int)marker_match_cb);
    if (*(int *)D_00195B84 == 0) return 0;
    l_18 = (*(int *)D_00195B84 = rand() % *(int *)D_00195B84);
    D_001AA644 = -1;
    object_foreach_open(a1, (int)marker_match_cb);
    D_00195F81 = l_18;
    return (struct record *)found_marker;
}

void marker_nearest_cb(struct record *a1)
{
    struct block *l_28;
    struct block_flat *l_24;
    int l_20;
    int l_1C;
    int l_18;

    switch (a1->type) {
    case 34:
        if ((a1->image >> 7) == 199 && ((a1->image & 31) - 2) != marker_kind) return;
        l_1C = (a1->x - player_object->x) * (a1->x - player_object->x);
        l_1C += ((a1->y - player_object->y) * (a1->y - player_object->y)) * 2;
        l_1C += (a1->z - player_object->z) * (a1->z - player_object->z);
        if (l_1C != 0) l_1C = func_0014BC00(l_1C);
        if (l_1C < D_001AA644) {
            D_00195AF4 = a1;
            D_001AA644 = l_1C;
            mc_memcpy((int)found_marker, a1, 55, (int)D_00177358, 363, 4);
        }
        return;
    case 43:
        l_28 = &a1->data.block;
        l_24 = l_28->flats;
        for (l_18 = 0; l_28->flat_count > l_18; l_18++, l_24++) {
            if ((l_24->image >> 7) == 199 && (((int)(unsigned short)(l_24->image & 31)) - 2) == marker_kind) {
                l_1C = (l_24->x - player_object->x) * (l_24->x - player_object->x);
                l_1C += ((l_24->y - player_object->y) * (l_24->y - player_object->y)) * 2;
                l_1C += (l_24->z - player_object->z) * (l_24->z - player_object->z);
                if (l_1C != 0) l_1C = func_0014BC00(l_1C);
                if (l_1C < D_001AA644) {
                    D_001AA644 = l_1C;
                    *(signed char *)found_marker = 34;
                    D_00195F71 = l_24->x;
                    D_00195F79 = l_24->z;
                    D_00195F75 = l_24->y;
                    D_00195F85 = l_24->image;
                    D_00195F89 = a1->id;
                }
            }
        }
        return;
    case 56:
        l_20 = (int)RECORD_DATA(a1);
        l_24 = (struct block_flat *)(l_20 + (a1->model_count * 66));
        for (l_18 = 0; a1->flat_count > l_18; l_18++, l_24++) {
            if ((l_24->image >> 7) == 199 && (((int)(unsigned short)(l_24->image & 31)) - 2) == marker_kind) {
                l_1C = (l_24->x - player_object->x) * (l_24->x - player_object->x);
                l_1C += ((l_24->y - player_object->y) * (l_24->y - player_object->y)) * 2;
                l_1C += (l_24->z - player_object->z) * (l_24->z - player_object->z);
                if (l_1C != 0) l_1C = func_0014BC00(l_1C);
                if (l_1C < D_001AA644) {
                    D_001AA644 = l_1C;
                    *(signed char *)found_marker = 34;
                    D_00195F71 = l_24->x;
                    D_00195F79 = l_24->z;
                    D_00195F75 = l_24->y;
                    D_00195F85 = l_24->image;
                    D_00195F89 = a1->id;
                }
            }
        }
    default:;
    }
}

struct record *marker_find_nearest(struct record *a1, int a2)
{
    marker_kind = a2;
    D_001AA644 = 500000;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 432, 4);
    object_foreach_open(a1, (int)marker_nearest_cb);
    if (*(signed char *)found_marker != 0) return (struct record *)found_marker;
    return 0;
}

int marker_count(struct record *a1, int a2)
{
    marker_kind = a2;
    *(int *)D_00195B84 = 0;
    D_001AA644 = 1;
    mc_memset((int)found_marker, 0, 71, (int)D_00177358, 456, 4);
    object_find_open(a1, (int)marker_match_cb);
    return *(int *)D_00195B84;
}

int player_to_nearest_marker(struct record *a1, int a2)
{
    struct record *l_18;

    l_18 = marker_find_nearest(a1, a2);
    if (l_18 != 0) {
        player_object->x = D_00195F71;
        player_object->y = D_00195F75;
        player_object->z = D_00195F79;
        D_001940D5 |= 2;
        return 1;
    }
    return 0;
}

int player_to_random_marker(struct record *a1, int a2)
{
    struct record *l_18;

    l_18 = marker_find_random(a1, a2);
    if (l_18 != 0) {
        player_object->x = D_00195F71;
        player_object->y = D_00195F75;
        player_object->z = D_00195F79;
        D_001940D5 |= 2;
        return 1;
    }
    return 0;
}

int location_cell_at(int a1, int a2)
{
    int l_18;

    a1 = ((a1 - D_00195AC4->x) + D_00195CE0) / 1024;
    a2 = ((a2 - D_00195AC4->z) + D_00195CE4) / 1024;
    l_18 = a1 + (a2 << 5);
    return *(int *)(D_001940E4 + (l_18 << 2));
}

int travel_map_open(int a1)
{
    short l_18;

    if (((int)D_0019626F) == 19 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (((int)player_environment) != 1) return 0;
    if (a1 != 0) {
        if ((player_character->race == 8 || (player_class->flags & 16) != 0) && D_00196280 != 0) {
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
        D_001AA6A6 = *(signed char *)&a1;
        window_image = disk_read_file((int)D_00177394, 0);
        if (a1 == 100) {
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
        disk_read_file((int)D_00177430, *(int *)D_00195C44);
        func_000CD367(*(int *)D_00195C44 + 8);
        *(int *)&l_18 = 1132;
        D_00190CAC = *(int *)(*(char **)&l_18);
        D_001AA6A4 = (D_001AA6A5 = 0);
        D_00190CE5 = (itemmaker_slot_kinds[0] = 0);
        *(int *)D_001AA668 = 0;
        game_mode = 19;
        D_00196272 = 1;
        D_00187CA8 = 0;
        D_00190CE7 = 0;
    }
    return ((((int)(unsigned char)game_mode) == 19) ? 1 : 0);
}
