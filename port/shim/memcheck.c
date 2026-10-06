/* memcheck.c: StratosWare MemCheck's checked replacements, as the game calls them (docs/state.md
   "The library region"). Each takes the caller's __FILE__ and __LINE__ after the usual
   arguments; mc_sprintf takes them from mc_set_location just before. The native build has
   no checking to do and passes the calls to the C library. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port_host.h"

void *mc_malloc(unsigned int size, const char *file, int line)
{
    (void)file;
    (void)line;
    return malloc(size ? size : 1);
}

void mc_free(void *p, const char *file, int line)
{
    (void)file;
    (void)line;
    free(p);
}

void *port_copy_forward(void *dst, const void *src, unsigned int n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    /* apart, or the destination first: the host's copy does the same */
    if (d <= s || d >= s + n)
        return memmove(dst, src, n);
    for (; n >= 4; n -= 4, d += 4, s += 4) {
        unsigned char t[4];
        memcpy(t, s, 4);                /* a dword read, then written, as movsd */
        memcpy(d, t, 4);
    }
    while (n--)
        *d++ = *s++;
    return dst;
}

void *mc_memcpy(void *dst, const void *src, unsigned int n, const char *file, int line, int kind)
{
    (void)file;
    (void)line;
    (void)kind;
    return port_copy_forward(dst, src, n);
}

void *mc_memmove(void *dst, const void *src, unsigned int n, const char *file, int line, int kind)
{
    (void)file;
    (void)line;
    (void)kind;
    return memmove(dst, src, n);
}

void *mc_memset(void *dst, int c, unsigned int n, const char *file, int line, int kind)
{
    (void)file;
    (void)line;
    (void)kind;
    return memset(dst, c, n);
}

/* FALL.EXE's 0xA0AD9, named mc_strncpy in config/names.csv, is MemCheck's strcpy: it enters
   MemCheck's API with index 0x17 ("strcpy" in MemCheck's name table at 0x188F01) and copies
   with 0xA8CFD, a strcpy; n is only the destination's size, for the check. The real
   mc_strncpy is 0xA14E8 (mcheck2.c). file_index_scan (hand/func_0006D6A4.c) relies on it:
   it copies a whole directory name with n = 4. */
char *mc_strncpy(char *dst, const char *src, unsigned int n, const char *file, int line)
{
    (void)n;
    (void)file;
    (void)line;
    /* the string's length first, then the bytes forward (the game copies within a buffer) */
    port_copy_forward(dst, src, (unsigned int)strlen(src) + 1);
    return dst;
}

/* the location for the next checked call that cannot take one (mc_sprintf) */
void mc_set_location(int line, const char *file)
{
    (void)line;
    (void)file;
}

int mc_sprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsprintf(buf, fmt, ap);
    va_end(ap);
    return n;
}
