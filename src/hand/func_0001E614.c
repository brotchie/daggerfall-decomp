/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E614 */
#include "records.h"
struct bits { unsigned char lo:4; unsigned char b4:1; unsigned char b5:2; };
extern char D_001704CC[];
extern char D_00170530[];
extern char D_00179E10[];
extern char rmb_name_templates[][13];   /* 'TVRN????.RMB'... */
extern signed char text_buffer[];
extern char D_001968BA;
extern unsigned char location_block_indexes[];
extern unsigned char location_block_numbers[];
extern struct bits location_block_letters[];
extern struct rmb_file *rmb_block;
extern int blocks_bsa;
extern char cfg_block_str[];
extern char cfg_debug;
extern int archive_find_record(int, char *, int);
extern int archive_read_record(int, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);
extern char *itoa(int, char *, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern void mc_sprintf(char *, char *, ...);

void town_block_load_rmb(int block_index)
{
    int template;
    int unused;
    int record;
    char digits[4];

    template = location_block_indexes[block_index];
    if (location_block_letters[block_index].b4)
        rmb_name_templates[template][4] = D_001968BA;
    else
        rmb_name_templates[template][4] = 'A';
    rmb_name_templates[template][5] = D_00179E10[location_block_letters[block_index].b5];
    itoa(location_block_numbers[block_index], digits, 10);
    if (template == 13 || template == 14) {
        rmb_name_templates[template][6] = location_block_letters[block_index].lo + 'A';
        rmb_name_templates[template][7] = digits[0];
    } else if (location_block_numbers[block_index] < 10) {
        rmb_name_templates[template][6] = '0';
        rmb_name_templates[template][7] = digits[0];
    } else {
        rmb_name_templates[template][6] = digits[0];
        rmb_name_templates[template][7] = digits[1];
    }
    if (cfg_debug == 0) {
        mc_set_location(476, D_001704CC);
        mc_sprintf(((char *)text_buffer), D_00170530, rmb_name_templates[template]);
    } else {
        mc_strncpy(((char *)text_buffer), cfg_block_str, 160, D_001704CC, 478);
    }
    record = archive_find_record(blocks_bsa, ((char *)text_buffer), 8);
    archive_read_record(blocks_bsa, record, (int)rmb_block);
}
