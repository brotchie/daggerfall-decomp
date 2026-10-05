/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006AED1 */
#include "records.h"

struct log {                    /* the logbook record's data (type 24, 3008 bytes) */
    short id[32];
    short val[32][10];
    int time[32][10];
    char text[32][32];
};
extern char D_00175C86[];
extern struct record *logbook_object;
extern struct location *current_location;
extern int game_minutes;
extern int quest_find_by_id(short);
extern void logbook_prune_quests(void);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);

void logbook_add_entry(unsigned char a1, int a2, int a3)
{
    struct log *p;
    int i;
    int slot;
    int fresh;
    int r;

    p = (struct log *)RECORD_DATA(logbook_object);
    slot = -1;
    fresh = 1;
    logbook_prune_quests();
    for (i = 0; i < 32; i++) {
        if (p->id[i] != 0) {
            r = quest_find_by_id(p->id[i]);
            if (r == 0) {
                p->id[i] = 0;
                mc_memset(p->val[i], 0, 20, D_00175C86, 301, 20);
            }
        }
        if (p->id[i] == 0 && slot == -1) {
            slot = i;
            continue;
        }
        if (a1 == p->id[i]) {
            fresh = 0;
            slot = i;
            break;
        }
    }
    if (fresh)
        mc_memset(p->val[slot], 0, 20, D_00175C86, 319, 20);
    p->id[slot] = a1;
    if (a3 > 9)
        a3 %= 10;
    p->val[slot][a3] = a2;
    p->time[slot][a3] = game_minutes;
    mc_strncpy(p->text[slot], current_location->name, 32, D_00175C86, 325);
}
