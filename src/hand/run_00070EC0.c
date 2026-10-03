/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x70AFA to 0x70EC0, kept together for its switch table's alignment */
struct rep { short value; char pad[78]; };
extern unsigned char D_00142324;
extern int D_00185077;
extern unsigned char *D_00187545;
extern struct rep D_0018F08E[];
extern int D_00195B8C;
extern unsigned char *D_00195B50;
extern unsigned char *D_00195BE0;
extern unsigned char D_00196268;
extern unsigned char *D_0019671C;
extern unsigned char *D_001A4A14;
extern void func_0003F09F(int, int);
extern int func_0004C274(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void func_000756C6(unsigned char *);
extern int func_0007D6AE(int, int);
extern void func_0007DE82(int, int);
extern void func_0007F1E3(int);
extern int func_0007F2A8(int);
extern int func_0009DEAC(short);

void func_00070AFA(unsigned char *a1)
{
    if (a1[0] == 255) {
        D_0018F08E[a1[6]].value -= a1[1];
        return;
    }
    if (a1[0] & 128) {
        *(short *)(D_00195BE0 + 32 + (a1[0] & 127) * 2) -= a1[1];
        return;
    }
    *(short *)(D_00195BE0 + 157 + a1[0] * 6) -= a1[1];
}

int func_00070B9C(unsigned char *a1, int a2)
{
    int l_18;

    if (a1[0] == 255) {
        D_0018F08E[D_00196268].value += a2;
        if (D_0018F08E[D_00196268].value > 100) {
            l_18 = a2 - (D_0018F08E[D_00196268].value - 100);
            D_0018F08E[D_00196268].value = 100;
        }
    } else if (a1[0] & 128) {
        *(short *)(D_00195BE0 + 32 + (a1[0] & 127) * 2) += a2;
        if (*(short *)(D_00195BE0 + 32 + (a1[0] & 127) * 2) > 100) {
            l_18 = a2 - (*(short *)(D_00195BE0 + 32 + (a1[0] & 127) * 2) - 100);
            *(short *)(D_00195BE0 + 32 + (a1[0] & 127) * 2) = 100;
        }
    } else {
        *(short *)(D_00195BE0 + 157 + a1[0] * 6) += a2;
        if (*(short *)(D_00195BE0 + 157 + a1[0] * 6) > 100) {
            l_18 = a2 - (*(short *)(D_00195BE0 + 157 + a1[0] * 6) - 100);
            *(short *)(D_00195BE0 + 157 + a1[0] * 6) = 100;
        }
    }
    return a2;
}

void func_00070D48(void)
{
    func_0007DE82(1000, D_00185077);
    while (D_00142324 != 0)
        ;
    if (D_00195B8C < 1)
        return;
    if (func_0007F2A8(D_00195B8C) == 0) {
        func_0003F09F(702, 1);
        return;
    }
    func_0007F1E3(D_00195B8C);
    if (func_0007D6AE(1, 100) <= D_00195B8C * 2 / (func_0009DEAC(*(short *)(D_0019671C + 29)) + 1))
        (*(short *)(D_0019671C + 29))++;
    func_0003F09F(703, 1);
}

void func_00070DFD(void)
{
    if (D_00195B50[38] != 0) {
        func_000756C6(D_00195B50);
        return;
    }
    if (D_001A4A14 != 0) {
        func_0004C274(*(D_00187545 - 142 + D_001A4A14[2]), 67, 48, 66, D_001A4A14[0]);
        return;
    }
    func_0004C274(*(D_00187545 - 142 + D_001A4A14[2]), 67, 48, 67, D_00195BE0[129]);
}

int func_00070EC0(unsigned char *a1)
{
    switch (*(unsigned short *)(a1 + 33)) {
    case 108:
        return 0;
    case 42:
        return 3;
    case 40:
        return 1;
    case 41:
        return 2;
    case 21:
    case 82:
        return 142;
    case 22:
    case 84:
        return 143;
    case 24:
    case 28:
        return 144;
    case 26:
    case 92:
        return 145;
    case 27:
    case 94:
        return 146;
    case 29:
    case 98:
        return 147;
    case 33:
    case 106:
        return 148;
    case 35:
    case 36:
        return 149;
    case 368:
        return 68;
    case 408:
        return 69;
    case 409:
        return 70;
    case 410:
        return 71;
    case 411:
        return 72;
    case 413:
        return 73;
    case 414:
        return 74;
    case 415:
        return 75;
    case 416:
        return 76;
    case 417:
        return 77;
    default:
        return -1;
    }
}
