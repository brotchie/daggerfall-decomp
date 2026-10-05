/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char xn_mouse_cursor_drawn;
extern char enchant_power_params[];
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern int list_popup_callback;
extern char itemmaker_slots[];
extern char D_001998E2[];

extern int itemmaker_pick_param_list(int);
extern int itemmaker_free_slot(void);
extern void msgbox_show_rsc(int, int);
extern void itemmaker_set_power_param_cb(short);
extern void func_00057147(short, short, short, short, short, short, short);
extern void itemmaker_show_param_list(int, short);

void itemmaker_add_power_cb(int a1)
{
    short l_18;

    *(short *)scratch_190d66 = a1;
    *(int *)&l_18 = itemmaker_free_slot();
    *(short *)scratch_190d64 = *(int *)&l_18;
    if (((int)(short)l_18) == (-1)) {
        msgbox_show_rsc(1657, 1);
        return;
    }
    scratch_190ce4[(int)(short)l_18] = 0;
    if (*(int *)(enchant_power_params + (((int)(short)*(short *)&a1) << 2)) != 0) {
        *(short *)(itemmaker_slots + (((int)(short)l_18) << 2)) = a1;
        if (((unsigned)*(int *)(enchant_power_params + (((int)(short)*(short *)&a1) << 2))) < 5) {
            if (itemmaker_pick_param_list(*(int *)(enchant_power_params + (((int)(short)*(short *)&a1) << 2))) == 0) {
                scratch_190ce4[(int)(short)l_18] = 255;
                return;
            }
        } else {
            itemmaker_show_param_list(*(int *)(enchant_power_params + (((int)(short)*(short *)&a1) << 2)), (int)(short)*(short *)&a1);
        }
        list_popup_callback = (int)itemmaker_set_power_param_cb;
    } else {
        if (((int)(short)*(short *)&a1) == 11) {
            func_00057147((int)(short)l_18, 23, -1, 11, -1, -1, -1);
        }
        if (((int)(short)*(short *)&a1) == 12) {
            func_00057147((int)(short)l_18, 24, -1, 12, -1, -1, -1);
        }
        *(short *)(itemmaker_slots + (((int)(short)l_18) << 2)) = a1;
        *(short *)(D_001998E2 + (((int)(short)l_18) << 2)) = 65535;
    }
    xn_mouse_cursor_drawn &= 254;
}
