/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E614 */
struct rec13 { char pad[4]; char c4, c5, c6, c7; char pad2[5]; };
struct bits { unsigned char lo:4; unsigned char b4:1; unsigned char b5:2; };
extern char D_001704CC[];
extern char D_00170530[];
extern char D_00179E10[];
extern struct rec13 rmb_name_templates[];
extern char text_buffer[];
extern char D_001968BA;
extern unsigned char location_block_indexes[];
extern unsigned char location_block_numbers[];
extern struct bits D_0019693D[];
extern int rmb_block;
extern int blocks_bsa;
extern char cfg_block_str[];
extern char cfg_debug;
extern int archive_find_record(int, char *, int);
extern void archive_read_record(int, int, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern char *func_000A0DD9(int, char *, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern void func_000A0F5C(char *, char *, ...);

void town_block_load_rmb(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    char l_18[4];

    l_24 = location_block_indexes[a1];
    if (D_0019693D[a1].b4)
        rmb_name_templates[l_24].c4 = D_001968BA;
    else
        rmb_name_templates[l_24].c4 = 'A';
    rmb_name_templates[l_24].c5 = D_00179E10[D_0019693D[a1].b5];
    func_000A0DD9(location_block_numbers[a1], l_18, 10);
    if (l_24 == 13 || l_24 == 14) {
        rmb_name_templates[l_24].c6 = D_0019693D[a1].lo + 'A';
        rmb_name_templates[l_24].c7 = l_18[0];
    } else if (location_block_numbers[a1] < 10) {
        rmb_name_templates[l_24].c6 = '0';
        rmb_name_templates[l_24].c7 = l_18[0];
    } else {
        rmb_name_templates[l_24].c6 = l_18[0];
        rmb_name_templates[l_24].c7 = l_18[1];
    }
    if (cfg_debug == 0) {
        func_000A0ED9(476, D_001704CC);
        func_000A0F5C(text_buffer, D_00170530, &rmb_name_templates[l_24]);
    } else {
        func_000A0AD9(text_buffer, cfg_block_str, 160, D_001704CC, 478);
    }
    l_1C = archive_find_record(blocks_bsa, text_buffer, 8);
    archive_read_record(blocks_bsa, l_1C, rmb_block);
}
