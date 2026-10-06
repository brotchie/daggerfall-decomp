/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007B0FC */
#include <dos.h>
#include "clib.h"
extern char *D_00147954;
extern char D_00176884[];
extern char D_001768DF[];
extern char D_0017696F[];
extern char D_00176977[];
extern char arena2_path[];
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux mc_set_location parm routine [];

void automap_delete_files(void)
{
    struct find_t ff;
    unsigned rc;

    mc_set_location(835, D_00176884);
    mc_sprintf(D_00147954, D_0017696F, arena2_path);
    rc = func_000A13DA(D_00147954, 0, &ff);
    while (rc == 0) {
        mc_set_location(839, D_00176884);
        mc_sprintf(D_00147954, D_001768DF, arena2_path, ff.name);
        unlink(D_00147954);
        rc = func_000A13F7(&ff);
    }
    mc_set_location(844, D_00176884);
    mc_sprintf(D_00147954, D_00176977, arena2_path);
    rc = func_000A13DA(D_00147954, 0, &ff);
    while (rc == 0) {
        mc_set_location(848, D_00176884);
        mc_sprintf(D_00147954, D_001768DF, arena2_path, ff.name);
        unlink(D_00147954);
        rc = func_000A13F7(&ff);
    }
}
