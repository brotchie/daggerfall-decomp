/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002829B */
extern char D_001707AE[];
extern char D_00190CE5;
extern short D_00190D68;
extern short D_00190D6A;
extern char *D_00195C44;
extern int func_000281AF(void);
extern void func_000A0AD9(char *, int, int, char *, int);

void func_0002829B(int a1)
{
    int l_18;

    D_00190CE5 = 1;
    l_18 = func_000281AF();
    *(short *)(D_00195C44 + l_18) = D_00190D68;
    *(short *)(D_00195C44 + l_18 + 2) = D_00190D6A;
    func_000A0AD9(D_00195C44 + (l_18 + 4), a1, 4, D_001707AE, 823);
}
