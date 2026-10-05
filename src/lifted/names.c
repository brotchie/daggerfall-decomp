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
extern char **shop_name_last_words[];
extern char *shop_name_first_words[];
extern char *tavern_name_first_words[];
extern char *tavern_name_last_words[];
extern char region_names[];
extern signed char D_001841E3[];
extern int D_00184872;
extern signed char text_buffer[];
extern char D_00190B44[];
extern signed char text_rsc_buffer[];
extern struct record *location_object;
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
extern int name_generate_seeded(unsigned char, unsigned char, int);
extern int name_generate_surname(unsigned char, unsigned char);
extern int inpstr_edit(int, short, short, short, short, short);
extern int rand();
extern int srand();
extern int close();
extern int lseek();
extern int read();
extern int mc_strncpy();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A1054();
extern int strchr();
extern int xn_kbd_flush();
extern void parse_expand(int, int);
extern void parse_rsc_text(int, int, int);
extern void namegen_read_part(short, int);
int name_generate_first(unsigned char, unsigned char);
int str_list_random(char **);
#pragma aux mc_set_location parm routine [];

int name_generate(unsigned char bank, unsigned char female)
{
    char *surname;

    *(signed char *)namegen_name = 0;
    namegen_file = disk_open_data((int)D_00176D8C);
    lseek((int)(short)namegen_file, ((int)(unsigned char)bank) * 48, 0);
    read((int)(short)namegen_file, (int)namegen_part_offsets, 48);
    mc_strncpy((int)namegen_name, name_generate_first((int)(unsigned char)bank, (int)(unsigned char)female), 40, (int)D_00176D98, 90);
    surname = (char *)name_generate_surname((int)(unsigned char)bank, (int)(unsigned char)female);
    if (*surname != 0) {
        func_000A1054((int)namegen_name, (int)D_00176DA0, (int)D_00176D98, 96, 40);
        func_000A1054((int)namegen_name, surname, (int)D_00176D98, 97, 40);
    }
    close((int)(short)namegen_file);
    return (int)namegen_name;
}

int name_generate_first(unsigned char bank, unsigned char female)
{
    short part;

    {
        int default_part;
        int nord_part;
        switch ((unsigned char)bank) {
        case 1:
            namegen_read_part((int)(short)namegen_file, 0);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 156);
            namegen_read_part((int)(short)namegen_file, 1);
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 158, 30);
            namegen_read_part((int)(short)namegen_file, 2);
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 160, 30);
            if (female == 0 && (rand() % 100) < 75) {
                namegen_read_part((int)(short)namegen_file, 3);
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 165, 30);
            }
            if (female != 0) {
                namegen_read_part((int)(short)namegen_file, 4);
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 171, 30);
            }
            break;
        case 2:
            if (female != 0) {
                nord_part = 2;
            } else {
                nord_part = 0;
            }
            *(int *)&part = nord_part;
            namegen_read_part((int)(short)namegen_file, (int)(short)part);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 177);
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 1));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 179, 30);
            break;
        case 8:
            *(int *)&part = 0;
            namegen_read_part((int)(short)namegen_file, (int)(short)part);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 184);
            if ((rand() % 50) < 25) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 1));
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 189, 30);
            }
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 2));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 193, 30);
            break;
        case 9:
            *(int *)&part = 0;
            namegen_read_part((int)(short)namegen_file, (int)(short)part);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 199);
            if ((rand() % 50) < 25) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 1));
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 204, 30);
            }
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 2));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 208, 30);
            if (female != 0) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 3));
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 213, 30);
            }
            break;
        case 10:
            *(int *)&part = 0;
            if (female == 0 && (rand() % 100) < 25) {
                namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 3));
                mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 223);
                func_000A1054((int)namegen_part_name, (int)D_00176DA0, (int)D_00176D98, 224, 30);
                namegen_read_part((int)(short)namegen_file, (int)(short)part);
                func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 226, 30);
            } else {
                namegen_read_part((int)(short)namegen_file, (int)(short)part);
                mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 231);
            }
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 1));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 235, 30);
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 2));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 237, 30);
            break;
        default:
            if (female != 0) {
                default_part = 2;
            } else {
                default_part = 0;
            }
            *(int *)&part = default_part;
            namegen_read_part((int)(short)namegen_file, (int)(short)part);
            mc_strncpy((int)namegen_part_name, (int)namegen_syllable, 30, (int)D_00176D98, 242);
            namegen_read_part((int)(short)namegen_file, (int)(short)(*(int *)&part + 1));
            func_000A1054((int)namegen_part_name, (int)namegen_syllable, (int)D_00176D98, 244, 30);
        }
        return (int)namegen_part_name;
    }
}

int building_name(struct building *building)
{
    int saved_seed;
    int unused;
    struct faction *faction;
    char *resident_name;
    char *space;
    struct record *quest_object;

    if (building == 0) return (int)D_00176DA6;
    saved_seed = rand();
    if (building->id == player_character->house) {
        parse_expand(D_00184872, (int)D_00190B44);
        return (int)D_00190B44;
    }
    if (((int)(unsigned short)(building->name_seed & 32768)) != 0) {
        parse_rsc_text(building->name_seed & 0x7FFF, 0, 0);
        return (int)text_rsc_buffer;
    }
    srand(building->name_seed);
    switch (building->type) {
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
        mc_set_location(308, (int)D_00176D98);
        mc_sprintf((int)text_buffer, (int)D_00176DB0, str_list_random(shop_name_first_words), str_list_random(shop_name_last_words[building->type]));
        parse_expand((int)text_buffer, (int)text_rsc_buffer);
        break;
    case 1:
        if (((location_object->id & -65536) + building->id) == player_character->house) {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176DB6, 2048, (int)D_00176D98, 313);
        } else {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176DC2, 2048, (int)D_00176D98, 315);
        }
        break;
    case 3:
        mc_set_location(318, (int)D_00176D98);
        mc_sprintf((int)text_rsc_buffer, (int)D_00176DD1, *(int *)(region_names + (((int)(unsigned char)current_region) << 2)));
        break;
    case 11:
        faction = (struct faction *)faction_find((int)(short)building->faction_id);
        if (faction != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)faction->name, 2048, (int)D_00176D98, 323);
        }
        break;
    case 14:
        faction = (struct faction *)faction_find((int)(short)building->faction_id);
        if (faction->child != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)faction->child->name, 2048, (int)D_00176D98, 332);
        } else if (faction != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)faction->name, 2048, (int)D_00176D98, 334);
        }
        break;
    case 15:
        mc_set_location(341, (int)D_00176D98);
        mc_sprintf((int)text_rsc_buffer, (int)D_00176DB0, str_list_random(tavern_name_first_words), str_list_random(tavern_name_last_words));
        break;
    case 16:
        mc_set_location(344, (int)D_00176D98);
        mc_sprintf((int)text_rsc_buffer, (int)D_00176DE0);
        break;
    case 23:
        mc_strncpy((int)text_rsc_buffer, (int)D_00176DE7, 2048, (int)D_00176D98, 347);
        break;
    default:
        if (building->faction_id == 108 && guild_find_membership_by_kind(0) != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176DF1, 2048, (int)D_00176D98, 351);
        } else if (building->faction_id == 42 && guild_find_membership_by_kind(3) != 0) {
            mc_strncpy((int)text_rsc_buffer, (int)D_00176E06, 2048, (int)D_00176D98, 353);
        } else {
            quest_object = quest_find_site_for_building(building);
            if (quest_object != 0) {
                if (quest_object != 0 && (quest_object->type == 41 || quest_object->type == 8)) {
                    resident_name = (char *)name_generate_seeded((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)((signed char)quest_object->flags & 4), quest_object->name_seed);
                } else {
                    resident_name = (char *)name_generate_seeded((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], 0, (((unsigned)building->id) >> 16) ^ building->id);
                }
                space = (char *)strchr(resident_name, 32);
                if (space != 0) resident_name = space + 1;
                mc_set_location(363, (int)D_00176D98);
                mc_sprintf((int)text_rsc_buffer, (int)D_00176E18, resident_name);
            } else {
                mc_strncpy((int)text_rsc_buffer, (int)D_00176DA6, 2048, (int)D_00176D98, 366);
            }
        }
    }
    srand(saved_seed);
    return (int)text_rsc_buffer;
}

int str_list_random(char **list)
{
    int entry;

    entry = 0;
    while (list[entry++] != 0);
    entry = rand_range(0, entry - 2);
    return (int)list[entry];
}

void input_edit_number_box(char *text, short x, int y, short width, short height, short max_length)
{
    xn_kbd_flush();
    input_digits_only = 1;
    inpstr_edit((int)text, (int)(short)x, (int)(short)*(short *)&y, (int)(short)width, (int)(short)height, (int)(short)max_length);
    input_digits_only = 0;
}
