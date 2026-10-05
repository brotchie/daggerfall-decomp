/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002B5EF */
extern char D_0012B508[];
extern char D_0012DA44[];
extern char D_001707F0[];
extern char D_0017080E[];
extern char D_00170813[];
extern char D_00170819[];
extern char D_00170823[];
extern char D_00170832[];
extern char D_00170847[];
extern char D_00170856[];
extern char D_0017086D[];
extern char D_0017A134[];
extern char text_buffer[];
extern char quest_global_states[];
extern char player_object[];
extern char game_minutes[];
extern char game_mode[];
extern char quest_debug_object[];
extern char D_00196DC0[];
extern char D_00196DC1[];
extern char quest_debug_data[];
extern char cheat_mode[];
extern void quest_debug_next(void);
extern char *quest_section(char *, int);
extern void text_draw(char *, int, int);
extern int key_pressed_once(unsigned char);
extern int func_0012DB50();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int mc_sprintf(char *, ...);

#pragma pack(1)
struct Rec { char b0, b1; unsigned char flag, idx; int val; };
struct Ent { char name[20]; unsigned char type; char pad[2]; char *data; };
#pragma pack()

#define COLOR (*(unsigned char *)D_0012B508)
#define LINEH (*(short *)D_0012DA44)
#define CUR (*(char **)quest_debug_data)
#define NEXTLINE() \
    text_draw(text_buffer, l_34 >= l_30 ? 160 : 0, l_34 % l_30 * LINEH); \
    l_34++

void quest_debug_overlay(void)
{
    struct Ent *l_44;
    char *l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    char *l_2C;
    char *l_28;
    char *l_24;
    char *l_20;
    struct Rec *l_1C;
    char *l_18;

    if (*cheat_mode == 0) return;
    if (*(unsigned char *)game_mode == 4) return;
    if (key_pressed_once(39)) {
        *(unsigned char *)D_00196DC0 = (*(unsigned char *)D_00196DC0 + 1) % 6;
        *D_00196DC1 = D_0017A134[*(unsigned char *)D_00196DC0];
    }
    if (key_pressed_once(40)) quest_debug_next();
    if (key_pressed_once(67)) {
        if (*(int *)quest_debug_object) {
            *(int *)quest_debug_object = 0;
            CUR = 0;
            return;
        }
        quest_debug_next();
    }
    if (*(int *)quest_debug_object == 0 || CUR == 0) return;
    func_0012DB50(3);
    l_30 = 200 / LINEH;
    l_34 = 0;
    if (*(int *)(CUR + 56)) {
        l_44 = (struct Ent *)(*(int *)(CUR + 56) + CUR);
        while (l_44->name[0] != 0) {
            if (l_44->type == *D_00196DC1) {
                COLOR = 145;
                l_40 = 0;
                switch (l_44->type) {
                case 3:
                    l_2C = l_44->data;
                    l_40 = *(char **)(l_2C + 12);
                    break;
                case 4:
                    l_28 = l_44->data;
                    l_40 = *(char **)(l_28 + 16);
                    break;
                case 7:
                    l_24 = l_44->data;
                    l_40 = *(char **)(l_24 + 10);
                    break;
                case 0:
                    l_20 = l_44->data;
                    l_40 = *(char **)(l_20 + 11);
                    break;
                case 9:
                    l_1C = (struct Rec *)l_44->data;
                    if (l_1C->flag)
                        l_3C = ((unsigned char *)quest_global_states)[l_1C->idx];
                    else
                        l_3C = l_1C->idx;
                    if (l_3C) COLOR = 240;
                    func_000A0ED9(888, D_001707F0);
                    mc_sprintf(text_buffer, D_00170819, l_44, l_3C ? D_0017080E : D_00170813);
                    break;
                case 6:
                    l_18 = l_44->data;
                    func_000A0ED9(892, D_001707F0);
                    mc_sprintf(text_buffer, D_00170823, l_44, *(int *)game_minutes - *(int *)(l_18 + 13), *(int *)(l_18 + 17));
                    break;
                }
                if (l_40) {
                    if (*(char **)(l_40 + 51)) {
                        l_40 = *(char **)(l_40 + 51);
                        COLOR = 240;
                        func_000A0ED9(904, D_001707F0);
                        mc_sprintf(text_buffer, D_00170832, l_44, *(int *)(l_40 + 7), *(int *)(l_40 + 11), *(int *)(l_40 + 15));
                    } else {
                        func_000A0ED9(907, D_001707F0);
                        mc_sprintf(text_buffer, D_00170832, l_44, *(int *)(l_40 + 7), *(int *)(l_40 + 11), *(int *)(l_40 + 15));
                    }
                }
                NEXTLINE();
            }
            l_44++;
        }
    } else {
        int l_58;

        l_1C = (struct Rec *)quest_section(CUR, 9);
        for (l_58 = 0; l_58 < *(short *)(CUR + 34); l_58++, l_1C++) {
            if (l_1C->flag) {
                l_3C = ((unsigned char *)quest_global_states)[l_1C->idx];
                l_38 = l_1C->idx;
            } else {
                l_38 = l_1C->val;
                l_3C = l_1C->idx;
            }
            if (l_3C) COLOR = 240;
            else COLOR = 145;
            func_000A0ED9(935, D_001707F0);
            mc_sprintf(text_buffer, D_00170847, l_38, l_3C ? D_0017080E : D_00170813);
            NEXTLINE();
        }
    }
    COLOR = 112;
    func_000A0ED9(942, D_001707F0);
    mc_sprintf(text_buffer, D_00170856, *(int *)(*(char **)player_object + 7), *(int *)(*(char **)player_object + 11), *(int *)(*(char **)player_object + 15));
    NEXTLINE();
    func_000A0ED9(945, D_001707F0);
    mc_sprintf(text_buffer, D_0017086D, CUR + 6);
    text_draw(text_buffer, l_34 >= l_30 ? 160 : 0, l_34 % l_30 * LINEH);
    func_0012DB50(4);
}
