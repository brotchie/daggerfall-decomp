/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int enchant_side_effect_params[];
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern int list_popup_callback;
extern char itemmaker_slots[];
extern char D_001998E2[];

extern int itemmaker_pick_param_list(int);
extern int itemmaker_free_slot(void);
extern void msgbox_show_rsc(int, int);
extern void itemmaker_set_side_effect_param_cb(short);
extern void func_00057147(short, short, short, short, short, short, short);
extern void itemmaker_show_param_list(int, short);

void itemmaker_add_side_effect_cb(int a1)
{
    short l_18;

    *(int *)&l_18 = itemmaker_free_slot();
    *(short *)scratch_190d64 = *(int *)&l_18;
    if (((int)(short)l_18) == (-1)) {
        msgbox_show_rsc(1658, 1);
        return;
    }
    scratch_190ce4[(int)(short)l_18] = 1;
    if (enchant_side_effect_params[((int)(short)*(short *)&a1)] != 0) {
        *(short *)(itemmaker_slots + (((int)(short)l_18) << 2)) = a1;
        if (((unsigned)enchant_side_effect_params[((int)(short)*(short *)&a1)]) < 5) {
            if (itemmaker_pick_param_list(enchant_side_effect_params[((int)(short)*(short *)&a1)]) == 0) {
                scratch_190ce4[(int)(short)l_18] = 255;
                return;
            }
        } else {
            itemmaker_show_param_list(enchant_side_effect_params[((int)(short)*(short *)&a1)], (int)(short)(a1 + 15));
        }
        list_popup_callback = (int)itemmaker_set_side_effect_param_cb;
        return;
    }
    if (((int)(short)*(short *)&a1) == 8) func_00057147((int)(short)l_18, 11, -1, 23, -1, -1, -1);
    if (((int)(short)*(short *)&a1) == 9) func_00057147((int)(short)l_18, 12, -1, 24, -1, -1, -1);
    *(short *)(itemmaker_slots + (((int)(short)l_18) << 2)) = a1;
    *(short *)(D_001998E2 + (((int)(short)l_18) << 2)) = 65535;
}
