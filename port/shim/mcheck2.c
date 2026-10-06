/* mcheck2.c: the rest of StratosWare MemCheck 3.5 that the game calls (memcheck.c has the
   checked replacements config/names.csv already names). Identified by MemCheck's API entry
   (0xA34B8, called with the index of the function's name in MemCheck's own name table at
   0x188F01: 2 mc_check_buffers, 3 mc_check, 10 mc_debug, 11 mc_set_location, 12
   mc_stack_trace, 19 memset, 22 strcat, 25 strncpy, 53 _fmemcpy ...) and by the calls.

   MemCheck is linked in but never started: nothing calls mc_startcheck, and its "active"
   flags (0x188ACB, 0x188AC2, 0x188A44) stay 0. So the checks report nothing and the debug
   output goes nowhere; the native build only does each call's effect. The calls that take a
   location take the caller's __FILE__ and __LINE__, and a destination size for the check. */
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "port_host.h"

/* mc_set_location, MemCheck's public API entry (0x9DA1C): it brackets the internal location
   setter (0xA0ED9, the game's mc_set_location) with API entry index 11, "mc_set_location".
   The game calls it before printf. */
void func_0009DA1C(int line, const char *file)
{
    (void)line;
    (void)file;
}

/* mc_set_erf (0x9DBFE): sets the function MemCheck's messages go to (0x189330, by default
   0xADE60, which writes the message out) unless given NULL; returns the previous one. main
   gives it the empty 0x9DBF9 to keep MemCheck quiet. */
typedef void (*mc_erf)(const char *);

static void mc_erf_default(const char *msg)
{
    (void)msg;              /* MemCheck is never started, so it has nothing to say */
}

static mc_erf erf = mc_erf_default;

mc_erf func_0009DBFE(mc_erf f)
{
    mc_erf old = erf;

    if (f != NULL)
        erf = f;
    return old;
}

/* an empty error reporting function (0x9DBF9): what main passes to mc_set_erf */
void func_0009DBF9(void)
{
}

/* mc_debugf (0xA148C): API entry index 10 ("mc_debug"); printf-style, it formats into 200
   bytes and sends the line to the error reporting function, only while MemCheck is active */
void func_000A148C(const char *fmt, ...)
{
    (void)fmt;
}

/* mc_stack_trace (0xA18C3): API entry 12; while active, prints a heading and a stack trace
   and returns the depth; inactive, 0 */
int func_000A18C3(const char *heading)
{
    (void)heading;
    return 0;
}

/* mc_check (0xA29BA): API entry 3; checks one buffer while active; 0 means no error */
int func_000A29BA(const void *p)
{
    (void)p;
    return 0;
}

/* mc_check_buffers (0xA2A2B): API entry 2; checks every buffer while active; 0 no error */
int func_000A2A2B(void)
{
    return 0;
}

/* mc_strcat (0xA1054): strcat (0xA9554, API entry 22) after the location; size is the
   destination's */
char *func_000A1054(char *dst, const char *src, const char *file, int line, int size)
{
    (void)file;
    (void)line;
    (void)size;
    port_copy_forward(dst + strlen(dst), src, (unsigned int)strlen(src) + 1);
    return dst;
}

/* mc_strncpy (0xA14E8): Watcom's strncpy (0xB37B7, via 0xA988B, API entry 25) after the
   location; size is the destination's */
char *func_000A14E8(char *dst, const char *src, unsigned int n, const char *file, int line,
                    int size)
{
    (void)file;
    (void)line;
    (void)size;
    {
        unsigned int len = (unsigned int)strnlen(src, n);
        port_copy_forward(dst, src, len);
        if (len < n)
            memset(dst + len, 0, n - len);
    }
    return dst;
}

/* memset (0xA1944): MemCheck's interception of memset itself, without a location (API entry
   19, then Watcom's memset 0xAB6C0). sosMIDIInitSong and sosDIGIInitDriver use it too. */
void *func_000A1944(void *dst, int c, unsigned int n)
{
    return memset(dst, c, n);
}

/* mc__fmemcpy (0xA2EC5): _fmemcpy (0xA6CBC, API entry 53) after the location; far pointers
   are plain pointers here (port/include/i86.h) */
void *func_000A2EC5(void *dst, const void *src, unsigned int n, const char *file, int line,
                    int kind)
{
    (void)file;
    (void)line;
    (void)kind;
    return port_copy_forward(dst, src, n);
}
