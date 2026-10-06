/* str_t.c: test shims of src/engine/str.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xstr.h"

void xn_str_copy_dword_r(xn_regs *r)
{
    xn_str_copy_dword((u32 *)r->eax, (const u32 *)r->edx);
}

void xn_str_copy_line_r(xn_regs *r)
{
    r->eax = (u32)xn_str_copy_line((char *)r->eax, (const char *)r->edx);
}

void xn_str_copy_word_r(xn_regs *r)
{
    r->eax = xn_str_copy_word((char *)r->eax, (const char *)r->edx);
}

void xn_str_copy_alnum_r(xn_regs *r)
{
    r->eax = xn_str_copy_alnum((char *)r->eax, (const char *)r->edx);
}

/* s EAX, ch DL, n EBX -> EAX */
void xn_str_find_char_n_r(xn_regs *r)
{
    r->eax = (u32)xn_str_find_char_n((const char *)r->eax, (char)r->edx, r->ebx);
}

void xn_str_find_u32_r(xn_regs *r)
{
    r->eax = (u32)xn_str_find_u32((u32 *)r->eax, r->edx, r->ebx);
}

void xn_str_find_u16_r(xn_regs *r)
{
    r->eax = (u32)xn_str_find_u16((u16 *)r->eax, (u16)r->edx, r->ebx);
}

void xn_str_copy_until_r(xn_regs *r)
{
    xn_str_copy_until((char *)r->eax, (const char *)r->edx, (char)r->ebx);
}

/* dst EAX, value DX, bytes BX */
void xn_str_fill_u16_r(xn_regs *r)
{
    xn_str_fill_u16((u16 *)r->eax, (u16)r->edx, (u16)r->ebx);
}

void xn_str_fill_ascending_v2_r(xn_regs *r)
{
    xn_str_fill_ascending_v2((u8 *)r->eax, (u8)r->edx, r->ebx);
}

void xn_str_append_char_r(xn_regs *r)
{
    xn_str_append_char((char *)r->eax, (char)r->edx);
}

void xn_str_fill_ascending_r(xn_regs *r)
{
    xn_str_fill_ascending((u8 *)r->eax, (u8)r->edx, r->ebx);
}

void xn_str_count_nonzero_r(xn_regs *r)
{
    r->eax = xn_str_count_nonzero((const u8 *)r->eax, r->edx);
}

void xn_str_skip_fields_r(xn_regs *r)
{
    r->eax = (u32)xn_str_skip_fields((const char *)r->eax, (char)r->edx, r->ebx);
}

void xn_str_find_byte_pair_r(xn_regs *r)
{
    r->eax = (u32)xn_str_find_byte_pair((u8 *)r->eax, (u16)r->edx, r->ebx);
}

void xn_str_find_nonzero_r(xn_regs *r)
{
    r->eax = (u32)xn_str_find_nonzero((u8 *)r->eax, r->edx);
}

/* c AL -> AL */
void xn_str_char_lower_r(xn_regs *r)
{
    r->eax = (r->eax & ~0xFFu) | xn_str_char_lower((u8)r->eax);
}

void xn_str_char_upper_r(xn_regs *r)
{
    r->eax = (r->eax & ~0xFFu) | xn_str_char_upper((u8)r->eax);
}

void xn_str_copy_r(xn_regs *r)
{
    xn_str_copy((const char *)r->eax, (char *)r->edx);
}

void xn_str_nop_r(xn_regs *r)
{
    xn_str_nop();
}

/* c AL, s ESI, at EDI */
void xn_str_insert_char_r(xn_regs *r)
{
    xn_str_insert_char((char *)r->esi, r->edi, (char)r->eax);
}

/* at EAX, s EDX */
void xn_str_delete_char_r(xn_regs *r)
{
    xn_str_delete_char((char *)r->edx, r->eax);
}

void xn_str_length_r(xn_regs *r)
{
    r->eax = xn_str_length((const char *)r->eax);
}

/* s EAX, ch DL -> EAX */
void xn_str_find_char_r(xn_regs *r)
{
    r->eax = xn_str_find_char((const char *)r->eax, (char)r->edx);
}

/* value EAX, dst EDX, ndigits EBX */
void xn_str_from_int_r(xn_regs *r)
{
    xn_str_from_int(r->eax, (char *)r->edx, r->ebx);
}

void xn_str_to_int_r(xn_regs *r)
{
    r->eax = xn_str_to_int((const char *)r->eax);
}

/* src ESI, dst EDI */
void xn_str_copy_word_max80_r(xn_regs *r)
{
    xn_str_copy_word_max80((char *)r->edi, (const char *)r->esi);
}
