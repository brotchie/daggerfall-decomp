/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006D6A4 */
#include <dos.h>
extern char D_00175D00[];        /* __FILE__ */
extern char D_00175D88[];
extern char D_00175D8F[];
extern char D_00175D95[];
extern char D_00175D97[];
extern char D_00175D9A[];
extern void file_index_add_dir(char *);
extern void file_index_add_name(char *);
extern void mc_strncpy(char *, char *, int, char *, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int strlen(char *);
extern int stricmp(char *, char *);
extern int mc_sprintf(char *, char *, ...);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

void file_index_scan(char *dir)
{
    int rc;
    int i;
    char path[80];
    struct find_t ff;

    if (dir[strlen(dir) - 1] != '\\') {
        mc_set_location(337, D_00175D00);
        mc_sprintf(path, D_00175D88, dir);
    } else {
        mc_set_location(339, D_00175D00);
        mc_sprintf(path, D_00175D8F, dir);
    }
    rc = func_000A13DA(path, 16, &ff);
    while (rc == 0) {
        if ((ff.attrib & 16) == 0)
            file_index_add_name(ff.name);
        rc = func_000A13F7(&ff);
    }
    rc = func_000A13DA(path, 16, &ff);
    while (rc == 0) {
        if (ff.attrib & 16) {
            if (stricmp(ff.name, D_00175D95) == 0 || stricmp(ff.name, D_00175D97) == 0) {
                rc = func_000A13F7(&ff);
                continue;
            }
            file_index_add_dir(ff.name);
            mc_strncpy(&path[strlen(path) - 3], ff.name, 4, D_00175D00, 359);
            file_index_scan(path);
            i = strlen(path) - 1;
            while (i != 0 && path[i] != '\\')
                i--;
            mc_strncpy(path + i, D_00175D9A, 4, D_00175D00, 363);
        }
        rc = func_000A13F7(&ff);
    }
}
