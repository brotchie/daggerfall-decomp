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
extern unsigned char D_00185A94[];
extern unsigned char D_00185A95[];
extern char D_00185A96[];
extern char D_00185A97[];
extern char D_00185AC4[];
extern char D_00185AE4[];
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern signed char D_0019986A[];
extern signed char D_0019986B[];
extern signed char D_0019986E[];
extern signed char D_0019986F[];
extern signed char D_00199870[];
extern signed char D_00199871[];
extern char D_001998CC[];
extern char D_001998CD[];
extern char D_001998D6[];
extern char D_001998D7[];
extern char itemmaker_slots[];
extern char D_001998E2[];
extern signed char D_00199910[];
extern void msgbox_show_string(char *, int);
extern void func_00057147(short, short, short, short, short, short, short);
extern int itemmaker_free_slot(void);
extern int itemmaker_free_slot_count(void);
extern char *memchr(char *, int, unsigned);

void itemmaker_add_soul_powers(int a1)
{
    int idx;
    int count;
    int i;
    int j;
    int x;
    int y;
    int slot;
    char *p;

    p = memchr(D_00185AC4, a1, 12);
    if (p == 0) return;
    idx = p - D_00185AC4;
    for (i = 0; i < 4; i += 2) {
        if (D_00185A94[idx * 4 + i] == 128) {
            for (j = 0; j < 5; j++) {
                p = memchr(D_00185825, (*(unsigned char **)(D_00185AE4 + D_00185A95[idx * 4 + i] * 4))[j], 39);
                if (p == 0)
                    continue;
                D_001998CC[j * 2] = 0;
                D_001998CD[j * 2] = p - D_00185825;
            }
        } else if (D_00185A94[idx * 4 + i] == 129) {
            for (j = 0; j < 5; j++) {
                p = memchr(D_0018584D, (*(unsigned char **)(D_00185AE4 + D_00185A95[idx * 4 + i] * 4))[j], 26);
                if (p == 0)
                    continue;
                D_001998D6[j * 2] = 1;
                D_001998D7[j * 2] = p - D_0018584D;
            }
        } else if (D_00185A94[idx * 4 + i] == 255)
            continue;
        ((char (*)[10])((char *)D_0019986A))[*(short *)scratch_190d64][i * 2] = ((char *)D_00185A94)[idx * 4 + i];
        ((char (*)[10])((char *)D_0019986B))[*(short *)scratch_190d64][i * 2] = ((char *)D_00185A95)[idx * 4 + i];
    }
    count = 0;
    while (*(short *)(D_001859A4 + idx * 20 + count * 4) != -1)
        count++;
    if (count > 5)
        count = 5;
    if (itemmaker_free_slot_count() < count) {
        msgbox_show_string(D_00175710, 1);
        return;
    }
    for (i = 0; i < count; i++) {
        slot = itemmaker_free_slot();
        ((char *)D_00199910)[slot] = 1;
        *(short *)(itemmaker_slots + slot * 4) = *(short *)(D_001859A4 + idx * 20 + i * 4);
        *(short *)(D_001998E2 + slot * 4) = *(short *)(D_001859A6 + idx * 20 + i * 4);
        x = *(short *)(D_001859A4 + idx * 20 + i * 4);
        y = *(short *)(D_001859A6 + idx * 20 + i * 4);
        if (*(short *)(D_001859A4 + idx * 20 + i * 4) < 15) {
            ((char *)scratch_190ce4)[slot] = 0;
            j = *(unsigned char *)(D_00185766 + x);
            if (j == 0)
                func_00057147(slot, x, y, -1, -1, -1, -1);
            else {
                j--;
                func_00057147(slot, x, y, *(unsigned char *)(D_00185716 + j * 20 + y * 4), *(unsigned char *)(D_00185717 + j * 20 + y * 4), *(unsigned char *)(D_00185718 + j * 20 + y * 4), *(unsigned char *)(D_00185719 + j * 20 + y * 4));
            }
        } else {
            ((char *)scratch_190ce4)[slot] = 1;
            *(short *)(itemmaker_slots + slot * 4) -= 15;
            j = *(unsigned char *)(D_0018597F + x);
            if (j == 0)
                func_00057147(slot, x, y, -1, -1, -1, -1);
            else {
                j--;
                func_00057147(slot, x, y, *(unsigned char *)(D_00185907 + j * 20 + y * 4), *(unsigned char *)(D_00185908 + j * 20 + y * 4), *(unsigned char *)(D_00185909 + j * 20 + y * 4), *(unsigned char *)(D_0018590A + j * 20 + y * 4));
            }
        }
        ((char *)D_0019986E)[slot * 10] = ((char *)D_00185A94)[idx * 4];
        ((char *)D_0019986F)[slot * 10] = ((char *)D_00185A95)[idx * 4];
        ((char *)D_00199870)[slot * 10] = D_00185A96[idx * 4];
        ((char *)D_00199871)[slot * 10] = D_00185A97[idx * 4];
    }
}
