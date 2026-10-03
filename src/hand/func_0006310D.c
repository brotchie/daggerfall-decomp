/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006310D */
extern char *D_00195AA4;
extern void func_000633BC(char *, int);
extern int func_00069938(int, char *, int);
extern int func_0009DC25(void);
extern int func_000C7FD9(int, int, int, int);

void func_0006310D(char *a1, char *a2)
{
    int l_24[2];
    int l_14;

    if (func_0009DC25() > 195) return;
    l_14 = func_000C7FD9(*(int *)(a1 + 7), *(int *)(a1 + 15), *(int *)(D_00195AA4 + 7), *(int *)(D_00195AA4 + 15));
    if (l_14 >= 1024) return;
    if (*(unsigned char *)(a2 + 506) == 146) {
        func_00069938(11461, a1, 100);
        return;
    }
    if (*(unsigned char *)(a2 + 67) >= 43) return;
    func_000633BC(a1, l_14);
}
