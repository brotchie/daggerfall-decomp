/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00185716[];
extern char D_00185717[];
extern char D_00185718[];
extern char D_00185719[];
extern char D_00185766[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern char D_001998E2[];

extern void func_00057147(short, short, short, short, short, short, short);

void itemmaker_set_power_param_cb(int param)
{
    short exclusion_row;

    *(short *)(D_001998E2 + (((int)(short)*(short *)scratch_190d64) << 2)) = param;
    *(int *)&exclusion_row = (int)(unsigned char)*(signed char *)(D_00185766 + ((int)(short)*(short *)scratch_190d66));
    if (exclusion_row == 0) {
        func_00057147((int)(short)*(short *)scratch_190d64, (int)(short)*(short *)scratch_190d66, (int)(short)*(short *)&param, -1, -1, -1, -1);
        return;
    }
    (*(int *)&exclusion_row)--;
    func_00057147((int)(short)*(short *)scratch_190d64, (int)(short)*(short *)scratch_190d66, (int)(short)*(short *)&param, (int)(short)((int)(unsigned char)*(signed char *)(D_00185716 + ((((int)(short)exclusion_row) * 20) + (((int)(short)*(short *)&param) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185717 + ((((int)(short)exclusion_row) * 20) + (((int)(short)*(short *)&param) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185718 + ((((int)(short)exclusion_row) * 20) + (((int)(short)*(short *)&param) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185719 + ((((int)(short)exclusion_row) * 20) + (((int)(short)*(short *)&param) << 2)))));
}
