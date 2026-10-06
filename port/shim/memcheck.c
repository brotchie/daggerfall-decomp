/* memcheck.c: StratosWare MemCheck's checked replacements, as the game calls them (docs/state.md
   "The library region"). Each takes the caller's __FILE__ and __LINE__ after the usual
   arguments; mc_sprintf takes them from mc_set_location just before. The native build has
   no checking to do and passes the calls to the C library. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void *mc_memcpy(void *dst, const void *src, unsigned int n, const char *file, int line, int kind)
{
    (void)file;
    (void)line;
    (void)kind;
    return memcpy(dst, src, n);
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
    return strcpy(dst, src);
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
