/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00058E15 */
#include "records.h"

#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct img {
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    unsigned short f8;
    unsigned short size;        /* 10 */
    char data[1];               /* 12 */
};
#pragma pack()
extern int D_000CB24E;
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
extern char text_buffer[];
extern struct bits8 D_001940D8;
extern struct record *player_entity;
extern int D_00195B64;
extern char *D_00195B74;
extern char *D_00195B78;
extern int D_00195B80;
extern struct character *player_character;
extern struct settings *game_settings;
extern char *D_00195C44;
extern char *D_00199B4C;
extern char *D_00199B50;
extern struct item D_00199B54[];
extern int D_001AA600;
extern void character_update_armor_values(struct record *);
extern void func_0005978F(struct item *, int);
extern void func_00059909(int, int);
extern void func_0005E7FC(char *);
extern struct img *disk_read_file(char *, int);
extern void mc_free(void *, char *, int);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
extern void func_00144E84(int, int, int, int, int, int);
extern void func_00144F68(int, int, int, int, char *);
extern void func_00144FB4(int, int, int, int, char *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

void paperdoll_draw(int x, int y)
{
    struct img *pic;
    struct img *strip;
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
        mc_strncpy(text_buffer, D_00175743, 160, D_0017573C, 40);
    } else if (player_character->race == 10) {
        mc_strncpy(text_buffer, D_00175750, 160, D_0017573C, 42);
    } else {
        func_000A0ED9(44, D_0017573C);
        mc_sprintf(text_buffer, D_0017575D, player_character->race);
    }
    pic = disk_read_file(text_buffer, 0);
    func_00144F68(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    mc_memset((void *)D_00147954, 0, 64000, D_0017573C, 48, 4);
    saved = screen_buffer;
    screen_buffer = D_00147954;
    func_00144F68(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    D_000CB24E = D_00147954;
    screen_buffer = saved;
    if (pic != 0 && pic != (struct img *)0x97979797) {
        mc_free(pic, D_0017573C, 54);
        pic = (struct img *)0x97979797;
    }
    D_00199B50 = D_00195C44 + 64000;
    D_00199B4C = D_00195C44 + 64500;
    mc_memset(D_00199B50, 0, 112, D_0017573C, 58, 4);
    mc_memset(D_00195B74, 0, 24625, D_0017573C, 59, 4);
    if (player_character->race == 10) {
        mc_strncpy(text_buffer, D_00175750, 160, D_0017573C, 62);
    } else if (player_character->race == 9) {
        mc_strncpy(text_buffer, D_00175743, 160, D_0017573C, 64);
    } else if (player_character->race == 8) {
        func_000A0ED9(66, D_0017573C);
        mc_sprintf(text_buffer, D_0017576C, (unsigned short)(player_character->flags & 1), player_character->original_race, (game_settings->view_flags & 4) != 0 ? 49 : 48);
    } else {
        func_000A0ED9(68, D_0017573C);
        mc_sprintf(text_buffer, D_0017576C, (unsigned short)(player_character->flags & 1), player_character->race, (game_settings->view_flags & 4) != 0 ? 49 : 48);
    }
    pic = disk_read_file(text_buffer, 0);
    func_00144FB4(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    if (pic != 0 && pic != (struct img *)0x97979797) {
        mc_free(pic, D_0017573C, 71);
        pic = (struct img *)0x97979797;
    }
    n = player_character->face;
    if (player_character->race == 9 || player_character->race == 10) {
        n = 0;
        func_000A0ED9(78, D_0017573C);
        mc_sprintf(text_buffer, D_0017577C, 1 - (player_character->race - 9));
    } else if (player_character->race == 8) {
        mc_strncpy(text_buffer, D_0017578A, 160, D_0017573C, 82);
        n = ((unsigned short)player_character->flags & 1) != 0 ? 0 : 8;
        n += player_character->original_race;
    } else {
        func_000A0ED9(87, D_0017573C);
        mc_sprintf(text_buffer, D_00175797, (unsigned short)(player_character->flags & 1), player_character->race);
    }
    pic = disk_read_file(text_buffer, 0);
    strip = pic;
    i = 0;
    while (i < n) {
        pic = (struct img *)(pic->size + (char *)pic + 12);
        i++;
    }
    if (player_character->race < 9)
        func_00144FB4(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    mc_memcpy(D_00195B78, pic, pic->size + 12, D_0017573C, 99, 4);
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
                        func_0005978F(&D_00199B54[cnt++], i);
                    } else {
                        func_0005978F(&player_character->equipped[i]->data.item, i);
                    }
                    if (found == 0) {
                        mc_memcpy(buf, &player_character->equipped[i]->data.item, 107, D_0017573C, 128, 4);
                        if ((n == 6 && (sub == 13 || sub == 14)) || (n == 12 && (sub == 9 || sub == 10))) {
                            found = 1;
                            buf[66] = 0;
                            func_0005E7FC(buf);
                            func_0005978F((struct item *)buf, i);
                        }
                    }
                }
            }
        }
        func_00059909(x, y);
    }
    if (strip != 0 && strip != (struct img *)0x97979797) {
        mc_free(strip, D_0017573C, 143);
        strip = (struct img *)0x97979797;
    }
    func_00144E84(x + 192, y + 1, 125, 197, D_00195B64, 0);
    D_00195B80 = D_001AA600;
    character_update_armor_values(player_entity);
}
