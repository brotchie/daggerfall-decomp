/* matched by the real Watcom C32 10.0a (-d2): a run of runspell from 0x5AE5F to 0x5AFD5, kept together for its switch table's alignment */
struct flags138 { unsigned char b0:1; };
extern char *D_001842D5;
extern short D_00185C14;
extern int D_00195AA0;
extern char *D_00195AA4;
extern char *D_00195B40;
extern char *D_00195B48;
extern char *D_00195BE0;
extern int D_00195D5C;
extern int D_00195D60;
extern char D_0019629A;
extern void func_0004BC43(int, char *, int);
extern void func_0005ABE6(char *, int, int);
extern void func_0005C6A2(char *);
extern void func_00067027(int);
extern int func_0007CBA1(char *);
extern void func_0007D7BA(void);

int func_0005AE5F(char *a1)
{
    unsigned char *l_1C;

    l_1C = (unsigned char *)a1 + 71;
    *(int *)(a1 + 47) = D_00195AA0;
    D_00185C14 = l_1C[73];
    if (l_1C[73] == 92) {
        func_00067027(0);
        return 1;
    }
    if (D_0019629A == 0 && ((struct flags138 *)(D_00195BE0 + 138))->b0 != 0)
        return 1;
    D_00195D60 = 130 - *(short *)(D_00195BE0 + 34) * 50;
    func_0004BC43(73, a1, 0);
    switch (l_1C[7]) {
    case 0:
        func_0005ABE6(a1, D_00195AA0, 0);
        return 1;
    case 1:
        func_0007CBA1(D_001842D5);
        D_00195B48 = a1;
        return 0;
    case 2:
        func_0007CBA1(D_001842D5);
        D_00195B40 = a1;
        return 0;
    case 3:
        func_0007CBA1(D_001842D5);
        D_00195B40 = a1;
        return 0;
    case 4:
        func_0007CBA1(D_001842D5);
        D_00195B40 = a1;
        return 0;
    default:
        return 1;
    }
}

int func_0005AFD5(char *a1, int a2)
{
    unsigned char *l_18;

    l_18 = (unsigned char *)a1 + 71;
    *(int *)(a1 + 47) = D_00195AA0;
    if (l_18[73] == 92) {
        func_00067027(0);
        return 1;
    }
    switch (l_18[7]) {
    case 0:
        func_0005ABE6(a1, D_00195AA0, 0);
        return 1;
    case 1:
        func_0005ABE6(a1, a2, 0);
        return 1;
    case 2:
        func_0005ABE6(a1, a2, 0);
        return 1;
    case 3:
        D_00195D5C = 0;
        *(int *)(a1 + 7) = *(int *)(D_00195AA4 + 7);
        *(int *)(a1 + 11) = *(int *)(D_00195AA4 + 11);
        *(int *)(a1 + 15) = *(int *)(D_00195AA4 + 15);
        func_0005C6A2(a1);
        func_0007D7BA();
        return 1;
    case 4:
        return 1;
    }
    return 1;
}
