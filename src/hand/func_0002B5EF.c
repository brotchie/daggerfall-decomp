/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002B5EF */
#include "records.h"

extern signed char D_0012B508;
extern short font_height;
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
extern signed char text_buffer[];
extern signed char quest_global_states[];
extern struct record *player_object;
extern int game_minutes;
extern signed char game_mode;
extern iptr quest_debug_object;
extern unsigned char D_00196DC0;
extern char D_00196DC1[];
extern iptr quest_debug_data;
extern signed char cheat_mode;
extern void quest_debug_next(void);
extern void *quest_section(struct quest *, int);
extern void text_draw(char *, int, int);
extern int key_pressed_once(unsigned char);
extern int xn_font_select();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, ...);


#define COLOR (*(unsigned char *)&D_0012B508)
#define LINEH (font_height)
#define CUR (*(struct quest **)&quest_debug_data)
#define NEXTLINE() \
    text_draw(((char *)text_buffer), line >= column_lines ? 160 : 0, line % column_lines * LINEH); \
    line++

void quest_debug_overlay(void)
{
    struct qbn_text_var *text_var;
    struct record *object;
    int state_value;
    int flag_id;
    int line;
    int column_lines;
    struct qbn_person *qbn_person;
    struct qbn_place *qbn_place;
    struct qbn_foe *foe;
    struct qbn_item *qbn_item;
    struct qbn_state *state;
    struct qbn_timer *timer;

    if (*((char *)&cheat_mode) == 0) return;
    if ((unsigned char)game_mode == 4) return;
    if (key_pressed_once(39)) {
        D_00196DC0 = (D_00196DC0 + 1) % 6;
        *D_00196DC1 = D_0017A134[D_00196DC0];
    }
    if (key_pressed_once(40)) quest_debug_next();
    if (key_pressed_once(67)) {
        if (quest_debug_object) {
            quest_debug_object = 0;
            CUR = 0;
            return;
        }
        quest_debug_next();
    }
    if (quest_debug_object == 0 || CUR == 0) return;
    xn_font_select(3);
    column_lines = 200 / LINEH;
    line = 0;
    if (CUR->text_offset) {
        text_var = (struct qbn_text_var *)(CUR->text_offset + (char *)CUR);
        while (text_var->name[0] != 0) {
            if (text_var->section == *D_00196DC1) {
                COLOR = 145;
                object = 0;
                switch (text_var->section) {
                case 3:
                    qbn_person = (struct qbn_person *)text_var->record;
                    object = qbn_person->object;
                    break;
                case 4:
                    qbn_place = (struct qbn_place *)text_var->record;
                    object = qbn_place->object;
                    break;
                case 7:
                    foe = (struct qbn_foe *)text_var->record;
                    object = foe->object;
                    break;
                case 0:
                    qbn_item = (struct qbn_item *)text_var->record;
                    object = qbn_item->object;
                    break;
                case 9:
                    state = (struct qbn_state *)text_var->record;
                    if (state->is_global)
                        state_value = ((unsigned char *)quest_global_states)[state->value];
                    else
                        state_value = state->value;
                    if (state_value) COLOR = 240;
                    mc_set_location(888, D_001707F0);
                    mc_sprintf(((char *)text_buffer), D_00170819, text_var, state_value ? D_0017080E : D_00170813);
                    break;
                case 6:
                    timer = (struct qbn_timer *)text_var->record;
                    mc_set_location(892, D_001707F0);
                    mc_sprintf(((char *)text_buffer), D_00170823, text_var, game_minutes - timer->start, timer->delay);
                    break;
                }
                if (object) {
                    if (object->twin) {
                        object = object->twin;
                        COLOR = 240;
                        mc_set_location(904, D_001707F0);
                        mc_sprintf(((char *)text_buffer), D_00170832, text_var, object->x, object->y, object->z);
                    } else {
                        mc_set_location(907, D_001707F0);
                        mc_sprintf(((char *)text_buffer), D_00170832, text_var, object->x, object->y, object->z);
                    }
                }
                NEXTLINE();
            }
            text_var++;
        }
    } else {
        int i;

        state = quest_section(CUR, 9);
        for (i = 0; i < CUR->section_counts[9]; i++, state++) {
            if (state->is_global) {
                state_value = ((unsigned char *)quest_global_states)[state->value];
                flag_id = state->value;
            } else {
                flag_id = state->name_hash;
                state_value = state->value;
            }
            if (state_value) COLOR = 240;
            else COLOR = 145;
            mc_set_location(935, D_001707F0);
            mc_sprintf(((char *)text_buffer), D_00170847, flag_id, state_value ? D_0017080E : D_00170813);
            NEXTLINE();
        }
    }
    COLOR = 112;
    mc_set_location(942, D_001707F0);
    mc_sprintf(((char *)text_buffer), D_00170856, player_object->x, player_object->y, player_object->z);
    NEXTLINE();
    mc_set_location(945, D_001707F0);
    mc_sprintf(((char *)text_buffer), D_0017086D, CUR->name);
    text_draw(((char *)text_buffer), line >= column_lines ? 160 : 0, line % column_lines * LINEH);
    xn_font_select(4);
}
