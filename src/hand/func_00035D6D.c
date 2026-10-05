/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00035D6D */
#include "records.h"

extern char D_00170AB4[];       /* __FILE__ */
extern char D_00170ABC[];
extern char D_00170AC7[];
extern unsigned char D_0017A844[];
extern signed char text_buffer[];
extern struct record *location_object;
extern char *scratch_buffer;
extern unsigned char D_001962A1;
extern int blocks_bsa;
extern struct record *D_001995D4[2][2];
extern char *rdb_data;
extern int D_001995F8;
extern int D_001995FC;
extern struct rdb_file *rdb_loaded_file;
extern struct dungeon_block *rdb_dungeon_block;
extern int rdb_object_id_count;
extern int archive_find_record(int, char *, int);
extern int archive_read_record(int, int, char *);
extern void rdb_create_objects(struct record *, struct rdb_object *, int);
extern void rdb_link_actions(struct record *, struct rdb_object *, int);
extern struct record *object_create_in_block(struct record *, int, int, int, int);
extern int xn_tex_cache_begin_frame();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void dungeon_load_rdb_block(struct dungeon_block *block)
{
    int i;
    int j;
    int *list_offsets;
    int record;
    struct record *rdi_object;
    struct rdb_object *object_list;

    rdb_dungeon_block = block;
    rdb_data = scratch_buffer;
    mc_set_location(59, D_00170AB4);
    mc_sprintf(((char *)text_buffer), D_00170ABC, D_0017A844[block->index], block->number);
    record = archive_find_record(blocks_bsa, ((char *)text_buffer), 13);
    archive_read_record(blocks_bsa, record, rdb_data);
    D_001995F8 = 16;
    D_001995FC = 10000;
    D_001962A1 = 0;
    rdb_object_id_count = 0;
    rdb_loaded_file = (struct rdb_file *)rdb_data;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            D_001995D4[j][i] = object_create_in_block(location_object, 47, 0, 0, i * rdb_loaded_file->width + j);
            D_001995D4[j][i]->image = block->number;
            D_001995D4[j][i]->x = location_object->x + (j << 10) + (block->x << 11);
            D_001995D4[j][i]->z = location_object->z + (i << 10) + (block->z << 11);
        }
    }
    list_offsets = (int *)(rdb_loaded_file->object_root_offset + rdb_data);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (list_offsets[i * rdb_loaded_file->width + j] <= 0)
                continue;
            xn_tex_cache_begin_frame();
            object_list = (struct rdb_object *)(list_offsets[i * rdb_loaded_file->width + j] + rdb_data);
            rdb_create_objects(D_001995D4[j][i], object_list, i * rdb_loaded_file->width + j);
        }
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (list_offsets[i * rdb_loaded_file->width + j] <= 0)
                continue;
            object_list = (struct rdb_object *)(list_offsets[i * rdb_loaded_file->width + j] + rdb_data);
            rdb_link_actions(D_001995D4[j][i], object_list, i * rdb_loaded_file->width + j);
            D_001995D4[j][i]->light_level = D_001995F8;
            D_001995D4[j][i]->water_level = D_001995FC;
            D_001995D4[j][i]->pad13 = D_001962A1;
        }
    }
    rdi_object = object_create_in_block(D_001995D4[0][0], 60, 512, 0, 0);
    mc_set_location(107, D_00170AB4);
    mc_sprintf(((char *)text_buffer), D_00170AC7, D_0017A844[block->index], block->number);
    record = archive_find_record(blocks_bsa, ((char *)text_buffer), 13);
    archive_read_record(blocks_bsa, record, RECORD_DATA(rdi_object));
    rdi_object->x = D_001995D4[0][0]->x;
    rdi_object->z = D_001995D4[0][0]->z;
}
