/* inpstr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_0012DA50[];
extern char key_down_home[];
extern char key_down_left[];
extern char key_down_right[];
extern char key_down_end[];
extern char D_00142948[];
extern char D_00176E2C[];
extern char D_00190B44[];
extern char inpstr_text[];
extern char D_00195F36[];
extern char D_001A9AA0[];
extern char inpstr_max_length[];
extern char inpstr_cursor[];
extern char input_digits_only[];
extern char D_001A9AB1[];

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
    if (l_18 == 0) goto L8C488;
    return (int)(unsigned char)l_18;
L8C488:;
    if (*(signed char *)key_down_left == 0) goto L8C49A;
    return 128;
L8C49A:;
    if (*(signed char *)key_down_right == 0) goto L8C4AC;
    return 129;
L8C4AC:;
    if (*(signed char *)key_down_home == 0) goto L8C4BE;
    return 131;
L8C4BE:;
    if (*(signed char *)key_down_end == 0) goto L8C4D0;
    return 130;
L8C4D0:;
    return 0;
}

void inpstr_begin_number(int a1)
{
    func_00142790();
    *(signed char *)input_digits_only = 1;
    func_000A0DD9(a1, (int)D_001A9AA0, 10);
    *(int *)inpstr_text = (int)D_001A9AA0;
    mc_strncpy((int)D_00190B44, *(int *)inpstr_text, 160, (int)D_00176E2C, 110);
    *(short *)inpstr_cursor = func_000A0DF4((int)D_001A9AA0);
    *(short *)inpstr_max_length = 8;
    *(signed char *)D_001A9AB1 = *(signed char *)D_0012DA50;
}

int inpstr_handle_key(unsigned char a1)
{
    switch ((unsigned char)a1) {
    goto L8C89D;
case 13:
    func_0012B3ED();
    return atoi(*(int *)inpstr_text);
case 131:
    *(short *)inpstr_cursor = 0;
    goto L8C9BE;
case 130:
    *(short *)inpstr_cursor = func_000A0DF4(*(int *)inpstr_text);
    goto L8C9BE;
case 27:
    func_0012B3ED();
    return 32768;
case 128:
    if (*(short *)inpstr_cursor == 0) goto L8C7BA;
    (*(short *)inpstr_cursor)--;
L8C7BA:;
    goto L8C9BE;
case 129:
    if (((unsigned)((int)(short)*(short *)inpstr_cursor)) >= func_000A0DF4(*(int *)inpstr_text)) goto L8C7DB;
    (*(short *)inpstr_cursor)++;
L8C7DB:;
    goto L8C9BE;
case 8:
    if (*(short *)inpstr_cursor == 0) goto L8C836;
    mc_memcpy((int)(*(char **)inpstr_text + ((int)(short)*(short *)inpstr_cursor)) - 1, (int)(*(char **)inpstr_text + ((int)(short)*(short *)inpstr_cursor)), (int)&*(signed char *)((char *)(func_000A0DF4(*(int *)inpstr_text) - ((int)(short)*(short *)inpstr_cursor)) + 1), (int)D_00176E2C, 217, 4);
    (*(short *)inpstr_cursor)--;
L8C836:;
    goto L8C9BE;
case 127:
    if (((unsigned)((int)(short)*(short *)inpstr_cursor)) >= func_000A0DF4(*(int *)inpstr_text)) goto L8C898;
    mc_memcpy((int)(*(char **)inpstr_text + ((int)(short)*(short *)inpstr_cursor)), (int)&*(signed char *)((char *)(int)(*(char **)inpstr_text + ((int)(short)*(short *)inpstr_cursor)) + 1), (int)&*(signed char *)((char *)(func_000A0DF4(*(int *)inpstr_text) - ((int)(short)*(short *)inpstr_cursor)) + 1), (int)D_00176E2C, 226, 4);
L8C898:;
    goto L8C9BE;
default:
L8C89D:;
    if (((int)(unsigned char)a1) >= 128) goto L8C8BE;
    if (((unsigned)func_000A0DF4(*(int *)inpstr_text)) < ((int)(short)*(short *)inpstr_max_length)) goto L8C8C3;
L8C8BE:;
    goto L8C9BE;
L8C8C3:;
    if (*(signed char *)input_digits_only == 0) goto L8C8E2;
    if (((int)(unsigned char)a1) < 48) goto L8C8E0;
    if (((int)(unsigned char)a1) <= 57) goto L8C8E2;
L8C8E0:;
    goto L8C8E4;
L8C8E2:;
    goto L8C8E9;
L8C8E4:;
    goto L8C9BE;
L8C8E9:;
    if (((font_text_width(*(int *)inpstr_text) + font_char_width((int)(unsigned char)a1)) + ((int)(unsigned short)*(short *)D_00195F36)) >= ((int)(short)*(short *)D_00142948)) goto L8C9BE;
    if (((int)(short)*(short *)inpstr_cursor) != func_000A0DF4(*(int *)inpstr_text)) goto L8C95D;
    *(signed char *)((char *)(int)(*(char **)inpstr_text + ((int)(short)(*(short *)inpstr_cursor)++))) = a1;
    *(signed char *)((char *)(int)(*(char **)inpstr_text + ((int)(short)*(short *)inpstr_cursor))) = 0;
    goto L8C9BE;
L8C95D:;
    mc_memmove((int)(*(char **)inpstr_text + ((int)(short)*(short *)inpstr_cursor)) + 1, (int)(*(char **)inpstr_text + ((int)(short)*(short *)inpstr_cursor)), (int)&*(signed char *)((char *)(func_000A0DF4(*(int *)inpstr_text) - ((int)(short)*(short *)inpstr_cursor)) + 1), (int)D_00176E2C, 245, 4);
    *(signed char *)((char *)(int)(*(char **)inpstr_text + ((int)(short)(*(short *)inpstr_cursor)++))) = a1;
L8C9BE:;
    return -2023406815;
}
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
