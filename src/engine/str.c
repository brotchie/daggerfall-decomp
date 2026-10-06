/* str.c: XnGine's string and array helpers (canonical C; the interface and the module's
   documentation are in xstr.h). */
#include "xstr.h"
#include "xrand.h"

void xn_str_copy_dword(u32 *dst, const u32 *src)
{
    *dst = *src;
}

char *xn_str_copy_line(char *dst, const char *src)
{
    char c;

    for (;;) {
        c = *src++;
        if (c == 0 || c == '\r')
            break;
        *dst++ = c;
    }
    *dst = 0;
    return (char *)src;
}

s32 xn_str_copy_word(char *dst, const char *src)
{
    s32 n = 0;

    while ((u8)*src > ' ') {
        *dst++ = *src++;
        n++;
    }
    *dst = 0;
    return n;
}

s32 xn_str_copy_alnum(char *dst, const char *src)
{
    s32 n = 0;
    u8 c;

    for (;; src++) {
        c = *src;
        if (c >= '0' && c <= '9')
            ;
        else if (c >= 'A' && c <= 'Z')
            c &= 0xDF;          /* Quirk Q-STR-04: the case mask lands on capitals: no change */
        else if (c < 'a' || c > 'z')
            break;
        *dst++ = c;
        n++;
    }
    *dst = 0;
    return n;
}

const char *xn_str_find_char_n(const char *s, char ch, u32 n)
{
    if (n == 0)
        return 0;
    do {
        if (*s == 0)
            return 0;
        if (*s == ch)
            return s;
        s++;
    } while (--n != 0);
    return 0;
}

u32 *xn_str_find_u32(u32 *array, u32 value, u32 count)
{
    if (count == 0)
        return 0;
    do {
        if (*array == value)
            return array;
        array++;
    } while (--count != 0);
    return 0;
}

u16 *xn_str_find_u16(u16 *array, u16 value, u32 count)
{
    do {                                /* Quirk Q-STR-02: the count is tested after */
        if (*array == value)
            return array;
        array++;
    } while (--count != 0);
    return 0;
}

void xn_str_copy_until(char *dst, const char *src, char delim)
{
    while (*src != delim && *src != 0)
        *dst++ = *src++;
    *dst = 0;
}

void xn_str_fill_u16(u16 *dst, u16 value, u16 bytes)
{
    u32 n;

    for (n = bytes >> 1; n != 0; n--)
        *dst++ = value;
}

void xn_str_fill_ascending_v2(u8 *dst, u8 first, u32 n)
{
    xn_str_fill_ascending(dst, first, n);
}

void xn_str_append_char(char *s, char ch)
{
    while (*s != 0)
        s++;
    *s = ch;                            /* Quirk Q-STR-03: no new terminator */
}

void xn_str_fill_ascending(u8 *dst, u8 first, u32 n)
{
    do {                                /* Quirk Q-STR-02 */
        *dst++ = first++;
    } while (--n != 0);
}

s32 xn_str_count_nonzero(const u8 *p, u32 n)
{
    s32 count = 0;

    do {                                /* Quirk Q-STR-02 */
        if (*p != 0)
            count++;
        p++;
    } while (--n != 0);
    return count;
}

const char *xn_str_skip_fields(const char *s, char delim, s32 n)
{
    for (; n != 0; n--) {
        for (;;) {
            if (*s == 0)                /* before the delimiter: delim 0 finds nothing */
                return 0;
            if (*s == delim)
                break;
            s++;
        }
        s++;
    }
    return s;
}

u8 *xn_str_find_byte_pair(u8 *p, u16 pair, u32 n)
{
    do {                                /* Quirk Q-STR-02 */
        if (*(u16 *)p == pair)
            return p;
        p++;
    } while (--n != 0);
    return 0;
}

u8 *xn_str_find_nonzero(u8 *p, u32 n)
{
    for (; n != 0; n--, p++)
        if (*p != 0)
            return p;
    return 0;
}

u8 xn_str_char_lower(u8 c)
{
    if (c >= 'A' && c <= 'Z')
        c += 0x20;
    return c;
}

u8 xn_str_char_upper(u8 c)
{
    if (c >= 'a' && c <= 'z')
        c -= 0x20;
    return c;
}

void xn_str_copy(const char *src, char *dst)
{
    while ((*dst++ = *src++) != 0)
        ;
}

void xn_str_nop(void)
{
}

void xn_str_insert_char(char *s, s32 at, char c)
{
    char moved;

    do {
        moved = s[at];
        s[at++] = c;
        c = moved;
    } while (c != 0);
    s[at] = 0;
}

void xn_str_delete_char(char *s, s32 at)
{
    char *p = s + at;

    do {
        p[0] = p[1];
    } while (*p++ != 0);
}

s32 xn_str_length(const char *s)
{
    s32 n = 0;

    while (*s++ != 0)
        n++;
    return n;
}

s32 xn_str_find_char(const char *s, char ch)
{
    s32 k = 0;
    char c;

    do {
        c = *s++;
        if (c == ch)
            return k;
        k++;
    } while (c != 0);
    return 0;                           /* Quirk Q-STR-05: as index 0 */
}

void xn_str_from_int(s32 value, char *dst, s32 ndigits)
{
    s32 k = ndigits - 1;
    xn_s64 rest;
    u32 left;

    if (k == 0)
        return;                         /* Quirk Q-STR-01: one digit writes nothing */
    if (value < 0) {
        *dst++ = '-';
        value = -value;
    }
    left = value;
    do {                                /* the place values 10^k .. 1 */
        rest.lo = left;
        rest.hi = 0;
        /* Quirk Q-STR-01: no digits divides by the dword before the table, xn_rand_seed (a
           seed of 0 divides by zero: 0, Q-SYS-01) */
        *dst++ = '0' + (char)xn_u64_divrem_or0(&rest, k >= 0 ? xn_pow10_table[k] : xn_rand_seed,
                                               &left);
    } while (--k >= 0);
}

s32 xn_str_to_int(const char *s)
{
    s32 sign = 1;
    s32 value = 0;
    s32 d;

    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    for (;;) {
        d = (u8)*s - '0';
        if (d < 0 || d > 9)
            break;
        value = value * 10 + d;
        s++;
    }
    return value * sign;
}

void xn_str_copy_word_max80(char *dst, const char *src)
{
    s32 n = 80;
    s8 c;

    do {
        c = *src++;
        if (c <= ' ')                   /* Quirk Q-STR-06: signed */
            break;
        *dst++ = c;
    } while (--n != 0);
    *dst = 0;
}
