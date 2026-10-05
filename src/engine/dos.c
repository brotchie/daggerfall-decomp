/* dos.c: XnGine's DOS file I/O as readable C (xdos.h; see xngine.h). */
#include "xdos.h"

/* xn_gfx_restore_mode (group 5), through its asm entry: it keeps every register */
void asm_xn_gfx_restore_mode(void);
#pragma aux asm_xn_gfx_restore_mode parm [] modify exact [];

/* int 21h with AH = fn and the rest of the registers as r has them (the caller's, for a
   wrapper whose asm sets only AH): DOS's results back in r */
static void dos_ah(xn_regs *r, u8 fn)
{
    r->eax = (r->eax & 0xFFFF00FF) | (u32)fn << 8;
    xn_int21(r);
}

/* The 'DOS:' fatal error (C0E8C), shared by open, create and load_file: check becomes the
   game's failed internal check, which fatal_error reports instead of msg when it is not 0
   (the asm stores its EAX there: for a missing file, its caller's EAX). fatal_error exits;
   when the game is already exiting it returns, and then: the video mode back, msg and
   xn_dos_path (with CR LF) printed, and exit. */
static void dos_fatal(u32 check, const char *msg)
{
    xn_regs d;
    char *end;
    s32 n;

    internal_check_failed = check;
    d.eax = fatal_error(msg);
    asm_xn_gfx_restore_mode();
    d.edx = (u32)msg;
    dos_ah(&d, 0x09);                   /* print msg ('$'-terminated) */
    end = xn_dos_path;
    for (n = 0x4C; n != 0 && *end != 0; n--)
        end++;
    *(u32 *)end = 0x00240A0D;           /* CR LF '$' */
    d.edx = (u32)xn_dos_path;
    xn_int21(&d);                       /* AH = 9 still */
    exit(d.eax);
    d.eax = 0x4C01;                     /* (exit does not return) */
    xn_int21(&d);
}

u8 *xn_dos_load_palette(const char *name, u8 *buf)
{
    const char *path = xn_dos_make_path(name);
    u32 size = xn_dos_file_size(path, (u32)name);
    u32 eax = size;                     /* the asm's EAX from here on (see xdos.h) */
    xn_regs d;
    s32 h, k;

    if (buf == 0) {
        buf = func_000A10A8(0x300);
        eax = (u32)buf;
    }
    h = xn_dos_open(&path, eax);
    if (size == 0x300) {
        xn_dos_read(buf, 0x300, h);
        xn_dos_close(h);
        return buf;
    }
    d.eax = eax & 0xFFFF0000;           /* AL = 0: from the start */
    d.ebx = h;
    d.ecx = 0;
    d.edx = 8;
    dos_ah(&d, 0x42);
    xn_dos_read(buf, 0x300, h);
    xn_dos_close(h);
    for (k = 0; k < 0x300; k++)
        buf[k] >>= 2;
    return buf;
}

s32 xn_dos_open_c(const char *path)
{
    return xn_dos_open(&path, (u32)path);
}

s32 xn_dos_open(const char **path, u32 check)
{
    xn_regs d;

    if (file_resolver != 0)
        *path = file_resolver((char *)*path);
    d.eax = 0x3D00;
    d.edx = (u32)*path;
    xn_int21(&d);
    if (d.eflags & XN_CF)
        dos_fatal(check, xn_dos_msg_not_found);
    return (u16)d.eax;
}

void xn_dos_open_r(xn_regs *r)
{
    const char *path = (const char *)r->edx;

    r->ebx = xn_dos_open(&path, r->eax);
    r->edx = (u32)path;
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_dos_close_c(s32 handle)
{
    xn_dos_close(handle);
}

void xn_dos_close(s32 handle)
{
    xn_regs d;

    d.eax = 0x3E00;
    d.ebx = handle;
    xn_int21(&d);
}

s32 xn_dos_find_first_c(const char *spec)
{
    xn_regs d;

    d.eax = (u32)spec;
    d.edx = (u32)spec;
    xn_dos_find_first_r(&d);
    return (d.eflags & XN_CF) != 0;
}

void xn_dos_find_first_r(xn_regs *r)
{
    r->ecx = 0;                         /* any attributes */
    dos_ah(r, 0x4E);
}

s32 xn_dos_find_next_c(u32 eax)
{
    xn_regs d;

    d.eax = eax;
    d.edx = eax;
    xn_dos_find_next_r(&d);
    return (d.eflags & XN_CF) != 0;
}

void xn_dos_find_next_r(xn_regs *r)
{
    dos_ah(r, 0x4F);
}

s32 xn_dos_file_exists_c(const char *path)
{
    s32 handle;

    return !xn_dos_file_exists(path, &handle);
}

s32 xn_dos_file_exists(const char *path, s32 *handle)
{
    xn_regs d;

    d.eax = 0x3D00;
    d.edx = (u32)path;
    xn_int21(&d);
    if (d.eflags & XN_CF)
        return 0;
    *handle = d.ebx = d.eax;
    d.eax = 0x3E00;
    xn_int21(&d);
    return 1;
}

void xn_dos_file_exists_r(xn_regs *r)
{
    s32 handle;
    s32 found = xn_dos_file_exists((const char *)r->edx, &handle);

    if (found)
        r->ebx = handle;
    XN_SETFLAG(r, XN_CF, !found);
}

s32 xn_dos_create_c(const char *path)
{
    return xn_dos_create(path, (u32)path);
}

s32 xn_dos_create(const char *path, u32 eax)
{
    xn_regs d;

    d.eax = (eax & 0xFFFF0000) | 0x3C00;
    d.ecx = 0;                          /* attributes */
    d.edx = (u32)path;
    xn_int21(&d);
    if (d.eflags & XN_CF)
        dos_fatal(eax, xn_dos_msg_create_failed);
    return (u16)d.eax;
}

void xn_dos_create_r(xn_regs *r)
{
    r->ebx = xn_dos_create((const char *)r->edx, r->eax);
}

s32 xn_dos_write_c(const u8 *buf, u32 count, s32 handle)
{
    return xn_dos_write(&buf, count, handle);
}

s32 xn_dos_write(const u8 **buf, s32 count, s32 handle)
{
    xn_regs d;

    while (count > 0x8000) {
        d.eax = 0x4000;
        d.ebx = handle;
        d.ecx = 0x8000;
        d.edx = (u32)*buf;
        xn_int21(&d);
        count -= 0x8000;
        *buf += 0x8000;
    }
    d.eax = 0x4000;
    d.ebx = handle;
    d.ecx = count;
    d.edx = (u32)*buf;
    xn_int21(&d);
    return (d.eflags & XN_CF) != 0;
}

void xn_dos_write_r(xn_regs *r)
{
    const u8 *buf = (const u8 *)r->edx;

    XN_SETFLAG(r, XN_CF, xn_dos_write(&buf, r->ecx, r->ebx));
    r->edx = (u32)buf;
}

s32 xn_dos_read_c(void *buf, u32 count, s32 handle)
{
    return xn_dos_read(buf, count, handle);
}

s32 xn_dos_read(void *buf, u32 count, s32 handle)
{
    xn_regs d;

    d.eax = 0x3F00;
    d.ebx = handle;
    d.ecx = count;
    d.edx = (u32)buf;
    xn_int21(&d);
    return d.eax;
}

s32 xn_dos_seek_c(u32 eax, u32 offset, s32 handle)
{
    xn_regs d;

    d.eax = eax;
    d.ebx = handle;
    d.ecx = offset >> 16;
    d.edx = offset & 0xFFFF;
    xn_dos_seek_r(&d);
    return (d.eflags & XN_CF) != 0;
}

void xn_dos_seek_r(xn_regs *r)
{
    dos_ah(r, 0x42);
}

u32 xn_dos_file_size_c(const char *path)
{
    return xn_dos_file_size(path, (u32)path);
}

u32 xn_dos_file_size(const char *path, u32 eax)
{
    s32 h = xn_dos_open(&path, eax);
    xn_regs d;

    d.eax = (eax & 0xFFFF0000) | 2;     /* AL = 2: from the end */
    d.ebx = h;
    d.ecx = 0;
    d.edx = 0;
    dos_ah(&d, 0x42);
    xn_dos_close(h);
    return d.edx << 16 | (d.eax & 0xFFFF);  /* DX:AX */
}

void xn_dos_file_size_r(xn_regs *r)
{
    r->eax = xn_dos_file_size((const char *)r->edx, r->eax);
}

u8 *xn_dos_load_file(const char *name, u8 *buf)
{
    const char *path;
    s32 h;
    u8 *p;
    u32 n;

    disk_last_file_size = 0;
    path = xn_dos_make_path(name);
    h = xn_dos_open(&path, (u32)name);
    if (buf == 0) {
        buf = func_000A10A8(filelength(h));
        if (buf == 0)
            dos_fatal(0, xn_dos_msg_out_of_memory);
    }
    p = buf;
    do {
        n = (u16)xn_dos_read(p, 0xFFFF, h);
        disk_last_file_size += n;
        p += n;
    } while (n == 0xFFFF);
    xn_dos_close(h);
    return buf;
}

void xn_dos_load_file_r(xn_regs *r)
{
    r->eax = (u32)xn_dos_load_file((const char *)r->eax, (u8 *)r->edx);
}

char *xn_dos_make_path(const char *name)
{
    const char *s = arena2_path;
    char *p = xn_dos_path;
    u32 n = 80;
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
    do {                                /* the same count: 80 - the path's length */
        c = *s++;
        if (c == 0)
            break;
        *p++ = c;
    } while (--n != 0);
    *p = 0;
    return xn_dos_path;
}

void xn_dos_make_path_r(xn_regs *r)
{
    r->edx = (u32)xn_dos_make_path((const char *)r->edx);
    XN_SETFLAG(r, XN_CF, 0);            /* the asm's last flags: xor al, al */
}
