/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00035D6D */
#include "records.h"

struct rec {
    signed char x;
    signed char y;
    unsigned short num:10;
    unsigned short pad:1;
    unsigned short kind:3;
};
struct hdr { int f0; int w; int f8; int tbl; };
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
extern char *D_001995E8;
extern int D_001995F8;
extern int D_001995FC;
extern struct hdr *D_00199604;
extern struct rec *D_00199608;
extern int D_00199614;
extern int archive_find_record(int, char *, int);
extern int archive_read_record(int, int, char *);
extern void rdb_create_objects(struct record *, char *, int);
extern void rdb_link_actions(struct record *, char *, int);
extern struct record *object_create_in_block(struct record *, unsigned char, int, int, int);
extern int xn_tex_cache_begin_frame();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void dungeon_load_rdb_block(struct rec *block)
{
    int i;
    int j;
    int *list_offsets;
    int record;
    struct record *rdi_object;
    char *object_list;

    D_00199608 = block;
    D_001995E8 = scratch_buffer;
    mc_set_location(59, D_00170AB4);
    mc_sprintf(((char *)text_buffer), D_00170ABC, D_0017A844[block->kind], block->num);
    record = archive_find_record(blocks_bsa, ((char *)text_buffer), 13);
    archive_read_record(blocks_bsa, record, D_001995E8);
    D_001995F8 = 16;
    D_001995FC = 10000;
    D_001962A1 = 0;
    D_00199614 = 0;
    D_00199604 = (struct hdr *)D_001995E8;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            D_001995D4[j][i] = object_create_in_block(location_object, 47, 0, 0, i * D_00199604->w + j);
            D_001995D4[j][i]->image = block->num;
            D_001995D4[j][i]->x = location_object->x + (j << 10) + (block->x << 11);
            D_001995D4[j][i]->z = location_object->z + (i << 10) + (block->y << 11);
        }
    }
    list_offsets = (int *)(D_00199604->tbl + D_001995E8);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (list_offsets[i * D_00199604->w + j] <= 0)
                continue;
            xn_tex_cache_begin_frame();
            object_list = list_offsets[i * D_00199604->w + j] + D_001995E8;
            rdb_create_objects(D_001995D4[j][i], object_list, i * D_00199604->w + j);
        }
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (list_offsets[i * D_00199604->w + j] <= 0)
                continue;
            object_list = list_offsets[i * D_00199604->w + j] + D_001995E8;
            rdb_link_actions(D_001995D4[j][i], object_list, i * D_00199604->w + j);
            *(short *)((char *)D_001995D4[j][i] + 23) = D_001995F8;
            *(short *)((char *)D_001995D4[j][i] + 25) = D_001995FC;
            *(short *)((char *)D_001995D4[j][i] + 19) = (unsigned short)D_001962A1;
        }
    }
    rdi_object = object_create_in_block(D_001995D4[0][0], 60, 512, 0, 0);
    mc_set_location(107, D_00170AB4);
    mc_sprintf(((char *)text_buffer), D_00170AC7, D_0017A844[block->kind], block->num);
    record = archive_find_record(blocks_bsa, ((char *)text_buffer), 13);
    archive_read_record(blocks_bsa, record, RECORD_DATA(rdi_object));
    rdi_object->x = D_001995D4[0][0]->x;
    rdi_object->z = D_001995D4[0][0]->z;
}
