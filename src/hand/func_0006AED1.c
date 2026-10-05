/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006AED1 */
struct log {
    short id[32];
    short val[32][10];
    int time[32][10];
    char text[32][32];
};
extern char D_00175C86[];
extern char *logbook_object;
extern char *current_location;
extern int game_minutes;
extern int quest_find_by_id(short);
extern void logbook_prune_quests(void);
extern void func_000A0040(void *, int, int, char *, int, int);
extern void func_000A0AD9(char *, char *, int, char *, int);

void logbook_add_entry(unsigned char a1, int a2, int a3)
{
    struct log *p;
    int i;
    int slot;
    int fresh;
    int r;

    p = (struct log *)(logbook_object + 71);
    slot = -1;
    fresh = 1;
    logbook_prune_quests();
    for (i = 0; i < 32; i++) {
        if (p->id[i] != 0) {
            r = quest_find_by_id(p->id[i]);
            if (r == 0) {
                p->id[i] = 0;
                func_000A0040(p->val[i], 0, 20, D_00175C86, 301, 20);
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
        func_000A0040(p->val[slot], 0, 20, D_00175C86, 319, 20);
    p->id[slot] = a1;
    if (a3 > 9)
        a3 %= 10;
    p->val[slot][a3] = a2;
    p->time[slot][a3] = game_minutes;
    func_000A0AD9(p->text[slot], current_location, 32, D_00175C86, 325);
}
