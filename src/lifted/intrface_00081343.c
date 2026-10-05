/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern short xn_gfx_clip_left;
extern short xn_gfx_clip_top;
extern short steer_row_y1;
extern short steer_row_y2;
extern short steer_col_x1;
extern short steer_col_x2;


int intrface_region_at(int x, int y, int *region_x, int *region_y)
{
    int region;

    if (((int)(short)steer_row_y1) > y) {
        region = 0;
        *region_y = (int)(short)xn_gfx_clip_top;
    } else if (((int)(short)steer_row_y1) <= y && ((int)(short)steer_row_y2) > y) {
        region = 3;
        *region_y = (int)(short)steer_row_y1;
    } else {
        region = 6;
        *region_y = (int)(short)steer_row_y2;
    }
    if (((int)(short)steer_col_x1) > x) {
        *region_x = (int)(short)xn_gfx_clip_left;
    } else if (((int)(short)steer_col_x1) <= x && ((int)(short)steer_col_x2) > x) {
        region++;
        *region_x = (int)(short)steer_col_x1;
    } else {
        region += 2;
        *region_x = (int)(short)steer_col_x2;
    }
    return region;
}
