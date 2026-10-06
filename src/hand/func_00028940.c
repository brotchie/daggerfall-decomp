/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00028940 */
#include <dos.h>
#include "clib.h"
extern char D_001707AE[];
extern char D_001707D0[];
extern char D_001707DA[];
extern signed char text_buffer[];
extern char arena2_path[];
extern unsigned game_minutes;
extern int disk_open_data(char *);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux mc_set_location parm routine [];

void automap_purge_old_files(void)
{
    struct find_t ff;
    unsigned rc;
    unsigned t;
    int fh;

    mc_set_location(996, D_001707AE);
    mc_sprintf(((char *)text_buffer), D_001707D0, arena2_path);
    rc = func_000A13DA(((char *)text_buffer), 0, &ff);
    while (rc == 0) {
        fh = disk_open_data(ff.name);
        read(fh, &t, 4);
        close(fh);
        if (game_minutes - t > 129600)
            unlink(((char *)text_buffer));
        rc = func_000A13F7(&ff);
    }
    mc_set_location(1008, D_001707AE);
    mc_sprintf(((char *)text_buffer), D_001707DA, arena2_path);
    rc = func_000A13DA(((char *)text_buffer), 0, &ff);
    while (rc == 0) {
        fh = disk_open_data(ff.name);
        read(fh, &t, 4);
        close(fh);
        if (game_minutes - t > 129600)
            unlink(((char *)text_buffer));
        rc = func_000A13F7(&ff);
    }
}
