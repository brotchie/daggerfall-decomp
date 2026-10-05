/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char quest_global_states[];


void quest_set_state(int a1, int a2, short a3)
{
    int l_14;

    if (*(int *)((char *)a2 + 13) == (-1)) return;
    l_14 = *(int *)((char *)a2 + 7);
    if (((int)(unsigned char)(*(signed char *)((char *)a2 + 6) & 1)) == 0) goto L3049B;
    *(int *)&a3 ^= 1;
L3049B:;
    if (*(signed char *)((char *)l_14 + 2) == 0) goto L304B7;
    *(signed char *)(quest_global_states + ((int)(unsigned char)*(signed char *)((char *)l_14 + 3))) = *(signed char *)&a3;
    return;
L304B7:;
    *(signed char *)((char *)l_14 + 3) = *(signed char *)&a3;
}
