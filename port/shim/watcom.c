/* watcom.c: Watcom C32 10.0a runtime functions the game calls by address (docs/state.md, "The
   library region"), done as Watcom's do them. libc.c has the ones port.h renames. */
#include <malloc/malloc.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* pow (MATH387R, 0xA166C -> 0xA1516). FALL.EXE runs the FPU with control word 0x127F
   (__init_80x87, from 0x18D948): 53-bit precision, round to nearest, so each x87 operation
   rounds as a double does. For an integral y with 1 <= |y| < 65536 (and x != 0) Watcom
   multiplies (0xA1649: square-and-multiply, low bit first) and takes the reciprocal for a
   negative y; that is done here operation for operation. Otherwise it computes
   exp(y * ln|x|) with the x87's fyl2x and its own exp; libm's pow stands in for that (it may
   differ in the last bit). The game's one call is pow(1.04, level) (skill_ready_to_advance). */
static double ipow(double b, unsigned int n)
{
    double acc;

    while ((n & 1) == 0) {
        b *= b;
        n >>= 1;
    }
    n >>= 1;
    acc = b;
    while (n != 0) {
        b *= b;
        if (n & 1)
            acc *= b;
        n >>= 1;
    }
    return acc;
}

double func_000A166C(double x, double y)
{
    if (x != 0.0 && y == nearbyint(y) && fabs(y) >= 1.0 && fabs(y) < 65536.0) {
        double r = ipow(x, (unsigned int)fabs(y));
        return y < 0.0 ? 1.0 / r : r;
    }
    return pow(x, y);
}

/* _msize (0xA277F -> MemCheck's _nmsize 0xAB753 -> Watcom's 0xB7273): the usable size of a
   block from malloc. The game only uses it as the length to lock (music_play). */
unsigned int func_000A277F(void *p)
{
    return p ? (unsigned int)malloc_size(p) : 0;
}

/* _nheapwalk (0xA2A76): walks Watcom's near heap one block per call. The native heap is the
   host's and cannot be walked, so the walk is at its end at once: _HEAPEND (4), with the
   entry cleared as Watcom clears it at the end. The game (mem_check_crt_heap, only with
   mem_check_level set) loops until a non-zero result and treats 1 and 4 as a good heap. */
#pragma pack(push, 1)
struct watcom_heapinfo {
    unsigned int pentry_off;        /* void __far *_pentry */
    unsigned short pentry_seg;
    unsigned int size;              /* size_t _size */
    int useflag;                    /* int _useflag */
};
#pragma pack(pop)

int func_000A2A76(struct watcom_heapinfo *h)
{
    h->pentry_off = 0;
    h->pentry_seg = 0;
    h->size = 0;
    h->useflag = 0;
    return 4;                       /* _HEAPEND */
}

/* fscanf (0xA31A6, named fprintf_2 in config/names.csv: the fscanf module, after cget_file
   0xA3129 and vfscanf 0xA317F). The game's one call reads its config file (args.c):
   fscanf(file, "%s %s", key, value) until EOF. Watcom's text mode drops the \r of each line
   and the host's keeps it; "%s" skips both as white space. */
int fprintf_2(FILE *f, const char *fmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vfscanf(f, fmt, ap);
    va_end(ap);
    return n;
}

/* sscanf (0xA16F8, named fprintf in config/names.csv; port.h sends the game's fprintf here):
   kludge.c's sscanf(version, "%d", &build) */
int port_sscanf(const char *s, const char *fmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsscanf(s, fmt, ap);
    va_end(ap);
    return n;
}

/* Watcom's malloc and free (0xA10A8, 0xA117E), which XnGine calls (its game_malloc and
   game_free, xgfx.h) */
void *func_000A10A8(unsigned int size)
{
    return malloc(size ? size : 1);
}

void func_000A117E(void *block)
{
    free(block);
}
