/* question.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00190D64[];
extern int D_00195C44;

extern int func_0012B2EB();
extern int func_0012B3ED();
extern int func_00144FB4();
extern void func_0005175B(int, short);

void class_question_scroll(int a1, short a2)
{
    func_0012B2EB();
    func_0005175B(a1, (int)(short)a2);
    func_00144FB4(0, 135, 320, 48, (int)(*(char **)&D_00195C44 + (((int)(short)*(short *)D_00190D64) * 320)));
    func_0012B3ED();
}
