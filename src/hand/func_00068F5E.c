/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00068F5E */
struct voice {
    int sample;                 /* 0x00 */
    char pad04[8];
    int len;                    /* 0x0c */
    int len2;                   /* 0x10 */
    char pad14[24];
    int volume;                 /* 0x2c */
    int loop;                   /* 0x30 */
    int rate;                   /* 0x34 */
    int bits;                   /* 0x38 */
    int chans;                  /* 0x3c */
    int pan;                    /* 0x40 */
    int x;                      /* 0x44 */
    char pad48[168];
    int handle;                 /* 0xf0 */
    int prio;                   /* 0xf4 */
    char padf8[4];
    char *ptr;                  /* 0xfc */
    char buf[12];               /* 0x100 */
};
extern char D_00175ACC[];        /* __FILE__ */
extern int D_0018DD5C;
extern int D_0018DD60;
extern struct voice D_001A3AE8[3];
extern int D_001A3EFC;
extern int D_001A3F34;
extern unsigned char D_001A3F5D;
extern void func_00068BA8(int, int);
extern void func_00068DA6(char *, char *, int *, int *, char *);
extern void func_000A0040(void *, int, int, char *, int, int);
extern short func_000A2460(int, int);
extern int func_000A2504(int, struct voice *);
extern void func_000A2687(int, int);

int func_00068F5E(int a1, int a2, int a3, int a4)
{
    int i;
    int vol;
    int x;
    int u;
    int loop;

    loop = 0;
    if (D_001A3F5D == 0)
        return -1;
    if (D_0018DD5C == -1)
        return -1;
    if (a4 == -1) {
        a4 = 127;
        loop = 1;
        i = 3;
        if (D_001A3EFC != 0x12345678)
            return -1;
    } else {
        for (i = 0; i < 3; i++) {
            if (D_001A3AE8[i].handle == 0x12345678)
                break;
            if (func_000A2460(D_0018DD60, D_001A3AE8[i].handle) != 0) {
                D_001A3AE8[i].handle = 0x12345678;
                break;
            }
        }
        if (a4 == -2) {
            a4 = 127;
            loop = 1;
        }
        if (i == 3) {
            for (i = 0; i < 3; i++) {
                if (D_001A3AE8[i].prio < a4) {
                    func_000A2687(D_0018DD60, D_001A3AE8[i].handle);
                    break;
                }
            }
        }
        if (loop == 0 && i == 3)
            return -1;
    }
    D_001A3F34 = i;
    func_00068BA8(a3, i);
    if (D_001A3AE8[i].ptr != 0)
        func_00068DA6(D_001A3AE8[i].buf, D_001A3AE8[i].ptr + 7, &vol, &x, D_001A3AE8[i].ptr);
    else
        func_00068DA6(D_001A3AE8[i].buf, D_001A3AE8[i].buf, &vol, &x, D_001A3AE8[i].ptr);
    func_000A0040(&D_001A3AE8[i], 0, 240, D_00175ACC, 246, 4);
    D_001A3AE8[i].prio = a4;
    D_001A3AE8[i].sample = a1;
    D_001A3AE8[i].len = a2;
    D_001A3AE8[i].volume = (short)vol | ((short)vol << 16);
    D_001A3AE8[i].rate = 11025;
    D_001A3AE8[i].pan = 32768;
    D_001A3AE8[i].x = x;
    D_001A3AE8[i].loop = loop != 0 ? -1 : 0;
    D_001A3AE8[i].len2 = a2;
    D_001A3AE8[i].bits = 8;
    D_001A3AE8[i].chans = 1;
    D_001A3AE8[i].handle = func_000A2504(D_0018DD60, &D_001A3AE8[i]);
    return i;
}
