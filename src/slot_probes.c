/* Functions matched while pinning the -od stack-slot order (docs/progress.md).
 * They move to proper units once the code is split up. */

extern int D_00195AF4, D_00190BE4, D_00190BE8, D_00195B84, D_0019972C;
extern char *D_00195AC4, *D_00195AA4, *D_00195BE0;
extern unsigned char D_00190C78;
extern short D_00199788[];
extern void func_0008E3F7(int, void (*)());
extern void func_00013981();
extern char *func_0008E925(char *, int);
extern int func_000C7FD9(int, int, int, int);
extern void func_000193DD(int);
extern unsigned char func_0002010F(int, int);
extern char *func_0008DCE3(char *, int, int);
extern void func_0005E450(unsigned short, char *);
extern int func_0009DC25(void);
extern void func_0009DC49(int);
extern int func_0008B572(unsigned char, unsigned char);
extern int func_000309E8(char *, short);

int func_00013A00(char *p1, int p2, int p3)
{
    D_00195AF4 = 0;
    D_00190BE4 = p2;
    D_00190BE8 = p3;
    func_0008E3F7(*(int *)(p1 + 0x3f), func_00013981);
    return D_00195AF4;
}

int func_0001811D(char *p)
{
    char *l;
    l = func_0008E925(D_00195AC4, *(int *)(p + 0x14));
    return func_000C7FD9(*(int *)(l + 7), *(int *)(l + 0xf),
                         *(int *)(D_00195AA4 + 7), *(int *)(D_00195AA4 + 0xf));
}

int func_0001939C(int a, int b)
{
    D_00190BE4 = a;
    D_00195B84 = 0;
    func_000193DD(b);
    return D_00195B84;
}

int func_00020057(int a, int b) { return func_0002010F(a, b); }

int func_000309E8(char *p1, short p2)
{
    short l;
    l = ((short *)(p1 + 0x24))[p2];
    return (int)(p1 + l);
}

int func_00030A23(char *p1, short p2, short p3)
{
    int l;
    l = func_000309E8(p1, p2);
    return l + p3 * D_00199788[p2];
}

char *func_00045AED(char *p1, int p2)
{
    char *l1;
    char *l2;
    l1 = func_0008DCE3(p1, 0, 0x6b);
    *l1 = 2;
    l2 = l1 + 0x47;
    func_0005E450((unsigned short)p2, l2);
    return l1;
}

int func_00046C62(void)
{
    int l1;
    int l2;
    l1 = func_0009DC25();
    func_0009DC49(D_0019972C + 0xd81);
    l2 = func_0008B572(D_00195BE0[0x43], D_00190C78 & 1);
    func_0009DC49(l1);
    return l2;
}
