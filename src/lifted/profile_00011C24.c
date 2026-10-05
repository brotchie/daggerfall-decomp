/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char D_00170129[];

extern int open(int, ...);
extern int func_0009DEA7();
extern int mc_free();
extern int write();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_close;

int profile_close(int a1)
{
    int l_10;

    if (((struct bf8_7_1 *)((char *)a1 + 1))->f == 0) goto L11CC8;
    l_10 = open(a1 + 4, 610, 0);
    if (l_10 != (-1)) goto L11CA6;
    if (*(int *)((char *)a1 + 132) == 0) goto L11C76;
    if (*(int *)((char *)a1 + 132) != (-1751672937)) goto L11C78;
L11C76:;
    goto L11C9D;
L11C78:;
    mc_free(*(int *)((char *)a1 + 132), (int)D_00170129, 171);
    *(int *)((char *)a1 + 132) = -1751672937;
L11C9D:;
    return 0;
L11CA6:;
    write(l_10, *(int *)((char *)a1 + 132), *(int *)((char *)a1 + 136));
    func_0009DEA7(l_10);
L11CC8:;
    if (*(int *)((char *)a1 + 132) == 0) goto L11CE3;
    if (*(int *)((char *)a1 + 132) != (-1751672937)) goto L11CE5;
L11CE3:;
    goto L11D0A;
L11CE5:;
    mc_free(*(int *)((char *)a1 + 132), (int)D_00170129, 185);
    *(int *)((char *)a1 + 132) = -1751672937;
L11D0A:;
    return 1;
}
