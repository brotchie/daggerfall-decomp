/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002B5EF */
#include "records.h"

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
extern struct record *player_object;
extern char game_minutes[];
extern char game_mode[];
extern char quest_debug_object[];
extern char D_00196DC0[];
extern char D_00196DC1[];
extern char quest_debug_data[];
extern char cheat_mode[];
extern void quest_debug_next(void);
extern void *quest_section(struct quest *, int);
extern void text_draw(char *, int, int);
extern int key_pressed_once(unsigned char);
extern int func_0012DB50();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int mc_sprintf(char *, ...);

#pragma pack(1)
struct Ent { char name[20]; unsigned char type; char pad[2]; char *data; };   /* a text variable (section 10) */
#pragma pack()

#define COLOR (*(unsigned char *)D_0012B508)
#define LINEH (*(short *)D_0012DA44)
#define CUR (*(struct quest **)quest_debug_data)
#define NEXTLINE() \
    text_draw(text_buffer, l_34 >= l_30 ? 160 : 0, l_34 % l_30 * LINEH); \
    l_34++

void quest_debug_overlay(void)
{
    struct Ent *l_44;
    struct record *l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    struct qbn_person *l_2C;
    struct qbn_place *l_28;
    struct qbn_foe *l_24;
    struct qbn_item *l_20;
    struct qbn_state *l_1C;
    struct qbn_timer *l_18;

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
    if (*(int *)((char *)CUR + 56)) {
        l_44 = (struct Ent *)(*(int *)((char *)CUR + 56) + (char *)CUR);
        while (l_44->name[0] != 0) {
            if (l_44->type == *D_00196DC1) {
                COLOR = 145;
                l_40 = 0;
                switch (l_44->type) {
                case 3:
                    l_2C = (struct qbn_person *)l_44->data;
                    l_40 = l_2C->object;
                    break;
                case 4:
                    l_28 = (struct qbn_place *)l_44->data;
                    l_40 = l_28->object;
                    break;
                case 7:
                    l_24 = (struct qbn_foe *)l_44->data;
                    l_40 = l_24->object;
                    break;
                case 0:
                    l_20 = (struct qbn_item *)l_44->data;
                    l_40 = l_20->object;
                    break;
                case 9:
                    l_1C = (struct qbn_state *)l_44->data;
                    if (l_1C->is_global)
                        l_3C = ((unsigned char *)quest_global_states)[l_1C->value];
                    else
                        l_3C = l_1C->value;
                    if (l_3C) COLOR = 240;
                    func_000A0ED9(888, D_001707F0);
                    mc_sprintf(text_buffer, D_00170819, l_44, l_3C ? D_0017080E : D_00170813);
                    break;
                case 6:
                    l_18 = (struct qbn_timer *)l_44->data;
                    func_000A0ED9(892, D_001707F0);
                    mc_sprintf(text_buffer, D_00170823, l_44, *(int *)game_minutes - l_18->start, l_18->delay);
                    break;
                }
                if (l_40) {
                    if (l_40->twin) {
                        l_40 = l_40->twin;
                        COLOR = 240;
                        func_000A0ED9(904, D_001707F0);
                        mc_sprintf(text_buffer, D_00170832, l_44, l_40->x, l_40->y, l_40->z);
                    } else {
                        func_000A0ED9(907, D_001707F0);
                        mc_sprintf(text_buffer, D_00170832, l_44, l_40->x, l_40->y, l_40->z);
                    }
                }
                NEXTLINE();
            }
            l_44++;
        }
    } else {
        int l_58;

        l_1C = quest_section(CUR, 9);
        for (l_58 = 0; l_58 < CUR->section_counts[9]; l_58++, l_1C++) {
            if (l_1C->is_global) {
                l_3C = ((unsigned char *)quest_global_states)[l_1C->value];
                l_38 = l_1C->value;
            } else {
                l_38 = l_1C->name_hash;
                l_3C = l_1C->value;
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
    mc_sprintf(text_buffer, D_00170856, player_object->x, player_object->y, player_object->z);
    NEXTLINE();
    func_000A0ED9(945, D_001707F0);
    mc_sprintf(text_buffer, D_0017086D, CUR->name);
    text_draw(text_buffer, l_34 >= l_30 ? 160 : 0, l_34 % l_30 * LINEH);
    func_0012DB50(4);
}
