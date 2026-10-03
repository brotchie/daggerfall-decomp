/* matched by the real Watcom C32 10.0a (-d2): a run of custom.c from 0x0005506F to 0x000553B2, kept together for its switch table's alignment */
#pragma pack(1)
struct slot { unsigned char kind; unsigned char bit; };
#pragma pack()
extern unsigned char D_0012AC00;
extern char D_00175420[];
extern signed char D_00190D7E[];
extern short D_00190D68;
extern short D_00190D82;
extern short D_00190D84;
extern int D_00190DE4;
extern unsigned short *D_00190DEC;
extern char *D_00195BEC;
extern struct slot D_00199820[][7];
extern void func_000A1023(void *, void *, int, char *, int, int);
extern int func_0012B136();
extern void func_000CE4A9(char *, int, int);
extern void func_000CE4B5(char *, int, int);
extern int func_00144F68();
extern int func_00144FB4();
void func_000551B1(int, int);
void func_000553B2(int, int);

void func_0005506F(void)
{
    func_00144F68(219, 46, 40, 138, D_00190DE4);
    func_00144FB4(219, D_00190D68, D_00190DEC[2], D_00190DEC[3], (char *)D_00190DEC + 12);
}

void func_000550D6(void)
{
    if (D_00190D84 == -1) return;
    if (D_00190D82 == 0)
        func_000551B1(D_00190D84, 1);
    else
        func_000553B2(D_00190D84, 1);
    if (D_00190D84 != 6)
        func_000A1023(&D_00199820[D_00190D82][D_00190D84], &D_00199820[D_00190D82][D_00190D84 + 1], (6 - D_00190D84) * 2, D_00175420, 952, 4);
    D_00190D7E[D_00190D82]--;
    while (D_0012AC00 != 0)
        func_0012B136();
}

void func_000551B1(int a1, int a2)
{
    int bit;

    bit = D_00199820[D_00190D82][a1].bit;
    switch (D_00199820[D_00190D82][a1].kind) {
    case 0:
        func_000CE4A9(D_00195BEC, 1 << bit, a2);
        break;
    case 1:
        func_000CE4A9(D_00195BEC + 1, 1 << bit, a2);
        break;
    case 2:
        func_000CE4B5(D_00195BEC + 4, 1, a2);
        break;
    case 3:
        func_000CE4A9(D_00195BEC + 9, 1 << bit, a2);
        break;
    case 4:
        func_000CE4A9(D_00195BEC + 6, 1 << bit, a2);
        break;
    case 5:
        func_000CE4A9(D_00195BEC + 7, 1 << bit, a2);
        break;
    case 6:
        func_000CE4A9(D_00195BEC + 10, 1 << bit, a2);
        break;
    case 7:
        func_000CE4B5(D_00195BEC + 4, 2, a2);
        break;
    case 8:
        D_00195BEC[5] &= 227;
        if (a2 == 0)
            *(short *)(D_00195BEC + 4) |= bit << 10;
        else
            *(short *)(D_00195BEC + 4) = 5120;
        break;
    case 9:
        func_000CE4B5(D_00195BEC + 4, 4, a2);
        break;
    case 10:
        func_000CE4A9(D_00195BEC + 13, 1 << bit, a2);
        break;
    case 11:
        func_000CE4A9(D_00195BEC + 8, 1 << bit, a2);
        break;
    }
}

void func_000553B2(int a1, int a2)
{
    int bit;

    bit = D_00199820[D_00190D82][a1].bit;
    switch (D_00199820[D_00190D82][a1].kind) {
    case 0:
        func_000CE4B5(D_00195BEC + 4, 8, a2);
        break;
    case 1:
        func_000CE4B5(D_00195BEC + 4, (1 << bit) << 4, a2);
        break;
    case 2:
        func_000CE4B5(D_00195BEC + 10, (1 << bit) << 4, a2);
        break;
    case 3:
        func_000CE4B5(D_00195BEC + 4, (1 << bit) << 6, a2);
        break;
    case 4:
        func_000CE4B5(D_00195BEC + 4, (1 << bit) << 8, a2);
        break;
    case 5:
        func_000CE4B5(D_00195BEC + 14, 1 << bit, a2);
        break;
    case 6:
        func_000CE4A9(D_00195BEC + 2, 1 << bit, a2);
        break;
    case 7:
        func_000CE4A9(D_00195BEC + 3, 1 << bit, a2);
        break;
    case 8:
        func_000CE4B5(D_00195BEC + 14, (1 << bit) << 6, a2);
        break;
    case 9:
        func_000CE4B5(D_00195BEC + 14, (1 << bit) << 9, a2);
        break;
    case 10:
        func_000CE4B5(D_00195BEC + 11, 1 << bit, a2);
        break;
    }
}
