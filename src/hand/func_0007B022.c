/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007B022 */
#include <dos.h>
#include "clib.h"
extern char *D_00147954;
extern char D_00176884[];
extern char D_0017696F[];
extern char D_00176977[];
extern char arena2_path[];
extern void disk_copy_file(char *, char *, char *);
extern void automap_delete_files(void);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux mc_set_location parm routine [];

void load_copy_automap_files(char *save_dir)
{
    unsigned rc;
    struct find_t f;

    automap_delete_files();
    mc_set_location(813, D_00176884);
    mc_sprintf(D_00147954, D_0017696F, save_dir);
    rc = func_000A13DA(D_00147954, 0, &f);
    while (rc == 0) {
        disk_copy_file(f.name, save_dir, arena2_path);
        rc = func_000A13F7(&f);
    }
    mc_set_location(821, D_00176884);
    mc_sprintf(D_00147954, D_00176977, save_dir);
    rc = func_000A13DA(D_00147954, 0, &f);
    while (rc == 0) {
        disk_copy_file(f.name, save_dir, arena2_path);
        rc = func_000A13F7(&f);
    }
}
