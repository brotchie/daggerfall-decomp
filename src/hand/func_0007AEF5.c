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
extern char text_buffer[];
extern char D_001917E4[];
extern void disk_copy_file(char *, char *, char *);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);   /* _dos_findfirst */
extern unsigned func_000A13F7(struct find_t *);                     /* _dos_findnext */
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

void save_copy_automap_files(int a1)
{
    int r;
    struct find_t f;

    func_000A0ED9(787, D_00176884);
    mc_sprintf(text_buffer, D_0017696F, D_001917E4);
    r = func_000A13DA(text_buffer, 0, &f);
    func_000A0ED9(789, D_00176884);
    mc_sprintf(text_buffer, D_00176909, a1);
    while (r == 0) {
        disk_copy_file(f.name, D_001917E4, text_buffer);
        r = func_000A13F7(&f);
    }
    func_000A0ED9(796, D_00176884);
    mc_sprintf(text_buffer, D_00176977, D_001917E4);
    r = func_000A13DA(text_buffer, 0, &f);
    func_000A0ED9(798, D_00176884);
    mc_sprintf(text_buffer, D_00176909, a1);
    while (r == 0) {
        disk_copy_file(f.name, D_001917E4, text_buffer);
        r = func_000A13F7(&f);
    }
}
