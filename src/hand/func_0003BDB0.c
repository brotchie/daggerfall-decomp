/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003BDB0 */
struct ch { char pad[16]; unsigned char attr[8]; };
struct mob { char pad[337]; short level; };
extern char D_00170C67[];
extern char D_00170CC0[];
extern short D_00178A08;
extern char text_buffer[];
extern char D_001903A5;
extern struct mob *player_character;
extern int window_image;
extern struct ch *player_class;
extern char *D_00195C44;
extern char D_00199644;
extern void sheet_draw(void);
extern void sheet_format_skill(char *, short, int);
extern int func_0003C3A8(unsigned char);
extern void msgbox_show_string(char *, int);
extern void paperdoll_draw(int, int);
extern int func_000A0DF4(char *);
extern void func_000A1054(char *, char *, char *, int, int);
extern void func_000CB552(int);
extern void func_000CDD81(int);
extern void func_0012DB50(int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

void sheet_show_career_skills(short a1, short a2)
{
    char *p;
    short x;

    D_00199644 = 0;
    p = D_00195C44 + 55000;
    D_00178A08 = 250;
    *p = 0;
    sheet_format_skill(p, player_class->attr[a1], func_0003C3A8(player_class->attr[a1]));
    sheet_format_skill(p, player_class->attr[a1 + 1], func_0003C3A8(player_class->attr[a1 + 1]));
    sheet_format_skill(p, player_class->attr[a1 + 2], func_0003C3A8(player_class->attr[a1 + 2]));
    if (a2) {
        sheet_format_skill(p, player_class->attr[a1 + 3], func_0003C3A8(player_class->attr[a1 + 3]));
        sheet_format_skill(p, (int)player_class->attr[a1 + 4], func_0003C3A8(player_class->attr[a1 + 4]));
        sheet_format_skill(p, player_class->attr[a1 + 5], func_0003C3A8(player_class->attr[a1 + 5]));
    }
    if (D_00199644) {
        func_000A0ED9(327, D_00170C67);
        mc_sprintf(text_buffer, D_00170CC0, player_character->level / 10 + 1, player_character->level / 5 + 1);
        D_001903A5 = 96;
        func_000A1054(p, text_buffer, D_00170C67, 329, 4);
    }
    p[func_000A0DF4(p) - 1] = 0;
    func_000CB552(window_image);
    paperdoll_draw(0, 0);
    func_0012DB50(4);
    sheet_draw();
    func_000CDD81(1);
    msgbox_show_string(p, 1);
    D_00178A08 = 310;
}
