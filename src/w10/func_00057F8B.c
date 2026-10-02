/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00057F8B */
extern char D_00175710[];
extern char D_00185716[];
extern char D_00185717[];
extern char D_00185718[];
extern char D_00185719[];
extern char D_00185766[];
extern char D_00185825[];
extern char D_0018584D[];
extern char D_00185907[];
extern char D_00185908[];
extern char D_00185909[];
extern char D_0018590A[];
extern char D_0018597F[];
extern char D_001859A4[];
extern char D_001859A6[];
extern char D_00185A94[];
extern char D_00185A95[];
extern char D_00185A96[];
extern char D_00185A97[];
extern char D_00185AC4[];
extern char D_00185AE4[];
extern char D_00190CE4[];
extern char D_00190D64[];
extern char D_0019986A[];
extern char D_0019986B[];
extern char D_0019986E[];
extern char D_0019986F[];
extern char D_00199870[];
extern char D_00199871[];
extern char D_001998CC[];
extern char D_001998CD[];
extern char D_001998D6[];
extern char D_001998D7[];
extern char D_001998E0[];
extern char D_001998E2[];
extern char D_00199910[];
extern void func_0003EC2A(char *, int);
extern void func_00057147(short, short, short, short, short, short, short);
extern int func_0005742F(void);
extern int func_00057485(void);
extern char *func_000A1079(char *, int, unsigned);

void func_00057F8B(int a1)
{
    int idx;
    int count;
    int i;
    int j;
    int x;
    int y;
    int slot;
    char *p;

    p = func_000A1079(D_00185AC4, a1, 12);
    if (p == 0) return;
    idx = p - D_00185AC4;
    for (i = 0; i < 4; i += 2) {
        if (*(unsigned char *)(D_00185A94 + idx * 4 + i) == 128) {
            for (j = 0; j < 5; j++) {
                p = func_000A1079(D_00185825, (*(unsigned char **)(D_00185AE4 + *(unsigned char *)(D_00185A95 + idx * 4 + i) * 4))[j], 39);
                if (p == 0)
                    continue;
                D_001998CC[j * 2] = 0;
                D_001998CD[j * 2] = p - D_00185825;
            }
        } else if (*(unsigned char *)(D_00185A94 + idx * 4 + i) == 129) {
            for (j = 0; j < 5; j++) {
                p = func_000A1079(D_0018584D, (*(unsigned char **)(D_00185AE4 + *(unsigned char *)(D_00185A95 + idx * 4 + i) * 4))[j], 26);
                if (p == 0)
                    continue;
                D_001998D6[j * 2] = 1;
                D_001998D7[j * 2] = p - D_0018584D;
            }
        } else if (*(unsigned char *)(D_00185A94 + idx * 4 + i) == 255)
            continue;
        ((char (*)[10])D_0019986A)[*(short *)D_00190D64][i * 2] = D_00185A94[idx * 4 + i];
        ((char (*)[10])D_0019986B)[*(short *)D_00190D64][i * 2] = D_00185A95[idx * 4 + i];
    }
    count = 0;
    while (*(short *)(D_001859A4 + idx * 20 + count * 4) != -1)
        count++;
    if (count > 5)
        count = 5;
    if (func_00057485() < count) {
        func_0003EC2A(D_00175710, 1);
        return;
    }
    for (i = 0; i < count; i++) {
        slot = func_0005742F();
        D_00199910[slot] = 1;
        *(short *)(D_001998E0 + slot * 4) = *(short *)(D_001859A4 + idx * 20 + i * 4);
        *(short *)(D_001998E2 + slot * 4) = *(short *)(D_001859A6 + idx * 20 + i * 4);
        x = *(short *)(D_001859A4 + idx * 20 + i * 4);
        y = *(short *)(D_001859A6 + idx * 20 + i * 4);
        if (*(short *)(D_001859A4 + idx * 20 + i * 4) < 15) {
            D_00190CE4[slot] = 0;
            j = *(unsigned char *)(D_00185766 + x);
            if (j == 0)
                func_00057147(slot, x, y, -1, -1, -1, -1);
            else {
                j--;
                func_00057147(slot, x, y, *(unsigned char *)(D_00185716 + j * 20 + y * 4), *(unsigned char *)(D_00185717 + j * 20 + y * 4), *(unsigned char *)(D_00185718 + j * 20 + y * 4), *(unsigned char *)(D_00185719 + j * 20 + y * 4));
            }
        } else {
            D_00190CE4[slot] = 1;
            *(short *)(D_001998E0 + slot * 4) -= 15;
            j = *(unsigned char *)(D_0018597F + x);
            if (j == 0)
                func_00057147(slot, x, y, -1, -1, -1, -1);
            else {
                j--;
                func_00057147(slot, x, y, *(unsigned char *)(D_00185907 + j * 20 + y * 4), *(unsigned char *)(D_00185908 + j * 20 + y * 4), *(unsigned char *)(D_00185909 + j * 20 + y * 4), *(unsigned char *)(D_0018590A + j * 20 + y * 4));
            }
        }
        D_0019986E[slot * 10] = D_00185A94[idx * 4];
        D_0019986F[slot * 10] = D_00185A95[idx * 4];
        D_00199870[slot * 10] = D_00185A96[idx * 4];
        D_00199871[slot * 10] = D_00185A97[idx * 4];
    }
}
