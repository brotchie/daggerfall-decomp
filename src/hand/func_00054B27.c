/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00054B27 */
extern char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern short D_0012DA44;
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
extern void text_draw_colored(char *, short, short, int, unsigned char);
extern int func_0012B136();

void func_00054B27(void)
{
    short l_24;
    short l_1C;
    short l_20;
    char **l_28;
    short l_18;

    l_28 = D_00190E0C;
    l_20 = 30;
    l_18 = (mouse_buttons != 0 && mouse_buttons != mouse_buttons_prev && mouse_x > 10 && mouse_x < 161) ? 1 : 0;
    D_00190D7A = -1;
    l_1C = 0;
    while (*l_28 != 0) {
        text_draw_colored(*l_28, 10, l_20, 145, 141);
        l_24 = l_20;
        l_20 += D_0012DA44;
        if (l_18 && mouse_y > l_24 && mouse_y < l_20) {
            D_00190D7A = l_1C;
            while (mouse_buttons != 0)
                func_0012B136();
        }
        l_1C++;
        l_28++;
    }
    l_1C = classmaker_special_counts[classmaker_special_list];
    classmaker_specials[classmaker_special_list][l_1C][D_00190D7C] = D_00190D7A;
    if (D_00190D7C == 0)
        classmaker_specials[classmaker_special_list][l_1C][1] = 0;
    if (D_00190D7A != -1 && D_00190D7C != 0) {
        if (classmaker_special_list == 0)
            classmaker_set_advantage(l_1C, 0);
        else
            classmaker_set_disadvantage(l_1C, 0);
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
