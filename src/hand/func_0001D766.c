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
extern char D_00190FE4[];
extern char *D_00195BF4;
extern char D_00196295;
extern int D_00196704;
extern void func_0004A748(char *, int, int, int);
extern int func_0006CDAB(char *);
extern int func_0006CE7E(char *);
extern int func_0009DEA7(int);
extern int func_000A0040();
extern int func_000A006E(int, int, int);
extern int func_000A0AD9();
extern int func_000A0B42(int, void *, int);
extern int func_000A0DF4(char *);

void func_0001D766(char *a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    struct rec r;

    if (func_0006CE7E(D_001704BB) == 0) return;
    D_00196295 = 1;
    D_00190FE4[0] = 0;
    func_0004A748(a1, a2, 0, 0);
    if (D_00190FE4[0] == 0) return;
    if ((D_00196704 = func_0006CDAB(D_001704BB)) < 0) return;
    func_000A006E(D_00196704, 0, 2);
    l_10 = (a4 & 2) ? 180 : 30;
    func_000A0040(&r, 0, 34, D_00170464, 1808, 4);
    func_000A0AD9(r.name, a1 + 6, 9, D_00170464, 1809);
    r.kind = a1[0];
    r.a2 = a2;
    r.a3 = a3;
    r.flags = a4;
    r.ptr = D_00195BF4 + l_10 * 1440;
    r.len = func_000A0DF4(D_00190FE4) + 1;
    func_000A0B42(D_00196704, &r, 34);
    func_000A0B42(D_00196704, D_00190FE4, r.len);
    func_0009DEA7(D_00196704);
}
