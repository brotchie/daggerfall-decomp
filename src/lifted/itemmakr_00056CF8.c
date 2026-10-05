/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char enchant_side_effect_params[];
extern char itemmaker_slot_kinds[];
extern char D_00190D64[];
extern char list_popup_callback[];
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
    *(short *)D_00190D64 = *(int *)&l_18;
    if (((int)(short)l_18) != (-1)) goto L56D37;
    msgbox_show_rsc(1658, 1);
    return;
L56D37:;
    *(signed char *)(itemmaker_slot_kinds + ((int)(short)l_18)) = 1;
    if (*(int *)(enchant_side_effect_params + (((int)(short)*(short *)&a1) << 2)) == 0) goto L56DCB;
    *(short *)(itemmaker_slots + (((int)(short)l_18) << 2)) = a1;
    if (((unsigned)*(int *)(enchant_side_effect_params + (((int)(short)*(short *)&a1) << 2))) >= 5) goto L56D9F;
    if (itemmaker_pick_param_list(*(int *)(enchant_side_effect_params + (((int)(short)*(short *)&a1) << 2))) != 0) goto L56D9D;
    *(signed char *)(itemmaker_slot_kinds + ((int)(short)l_18)) = 255;
    return;
L56D9D:;
    goto L56DBC;
L56D9F:;
    itemmaker_show_param_list(*(int *)(enchant_side_effect_params + (((int)(short)*(short *)&a1) << 2)), (int)(short)(a1 + 15));
L56DBC:;
    *(int *)list_popup_callback = (int)itemmaker_set_side_effect_param_cb;
    return;
L56DCB:;
    if (((int)(short)*(short *)&a1) != 8) goto L56DFE;
    func_00057147((int)(short)l_18, 11, -1, 23, -1, -1, -1);
L56DFE:;
    if (((int)(short)*(short *)&a1) != 9) goto L56E31;
    func_00057147((int)(short)l_18, 12, -1, 24, -1, -1, -1);
L56E31:;
    *(short *)(itemmaker_slots + (((int)(short)l_18) << 2)) = a1;
    *(short *)(D_001998E2 + (((int)(short)l_18) << 2)) = 65535;
}
