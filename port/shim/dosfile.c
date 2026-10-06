/* dosfile.c: DOS files for the game (docs/port.md). The game names files the DOS way: drive
   letters, backslashes, any case, 8.3 names. The DOS game ran with C:\ as its install folder
   (tools/fallemu.py maps it the same way), so every path, absolute or relative, is taken from
   the install folder. Writes go to an overlay folder, and reads look there first, so the
   install is never changed: a file opened for update is copied to the overlay first.

   open() takes Watcom's flags (fcntl.h of Watcom 10.0a: O_CREAT 0x20, O_BINARY 0x200 ...),
   which differ from the host's. */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dos.h"
#include "port_host.h"

static char game_dir[1024] = ".";
static char overlay_dir[1024] = ".";

void dos_set_dirs(const char *game, const char *overlay)
{
    snprintf(game_dir, sizeof game_dir, "%s", game);
    snprintf(overlay_dir, sizeof overlay_dir, "%s", overlay);
    mkdir(overlay_dir, 0755);
}

/* split a DOS path into its components below C:\ (drive and leading separators dropped) */
static int dos_components(const char *dos, char comp[][64], int max)
{
    int n = 0, k = 0;

    if (dos[0] != '\0' && dos[1] == ':')
        dos += 2;
    for (; ; dos++) {
        char c = *dos;
        if (c == '\\' || c == '/' || c == '\0') {
            if (k > 0 && n < max) {
                comp[n][k] = '\0';
                if (strcmp(comp[n], ".") != 0)
                    n++;
            }
            k = 0;
            if (c == '\0')
                break;
        } else if (k < 63) {
            comp[n < max ? n : max - 1][k++] = c;
        }
    }
    return n;
}

/* root/comp[0]/comp[1]/... with each component matched without regard to case; 1 when it
   exists. A component that matches nothing is kept as the game spelled it, upper-cased. */
static int resolve(const char *root, char comp[][64], int n, char *out, size_t size)
{
    int i, exists = 1;

    snprintf(out, size, "%s", root);
    for (i = 0; i < n; i++) {
        size_t len = strlen(out);
        const char *name = comp[i];
        char found[256] = "";
        if (exists) {
            DIR *d = opendir(out);
            struct dirent *e;
            while (d && (e = readdir(d)) != NULL) {
                if (strcasecmp(e->d_name, name) == 0) {
                    snprintf(found, sizeof found, "%s", e->d_name);
                    break;
                }
            }
            if (d)
                closedir(d);
        }
        if (found[0] == '\0') {
            char *p;
            exists = 0;
            snprintf(found, sizeof found, "%s", name);
            for (p = found; *p; p++)
                if (*p >= 'a' && *p <= 'z')
                    *p -= 'a' - 'A';
        }
        snprintf(out + len, size - len, "/%s", found);
    }
    if (!exists)
        return 0;
    {
        struct stat st;
        return stat(out, &st) == 0;
    }
}

static void make_parents(const char *path)
{
    char tmp[1024];
    char *p;

    snprintf(tmp, sizeof tmp, "%s", path);
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
}

static int copy_file(const char *from, const char *to)
{
    FILE *a = fopen(from, "rb"), *b;
    char buf[65536];
    size_t n;

    if (a == NULL)
        return -1;
    make_parents(to);
    b = fopen(to, "wb");
    if (b == NULL) {
        fclose(a);
        return -1;
    }
    while ((n = fread(buf, 1, sizeof buf, a)) > 0)
        fwrite(buf, 1, n, b);
    fclose(a);
    fclose(b);
    return 0;
}

int dos_host_path(const char *dos, int writing, char *out, size_t size)
{
    char comp[16][64];
    int n = dos_components(dos, comp, 16);

    if (resolve(overlay_dir, comp, n, out, size))
        return 1;
    if (!writing)
        return resolve(game_dir, comp, n, out, size);
    /* writing: into the overlay, from the install's copy when it has one (copy on write is
       the caller's: it knows whether the file is truncated) */
    make_parents(out);
    return 1;
}

/* Watcom's open flags */
#define W_WRONLY 0x0001
#define W_RDWR 0x0002
#define W_APPEND 0x0010
#define W_CREAT 0x0020
#define W_TRUNC 0x0040
#define W_EXCL 0x0400

int port_open(const char *dos, int wflags, ...)
{
    char path[1024], orig[1024];
    int writing = (wflags & (W_WRONLY | W_RDWR | W_APPEND | W_CREAT | W_TRUNC)) != 0;
    int flags = 0, mode = 0644;

    if (wflags & W_CREAT) {
        va_list ap;
        va_start(ap, wflags);
        mode = va_arg(ap, int) & 0777;
        va_end(ap);
        if (mode == 0)
            mode = 0644;
    }
    flags |= (wflags & W_RDWR) ? O_RDWR : (wflags & W_WRONLY) ? O_WRONLY : O_RDONLY;
    if (wflags & W_APPEND) flags |= O_APPEND;
    if (wflags & W_CREAT) flags |= O_CREAT;
    if (wflags & W_TRUNC) flags |= O_TRUNC;
    if (wflags & W_EXCL) flags |= O_EXCL;

    if (!dos_host_path(dos, writing, path, sizeof path)) {
        errno = ENOENT;
        return -1;
    }
    if (writing && !(wflags & W_TRUNC) && access(path, F_OK) != 0) {
        char comp[16][64];
        int n = dos_components(dos, comp, 16);
        if (resolve(game_dir, comp, n, orig, sizeof orig))
            copy_file(orig, path);
    }
    return open(path, flags, mode);
}

int port_read(int fd, void *buf, unsigned int n)
{
    return (int)read(fd, buf, n);
}

int port_write(int fd, const void *buf, unsigned int n)
{
    return (int)write(fd, buf, n);
}

int port_lseek(int fd, int offset, int whence)
{
    return (int)lseek(fd, offset, whence);
}

int port_close(int fd)
{
    return close(fd);
}

int port_filelength(int fd)
{
    struct stat st;

    if (fstat(fd, &st) != 0)
        return -1;
    return (int)st.st_size;
}

/* only the overlay's copy goes; the install is never changed */
int port_unlink(const char *dos)
{
    char path[1024], comp[16][64];
    int n = dos_components(dos, comp, 16);

    if (!resolve(overlay_dir, comp, n, path, sizeof path)) {
        errno = ENOENT;
        return -1;
    }
    return unlink(path);
}

FILE *port_fopen(const char *dos, const char *mode)
{
    char path[1024], orig[1024], m[8];
    int writing = strpbrk(mode, "wa+") != NULL;
    int i, k = 0;

    for (i = 0; mode[i] && k < 7; i++)
        if (mode[i] != 't')
            m[k++] = mode[i];
    m[k] = '\0';
    if (!dos_host_path(dos, writing, path, sizeof path))
        return NULL;
    if (writing && mode[0] != 'w' && access(path, F_OK) != 0) {
        char comp[16][64];
        int n = dos_components(dos, comp, 16);
        if (resolve(game_dir, comp, n, orig, sizeof orig))
            copy_file(orig, path);
    }
    return fopen(path, m);
}

int port_fclose(FILE *f)
{
    return fclose(f);
}

int port_fprintf(FILE *f, const char *fmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vfprintf(f, fmt, ap);
    va_end(ap);
    return n;
}
