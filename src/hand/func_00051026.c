/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00051026 */
#include "records.h"

struct anims {
    char *a;
    char *b;
};
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
extern unsigned char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern unsigned char key_down_ctrl;
extern unsigned char key_down_lshift;
extern unsigned char key_down_x;
extern unsigned char key_down_up;
extern unsigned char key_down_down;
extern char *screen_buffer;
extern struct bits8 D_00147964;
extern char D_0017539B[];        /* __FILE__ */
extern char D_001753A6[];
extern char D_001753B3[];
extern char D_001753C0[];
extern char D_001753CD[];
extern unsigned char D_0018520B[][3];
extern unsigned char D_00185284[];
extern unsigned char D_0018528E[];
extern unsigned char D_00185291[];
extern signed char text_buffer[];
extern short D_00190D68;
extern struct career *player_class;
extern char *D_00195C44;
extern unsigned char mouse_buttons_prev;
extern char class_questions_asked[];
extern unsigned char class_answer_counts[];
extern int chargen_popup_choice(short, short, short, char *, unsigned char, unsigned char);
extern void palette_restore(void);
extern void game_exit(int);
extern void class_question_show(struct anims *);
extern void class_question_scroll(struct anims *, int);
extern void class_question_answer_anim(short);
extern short class_question_get_answer(void);
extern int class_question_pick_class(void);
extern char *disk_read_file(char *, int);
extern void mc_free(char *, char *, int);
extern void mc_memset(void *, int, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);
extern void mc_memcpy(void *, void *, int, char *, int, int);
extern void func_000CD33A(unsigned char *, int, int);
extern void func_000CD367(char *);
extern void func_0012B136(void);
extern void func_0012B2D3(short, short);
extern void func_00143914(int);

int class_questions_run(void)
{
    short sel;
    short tries;
    short done;
    struct anims h;
    short unused;
    unsigned char rgb[4];

    tries = 10;
    mc_memset(screen_buffer, 0, 64000, D_0017539B, 79, 4);
    mc_memcpy((void *)0xa0000, screen_buffer, 64000, D_0017539B, 80, 4);
    mc_memset(class_questions_asked, 0, 10, D_0017539B, 81, 10);
    mc_memset(class_answer_counts, 0, 3, D_0017539B, 82, 3);
    rgb[0] = rgb[1] = rgb[2] = 0;
    D_00147964.b0 = 0;
    disk_read_file(D_001753A6, (int)D_00195C44);
    for (sel = 0; sel < 768; sel++)
        (sel + D_00195C44)[64000] <<= 2;
    func_000CD367(D_00195C44 + 64000);
    mc_memcpy(screen_buffer, D_00195C44, 64000, D_0017539B, 90, 4);
    h.a = disk_read_file(D_001753B3, 0);
    h.b = disk_read_file(D_001753C0, 0);
    while (tries-- != 0) {
        done = 0;
        D_00147964.b0 = 0;
        class_question_show(&h);
        while (done == 0) {
            if (key_down_ctrl && key_down_x && key_down_lshift)
                game_exit(0);
            mouse_buttons_prev = mouse_buttons;
            func_0012B136();
            func_0012B2D3(mouse_x, mouse_y);
            if ((mouse_buttons & 1) && (mouse_x > 0 && mouse_x < 320 && mouse_y > 120 && mouse_y < 140) || key_down_up)
                class_question_scroll(&h, -1);
            else if ((mouse_buttons & 1) && (mouse_x > 0 && mouse_x < 320 && mouse_y > 180 && mouse_y < 200) || key_down_down)
                class_question_scroll(&h, 1);
            sel = class_question_get_answer();
            if (sel != 0) {
                sel = D_0018520B[D_00190D68 - 1][sel];
                if (class_answer_counts[sel] < 10)
                    class_answer_counts[sel]++;
                rgb[2] = D_00185284[class_answer_counts[sel]] << 2;
                class_question_answer_anim(sel);
                func_000CD33A(rgb, D_0018528E[sel], 1);
                done = 1;
            }
            mc_memcpy((void *)0xa0000, screen_buffer, 64000, D_0017539B, 122, 4);
        }
    }
    func_00143914(0);
    mc_memcpy((void *)0xa0000, screen_buffer, 64000, D_0017539B, 127, 4);
    palette_restore();
    sel = D_00185291[class_question_pick_class()];
    if (chargen_popup_choice(sel + 2100, 4, 5, 0, 21, 49) != 0)
        sel = -1;
    if (sel != -1) {
        func_000A0ED9(134, D_0017539B);
        mc_sprintf(((char *)text_buffer), D_001753CD, sel);
        disk_read_file(((char *)text_buffer), (int)player_class);
    }
    if (h.a != 0 && h.a != (char *)0x97979797) {
        mc_free(h.a, D_0017539B, 138);
        h.a = (char *)0x97979797;
    }
    if (h.b != 0 && h.b != (char *)0x97979797) {
        mc_free(h.b, D_0017539B, 139);
        h.b = (char *)0x97979797;
    }
    mc_memset(screen_buffer, 0, 64000, D_0017539B, 140, 4);
    mc_memcpy((void *)0xa0000, screen_buffer, 64000, D_0017539B, 141, 4);
    palette_restore();
    return sel;
}
