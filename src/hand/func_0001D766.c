/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001D766 */
#pragma pack(1)
struct rec {
    char pad[9];
    char flags;
    char kind;
    char name[9];
    short a2;
    int a3;
    int len;
    char *ptr;
};
#pragma pack()
extern char D_00170464[];
extern char D_001704BB[];
extern char text_rsc_buffer[];
extern char *game_minutes;
extern char D_00196295;
extern int rumor_file;
extern void quest_load_text(char *, int, int, int);
extern int disk_open_rw(char *);
extern int disk_file_exists(char *);
extern int func_0009DEA7(int);
extern int mc_memset();
extern int lseek(int, int, int);
extern int mc_strncpy();
extern int write(int, void *, int);
extern int func_000A0DF4(char *);

void rumor_add_quest(char *a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    struct rec r;

    if (disk_file_exists(D_001704BB) == 0) return;
    D_00196295 = 1;
    text_rsc_buffer[0] = 0;
    quest_load_text(a1, a2, 0, 0);
    if (text_rsc_buffer[0] == 0) return;
    if ((rumor_file = disk_open_rw(D_001704BB)) < 0) return;
    lseek(rumor_file, 0, 2);
    l_10 = (a4 & 2) ? 180 : 30;
    mc_memset(&r, 0, 34, D_00170464, 1808, 4);
    mc_strncpy(r.name, a1 + 6, 9, D_00170464, 1809);
    r.kind = a1[0];
    r.a2 = a2;
    r.a3 = a3;
    r.flags = a4;
    r.ptr = game_minutes + l_10 * 1440;
    r.len = func_000A0DF4(text_rsc_buffer) + 1;
    write(rumor_file, &r, 34);
    write(rumor_file, text_rsc_buffer, r.len);
    func_0009DEA7(rumor_file);
}
