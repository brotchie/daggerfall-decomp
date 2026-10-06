/* dosfile.c: DOS files for the game (docs/port.md). The game names files the DOS way: drive
   letters, backslashes, any case, 8.3 names. The DOS game ran with C:\ as its install folder
   (tools/fallemu.py maps it the same way), so every path, absolute or relative, is taken from
   the install folder. Writes go to an overlay folder, and reads look there first, so the
   install is never changed: a file opened for update is copied to the overlay first.

   open() takes Watcom's flags (fcntl.h of Watcom 10.0a: O_CREAT 0x20, O_BINARY 0x200 ...),
   which differ from the host's. */
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
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

/* DOS's handles: the lowest free one from 5 (0-4 are the standard devices), at most 255, each
   standing for a host file descriptor. The game keeps tables by handle (the archives' names,
   directories and types: 20 handles, DOS's default), which a host's descriptor numbers (SDL,
   the audio and the GPU hold many) would run past. */
#define DOS_HANDLES 256
static int host_fds[DOS_HANDLES];       /* host fd + 1; 0 when free */

static int dos_handle(int fd)
{
    int h;

    if (fd < 0)
        return fd;
    for (h = 5; h < DOS_HANDLES; h++) {
        if (host_fds[h] == 0) {
            host_fds[h] = fd + 1;
            return h;
        }
    }
    close(fd);
    errno = EMFILE;
    return -1;
}

static int host_fd(int h)
{
    if (h < 0 || h >= DOS_HANDLES || host_fds[h] == 0)
        return h;           /* the standard devices, or a host descriptor (port/test/savetest) */
    return host_fds[h] - 1;
}

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
    return dos_handle(open(path, flags, mode));
}

int port_read(int fd, void *buf, unsigned int n)
{
    return (int)read(host_fd(fd), buf, n);
}

int port_write(int fd, const void *buf, unsigned int n)
{
    return (int)write(host_fd(fd), buf, n);
}

int port_lseek(int fd, int offset, int whence)
{
    return (int)lseek(host_fd(fd), offset, whence);
}

int port_close(int fd)
{
    int h = host_fd(fd);

    if (fd >= 5 && fd < DOS_HANDLES)
        host_fds[fd] = 0;
    return h < 0 ? -1 : close(h);
}

int port_filelength(int fd)
{
    struct stat st;

    if (fstat(host_fd(fd), &st) != 0)
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

/* ---- _dos_findfirst and _dos_findnext ------------------------------------------------------
   Watcom 10.0a's (FALL.EXE 0xA13DA and 0xA13F7, CLIB3R's find386): the find_t is the DTA of
   DOS's int 21h 4Eh / 4Fh, the result is 0 or DOS's error code (and errno is set).

   As DOS finds them:
   - a name matches the way DOS matches, in FCB form (8 + 3 characters, '?' any character, a
     '*' fills the rest of its field with '?'), so "*.*" matches every name and "*" only the
     names without an extension;
   - only names that are valid 8.3 names are seen, upper-cased;
   - a directory is found only when the search asks for _A_SUBDIR; a subdirectory (not the
     root) has "." and ".." first;
   - the folder is the overlay's and the install's together, the overlay's file winning, in
     name order (as tools/fallemu.py lists them: DOS would give the directory's own order).
   The search's state is the index of a table entry here, kept in find_t.reserved. */

#define FIND_SLOTS 32
#define DOSERR_PATH_NOT_FOUND 3
#define DOSERR_NO_MORE_FILES 0x12

struct find_entry {
    char name[13];
    unsigned char attrib;
    unsigned short time, date;
    unsigned int size;
};

static struct {
    unsigned int gen;           /* 0: free */
    int n, pos;
    struct find_entry *e;
} searches[FIND_SLOTS];
static unsigned int find_gen;

/* a name or a pattern in FCB form; 0 when `s` is no 8.3 name (only for names: a pattern's
   extra characters are dropped, as DOS drops them) */
static int fcb_form(const char *s, char out[11], int pattern)
{
    int k = 0, len = 0;

    memset(out, ' ', 11);
    if (strcmp(s, ".") == 0 || strcmp(s, "..") == 0) {
        memcpy(out, s, strlen(s));
        return 1;
    }
    if (!pattern && strchr(s, ' '))
        return 0;
    for (; *s && *s != '.'; s++, len++) {
        if (*s == '*' && pattern) {
            while (k < 8)
                out[k++] = '?';
        } else if (k < 8) {
            out[k++] = (char)toupper((unsigned char)*s);
        }
    }
    if (!pattern && (len == 0 || len > 8))
        return 0;
    if (*s == '.') {
        s++;
        k = 8;
        for (len = 0; *s && (pattern || *s != '.'); s++, len++) {
            if (*s == '*' && pattern) {
                while (k < 11)
                    out[k++] = '?';
            } else if (k < 11) {
                out[k++] = (char)toupper((unsigned char)*s);
            }
        }
        if (!pattern && (len > 3 || *s == '.'))
            return 0;
    }
    if (!pattern) {
        for (k = 0; k < 11; k++) {
            unsigned char c = (unsigned char)out[k];
            if ((c < 0x21 && c != ' ') || c >= 0x80 || strchr("\"*+,/:;<=>?[\\]|", c))
                return 0;
        }
    }
    return 1;
}

static int fcb_match(const char pat[11], const char name[11])
{
    int i;

    for (i = 0; i < 11; i++)
        if (pat[i] != '?' && pat[i] != name[i])
            return 0;
    return 1;
}

static void entry_set(struct find_entry *e, const char *name, const struct stat *st)
{
    struct tm tm;
    time_t t = st->st_mtime;
    int i;

    memset(e, 0, sizeof *e);
    for (i = 0; name[i] && i < 12; i++)
        e->name[i] = (char)toupper((unsigned char)name[i]);
    e->attrib = S_ISDIR(st->st_mode) ? _A_SUBDIR : _A_ARCH;
    e->size = S_ISDIR(st->st_mode) ? 0 : (unsigned int)st->st_size;
    if (localtime_r(&t, &tm) && tm.tm_year >= 80) {
        e->time = (unsigned short)(tm.tm_hour << 11 | tm.tm_min << 5 | tm.tm_sec / 2);
        e->date = (unsigned short)((tm.tm_year - 80) << 9 | (tm.tm_mon + 1) << 5 | tm.tm_mday);
    }
}

/* the entries of a host folder that match, added to (or, for the overlay, replacing) *list */
static void find_scan(const char *dir, const char pat[11], unsigned attr,
                      struct find_entry **list, int *n, int *cap)
{
    DIR *d = opendir(dir);
    struct dirent *de;

    if (d == NULL)
        return;
    while ((de = readdir(d)) != NULL) {
        char fcb[11], path[1200];
        struct stat st;
        struct find_entry e;
        int i;

        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        if (!fcb_form(de->d_name, fcb, 0) || !fcb_match(pat, fcb))
            continue;
        snprintf(path, sizeof path, "%s/%s", dir, de->d_name);
        if (stat(path, &st) != 0 || !(S_ISDIR(st.st_mode) || S_ISREG(st.st_mode)))
            continue;
        if (S_ISDIR(st.st_mode) && !(attr & _A_SUBDIR))
            continue;
        entry_set(&e, de->d_name, &st);
        for (i = 0; i < *n; i++)
            if (strcmp((*list)[i].name, e.name) == 0)
                break;
        if (i == *n) {
            if (*n == *cap) {
                *cap = *cap ? *cap * 2 : 32;
                *list = realloc(*list, (size_t)*cap * sizeof **list);
            }
            (*n)++;
        }
        (*list)[i] = e;
    }
    closedir(d);
}

static int entry_cmp(const void *a, const void *b)
{
    const struct find_entry *x = a, *y = b;
    int dx = strcmp(x->name, ".") == 0 ? 0 : strcmp(x->name, "..") == 0 ? 1 : 2;
    int dy = strcmp(y->name, ".") == 0 ? 0 : strcmp(y->name, "..") == 0 ? 1 : 2;

    if (dx != dy)
        return dx - dy;
    return strcmp(x->name, y->name);
}

static void find_fill(struct find_t *ff, const struct find_entry *e)
{
    ff->attrib = (char)e->attrib;
    ff->wr_time = e->time;
    ff->wr_date = e->date;
    ff->size = e->size;
    memset(ff->name, 0, sizeof ff->name);
    memcpy(ff->name, e->name, sizeof e->name);
}

static void find_release(int slot)
{
    free(searches[slot].e);
    searches[slot].e = NULL;
    searches[slot].gen = 0;
}

/* _dos_findfirst (Watcom 10.0a, FALL.EXE 0xA13DA) */
unsigned func_000A13DA(const char *path, unsigned attr, struct find_t *ff)
{
    char comp[16][64], pat[11], odir[1024], gdir[1024];
    struct find_entry *list = NULL;
    int n = dos_components(path, comp, 16), count = 0, cap = 0, have_o, have_g, slot, i;
    unsigned int oldest = ~0u;
    struct stat st;

    memset(ff->reserved, 0, sizeof ff->reserved);
    if (n == 0) {
        errno = ENOENT;
        return DOSERR_NO_MORE_FILES;
    }
    fcb_form(comp[n - 1], pat, 1);
    have_o = resolve(overlay_dir, comp, n - 1, odir, sizeof odir) &&
             stat(odir, &st) == 0 && S_ISDIR(st.st_mode);
    have_g = resolve(game_dir, comp, n - 1, gdir, sizeof gdir) &&
             stat(gdir, &st) == 0 && S_ISDIR(st.st_mode);
    if (!have_o && !have_g) {
        errno = ENOENT;
        return DOSERR_PATH_NOT_FOUND;
    }
    if (n > 1 && (attr & _A_SUBDIR)) {
        static const char *dots[2] = {".", ".."};
        for (i = 0; i < 2; i++) {
            char fcb[11];
            fcb_form(dots[i], fcb, 0);
            if (fcb_match(pat, fcb)) {
                if (count == cap) {
                    cap = cap ? cap * 2 : 32;
                    list = realloc(list, (size_t)cap * sizeof *list);
                }
                entry_set(&list[count], dots[i], &st);
                list[count].attrib = _A_SUBDIR;
                count++;
            }
        }
    }
    if (have_g)
        find_scan(gdir, pat, attr, &list, &count, &cap);
    if (have_o)
        find_scan(odir, pat, attr, &list, &count, &cap);
    if (count == 0) {
        free(list);
        errno = ENOENT;
        return DOSERR_NO_MORE_FILES;
    }
    qsort(list, (size_t)count, sizeof *list, entry_cmp);

    /* a free slot, else the oldest search (one the game left unfinished) */
    for (slot = 0, i = 0; i < FIND_SLOTS; i++) {
        if (searches[i].gen == 0) {
            slot = i;
            break;
        }
        if (searches[i].gen < oldest) {
            oldest = searches[i].gen;
            slot = i;
        }
    }
    if (searches[slot].gen != 0)
        find_release(slot);
    if (++find_gen == 0)
        find_gen = 1;
    searches[slot].gen = find_gen;
    searches[slot].e = list;
    searches[slot].n = count;
    searches[slot].pos = 0;
    ff->reserved[0] = 'P';
    ff->reserved[1] = 'F';
    ff->reserved[2] = (char)slot;
    memcpy(&ff->reserved[3], &find_gen, sizeof find_gen);
    find_fill(ff, &list[0]);
    return 0;
}

/* _dos_findnext (Watcom 10.0a, FALL.EXE 0xA13F7) */
unsigned func_000A13F7(struct find_t *ff)
{
    unsigned int gen;
    int slot = (unsigned char)ff->reserved[2];

    memcpy(&gen, &ff->reserved[3], sizeof gen);
    if (ff->reserved[0] != 'P' || ff->reserved[1] != 'F' || slot >= FIND_SLOTS ||
        gen == 0 || searches[slot].gen != gen) {
        errno = ENOENT;
        return DOSERR_NO_MORE_FILES;
    }
    if (++searches[slot].pos >= searches[slot].n) {
        find_release(slot);
        memset(ff->reserved, 0, sizeof ff->reserved);
        errno = ENOENT;
        return DOSERR_NO_MORE_FILES;
    }
    find_fill(ff, &searches[slot].e[searches[slot].pos]);
    return 0;
}

/* int 21h 4Eh/4Fh for XnGine (port/host/vpc.c): the DTA has struct find_t's layout (21
   reserved bytes, attribute, time, date, size, name), so a find fills it directly; a failed
   find leaves the name empty */
void port_dos_find(int first, const char *pattern, unsigned int attr, unsigned char *dta)
{
    struct find_t *ff = (struct find_t *)dta;
    unsigned err = first ? func_000A13DA(pattern, attr, ff) : func_000A13F7(ff);

    if (err != 0)
        ff->name[0] = '\0';
}
