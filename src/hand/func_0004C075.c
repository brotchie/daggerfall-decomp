/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004C075 */
extern char D_0012B508[];
extern char D_00174F47[];
extern char D_001850D4[];
extern char D_001850E5[];
extern char D_00195D48[];
extern char D_00196271[];
extern char D_00196274[];
extern char D_0019629B[];
extern char D_00199764[];
extern void func_0003EDF4(int, short, int);
extern void func_0003F358(void);
extern void func_0004FF2E(void);
extern int func_00051DCF(int, int, int, int);
extern void func_0007CD98(void);
extern void func_0007DE23(short);
extern void func_00081425(void);
extern int func_000A0040();
extern int func_000A0E74();
extern int func_000A17C1();
extern int func_000CDD81();

int func_0004C075(unsigned char *a1)
{
    char l_50[44];
    int l_20;
    int l_1C;

    l_1C = 0;
    *(signed char *)D_0012B508 = 146;
    l_20 = func_000A17C1((int)D_001850D4, func_000A0E74(a1[6]));
    if (l_20 != 0) {
        l_1C = func_00051DCF(*(int *)(D_001850E5 + ((l_20 - ((int)D_001850D4)) << 2)), (int)l_50, 1000, 1) == 1;
        func_00051DCF(*(int *)(D_001850E5 + ((l_20 - ((int)D_001850D4)) << 2)), (int)l_50, l_1C ? 1002 : 1001, 0);
        func_000A0040(655360, 0, 64000, (int)D_00174F47, 277, 4);
        func_0004FF2E();
        *(int *)D_00195D48 = 10000;
        *(signed char *)D_0019629B = 0;
        return l_1C;
    }
    switch (func_000A0E74(a1[11])) {
    case 'Y':
        func_0007DE23(1000);
        while (*(unsigned char *)D_00196274 == 8) {
            func_0007CD98();
            func_0003F358();
            func_00081425();
            func_000CDD81(1);
        }
        l_1C = *(unsigned char *)D_00196271 == 1;
        *(signed char *)D_0012B508 = 146;
        func_0003EDF4(*(int *)D_00199764, l_1C ? 1002 : 1001, 1);
        break;
    default:
        l_1C = 1;
    }
    return l_1C;
}
