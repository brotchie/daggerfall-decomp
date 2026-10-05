/* inpstr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00176E2C[];
extern char D_00190B44[];
extern char *inpstr_text;
extern short inpstr_max_length;
extern short inpstr_cursor;
extern signed char input_digits_only;

extern int mc_strncpy();
extern int strlen();
extern int xn_kbd_flush();

void inpstr_begin_text(char *text, int max_length)
{
    xn_kbd_flush();
    input_digits_only = 0;
    inpstr_text = text;
    mc_strncpy(D_00190B44, inpstr_text, 160, D_00176E2C, 121);
    inpstr_cursor = strlen(text);
    inpstr_max_length = max_length;
}
