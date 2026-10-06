/* libc.c: the Watcom C library functions the game calls whose host versions differ or are
   missing (docs/port.md). The game's calls reach them through port/include/port.h's macros
   (port_rand ...) or, for names the host lacks (itoa, stricmp ...), directly. */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "port_host.h"

/* Watcom 10.0a's rand (FALL.EXE 0x9DC25, byte-identical to CLIB3R): the game's random
   sequence, so a native run can follow an emulated one */
static unsigned int rand_next = 1;

int port_rand(void)
{
    rand_next = rand_next * 1103515245u + 12345u;
    return (int)((rand_next >> 16) & 0x7FFF);
}

void port_srand(unsigned int seed)
{
    rand_next = seed;
}

/* Watcom's utoa and itoa: digits in lower case; itoa writes a sign only in base 10 */
char *utoa(unsigned int value, char *buf, int radix)
{
    char tmp[33];
    int n = 0;
    char *p = buf;

    do {
        unsigned int d = value % (unsigned int)radix;
        tmp[n++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
        value /= (unsigned int)radix;
    } while (value != 0);
    while (n > 0)
        *p++ = tmp[--n];
    *p = '\0';
    return buf;
}

char *itoa(int value, char *buf, int radix)
{
    if (radix == 10 && value < 0) {
        buf[0] = '-';
        utoa(0u - (unsigned int)value, buf + 1, radix);
        return buf;
    }
    return utoa((unsigned int)value, buf, radix);
}

/* Watcom's stricmp and strnicmp compare lower-cased bytes */
int stricmp(const char *a, const char *b)
{
    int ca, cb;

    do {
        ca = tolower((unsigned char)*a++);
        cb = tolower((unsigned char)*b++);
    } while (ca == cb && ca != 0);
    return ca - cb;
}

int strnicmp(const char *a, const char *b, unsigned int n)
{
    int ca = 0, cb = 0;

    while (n-- > 0) {
        ca = tolower((unsigned char)*a++);
        cb = tolower((unsigned char)*b++);
        if (ca != cb || ca == 0)
            break;
    }
    return ca - cb;
}

void port_exit(int status)
{
    host_shutdown();
    exit(status);
}
