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
extern char arena2_path[];
extern char arena2_cd_path[];
extern char disk_path[];
extern int file_index_names_end;
extern int file_index_dirs;
extern int file_index_dirs_end;
extern int file_index_names;
extern int file_resolver;
extern int file_index_count;

extern int open(int, ...);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int read();
extern int mc_strncpy();
extern int write();
extern int strlen();
extern int stricmp();
extern int strnicmp();
extern int mc_set_location(int, int);
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
void file_index_add_dir(int);
#pragma aux mc_set_location parm routine [];

int disk_read_file(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = a2;
    l_20 = disk_open_data(a1);
    if (l_20 < 0) {
        mc_set_location(45, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D07, a1);
        fatal_error((int)disk_path);
    }
    l_1C = (*(int *)disk_last_file_size = filelength(l_20));
    if (l_24 == 0) l_24 = mc_malloc(l_1C, (int)D_00175D00, 52);
    if (l_24 == 0) {
        mc_set_location(56, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D22, a1);
        fatal_error((int)disk_path);
    }
    mc_memset(l_24, 0, l_1C, (int)D_00175D00, 60, 4);
    l_18 = read(l_20, l_24, l_1C);
    if (l_18 != l_1C) {
        mc_set_location(65, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D44, a1);
        fatal_error((int)disk_path);
    }
    close(l_20);
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
    close(l_18);
    return l_14;
}

int disk_write_arena2_file(int a1, int a2, int a3)
{
    mc_set_location(107, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, a1);
    return disk_write_file((int)disk_path, a2, a3);
}

int disk_open_data(int a1)
{
    a1 = disk_resolve_path(a1);
    return open(a1, 512);
}

int disk_open_rw(int a1)
{
    mc_set_location(135, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, a1);
    return open((int)disk_path, 514);
}

int disk_create(int a1)
{
    mc_set_location(149, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, a1);
    unlink((int)disk_path);
    return open((int)disk_path, 546, 384);
}

int disk_file_exists(int a1)
{
    short l_18;
    char l_7C[92];

    mc_set_location(159, (int)D_00175D00);
    mc_sprintf((int)l_7C, (int)D_00175D60, (int)arena2_path, a1);
    *(int *)&l_18 = open((int)l_7C, 512);
    if (l_18 < 0) return 0;
    close((int)(short)l_18);
    return 1;
}

void disk_copy_file(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_1C = mc_malloc(4096, (int)D_00175D00, 172);
    if (((int)(unsigned char)*(signed char *)((char *)(strlen(a2) + a2) - 1)) == 92) {
        mc_set_location(176, (int)D_00175D00);
        mc_sprintf(l_1C, (int)D_00175D60, a2, a1);
    } else {
        mc_set_location(178, (int)D_00175D00);
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
    if (((int)(unsigned char)*(signed char *)((char *)(strlen(a3) + a3) - 1)) == 92) {
        mc_set_location(189, (int)D_00175D00);
        mc_sprintf(l_1C, (int)D_00175D60, a3, a1);
    } else {
        mc_set_location(191, (int)D_00175D00);
        mc_sprintf(l_1C, (int)D_00175D65, a3, a1);
    }
    l_14 = open(l_1C, 610, 384);
    if (l_14 == (-1)) {
        if (l_1C != 0 && l_1C != (-1751672937)) {
            mc_free(l_1C, (int)D_00175D00, 197);
            l_1C = -1751672937;
        }
        close(l_18);
        return;
    }
    l_10 = read(l_18, D_00147954, 102400);
    while (l_10 == 102400) {
        write(l_14, D_00147954, l_10);
        l_10 = read(l_18, D_00147954, 102400);
    }
    write(l_14, D_00147954, l_10);
    close(l_14);
    close(l_18);
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
        a1 += strlen(a1) - 1;
        while (((int)(unsigned char)*(signed char *)((char *)a1)) != 92 && a1 != l_24) a1--;
        if (((int)(unsigned char)*(signed char *)((char *)a1)) == 92) a1++;
        if (strnicmp((strlen(a1) - 3) + a1, (int)D_00175D6B, 3) == 0) {
            mc_set_location(246, (int)D_00175D00);
            mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, a1);
            return (int)disk_path;
        }
        if (strnicmp((strlen(a1) - 3) + a1, (int)D_00175D6F, 3) == 0) {
            mc_set_location(252, (int)D_00175D00);
            mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, a1);
            return (int)disk_path;
        }
        l_24 = file_index_names;
        for (l_20 = 0; l_20 < file_index_count; l_20++) {
            if (stricmp(a1, l_24) == 0) {
                l_1C = l_24 - file_index_names;
                *(signed char *)l_8C = 0;
                l_24 = file_index_dirs;
                for (;;) {
                    l_28 = *(int *)((char *)(strlen(l_24) + l_24) + 1);
                    if (l_1C < l_28) {
                        if (*(signed char *)l_8C == 0) {
                            mc_set_location(269, (int)D_00175D00);
                            mc_sprintf((int)disk_path, (int)D_00175D73, (int)arena2_path, (int)l_8C, a1);
                        } else {
                            mc_set_location(271, (int)D_00175D00);
                            mc_sprintf((int)disk_path, (int)D_00175D7A, (int)arena2_path, (int)l_8C, a1);
                        }
                        return (int)disk_path;
                    }
                    mc_strncpy((int)l_8C, l_24, 80, (int)D_00175D00, 274);
                    l_24 += strlen(l_24) + 5;
                }
            }
            l_24 += strlen(l_24) + 1;
        }
        mc_set_location(281, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D73, (int)arena2_cd_path, (int)l_3C, a1);
        return (int)disk_path;
    }
}

void file_index_add_dir(int a1)
{
    mc_strncpy(file_index_dirs_end, a1, 4, (int)D_00175D00, 287);
    file_index_dirs_end += strlen(a1) + 1;
    *(int *)(*(char **)&file_index_dirs_end) = file_index_names_end - file_index_names;
    file_index_dirs_end += 4;
}

void file_index_add_name(int a1)
{
    mc_strncpy(file_index_names_end, a1, 4, (int)D_00175D00, 295);
    file_index_names_end += strlen(a1) + 1;
    file_index_count++;
}

void file_index_build(void)
{
    int l_1C;
    int l_18;

    l_18 = mc_malloc(102400, (int)D_00175D00, 303);
    file_index_count = 0;
    file_index_dirs = (file_index_dirs_end = l_18);
    file_index_names = (file_index_names_end = l_18 + 1024);
    file_index_scan((int)arena2_path);
    file_index_add_dir((int)D_00175D82);
    l_1C = mc_malloc((file_index_dirs_end - file_index_dirs) + 1, (int)D_00175D00, 311);
    mc_memcpy(l_1C, file_index_dirs, (file_index_dirs_end - file_index_dirs) + 1, (int)D_00175D00, 312, 4);
    file_index_dirs = l_1C;
    l_1C = mc_malloc((file_index_names_end - file_index_names) + 1, (int)D_00175D00, 315);
    mc_memcpy(l_1C, file_index_names, (int)&*(signed char *)((char *)(file_index_names_end - file_index_names) + 1), (int)D_00175D00, 316, 4);
    file_index_names = l_1C;
    file_resolver = (int)disk_resolve_path;
    if (l_18 == 0 || l_18 == (-1751672937)) return;
    mc_free(l_18, (int)D_00175D00, 320);
    l_18 = -1751672937;
}

void file_index_free(void)
{
    if (file_index_dirs != 0 && file_index_dirs != (-1751672937)) {
        mc_free(file_index_dirs, (int)D_00175D00, 325);
        file_index_dirs = -1751672937;
    }
    if (file_index_names == 0 || file_index_names == (-1751672937)) return;
    mc_free(file_index_names, (int)D_00175D00, 326);
    file_index_names = -1751672937;
}
