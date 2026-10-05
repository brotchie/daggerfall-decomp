/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char wagon_container[];
extern char D_00195A80[];
extern char player_object[];
extern char D_00195AA8[];
extern char D_00195AC4[];
extern char inv_right_container[];
extern char trade_mode[];
extern char inventory_action[];
extern char D_00195D54[];
extern char D_00196120[];
extern char D_001962AE[];
extern char inv_selected_item[];

extern int object_weight(int);
extern int object_reparent(int, int);
extern int object_new_id(int);
extern int trade_can_repair_item(int);
extern void msgbox_show_rsc(int, int);
extern void quest_raise_event(int, int, int);
extern void inv_use_item(void);
extern void inv_item_info(int, int);
extern void inv_equip_item(int);
extern void inv_toggle_hidden(void);
extern void func_000992FA(void);

void inv_click_left_item(int a1)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
{
    int l_3C;

__dagger_tbl946E8:;
    *(int *)D_00195AA8 = (*(int *)inv_selected_item = a1);
    l_34 = a1 + 71;
    *(int *)D_00195A80 = l_34;
    l_3C = *(int *)inventory_action - 1;
    switch (l_3C) {
case 0:
    inv_item_info(*(int *)inv_selected_item, l_34);
    return;
case 1:
    if (*(int *)trade_mode == 0) goto L94763;
    if (*(int *)trade_mode != 1) goto L94793;
L94763:;
    if (((int)(unsigned short)(*(short *)((char *)a1 + 21) & 32)) == 0) goto L94784;
    if (((int)(short)*(short *)((char *)l_34 + 67)) != (-1)) goto L9478E;
L94784:;
    inv_equip_item(*(int *)inv_selected_item);
L9478E:;
    return;
case 2:
L94793:;
    if (((int)(unsigned short)*(short *)((char *)l_34 + 32)) != 23) goto L947AD;
    if (*(int *)trade_mode != 2) goto L947AF;
L947AD:;
    goto L947B4;
L947AF:;
    return;
L947B4:;
    if (((int)(unsigned short)*(short *)((char *)l_34 + 32)) != 23) goto L947D2;
    if (*(int *)inv_right_container == *(int *)wagon_container) goto L947D4;
L947D2:;
    goto L947D9;
L947D4:;
    return;
L947D9:;
    if (*(int *)inv_right_container != *(int *)wagon_container) goto L9481D;
    l_28 = object_weight(a1);
    *(signed char *)D_001962AE = 1;
    l_24 = object_weight(*(int *)wagon_container);
    *(signed char *)D_001962AE = 0;
    if ((l_28 + l_24) > 3000) return;
L9481D:;
    if (*(int *)trade_mode != 3) goto L94832;
    if (((int)(short)*(short *)((char *)l_34 + 67)) != (-1)) goto L94834;
L94832:;
    goto L94848;
L94834:;
    msgbox_show_rsc(33, 1);
    return;
L94848:;
    if (*(int *)trade_mode != 4) goto L948BE;
    *(int *)((char *)a1 + 7) = *(int *)(*(char **)player_object + 7);
    *(int *)((char *)a1 + 11) = *(int *)(*(char **)player_object + 11);
    *(int *)((char *)a1 + 15) = *(int *)(*(char **)player_object + 15);
    *(int *)((char *)a1 + 47) = 0;
    if (*(short *)((char *)a1 + 27) != 0) goto L9489D;
    *(short *)((char *)a1 + 27) = *(short *)((char *)l_34 + 52);
L9489D:;
    object_reparent(*(int *)inv_right_container, a1);
    *(int *)((char *)a1 + 31) = object_new_id(0);
    return;
L948BE:;
    if (*(int *)trade_mode != 3) goto L948D3;
    if (trade_can_repair_item(l_34) == 0) goto L948D5;
L948D3:;
    goto L948DA;
L948D5:;
    return;
L948DA:;
    *(int *)((char *)a1 + 7) = *(int *)(*(char **)player_object + 7);
    *(int *)((char *)a1 + 11) = *(int *)(*(char **)player_object + 11);
    *(int *)((char *)a1 + 15) = *(int *)(*(char **)player_object + 15);
    *(int *)((char *)a1 + 47) = 0;
    if (*(short *)((char *)a1 + 27) != 0) goto L94926;
    *(short *)((char *)a1 + 27) = *(short *)((char *)l_34 + 52);
L94926:;
    object_reparent(*(int *)inv_right_container, a1);
    if (((int)D_00196120) != *(int *)((char *)a1 + 67)) goto L9494E;
    *(short *)((char *)a1 + 23) = *(short *)D_00195D54;
L9494E:;
    *(int *)((char *)a1 + 31) = object_new_id(((unsigned)*(int *)(*(char **)D_00195AC4 + 31)) >> 16);
    if (*(int *)((char *)a1 + 51) == 0) goto L9497E;
    *(int *)(*(char **)((char *)a1 + 51) + 31) = *(int *)((char *)a1 + 31);
L9497E:;
    quest_raise_event(5, a1, 0);
    func_000992FA();
    return;
case 3:
    inv_use_item();
    return;
case 4:
    inv_toggle_hidden();
default:;
}
}
}
