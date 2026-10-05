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

void dpmi_get_free_memory(int a1)
{
{
    char l_40[28];
    char l_24[12];

    mc_memset((int)l_24, 0, 12, (int)D_00177348, 39, 4);
    *(int *)l_40 = 1280;
    *(int *)((char *)l_40 + 20) = a1;
    *(short *)l_24 = _FP_SEG((void *)a1);
    int386x(49, (int)l_40, (int)l_40, (int)l_24);
}
}

int dpmi_lock_region(int a1, int a2)
{
    char l_38[28];

    if (a1 == 0 || a2 == 0) return 0;
    mc_memset((int)l_38, 0, 28, (int)D_00177348, 66, 4);
    *(short *)l_38 = 1536;
    *(short *)((char *)l_38 + 4) = a1 >> 16;
    *(short *)((char *)l_38 + 8) = a1;
    *(short *)((char *)l_38 + 16) = a2 >> 16;
    *(short *)((char *)l_38 + 20) = a2;
    int386(49, (int)l_38, (int)l_38);
    return *(int *)((char *)l_38 + 24) & 1;
}

int dpmi_unlock_region(int a1, int a2)
{
    char l_38[28];

    if (a1 == 0 || a2 == 0) return 0;
    mc_memset((int)l_38, 0, 28, (int)D_00177348, 86, 4);
    *(short *)l_38 = 1537;
    *(short *)((char *)l_38 + 4) = a1 >> 16;
    *(short *)((char *)l_38 + 8) = a1;
    *(short *)((char *)l_38 + 16) = a2 >> 16;
    *(short *)((char *)l_38 + 20) = a2;
    int386(49, (int)l_38, (int)l_38);
    return *(int *)((char *)l_38 + 24) & 1;
}

void causeway_disable_error_dump(void)
{
    char l_3C[28];
    char l_20[12];

    segread((int)l_20);
    *(signed char *)((char *)l_3C + 8) = 0;
    *(short *)l_3C = 65328;
    int386x(49, (int)l_3C, (int)l_3C, (int)l_20);
}
