/* moninit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int rand_range(int, int);
extern int object_create_child(int, int, int);
extern int object_new_id(int);
extern void func_0005E37F(unsigned short, int, int, int);

int monster_make_item(int a1, int a2, int a3, int a4, int a5, int a6)
{
    int l_14;
    int l_10;

    if (rand_range(1, 100) <= a6) goto L79253;
    return 0;
L79253:;
    l_14 = object_create_child(a1, 0, 107);
    *(signed char *)((char *)l_14) = 2;
    *(int *)((char *)l_14 + 31) = object_new_id(((unsigned)*(int *)((char *)a1 + 31)) >> 16);
    l_10 = l_14 + 71;
L7928A:;
    func_0005E37F((int)(unsigned short)*(short *)&a2, a3, a4, l_10);
    if (((int)(unsigned short)*(short *)((char *)l_10 + 34)) == a5) goto L7928A;
    return l_14;
}
