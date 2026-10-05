/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0012B508[];
extern char D_0012DA44[];
extern char D_00142928[];
extern char D_0014292C[];
extern char text_buffer[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00195C44[];
extern char book_page_offsets[];
extern char book_file[];

extern int lseek();
extern int func_000A00CB();
extern int func_0012DB50();
extern void book_flush_line(void);
extern void func_0005A1C8(int);

void book_draw_page(int a1)
{
    short l_18;
{
    int l_20;

    func_0012DB50(4);
    lseek((int)(short)*(short *)book_file, *(int *)((char *)(int)(*(char **)book_page_offsets + (((int)(short)*(short *)&a1) << 2))), 0);
    func_000A00CB((int)(short)*(short *)book_file, *(int *)D_00195C44, 16000);
    l_20 = *(int *)D_00195C44;
    *(signed char *)D_0012B508 = 145;
    *(short *)D_00190D64 = 0;
    l_18 = *(short *)D_0012DA44;
    *(short *)D_00142928 = 10;
    *(short *)D_0014292C = 20;
L59FA8:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 246) goto L5A117;
    switch (*(unsigned char *)((char *)l_20)) {
    goto L5A0F3;
case 251:
    *(short *)D_00142928 = *(short *)((char *)l_20 + 1);
    l_20 += 3;
    goto L5A112;
case 247:
    func_0005A1C8(l_20 + 1);
L5A03B:;
    if (*(signed char *)((char *)l_20++) != 0) goto L5A03B;
    goto L5A112;
case 250:
    *(signed char *)D_0012B508 = *(signed char *)((char *)l_20 + 1);
    l_20 += 2;
    goto L5A112;
case 249:
    book_flush_line();
    func_0012DB50((int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)l_20 + 1)));
    l_20 += 2;
    if ((short)(short)*(int *)&l_18 >= *(short *)D_0012DA44) goto L5A08B;
    l_18 = *(short *)D_0012DA44;
L5A08B:;
    goto L5A112;
case 253:
    *(short *)D_00190D66 = 1;
    l_20++;
    goto L5A112;
case 1:
    l_20++;
    *(signed char *)(text_buffer + ((int)(short)*(short *)D_00190D64)) = 0;
    book_flush_line();
    goto L5A112;
case 0:
    l_20++;
    *(signed char *)(text_buffer + ((int)(short)*(short *)D_00190D64)) = 0;
    book_flush_line();
    *(short *)D_00142928 = 10;
    *(short *)D_0014292C += *(int *)&l_18;
    l_18 = *(short *)D_0012DA44;
    goto L5A112;
default:
L5A0F3:;
    *(signed char *)(text_buffer + ((int)(short)(*(short *)D_00190D64)++)) = *(signed char *)((char *)l_20++);
L5A112:;
    goto L59FA8;
L5A117:;
    if (*(signed char *)text_buffer == 0) return;
    *(signed char *)(text_buffer + ((int)(short)*(short *)D_00190D64)) = 0;
    book_flush_line();
}
}
}
