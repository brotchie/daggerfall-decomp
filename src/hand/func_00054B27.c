/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00054B27 */
extern char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern short font_height;
extern char **classmaker_advantage_sublists[];
extern char **classmaker_disadvantage_sublists[];
extern short classmaker_screen;
extern short D_00190D7A;
extern short D_00190D7C;
extern unsigned char classmaker_special_counts[];
extern short classmaker_special_list;
extern char **D_00190E0C;
extern char mouse_buttons_prev;
extern char classmaker_specials[][7][2];
extern void classmaker_specials_picked(void);
extern void classmaker_set_advantage(short, int);
extern void classmaker_set_disadvantage(short, int);
extern void text_draw_coloured(char *, short, short, int, unsigned char);
extern int xn_mouse_poll_clamped();

void func_00054B27(void)
{
    short top;
    short row;
    short y;
    char **names;
    short clicked;

    names = D_00190E0C;
    y = 30;
    clicked = (mouse_buttons != 0 && mouse_buttons != mouse_buttons_prev && mouse_x > 10 && mouse_x < 161) ? 1 : 0;
    D_00190D7A = -1;
    row = 0;
    while (*names != 0) {
        text_draw_coloured(*names, 10, y, 145, 141);
        top = y;
        y += font_height;
        if (clicked && mouse_y > top && mouse_y < y) {
            D_00190D7A = row;
            while (mouse_buttons != 0)
                xn_mouse_poll_clamped();
        }
        row++;
        names++;
    }
    row = classmaker_special_counts[classmaker_special_list];
    classmaker_specials[classmaker_special_list][row][D_00190D7C] = D_00190D7A;
    if (D_00190D7C == 0)
        classmaker_specials[classmaker_special_list][row][1] = 0;
    if (D_00190D7A != -1 && D_00190D7C != 0) {
        if (classmaker_special_list == 0)
            classmaker_set_advantage(row, 0);
        else
            classmaker_set_disadvantage(row, 0);
        classmaker_specials_picked();
    }
    if ((int)(short)(classmaker_screen & 16) != 0 && D_00190D7A != -1 && (int)(short)(classmaker_screen & 15) == 2) {
        D_00190E0C = classmaker_advantage_sublists[D_00190D7A];
        D_00190D7C = 1;
        if (D_00190E0C == 0)
            classmaker_specials_picked();
    } else if ((int)(short)(classmaker_screen & 16) != 0 && D_00190D7A != -1) {
        D_00190E0C = classmaker_disadvantage_sublists[D_00190D7A];
        D_00190D7C = 1;
        if (D_00190E0C == 0)
            classmaker_specials_picked();
    }
}
