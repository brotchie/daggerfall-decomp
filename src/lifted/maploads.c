/* maploads.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern int D_00147954;
extern char D_001704CC[];
extern char D_001704D7[];
extern char D_001704E4[];
extern char D_001704F2[];
extern char D_00170500[];
extern char D_0017050E[];
extern char D_00170522[];
extern char D_0017053F[];
extern char D_0017054A[];
extern char tavern_buttons[];
extern char D_00179E26[];
extern char D_00179E28[];
extern char D_00179E2A[];
extern char D_00179E2C[];
extern signed char text_buffer[];
extern signed char tavern_state;
extern signed char D_001940D4;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern int D_00195C44;
extern signed char current_region;
extern signed char mouse_buttons_prev;
extern int region_location_type_counts[];
extern char dungeon_blocks[];
extern int D_00196800;
extern int D_00196808;
extern char location_exterior[];
extern int D_00196A28;
extern char *rmb_block;
extern char region_dungeon_type_counts[];
extern char *D_00196A7C;
extern struct map_location *location_here;
extern int region_dungeon_count;
extern struct loaded_location loaded_location;
extern struct map_location *D_00196A9C;
extern int blocks_bsa;
extern int block_origin_x;
extern int block_origin_z;
extern int maps_bsa;
extern char D_00196AB0[];
extern short D_00196ABA;
extern int tavern_menu_image;
extern char cfg_mapsave_file[];
extern short nature_texture_archive;

extern int archive_open(int, int, int);
extern int archive_find_record(int, int, int);
extern int archive_record_size(int, int);
extern int archive_record_offset(int, int);
extern int archive_read_record(int, int, int);
extern int tavern_open(int);
extern int sound_play(int, struct record *, int);
extern int picklist_update(void);
extern struct record *rmb_make_flat(struct record *, short, short, int);
extern struct record *rmb_add_building(struct record *, int);
extern int region_find_location(int);
extern struct record *object_create_child(struct record *, int, int);
extern int func_00097B2A(void);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int mc_malloc();
extern int func_000A00CB();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000C2D81();
extern int func_0012DB50();
extern int func_00135DE4();
extern int func_00135E39();
extern int func_00144F68();
extern int func_0014B45B();
extern void archive_close(int);
extern void archive_write_record(int, int, int);
extern void town_block_load_rmb(int);
extern void func_0001E854(struct building *, int);
extern void tavern_close(void);
extern void tavern_room_offer(void);
extern void tavern_room_pay(void);
extern void tavern_buy_food(int);
extern void fatal_error(int);
extern void town_block_apply_ground(int, int);
extern void town_map_add_block(int, int);
struct record *func_0001E576(void);
void region_locations_load_discovered(int);
void region_locations_save_discovered(int);
void maploads_load_region(int);
void location_read_record(struct loaded_location *, int);
void location_load_exterior(struct loaded_location *, int);
void rmb_index_records(void);
void func_0001E928(struct record *);
#pragma aux func_000A0ED9 parm routine [];

void region_locations_load_discovered(int a1)
{
    struct map_location *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = D_00196A9C;
    l_28 = D_00195C44;
    l_18 = mc_malloc(4096, (int)D_001704CC, 57);
    func_000A0ED9(59, (int)D_001704CC);
    mc_sprintf(l_18, (int)D_001704D7, a1);
    l_24 = archive_open((int)cfg_mapsave_file, 0, 1);
    l_20 = archive_find_record(l_24, l_18, 12);
    archive_read_record(l_24, l_20, l_28);
    archive_close(l_24);
    for (l_1C = 0; l_1C < D_00196A28; l_1C++, l_2C++, l_28++) {
        if (((int)(unsigned char)(*(signed char *)((char *)l_28) & 64)) != 0) {
            l_2C->x_type_flags |= 0x40000000;
        }
        if (((int)(unsigned char)(*(signed char *)((char *)l_28) & 128)) != 0) {
            l_2C->x_type_flags |= 0x80000000;
        }
    }
    if (l_18 == 0 || l_18 == (-1751672937)) return;
    mc_free(l_18, (int)D_001704CC, 72);
    l_18 = -1751672937;
}

void region_locations_save_discovered(int a1)
{
    struct map_location *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = D_00196A9C;
    l_28 = D_00195C44;
    l_18 = mc_malloc(4096, (int)D_001704CC, 88);
    for (l_1C = 0; l_1C < D_00196A28; l_1C++, l_2C++, l_28++) {
        *(signed char *)((char *)l_28) = rand() & -193;
        if ((l_2C->x_type_flags & 0x40000000) != 0) *(signed char *)((char *)l_28) |= 64;
        if ((l_2C->x_type_flags & 0x80000000) != 0) *(signed char *)((char *)l_28) |= 128;
    }
    func_000A0ED9(97, (int)D_001704CC);
    mc_sprintf(l_18, (int)D_001704D7, a1);
    l_24 = archive_open((int)cfg_mapsave_file, 0, 1);
    l_20 = archive_find_record(l_24, l_18, 12);
    archive_write_record(l_24, l_20, D_00195C44);
    archive_close(l_24);
    if (l_18 == 0 || l_18 == (-1751672937)) return;
    mc_free(l_18, (int)D_001704CC, 103);
    l_18 = -1751672937;
}

void maploads_load_region(int a1)
{
    struct map_location *l_24;
    int l_20;
    int l_1C;
    int l_18;

    if ((int)D_00196A9C != 0) {
        region_locations_save_discovered((int)(unsigned short)D_00196ABA);
        if ((int)D_00196A9C != 0 && (int)D_00196A9C != (-1751672937)) {
            mc_free((int)D_00196A9C, (int)D_001704CC, 123);
            D_00196A9C = (struct map_location *)-1751672937;
        }
    }
    D_00196ABA = a1;
    func_000A0ED9(127, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_001704E4, a1);
    l_20 = archive_find_record(maps_bsa, (int)text_buffer, 13);
    l_1C = archive_record_size(maps_bsa, l_20);
    D_00196A28 = ((unsigned)l_1C) / 17;
    l_24 = (struct map_location *)(*(int *)&D_00196A9C = mc_malloc(l_1C, (int)D_001704CC, 133));
    archive_read_record(maps_bsa, l_20, (int)D_00196A9C);
    mc_memset((int)region_location_type_counts, 0, 56, (int)D_001704CC, 137, 56);
    mc_memset((int)region_dungeon_type_counts, 0, 76, (int)D_001704CC, 138, 76);
    region_dungeon_count = 0;
    for (l_18 = 0; l_18 < D_00196A28; l_18++, l_24++) {
        (region_location_type_counts[((l_24->x_type_flags << 2) >> 27)])++;
        if (l_24->dungeon_type != 255) {
            (*(int *)(region_dungeon_type_counts + (l_24->dungeon_type << 2)))++;
            (region_dungeon_count)++;
        }
    }
    region_locations_load_discovered(a1);
    location_here = (struct map_location *)region_find_location(func_000C2D81(player_object->x, player_object->z));
}

void region_load_location_names(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    func_000A0ED9(170, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_001704F2, a1);
    *(int *)&D_00196A7C = D_00147954;
    l_24 = archive_find_record(maps_bsa, (int)text_buffer, 13);
    archive_read_record(maps_bsa, l_24, D_00147954);
}

void func_0001DF7F(int a1)
{
    maploads_load_region(a1);
}

void func_0001DFA2(void)
{
    if ((int)D_00196A7C == 0) return;
    *(int *)&D_00196A7C = 0;
}

void location_read_record(struct loaded_location *a1, int a2)
{
    int l_14;

    func_000A00CB(a2, (int)&a1->door_count, 4);
    a1->doors = (char *)mc_malloc(a1->door_count * 6, (int)D_001704CC, 219);
    func_000A00CB(a2, (int)a1->doors, a1->door_count * 6);
    a1->object = (struct record *)mc_malloc(119, (int)D_001704CC, 223);
    a1->data = &a1->object->data.location;
    func_000A00CB(a2, (int)a1->object, 119);
    if (a1->data->building_count == 0) return;
    a1->data->buildings = (struct building *)mc_malloc(a1->data->building_count * 26, (int)D_001704CC, 231);
    func_000A00CB(a2, (int)a1->data->buildings, a1->data->building_count * 26);
}

void location_load_dungeon(struct loaded_location *a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_14 = D_00195C44;
    func_000A0ED9(248, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_00170500, (int)(unsigned char)current_region);
    l_1C = archive_find_record(maps_bsa, (int)text_buffer, 13);
    l_18 = archive_record_offset(maps_bsa, l_1C);
    lseek(maps_bsa, l_18, 0);
    func_000A00CB(maps_bsa, (int)&l_20, 4);
    func_000A00CB(maps_bsa, l_14, l_20 << 3);
    l_18 = *(int *)((char *)((a2 << 3) + l_14));
    lseek(maps_bsa, l_18, 1);
    a1->index = a2;
    location_read_record(a1, maps_bsa);
    if (&loaded_location != a1) return;
    a1->data->object_counter = 2;
    a1->data->marker_counter = 64000;
    func_000A00CB(maps_bsa, (int)D_00196AB0, 10);
    func_000A00CB(maps_bsa, (int)dungeon_blocks, 128);
}

void location_load_dungeon_by_id(struct loaded_location *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_14 = D_00195C44;
    func_000A0ED9(287, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_00170500, (int)(unsigned char)current_region);
    l_1C = archive_find_record(maps_bsa, (int)text_buffer, 13);
    l_18 = archive_record_offset(maps_bsa, l_1C);
    lseek(maps_bsa, l_18, 0);
    func_000A00CB(maps_bsa, (int)&l_24, 4);
    func_000A00CB(maps_bsa, l_14, l_24 << 3);
    for (l_20 = 0; l_20 < l_24; l_20++, (*(char (**)[8])&l_14)++) {
        if (*(int *)((char *)l_14 + 4) == a2) break;
    }
    if (l_20 == l_24) fatal_error((int)D_0017050E);
    lseek(maps_bsa, *(int *)((char *)l_14), 1);
    a1->index = l_20;
    location_read_record(a1, maps_bsa);
    if (&loaded_location != a1) return;
    a1->data->object_counter = 2;
    a1->data->marker_counter = 64000;
    func_000A00CB(maps_bsa, (int)D_00196AB0, 10);
    func_000A00CB(maps_bsa, (int)dungeon_blocks, 128);
}

void location_load_exterior(struct loaded_location *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    func_000A0ED9(361, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_00170522, (int)(unsigned char)current_region);
    l_20 = archive_find_record(maps_bsa, (int)text_buffer, 13);
    l_1C = archive_record_offset(maps_bsa, l_20);
    lseek(maps_bsa, (a2 << 2) + l_1C, 0);
    func_000A00CB(maps_bsa, (int)&l_14, 4);
    lseek(maps_bsa, l_14 + ((D_00196A28 << 2) + l_1C), 0);
    a1->index = a2;
    location_read_record(a1, maps_bsa);
    if (&loaded_location != a1) return;
    l_18 = a1->data->width * a1->data->height;
    a1->data->object_counter = 2;
    a1->data->marker_counter = 64000;
    func_000A00CB(maps_bsa, (int)location_exterior, 412);
    func_000A00CB(maps_bsa, (int)&l_24, 4);
}

void func_0001E502(struct loaded_location *a1, int a2, int a3)
{
    struct map_location *l_14;
    int l_10;

    l_14 = D_00196A9C;
    for (l_10 = 0; l_10 < D_00196A28; l_10++, l_14++) {
        if (((l_14->x_type_flags << 2) >> 27) == a2) {
            if (a3 == 0) {
                location_load_exterior(a1, l_10);
                return;
            }
            a3--;
        }
    }
}

struct record *func_0001E576(void)
{
    struct record *l_1C;

    l_1C = object_create_child(D_00195AC4, 0, 429);
    l_1C->type = 38;
    l_1C->x = block_origin_x;
    l_1C->y = func_0014B45B(block_origin_x, block_origin_z) - 8;
    l_1C->z = block_origin_z - 4096;
    l_1C->pad13 = 32768;
    l_1C->id = D_00195AC4->id + current_location->object_counter++;
    return l_1C;
}

void rmb_index_records(void)
{
    int l_1C;
    int l_18;

    l_1C = (int)rmb_block + 6776;
    for (l_18 = 0; ((int)(unsigned char)*(signed char *)(rmb_block)) > l_18; l_18++) {
        *(int *)(rmb_block + 1475 + (l_18 << 2)) = l_1C;
        l_1C += *(int *)(rmb_block + 1603 + (l_18 << 2));
    }
    *(int *)(rmb_block + 1731) = l_1C;
    *(int *)(rmb_block + 1735) = (int)(*(char **)(rmb_block + 1731) + (((int)(unsigned char)*(signed char *)(rmb_block + 1)) * 66));
}

void func_0001E928(struct record *a1)
{
    struct record *l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = ((int)(unsigned char)*(signed char *)(rmb_block + 1)) * 66;
    l_20 += ((int)(unsigned char)*(signed char *)(rmb_block + 2)) * 17;
    l_34 = object_create_child(a1, 0, l_20);
    l_34->type = 56;
    l_34->model_count = (unsigned short)(unsigned char)*(signed char *)(rmb_block + 1);
    l_34->flat_count = (unsigned short)(unsigned char)*(signed char *)(rmb_block + 2);
    l_34->id = D_00195AC4->id;
    l_30 = (int)RECORD_DATA(l_34);
    l_2C = l_30 + (((int)(unsigned char)*(signed char *)(rmb_block + 1)) * 66);
    mc_memcpy(l_30, *(int *)(rmb_block + 1731), ((int)(unsigned char)*(signed char *)(rmb_block + 1)) * 66, (int)D_001704CC, 565, 4);
    mc_memcpy(l_2C, *(int *)(rmb_block + 1735), ((int)(unsigned char)*(signed char *)(rmb_block + 2)) * 17, (int)D_001704CC, 566, 4);
    for (l_24 = 0; ((int)(unsigned char)*(signed char *)(rmb_block + 1)) > l_24; l_24++, (*(char (**)[66])&l_30)++) {
        *(int *)((char *)l_30 + 4) = 0;
        *(int *)((char *)l_30 + 36) += block_origin_x;
        *(int *)((char *)l_30 + 44) += block_origin_z;
        *(int *)((char *)l_30 + 40) += func_0014B45B(*(int *)((char *)l_30 + 36), *(int *)((char *)l_30 + 44));
    }
    l_24 = 0;
L1EA8C:;
    if (((int)(unsigned char)*(signed char *)(rmb_block + 2)) > l_24) goto L1EAB2;
    goto L1EC01;
L1EAA3:;
    l_24++;
    (*(char (**)[17])&l_2C)++;
    goto L1EA8C;
L1EAB2:;
    *(int *)((char *)l_2C) += block_origin_x;
    *(int *)((char *)l_2C + 8) += block_origin_z;
    *(int *)((char *)l_2C + 4) += func_0014B45B(*(int *)((char *)l_2C), *(int *)((char *)l_2C + 8));
    if (*(short *)((char *)l_2C + 14) != 0) {
        l_34 = rmb_make_flat(a1, (int)(short)*(short *)((char *)l_2C + 12), (int)(short)*(short *)((char *)l_2C + 14), 0);
        l_34->x = *(int *)((char *)l_2C);
        l_34->y = *(int *)((char *)l_2C + 4);
        l_34->z = *(int *)((char *)l_2C + 8);
        *(short *)((char *)l_2C + 12) = 0;
        goto L1EBFC;
    }
    switch (((int)(unsigned short)*(short *)((char *)l_2C + 12)) >> 7) {
    case 502:
    case 503:
    case 504:
    case 510:
        *(short *)((char *)l_2C + 12) = (nature_texture_archive << 7) + (*(short *)((char *)l_2C + 12) & 63);
        goto L1EBFC;
    case 210:
        {
            int l_40;
            while ((l_28 = func_00135DE4(((int)(unsigned short)*(short *)((char *)l_2C + 12)) >> 7, (int)(unsigned short)(*(short *)((char *)l_2C + 12) & 49))) == 0) {
                func_00135E39();
            }
            if (((int)(unsigned short)*(short *)((char *)l_28 + 6)) < 255) {
                l_40 = (int)(unsigned short)*(short *)((char *)l_28 + 6);
            } else {
                l_40 = 255;
            }
            *(signed char *)((char *)l_2C + 16) = *(signed char *)&l_40;
        default:
L1EBFC:;
            goto L1EAA3;
L1EC01:;
            mc_memcpy(RECORD_DATA(a1), (int)&*(signed char *)(rmb_block + 6347), 429, (int)D_001704CC, 607, 4);
        }
    }
}

void town_load_blocks(void)
{
    struct building *l_30;
    struct record *l_2C;
    struct record *l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_30 = current_location->buildings;
    D_00196808 = 0;
    *(int *)&rmb_block = D_00147954;
    blocks_bsa = archive_open((int)D_0017053F, 0, 0);
    switch (current_location->kind) {
        goto L1ECD0;
    case 0:
        if ((current_location->width * current_location->height) == 64) {
            D_00196800 = 66;
        } else {
            D_00196800 = 50;
        }
        break;
    case 1:
        D_00196800 = 33;
        break;
    default:
L1ECD0:;
        D_00196800 = 0;
    }
    for (l_1C = 0; current_location->height > l_1C; l_1C++) {
        for (l_20 = 0; current_location->width > l_20; l_20++) {
            block_origin_x = D_00195AC4->x + (l_20 << 12);
            block_origin_z = D_00195AC4->z + ((l_1C + 1) << 12);
            l_2C = func_0001E576();
            town_block_load_rmb((current_location->width * l_1C) + l_20);
            rmb_index_records();
            town_map_add_block(l_20, (int)&*(signed char *)((char *)(current_location->height - l_1C) - 1));
            town_block_apply_ground(block_origin_x, block_origin_z);
            for (l_24 = 0; ((int)(unsigned char)*(signed char *)(rmb_block)) > l_24; l_24++) {
                func_0001E854(l_30, l_24);
                l_28 = rmb_add_building(l_2C, l_24);
                if ((l_28->flags & 8) == 0) {
                    if (l_28->id == current_location->buildings[l_28->image].id) {
                        l_28->building_type = l_30->type;
                        l_30++;
                    } else {
                        fatal_error((int)D_0017054A);
                    }
                }
                if ((l_28->flags & 8) != 0) l_28->flags &= ~0x8;
            }
            func_0001E928(l_2C);
        }
    }
    archive_close(blocks_bsa);
}

void tavern_frame(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (tavern_open(0) == 0) return;
    func_0012DB50(4);
    D_0012B508 = 146;
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0 && (l_1C = picklist_update()) > (-1)) {
        tavern_buy_food(l_1C);
    }
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0) return;
    if (((int)(signed char)tavern_state) == 1) {
        tavern_room_offer();
        return;
    }
    if (((int)(signed char)tavern_state) == 2) {
        tavern_room_pay();
        return;
    }
    func_00097B2A();
    l_18 = tavern_menu_image;
    func_00144F68((int)(unsigned short)*(short *)((char *)l_18), (int)(unsigned short)*(short *)((char *)l_18 + 2), (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
    if (key_down_esc != 0) tavern_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (l_20 = 0; l_20 < 4; l_20++) {
        if (mouse_x > *(short *)(tavern_buttons + (l_20 * 12)) && mouse_x < *(short *)(D_00179E28 + (l_20 * 12)) && mouse_y > *(short *)(D_00179E26 + (l_20 * 12)) && mouse_y < *(short *)(D_00179E2A + (l_20 * 12))) {
            sound_play(203, player_object, 110);
            ((int (*)())(*(int *)(D_00179E2C + (l_20 * 12))))();
            return;
        }
    }
}
