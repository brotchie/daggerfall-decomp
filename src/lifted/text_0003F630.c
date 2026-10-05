/* text.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0017B630[];
extern char D_0017B631[];
extern char msgbox_spop_tiles[];
extern char D_00199658[];
extern char D_00199660[];
extern char D_00199662[];
extern char D_00199666[];


void func_0003F630(short a1)
{
    *(int *)D_00199658 = *(int *)(msgbox_spop_tiles + (((int)(short)a1) << 2));
    *(short *)D_00199666 = (unsigned short)(unsigned char)*(signed char *)(D_0017B630 + (((int)(short)a1) * 2));
    *(short *)D_00199662 = (unsigned short)(unsigned char)*(signed char *)(D_0017B631 + (((int)(short)a1) * 2));
    *(short *)D_00199660 = *(short *)D_00199666 * *(short *)D_00199662;
}
