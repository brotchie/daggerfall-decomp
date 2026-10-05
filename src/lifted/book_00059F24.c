/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_0012B508;
extern short font_height;
extern short D_00142928;
extern short D_0014292C;
extern signed char text_buffer[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern char scratch_buffer[];
extern int book_page_offsets;
extern short book_file;

extern int lseek();
extern int read();
extern int xn_font_select();
extern void book_flush_line(void);
extern void func_0005A1C8(char *);

void book_draw_page(int page)
{
    short line_height;
    {
        char *text;

        xn_font_select(4);
        lseek((int)(short)book_file, *(int *)((char *)(int)(*(char **)&book_page_offsets + (((int)(short)*(short *)&page) << 2))), 0);
        read((int)(short)book_file, *(int *)scratch_buffer, 16000);
        text = *(char **)scratch_buffer;
        D_0012B508 = 145;
        *(short *)scratch_190d64 = 0;
        line_height = font_height;
        D_00142928 = 10;
        D_0014292C = 20;
        while (((int)(unsigned char)*(signed char *)text) != 246) {
            switch (*(unsigned char *)text) {
            case 251:
                D_00142928 = *(short *)(text + 1);
                text += 3;
                break;
            case 247:
                func_0005A1C8(text + 1);
                while (*(signed char *)(text++) != 0);
                break;
            case 250:
                D_0012B508 = *(signed char *)(text + 1);
                text += 2;
                break;
            case 249:
                book_flush_line();
                xn_font_select((int)(short)((unsigned short)(unsigned char)*(signed char *)(text + 1)));
                text += 2;
                if ((short)(short)*(int *)&line_height < font_height) line_height = font_height;
                break;
            case 253:
                *(short *)scratch_190d66 = 1;
                text++;
                break;
            case 1:
                text++;
                text_buffer[(int)(short)*(short *)scratch_190d64] = 0;
                book_flush_line();
                break;
            case 0:
                text++;
                text_buffer[(int)(short)*(short *)scratch_190d64] = 0;
                book_flush_line();
                D_00142928 = 10;
                D_0014292C += *(int *)&line_height;
                line_height = font_height;
                break;
            default:
                text_buffer[(int)(short)(*(short *)scratch_190d64)++] = *(signed char *)(text++);
            }
        }
        if (text_buffer[0] == 0) return;
        text_buffer[(int)(short)*(short *)scratch_190d64] = 0;
        book_flush_line();
    }
}
