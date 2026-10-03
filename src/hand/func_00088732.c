/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088732 */
#pragma pack(1)
struct snd {
    char pad0[4];
    unsigned _lo:25;
    unsigned kind:5;
    unsigned _hi:2;
    char pad8[4];
    unsigned char f12;
};
#pragma pack()
extern int D_000C2893[];
extern char *D_000C28BC;
extern char *D_000C28C0;
extern int D_00187F30[];
extern char D_00190FE4[];
extern char *D_00195AA4;
extern char D_00196289;
extern struct snd *D_00196A80;
extern int D_001A94A0[];
extern int D_001A94B0[];
extern int D_001A94C0;
extern void func_0004A6B5(int, int, int);
extern int func_0007CBA1(char *);
extern struct snd *func_0008649F(int);
extern void func_00088281(char *, char *);
extern int func_000C2D81();
extern int func_000C3A60();
extern int func_000C3FCB();

void func_00088732(void)
{
    int i;
    int cur;
    struct snd *saved;

    cur = func_000C2D81(*(int *)(D_00195AA4 + 7), *(int *)(D_00195AA4 + 15));
    saved = D_00196A80;
    D_001A94C0 = 4;
    for (i = 0; i < 4; i++) {
        if (D_001A94A0[i] != D_000C2893[i]) {
            D_001A94A0[i] = D_000C2893[i];
            D_001A94B0[i] = 1;
            D_001A94C0 = i;
            if ((D_00196A80 = func_0008649F(D_001A94A0[i])) != 0) {
                if (D_00196289 == 0 && cur == D_001A94A0[i]) {
                    switch (D_00196A80->kind) {
                    case 4:
                    case 7:
                    case 10:
                    case 12:
                        func_0004A6B5(D_00196A80->f12 + 500, 0, 0);
                        func_0007CBA1(D_00190FE4);
                    }
                }
                func_00088281(D_000C28BC + D_00187F30[D_001A94C0], D_000C28C0 + D_00187F30[D_001A94C0]);
                func_000C3FCB();
            }
            func_000C3A60(i);
        }
    }
    D_00196A80 = saved;
}
