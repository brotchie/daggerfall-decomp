/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00058E15 */
#include "records.h"

#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
#pragma pack()
extern int xn_paperdoll_background;
extern int screen_buffer;
extern int D_00147954;
extern char D_0017573C[];
extern char D_00175743[];
extern char D_00175750[];
extern char D_0017575D[];
extern char D_0017576C[];
extern char D_0017577C[];
extern char D_0017578A[];
extern char D_00175797[];
extern signed char text_buffer[];
extern struct bits8 D_001940D8;
extern struct record *player_entity;
extern int D_00195B64;
extern char *paperdoll_mask;
extern char *hud_portrait;
extern int D_00195B80;
extern struct character *player_character;
extern struct settings *game_settings;
extern char *scratch_buffer;
extern char *paperdoll_slots;
extern char *paperdoll_items;
extern struct item D_00199B54[];
extern int color_remap_tables;
extern void character_update_armor_values(struct record *);
extern void paperdoll_add_item(struct item *, int);
extern void paperdoll_draw_items(int, int);
extern void func_0005E7FC(char *);
extern struct image *disk_read_file(char *, int);
extern void mc_free(void *, char *, int);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
extern void xn_draw_get_rect(int, int, int, int, int, int);
extern void xn_draw_image(int, int, int, int, char *);
extern void xn_draw_image_transparent(int, int, int, int, char *);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void paperdoll_draw(int x, int y)
{
    struct image *pic;
    struct image *strip;
    char buf[108];
    int i;
    int n;
    int sub;
    int cnt;
    int found;
    struct item *it;
    int saved;

    found = 0;
    if (!D_001940D8.b3) return;
    D_001940D8.b3 = 0;
    if (player_character->race == 9) {
        mc_strncpy(((char *)text_buffer), D_00175743, 160, D_0017573C, 40);
    } else if (player_character->race == 10) {
        mc_strncpy(((char *)text_buffer), D_00175750, 160, D_0017573C, 42);
    } else {
        mc_set_location(44, D_0017573C);
        mc_sprintf(((char *)text_buffer), D_0017575D, player_character->race);
    }
    pic = disk_read_file(((char *)text_buffer), 0);
    xn_draw_image(pic->x + x, pic->y + y, pic->width, pic->height, pic->pixels);
    mc_memset((void *)D_00147954, 0, 64000, D_0017573C, 48, 4);
    saved = screen_buffer;
    screen_buffer = D_00147954;
    xn_draw_image(pic->x + x, pic->y + y, pic->width, pic->height, pic->pixels);
    xn_paperdoll_background = D_00147954;
    screen_buffer = saved;
    if (pic != 0 && pic != (struct image *)0x97979797) {
        mc_free(pic, D_0017573C, 54);
        pic = (struct image *)0x97979797;
    }
    paperdoll_items = scratch_buffer + 64000;
    paperdoll_slots = scratch_buffer + 64500;
    mc_memset(paperdoll_items, 0, 112, D_0017573C, 58, 4);
    mc_memset(paperdoll_mask, 0, 24625, D_0017573C, 59, 4);
    if (player_character->race == 10) {
        mc_strncpy(((char *)text_buffer), D_00175750, 160, D_0017573C, 62);
    } else if (player_character->race == 9) {
        mc_strncpy(((char *)text_buffer), D_00175743, 160, D_0017573C, 64);
    } else if (player_character->race == 8) {
        mc_set_location(66, D_0017573C);
        mc_sprintf(((char *)text_buffer), D_0017576C, (unsigned short)(player_character->flags & 1), player_character->original_race, (game_settings->view_flags & 4) != 0 ? 49 : 48);
    } else {
        mc_set_location(68, D_0017573C);
        mc_sprintf(((char *)text_buffer), D_0017576C, (unsigned short)(player_character->flags & 1), player_character->race, (game_settings->view_flags & 4) != 0 ? 49 : 48);
    }
    pic = disk_read_file(((char *)text_buffer), 0);
    xn_draw_image_transparent(pic->x + x, pic->y + y, pic->width, pic->height, pic->pixels);
    if (pic != 0 && pic != (struct image *)0x97979797) {
        mc_free(pic, D_0017573C, 71);
        pic = (struct image *)0x97979797;
    }
    n = player_character->face;
    if (player_character->race == 9 || player_character->race == 10) {
        n = 0;
        mc_set_location(78, D_0017573C);
        mc_sprintf(((char *)text_buffer), D_0017577C, 1 - (player_character->race - 9));
    } else if (player_character->race == 8) {
        mc_strncpy(((char *)text_buffer), D_0017578A, 160, D_0017573C, 82);
        n = ((unsigned short)player_character->flags & 1) != 0 ? 0 : 8;
        n += player_character->original_race;
    } else {
        mc_set_location(87, D_0017573C);
        mc_sprintf(((char *)text_buffer), D_00175797, (unsigned short)(player_character->flags & 1), player_character->race);
    }
    pic = disk_read_file(((char *)text_buffer), 0);
    strip = pic;
    i = 0;
    while (i < n) {
        pic = (struct image *)(pic->data_size + (char *)pic + 12);
        i++;
    }
    if (player_character->race < 9)
        xn_draw_image_transparent(pic->x + x, pic->y + y, pic->width, pic->height, pic->pixels);
    mc_memcpy(hud_portrait, pic, pic->data_size + 12, D_0017573C, 99, 4);
    if (player_character->race <= 8) {
        cnt = 0;
        for (i = 12; i <= 26; i++) {
            if (player_character->equipped[i] != 0) {
                it = &player_character->equipped[i]->data.item;
                n = it->group;
                sub = it->index;
                if (n == 6 || n == 12 || n == 2 || n == 3) {
                    if (n == 3) {
                        mc_memcpy(&D_00199B54[cnt], &player_character->equipped[i]->data.item, 107, D_0017573C, 121, 4);
                        paperdoll_add_item(&D_00199B54[cnt++], i);
                    } else {
                        paperdoll_add_item(&player_character->equipped[i]->data.item, i);
                    }
                    if (found == 0) {
                        mc_memcpy(buf, &player_character->equipped[i]->data.item, 107, D_0017573C, 128, 4);
                        if ((n == 6 && (sub == 13 || sub == 14)) || (n == 12 && (sub == 9 || sub == 10))) {
                            found = 1;
                            buf[66] = 0;
                            func_0005E7FC(buf);
                            paperdoll_add_item((struct item *)buf, i);
                        }
                    }
                }
            }
        }
        paperdoll_draw_items(x, y);
    }
    if (strip != 0 && strip != (struct image *)0x97979797) {
        mc_free(strip, D_0017573C, 143);
        strip = (struct image *)0x97979797;
    }
    xn_draw_get_rect(x + 192, y + 1, 125, 197, D_00195B64, 0);
    D_00195B80 = color_remap_tables;
    character_update_armor_values(player_entity);
}
