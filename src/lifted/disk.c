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

extern int open(char *, ...);
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
extern void fatal_error(char *);
extern void file_index_scan(char *);
int disk_write_file(char *, int, int);
int disk_open_data(char *);
char *disk_resolve_path(char *);
void file_index_add_dir(char *);
#pragma aux mc_set_location parm routine [];

int disk_read_file(char *name, int buffer)
{
    int data;
    int handle;
    int size;
    int bytes_read;

    data = buffer;
    handle = disk_open_data(name);
    if (handle < 0) {
        mc_set_location(45, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D07, name);
        fatal_error(disk_path);
    }
    size = (*(int *)disk_last_file_size = filelength(handle));
    if (data == 0) data = mc_malloc(size, (int)D_00175D00, 52);
    if (data == 0) {
        mc_set_location(56, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D22, name);
        fatal_error(disk_path);
    }
    mc_memset(data, 0, size, (int)D_00175D00, 60, 4);
    bytes_read = read(handle, data, size);
    if (bytes_read != size) {
        mc_set_location(65, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D44, name);
        fatal_error(disk_path);
    }
    close(handle);
    return data;
}

int disk_write_file(char *path, int data, int size)
{
    int handle;
    int ok;

    unlink(path);
    handle = open(path, 546, 384);
    if (handle < 1) return 0;
    ok = ((write(handle, data, size) == size) ? 1 : 0);
    close(handle);
    return ok;
}

int disk_write_arena2_file(char *name, int data, int size)
{
    mc_set_location(107, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, name);
    return disk_write_file(disk_path, data, size);
}

int disk_open_data(char *name)
{
    name = disk_resolve_path(name);
    return open(name, 512);
}

int disk_open_rw(char *name)
{
    mc_set_location(135, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, name);
    return open(disk_path, 514);
}

int disk_create(char *name)
{
    mc_set_location(149, (int)D_00175D00);
    mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, name);
    unlink((int)disk_path);
    return open(disk_path, 546, 384);
}

int disk_file_exists(char *name)
{
    short handle;
    char path[92];

    mc_set_location(159, (int)D_00175D00);
    mc_sprintf((int)path, (int)D_00175D60, (int)arena2_path, name);
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

    path = (char *)mc_malloc(4096, (int)D_00175D00, 172);
    if (from_dir[strlen(from_dir) - 1] == 92) {
        mc_set_location(176, (int)D_00175D00);
        mc_sprintf((int)path, (int)D_00175D60, from_dir, name);
    } else {
        mc_set_location(178, (int)D_00175D00);
        mc_sprintf((int)path, (int)D_00175D65, from_dir, name);
    }
    src_handle = open(path, 512);
    if (src_handle == (-1)) {
        if (path != 0 && path != (char *)-1751672937) {
            mc_free(path, (int)D_00175D00, 184);
            path = (char *)-1751672937;
        }
        return;
    }
    if (to_dir[strlen(to_dir) - 1] == 92) {
        mc_set_location(189, (int)D_00175D00);
        mc_sprintf((int)path, (int)D_00175D60, to_dir, name);
    } else {
        mc_set_location(191, (int)D_00175D00);
        mc_sprintf((int)path, (int)D_00175D65, to_dir, name);
    }
    dst_handle = open(path, 610, 384);
    if (dst_handle == (-1)) {
        if (path != 0 && path != (char *)-1751672937) {
            mc_free(path, (int)D_00175D00, 197);
            path = (char *)-1751672937;
        }
        close(src_handle);
        return;
    }
    count = read(src_handle, D_00147954, 102400);
    while (count == 102400) {
        write(dst_handle, D_00147954, count);
        count = read(src_handle, D_00147954, 102400);
    }
    write(dst_handle, D_00147954, count);
    close(dst_handle);
    close(src_handle);
    if (path == 0 || path == (char *)-1751672937) return;
    mc_free(path, (int)D_00175D00, 213);
    path = (char *)-1751672937;
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
        if (strnicmp((strlen(name) - 3) + name, (int)D_00175D6B, 3) == 0) {
            mc_set_location(246, (int)D_00175D00);
            mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, name);
            return disk_path;
        }
        if (strnicmp((strlen(name) - 3) + name, (int)D_00175D6F, 3) == 0) {
            mc_set_location(252, (int)D_00175D00);
            mc_sprintf((int)disk_path, (int)D_00175D60, (int)arena2_path, name);
            return disk_path;
        }
        entry = (char *)file_index_names;
        for (i = 0; i < file_index_count; i++) {
            if (stricmp(name, entry) == 0) {
                offset = (int)entry - file_index_names;
                dir[0] = 0;
                entry = (char *)file_index_dirs;
                for (;;) {
                    dir_start = *(int *)(entry + strlen(entry) + 1);
                    if (offset < dir_start) {
                        if (dir[0] == 0) {
                            mc_set_location(269, (int)D_00175D00);
                            mc_sprintf((int)disk_path, (int)D_00175D73, (int)arena2_path, (int)dir, name);
                        } else {
                            mc_set_location(271, (int)D_00175D00);
                            mc_sprintf((int)disk_path, (int)D_00175D7A, (int)arena2_path, (int)dir, name);
                        }
                        return disk_path;
                    }
                    mc_strncpy((int)dir, entry, 80, (int)D_00175D00, 274);
                    entry += strlen(entry) + 5;
                }
            }
            entry += strlen(entry) + 1;
        }
        mc_set_location(281, (int)D_00175D00);
        mc_sprintf((int)disk_path, (int)D_00175D73, (int)arena2_cd_path, (int)prefix, name);
        return disk_path;
    }
}

void file_index_add_dir(char *name)
{
    mc_strncpy(file_index_dirs_end, name, 4, (int)D_00175D00, 287);
    file_index_dirs_end += strlen(name) + 1;
    *(int *)(*(char **)&file_index_dirs_end) = file_index_names_end - file_index_names;
    file_index_dirs_end += 4;
}

void file_index_add_name(char *name)
{
    mc_strncpy(file_index_names_end, name, 4, (int)D_00175D00, 295);
    file_index_names_end += strlen(name) + 1;
    file_index_count++;
}

void file_index_build(void)
{
    int copy;
    int buffer;

    buffer = mc_malloc(102400, (int)D_00175D00, 303);
    file_index_count = 0;
    file_index_dirs = (file_index_dirs_end = buffer);
    file_index_names = (file_index_names_end = buffer + 1024);
    file_index_scan(arena2_path);
    file_index_add_dir(D_00175D82);
    copy = mc_malloc((file_index_dirs_end - file_index_dirs) + 1, (int)D_00175D00, 311);
    mc_memcpy(copy, file_index_dirs, (file_index_dirs_end - file_index_dirs) + 1, (int)D_00175D00, 312, 4);
    file_index_dirs = copy;
    copy = mc_malloc((file_index_names_end - file_index_names) + 1, (int)D_00175D00, 315);
    mc_memcpy(copy, file_index_names, (int)&*(signed char *)((char *)(file_index_names_end - file_index_names) + 1), (int)D_00175D00, 316, 4);
    file_index_names = copy;
    file_resolver = (int)disk_resolve_path;
    if (buffer == 0 || buffer == (-1751672937)) return;
    mc_free(buffer, (int)D_00175D00, 320);
    buffer = -1751672937;
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
