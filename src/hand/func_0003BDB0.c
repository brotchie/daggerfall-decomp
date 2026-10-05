/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003BDB0 */
#include "records.h"

extern char D_00170C67[];
extern char D_00170CC0[];
extern short msgbox_wrap_width;
extern signed char text_buffer[];
extern char D_001903A5;
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern char *scratch_buffer;
extern char sheet_hth_damage_line;
extern void sheet_draw(void);
extern void sheet_format_skill(char *, short, int);
extern int skill_raised_recently(int);
extern void msgbox_show_string(char *, short);
extern void paperdoll_draw(int, int);
extern int strlen(char *);
extern void func_000A1054(char *, char *, char *, int, int);
extern void xn_draw_fullscreen_overlay_shaded(int);
extern void xn_gfx_present_inclusive(int);
extern void xn_font_select(int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void sheet_show_career_skills(short first, short all_six)
{
    char *text;
    short unused;

    sheet_hth_damage_line = 0;
    text = scratch_buffer + 55000;
    msgbox_wrap_width = 250;
    *text = 0;
    sheet_format_skill(text, player_class->skills[first], skill_raised_recently(player_class->skills[first]));
    sheet_format_skill(text, player_class->skills[first + 1], skill_raised_recently(player_class->skills[first + 1]));
    sheet_format_skill(text, player_class->skills[first + 2], skill_raised_recently(player_class->skills[first + 2]));
    if (all_six) {
        sheet_format_skill(text, player_class->skills[first + 3], skill_raised_recently(player_class->skills[first + 3]));
        sheet_format_skill(text, (int)player_class->skills[first + 4], skill_raised_recently(player_class->skills[first + 4]));
        sheet_format_skill(text, player_class->skills[first + 5], skill_raised_recently(player_class->skills[first + 5]));
    }
    if (sheet_hth_damage_line) {
        mc_set_location(327, D_00170C67);
        mc_sprintf(((char *)text_buffer), D_00170CC0, player_character->skills[30].value / 10 + 1, player_character->skills[30].value / 5 + 1);
        D_001903A5 = 96;
        func_000A1054(text, ((char *)text_buffer), D_00170C67, 329, 4);
    }
    text[strlen(text) - 1] = 0;
    xn_draw_fullscreen_overlay_shaded(window_image);
    paperdoll_draw(0, 0);
    xn_font_select(4);
    sheet_draw();
    xn_gfx_present_inclusive(1);
    msgbox_show_string(text, 1);
    msgbox_wrap_width = 310;
}
