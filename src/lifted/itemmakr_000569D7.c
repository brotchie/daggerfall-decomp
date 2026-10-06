/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char xn_mouse_cursor_drawn;
extern iptr enchant_power_params[];
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern iptr list_popup_callback;
extern struct enchantment itemmaker_slots[];

extern int itemmaker_pick_param_list(iptr);
extern int itemmaker_free_slot(void);
extern void msgbox_show_rsc(int, int);
extern void itemmaker_set_power_param_cb(int);
extern void func_00057147(short, short, short, short, short, short, short);
extern void itemmaker_show_param_list(iptr, short);

void itemmaker_add_power_cb(int power)
{
    short slot;

    *(short *)scratch_190d66 = power;
    *(int *)&slot = itemmaker_free_slot();
    *(short *)scratch_190d64 = *(int *)&slot;
    if (((int)(short)slot) == (-1)) {
        msgbox_show_rsc(1657, 1);
        return;
    }
    scratch_190ce4[(int)(short)slot] = 0;
    if (enchant_power_params[((int)(short)*(short *)&power)] != 0) {
        itemmaker_slots[(short)slot].type = power;
        if (((unsigned)enchant_power_params[((int)(short)*(short *)&power)]) < 5) {
            if (itemmaker_pick_param_list(enchant_power_params[((int)(short)*(short *)&power)]) == 0) {
                scratch_190ce4[(int)(short)slot] = 255;
                return;
            }
        } else {
            itemmaker_show_param_list(enchant_power_params[((int)(short)*(short *)&power)], (int)(short)*(short *)&power);
        }
        list_popup_callback = (iptr)itemmaker_set_power_param_cb;
    } else {
        if (((int)(short)*(short *)&power) == 11) {
            func_00057147((int)(short)slot, 23, -1, 11, -1, -1, -1);
        }
        if (((int)(short)*(short *)&power) == 12) {
            func_00057147((int)(short)slot, 24, -1, 12, -1, -1, -1);
        }
        itemmaker_slots[(short)slot].type = power;
        itemmaker_slots[(short)slot].param = 65535;
    }
    xn_mouse_cursor_drawn &= 254;
}
