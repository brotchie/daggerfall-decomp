/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001D766 */
#include "records.h"

extern char D_00170464[];
extern char D_001704BB[];
extern signed char text_rsc_buffer[];
extern char *game_minutes;
extern char text_missing_ok;
extern int rumor_file;
extern void quest_load_text(struct quest *, int, short, int);
extern int disk_open_rw(char *);
extern int disk_file_exists(char *);
extern int close(int);
extern int mc_memset();
extern int lseek(int, int, int);
extern int mc_strncpy();
extern int write(int, void *, int);
extern int strlen(char *);

void rumor_add_quest(struct quest *quest, int message, int target, int flags)
{
    int unused;
    int days;
    struct rumor rumor;

    if (disk_file_exists(D_001704BB) == 0) return;
    text_missing_ok = 1;
    ((char *)text_rsc_buffer)[0] = 0;
    quest_load_text(quest, message, 0, 0);
    if (((char *)text_rsc_buffer)[0] == 0) return;
    if ((rumor_file = disk_open_rw(D_001704BB)) < 0) return;
    lseek(rumor_file, 0, 2);
    days = (flags & 2) ? 180 : 30;
    mc_memset(&rumor, 0, 34, D_00170464, 1808, 4);
    mc_strncpy(rumor.quest_name, quest->name, 9, D_00170464, 1809);
    rumor.quest_id = quest->id;
    rumor.message = message;
    rumor.target = target;
    rumor.flags = flags;
    rumor.expires = (unsigned)(game_minutes + days * 1440);
    rumor.text_length = strlen(((char *)text_rsc_buffer)) + 1;
    write(rumor_file, &rumor, 34);
    write(rumor_file, ((char *)text_rsc_buffer), rumor.text_length);
    close(rumor_file);
}
