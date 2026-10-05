/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001D766 */
#include "records.h"

extern char D_00170464[];
extern char D_001704BB[];
extern signed char text_rsc_buffer[];
extern char *game_minutes;
extern char D_00196295;
extern int rumor_file;
extern void quest_load_text(struct quest *, int, int, int);
extern int disk_open_rw(char *);
extern int disk_file_exists(char *);
extern int func_0009DEA7(int);
extern int mc_memset();
extern int lseek(int, int, int);
extern int mc_strncpy();
extern int write(int, void *, int);
extern int func_000A0DF4(char *);

void rumor_add_quest(struct quest *a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    struct rumor r;

    if (disk_file_exists(D_001704BB) == 0) return;
    D_00196295 = 1;
    ((char *)text_rsc_buffer)[0] = 0;
    quest_load_text(a1, a2, 0, 0);
    if (((char *)text_rsc_buffer)[0] == 0) return;
    if ((rumor_file = disk_open_rw(D_001704BB)) < 0) return;
    lseek(rumor_file, 0, 2);
    l_10 = (a4 & 2) ? 180 : 30;
    mc_memset(&r, 0, 34, D_00170464, 1808, 4);
    mc_strncpy(r.quest_name, a1->name, 9, D_00170464, 1809);
    r.quest_id = a1->id;
    r.message = a2;
    r.target = a3;
    r.flags = a4;
    r.expires = (unsigned)(game_minutes + l_10 * 1440);
    r.text_length = func_000A0DF4(((char *)text_rsc_buffer)) + 1;
    write(rumor_file, &r, 34);
    write(rumor_file, ((char *)text_rsc_buffer), r.text_length);
    func_0009DEA7(rumor_file);
}
