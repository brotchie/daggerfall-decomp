/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004D402 */
#include "structs.h"
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern short font_height;
extern signed char key_down_esc;
extern signed char key_down_enter;
extern short D_00142928;
extern short D_0014292C;
extern char D_00174FAC[];
extern struct rect note_buttons[];
extern char D_001851F0[];
extern char note_colour[];
extern signed char D_00185201[];
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern signed char D_001940D5;
extern char inpstr_result[];
extern iptr window_image;
extern unsigned short text_cursor_y;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern struct rect D_001997B0;      /* the selection box */
extern iptr note_rci;
extern union note_entry *note_selected;
extern char note_font[];
extern char note_text_flags[];
extern short note_cursor_y;
extern short note_page_index;
extern short note_cursor_x;
extern signed char note_tool;
extern signed char D_001997EB;
extern signed char note_action;
extern int note_open_notebook(short);
extern int note_close(void);
extern void note_click_page(void);
extern void note_draw_page(void);
extern void note_add_text(char *);
extern void note_goto_page(int);
extern void note_prev_page(void);
extern void note_next_page(void);
extern void note_find(void);
extern void note_delete_in_box(void);
extern void note_delete_selected(void);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
extern int key_pressed_once(unsigned char);
extern void inpstr_begin_text(char *, int);
extern int inpstr_update(void);
extern int mc_memset();
extern int itoa();
extern int xn_draw_fullscreen_overlay_shaded();
extern int xn_font_select();
extern int xn_draw_image();
extern int xn_draw_image_transparent();
extern int xn_draw_line_to();

void note_update(void)
{
    int unused1;
    int i;
    int unused2;

    if (note_open_notebook(0) == 0) return;
    xn_draw_fullscreen_overlay_shaded(window_image);
    xn_font_select(4);
    xn_draw_image(182, 176, 44, 9, (iptr)(*(char **)&note_rci + (((int)(unsigned char)note_tool) * 396)));
    xn_draw_image(226, 176, 44, 9, (iptr)(*(char **)&note_rci + ((((int)(short)*(short *)note_font) * 396) + 1584)));
    xn_draw_image_transparent((((int)(unsigned char)D_001997EB) << 3) + 53, 185, 5, 3, (iptr)D_001851F0);
    D_0012B508 = 145;
    text_draw_centred_coloured(itoa((((int)(short)note_page_index) + 1), (iptr)text_buffer, 10), 303, 4, 145, 141);
    note_find();
    note_draw_page();
    if (((int)(unsigned char)note_action) == 1) {
        D_0012B508 = *(signed char *)note_colour;
        xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(short)*(short *)note_font]));
        if (inpstr_update() != 0) {
            if (text_rsc_buffer[0] != 0 && ((int)(short)(*(short *)note_text_flags & 32)) == 0) {
                note_add_text(text_rsc_buffer);
            }
            if (text_rsc_buffer[0] == 0 && ((int)(short)(*(short *)note_text_flags & 32)) != 0) {
                note_selected->text.flags |= 64;
                note_delete_selected();
            }
            *(signed char *)note_text_flags &= 223;
            if (key_down_enter != 0 && text_rsc_buffer[0] != 0) {
                note_cursor_y += font_height;
                text_cursor_y = note_cursor_y;
                mc_memset((iptr)text_rsc_buffer, 0, 81, (iptr)D_00174FAC, 137, 2048);
                inpstr_begin_text(text_rsc_buffer, 79);
            } else {
                D_001940D5 &= 251;
                note_action = 0;
            }
        }
    } else if (((int)(unsigned char)note_action) == 2) {
        D_0012B508 = *(signed char *)note_colour;
        D_00142928 = note_cursor_x;
        D_0014292C = note_cursor_y;
        xn_draw_line_to((int)(short)mouse_x, (int)(short)mouse_y);
    } else if (((int)(unsigned char)note_action) == 3) {
        if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
            D_00142928 = D_001997B0.x0;
            D_0014292C = D_001997B0.y0;
            xn_draw_line_to((int)(short)mouse_x, (int)(short)D_001997B0.y0);
            xn_draw_line_to((int)(short)mouse_x, (int)(short)mouse_y);
            xn_draw_line_to((int)(short)D_001997B0.x0, (int)(short)mouse_y);
            xn_draw_line_to((int)(short)D_001997B0.x0, (int)(short)D_001997B0.y0);
        } else {
            D_001997B0.x1 = mouse_x;
            D_001997B0.y1 = mouse_y;
            if (D_001997B0.x0 > D_001997B0.x1) {
                D_001997B0.x0 ^= D_001997B0.x1;
                D_001997B0.x1 ^= D_001997B0.x0;
                D_001997B0.x0 ^= D_001997B0.x1;
            }
            if (D_001997B0.y0 > D_001997B0.y1) {
                D_001997B0.y0 ^= D_001997B0.y1;
                D_001997B0.y1 ^= D_001997B0.y0;
                D_001997B0.y0 ^= D_001997B0.y1;
            }
            note_delete_in_box();
            note_action = 0;
        }
    } else if (((int)(unsigned char)note_action) == 4) {
        if (((int)(unsigned char)game_mode) != 8) {
            note_action = 0;
            note_goto_page((int)(short)(*(short *)inpstr_result - 1));
        }
    }
    D_0012B508 = 146;
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    if (key_pressed_once(72) != 0 || key_pressed_once(73) != 0) note_prev_page();
    if (key_pressed_once(80) != 0 || key_pressed_once(81) != 0) note_next_page();
    if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && ((int)(short)mouse_y) > 10 && ((int)(short)mouse_y) < 173) {
        note_click_page();
    } else if (note_action == 0 && ((int)(unsigned char)(mouse_buttons & 1)) != 0) {
        for (i = 0; ((int)(short)*(short *)&i) < 14; i++) {
            if (mouse_x > note_buttons[(int)(short)*(short *)&i].x0 && mouse_x < note_buttons[(int)(short)*(short *)&i].x1 && mouse_y > note_buttons[(int)(short)*(short *)&i].y0 && mouse_y < note_buttons[(int)(short)*(short *)&i].y1) {
                note_buttons[(int)(short)*(short *)&i].handler();
            }
        }
    }
    if (key_down_esc == 0) return;
    note_close();
}
