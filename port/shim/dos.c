/* dos.c: the DOS, DPMI and mouse services the game asks for through int386 and int386x
   (docs/port.md), and the far pointers of <i86.h>.

   The services the game's C requests (src/lifted/int.c, support.c, hand/func_0007EF86.c):
     int 31h 0500h  free memory information (dpmi_get_free_memory, ES:EDI -> 48 bytes)
     int 31h 0600h  lock a linear region (dpmi_lock_region)        -> success
     int 31h 0601h  unlock a linear region (dpmi_unlock_region)    -> success
     int 31h FF30h  CauseWay: set the error dump (causeway_disable_error_dump) -> success
     int 31h 0101h  free DOS memory (support.c, behind a flag that stays 0)
   and 0100h (allocate DOS memory), 0006h (segment base) to go with them. Anything else logs
   its interrupt and function and fails (carry set), as before; that includes 0300h,
   simulate a real-mode interrupt (hand/func_0007EF86.c, behind the same flag as 0101h):
   there is no real mode here.

   Selectors (see <i86.h>): selector 0 and FLAT_SEL are the program's own 4 GB window; each
   other window a pointer lives in gets the next selector; a DOS memory block gets one based
   at the block. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "i86.h"

#define FLAT_SEL 0x0008     /* DS, ES and SS of the flat model */
#define MAX_SEL 64

static uintptr_t sel_base[MAX_SEL];     /* by selector >> 3 */
static int sel_used[MAX_SEL];

static uintptr_t flat_base(void)
{
    static const int anchor = 0;
    return (uintptr_t)&anchor & ~(uintptr_t)0xFFFFFFFFu;
}

uintptr_t port_sel_base(unsigned short sel);
unsigned short port_sel_new(uintptr_t base);

static uintptr_t base_of(unsigned short sel)
{
    unsigned i = sel >> 3;

    if (i == 0 || sel == FLAT_SEL || i >= MAX_SEL || !sel_used[i])
        return flat_base();
    return sel_base[i];
}

static unsigned short new_sel(uintptr_t base)
{
    unsigned i;

    for (i = 2; i < MAX_SEL; i++) {
        if (!sel_used[i]) {
            sel_used[i] = 1;
            sel_base[i] = base;
            return (unsigned short)(i << 3);
        }
    }
    fprintf(stderr, "port: out of selectors\n");
    return 0;
}

unsigned short port_fp_seg(const volatile void *p)
{
    uintptr_t hi = (uintptr_t)p & ~(uintptr_t)0xFFFFFFFFu;
    unsigned i;

    /* a near pointer (a 32-bit value, zero- or sign-extended) is in the flat window */
    if (hi == 0 || hi == ~(uintptr_t)0xFFFFFFFFu || hi == flat_base())
        return FLAT_SEL;
    for (i = 2; i < MAX_SEL; i++)
        if (sel_used[i] && sel_base[i] == hi)
            return (unsigned short)(i << 3);
    return new_sel(hi);
}

void *port_mk_fp(unsigned short sel, unsigned int off)
{
    return (void *)(base_of(sel) + off);
}

/* for the engine's DPMI calls (port/host/vpc.c): a selector's base, a new selector */
uintptr_t port_sel_base(unsigned short sel)
{
    return base_of(sel);
}

unsigned short port_sel_new(uintptr_t base)
{
    return new_sel(base);
}

/* int.c's own FP_SEG (it undefines <i86.h>'s) */
unsigned short _FP_SEG(const volatile void *p)
{
    return port_fp_seg(p);
}

void port_segread(struct SREGS *sr)
{
    sr->cs = FLAT_SEL;
    sr->ds = sr->es = sr->ss = sr->fs = sr->gs = FLAT_SEL;
}

/* DPMI 0100h's DOS memory: host blocks, with made-up real-mode segments */
#define MAX_DOSMEM 16
static struct {
    void *p;
    unsigned short seg, sel;
} dosmem[MAX_DOSMEM];
static unsigned short dos_next_seg = 0x2000;

/* 0500h: what tools/fallemu.py's machine reports (64 MB, 16384 pages, 13376 free:
   dpmi_memory_stats then shows 12032 KB used of 65536 KB) */
#define MEM_PAGES 16384u
#define FREE_PAGES 13376u

static void dpmi_free_memory(void *info)
{
    unsigned int v[12];
    int i;

    v[0] = FREE_PAGES << 12;        /* largest free block, bytes */
    v[1] = FREE_PAGES;              /* maximum unlocked page allocation */
    v[2] = FREE_PAGES;              /* maximum locked page allocation */
    v[3] = MEM_PAGES;               /* linear address space, pages */
    v[4] = FREE_PAGES;              /* unlocked pages */
    v[5] = FREE_PAGES;              /* free pages */
    v[6] = MEM_PAGES;               /* physical pages */
    v[7] = FREE_PAGES;              /* free linear address space, pages */
    for (i = 8; i < 12; i++)
        v[i] = 0;                   /* paging file; reserved */
    memcpy(info, v, sizeof v);
}

static void int31(union REGS *r, struct SREGS *sr)
{
    unsigned short es = sr ? sr->es : FLAT_SEL;
    int i;

    r->x.cflag = 0;
    switch (r->w.ax) {
    case 0x0006: {                  /* segment base address: BX -> CX:DX */
        uintptr_t b = base_of(r->w.bx);
        r->w.cx = (unsigned short)(b >> 16);
        r->w.dx = (unsigned short)b;
        break;
    }
    case 0x0100: {                  /* allocate DOS memory: BX paragraphs -> AX seg, DX sel */
        unsigned int n = (unsigned int)r->w.bx << 4;
        for (i = 0; i < MAX_DOSMEM && dosmem[i].p; i++)
            ;
        if (i == MAX_DOSMEM || dos_next_seg + r->w.bx > 0x9F00) {
            r->w.ax = 8;            /* insufficient memory */
            r->w.bx = 0;
            r->x.cflag = 1;
            break;
        }
        dosmem[i].p = calloc(1, n ? n : 16);
        dosmem[i].seg = dos_next_seg;
        dosmem[i].sel = new_sel((uintptr_t)dosmem[i].p);
        dos_next_seg += r->w.bx;
        r->w.ax = dosmem[i].seg;
        r->w.dx = dosmem[i].sel;
        break;
    }
    case 0x0101:                    /* free DOS memory: DX selector */
        for (i = 0; i < MAX_DOSMEM; i++) {
            if (dosmem[i].p && dosmem[i].sel == r->w.dx) {
                free(dosmem[i].p);
                sel_used[dosmem[i].sel >> 3] = 0;
                dosmem[i].p = NULL;
                return;
            }
        }
        r->w.ax = 9;                /* invalid memory block */
        r->x.cflag = 1;
        break;
    case 0x0500:                    /* free memory information: ES:EDI */
        dpmi_free_memory(port_mk_fp(es, r->x.edi));
        break;
    case 0x0600:                    /* lock / unlock a region: nothing pages out here */
    case 0x0601:
    case 0x0602:
    case 0x0603:
        break;
    case 0xFF30:                    /* CauseWay: error dump settings */
        break;
    default:
        fprintf(stderr, "port: int 31h ax=%04X not implemented\n", r->w.ax);
        r->x.cflag = 1;
        break;
    }
}

static int service(int intno, union REGS *in, union REGS *out, struct SREGS *sr)
{
    if (out != in)
        *out = *in;
    if (intno == 0x31) {
        int31(out, sr);
    } else {
        fprintf(stderr, "port: int %02Xh ax=%04X not implemented\n", intno, in->w.ax);
        out->x.cflag = 1;
    }
    return (int)out->x.eax;
}

int port_int386(int intno, union REGS *in, union REGS *out)
{
    return service(intno, in, out, NULL);
}

int port_int386x(int intno, union REGS *in, union REGS *out, struct SREGS *sr)
{
    return service(intno, in, out, sr);
}
