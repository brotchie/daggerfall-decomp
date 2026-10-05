/* int.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00177348[];

extern int mc_memset();
extern int int386x();
extern int int386();
extern int segread();
extern unsigned short _FP_SEG( const volatile void __far * );
#pragma aux _FP_SEG = parm caller [eax dx] value [dx] modify exact [];

void dpmi_get_free_memory(int info)
{
{
    char regs[28];
    char sregs[12];

    mc_memset((int)sregs, 0, 12, (int)D_00177348, 39, 4);
    *(int *)regs = 1280;
    *(int *)((char *)regs + 20) = info;
    *(short *)sregs = _FP_SEG((void *)info);
    int386x(49, (int)regs, (int)regs, (int)sregs);
}
}

int dpmi_lock_region(int address, int size)
{
    char regs[28];

    if (address == 0 || size == 0) return 0;
    mc_memset((int)regs, 0, 28, (int)D_00177348, 66, 4);
    *(short *)regs = 1536;
    *(short *)((char *)regs + 4) = address >> 16;
    *(short *)((char *)regs + 8) = address;
    *(short *)((char *)regs + 16) = size >> 16;
    *(short *)((char *)regs + 20) = size;
    int386(49, (int)regs, (int)regs);
    return *(int *)((char *)regs + 24) & 1;
}

int dpmi_unlock_region(int address, int size)
{
    char regs[28];

    if (address == 0 || size == 0) return 0;
    mc_memset((int)regs, 0, 28, (int)D_00177348, 86, 4);
    *(short *)regs = 1537;
    *(short *)((char *)regs + 4) = address >> 16;
    *(short *)((char *)regs + 8) = address;
    *(short *)((char *)regs + 16) = size >> 16;
    *(short *)((char *)regs + 20) = size;
    int386(49, (int)regs, (int)regs);
    return *(int *)((char *)regs + 24) & 1;
}

void causeway_disable_error_dump(void)
{
    char regs[28];
    char sregs[12];

    segread((int)sregs);
    *(signed char *)((char *)regs + 8) = 0;
    *(short *)regs = 65328;
    int386x(49, (int)regs, (int)regs, (int)sregs);
}
