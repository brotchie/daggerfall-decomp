/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern short D_00142940;
extern short D_00142944;
extern short steer_row_y1;
extern short steer_row_y2;
extern short steer_col_x1;
extern short steer_col_x2;


int intrface_region_at(int a1, int a2, int a3, int a4)
{
    int l_10;

    if (((int)(short)steer_row_y1) > a2) {
        l_10 = 0;
        *(int *)((char *)a4) = (int)(short)D_00142944;
    } else if (((int)(short)steer_row_y1) <= a2 && ((int)(short)steer_row_y2) > a2) {
        l_10 = 3;
        *(int *)((char *)a4) = (int)(short)steer_row_y1;
    } else {
        l_10 = 6;
        *(int *)((char *)a4) = (int)(short)steer_row_y2;
    }
    if (((int)(short)steer_col_x1) > a1) {
        *(int *)((char *)a3) = (int)(short)D_00142940;
    } else if (((int)(short)steer_col_x1) <= a1 && ((int)(short)steer_col_x2) > a1) {
        l_10++;
        *(int *)((char *)a3) = (int)(short)steer_col_x1;
    } else {
        l_10 += 2;
        *(int *)((char *)a3) = (int)(short)steer_col_x2;
    }
    return l_10;
}
