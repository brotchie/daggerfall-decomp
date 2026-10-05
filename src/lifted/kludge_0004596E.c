/* kludge.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int object_create_child(int, int, int);
extern int object_reparent(int, int);
extern void item_make_random(unsigned short, int);
extern void item_make(int, int, int);

int func_0004596E(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = object_create_child(a1, 0, 107);
    *(signed char *)((char *)l_20) = 2;
    l_18 = l_20 + 71;
    item_make_random((int)(unsigned short)*(short *)&a2, l_18);
    if (((int)(unsigned short)(*(short *)((char *)l_18 + 42) & 8)) == 0) goto L45A0B;
    l_1C = object_create_child(a1, 0, 107);
    *(signed char *)((char *)l_1C) = 2;
    object_reparent(l_1C, l_20);
    l_18 = l_1C + 71;
    item_make(1, 1, l_18);
    return l_1C;
L45A0B:;
    return l_20;
}
