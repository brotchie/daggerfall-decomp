/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00054526 */
#include "ptrint.h"
struct AB { unsigned char a; unsigned char b; };
extern short mouse_y;
extern short font_height;
extern iptr screen_buffer;
extern char D_00175420[];
extern char *classmaker_advantage_names[];
extern char *classmaker_disadvantage_names[];
extern char **classmaker_advantage_sublists[];
extern char **classmaker_disadvantage_sublists[];
extern short classmaker_screen;
extern unsigned char classmaker_special_counts[];
extern short classmaker_special_list;
extern short D_00190D84;
extern iptr scratch_190df4;
extern char *scratch_190dfc;
extern char *D_00190E00;
extern char **D_00190E0C;
extern struct AB classmaker_specials[][7];
extern void msgbox_show_rsc(int, int);
extern void msgbox_update(void);
extern int classmaker_picklist_wait(void);
extern int classmaker_drop_duplicate_special(int);
extern void classmaker_specials_add(void);
extern void classmaker_specials_picked(void);
extern int classmaker_update_advancement(void);
extern void classmaker_draw_dagger(void);
extern void classmaker_set_advantage(int, int);
extern void classmaker_set_disadvantage(int, int);
extern int classmaker_special_conflicts(int, int, int);
extern void text_draw_coloured(char *, short, short, int, unsigned char);
extern int mc_memcpy();
extern int xn_mouse_cursor_erase();
extern int xn_mouse_cursor_draw();
extern int xn_font_select();
extern int xn_draw_image();

void classmaker_specials_screen(void)
{
    short index;
    short special;
    short param;
    short y;
    short picked;
    short top;
    short bottom;

    xn_font_select(3);
    if ((short)(classmaker_screen & 15) == 2)
        index = 2;
    else
        index = 0;
    if ((int)(short)(classmaker_screen & 16) != 0)
        index++;
    xn_mouse_cursor_erase();
    mc_memcpy(screen_buffer, scratch_190df4, 64000, D_00175420, 675, 4);
    xn_draw_image(0, 0, *(unsigned short *)(D_00190E00 + 4), *(unsigned short *)(D_00190E00 + 6), D_00190E00 + 12);
    if (index == 2 || index == 3)
        xn_draw_image(0, 0, *(unsigned short *)(scratch_190dfc + 4), *(unsigned short *)(scratch_190dfc + 6), scratch_190dfc + 12);
    if ((int)(short)(classmaker_screen & 16) != 0) {
        picked = classmaker_picklist_wait();
        if (picked == 0) {
            classmaker_special_counts[classmaker_special_list]--;
            classmaker_specials_picked();
            goto done;
        }
        picked--;
        index = classmaker_special_counts[classmaker_special_list];
        classmaker_specials[classmaker_special_list][index].a = picked;
        classmaker_specials[classmaker_special_list][index].b = 0;
        if ((short)(classmaker_screen & 15) == 2)
            D_00190E0C = classmaker_advantage_sublists[picked];
        else
            D_00190E0C = classmaker_disadvantage_sublists[picked];
        if (D_00190E0C == 0) {
            if (classmaker_drop_duplicate_special(classmaker_special_list) == 0) {
                if (classmaker_special_list == 0)
                    classmaker_set_advantage(index, 0);
                else
                    classmaker_set_disadvantage(index, 0);
            }
            classmaker_specials_picked();
            goto done;
        }
        classmaker_specials_add();
        picked = classmaker_picklist_wait();
        if (picked == 0) {
            classmaker_special_counts[classmaker_special_list]--;
            classmaker_specials_picked();
            goto done;
        }
        picked--;
        index = classmaker_special_counts[classmaker_special_list];
        classmaker_specials[classmaker_special_list][index].b = picked;
        if (classmaker_special_conflicts(classmaker_special_list, classmaker_specials[classmaker_special_list][index].a, picked)) {
            classmaker_special_counts[classmaker_special_list]--;
            msgbox_show_rsc(1350, 1);
        } else if (classmaker_drop_duplicate_special(classmaker_special_list) == 0) {
            if (classmaker_special_list == 0)
                classmaker_set_advantage(index, 0);
            else
                classmaker_set_disadvantage(index, 0);
        }
        classmaker_specials_picked();
    } else {
        D_00190D84 = -1;
        y = 36;
        for (index = 0; classmaker_special_counts[classmaker_special_list] > index; index++, y += font_height * 2) {
            top = y;
            if (classmaker_screen == 2) {
                special = classmaker_specials[0][index].a;
                param = classmaker_specials[0][index].b;
                text_draw_coloured(classmaker_advantage_names[special], 10, y, 145, 141);
                if (classmaker_advantage_sublists[special]) {
                    y += font_height;
                    text_draw_coloured(classmaker_advantage_sublists[special][param], 10, y, 145, 141);
                }
            } else {
                special = classmaker_specials[1][index].a;
                param = classmaker_specials[1][index].b;
                text_draw_coloured(classmaker_disadvantage_names[special], 10, y, 145, 141);
                if (classmaker_disadvantage_sublists[special]) {
                    y += font_height;
                    text_draw_coloured(classmaker_disadvantage_sublists[special][param], 10, y, 145, 141);
                }
            }
            bottom = y + font_height;
            if (mouse_y >= top && mouse_y <= bottom)
                D_00190D84 = index;
        }
    }
done:
    xn_font_select(4);
    classmaker_update_advancement();
    classmaker_draw_dagger();
    msgbox_update();
    xn_mouse_cursor_draw();
}
