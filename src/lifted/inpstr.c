/* inpstr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char xn_font_current;
extern signed char key_down_home;
extern signed char key_down_left;
extern signed char key_down_right;
extern signed char key_down_end;
extern short xn_gfx_clip_right;
extern char D_00176E2C[];
extern char D_00190B44[];
extern iptr inpstr_text;
extern short text_cursor_x;
extern char inpstr_number_text[];
extern short inpstr_max_length;
extern short inpstr_cursor;
extern signed char input_digits_only;
extern signed char D_001A9AB1;

extern int font_char_width(unsigned char);
extern int font_text_width(iptr);
extern int mc_strncpy();
extern int atoi();
extern int itoa();
extern int strlen();
extern int mc_memmove();
extern int mc_memcpy();
extern int xn_mouse_cursor_draw();
extern int xn_kbd_flush();
extern int xn_kbd_read_key();
extern int xn_draw_get_rect();

int inpstr_read_key(void)
{
    unsigned char key;

    key = xn_kbd_read_key();
    if (key != 0) return (int)(unsigned char)key;
    if (key_down_left != 0) return 128;
    if (key_down_right != 0) return 129;
    if (key_down_home != 0) return 131;
    if (key_down_end != 0) return 130;
    return 0;
}

void inpstr_begin_number(int number)
{
    xn_kbd_flush();
    input_digits_only = 1;
    itoa(number, (iptr)inpstr_number_text, 10);
    inpstr_text = (iptr)inpstr_number_text;
    mc_strncpy((iptr)D_00190B44, inpstr_text, 160, (iptr)D_00176E2C, 110);
    inpstr_cursor = strlen((iptr)inpstr_number_text);
    inpstr_max_length = 8;
    D_001A9AB1 = xn_font_current;
}

int inpstr_handle_key(unsigned char key)
{
    switch ((unsigned char)key) {
    case 13:
        xn_mouse_cursor_draw();
        return atoi(inpstr_text);
    case 131:
        inpstr_cursor = 0;
        break;
    case 130:
        inpstr_cursor = strlen(inpstr_text);
        break;
    case 27:
        xn_mouse_cursor_draw();
        return 32768;
    case 128:
        if (inpstr_cursor != 0) (inpstr_cursor)--;
        break;
    case 129:
        if (((unsigned)((int)(short)inpstr_cursor)) < strlen(inpstr_text)) {
            inpstr_cursor++;
        }
        break;
    case 8:
        if (inpstr_cursor != 0) {
            mc_memcpy((iptr)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)) - 1, (iptr)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)), (int)(iptr)&*(signed char *)((char *)(iptr)(strlen(inpstr_text) - ((int)(short)inpstr_cursor)) + 1), (iptr)D_00176E2C, 217, 4);
            inpstr_cursor--;
        }
        break;
    case 127:
        if (((unsigned)((int)(short)inpstr_cursor)) < strlen(inpstr_text)) {
            mc_memcpy((iptr)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)), (iptr)&*(signed char *)((char *)(iptr)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)) + 1), (int)(iptr)&*(signed char *)((char *)(iptr)(strlen(inpstr_text) - ((int)(short)inpstr_cursor)) + 1), (iptr)D_00176E2C, 226, 4);
        }
        break;
    default:
        if (((int)(unsigned char)key) < 128 && ((unsigned)strlen(inpstr_text)) < ((int)(short)inpstr_max_length)) {
            if (input_digits_only != 0 && (((int)(unsigned char)key) < 48 || ((int)(unsigned char)key) > 57)) {
            } else if (((font_text_width(inpstr_text) + font_char_width((int)(unsigned char)key)) + ((int)(unsigned short)text_cursor_x)) < ((int)(short)xn_gfx_clip_right)) {
                if (((int)(short)inpstr_cursor) == strlen(inpstr_text)) {
                    *(signed char *)((char *)(iptr)(*(char **)&inpstr_text + ((int)(short)(inpstr_cursor)++))) = key;
                    *(signed char *)((char *)(iptr)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor))) = 0;
                } else {
                    mc_memmove((iptr)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)) + 1, (iptr)(*(char **)&inpstr_text + ((int)(short)inpstr_cursor)), (int)(iptr)&*(signed char *)((char *)(iptr)(strlen(inpstr_text) - ((int)(short)inpstr_cursor)) + 1), (iptr)D_00176E2C, 245, 4);
                    *(signed char *)((char *)(iptr)(*(char **)&inpstr_text + ((int)(short)(inpstr_cursor)++))) = key;
                }
            }
        }
    }
    return -2023406815;
}

int inpstr_text_width(char *text, short length)
{
    unsigned char saved_char;
    short width;

    saved_char = text[length];
    text[length] = 0;
    *(int *)&width = font_text_width((iptr)text);
    text[length] = saved_char;
    return width;
}

void picklist_save_background(struct picklist *list)
{
    if (list->framed == 0) return;
    xn_draw_get_rect(list->list_rect.x, list->list_rect.y, list->list_rect.w, list->list_rect.h, list->list_background, 0);
    xn_draw_get_rect(list->bar_rect.x, list->bar_rect.y, list->bar_rect.w, list->bar_rect.h, list->bar_background, 0);
}
