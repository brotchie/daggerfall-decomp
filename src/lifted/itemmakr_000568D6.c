/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00185716[];
extern char D_00185717[];
extern char D_00185718[];
extern char D_00185719[];
extern char D_00185766[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_001998E2[];

extern void func_00057147(short, short, short, short, short, short, short);

void itemmaker_set_power_param_cb(int a1)
{
    short l_18;

    *(short *)(D_001998E2 + (((int)(short)*(short *)D_00190D64) << 2)) = a1;
    *(int *)&l_18 = (int)(unsigned char)*(signed char *)(D_00185766 + ((int)(short)*(short *)D_00190D66));
    if (l_18 == 0) {
        func_00057147((int)(short)*(short *)D_00190D64, (int)(short)*(short *)D_00190D66, (int)(short)*(short *)&a1, -1, -1, -1, -1);
        return;
    }
    (*(int *)&l_18)--;
    func_00057147((int)(short)*(short *)D_00190D64, (int)(short)*(short *)D_00190D66, (int)(short)*(short *)&a1, (int)(short)((int)(unsigned char)*(signed char *)(D_00185716 + ((((int)(short)l_18) * 20) + (((int)(short)*(short *)&a1) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185717 + ((((int)(short)l_18) * 20) + (((int)(short)*(short *)&a1) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185718 + ((((int)(short)l_18) * 20) + (((int)(short)*(short *)&a1) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185719 + ((((int)(short)l_18) * 20) + (((int)(short)*(short *)&a1) << 2)))));
}
