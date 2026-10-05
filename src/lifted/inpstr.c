/* inpstr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char D_0012DA50;
extern signed char key_down_home;
extern signed char key_down_left;
extern signed char key_down_right;
extern signed char key_down_end;
extern short D_00142948;
extern char D_00176E2C[];
extern char D_00190B44[];
extern int inpstr_text;
extern short D_00195F36;
extern char D_001A9AA0[];
extern short inpstr_max_length;
extern short inpstr_cursor;
extern signed char input_digits_only;
extern signed char D_001A9AB1;

extern int font_char_width(unsigned char);
extern int font_text_width(int);
extern int mc_strncpy();
extern int atoi();
extern int func_000A0DD9();
extern int func_000A0DF4();
extern int mc_memmove();
extern int mc_memcpy();
extern int func_0012B3ED();
extern int func_00142790();
extern int func_001427A8();
extern int func_00144E84();

int inpstr_read_key(void)
{
    unsigned char l_18;

    l_18 = func_001427A8();
    if (l_18 != 0) return (int)(unsigned char)l_18;
    if (key_down_left != 0) return 128;
    if (key_down_right != 0) return 129;
    if (key_down_home != 0) return 131;
    if (key_down_end != 0) return 130;
    return 0;
}

void inpstr_begin_number(int a1)
{
    func_00142790();
    input_digits_only = 1;
    func_000A0DD9(a1, (int)D_001A9AA0, 10);
    inpstr_text = (int)D_001A9AA0;
    mc_strncpy((int)D_00190B44, inpstr_text, 160, (int)D_00176E2C, 110);
    inpstr_cursor = func_000A0DF4((int)D_001A9AA0);
    inpstr_max_length = 8;
    D_001A9AB1 = D_0012DA50;
}

int inpstr_handle_key(unsigned char a1)
{
    switch ((unsigned char)a1) {
    case 13:
        func_0012B3ED();
        return atoi(inpstr_text);
    case 131:
        inpstr_cursor = 0;
        break;
    case 130:
        inpstr_cursor = func_000A0DF4(inpstr_text);
        break;
    case 27:
        func_0012B3ED();
        return 32768;
    case 128:
        if (inpstr_cursor != 0) (inpstr_cursor)--;
        break;
    case 129:
        if (((unsigned)((int)(short)inpstr_cursor)) < func_000A0DF4(inpstr_text)) {
            inpstr_cursor++;
        }
        break;
    case 8:
        if (inpstr_cursor != 0) {
            mc_memcpy((int)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)) - 1, (int)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)), (int)&*(signed char *)((char *)(func_000A0DF4(inpstr_text) - ((int)(short)inpstr_cursor)) + 1), (int)D_00176E2C, 217, 4);
            inpstr_cursor--;
        }
        break;
    case 127:
        if (((unsigned)((int)(short)inpstr_cursor)) < func_000A0DF4(inpstr_text)) {
            mc_memcpy((int)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)), (int)&*(signed char *)((char *)(int)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)) + 1), (int)&*(signed char *)((char *)(func_000A0DF4(inpstr_text) - ((int)(short)inpstr_cursor)) + 1), (int)D_00176E2C, 226, 4);
        }
        break;
    default:
        if (((int)(unsigned char)a1) < 128 && ((unsigned)func_000A0DF4(inpstr_text)) < ((int)(short)inpstr_max_length)) {
            if (input_digits_only != 0 && (((int)(unsigned char)a1) < 48 || ((int)(unsigned char)a1) > 57)) {
            } else if (((font_text_width(inpstr_text) + font_char_width((int)(unsigned char)a1)) + ((int)(unsigned short)D_00195F36)) < ((int)(short)D_00142948)) {
                if (((int)(short)inpstr_cursor) == func_000A0DF4(inpstr_text)) {
                    *(signed char *)((char *)(int)(*(char **)&inpstr_text + ((int)(short)(inpstr_cursor)++))) = a1;
                    *(signed char *)((char *)(int)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor))) = 0;
                } else {
                    mc_memmove((int)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)) + 1, (int)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)), (int)&*(signed char *)((char *)(func_000A0DF4(inpstr_text) - ((int)(short)inpstr_cursor)) + 1), (int)D_00176E2C, 245, 4);
                    *(signed char *)((char *)(int)(*(char **)&inpstr_text + ((int)(short)(inpstr_cursor)++))) = a1;
                }
            }
        }
    }
    return -2023406815;
}

int inpstr_text_width(int a1, short a2)
{
    unsigned char l_14;
    short l_18;

    l_14 = *(signed char *)((char *)(((int)(short)a2) + a1));
    *(signed char *)((char *)(((int)(short)a2) + a1)) = 0;
    *(int *)&l_18 = font_text_width(a1);
    *(signed char *)((char *)(((int)(short)a2) + a1)) = l_14;
    return (int)(short)l_18;
}

void picklist_save_background(struct picklist *a1)
{
    if (a1->framed == 0) return;
    func_00144E84(a1->list_rect.x, a1->list_rect.y, a1->list_rect.w, a1->list_rect.h, (int)a1->list_background, 0);
    func_00144E84(a1->bar_rect.x, a1->bar_rect.y, a1->bar_rect.w, a1->bar_rect.h, (int)a1->bar_background, 0);
}
