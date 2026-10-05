/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char screen_buffer[];
extern char D_00175420[];
extern char D_00195F36[];
extern char D_00195F38[];

extern int inpstr_update(void);
extern int mc_memcpy();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern void inpstr_begin_text(int, short);

void classmaker_input_text(int a1, int a2, int a3)
{
    short l_10;

    func_0012B2EB();
    *(short *)D_00195F36 = 100;
    *(short *)D_00195F38 = 5;
    inpstr_begin_text(a1, (int)(short)*(short *)&a2);
    *(int *)&l_10 = 0;
L53AC8:;
    if (l_10 != 0) goto L53B15;
    if (a3 == 0) goto L53ADE;
    if (((int (*)())(a3))(0) != 0) goto L53AE0;
L53ADE:;
    goto L53AE2;
L53AE0:;
    return;
L53AE2:;
    if (inpstr_update() == 0) goto L53AF2;
    *(int *)&l_10 = 1;
L53AF2:;
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00175420, 364, 4);
    goto L53AC8;
L53B15:;
    func_0012B3ED();
}
