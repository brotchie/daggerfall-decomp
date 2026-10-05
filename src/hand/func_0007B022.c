/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007B022 */
#pragma pack(1)
struct find_t {
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
extern char *D_00147954;
extern char D_00176884[];
extern char D_0017696F[];
extern char D_00176977[];
extern char arena2_path[];
extern void disk_copy_file(char *, int, char *);
extern void automap_delete_files(void);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void load_copy_automap_files(int a1)
{
    unsigned rc;
    struct find_t f;

    automap_delete_files();
    mc_set_location(813, D_00176884);
    mc_sprintf(D_00147954, D_0017696F, a1);
    rc = func_000A13DA(D_00147954, 0, &f);
    while (rc == 0) {
        disk_copy_file(f.name, a1, arena2_path);
        rc = func_000A13F7(&f);
    }
    mc_set_location(821, D_00176884);
    mc_sprintf(D_00147954, D_00176977, a1);
    rc = func_000A13DA(D_00147954, 0, &f);
    while (rc == 0) {
        disk_copy_file(f.name, a1, arena2_path);
        rc = func_000A13F7(&f);
    }
}
