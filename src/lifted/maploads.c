/* maploads.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"
#include "portio.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern iptr D_00147954;
extern char D_001704CC[];
extern char D_001704D7[];
extern char D_001704E4[];
extern char D_001704F2[];
extern char D_00170500[];
extern char D_0017050E[];
extern char D_00170522[];
extern char D_0017053F[];
extern char D_0017054A[];
extern struct rect tavern_buttons[];
extern signed char text_buffer[];
extern signed char tavern_state;
extern signed char D_001940D4;
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern char *scratch_buffer;
extern signed char current_region;
extern signed char mouse_buttons_prev;
extern int region_location_type_counts[];
extern struct dungeon_block dungeon_blocks[];
extern int town_house_skip_percent;
extern int town_building_counter;
extern char location_exterior[];
extern int region_location_count;
extern struct rmb_file *rmb_block;
extern char region_dungeon_type_counts[];
extern char *D_00196A7C;
extern struct map_location *location_here;
extern int region_dungeon_count;
extern struct loaded_location loaded_location;
extern struct map_location *region_locations;
extern int blocks_bsa;
extern int block_origin_x;
extern int block_origin_z;
extern int maps_bsa;
extern char dungeon_header[];
extern short region_locations_region;
extern iptr tavern_menu_image;
extern char cfg_mapsave_file[];
extern short nature_texture_archive;

extern int archive_open(char *, iptr, int);
extern int archive_find_record(int, iptr, int);
extern int archive_record_size(int, int);
extern int archive_record_offset(int, int);
extern iptr archive_read_record(int, int, iptr);
extern int tavern_open(short);
extern int sound_play(int, struct record *, int);
extern int list_popup_update(void);
extern struct record *rmb_make_flat(struct record *, short, short, int);
extern struct record *rmb_add_building(struct record *, int);
extern struct map_location *region_find_location(int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int trade_settle_offer(void);
extern int xn_world_cell_at(int, int);
extern int xn_font_select(int);
extern void *xn_tex_cache_lookup_image(int, int);
extern void xn_tex_cache_flush(void);
extern void xn_draw_image(int, int, int, int, char *);
extern int xn_terrain_height_at(int, int);
extern void archive_close(int);
extern void archive_write_record(int, int, iptr);
extern void town_block_load_rmb(int);
extern void town_block_place_building(struct building *, int);
extern void tavern_close(void);
extern void tavern_room_offer(void);
extern void tavern_room_pay(void);
extern void tavern_buy_food(int);
extern void fatal_error(char *);
extern void town_block_apply_ground(int, int);
extern void town_map_add_block(int, int);
struct record *town_block_create_object(void);
void region_locations_load_discovered(int);
void region_locations_save_discovered(int);
void maploads_load_region(int);
void location_read_record(struct loaded_location *, int);
void location_load_exterior(struct loaded_location *, int);
void rmb_index_records(void);
void town_block_create_misc_objects(struct record *);
#pragma aux mc_set_location parm routine [];

void region_locations_load_discovered(int region)
{
    struct map_location *location;
    signed char *saved;
    int archive;
    int record;
    int i;
    iptr name;

    location = region_locations;
    saved = (signed char *)scratch_buffer;
    name = (iptr)mc_malloc(4096, D_001704CC, 57);
    mc_set_location(59, D_001704CC);
    mc_sprintf((char *)name, D_001704D7, region);
    archive = archive_open(cfg_mapsave_file, 0, 1);
    record = archive_find_record(archive, name, 12);
    archive_read_record(archive, record, (iptr)saved);
    archive_close(archive);
    for (i = 0; i < region_location_count; i++, location++, saved++) {
        if (((int)(unsigned char)(*saved & 64)) != 0) {
            location->x_type_flags |= 0x40000000;
        }
        if (((int)(unsigned char)(*saved & 128)) != 0) {
            location->x_type_flags |= 0x80000000;
        }
    }
    if (name == 0 || name == (-1751672937)) return;
    mc_free((void *)name, D_001704CC, 72);
    name = -1751672937;
}

void region_locations_save_discovered(int region)
{
    struct map_location *location;
    signed char *saved;
    int archive;
    int record;
    int i;
    iptr name;

    location = region_locations;
    saved = (signed char *)scratch_buffer;
    name = (iptr)mc_malloc(4096, D_001704CC, 88);
    for (i = 0; i < region_location_count; i++, location++, saved++) {
        *saved = rand() & -193;
        if ((location->x_type_flags & 0x40000000) != 0) *saved |= 64;
        if ((location->x_type_flags & 0x80000000) != 0) *saved |= 128;
    }
    mc_set_location(97, D_001704CC);
    mc_sprintf((char *)name, D_001704D7, region);
    archive = archive_open(cfg_mapsave_file, 0, 1);
    record = archive_find_record(archive, name, 12);
    archive_write_record(archive, record, (iptr)scratch_buffer);
    archive_close(archive);
    if (name == 0 || name == (-1751672937)) return;
    mc_free((void *)name, D_001704CC, 103);
    name = -1751672937;
}

void maploads_load_region(int region)
{
    struct map_location *location;
    int record;
    int size;
    int i;

    if ((iptr)region_locations != 0) {
        region_locations_save_discovered((int)(unsigned short)region_locations_region);
        if ((iptr)region_locations != 0 && (iptr)region_locations != (-1751672937)) {
            mc_free(region_locations, D_001704CC, 123);
            region_locations = (struct map_location *)(iptr)-1751672937;
        }
    }
    region_locations_region = region;
    mc_set_location(127, D_001704CC);
    mc_sprintf((char *)text_buffer, D_001704E4, region);
    record = archive_find_record(maps_bsa, (iptr)text_buffer, 13);
    size = archive_record_size(maps_bsa, record);
    region_location_count = ((unsigned)size) / 17;
    location = (struct map_location *)(*(iptr *)&region_locations = (iptr)mc_malloc(size, D_001704CC, 133));
    archive_read_record(maps_bsa, record, (iptr)region_locations);
    mc_memset(region_location_type_counts, 0, 56, D_001704CC, 137, 56);
    mc_memset(region_dungeon_type_counts, 0, 76, D_001704CC, 138, 76);
    region_dungeon_count = 0;
    for (i = 0; i < region_location_count; i++, location++) {
        (region_location_type_counts[((location->x_type_flags << 2) >> 27)])++;
        if (location->dungeon_type != 255) {
            (*(int *)(region_dungeon_type_counts + (location->dungeon_type << 2)))++;
            region_dungeon_count++;
        }
    }
    region_locations_load_discovered(region);
    location_here = (struct map_location *)region_find_location(xn_world_cell_at(player_object->x, player_object->z));
}

void region_load_location_names(int region)
{
    int record;
    int unused1;
    int unused2;
    int unused3;

    mc_set_location(170, D_001704CC);
    mc_sprintf((char *)text_buffer, D_001704F2, region);
    *(iptr *)&D_00196A7C = D_00147954;
    record = archive_find_record(maps_bsa, (iptr)text_buffer, 13);
    archive_read_record(maps_bsa, record, D_00147954);
}

void maploads_enter_region(int region)
{
    maploads_load_region(region);
}

void region_location_names_release(void)
{
    if ((iptr)D_00196A7C == 0) return;
    *(int *)&D_00196A7C = 0;
}

void location_read_record(struct loaded_location *location, int fd)
{
    int unused;

    read(fd, &location->door_count, 4);
    location->doors = (struct location_door *)mc_malloc(location->door_count * 6, D_001704CC, 219);
    read(fd, location->doors, location->door_count * 6);
    location->object = (struct record *)mc_malloc(LOCATION_RECORD_SIZE, D_001704CC, 223);
    location->data = &location->object->data.location;
    PORT_READ_LOCATION(fd, location->object, 119);
    if (location->data->building_count == 0) return;
    location->data->buildings = (struct building *)mc_malloc(location->data->building_count * 26, D_001704CC, 231);
    read(fd, location->data->buildings, location->data->building_count * 26);
}

void location_load_dungeon(struct loaded_location *location, int dungeon_index)
{
    int dungeon_count;
    int record;
    int offset;
    struct dungeon_entry *table;

    table = (struct dungeon_entry *)scratch_buffer;
    mc_set_location(248, D_001704CC);
    mc_sprintf((char *)text_buffer, D_00170500, (int)(unsigned char)current_region);
    record = archive_find_record(maps_bsa, (iptr)text_buffer, 13);
    offset = archive_record_offset(maps_bsa, record);
    lseek(maps_bsa, offset, 0);
    read(maps_bsa, &dungeon_count, 4);
    read(maps_bsa, table, dungeon_count << 3);
    offset = table[dungeon_index].offset;
    lseek(maps_bsa, offset, 1);
    location->index = dungeon_index;
    location_read_record(location, maps_bsa);
    if (&loaded_location != location) return;
    location->data->object_counter = 2;
    location->data->marker_counter = 64000;
    read(maps_bsa, dungeon_header, 10);
    read(maps_bsa, dungeon_blocks, 128);
}

void location_load_dungeon_by_id(struct loaded_location *location, int id)
{
    int dungeon_count;
    int i;
    int record;
    int offset;
    struct dungeon_entry *entry;

    entry = (struct dungeon_entry *)scratch_buffer;
    mc_set_location(287, D_001704CC);
    mc_sprintf((char *)text_buffer, D_00170500, (int)(unsigned char)current_region);
    record = archive_find_record(maps_bsa, (iptr)text_buffer, 13);
    offset = archive_record_offset(maps_bsa, record);
    lseek(maps_bsa, offset, 0);
    read(maps_bsa, &dungeon_count, 4);
    read(maps_bsa, entry, dungeon_count << 3);
    for (i = 0; i < dungeon_count; i++, entry++) {
        if (entry->id == id) break;
    }
    if (i == dungeon_count) fatal_error(D_0017050E);
    lseek(maps_bsa, entry->offset, 1);
    location->index = i;
    location_read_record(location, maps_bsa);
    if (&loaded_location != location) return;
    location->data->object_counter = 2;
    location->data->marker_counter = 64000;
    read(maps_bsa, dungeon_header, 10);
    read(maps_bsa, dungeon_blocks, 128);
}

void location_load_exterior(struct loaded_location *location, int location_index)
{
    int trailer;
    int record;
    int offset;
    int block_count;
    int location_offset;

    mc_set_location(361, D_001704CC);
    mc_sprintf((char *)text_buffer, D_00170522, (int)(unsigned char)current_region);
    record = archive_find_record(maps_bsa, (iptr)text_buffer, 13);
    offset = archive_record_offset(maps_bsa, record);
    lseek(maps_bsa, (location_index << 2) + offset, 0);
    read(maps_bsa, &location_offset, 4);
    lseek(maps_bsa, location_offset + ((region_location_count << 2) + offset), 0);
    location->index = location_index;
    location_read_record(location, maps_bsa);
    if (&loaded_location != location) return;
    block_count = location->data->width * location->data->height;
    location->data->object_counter = 2;
    location->data->marker_counter = 64000;
    read(maps_bsa, location_exterior, 412);
    read(maps_bsa, &trailer, 4);
}

void location_load_nth_of_type(struct loaded_location *location, int type, int n)
{
    struct map_location *map_location;
    int i;

    map_location = region_locations;
    for (i = 0; i < region_location_count; i++, map_location++) {
        if (((map_location->x_type_flags << 2) >> 27) == type) {
            if (n == 0) {
                location_load_exterior(location, i);
                return;
            }
            n--;
        }
    }
}

struct record *town_block_create_object(void)
{
    struct record *object;

    object = object_create_child(location_object, 0, 429);
    object->type = 38;
    object->x = block_origin_x;
    object->y = xn_terrain_height_at(block_origin_x, block_origin_z) - 8;
    object->z = block_origin_z - 4096;
    object->pad13 = 32768;
    object->id = location_object->id + current_location->object_counter++;
    return object;
}

void rmb_index_records(void)
{
    iptr cursor;
    int i;

    cursor = (iptr)rmb_block->data;
    for (i = 0; rmb_block->block_data_count > i; i++) {
        RMB_BLOCK_DATA(rmb_block, i) = (struct block *)cursor;
        cursor += rmb_block->block_data_sizes[i];
    }
    RMB_MISC_MODELS(rmb_block) = (struct block_model *)cursor;
    RMB_MISC_FLATS(rmb_block) = RMB_FLATS_AFTER_MODELS(RMB_MISC_MODELS(rmb_block), rmb_block->misc_model_count);
}

void town_block_create_misc_objects(struct record *block_object)
{
    struct record *object;
    struct block_model *model;
    struct block_flat *flat;
    struct texture_header *image;
    int i;
    int size;
    int unused1;
    int unused2;

    size = rmb_block->misc_model_count * REC_SIZEOF(struct block_model);
    size += rmb_block->misc_flat_count * REC_SIZEOF(struct block_flat);
    object = object_create_child(block_object, 0, size);
    object->type = 56;
    object->model_count = rmb_block->misc_model_count;
    object->flat_count = rmb_block->misc_flat_count;
    object->id = location_object->id;
    model = (struct block_model *)RECORD_DATA(object);
    flat = (struct block_flat *)(model + rmb_block->misc_model_count);
    PORT_COPY_BLOCK_MODELS(model, RMB_MISC_MODELS(rmb_block), rmb_block->misc_model_count, D_001704CC, 565);
    mc_memcpy(flat, RMB_MISC_FLATS(rmb_block), rmb_block->misc_flat_count * REC_SIZEOF(struct block_flat), D_001704CC, 566, 4);
    for (i = 0; rmb_block->misc_model_count > i; i++, model++) {
        model->model = 0;
        model->x += block_origin_x;
        model->z += block_origin_z;
        model->y += xn_terrain_height_at(model->x, model->z);
    }
    for (i = 0; rmb_block->misc_flat_count > i; i++, flat++) {
        flat->x += block_origin_x;
        flat->z += block_origin_z;
        flat->y += xn_terrain_height_at(flat->x, flat->z);
        if (flat->faction_id != 0) {
            object = rmb_make_flat(block_object, flat->image, flat->faction_id, 0);
            object->x = flat->x;
            object->y = flat->y;
            object->z = flat->z;
            flat->image = 0;
        } else {
            {
                int height;
                switch (flat->image >> 7) {
                case 502:
                case 503:
                case 504:
                case 510:
                    flat->image = (nature_texture_archive << 7) + (flat->image & 63);
                    break;
                case 210:
                    while ((image = (struct texture_header *)xn_tex_cache_lookup_image(flat->image >> 7, flat->image & 49)) == 0) {
                        xn_tex_cache_flush();
                    }
                    if (image->height < 255) {
                        height = image->height;
                    } else {
                        height = 255;
                    }
                    flat->flags = *(signed char *)&height;
                }
            }
        }
    }
    mc_memcpy(RECORD_DATA(block_object), rmb_block->name, 429, D_001704CC, 607, 4);
}

void town_load_blocks(void)
{
    struct building *building;
    struct record *block_object;
    struct record *building_object;
    int i;
    int block_x;
    int block_z;
    int unused;

    building = current_location->buildings;
    town_building_counter = 0;
    rmb_block = (struct rmb_file *)D_00147954;
    blocks_bsa = archive_open(D_0017053F, 0, 0);
    switch (current_location->kind) {
    case 0:
        if ((current_location->width * current_location->height) == 64) {
            town_house_skip_percent = 66;
        } else {
            town_house_skip_percent = 50;
        }
        break;
    case 1:
        town_house_skip_percent = 33;
        break;
    default:
        town_house_skip_percent = 0;
    }
    for (block_z = 0; current_location->height > block_z; block_z++) {
        for (block_x = 0; current_location->width > block_x; block_x++) {
            block_origin_x = location_object->x + (block_x << 12);
            block_origin_z = location_object->z + ((block_z + 1) << 12);
            block_object = town_block_create_object();
            town_block_load_rmb((current_location->width * block_z) + block_x);
            rmb_index_records();
            town_map_add_block(block_x, (int)(iptr)&*(signed char *)((char *)(iptr)(current_location->height - block_z) - 1));
            town_block_apply_ground(block_origin_x, block_origin_z);
            for (i = 0; rmb_block->block_data_count > i; i++) {
                town_block_place_building(building, i);
                building_object = rmb_add_building(block_object, i);
                if ((building_object->flags & 8) == 0) {
                    if (building_object->id == current_location->buildings[building_object->image].id) {
                        building_object->building_type = building->type;
                        building++;
                    } else {
                        fatal_error(D_0017054A);
                    }
                }
                if ((building_object->flags & 8) != 0) building_object->flags &= ~0x8;
            }
            town_block_create_misc_objects(block_object);
        }
    }
    archive_close(blocks_bsa);
}

void tavern_frame(void)
{
    int i;
    int row;
    struct image *image;

    if (tavern_open(0) == 0) return;
    xn_font_select(4);
    D_0012B508 = 146;
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0 && (row = list_popup_update()) > (-1)) {
        tavern_buy_food(row);
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
    trade_settle_offer();
    image = (struct image *)tavern_menu_image;
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    if (key_down_esc != 0) tavern_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (i = 0; i < 4; i++) {
        if (mouse_x > tavern_buttons[i].x0 && mouse_x < tavern_buttons[i].x1 && mouse_y > tavern_buttons[i].y0 && mouse_y < tavern_buttons[i].y1) {
            sound_play(203, player_object, 110);
            tavern_buttons[i].handler();
            return;
        }
    }
}
