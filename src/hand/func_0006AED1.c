/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006AED1 */
struct log {
    short id[32];
    short val[32][10];
    int time[32][10];
    char text[32][32];
};
extern char D_00175C86[];
extern char *D_001959B0;
extern char *D_00195BDC;
extern int D_00195BF4;
extern int func_0004BB64(short);
extern void func_0006B2E2(void);
extern void func_000A0040(void *, int, int, char *, int, int);
extern void func_000A0AD9(char *, char *, int, char *, int);

void func_0006AED1(unsigned char a1, int a2, int a3)
{
    struct log *p;
    int i;
    int slot;
    int fresh;
    int r;

    p = (struct log *)(D_001959B0 + 71);
    slot = -1;
    fresh = 1;
    func_0006B2E2();
    for (i = 0; i < 32; i++) {
        if (p->id[i] != 0) {
            r = func_0004BB64(p->id[i]);
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
    p->time[slot][a3] = D_00195BF4;
    func_000A0AD9(p->text[slot], D_00195BDC, 32, D_00175C86, 325);
}
