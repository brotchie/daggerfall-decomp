/* names.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00176D8C[];
extern char D_00176D98[];
extern char D_00176DA0[];
extern char D_00176DA6[];
extern char D_00176DB0[];
extern char D_00176DB6[];
extern char D_00176DC2[];
extern char D_00176DD1[];
extern char D_00176DE0[];
extern char D_00176DE7[];
extern char D_00176DF1[];
extern char D_00176E06[];
extern char D_00176E18[];
extern char shop_name_last_words[];
extern char shop_name_first_words[];
extern char tavern_name_first_words[];
extern char tavern_name_last_words[];
extern char region_names[];
extern signed char D_001841E3[];
extern int D_00184872;
extern signed char text_buffer[];
extern char D_00190B44[];
extern signed char text_rsc_buffer[];
extern struct record *D_00195AC4;
extern struct character *player_character;
extern signed char current_region;
extern int namegen_part_offsets[];
extern char namegen_name[];
extern char namegen_part_name[];
extern char namegen_syllable[];
extern short namegen_file;
extern signed char input_digits_only;

extern int faction_find(short);
extern struct record *quest_find_site_for_building(struct building *);
extern int disk_open_data(int);
extern int guild_find_membership_by_kind(unsigned char);
extern int rand_range(int, int);
extern int func_0008B43B(unsigned char, unsigned char, int);
extern int name_generate_surname(unsigned char, unsigned char);
extern int inpstr_edit(int, short, short, short, short, short);
extern int rand();
extern int srand();
extern int func_0009DEA7();
extern int lseek();
extern int func_000A00CB();
extern int mc_strncpy();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A1054();
extern int strchr();
extern int func_00142790();
extern void parse_expand(int, int);
extern void parse_rsc_text(int, int, int);
extern void namegen_read_part(short, int);
int name_generate_first(unsigned char, unsigned char);
int str_list_random(int);
#pragma aux func_000A0ED9 parm routine [];

int name_generate(unsigned char a1, unsigned char a2)
{
    int l_20;

    *(signed char *)namegen_name = 0;
    namegen_file = disk_open_data((int)D_00176D8C);
    lseek((int)(short)namegen_file, ((int)(unsigned char)a1) * 48, 0);
    func_000A00CB((int)(short)namegen_file, (int)namegen_part_offsets, 48);
    mc_strncpy((int)namegen_name, name_generate_first((int)(unsigned char)a1, (int)(unsigned char)a2), 40, (int)D_00176D98, 90);
    l_20 = name_generate_surname((int)(unsigned char)a1, (int)(unsigned char)a2);
    if (*(signed char *)((char *)l_20) != 0) {
        func_000A1054((int)namegen_name, (int)D_00176DA0, (int)D_00176D98, 96, 40);
        func_000A1054((int)namegen_name, l_20, (int)D_00176D98, 97, 40);
    }
    func_0009DEA7((int)(short)namegen_file);
    return (int)namegen_name;
}

int name_generate_first(unsigned char a1, unsigned char a2)
{
    short l_1C;

    {
        int l_2C;
        int l_28;
        switch ((unsigned char)a1) {
        case 1:
            namegen_read_part((int)(short)namegen_file, 0);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 156);
            namegen_read_part((int)(short)namegen_file, 1);
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 158, 30);
            namegen_read_part((int)(short)namegen_file, 2);
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 160, 30);
            if (a2 == 0 && (rand() % 100) < 75) {
                namegen_read_part((int)(short)namegen_file, 3);
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 165, 30);
            }
            if (a2 != 0) {
                namegen_read_part((int)(short)namegen_file, 4);
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 171, 30);
            }
            break;
        case 2:
            if (a2 != 0) {
                l_28 = 2;
            } else {
                l_28 = 0;
            }
            *(int *)&l_1C = l_28;
            namegen_read_part((int)(short)namegen_file, (int)(short)l_1C);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 177);
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 1));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 179, 30);
            break;
        case 8:
            *(int *)&l_1C = 0;
            namegen_read_part((int)(short)namegen_file, (int)(short)l_1C);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 184);
            if ((rand() % 50) < 25) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 1));
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 189, 30);
            }
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 2));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 193, 30);
            break;
        case 9:
            *(int *)&l_1C = 0;
            namegen_read_part((int)(short)namegen_file, (int)(short)l_1C);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 199);
            if ((rand() % 50) < 25) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 1));
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 204, 30);
            }
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 2));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 208, 30);
            if (a2 != 0) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 3));
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 213, 30);
            }
            break;
        case 10:
            *(int *)&l_1C = 0;
            if (a2 == 0 && (rand() % 100) < 25) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 3));
                mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 223);
                func_000A1054((int)namegen_part_name, (int)D_00176DA0, (int)D_00176D98, 224, 30);
                namegen_read_part((int)(short)namegen_file, (int)(short)l_1C);
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 226, 30);
            } else {
                namegen_read_part((int)(short)namegen_file, (int)(short)l_1C);
                mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 231);
            }
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 1));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 235, 30);
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 2));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 237, 30);
            break;
        default:
            if (a2 != 0) {
                l_2C = 2;
            } else {
                l_2C = 0;
            }
            *(int *)&l_1C = l_2C;
            namegen_read_part((int)(short)namegen_file, (int)(short)l_1C);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 242);
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&l_1C + 1));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 244, 30);
        }
        return (int)namegen_part_name;
    }
}

int building_name(struct building *a1)
{
    int l_30;
    int l_2C;
    struct faction *l_28;
    int l_24;
    int l_20;
    struct record *l_1C;

    if (a1 == 0) return (int)D_00176DA6;
    l_30 = rand();
    if (a1->id == player_character->house) {
        parse_expand(D_00184872, (int)D_00190B44);
        return (int)D_00190B44;
    }
    if (((int)(unsigned short)(a1->name_seed & 32768)) != 0) {
        parse_rsc_text(a1->name_seed & 0x7FFF, 0, 0);
        return (int)text_rsc_buffer;
    }
    srand(a1->name_seed);
    switch (a1->type) {
    case 0:
    case 2:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 12:
    case 13:
        func_000A0ED9(308, (int)D_00176D98);
        mc_sprintf((int)text_buffer, (int)D_00176DB0, str_list_random((int)shop_name_first_words), str_list_random(*(int *)(shop_name_last_words + (a1->type << 2))));
        parse_expand((int)text_buffer, (int)text_rsc_buffer);
        break;
    case 1:
        if (((D_00195AC4->id & -65536) + a1->id) == player_character->house) {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176DB6, 2048, (int)D_00176D98, 313);
        } else {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176DC2, 2048, (int)D_00176D98, 315);
        }
        break;
    case 3:
        func_000A0ED9(318, (int)D_00176D98);
        mc_sprintf((int)text_rsc_buffer, (int)D_00176DD1, *(int *)(region_names + (((int)(unsigned char)current_region) << 2)));
        break;
    case 11:
        l_28 = (struct faction *)faction_find((int)(short)a1->faction_id);
        if (l_28 != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)l_28->name, 2048, (int)D_00176D98, 323);
        }
        break;
    case 14:
        l_28 = (struct faction *)faction_find((int)(short)a1->faction_id);
        if (l_28->child != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)l_28->child->name, 2048, (int)D_00176D98, 332);
        } else if (l_28 != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)l_28->name, 2048, (int)D_00176D98, 334);
        }
        break;
    case 15:
        func_000A0ED9(341, (int)D_00176D98);
        mc_sprintf((int)text_rsc_buffer, (int)D_00176DB0, str_list_random((int)tavern_name_first_words), str_list_random((int)tavern_name_last_words));
        break;
    case 16:
        func_000A0ED9(344, (int)D_00176D98);
        mc_sprintf((int)text_rsc_buffer, (int)D_00176DE0);
        break;
    case 23:
        mc_strncpy((int)text_rsc_buffer, (int)D_00176DE7, 2048, (int)D_00176D98, 347);
        break;
    default:
        if (a1->faction_id == 108 && guild_find_membership_by_kind(0) != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176DF1, 2048, (int)D_00176D98, 351);
        } else if (a1->faction_id == 42 && guild_find_membership_by_kind(3) != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176E06, 2048, (int)D_00176D98, 353);
        } else {
            l_1C = quest_find_site_for_building(a1);
            if (l_1C != 0) {
                if (l_1C != 0 && (l_1C->type == 41 || l_1C->type == 8)) {
                    l_24 = func_0008B43B((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)((signed char)l_1C->flags & 4), l_1C->name_seed);
                } else {
                    l_24 = func_0008B43B((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], 0, (((unsigned)a1->id) >> 16) ^ a1->id);
                }
                l_20 = strchr(l_24, 32);
                if (l_20 != 0) l_24 = l_20 + 1;
                func_000A0ED9(363, (int)D_00176D98);
                mc_sprintf((int)text_rsc_buffer, (int)D_00176E18, l_24);
            } else {
                mc_strncpy((int)text_rsc_buffer, (int)D_00176DA6, 2048, (int)D_00176D98, 366);
            }
        }
    }
    srand(l_30);
    return (int)text_rsc_buffer;
}

int str_list_random(int a1)
{
    int l_1C;

    l_1C = 0;
    while (*(int *)((char *)((l_1C++ << 2) + a1)) != 0);
    l_1C = rand_range(0, l_1C - 2);
    return *(int *)((char *)((l_1C << 2) + a1));
}

void func_0008C286(int a1, short a2, int a3, short a4, short a5, short a6)
{
    func_00142790();
    input_digits_only = 1;
    inpstr_edit(a1, (int)(short)a2, (int)(short)*(short *)&a3, (int)(short)a4, (int)(short)a5, (int)(short)a6);
    input_digits_only = 0;
}
