/* disk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"

extern char disk_last_file_size[];
extern iptr D_00147954;
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
extern iptr file_index_names_end;
extern iptr file_index_dirs;
extern iptr file_index_dirs_end;
extern iptr file_index_names;
extern iptr file_resolver;
extern int file_index_count;

extern void fatal_error(char *);
extern void file_index_scan(char *);
int disk_write_file(char *, iptr, int);
int disk_open_data(char *);
char *disk_resolve_path(char *);
void file_index_add_dir(char *);
#pragma aux mc_set_location parm routine [];

iptr disk_read_file(char *name, iptr buffer)
{
    iptr data;
    int handle;
    int size;
    int bytes_read;

    data = buffer;
    handle = disk_open_data(name);
    if (handle < 0) {
        mc_set_location(45, D_00175D00);
        mc_sprintf(disk_path, D_00175D07, name);
        fatal_error(disk_path);
    }
    size = (*(int *)disk_last_file_size = filelength(handle));
    if (data == 0) data = (iptr)mc_malloc(size, D_00175D00, 52);
    if (data == 0) {
        mc_set_location(56, D_00175D00);
        mc_sprintf(disk_path, D_00175D22, name);
        fatal_error(disk_path);
    }
    mc_memset((void *)data, 0, size, D_00175D00, 60, 4);
    bytes_read = read(handle, (void *)data, size);
    if (bytes_read != size) {
        mc_set_location(65, D_00175D00);
        mc_sprintf(disk_path, D_00175D44, name);
        fatal_error(disk_path);
    }
    close(handle);
    return data;
}

int disk_write_file(char *path, iptr data, int size)
{
    int handle;
    int ok;

    unlink(path);
    handle = open(path, 546, 384);
    if (handle < 1) return 0;
    ok = ((write(handle, (void *)data, size) == size) ? 1 : 0);
    close(handle);
    return ok;
}

int disk_write_arena2_file(char *name, iptr data, int size)
{
    mc_set_location(107, D_00175D00);
    mc_sprintf(disk_path, D_00175D60, (iptr)arena2_path, name);
    return disk_write_file(disk_path, data, size);
}

int disk_open_data(char *name)
{
    name = disk_resolve_path(name);
    return open(name, 512);
}

int disk_open_rw(char *name)
{
    mc_set_location(135, D_00175D00);
    mc_sprintf(disk_path, D_00175D60, (iptr)arena2_path, name);
    return open(disk_path, 514);
}

int disk_create(char *name)
{
    mc_set_location(149, D_00175D00);
    mc_sprintf(disk_path, D_00175D60, (iptr)arena2_path, name);
    unlink(disk_path);
    return open(disk_path, 546, 384);
}

int disk_file_exists(char *name)
{
    short handle;
    char path[92];

    mc_set_location(159, D_00175D00);
    mc_sprintf(path, D_00175D60, (iptr)arena2_path, name);
    *(int *)&handle = open(path, 512);
    if (handle < 0) return 0;
    close((int)(short)handle);
    return 1;
}

void disk_copy_file(char *name, char *from_dir, char *to_dir)
{
    char *path;
    int src_handle;
    int dst_handle;
    int count;

    path = (char *)mc_malloc(4096, D_00175D00, 172);
    if (from_dir[strlen(from_dir) - 1] == 92) {
        mc_set_location(176, D_00175D00);
        mc_sprintf(path, D_00175D60, from_dir, name);
    } else {
        mc_set_location(178, D_00175D00);
        mc_sprintf(path, D_00175D65, from_dir, name);
    }
    src_handle = open(path, 512);
    if (src_handle == (-1)) {
        if (path != 0 && path != (char *)(iptr)-1751672937) {
            mc_free(path, D_00175D00, 184);
            path = (char *)(iptr)-1751672937;
        }
        return;
    }
    if (to_dir[strlen(to_dir) - 1] == 92) {
        mc_set_location(189, D_00175D00);
        mc_sprintf(path, D_00175D60, to_dir, name);
    } else {
        mc_set_location(191, D_00175D00);
        mc_sprintf(path, D_00175D65, to_dir, name);
    }
    dst_handle = open(path, 610, 384);
    if (dst_handle == (-1)) {
        if (path != 0 && path != (char *)(iptr)-1751672937) {
            mc_free(path, D_00175D00, 197);
            path = (char *)(iptr)-1751672937;
        }
        close(src_handle);
        return;
    }
    count = read(src_handle, (void *)D_00147954, 102400);
    while (count == 102400) {
        write(dst_handle, (void *)D_00147954, count);
        count = read(src_handle, (void *)D_00147954, 102400);
    }
    write(dst_handle, (void *)D_00147954, count);
    close(dst_handle);
    close(src_handle);
    if (path == 0 || path == (char *)(iptr)-1751672937) return;
    mc_free(path, D_00175D00, 213);
    path = (char *)(iptr)-1751672937;
}

char *disk_resolve_path(char *name)
{
    int dir_start;
    char *entry;
    int i;
    int offset;
    {
        char dir[80];
        char prefix[16];

        if (strchr(name, 92) != 0 && name[1] != 58) {
            i = 0;
            while (name[i] != 92) {
                prefix[i] = name[i];
                i++;
            }
            prefix[i++] = 92;
            prefix[i] = 0;
        } else {
            prefix[0] = 0;
        }
        entry = name;
        name += strlen(name) - 1;
        while (*name != 92 && name != entry) name--;
        if (*name == 92) name++;
        if (strnicmp((strlen(name) - 3) + name, D_00175D6B, 3) == 0) {
            mc_set_location(246, D_00175D00);
            mc_sprintf(disk_path, D_00175D60, (iptr)arena2_path, name);
            return disk_path;
        }
        if (strnicmp((strlen(name) - 3) + name, D_00175D6F, 3) == 0) {
            mc_set_location(252, D_00175D00);
            mc_sprintf(disk_path, D_00175D60, (iptr)arena2_path, name);
            return disk_path;
        }
        entry = (char *)file_index_names;
        for (i = 0; i < file_index_count; i++) {
            if (stricmp(name, entry) == 0) {
                offset = (int)((iptr)entry - file_index_names);
                dir[0] = 0;
                entry = (char *)file_index_dirs;
                for (;;) {
                    dir_start = *(int *)(entry + strlen(entry) + 1);
                    if (offset < dir_start) {
                        if (dir[0] == 0) {
                            mc_set_location(269, D_00175D00);
                            mc_sprintf(disk_path, D_00175D73, (iptr)arena2_path, (iptr)dir, name);
                        } else {
                            mc_set_location(271, D_00175D00);
                            mc_sprintf(disk_path, D_00175D7A, (iptr)arena2_path, (iptr)dir, name);
                        }
                        return disk_path;
                    }
                    mc_strncpy(dir, entry, 80, D_00175D00, 274);
                    entry += strlen(entry) + 5;
                }
            }
            entry += strlen(entry) + 1;
        }
        mc_set_location(281, D_00175D00);
        mc_sprintf(disk_path, D_00175D73, (iptr)arena2_cd_path, (iptr)prefix, name);
        return disk_path;
    }
}

void file_index_add_dir(char *name)
{
    mc_strncpy((char *)file_index_dirs_end, name, 4, D_00175D00, 287);
    file_index_dirs_end += strlen(name) + 1;
    *(iptr *)(*(char **)&file_index_dirs_end) = file_index_names_end - file_index_names;
    file_index_dirs_end += 4;
}

void file_index_add_name(char *name)
{
    mc_strncpy((char *)file_index_names_end, name, 4, D_00175D00, 295);
    file_index_names_end += strlen(name) + 1;
    file_index_count++;
}

void file_index_build(void)
{
    iptr copy;
    iptr buffer;

    buffer = (iptr)mc_malloc(102400, D_00175D00, 303);
    file_index_count = 0;
    file_index_dirs = (file_index_dirs_end = buffer);
    file_index_names = (file_index_names_end = buffer + 1024);
    file_index_scan(arena2_path);
    file_index_add_dir(D_00175D82);
    copy = (iptr)mc_malloc((int)(file_index_dirs_end - file_index_dirs) + 1, D_00175D00, 311);
    mc_memcpy((void *)copy, (void *)file_index_dirs, (int)(file_index_dirs_end - file_index_dirs) + 1, D_00175D00, 312, 4);
    file_index_dirs = copy;
    copy = (iptr)mc_malloc((int)(file_index_names_end - file_index_names) + 1, D_00175D00, 315);
    mc_memcpy((void *)copy, (void *)file_index_names, (int)(iptr)&*(signed char *)((char *)(file_index_names_end - file_index_names) + 1), D_00175D00, 316, 4);
    file_index_names = copy;
    file_resolver = (iptr)disk_resolve_path;
    if (buffer == 0 || buffer == (-1751672937)) return;
    mc_free((void *)buffer, D_00175D00, 320);
    buffer = -1751672937;
}

void file_index_free(void)
{
    if (file_index_dirs != 0 && file_index_dirs != (-1751672937)) {
        mc_free((void *)file_index_dirs, D_00175D00, 325);
        file_index_dirs = -1751672937;
    }
    if (file_index_names == 0 || file_index_names == (-1751672937)) return;
    mc_free((void *)file_index_names, D_00175D00, 326);
    file_index_names = -1751672937;
}
