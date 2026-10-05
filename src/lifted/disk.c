/* disk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char disk_last_file_size[];
extern int D_00147954;
extern char D_00175D00[];
extern char D_00175D07[];
extern char D_00175D22[];
extern char D_00175D44[];
extern char D_00175D60[];
extern char D_00175D65[];
extern char D_00175D6B[];
extern char D_00175D6F[];
extern char D_00175D73[];
extern char D_00175D7A[];
extern char D_00175D82[];
extern char D_001917E4[];
extern char D_00191834[];
extern char disk_path[];
extern int D_001A49F4;
extern int D_001A49F8;
extern int D_001A49FC;
extern int D_001A4A00;
extern int D_001A4A04;
extern int D_001A4A08;

extern int open(int, ...);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int func_000A00CB();
extern int mc_strncpy();
extern int write();
extern int func_000A0DF4();
extern int stricmp();
extern int strnicmp();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int unlink();
extern int mc_memcpy();
extern int filelength();
extern int strchr();
extern void fatal_error(int);
extern void file_index_scan(int);
int disk_write_file(int, int, int);
int disk_open_data(int);
int disk_resolve_path(int);
void func_0006D430(int);
#pragma aux func_000A0ED9 parm routine [];

int disk_read_file(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = a2;
    l_20 = disk_open_data(a1);
    if (l_20 < 0) {
        func_000A0ED9(45, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D07, a1);
        fatal_error((int)disk_path);
    }
    l_1C = (*(int *)disk_last_file_size = filelength(l_20));
    if (l_24 == 0) l_24 = mc_malloc(l_1C, (int)D_00175D00, 52);
    if (l_24 == 0) {
        func_000A0ED9(56, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D22, a1);
        fatal_error((int)disk_path);
    }
    mc_memset(l_24, 0, l_1C, (int)D_00175D00, 60, 4);
    l_18 = func_000A00CB(l_20, l_24, l_1C);
    if (l_18 != l_1C) {
        func_000A0ED9(65, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D44, a1);
        fatal_error((int)disk_path);
    }
    func_0009DEA7(l_20);
    return l_24;
}

int disk_write_file(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    unlink(a1);
    l_18 = open(a1, 546, 384);
    if (l_18 < 1) return 0;
    l_14 = ((write(l_18, a2, a3) == a3) ? 1 : 0);
    func_0009DEA7(l_18);
    return l_14;
}

int disk_write_arena2_file(int a1, int a2, int a3)
{
    func_000A0ED9(107, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)D_001917E4, a1);
    return disk_write_file((int)disk_path, a2, a3);
}

int disk_open_data(int a1)
{
    a1 = disk_resolve_path(a1);
    return open(a1, 512);
}

int disk_open_rw(int a1)
{
    func_000A0ED9(135, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)D_001917E4, a1);
    return open((int)disk_path, 514);
}

int disk_create(int a1)
{
    func_000A0ED9(149, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)D_001917E4, a1);
    unlink((int)disk_path);
    return open((int)disk_path, 546, 384);
}

int disk_file_exists(int a1)
{
    short l_18;
    char l_7C[92];

    func_000A0ED9(159, (int)D_00175D00);
    mc_sprintf((int)l_7C, (int)D_00175D60, (int)D_001917E4, a1);
    *(int *)&l_18 = open((int)l_7C, 512);
    if (l_18 < 0) return 0;
    func_0009DEA7((int)(short)l_18);
    return 1;
}

void disk_copy_file(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_1C = mc_malloc(4096, (int)D_00175D00, 172);
    if (((int)(unsigned char)*(signed char *)((char *)(func_000A0DF4(a2) + a2) - 1)) == 92) {
        func_000A0ED9(176, (int)D_00175D00);
        mc_sprintf(l_1C, (int)D_00175D60, a2, a1);
    } else {
        func_000A0ED9(178, (int)D_00175D00);
        mc_sprintf(l_1C, (int)D_00175D65, a2, a1);
    }
    l_18 = open(l_1C, 512);
    if (l_18 == (-1)) {
        if (l_1C != 0 && l_1C != (-1751672937)) {
            mc_free(l_1C, (int)D_00175D00, 184);
            l_1C = -1751672937;
        }
        return;
    }
    if (((int)(unsigned char)*(signed char *)((char *)(func_000A0DF4(a3) + a3) - 1)) == 92) {
        func_000A0ED9(189, (int)D_00175D00);
        mc_sprintf(l_1C, (int)D_00175D60, a3, a1);
    } else {
        func_000A0ED9(191, (int)D_00175D00);
        mc_sprintf(l_1C, (int)D_00175D65, a3, a1);
    }
    l_14 = open(l_1C, 610, 384);
    if (l_14 == (-1)) {
        if (l_1C != 0 && l_1C != (-1751672937)) {
            mc_free(l_1C, (int)D_00175D00, 197);
            l_1C = -1751672937;
        }
        func_0009DEA7(l_18);
        return;
    }
    l_10 = func_000A00CB(l_18, D_00147954, 102400);
    while (l_10 == 102400) {
        write(l_14, D_00147954, l_10);
        l_10 = func_000A00CB(l_18, D_00147954, 102400);
    }
    write(l_14, D_00147954, l_10);
    func_0009DEA7(l_14);
    func_0009DEA7(l_18);
    if (l_1C == 0 || l_1C == (-1751672937)) return;
    mc_free(l_1C, (int)D_00175D00, 213);
    l_1C = -1751672937;
}

int disk_resolve_path(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    {
        char l_8C[80];
        char l_3C[16];

        if (strchr(a1, 92) != 0 && ((int)(unsigned char)*(signed char *)((char *)a1 + 1)) != 58) {
            l_20 = 0;
            while (((int)(unsigned char)*(signed char *)((char *)(a1 + l_20))) != 92) {
                *(signed char *)((char *)l_3C + l_20) = *(signed char *)((char *)(a1 + l_20));
                l_20++;
            }
            *(signed char *)((char *)l_3C + l_20++) = 92;
            *(signed char *)((char *)l_3C + l_20) = 0;
        } else {
            *(signed char *)l_3C = 0;
        }
        l_24 = a1;
        a1 += func_000A0DF4(a1) - 1;
        while (((int)(unsigned char)*(signed char *)((char *)a1)) != 92 && a1 != l_24) a1--;
        if (((int)(unsigned char)*(signed char *)((char *)a1)) == 92) a1++;
        if (strnicmp((func_000A0DF4(a1) - 3) + a1, (int)D_00175D6B, 3) == 0) {
            func_000A0ED9(246, (int)D_00175D00);
            mc_sprintf((int)disk_path, (int)D_00175D60, (int)D_001917E4, a1);
            return (int)disk_path;
        }
        if (strnicmp((func_000A0DF4(a1) - 3) + a1, (int)D_00175D6F, 3) == 0) {
            func_000A0ED9(252, (int)D_00175D00);
            mc_sprintf((int)disk_path, (int)D_00175D60, (int)D_001917E4, a1);
            return (int)disk_path;
        }
        l_24 = D_001A4A00;
        for (l_20 = 0; l_20 < D_001A4A08; l_20++) {
            if (stricmp(a1, l_24) == 0) {
                l_1C = l_24 - D_001A4A00;
                *(signed char *)l_8C = 0;
                l_24 = D_001A49F8;
                for (;;) {
                    l_28 = *(int *)((char *)(func_000A0DF4(l_24) + l_24) + 1);
                    if (l_1C < l_28) {
                        if (*(signed char *)l_8C == 0) {
                            func_000A0ED9(269, (int)D_00175D00);
                            mc_sprintf((int)disk_path, (int)D_00175D73, (int)D_001917E4, (int)l_8C, a1);
                        } else {
                            func_000A0ED9(271, (int)D_00175D00);
                            mc_sprintf((int)disk_path, (int)D_00175D7A, (int)D_001917E4, (int)l_8C, a1);
                        }
                        return (int)disk_path;
                    }
                    mc_strncpy((int)l_8C, l_24, 80, (int)D_00175D00, 274);
                    l_24 += func_000A0DF4(l_24) + 5;
                }
            }
            l_24 += func_000A0DF4(l_24) + 1;
        }
        func_000A0ED9(281, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D73, (int)D_00191834, (int)l_3C, a1);
        return (int)disk_path;
    }
}

void func_0006D430(int a1)
{
    mc_strncpy(D_001A49FC, a1, 4, (int)D_00175D00, 287);
    D_001A49FC += func_000A0DF4(a1) + 1;
    *(int *)(*(char **)&D_001A49FC) = D_001A49F4 - D_001A4A00;
    D_001A49FC += 4;
}

void func_0006D491(int a1)
{
    mc_strncpy(D_001A49F4, a1, 4, (int)D_00175D00, 295);
    D_001A49F4 += func_000A0DF4(a1) + 1;
    D_001A4A08++;
}

void file_index_build(void)
{
    int l_1C;
    int l_18;

    l_18 = mc_malloc(102400, (int)D_00175D00, 303);
    D_001A4A08 = 0;
    D_001A49F8 = (D_001A49FC = l_18);
    D_001A4A00 = (D_001A49F4 = l_18 + 1024);
    file_index_scan((int)D_001917E4);
    func_0006D430((int)D_00175D82);
    l_1C = mc_malloc((D_001A49FC - D_001A49F8) + 1, (int)D_00175D00, 311);
    mc_memcpy(l_1C, D_001A49F8, (D_001A49FC - D_001A49F8) + 1, (int)D_00175D00, 312, 4);
    D_001A49F8 = l_1C;
    l_1C = mc_malloc((D_001A49F4 - D_001A4A00) + 1, (int)D_00175D00, 315);
    mc_memcpy(l_1C, D_001A4A00, (int)&*(signed char *)((char *)(D_001A49F4 - D_001A4A00) + 1), (int)D_00175D00, 316, 4);
    D_001A4A00 = l_1C;
    D_001A4A04 = (int)disk_resolve_path;
    if (l_18 == 0 || l_18 == (-1751672937)) return;
    mc_free(l_18, (int)D_00175D00, 320);
    l_18 = -1751672937;
}

void file_index_free(void)
{
    if (D_001A49F8 != 0 && D_001A49F8 != (-1751672937)) {
        mc_free(D_001A49F8, (int)D_00175D00, 325);
        D_001A49F8 = -1751672937;
    }
    if (D_001A4A00 == 0 || D_001A4A00 == (-1751672937)) return;
    mc_free(D_001A4A00, (int)D_00175D00, 326);
    D_001A4A00 = -1751672937;
}
