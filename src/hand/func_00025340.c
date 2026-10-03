/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00025340 */
struct rec { char pad0[16]; char name[58]; };      /* 74 bytes */
struct pc { char pad0[16]; unsigned char f16[12]; };
extern char D_00170738[];        /* __FILE__ */
extern char D_00170765[];
extern char D_001903A4[];
extern struct pc *D_00195BEC;
extern struct rec *D_00195C44;
extern int func_000252FD(int);
extern int func_0006CB53(char *, struct rec *);
extern int func_0009DEAC(int);
extern void func_000A0040(void *, int, int, char *, int, int);
extern char *func_000A1079(char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

int func_00025340(void)
{
    int sc[18];
    struct rec *base;
    int j;
    int k;
    int i;
    int s;

    base = D_00195C44;
    func_000A0040(sc, 0, 72, D_00170738, 397, 72);
    for (i = 0; i < 18; i++) {
        func_000A0ED9(401, D_00170738);
        func_000A0F5C(D_001903A4, D_00170765, i);
        func_0006CB53(D_001903A4, &base[i]);
    }
    for (i = 0; i < 12; i++) {
        s = func_000252FD(i);
        for (j = 0; j < 18; j++) {
            k = func_000A1079(base[j].name, D_00195BEC->f16[i], 12) - base[j].name;
            if (k >= 0) {
                if (func_000252FD(k) == s)
                    sc[j] += s;
                else
                    sc[j] += 3 - func_0009DEAC(func_000252FD(k) - s);
            }
        }
    }
    for (j = k = i = 0; i < 18; i++) {
        if (sc[i] > j) {
            j = sc[i];
            k = i;
        }
    }
    return k;
}
