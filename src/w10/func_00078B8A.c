/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00078B8A */
extern char D_00176844[];
extern char D_0017685B[];
extern char D_001903A4[];
extern char D_00190704[];
extern char D_00195AC8[];
extern int func_00012FCE(int, int, int);
extern int func_00013260(int, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, int);
extern int func_000A0F5C(int, ...);

struct span {
    int f0;
    char *start;
    char *cur;
};

void func_00078B8A(unsigned char *a1)
{
    unsigned char *l_28;
    struct span *l_24;
    unsigned char *l_20;
    int l_1C;
    int l_18;

    if (*a1 != 18) return;
    l_28 = a1 + 71;
    l_20 = l_28 + 560;
    l_24 = (struct span *)(l_20 + 74);
    l_1C = l_24->cur - l_24->start;
    func_000A0ED9(196, (int)D_00176844);
    func_000A0F5C((int)D_001903A4, (int)D_0017685B, l_28[503]);
    l_18 = func_00012FCE(*(int *)D_00195AC8, (int)D_001903A4, 8);
    ((char **)D_00190704)[l_28[75]] = l_24->start = (char *)func_00013260(*(int *)D_00195AC8, l_18, 0);
    if (l_24->cur == 0) return;
    l_24->cur = l_24->start + l_1C;
}
