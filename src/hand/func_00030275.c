/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00030275 */
struct item {
    char pad0[2];
    unsigned char global;       /* 0x02 */
    unsigned char state;        /* 0x03 */
};
struct rec {
    char pad0[5];
    struct item *p;             /* +0x05 */
    char pad9[2];
    int id;                     /* +0x0b */
};
struct grp {
    char pad0[2];
    struct rec r[5];
};
extern unsigned char quest_global_states[];
extern int rand(void);

void qaction_op34_pick_one_state(int a1, struct grp *a2)
{
    struct item *arr[4];
    short i;
    short n;
    short pick;

    n = i = 0;
    for (; i < 4; i++) {
        if (a2->r[i + 1].id != -1 && a2->r[i + 1].id != -2)
            arr[n++] = a2->r[i + 1].p;
    }
    if (n == 0)
        return;
    pick = rand() % n;
    for (i = 0; i < n; i++) {
        if (i == pick) {
            if (arr[i]->global != 0)
                quest_global_states[arr[i]->state] = i == pick;
            else
                arr[i]->state = i == pick;
        }
    }
}
