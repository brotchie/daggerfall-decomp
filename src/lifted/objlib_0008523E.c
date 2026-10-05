/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00196280[];

extern int model_cache_find(int);

int model_get(int a1, int a2, int a3)
{
    if (a1 != 4) goto L85267;
    if (a2 == 46) goto L85265;
    if (a2 != 47) goto L85267;
L85265:;
    goto L85269;
L85267:;
    goto L85282;
L85269:;
    if (*(signed char *)D_00196280 == 0) goto L8527B;
    a2 = 46;
    goto L85282;
L8527B:;
    a2 = 47;
L85282:;
    return model_cache_find((a2 + (a1 * 100)) + (a3 << 17));
}
