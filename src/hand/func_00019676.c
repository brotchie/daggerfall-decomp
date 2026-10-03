/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00019676 */
struct fac {
    unsigned char type;
    unsigned char id;
    char pad2[29];
    short power;
    short f33;
    char pad35[2];
    unsigned short flags;
    int f39;
    unsigned int f43;
    char pad47[9];
    struct fac *allies[3];
    struct fac *enemies[3];
    char pad80[8];
    struct fac *f88;
};
struct gstate {
    char f0;
    char f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12;
    char pad13[6];
    char f19, f20, f21;
    char pad22[8];
    char f30[18];
    short f48;
    char pad50[30];
};
extern unsigned char D_00178E59;
extern unsigned char D_00178E5F;
extern int D_0017C912[];
extern struct gstate D_0018F060[];
extern int D_00195B84;
extern char D_00196268;
extern char D_00196269;
extern char D_001962A5;
extern int D_00196710;
extern struct fac *D_0019672C;
extern void func_00013E17(int, int);
extern void func_00013F06(int, int);
extern struct fac *func_000191DA(short, short);
extern struct fac *func_000192EE(short);
extern struct fac *func_00019485(void);
extern int func_0001AC53(struct fac *, struct fac *);
extern int func_0001ACCA(struct fac *);
extern int func_0001AD21(struct fac *, struct fac *);
extern int func_0001AD97(struct fac *, struct fac *);
extern void func_0001AE0D(struct fac *, int);
extern int func_0001AEBE(struct fac *);
extern int func_0001AEF9(struct fac *, struct fac *);
extern int func_0001B06F(struct fac *, struct fac *);
extern int func_0001B144(struct fac *, struct fac *);
extern int func_0001B1F7(struct fac *);
extern void func_0001B22E(struct fac *);
extern int func_0001CBBD(struct fac *);
extern void func_0001CCC6(struct fac *, int, struct fac *);
extern void func_0001CD56(struct fac *, int, struct fac *);
extern void func_0001CDE6(struct fac *, int);
extern void func_0001CE92(struct fac *, int);
extern void func_0001CF3E(struct fac *, struct fac *, int, unsigned char, int);
extern void func_0001D113(void);
extern void func_0001D176(void);
extern void func_0001D8AD(void);
extern int func_0007D6AE(int, int);
extern int func_0009DC25(void);

void func_00019676(int a1)
{
    int i;
    int a;
    int b;
    int c;
    int rnd;
    int d;
    int l_34;
    int j;
    int r;
    int bonus;
    int p1;
    int p2;
    struct fac *f;
    struct fac *g;

    f = D_0019672C;
    if (a1 == 1)
        return;
    if (D_001962A5)
        func_0001D8AD();
    func_0001D113();
    for (i = 0; i < D_00196710; i++, f++) {
        if (!(f->type == 7 || f->type == 2 || f->type == 3))
            continue;
        a = (func_0001B1F7(f->allies[0]) + func_0001B1F7(f->allies[1]) + func_0001B1F7(f->allies[2])) / 10;
        b = (func_0001B1F7(f->enemies[0]) + func_0001B1F7(f->enemies[1]) + func_0001B1F7(f->enemies[2])) / 10;
        if (f->f88)
            c = f->f88->power / 10;
        else
            c = 0;
        rnd = func_0007D6AE(0, 100);
        if (f->f43 + a - b + c > rnd)
            func_0001AE0D(f, 1);
        else
            func_0001AE0D(f, -1);
        if (f->power < func_0001AEBE(f))
            func_0001AE0D(f, 1);
        if (a1 == 2) {
            d = f->power / 5;
            if (func_0001ACCA(f) && D_0018F060[f->id].f1) {
                D_0018F060[f->id].f1 = 0;
                D_0018F060[f->id].f2 = 1;
            }
            for (j = 0; j < 3; j++) {
                rnd = func_0007D6AE(0, 100);
                if (f->allies[j] && (func_0001AEF9(f, f->allies[j]) + (f->f43 + d)) / 5 + 70 < rnd)
                    func_0001CDE6(f, j);
            }
            for (j = 0; j < 3; j++) {
                if (func_0001B144(f, f->enemies[j]))
                    continue;
                rnd = func_0007D6AE(0, 100);
                if (f->enemies[j] && (func_0001AEF9(f, f->enemies[j]) + (f->f43 + d)) / 5 < rnd)
                    func_0001CE92(f, j);
            }
            for (j = 0; j < 3; j++) {
                if (f->allies[j])
                    continue;
                do {
                    g = func_00019485();
                } while (!(g->type == 2 || g->type == 3 || g->type == 7));
                if (func_0001AD97(f, g) || func_0001AD21(f, g))
                    continue;
                if (func_0001AD21(f->allies[0], g) || func_0001AD21(f->allies[1], g) || func_0001AD21(f->allies[2], g))
                    continue;
                if (func_0001AD97(f->enemies[0], g) || func_0001AD97(f->enemies[1], g) || func_0001AD97(f->enemies[2], g))
                    continue;
                if (func_0001B06F(f, g))
                    continue;
                rnd = func_0007D6AE(0, 100);
                if ((func_0001AEF9(f, g) + (f->f43 + d)) / 5 <= rnd)
                    break;
                if (f->type == 7 && f->id != 255)
                    func_0001CF3E(f, g, 26, f->id, 1481);
                if (g->type == 7 && g->id != 255)
                    func_0001CF3E(g, f, 26, g->id, 1481);
                func_0001CCC6(f, j, g);
                break;
            }
            D_00195B84 = 0;
            if (func_0001AC53(f, f->enemies[0]) || func_0001AC53(f, f->enemies[1]) || func_0001AC53(f, f->enemies[2])) {
                D_00195B84--;
                if (D_0018F060[f->id].f3 || D_0018F060[f->id].f4) {
                    func_0001B22E(f);
                    func_0001B22E(f->enemies[D_00195B84]);
                    j = D_00195B84;
                    D_00195B84 = 0;
                    if (func_0001AC53(f->enemies[0], f) || func_0001AC53(f->enemies[1], f) || func_0001AC53(f->enemies[2], f))
                        f->enemies[j]->enemies[D_00195B84 - 1] = 0;
                    f->enemies[j] = 0;
                } else if (D_0018F060[f->id].f1) {
                    g = f->enemies[D_00195B84];
                    func_0001CF3E(f, g, 0, f->id, 1479);
                    func_0001CF3E(g, f, 0, g->id, 1479);
                    func_00013E17(f->id, 1);
                    func_00013E17(g->id, 1);
                } else if (D_0018F060[f->id].f2) {
                    if (func_0007D6AE(1, 100) <= 5) {
                        func_00013F06(f->id, 1);
                        func_00013F06(f->enemies[D_00195B84]->id, 1);
                    } else {
                        a = (func_0001B1F7(f->allies[0]) + func_0001B1F7(f->allies[1]) + func_0001B1F7(f->allies[2])) / 5;
                        p1 = a + f->power;
                        a = (func_0001B1F7(f->enemies[D_00195B84]->allies[0])
                             + func_0001B1F7(f->enemies[D_00195B84]->allies[1])
                             + func_0001B1F7(f->enemies[D_00195B84]->allies[2])) / 5;
                        p2 = a + f->enemies[D_00195B84]->power;
                        f->power -= func_0007D6AE(1, p1 / 10);
                        f->enemies[D_00195B84]->power -= func_0007D6AE(1, p2 / 10);
                        if (p1 < p2) {
                            if (p2 - p1 > p1) {
                                func_0001CF3E(f->enemies[D_00195B84], f, 100, 0, 1408);
                                func_0001AE0D(f->enemies[D_00195B84], f->power / 2);
                                func_00013E17(f->enemies[D_00195B84]->id, 2);
                                func_00013E17(f->id, 3);
                            } else {
                                func_0001CF3E(f, f->enemies[D_00195B84], 100, 0, 1407);
                            }
                        } else if (p1 - p2 > p2) {
                            func_0001CF3E(f, f->enemies[D_00195B84], 100, 0, 1408);
                            func_0001AE0D(f, f->enemies[D_00195B84]->power / 2);
                            func_00013E17(f->enemies[D_00195B84]->id, 3);
                            func_00013E17(f->id, 2);
                        } else {
                            func_0001CF3E(f, f->enemies[D_00195B84], 100, 0, 1407);
                        }
                    }
                }
            }
            for (j = 0; j < 3; j++) {
                if (f->enemies[j])
                    continue;
                do {
                    g = func_00019485();
                } while (!(g->type == 2 || g->type == 3 || g->type == 7));
                if (func_0001AD97(f, g) || func_0001AD21(f, g))
                    continue;
                if (func_0001AD21(f->enemies[0], g) || func_0001AD21(f->enemies[1], g) || func_0001AD21(f->enemies[2], g))
                    continue;
                if (func_0001AD97(f->allies[0], g) || func_0001AD97(f->allies[1], g) || func_0001AD97(f->allies[2], g))
                    continue;
                r = func_0001B06F(f, g);
                if (r == 1 || r == 3)
                    continue;
                if (r == 2)
                    bonus = 10;
                else
                    bonus = 0;
                rnd = func_0007D6AE(0, 100);
                if ((func_0001AEF9(f, g) + (f->f43 + d)) / 5 + bonus + 70 >= rnd)
                    break;
                if (f->type == 7 && f->id != 255)
                    func_0001CF3E(f, g, 27, f->id, 1482);
                if (g->type == 7 && g->id != 255)
                    func_0001CF3E(g, f, 27, g->id, 1482);
                func_0001CD56(f, j, g);
                if (func_0001ACCA(f) && func_0001ACCA(g) && func_0001B144(f, g)) {
                    func_0001CF3E(f, g, 100, 0, 1407);
                    if (f->type == 7 && f->id != 255)
                        func_0001CF3E(f, g, 28, f->id, 1479);
                    if (g->type == 7 && g->id != 255)
                        func_0001CF3E(g, f, 28, g->id, 1479);
                    func_00013E17(f->id, 0);
                    func_00013E17(g->id, 0);
                }
                break;
            }
            if (!(f->flags & 16) && (unsigned)func_0007D6AE(0, 100) > f->f43 / 3 + 70) {
                if (f->type == 7 && f->id != 255)
                    func_0001CF3E(f, 0, 12, f->id, 1480);
                f->f43 = func_0007D6AE(0, 50) + 20;
                f->f39 <<= 16;
                f->f39 = (f->f39 & 0xffff0000) | func_0009DC25();
                if (func_0001CBBD(f))
                    func_0001CF3E(f, 0, 100, 0, 1406);
            }
            if (f->id == 255 || f->type != 7)
                continue;
            a = (func_0001B1F7(f->allies[0]) + func_0001B1F7(f->allies[1]) + func_0001B1F7(f->allies[2])) / 10;
            if (D_0018F060[f->id].f10) {
                func_00013F06(f->id, 9);
            } else if (D_0018F060[f->id].f9) {
                if ((unsigned)func_0007D6AE(0, 100) < a + f->f43 / 5 + f->power / 5) {
                    func_0001CF3E(f, 0, 7, f->id, 1477);
                    func_00013E17(f->id, 9);
                }
            } else if (D_0018F060[f->id].f8) {
                func_0001CF3E(f, 0, 7, f->id, 1477);
                func_00013E17(f->id, 8);
            } else if (func_0007D6AE(1, 100) <= 2) {
                if ((unsigned)func_0007D6AE(0, 100) > f->f43 + a) {
                    func_0001CF3E(f, 0, 7, f->id, 1477);
                    func_00013E17(f->id, 7);
                }
            }
            if (D_0017C912[f->id])
                g = func_000192EE(D_0017C912[f->id]);
            else
                g = 0;
            if (D_0018F060[f->id].f7) {
                func_00013F06(f->id, 6);
            } else if (D_0018F060[f->id].f6) {
                if (g)
                    g->power--;
                f->power--;
                if ((unsigned)func_0007D6AE(0, 100) < a + f->f43 / 5 + f->power / 5) {
                    func_0001CF3E(f, 0, 4, f->id, 1478);
                    func_00013E17(f->id, 6);
                }
            } else if (D_0018F060[f->id].f5) {
                if (g)
                    g->power--;
                f->power--;
                func_0001CF3E(f, 0, 4, f->id, 1478);
                func_00013E17(f->id, 5);
            } else if (func_0007D6AE(1, 100) <= 2) {
                if ((unsigned)func_0007D6AE(0, 100) > f->f43 + a) {
                    if (g)
                        g->power--;
                    f->power--;
                    func_0001CF3E(f, 0, 4, f->id, 1478);
                    func_00013E17(f->id, 4);
                }
            }
            if (D_0017C912[f->id]) {
                g = func_000192EE(D_0017C912[f->id]);
                if (D_0018F060[f->id].f19)
                    g->power--;
                if (func_0007D6AE(0, 100) < (g->power - f->power + 5) / 5) {
                    if (g->power < f->power * 2) {
                        D_0018F060[f->id].f48 = g->f33;
                        func_0001CF3E(f, 0, 18, f->id, 1476);
                        func_00013E17(f->id, 18);
                        g->power--;
                    } else {
                        func_00013F06(f->id, 18);
                    }
                } else {
                    func_00013F06(f->id, 18);
                }
            } else {
                func_00013F06(f->id, 18);
            }
            if (D_0018F060[f->id].f12)
                f->power--;
            j = (func_000192EE(42)->power + func_000192EE(108)->power) / 2;
            if (func_0007D6AE(0, 100) < (j - f->power + 5) / 5) {
                func_0001CF3E(0, 0, 11, f->id, 1410);
                func_00013E17(f->id, 11);
                f->power--;
            } else {
                func_00013F06(f->id, 11);
            }
            if (D_0018F060[f->id].f11)
                func_0001AE0D(func_000191DA(f->id, 8), -1);
            g = func_000191DA(f->id, 8);
            if (g) {
                if (func_0007D6AE(0, 100) < (g->power - f->power + 5) / 5) {
                    func_0001CF3E(f, 0, 10, f->id, 1475);
                    func_00013E17(f->id, 10);
                    g->power--;
                } else {
                    func_00013F06(f->id, 10);
                }
            } else {
                func_00013F06(f->id, 10);
            }
        }
    }
    if (a1 == 2) {
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f12)
                j++;
        func_0001AE0D(func_000192EE(42), j - 1);
        func_0001AE0D(func_000192EE(108), j - 1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f20)
                j++;
        if (j >= 3)
            func_0001AE0D(func_000192EE(510), 1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f21)
                j++;
        if (j >= 3)
            func_0001AE0D(func_000192EE(510), -1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f30[D_00178E5F])
                j++;
        if (j >= 3)
            func_0001AE0D(func_000192EE(510), -1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f30[D_00178E59])
                j++;
        if (j >= 3)
            func_0001AE0D(func_000192EE(510), 1);
    }
    func_0001CF3E(0, 0, 100, 0, 1450);
    func_0001CF3E(0, 0, 100, 0, 1451);
    func_0001CF3E(0, 0, 100, 0, 1452);
    func_0001CF3E(0, 0, 100, 0, 1453);
    func_0001CF3E(0, 0, 100, 0, 1454);
    func_0001CF3E(0, 0, 100, 0, 1455);
    func_0001CF3E(0, 0, 100, 0, 1456);
    func_0001D176();
    D_00196269 = D_00196268;
}
