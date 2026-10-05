/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009063D */
#include "records.h"

#define FREED ((void *)0x97979797)
#define FREE(p, line) if ((p) != 0 && (p) != FREED) { mc_free((p), D_00176F41, (line)); (p) = FREED; }
extern unsigned char D_0012B508;
extern char D_00176F28[];
extern char D_00176F34[];
extern char D_00176F41[];       /* __FILE__ */
extern char D_00176F4C[];
extern char D_00176F5B[];
extern char D_00176F68[];
extern char D_00176F75[];
extern char D_00176F82[];
extern char D_00176F8F[];
extern char D_00176F9C[];
extern char D_00176FA9[];
extern char D_00176FB6[];
extern unsigned char chargen_career_skill_bonus[];
extern signed char text_buffer[];
extern int D_00190BE8;
extern unsigned char D_00190CEE[];
extern short D_00190D64;
extern short chargen_selected_attribute;
extern short D_00190DEA;
extern short text_macro_fe;
extern short D_00190DEE;
extern unsigned char D_001940D5;
extern void *D_00195B5C;
extern void *D_00195B60;
extern struct character *player_character;
extern void *window_image;
extern struct career *player_class;
extern int D_00195C44;
extern void *chargen_face_images;
extern void *chargen_reflex_image;
extern unsigned char chargen_roll_saved;
extern unsigned char chargen_screen;
extern int level_skill_sum(void);
extern void msgbox_update(void);
extern void classmaker_input_text(struct character *, short, int (*)(void));
extern void *disk_read_file(char *, int);
extern int rand_range(int, int);
extern int chargen_draw(void);
extern void chargen_free_images(void);
extern int chargen_screen_loop(int, int);
extern void chargen_select_skill(int);
extern void chargen_roll_attributes(void);
extern void chargen_select_attribute(int);
extern void mc_free(void *, char *, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

int chargen_name_character(void)
{
    int i;
    int unused1;            /* never used, but they have stack slots */
    int unused2;

    chargen_roll_saved = 0;
    for (i = 0; i < 35; i++) {
        if (player_character->skills[i].value == 0)
            player_character->skills[i].value = rand_range(3, 6);
    }
    for (i = 0; i < 12; i++) {
        D_00190CEE[i] = player_character->skills[player_class->skills[i]].value += chargen_career_skill_bonus[i];
    }
    player_character->level = 1;
    disk_read_file(D_00176F28, D_00195C44);
    D_00190DEA = text_macro_fe = D_00190DEE = D_00190D64 = chargen_selected_attribute = 0;
    D_00190BE8 = 0;
    player_character->name[0] = 0;
    D_0012B508 = 146;
    for (i = 0; i < 8; i++) {
        player_character->attributes[i] -= 5;
        player_character->base_attributes[i] -= 5;
    }
    while (player_character->name[0] == 0) {
        chargen_screen = 1;
        window_image = disk_read_file(D_00176F34, 0);
        classmaker_input_text(player_character, 31, chargen_draw);
        FREE(window_image, 114);
    }
    func_000A0ED9(117, D_00176F41);
    mc_sprintf(((char *)text_buffer), D_00176F4C, player_character->flags & 1, player_character->race);
    chargen_face_images = disk_read_file(((char *)text_buffer), 0);
    chargen_screen = 2;
    window_image = disk_read_file(D_00176F5B, 0);
    chargen_screen_loop(33, 34);
    FREE(window_image, 122);
    FREE(chargen_face_images, 123);
    chargen_roll_attributes();
    chargen_screen = 4;
    window_image = disk_read_file(D_00176F68, 0);
    D_00195B5C = disk_read_file(D_00176F75, 0);
    chargen_select_attribute(0);
    chargen_screen_loop(20, 32);
    FREE(window_image, 131);
    chargen_screen = 8;
    window_image = disk_read_file(D_00176F82, 0);
    D_00195B60 = disk_read_file(D_00176F8F, 0);
    D_00190DEA = text_macro_fe = D_00190DEE = 6;
    for (i = 0; i < 12; i++) {
        D_00190CEE[i] = player_character->skills[player_class->skills[i]].value;
    }
    chargen_select_skill(2);
    chargen_select_skill(5);
    chargen_select_skill(8);
    chargen_screen_loop(2, 19);
    FREE(window_image, 142);
    FREE(D_00195B60, 143);
    chargen_screen = 16;
    window_image = disk_read_file(D_00176F9C, 0);
    chargen_reflex_image = disk_read_file(D_00176FA9, 0);
    chargen_screen_loop(35, 39);
    FREE(window_image, 149);
    FREE(chargen_reflex_image, 150);
    D_001940D5 |= 32;
    msgbox_update();
    chargen_screen = 255;
    func_000A0ED9(155, D_00176F41);
    mc_sprintf(((char *)text_buffer), D_00176F4C, player_character->flags & 1, player_character->race);
    chargen_face_images = disk_read_file(((char *)text_buffer), 0);
    D_00195B60 = disk_read_file(D_00176F8F, 0);
    window_image = disk_read_file(D_00176FB6, 0);
    chargen_reflex_image = disk_read_file(D_00176FA9, 0);
    mc_memcpy(player_character->base_attributes, player_character->attributes, 16, D_00176F41, 161, 16);
    player_character->level_skill_sum_start = level_skill_sum();
    if (chargen_screen_loop(0, 39)) {
        chargen_free_images();
        return 1;
    }
    chargen_free_images();
    return 0;
}
