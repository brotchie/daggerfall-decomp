/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00054526 */
struct AB { unsigned char a; unsigned char b; };
extern short mouse_y;
extern short D_0012DA44;
extern int screen_buffer;
extern char D_00175420[];
extern char *classmaker_advantage_names[];
extern char *classmaker_disadvantage_names[];
extern char **classmaker_advantage_sublists[];
extern char **classmaker_disadvantage_sublists[];
extern short classmaker_screen;
extern unsigned char classmaker_special_counts[];
extern short classmaker_special_list;
extern short D_00190D84;
extern int text_macro_fae;
extern char *text_macro_fpa;
extern char *D_00190E00;
extern char **D_00190E0C;
extern struct AB classmaker_specials[][7];
extern void msgbox_show_rsc(int, int);
extern void msgbox_update(void);
extern int classmaker_picklist_wait(void);
extern int classmaker_drop_duplicate_special(short);
extern void classmaker_specials_add(void);
extern void classmaker_specials_picked(void);
extern int classmaker_update_advancement(void);
extern void classmaker_draw_dagger(void);
extern void classmaker_set_advantage(short, int);
extern void classmaker_set_disadvantage(short, int);
extern int classmaker_special_conflicts(short, int, short);
extern void text_draw_colored(char *, short, short, int, unsigned char);
extern int mc_memcpy();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern int func_0012DB50();
extern int func_00144F68();

void classmaker_specials_screen(void)
{
    short n;
    short a;
    short b;
    short y;
    short k;
    short y0;
    short y1;

    func_0012DB50(3);
    if ((short)(classmaker_screen & 15) == 2)
        n = 2;
    else
        n = 0;
    if ((int)(short)(classmaker_screen & 16) != 0)
        n++;
    func_0012B2EB();
    mc_memcpy(screen_buffer, text_macro_fae, 64000, D_00175420, 675, 4);
    func_00144F68(0, 0, *(unsigned short *)(D_00190E00 + 4), *(unsigned short *)(D_00190E00 + 6), D_00190E00 + 12);
    if (n == 2 || n == 3)
        func_00144F68(0, 0, *(unsigned short *)(text_macro_fpa + 4), *(unsigned short *)(text_macro_fpa + 6), text_macro_fpa + 12);
    if ((int)(short)(classmaker_screen & 16) != 0) {
        k = classmaker_picklist_wait();
        if (k == 0) {
            classmaker_special_counts[classmaker_special_list]--;
            classmaker_specials_picked();
            goto done;
        }
        k--;
        n = classmaker_special_counts[classmaker_special_list];
        classmaker_specials[classmaker_special_list][n].a = k;
        classmaker_specials[classmaker_special_list][n].b = 0;
        if ((short)(classmaker_screen & 15) == 2)
            D_00190E0C = classmaker_advantage_sublists[k];
        else
            D_00190E0C = classmaker_disadvantage_sublists[k];
        if (D_00190E0C == 0) {
            if (classmaker_drop_duplicate_special(classmaker_special_list) == 0) {
                if (classmaker_special_list == 0)
                    classmaker_set_advantage(n, 0);
                else
                    classmaker_set_disadvantage(n, 0);
            }
            classmaker_specials_picked();
            goto done;
        }
        classmaker_specials_add();
        k = classmaker_picklist_wait();
        if (k == 0) {
            classmaker_special_counts[classmaker_special_list]--;
            classmaker_specials_picked();
            goto done;
        }
        k--;
        n = classmaker_special_counts[classmaker_special_list];
        classmaker_specials[classmaker_special_list][n].b = k;
        if (classmaker_special_conflicts(classmaker_special_list, classmaker_specials[classmaker_special_list][n].a, k)) {
            classmaker_special_counts[classmaker_special_list]--;
            msgbox_show_rsc(1350, 1);
        } else if (classmaker_drop_duplicate_special(classmaker_special_list) == 0) {
            if (classmaker_special_list == 0)
                classmaker_set_advantage(n, 0);
            else
                classmaker_set_disadvantage(n, 0);
        }
        classmaker_specials_picked();
    } else {
        D_00190D84 = -1;
        y = 36;
        for (n = 0; classmaker_special_counts[classmaker_special_list] > n; n++, y += D_0012DA44 * 2) {
            y0 = y;
            if (classmaker_screen == 2) {
                a = classmaker_specials[0][n].a;
                b = classmaker_specials[0][n].b;
                text_draw_colored(classmaker_advantage_names[a], 10, y, 145, 141);
                if (classmaker_advantage_sublists[a]) {
                    y += D_0012DA44;
                    text_draw_colored(classmaker_advantage_sublists[a][b], 10, y, 145, 141);
                }
            } else {
                a = classmaker_specials[1][n].a;
                b = classmaker_specials[1][n].b;
                text_draw_colored(classmaker_disadvantage_names[a], 10, y, 145, 141);
                if (classmaker_disadvantage_sublists[a]) {
                    y += D_0012DA44;
                    text_draw_colored(classmaker_disadvantage_sublists[a][b], 10, y, 145, 141);
                }
            }
            y1 = y + D_0012DA44;
            if (mouse_y >= y0 && mouse_y <= y1)
                D_00190D84 = n;
        }
    }
done:
    func_0012DB50(4);
    classmaker_update_advancement();
    classmaker_draw_dagger();
    msgbox_update();
    func_0012B3ED();
}
