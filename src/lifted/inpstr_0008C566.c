/* inpstr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00176E2C[];
extern char D_00190B44[];
extern int inpstr_text;
extern short inpstr_max_length;
extern short inpstr_cursor;
extern signed char input_digits_only;

extern int mc_strncpy();
extern int strlen();
extern int xn_kbd_flush();

void inpstr_begin_text(int a1, int a2)
{
    xn_kbd_flush();
    input_digits_only = 0;
    inpstr_text = a1;
    mc_strncpy((int)D_00190B44, inpstr_text, 160, (int)D_00176E2C, 121);
    inpstr_cursor = strlen(a1);
    inpstr_max_length = a2;
}
