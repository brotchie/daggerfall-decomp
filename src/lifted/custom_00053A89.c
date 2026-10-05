/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int screen_buffer;
extern char D_00175420[];
extern short D_00195F36;
extern unsigned short D_00195F38;

extern int inpstr_update(void);
extern int mc_memcpy();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern void inpstr_begin_text(int, short);

void classmaker_input_text(int a1, int a2, int a3)
{
    short l_10;

    func_0012B2EB();
    D_00195F36 = 100;
    D_00195F38 = 5;
    inpstr_begin_text(a1, (int)(short)*(short *)&a2);
    *(int *)&l_10 = 0;
    while (l_10 == 0) {
        if (a3 != 0 && ((int (*)())(a3))(0) != 0) return;
        if (inpstr_update() != 0) *(int *)&l_10 = 1;
        mc_memcpy(655360, screen_buffer, 64000, (int)D_00175420, 364, 4);
    }
    func_0012B3ED();
}
