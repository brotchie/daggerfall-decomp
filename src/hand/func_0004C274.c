/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004C274 */
#include "records.h"

struct find_t {             /* DOS find buffer */
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    unsigned char name[13];
};
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

int quest_pick_file(unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, unsigned char a5)
{
    struct find_t ff;
    int rc;
    int n;
    char *p;

    p = scratch_buffer;
    n = 0;
    mc_set_location(323, D_00174F47);
    mc_sprintf(((char *)text_buffer), D_00174F69, arena2_cd_path);
    rc = func_000A13DA(((char *)text_buffer), 0, &ff);
    while (rc == 0) {
        if (ff.name[0] != a1 && ff.name[0] != a2)
            ;
        else if (ff.name[1] == a3)
            if (ff.name[2] == a4)
                if (ff.name[3] - '0' <= a5)
                    if ((game_settings->view_flags & 4) && (ff.name[4] == 'X' || ff.name[4] == 'Y'))
                        ;
                    else
                        n = quest_file_list_add(ff.name, n);
        rc = func_000A13F7(&ff);
    }
    mc_set_location(338, D_00174F47);
    mc_sprintf(((char *)text_buffer), D_00174F69, arena2_path);
    rc = func_000A13DA(((char *)text_buffer), 0, &ff);
    while (rc == 0) {
        if (ff.name[0] != a1 && ff.name[0] != a2)
            ;
        else if (ff.name[1] == a3)
            if (ff.name[2] == a4)
                if (ff.name[3] - '0' <= a5)
                    if ((game_settings->view_flags & 4) && (ff.name[4] == 'X' || ff.name[4] == 'Y'))
                        ;
                    else
                        n = quest_file_list_add(ff.name, n);
        rc = func_000A13F7(&ff);
    }
    if (n == 0)
        return 0;
    n = rand_range(0, n - 1);
    p = scratch_buffer;
    while (n != 0) {
        p += strlen(p) + 1;
        n--;
    }
    mc_strncpy(D_001961F5, p, 13, D_00174F47, 365);
    return 1;
}
