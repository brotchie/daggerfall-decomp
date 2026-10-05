/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int rand_range(int, int);

short func_00086093(int a1)
{
    int l_20;
    int l_1C;

    l_20 = rand_range(0, (int)(short)*(short *)((char *)a1 + 2));
L860B5:;
    l_1C = 2;
L860BC:;
    if (l_1C < 14) goto L860CD;
    goto L86126;
L860C7:;
    l_1C += 2;
    goto L860BC;
L860CD:;
    if (*(short *)((char *)((l_1C * 2) + a1)) != 0) goto L860EA;
    if (*(short *)((char *)((l_1C * 2) + a1) + 2) == 0) goto L860EC;
L860EA:;
    goto L860EE;
L860EC:;
    goto L86126;
L860EE:;
    if (((int)(short)*(short *)((char *)((l_1C * 2) + a1))) > l_20) goto L8610F;
    if (((int)(short)*(short *)((char *)((l_1C * 2) + a1) + 2)) >= l_20) goto L86111;
L8610F:;
    goto L86124;
L86111:;
    return l_20 + (*(short *)((char *)a1) << 7);
L86124:;
    goto L860C7;
L86126:;
    l_20 = rand_range(0, (int)(short)*(short *)((char *)a1 + 2));
    goto L860B5;
}
