/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004C274 */
#include "records.h"
#include <dos.h>

extern char D_00174F47[];        /* __FILE__ */
extern char D_00174F69[];
extern signed char text_buffer[];
extern char arena2_path[];
extern char arena2_cd_path[];
extern struct settings *game_settings;
extern char *scratch_buffer;
extern char D_001961F5[];
extern int quest_file_list_add(unsigned char *, int);
extern int rand_range(int, int);
extern void mc_strncpy(char *, char *, int, char *, int);
extern int strlen(char *);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

int quest_pick_file(unsigned char group_char, unsigned char group_char2, unsigned char second_char, unsigned char membership_char, unsigned char rank)
{
    struct find_t find_data;
    int status;
    int file_count;
    char *entry;

    entry = scratch_buffer;
    file_count = 0;
    mc_set_location(323, D_00174F47);
    mc_sprintf(((char *)text_buffer), D_00174F69, arena2_cd_path);
    status = func_000A13DA(((char *)text_buffer), 0, &find_data);
    while (status == 0) {
        if (find_data.name[0] != group_char && find_data.name[0] != group_char2)
            ;
        else if (find_data.name[1] == second_char)
            if (find_data.name[2] == membership_char)
                if (find_data.name[3] - '0' <= rank)
                    if ((game_settings->view_flags & 4) && (find_data.name[4] == 'X' || find_data.name[4] == 'Y'))
                        ;
                    else
                        file_count = quest_file_list_add(find_data.name, file_count);
        status = func_000A13F7(&find_data);
    }
    mc_set_location(338, D_00174F47);
    mc_sprintf(((char *)text_buffer), D_00174F69, arena2_path);
    status = func_000A13DA(((char *)text_buffer), 0, &find_data);
    while (status == 0) {
        if (find_data.name[0] != group_char && find_data.name[0] != group_char2)
            ;
        else if (find_data.name[1] == second_char)
            if (find_data.name[2] == membership_char)
                if (find_data.name[3] - '0' <= rank)
                    if ((game_settings->view_flags & 4) && (find_data.name[4] == 'X' || find_data.name[4] == 'Y'))
                        ;
                    else
                        file_count = quest_file_list_add(find_data.name, file_count);
        status = func_000A13F7(&find_data);
    }
    if (file_count == 0)
        return 0;
    file_count = rand_range(0, file_count - 1);
    entry = scratch_buffer;
    while (file_count != 0) {
        entry += strlen(entry) + 1;
        file_count--;
    }
    mc_strncpy(D_001961F5, entry, 13, D_00174F47, 365);
    return 1;
}
