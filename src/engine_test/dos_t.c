/* dos_t.c: test shims of src/engine/dos.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md): each asm entry's registers (config/xngine_abi.csv) to the C call and
   back. */
#include "xdos.h"

/* name EAX, buf EDX -> EAX */
void xn_dos_load_palette_r(xn_regs *r)
{
    r->eax = (u32)xn_dos_load_palette((const char *)r->eax, (u8 *)r->edx);
}

void xn_dos_open_c_r(xn_regs *r)
{
    r->eax = xn_dos_open_c((const char *)r->eax);
}

/* path EDX -> handle EBX, CF clear (a failure does not return). The asm also left the
   resolved path in EDX and took its caller's EAX for the fatal error's report: neither is
   read (config/xngine_dropped.csv; Q-DOS-01) */
void xn_dos_open_r(xn_regs *r)
{
    r->ebx = xn_dos_open((const char *)r->edx);
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_dos_close_c_r(xn_regs *r)
{
    xn_dos_close_c(r->eax);
}

/* handle EBX; every register kept */
void xn_dos_close_r(xn_regs *r)
{
    xn_dos_close(r->ebx);
}

void xn_dos_find_first_c_r(xn_regs *r)
{
    r->eax = xn_dos_find_first_c((const char *)r->eax);
}

/* spec EDX -> CF when nothing matched */
void xn_dos_find_first_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_dos_find_first((const char *)r->edx));
}

void xn_dos_find_next_c_r(xn_regs *r)
{
    r->eax = xn_dos_find_next_c();
}

void xn_dos_find_next_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_dos_find_next());
}

void xn_dos_file_exists_c_r(xn_regs *r)
{
    r->eax = xn_dos_file_exists_c((const char *)r->eax);
}

/* path EDX -> CF when it does not open (the closed handle the asm left in EBX is dropped) */
void xn_dos_file_exists_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_dos_file_exists((const char *)r->edx));
}

void xn_dos_create_c_r(xn_regs *r)
{
    r->eax = xn_dos_create_c((const char *)r->eax);
}

/* path EDX -> handle EBX */
void xn_dos_create_r(xn_regs *r)
{
    r->ebx = xn_dos_create((const char *)r->edx);
}

/* buf EAX, count EDX, handle EBX -> EAX */
void xn_dos_write_c_r(xn_regs *r)
{
    r->eax = xn_dos_write_c((const void *)r->eax, r->edx, r->ebx);
}

/* count ECX, buf EDX, handle EBX -> CF (the buffer the asm left advanced in EDX is dropped) */
void xn_dos_write_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_dos_write(r->ebx, (const void *)r->edx, r->ecx));
}

/* buf EAX, count EDX, handle EBX -> EAX */
void xn_dos_read_c_r(xn_regs *r)
{
    r->eax = xn_dos_read_c((void *)r->eax, r->edx, r->ebx);
}

/* count ECX, buf EDX, handle EBX -> EAX */
void xn_dos_read_r(xn_regs *r)
{
    r->eax = xn_dos_read(r->ebx, (void *)r->edx, r->ecx);
}

/* mode AL, offset EDX, handle EBX -> EAX */
void xn_dos_seek_c_r(xn_regs *r)
{
    r->eax = xn_dos_seek_c((u8)r->eax, r->edx, r->ebx);
}

/* mode AL, offset CX:DX, handle EBX -> DX:AX, CF on failure (then AX DOS's error, the rest
   of EAX and EDX as they were) */
void xn_dos_seek_r(xn_regs *r)
{
    u32 pos;
    u32 err = xn_dos_seek(r->ebx, r->ecx << 16 | (r->edx & 0xFFFF), (u8)r->eax, &pos);

    if (err != 0) {
        r->eax = (r->eax & 0xFFFF0000) | err;
    } else {
        r->eax = pos;
        r->edx = pos >> 16;
    }
    XN_SETFLAG(r, XN_CF, err != 0);
}

void xn_dos_file_size_c_r(xn_regs *r)
{
    r->eax = xn_dos_file_size_c((const char *)r->eax);
}

/* path EDX -> EAX */
void xn_dos_file_size_r(xn_regs *r)
{
    r->eax = xn_dos_file_size((const char *)r->edx);
}

/* name EAX, buf EDX -> EAX */
void xn_dos_load_file_r(xn_regs *r)
{
    r->eax = (u32)xn_dos_load_file((const char *)r->eax, (void *)r->edx);
}

/* name EDX -> EDX, CF clear (the asm's last flags: xor al, al) */
void xn_dos_make_path_r(xn_regs *r)
{
    r->edx = (u32)xn_dos_make_path((const char *)r->edx);
    XN_SETFLAG(r, XN_CF, 0);
}
