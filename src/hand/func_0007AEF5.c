/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007AEF5 */
struct find_t {                 /* <dos.h> */
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
extern char D_00176884[];       /* __FILE__ */
extern char D_00176909[];
extern char D_0017696F[];
extern char D_00176977[];
extern signed char text_buffer[];
extern char arena2_path[];
extern void disk_copy_file(char *, char *, char *);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);   /* _dos_findfirst */
extern unsigned func_000A13F7(struct find_t *);                     /* _dos_findnext */
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void save_copy_automap_files(int slot)
{
    int r;
    struct find_t f;

    mc_set_location(787, D_00176884);
    mc_sprintf(((char *)text_buffer), D_0017696F, arena2_path);
    r = func_000A13DA(((char *)text_buffer), 0, &f);
    mc_set_location(789, D_00176884);
    mc_sprintf(((char *)text_buffer), D_00176909, slot);
    while (r == 0) {
        disk_copy_file(f.name, arena2_path, ((char *)text_buffer));
        r = func_000A13F7(&f);
    }
    mc_set_location(796, D_00176884);
    mc_sprintf(((char *)text_buffer), D_00176977, arena2_path);
    r = func_000A13DA(((char *)text_buffer), 0, &f);
    mc_set_location(798, D_00176884);
    mc_sprintf(((char *)text_buffer), D_00176909, slot);
    while (r == 0) {
        disk_copy_file(f.name, arena2_path, ((char *)text_buffer));
        r = func_000A13F7(&f);
    }
}
