/* int.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include <i86.h>

extern char D_00177348[];

extern int mc_memset();
#undef _FP_SEG      /* <i86.h>'s is FP_SEG, without the modify list */
extern unsigned short _FP_SEG( const volatile void __far * );
#pragma aux _FP_SEG = parm caller [eax dx] value [dx] modify exact [];

void dpmi_get_free_memory(int info)
{
{
    union REGS regs;
    struct SREGS sregs;

    mc_memset(&sregs, 0, 12, (int)D_00177348, 39, 4);
    regs.x.eax = 1280;
    regs.x.edi = info;
    sregs.es = _FP_SEG((void *)info);
    int386x(49, &regs, &regs, &sregs);
}
}

int dpmi_lock_region(int address, int size)
{
    union REGS regs;

    if (address == 0 || size == 0) return 0;
    mc_memset(&regs, 0, 28, (int)D_00177348, 66, 4);
    regs.w.ax = 1536;
    regs.w.bx = address >> 16;
    regs.w.cx = address;
    regs.w.si = size >> 16;
    regs.w.di = size;
    int386(49, &regs, &regs);
    return regs.x.cflag & 1;
}

int dpmi_unlock_region(int address, int size)
{
    union REGS regs;

    if (address == 0 || size == 0) return 0;
    mc_memset(&regs, 0, 28, (int)D_00177348, 86, 4);
    regs.w.ax = 1537;
    regs.w.bx = address >> 16;
    regs.w.cx = address;
    regs.w.si = size >> 16;
    regs.w.di = size;
    int386(49, &regs, &regs);
    return regs.x.cflag & 1;
}

void causeway_disable_error_dump(void)
{
    union REGS regs;
    struct SREGS sregs;

    segread(&sregs);
    regs.h.cl = 0;
    regs.w.ax = 65328;
    int386x(49, &regs, &regs, &sregs);
}
