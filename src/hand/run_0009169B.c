/* matched by the real Watcom C32 10.0a (-d2): a run of generate.c from 0x00090FA1 to 0x0009169B, kept together for its switch table's alignment */
#include "records.h"

extern char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern unsigned char D_0012B508;
extern short font_height;
extern char *screen_buffer;
extern char D_00176F41[];       /* __FILE__ */
extern char D_00176FC3[];
extern char D_00176FC8[];
extern char D_00176FCD[];
extern char D_00176FD2[];
extern char D_00176FD7[];
extern char D_00176FDC[];
extern char skill_names[];
extern struct rect chargen_buttons[];
extern signed char text_buffer[];
extern char D_00190B44[];
extern int scratch_190be8;
extern short scratch_190d64;
extern short scratch_190d6a;
extern short scratch_190de4[];
extern short D_00190DEA[];
extern short scratch_190dec;
extern short D_00190DEE;
extern short scratch_190df0[3];
extern struct image *D_00195B5C;
extern char *D_00195B60;
extern struct character *player_character;
extern struct career *player_class;
extern char mouse_buttons_prev;
extern struct image *chargen_face_images;
extern unsigned char chargen_screen;
extern void msgbox_show_rsc(int, int);
extern void parse_expand(char *, char *);
extern void character_reset_magicka(struct character *, struct career *);
extern void classmaker_input_text(struct character *, short, int (*)(void));
extern void text_draw_coloured(char *, short, short, int, unsigned char);
extern void text_draw_centred_coloured(char *, short, short, int, unsigned char);
extern int chargen_draw(void);
extern char *itoa(int, char *, int);
extern int mc_memcpy();
extern void xn_font_select(int);
extern int xn_draw_image();
extern void xn_draw_image_transparent(int, int, int, int, char *);

int chargen_screen_loop(int first, int last)
{
    int i;
    int x;
    int y;

    for (;;) {
        chargen_draw();
        mc_memcpy(655360, screen_buffer, 64000, D_00176F41, 238, 4);
        if (mouse_buttons != 0 && mouse_buttons_prev == 0 &&
            mouse_x > chargen_buttons[0].x0 && mouse_x < chargen_buttons[0].x1 &&
            mouse_y > chargen_buttons[0].y0 && mouse_y < chargen_buttons[0].y1) {
            if (scratch_190d64 == 0 && D_00190DEA[0] == 0 && scratch_190dec == 0 && D_00190DEE == 0)
                return 0;
            msgbox_show_rsc(14, 1);
        }
        x = mouse_x;
        y = mouse_y;
        for (i = first; i <= last; i++) {
            if (chargen_screen == 255 && i == 35) {
                x += -119;
                y += 53;
            }
            if (mouse_buttons != 0 && chargen_buttons[i].x0 < x && chargen_buttons[i].x1 > x &&
                chargen_buttons[i].y0 < y && chargen_buttons[i].y1 > y)
                chargen_buttons[i].handler(i);
        }
        if (scratch_190be8 != 0)
            return 1;
    }
}

void chargen_reflexes_button(int button)
{
    player_character->reflexes = button - 35;
}

void chargen_draw_face(void)
{
    int k;
    struct image *image;

    image = chargen_face_images;
    k = 0;
    while (player_character->face > k) {
        image = (struct image *)(image->data_size + (char *)image + 12);
        k++;
    }
    k = image->y + 16 + image->height;
    if (k >= 63)
        k = 16 - (k - 63);
    else
        k = 16;
    xn_draw_image_transparent(image->x + 24, image->y + k, image->width, image->height, image->pixels);
}

void chargen_draw_attributes(void)
{
    int i;
    short x;

    xn_draw_image_transparent(44, scratch_190d6a, D_00195B5C->width, D_00195B5C->height, D_00195B5C->pixels);
    D_0012B508 = 146;
    x = (chargen_buttons[20].x0 + chargen_buttons[20].x1) >> 1;
    xn_font_select(4);
    for (i = 0; i < 8; i++) {
        text_draw_centred_coloured(itoa(player_character->attributes[i], ((char *)text_buffer), 10), x, (short)(chargen_buttons[i + 20].y1 - font_height + 1), 145, 141);
    }
    text_draw_centred_coloured(itoa(scratch_190d64, ((char *)text_buffer), 10), 51, (short)(scratch_190d6a + 13 - font_height + 1), 145, 141);
    character_reset_magicka(player_character, player_class);
    if (chargen_screen == 255)
        return;
    parse_expand(D_00176FC3, D_00190B44);
    text_draw_coloured(D_00190B44, 83, 22, 145, 141);
    parse_expand(D_00176FC8, D_00190B44);
    text_draw_coloured(D_00190B44, 103, 32, 145, 141);
    parse_expand(D_00176FCD, D_00190B44);
    text_draw_coloured(D_00190B44, 112, 49, 145, 141);
    parse_expand(D_00176FD2, D_00190B44);
    text_draw_coloured(D_00190B44, 121, 71, 145, 141);
    parse_expand(D_00176FD7, D_00190B44);
    text_draw_coloured(D_00190B44, 97, 93, 145, 141);
    parse_expand(D_00176FDC, D_00190B44);
    text_draw_coloured(D_00190B44, 101, 110, 145, 141);
    text_draw_coloured(D_00190B44, 122, 120, 145, 141);
}

void chargen_draw_skills(void)
{
    int i;
    int skill;

    for (i = 0; i < 3; i++) {
        xn_draw_image(203, scratch_190de4[i], *(unsigned short *)(D_00195B60 + 4), *(unsigned short *)(D_00195B60 + 6), D_00195B60 + 12);
        text_draw_centred_coloured(itoa(D_00190DEA[i], ((char *)text_buffer), 10), 221, scratch_190de4[i] + 8 - font_height + 1, 145, 141);
    }
    for (i = 0; i < 12; i++) {
        skill = player_class->skills[i];
        text_draw_coloured(*(char **)(skill_names + (skill << 2)), chargen_buttons[i + 2].x0 + 2, chargen_buttons[i + 2].y0 + 1, 145, 141);
        text_draw_centred_coloured(itoa(player_character->skills[skill].value, ((char *)text_buffer), 10), 192, chargen_buttons[i + 2].y0 + 1, 145, 141);
    }
}

void chargen_ok_button(void)
{
}

void chargen_name_button(void)
{
    classmaker_input_text(player_character, 31, chargen_draw);
}

void chargen_select_skill(int button)
{
    switch (button) {
    case 2:
    case 3:
    case 4:
        chargen_buttons[15].y0 = chargen_buttons[14].y0 = scratch_190de4[0] = chargen_buttons[button].y0;
        chargen_buttons[15].y1 = chargen_buttons[14].y1 = scratch_190de4[0] + 8;
        scratch_190df0[0] = button - 2;
        break;
    case 5:
    case 6:
    case 7:
        chargen_buttons[17].y0 = chargen_buttons[16].y0 = scratch_190de4[1] = chargen_buttons[button].y0;
        chargen_buttons[17].y1 = chargen_buttons[16].y1 = scratch_190de4[1] + 8;
        scratch_190df0[1] = button - 2;
        break;
    default:
        chargen_buttons[19].y0 = chargen_buttons[18].y0 = scratch_190de4[2] = chargen_buttons[button].y0;
        chargen_buttons[19].y1 = chargen_buttons[18].y1 = scratch_190de4[2] + 8;
        scratch_190df0[2] = button - 2;
        break;
    }
}
