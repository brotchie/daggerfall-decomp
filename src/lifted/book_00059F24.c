/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_0012B508;
extern short D_0012DA44;
extern short D_00142928;
extern short D_0014292C;
extern signed char text_buffer[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00195C44[];
extern int book_page_offsets;
extern short book_file;

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
        lseek((int)(short)book_file, *(int *)((char *)(int)(*(char **)&book_page_offsets + (((int)(short)*(short *)&a1) << 2))), 0);
        func_000A00CB((int)(short)book_file, *(int *)D_00195C44, 16000);
        l_20 = *(int *)D_00195C44;
        D_0012B508 = 145;
        *(short *)D_00190D64 = 0;
        l_18 = D_0012DA44;
        D_00142928 = 10;
        D_0014292C = 20;
        while (((int)(unsigned char)*(signed char *)((char *)l_20)) != 246) {
            switch (*(unsigned char *)((char *)l_20)) {
            case 251:
                D_00142928 = *(short *)((char *)l_20 + 1);
                l_20 += 3;
                break;
            case 247:
                func_0005A1C8(l_20 + 1);
                while (*(signed char *)((char *)l_20++) != 0);
                break;
            case 250:
                D_0012B508 = *(signed char *)((char *)l_20 + 1);
                l_20 += 2;
                break;
            case 249:
                book_flush_line();
                func_0012DB50((int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)l_20 + 1)));
                l_20 += 2;
                if ((short)(short)*(int *)&l_18 < D_0012DA44) l_18 = D_0012DA44;
                break;
            case 253:
                *(short *)D_00190D66 = 1;
                l_20++;
                break;
            case 1:
                l_20++;
                text_buffer[(int)(short)*(short *)D_00190D64] = 0;
                book_flush_line();
                break;
            case 0:
                l_20++;
                text_buffer[(int)(short)*(short *)D_00190D64] = 0;
                book_flush_line();
                D_00142928 = 10;
                D_0014292C += *(int *)&l_18;
                l_18 = D_0012DA44;
                break;
            default:
                text_buffer[(int)(short)(*(short *)D_00190D64)++] = *(signed char *)((char *)l_20++);
            }
        }
        if (text_buffer[0] == 0) return;
        text_buffer[(int)(short)*(short *)D_00190D64] = 0;
        book_flush_line();
    }
}
