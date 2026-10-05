/* maploads.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down_esc[];
extern char D_00147954[];
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
extern char text_buffer[];
extern char tavern_state[];
extern char D_001940D4[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern char D_00195C44[];
extern char current_region[];
extern char mouse_buttons_prev[];
extern char region_location_type_counts[];
extern char dungeon_blocks[];
extern char D_00196800[];
extern char D_00196808[];
extern char location_exterior[];
extern char D_00196A28[];
extern char rmb_block[];
extern char region_dungeon_type_counts[];
extern char D_00196A7C[];
extern struct map_location *location_here;
extern char region_dungeon_count[];
extern struct loaded_location loaded_location;
extern struct map_location *D_00196A9C;
extern char blocks_bsa[];
extern char block_origin_x[];
extern char block_origin_z[];
extern char maps_bsa[];
extern char D_00196AB0[];
extern char D_00196ABA[];
extern char tavern_menu_image[];
extern char cfg_mapsave_file[];
extern char nature_texture_archive[];

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
    l_28 = *(int *)D_00195C44;
    l_18 = mc_malloc(4096, (int)D_001704CC, 57);
    func_000A0ED9(59, (int)D_001704CC);
    mc_sprintf(l_18, (int)D_001704D7, a1);
    l_24 = archive_open((int)cfg_mapsave_file, 0, 1);
    l_20 = archive_find_record(l_24, l_18, 12);
    archive_read_record(l_24, l_20, l_28);
    archive_close(l_24);
    l_1C = 0;
L1DB8C:;
    if (l_1C < *(int *)D_00196A28) goto L1DBAE;
    goto L1DBDE;
L1DB99:;
    l_1C++;
    l_2C++;
    l_28++;
    goto L1DB8C;
L1DBAE:;
    if (((int)(unsigned char)(*(signed char *)((char *)l_28) & 64)) == 0) goto L1DBC5;
    l_2C->x_type_flags |= 0x40000000;
L1DBC5:;
    if (((int)(unsigned char)(*(signed char *)((char *)l_28) & 128)) == 0) goto L1DBDC;
    l_2C->x_type_flags |= 0x80000000;
L1DBDC:;
    goto L1DB99;
L1DBDE:;
    if (l_18 == 0) goto L1DBED;
    if (l_18 != (-1751672937)) goto L1DBEF;
L1DBED:;
    return;
L1DBEF:;
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
    l_28 = *(int *)D_00195C44;
    l_18 = mc_malloc(4096, (int)D_001704CC, 88);
    l_1C = 0;
L1DC51:;
    if (l_1C < *(int *)D_00196A28) goto L1DC73;
    goto L1DCA2;
L1DC5E:;
    l_1C++;
    l_2C++;
    l_28++;
    goto L1DC51;
L1DC73:;
    *(signed char *)((char *)l_28) = rand() & -193;
    if ((l_2C->x_type_flags & 0x40000000) == 0) goto L1DC91;
    *(signed char *)((char *)l_28) |= 64;
L1DC91:;
    if ((l_2C->x_type_flags & 0x80000000) == 0) goto L1DCA0;
    *(signed char *)((char *)l_28) |= 128;
L1DCA0:;
    goto L1DC5E;
L1DCA2:;
    func_000A0ED9(97, (int)D_001704CC);
    mc_sprintf(l_18, (int)D_001704D7, a1);
    l_24 = archive_open((int)cfg_mapsave_file, 0, 1);
    l_20 = archive_find_record(l_24, l_18, 12);
    archive_write_record(l_24, l_20, *(int *)D_00195C44);
    archive_close(l_24);
    if (l_18 == 0) goto L1DD14;
    if (l_18 != (-1751672937)) goto L1DD16;
L1DD14:;
    return;
L1DD16:;
    mc_free(l_18, (int)D_001704CC, 103);
    l_18 = -1751672937;
}

void maploads_load_region(int a1)
{
    struct map_location *l_24;
    int l_20;
    int l_1C;
    int l_18;

    if ((int)D_00196A9C == 0) goto L1DD95;
    region_locations_save_discovered((int)(unsigned short)*(short *)D_00196ABA);
    if ((int)D_00196A9C == 0) goto L1DD75;
    if ((int)D_00196A9C != (-1751672937)) goto L1DD77;
L1DD75:;
    goto L1DD95;
L1DD77:;
    mc_free((int)D_00196A9C, (int)D_001704CC, 123);
    D_00196A9C = (struct map_location *)-1751672937;
L1DD95:;
    *(short *)D_00196ABA = a1;
    func_000A0ED9(127, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_001704E4, a1);
    l_20 = archive_find_record(*(int *)maps_bsa, (int)text_buffer, 13);
    l_1C = archive_record_size(*(int *)maps_bsa, l_20);
    *(int *)D_00196A28 = ((unsigned)l_1C) / 17;
    l_24 = (struct map_location *)(*(int *)&D_00196A9C = mc_malloc(l_1C, (int)D_001704CC, 133));
    archive_read_record(*(int *)maps_bsa, l_20, (int)D_00196A9C);
    mc_memset((int)region_location_type_counts, 0, 56, (int)D_001704CC, 137, 56);
    mc_memset((int)region_dungeon_type_counts, 0, 76, (int)D_001704CC, 138, 76);
    *(int *)region_dungeon_count = 0;
    l_18 = 0;
L1DE78:;
    if (l_18 < *(int *)D_00196A28) goto L1DE94;
    goto L1DED7;
L1DE85:;
    l_18++;
    l_24++;
    goto L1DE78;
L1DE94:;
    (*(int *)(region_location_type_counts + (((l_24->x_type_flags << 2) >> 27) << 2)))++;
    if (l_24->dungeon_type == 255) goto L1DED5;
    (*(int *)(region_dungeon_type_counts + (l_24->dungeon_type << 2)))++;
    (*(int *)region_dungeon_count)++;
L1DED5:;
    goto L1DE85;
L1DED7:;
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
    *(int *)D_00196A7C = *(int *)D_00147954;
    l_24 = archive_find_record(*(int *)maps_bsa, (int)text_buffer, 13);
    archive_read_record(*(int *)maps_bsa, l_24, *(int *)D_00147954);
}

void func_0001DF7F(int a1)
{
    maploads_load_region(a1);
}

void func_0001DFA2(void)
{
    if (*(int *)D_00196A7C == 0) return;
    *(int *)D_00196A7C = 0;
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

    l_14 = *(int *)D_00195C44;
    func_000A0ED9(248, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_00170500, (int)(unsigned char)*(signed char *)current_region);
    l_1C = archive_find_record(*(int *)maps_bsa, (int)text_buffer, 13);
    l_18 = archive_record_offset(*(int *)maps_bsa, l_1C);
    lseek(*(int *)maps_bsa, l_18, 0);
    func_000A00CB(*(int *)maps_bsa, (int)&l_20, 4);
    func_000A00CB(*(int *)maps_bsa, l_14, l_20 << 3);
    l_18 = *(int *)((char *)((a2 << 3) + l_14));
    lseek(*(int *)maps_bsa, l_18, 1);
    a1->index = a2;
    location_read_record(a1, *(int *)maps_bsa);
    if (&loaded_location != a1) return;
    a1->data->object_counter = 2;
    a1->data->marker_counter = 64000;
    func_000A00CB(*(int *)maps_bsa, (int)D_00196AB0, 10);
    func_000A00CB(*(int *)maps_bsa, (int)dungeon_blocks, 128);
}

void location_load_dungeon_by_id(struct loaded_location *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_14 = *(int *)D_00195C44;
    func_000A0ED9(287, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_00170500, (int)(unsigned char)*(signed char *)current_region);
    l_1C = archive_find_record(*(int *)maps_bsa, (int)text_buffer, 13);
    l_18 = archive_record_offset(*(int *)maps_bsa, l_1C);
    lseek(*(int *)maps_bsa, l_18, 0);
    func_000A00CB(*(int *)maps_bsa, (int)&l_24, 4);
    func_000A00CB(*(int *)maps_bsa, l_14, l_24 << 3);
    l_20 = 0;
L1E29A:;
    if (l_20 < l_24) goto L1E2B3;
    goto L1E2BE;
L1E2A4:;
    l_20++;
    (*(char (**)[8])&l_14)++;
    goto L1E29A;
L1E2B3:;
    if (*(int *)((char *)l_14 + 4) != a2) goto L1E2A4;
L1E2BE:;
    if (l_20 != l_24) goto L1E2D0;
    fatal_error((int)D_0017050E);
L1E2D0:;
    lseek(*(int *)maps_bsa, *(int *)((char *)l_14), 1);
    a1->index = l_20;
    location_read_record(a1, *(int *)maps_bsa);
    if (&loaded_location != a1) return;
    a1->data->object_counter = 2;
    a1->data->marker_counter = 64000;
    func_000A00CB(*(int *)maps_bsa, (int)D_00196AB0, 10);
    func_000A00CB(*(int *)maps_bsa, (int)dungeon_blocks, 128);
}

void location_load_exterior(struct loaded_location *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    func_000A0ED9(361, (int)D_001704CC);
    mc_sprintf((int)text_buffer, (int)D_00170522, (int)(unsigned char)*(signed char *)current_region);
    l_20 = archive_find_record(*(int *)maps_bsa, (int)text_buffer, 13);
    l_1C = archive_record_offset(*(int *)maps_bsa, l_20);
    lseek(*(int *)maps_bsa, (a2 << 2) + l_1C, 0);
    func_000A00CB(*(int *)maps_bsa, (int)&l_14, 4);
    lseek(*(int *)maps_bsa, l_14 + ((*(int *)D_00196A28 << 2) + l_1C), 0);
    a1->index = a2;
    location_read_record(a1, *(int *)maps_bsa);
    if (&loaded_location != a1) return;
    l_18 = a1->data->width * a1->data->height;
    a1->data->object_counter = 2;
    a1->data->marker_counter = 64000;
    func_000A00CB(*(int *)maps_bsa, (int)location_exterior, 412);
    func_000A00CB(*(int *)maps_bsa, (int)&l_24, 4);
}

void func_0001E502(struct loaded_location *a1, int a2, int a3)
{
    struct map_location *l_14;
    int l_10;

    l_14 = D_00196A9C;
    l_10 = 0;
L1E526:;
    if (l_10 < *(int *)D_00196A28) goto L1E542;
    return;
L1E533:;
    l_10++;
    l_14++;
    goto L1E526;
L1E542:;
    if (((l_14->x_type_flags << 2) >> 27) != a2) goto L1E56C;
    if (a3 != 0) goto L1E566;
    location_load_exterior(a1, l_10);
    return;
L1E566:;
    a3--;
L1E56C:;
    goto L1E533;
}

struct record *func_0001E576(void)
{
    struct record *l_1C;

    l_1C = object_create_child(D_00195AC4, 0, 429);
    l_1C->type = 38;
    l_1C->x = *(int *)block_origin_x;
    l_1C->y = func_0014B45B(*(int *)block_origin_x, *(int *)block_origin_z) - 8;
    l_1C->z = *(int *)block_origin_z - 4096;
    l_1C->pad13 = 32768;
    l_1C->id = D_00195AC4->id + current_location->object_counter++;
    return l_1C;
}

void rmb_index_records(void)
{
    int l_1C;
    int l_18;

    l_1C = *(int *)rmb_block + 6776;
    l_18 = 0;
L1E7CA:;
    if (((int)(unsigned char)*(signed char *)(*(char **)rmb_block)) > l_18) goto L1E7E5;
    goto L1E813;
L1E7DD:;
    l_18++;
    goto L1E7CA;
L1E7E5:;
    *(int *)(*(char **)rmb_block + 1475 + (l_18 << 2)) = l_1C;
    l_1C += *(int *)(*(char **)rmb_block + 1603 + (l_18 << 2));
    goto L1E7DD;
L1E813:;
    *(int *)(*(char **)rmb_block + 1731) = l_1C;
    *(int *)(*(char **)rmb_block + 1735) = (int)(*(char **)(*(char **)rmb_block + 1731) + (((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 1)) * 66));
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

    l_20 = ((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 1)) * 66;
    l_20 += ((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 2)) * 17;
    l_34 = object_create_child(a1, 0, l_20);
    l_34->type = 56;
    l_34->model_count = (unsigned short)(unsigned char)*(signed char *)(*(char **)rmb_block + 1);
    l_34->flat_count = (unsigned short)(unsigned char)*(signed char *)(*(char **)rmb_block + 2);
    l_34->id = D_00195AC4->id;
    l_30 = (int)RECORD_DATA(l_34);
    l_2C = l_30 + (((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 1)) * 66);
    mc_memcpy(l_30, *(int *)(*(char **)rmb_block + 1731), ((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 1)) * 66, (int)D_001704CC, 565, 4);
    mc_memcpy(l_2C, *(int *)(*(char **)rmb_block + 1735), ((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 2)) * 17, (int)D_001704CC, 566, 4);
    l_24 = 0;
L1EA25:;
    if (((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 1)) > l_24) goto L1EA48;
    goto L1EA85;
L1EA39:;
    l_24++;
    (*(char (**)[66])&l_30)++;
    goto L1EA25;
L1EA48:;
    *(int *)((char *)l_30 + 4) = 0;
    *(int *)((char *)l_30 + 36) += *(int *)block_origin_x;
    *(int *)((char *)l_30 + 44) += *(int *)block_origin_z;
    *(int *)((char *)l_30 + 40) += func_0014B45B(*(int *)((char *)l_30 + 36), *(int *)((char *)l_30 + 44));
    goto L1EA39;
L1EA85:;
    l_24 = 0;
L1EA8C:;
    if (((int)(unsigned char)*(signed char *)(*(char **)rmb_block + 2)) > l_24) goto L1EAB2;
    goto L1EC01;
L1EAA3:;
    l_24++;
    (*(char (**)[17])&l_2C)++;
    goto L1EA8C;
L1EAB2:;
    *(int *)((char *)l_2C) += *(int *)block_origin_x;
    *(int *)((char *)l_2C + 8) += *(int *)block_origin_z;
    *(int *)((char *)l_2C + 4) += func_0014B45B(*(int *)((char *)l_2C), *(int *)((char *)l_2C + 8));
    if (*(short *)((char *)l_2C + 14) == 0) goto L1EB37;
    l_34 = rmb_make_flat(a1, (int)(short)*(short *)((char *)l_2C + 12), (int)(short)*(short *)((char *)l_2C + 14), 0);
    l_34->x = *(int *)((char *)l_2C);
    l_34->y = *(int *)((char *)l_2C + 4);
    l_34->z = *(int *)((char *)l_2C + 8);
    *(short *)((char *)l_2C + 12) = 0;
    goto L1EBFC;
L1EB37:;
    switch (((int)(unsigned short)*(short *)((char *)l_2C + 12)) >> 7) {
case 502:
case 503:
case 504:
case 510:
    *(short *)((char *)l_2C + 12) = (*(short *)nature_texture_archive << 7) + (*(short *)((char *)l_2C + 12) & 63);
    goto L1EBFC;
case 210:
L1EB96:;
    l_28 = func_00135DE4(((int)(unsigned short)*(short *)((char *)l_2C + 12)) >> 7, (int)(unsigned short)(*(short *)((char *)l_2C + 12) & 49));
    if (l_28 != 0) goto L1EBCB;
    func_00135E39();
    goto L1EB96;
L1EBCB:;
    if (((int)(unsigned short)*(short *)((char *)l_28 + 6)) >= 255) goto L1EBEC;
{
    int l_40;
    l_40 = (int)(unsigned short)*(short *)((char *)l_28 + 6);
    goto L1EBF3;
L1EBEC:;
    l_40 = 255;
L1EBF3:;
    *(signed char *)((char *)l_2C + 16) = *(signed char *)&l_40;
default:
L1EBFC:;
    goto L1EAA3;
L1EC01:;
    mc_memcpy(RECORD_DATA(a1), (int)&*(signed char *)(*(char **)rmb_block + 6347), 429, (int)D_001704CC, 607, 4);
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
    *(int *)D_00196808 = 0;
    *(int *)rmb_block = *(int *)D_00147954;
    *(int *)blocks_bsa = archive_open((int)D_0017053F, 0, 0);
    switch (current_location->kind) {
    goto L1ECD0;
case 0:
    if ((current_location->width * current_location->height) != 64) goto L1ECB8;
    *(int *)D_00196800 = 66;
    goto L1ECC2;
L1ECB8:;
    *(int *)D_00196800 = 50;
L1ECC2:;
    goto L1ECDA;
case 1:
    *(int *)D_00196800 = 33;
    goto L1ECDA;
default:
L1ECD0:;
    *(int *)D_00196800 = 0;
L1ECDA:;
    l_1C = 0;
L1ECE1:;
    if (current_location->height > l_1C) goto L1ED00;
    goto L1EE6E;
L1ECF8:;
    l_1C++;
    goto L1ECE1;
L1ED00:;
    l_20 = 0;
L1ED07:;
    if (current_location->width > l_20) goto L1ED26;
    goto L1EE69;
L1ED1E:;
    l_20++;
    goto L1ED07;
L1ED26:;
    *(int *)block_origin_x = D_00195AC4->x + (l_20 << 12);
    *(int *)block_origin_z = D_00195AC4->z + ((l_1C + 1) << 12);
    l_2C = func_0001E576();
    town_block_load_rmb((current_location->width * l_1C) + l_20);
    rmb_index_records();
    town_map_add_block(l_20, (int)&*(signed char *)((char *)(current_location->height - l_1C) - 1));
    town_block_apply_ground(*(int *)block_origin_x, *(int *)block_origin_z);
    l_24 = 0;
L1EDAC:;
    if (((int)(unsigned char)*(signed char *)(*(char **)rmb_block)) > l_24) goto L1EDCA;
    goto L1EE5C;
L1EDC2:;
    l_24++;
    goto L1EDAC;
L1EDCA:;
    func_0001E854(l_30, l_24);
    l_28 = rmb_add_building(l_2C, l_24);
    if ((l_28->flags & 8) != 0) goto L1EE3B;
    if (l_28->id != current_location->buildings[l_28->image].id) goto L1EE31;
    l_28->building_type = l_30->type;
    l_30++;
    goto L1EE3B;
L1EE31:;
    fatal_error((int)D_0017054A);
L1EE3B:;
    if ((l_28->flags & 8) == 0) goto L1EE57;
    l_28->flags &= ~0x8;
L1EE57:;
    goto L1EDC2;
L1EE5C:;
    func_0001E928(l_2C);
    goto L1ED1E;
L1EE69:;
    goto L1ECF8;
L1EE6E:;
    archive_close(*(int *)blocks_bsa);
}
}

void tavern_frame(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (tavern_open(0) == 0) return;
    func_0012DB50(4);
    *(signed char *)D_0012B508 = 146;
    if (((struct bf8_2_1 *)&D_001940D4)->f == 0) goto L1EF73;
    l_1C = picklist_update();
    if (l_1C > (-1)) goto L1EF75;
L1EF73:;
    goto L1EF7D;
L1EF75:;
    tavern_buy_food(l_1C);
L1EF7D:;
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0) return;
    if (((int)(signed char)*(signed char *)tavern_state) != 1) goto L1EFA0;
    tavern_room_offer();
    return;
L1EFA0:;
    if (((int)(signed char)*(signed char *)tavern_state) != 2) goto L1EFB6;
    tavern_room_pay();
    return;
L1EFB6:;
    func_00097B2A();
    l_18 = *(int *)tavern_menu_image;
    func_00144F68((int)(unsigned short)*(short *)((char *)l_18), (int)(unsigned short)*(short *)((char *)l_18 + 2), (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
    if (*(signed char *)key_down_esc == 0) goto L1F003;
    tavern_close();
L1F003:;
    if (*(signed char *)mouse_buttons == 0) goto L1F020;
    if (*(signed char *)mouse_buttons == 0) goto L1F01E;
    if (*(signed char *)mouse_buttons_prev != 0) goto L1F020;
L1F01E:;
    goto L1F025;
L1F020:;
    return;
L1F025:;
    l_20 = 0;
L1F02C:;
    if (l_20 < 4) goto L1F03F;
    return;
L1F037:;
    l_20++;
    goto L1F02C;
L1F03F:;
    if (*(short *)mouse_x <= *(short *)(tavern_buttons + (l_20 * 12))) goto L1F067;
    if (*(short *)mouse_x < *(short *)(D_00179E28 + (l_20 * 12))) goto L1F069;
L1F067:;
    goto L1F07D;
L1F069:;
    if (*(short *)mouse_y > *(short *)(D_00179E26 + (l_20 * 12))) goto L1F07F;
L1F07D:;
    goto L1F093;
L1F07F:;
    if (*(short *)mouse_y < *(short *)(D_00179E2A + (l_20 * 12))) goto L1F095;
L1F093:;
    goto L1F0B6;
L1F095:;
    sound_play(203, player_object, 110);
    ((int (*)())(*(int *)(D_00179E2C + (l_20 * 12))))();
    return;
L1F0B6:;
    goto L1F037;
}
