/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006D873 */
#include <dos.h>
#include "clib.h"
extern char D_00175D00[];        /* __FILE__ */
extern char D_00175D60[];
#pragma aux mc_set_location parm routine [];
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

void disk_delete_matching(char *dir, char *pattern)
{
    int rc;
    char path[80];
    struct find_t ff;
    int unused;

    mc_set_location(376, D_00175D00);
    mc_sprintf(path, D_00175D60, dir, pattern);
    rc = func_000A13DA(path, 0, &ff);
    while (rc == 0) {
        mc_set_location(380, D_00175D00);
        mc_sprintf(path, D_00175D60, dir, ff.name);
        unlink(path);
        rc = func_000A13F7(&ff);
    }
}
