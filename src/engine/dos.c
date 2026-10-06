/* dos.c: XnGine's DOS files (canonical C; the interface and the module's documentation are in
   xdos.h).

   Every DOS call goes through dos_call (int 21h with a register file: glue.asm's xn_int21,
   the platform boundary). The function number goes in AX; the registers DOS does not read
   are left 0. */
#include "xdos.h"
#include "xgfx.h"

/* int 21h function ax with ebx, ecx and edx; DOS's registers back in *r. Returns 1 when DOS
   reports a failure (its carry). */
static int dos_call(xn_regs *r, u16 ax, u32 ebx, u32 ecx, u32 edx)
{
    r->eax = ax;
    r->ebx = ebx;
    r->ecx = ecx;
    r->edx = edx;
    r->esi = r->edi = r->ebp = 0;
    xn_int21(r);
    return (r->eflags & XN_CF) != 0;
}

/* The 'DOS:' fatal error, shared by open, create and load_file: check becomes the game's
   failed internal check (Quirk Q-DOS-01: fatal_error then reports 'Failed internal check N.'
   instead of msg), and fatal_error ends the game. It returns only while the game is already
   exiting; then the video mode goes back, msg and xn_dos_path (with CR LF) are printed, and
   the program ends. */
static void dos_fatal(u32 check, const char *msg)
{
    char *end;
    s32 n;

    internal_check_failed = check;
    fatal_error(msg);
    xn_gfx_restore_mode();
    xn_dos_print(msg);
    end = xn_dos_path;                  /* the path's end, at most 76 characters in */
    for (n = 0x4C; n != 0 && *end != 0; n--)
        end++;
    end[0] = '\r';
    end[1] = '\n';
    end[2] = '$';
    end[3] = 0;
    xn_dos_print(xn_dos_path);
    exit(1);
    xn_dos_exit(1);                     /* (exit does not return) */
}

/* ---- files ------------------------------------------------------------------------------ */

s32 xn_dos_open(const char *path)
{
    xn_regs r;

    if (file_resolver != 0)
        path = file_resolver((char *)path);
    if (dos_call(&r, 0x3D00, 0, 0, (u32)path))
        /* Quirk Q-DOS-01: the asm reported its caller's EAX here (a leftover); the path's
           address stands for it */
        dos_fatal((u32)path, xn_dos_msg_not_found);
    return (u16)r.eax;
}

u32 xn_dos_open_mode(const char *path, u8 mode, s32 *handle)
{
    xn_regs r;
    int failed = dos_call(&r, 0x3D00 | mode, 0, 0, (u32)path);

    *handle = (u16)r.eax;
    return failed ? (u16)r.eax : 0;
}

s32 xn_dos_create(const char *path)
{
    xn_regs r;

    if (dos_call(&r, 0x3C00, 0, 0, (u32)path))
        dos_fatal((u32)path, xn_dos_msg_create_failed);     /* Quirk Q-DOS-01 */
    return (u16)r.eax;
}

void xn_dos_close(s32 handle)
{
    xn_regs r;

    dos_call(&r, 0x3E00, handle, 0, 0);
}

u32 xn_dos_read(s32 handle, void *buf, u32 count)
{
    xn_regs r;

    dos_call(&r, 0x3F00, handle, count, (u32)buf);
    return r.eax;                       /* Quirk Q-DOS-02: an error code after a failure */
}

int xn_dos_write(s32 handle, const void *buf, s32 count)
{
    const u8 *p = (const u8 *)buf;
    xn_regs r;

    while (count > 0x8000) {            /* Quirk Q-DOS-03: these pieces' failures go unseen */
        dos_call(&r, 0x4000, handle, 0x8000, (u32)p);
        count -= 0x8000;
        p += 0x8000;
    }
    return dos_call(&r, 0x4000, handle, count, (u32)p);
}

u32 xn_dos_seek(s32 handle, s32 offset, u8 mode, u32 *pos)
{
    xn_regs r;

    if (dos_call(&r, 0x4200 | mode, handle, (u32)offset >> 16, offset & 0xFFFF))
        return (u16)r.eax;
    if (pos != 0)
        *pos = r.edx << 16 | (r.eax & 0xFFFF);  /* DX:AX */
    return 0;
}

int xn_dos_find_first(const char *spec)
{
    xn_regs r;

    return !dos_call(&r, 0x4E00, 0, 0, (u32)spec);
}

int xn_dos_find_next(void)
{
    xn_regs r;

    return !dos_call(&r, 0x4F00, 0, 0, 0);
}

int xn_dos_file_exists(const char *path)
{
    s32 h;

    if (xn_dos_open_mode(path, 0, &h) != 0)
        return 0;
    xn_dos_close(h);
    return 1;
}

u32 xn_dos_file_size(const char *path)
{
    s32 h = xn_dos_open(path);
    u32 size = 0;

    xn_dos_seek(h, 0, XN_DOS_SEEK_END, &size);  /* (a handle just opened: it cannot fail) */
    xn_dos_close(h);
    return size;
}

void *xn_dos_load_file(const char *name, void *buf)
{
    s32 h;
    u8 *p;
    u32 n;

    disk_last_file_size = 0;
    h = xn_dos_open(xn_dos_make_path(name));
    if (buf == 0) {
        buf = func_000A10A8(filelength(h));
        if (buf == 0)
            dos_fatal(0, xn_dos_msg_out_of_memory);
    }
    p = (u8 *)buf;
    do {
        /* Quirk Q-DOS-05: AX as the count, an error code too */
        n = (u16)xn_dos_read(h, p, 0xFFFF);
        disk_last_file_size += n;
        p += n;
    } while (n == 0xFFFF);
    xn_dos_close(h);
    return buf;
}

char *xn_dos_make_path(const char *name)
{
    const char *s = arena2_path;
    char *p = xn_dos_path;
    u32 n = 80;                         /* Quirk Q-DOS-04: one count for the path and name */
    char c;

    do {
        c = *s++;
        if (c == 0)
            break;
        *p++ = c;
    } while (--n != 0);
    if (p[-1] != '\\')                  /* (an empty arena2_path: the byte before) */
        *p++ = '\\';
    s = name;
    do {                                /* the count left: 0 after a full path means 2^32 */
        c = *s++;
        if (c == 0)
            break;
        *p++ = c;
    } while (--n != 0);
    *p = 0;
    return xn_dos_path;
}

u8 *xn_dos_load_palette(const char *name, u8 *buf)
{
    const char *path = xn_dos_make_path(name);
    u32 size = xn_dos_file_size(path);
    s32 h, k;

    if (buf == 0)
        buf = func_000A10A8(0x300);
    h = xn_dos_open(path);
    if (size == 0x300) {
        xn_dos_read(h, buf, 0x300);
        xn_dos_close(h);
        return buf;
    }
    xn_dos_seek(h, 8, XN_DOS_SEEK_SET, 0);
    xn_dos_read(h, buf, 0x300);
    xn_dos_close(h);
    for (k = 0; k < 0x300; k++)
        buf[k] >>= 2;
    return buf;
}

/* ---- the C forms ------------------------------------------------------------------------- */

s32 xn_dos_open_c(const char *path)
{
    return xn_dos_open(path);
}

s32 xn_dos_create_c(const char *path)
{
    return xn_dos_create(path);
}

void xn_dos_close_c(s32 handle)
{
    xn_dos_close(handle);
}

u32 xn_dos_read_c(void *buf, u32 count, s32 handle)
{
    return xn_dos_read(handle, buf, count);
}

int xn_dos_write_c(const void *buf, s32 count, s32 handle)
{
    return xn_dos_write(handle, buf, count);
}

int xn_dos_seek_c(u8 mode, s32 offset, s32 handle)
{
    return xn_dos_seek(handle, offset, mode, 0) != 0;
}

int xn_dos_find_first_c(const char *spec)
{
    return !xn_dos_find_first(spec);
}

int xn_dos_find_next_c(void)
{
    return !xn_dos_find_next();
}

int xn_dos_file_exists_c(const char *path)
{
    return !xn_dos_file_exists(path);
}

u32 xn_dos_file_size_c(const char *path)
{
    return xn_dos_file_size(path);
}

/* ---- messages and the end ---------------------------------------------------------------- */

void xn_dos_print(const char *msg)
{
    xn_regs r;

    dos_call(&r, 0x0900, 0, 0, (u32)msg);
}

void xn_dos_exit(u8 code)
{
    xn_regs r;

    dos_call(&r, 0x4C00 | code, 0, 0, 0);
}

u16 xn_dos_psp(void)
{
    xn_regs r;

    dos_call(&r, 0x5100, 0, 0, 0);
    return (u16)r.ebx;
}
