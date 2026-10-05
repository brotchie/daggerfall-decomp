/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char quest_global_states[];


int quest_arg_state(int a1, short a2)
{
    short l_18;

    if (*(int *)((char *)((((int)(short)a2) * 15) + a1) + 13) != (-1)) goto L2FFD5;
    return 1;
L2FFD5:;
    *(int *)&l_18 = *(int *)((char *)((((int)(short)a2) * 15) + a1) + 7);
    if (*(int *)&l_18 != 0) goto L2FFF7;
    return 0;
L2FFF7:;
    if (((int)(unsigned char)(*(signed char *)((char *)((((int)(short)a2) * 15) + a1) + 6) & 1)) == 0) goto L30065;
    if (*(signed char *)(*(char **)&l_18 + 2) == 0) goto L30044;
    return ((*(signed char *)(quest_global_states + ((int)(unsigned char)*(signed char *)(*(char **)&l_18 + 3))) == 0) ? 1 : 0);
L30044:;
    return ((*(signed char *)(*(char **)&l_18 + 3) == 0) ? 1 : 0);
L30065:;
    if (*(signed char *)(*(char **)&l_18 + 2) == 0) goto L30083;
    return (int)(unsigned char)*(signed char *)(quest_global_states + ((int)(unsigned char)*(signed char *)(*(char **)&l_18 + 3)));
L30083:;
    return (int)(unsigned char)*(signed char *)(*(char **)&l_18 + 3);
}
