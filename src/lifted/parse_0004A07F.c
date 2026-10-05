/* parse.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001841E3[];
extern char D_00190D1F[];

extern int func_0008B43B(unsigned char, unsigned char, int);

int func_0004A07F(int a1, int a2)
{
    return func_0008B43B((int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(signed char)*(signed char *)D_00190D1F)), (int)(unsigned char)*(signed char *)&a2, a1);
}
