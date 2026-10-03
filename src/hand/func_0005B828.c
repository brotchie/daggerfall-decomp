/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005B828 */
struct ent { char pad[137]; int flags; };
struct spell { int id; int mask; };
extern struct spell D_00185C3F[];
extern int D_00195B50;
extern short D_00195F30;
extern char *func_0005B905(char *, int, int *, int *);
extern void func_0008A4E4(int);
extern void func_0008A550(char *, int, char *);

void func_0005B828(char *a1)
{
    char *l_24;
    struct ent *l_20;
    int l_1C;
    int l_18;

    l_20 = (struct ent *)(a1 + 71);
    if ((l_20->flags & 0x3004) == 0) return;
    for (l_18 = 0; l_18 < 3; l_18++) {
        l_24 = func_0005B905(a1, D_00185C3F[l_18].id, &l_1C, &l_1C);
        if (l_24 == 0) {
            l_20->flags &= ~D_00185C3F[l_18].mask;
            continue;
        }
        if (l_24[D_00195F30 * 2 + 1] == 0) {
            func_0008A550(l_24, D_00195F30, a1);
            if (D_00195F30 == 0 && *(unsigned char *)(l_24 + 2) == 255)
                func_0008A4E4(D_00195B50);
        }
    }
}
