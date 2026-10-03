/* matched by the real Watcom C32 10.0a (-d2): a run of click.c from 0x00077960 to 0x00077B9E, kept together for its switch table's alignment */
struct pos { char pad[7]; int x; int y; int z; };
struct town { char pad[34]; unsigned char kind; };
struct loc { char pad[27]; unsigned short region; };
struct bld { char pad[18]; unsigned short type; char pad20[4]; unsigned char kind; };
extern char D_001766F9[];
extern char D_001767E4[];
extern char D_001767EE[];
extern char D_001767FA[];
extern char D_00176807[];
extern char D_00176813[];
extern char D_0017681A[];
extern char D_00176823[];
extern char D_0017682A[];
extern char D_00176833[];
extern char D_0017683A[];
extern char D_001789E8[];
extern char D_001789F0[];
extern unsigned char D_001789FA;
extern char D_00187CA8;
extern struct bld *D_00195A9C;
extern struct pos *D_00195AA4;
extern struct loc *D_00195AC4;
extern struct town *D_00195BDC;
extern unsigned int D_00195BF4;
extern int D_00195C78;
extern unsigned char D_00195E2A[];
extern unsigned short D_00195F5E;
extern char D_00196263;
extern unsigned char D_00196268;
extern char D_00196280;
extern char D_001962A1;
extern char D_001A3F5E;
extern char **D_001A4FA0;
extern char **D_001A4FA4;
extern char **D_001A4FA8;
extern char **D_001A4FAC;
extern char **D_001A4FB0;
extern char **D_001A4FB4;
extern int func_0001FFF1(void);
extern void func_0006974E(char *);
extern char *func_00078463(void);
extern int func_0008661C(int, int);
extern int func_0009DC25(void);
extern void func_0009DC49(int);
extern char *func_000A1079(char *, int, int);
struct mob { char pad[137]; int flags; };
struct ctl { unsigned short f0; };
extern char *D_0018767C[];
extern char *D_001876AC[];
extern char *D_001876E8[];
extern char *D_00187734[];
extern char *D_00187750[];
extern char *D_00187770[];
extern char *D_00187784[];
extern char *D_001877A0[];
extern char *D_001877DC[];
extern char *D_00187828[];
extern char *D_00187844[];
extern char *D_00187864[];
extern char *D_00187878[];
extern int D_001940DB;
extern int D_00195AB0;
extern struct mob *D_00195BE0;
extern struct ctl *D_00195BF8;
extern char *D_00195D70;
extern char D_0019627D;
extern char D_0019628E;
extern int D_001A4FD0;
extern int D_001A4FD8;
extern int func_00076FF2(struct pos *);
extern int func_0007716B(struct pos *, int, int);
extern int func_00077412(struct pos *);
extern int func_000777B8(struct pos *, int, int);

int func_00077960(struct pos *a1, int a2, int a3)
{
    int r;

    switch (D_001789FA) {
    case 1:
        if (D_00195AC4->region == 0xffff)
            r = func_00076FF2(a1);
        else
            r = func_0007716B(a1, a2, a3);
        break;
    case 2:
        r = func_00077412(a1);
        break;
    case 3:
        r = func_000777B8(a1, a2, a3);
        break;
    }
    if (r == 0)
        a1->x = a1->y = a1->z = 0;
    return r;
}

void func_00077A23(void)
{
    if (!(D_00195BF8->f0 & 2) || (D_001940DB & 0x20) || D_0019627D || (D_00195BE0->flags & 8)) {
        D_00195D70 = 0;
        return;
    }
    if (D_0019628E)
        D_001A4FD0 += D_00195AB0;
    D_001A4FD0 = D_001A4FD0 % 1000;
    if (D_001A4FD8 >= D_001A4FD0 && !D_0019628E)
        D_001A4FD0 = 0;
    D_001A4FD8 = D_001A4FD0;
    D_00195D70 = D_0018767C[D_001A4FD0 / 100];
}

void func_00077B03(void)
{
    if (D_001A3F5E) {
        D_001A4FB0 = D_001877A0;
        D_001A4FA8 = D_001877DC;
        D_001A4FAC = D_00187828;
        D_001A4FA4 = D_00187844;
        D_001A4FB4 = D_00187864;
        D_001A4FA0 = D_00187878;
        return;
    }
    D_001A4FB0 = D_001876AC;
    D_001A4FA8 = D_001876E8;
    D_001A4FAC = D_00187734;
    D_001A4FA4 = D_00187750;
    D_001A4FB4 = D_00187770;
    D_001A4FA0 = D_00187784;
}

void func_00077B9E(void)
{
    int seed;
    int climate;
    int idx;
    char *p;

    if (D_00187CA8 == 0)
        return;
    seed = func_0009DC25();
    if (D_001789FA == 3) {
        if (D_001962A1) {
            func_0006974E(D_001A4FA8[10]);
        } else {
            p = func_00078463();
            if (p) {
                func_0006974E(p);
            } else {
                func_0009DC49((D_00196268 << 8) ^ D_00195AC4->region);
                func_0006974E(D_001A4FB0[func_0009DC25() % 15]);
            }
        }
    } else if (D_001789FA == 1) {
        climate = func_0001FFF1();
        func_0009DC49(D_00195BF4 / 1440);
        if (D_00196280 == 0) {
            func_0006974E(D_001A4FA0[func_0009DC25() % 7]);
        } else if (!func_0008661C(D_00195AA4->x, D_00195AA4->z) || (D_00195BDC->kind != 4 && D_00195BDC->kind <= 9 ? 1 : 0)) {
            switch (D_00195E2A[climate]) {
            case 0:
                func_0006974E(D_001A4FA8[func_0009DC25() % 7]);
                break;
            case 1:
                func_0006974E(D_001A4FA8[func_0009DC25() % 9]);
                break;
            case 2:
            case 3:
            case 6:
                func_0006974E(D_001A4FA8[func_0009DC25() % 5 + 7]);
                break;
            case 4:
                func_0006974E(D_001A4FA8[func_0009DC25() % 3 + 12]);
                break;
            case 5:
                func_0006974E(D_001A4FA8[func_0009DC25() % 3 + 15]);
                break;
            }
        } else if (func_0008661C(D_00195AA4->x, D_00195AA4->z) && (D_00195BDC->kind == 4 || D_00195BDC->kind >= 9)) {
            func_0006974E(D_001A4FA0[func_0009DC25() % 7]);
        } else {
            switch (D_00195E2A[climate]) {
            case 0:
                func_0006974E(D_001A4FA8[func_0009DC25() % 7]);
                break;
            case 1:
                func_0006974E(D_001A4FA8[func_0009DC25() % 9]);
                break;
            case 2:
            case 3:
            case 6:
                func_0006974E(D_001A4FA8[func_0009DC25() % 5 + 7]);
                break;
            case 4:
                func_0006974E(D_001A4FA8[func_0009DC25() % 3 + 12]);
                break;
            case 5:
                func_0006974E(D_001A4FA8[func_0009DC25() % 3 + 15]);
                break;
            }
        }
    } else {
        func_0009DC49(D_00195F5E);
        if (D_00195C78) {
            func_0006974E(D_001A4FAC[func_0009DC25() % 7]);
            func_0009DC49(seed);
            return;
        }
        switch (D_00195A9C->kind) {
        case 0:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 12:
        case 13:
            func_0006974E(D_001A3F5E == 0 ? D_001767E4 : D_001766F9);
            break;
        case 11:
            if (D_00195A9C->type == 40) {
                if (func_0009DC25() & 1)
                    func_0006974E(D_001A3F5E == 0 ? D_001767EE : D_001767FA);
                else
                    func_0006974E(D_001A3F5E == 0 ? D_00176807 : D_001767FA);
            } else {
                func_0006974E(D_001A3F5E == 0 ? D_00176813 : D_0017681A);
            }
            break;
        case 14:
            if (D_00196263) {
                func_0006974E(D_001A3F5E == 0 ? D_00176823 : D_0017682A);
            } else {
                p = func_000A1079(D_001789E8, D_00195A9C->type, 8);
                idx = p - D_001789E8;
                if (p == 0) {
                    p = func_000A1079(D_001789F0, D_00195A9C->type, 8);
                    if (p == 0) {
                        func_0006974E(D_001A3F5E == 0 ? D_00176823 : D_0017682A);
                        break;
                    }
                    idx = p - D_001789F0;
                }
                func_0006974E(D_001A4FA4[idx]);
            }
            break;
        case 15:
            func_0006974E(D_001A4FB4[D_00195BF4 / 1440 % 5]);
            break;
        case 16:
            /* a random pick from one choice: the code generator folds `% 1` to 0 and drops the
             * then-branch, but its ?: temp keeps the frame slot at [ebp-0x50] */
            if (func_0009DC25() % 1)
                func_0006974E(D_001A3F5E == 0 ? D_00176833 : D_0017683A);
            else
                func_0006974E(D_001A3F5E == 0 ? D_00176833 : D_0017683A);
            break;
        default:
            func_0006974E(D_001A3F5E == 0 ? D_00176813 : D_0017681A);
            break;
        }
    }
    func_0009DC49(seed);
}
