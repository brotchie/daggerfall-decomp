/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005CE17 */
#include "records.h"

extern char D_00142950[];
extern char screen_buffer[];
extern char D_00175898[];
extern char D_00185CDC[];
extern char D_001959FC[];
extern char hud_bar_image[];
extern char hud_mode_icons[];
extern char D_00195B78[];
extern struct character *player_character;
extern struct settings *game_settings;
extern char D_00195C7C[];
extern char D_00195C80[];
extern char D_00195C84[];
extern char D_00195D74[];
extern char interaction_mode[];
extern void hud_draw_compass(void);
extern int hud_portrait_overlay_index(void);
extern int mc_memcpy();
extern int func_00144ED8();
extern int func_00144F68();
extern int func_00144FB4();

struct img {
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    short f8;
    unsigned short size;
    char data[1];
};

#define IMG(g) (*(struct img **)(g))
#define BARS ((struct img **)D_00195C7C)

void hud_draw(void)
{
    unsigned l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct img *l_18;

    if ((game_settings->view_flags & 1) == 0) {
        mc_memcpy(*(char **)screen_buffer + ((int *)D_00142950)[IMG(hud_bar_image)->y], IMG(hud_bar_image)->data, IMG(hud_bar_image)->size, D_00175898, 124, 4);
        func_00144ED8(131, 154, 47, 22, *(char **)hud_mode_icons + ((int *)D_00185CDC)[*(unsigned char *)interaction_mode], 0);
        l_1C = hud_portrait_overlay_index();
        if (l_1C != -1) {
            l_18 = *(struct img **)D_00195D74;
            while (l_1C != 0) {
                l_18 = (struct img *)((char *)l_18 + l_18->size + 12);
                l_1C--;
            }
            func_00144F68(l_18->x, l_18->y, l_18->w, l_18->h, l_18->data);
        }
        l_2C = 23 - IMG(D_00195B78)->w / 2;
        l_28 = 176 - IMG(D_00195B78)->h / 2;
        func_00144FB4(l_2C, l_28, IMG(D_00195B78)->w, IMG(D_00195B78)->h, IMG(D_00195B78)->data);
        hud_draw_compass();
        l_20 = 0;
    } else {
        l_20 = -40;
    }
    if (player_character->health > 0) {
        l_24 = ((player_character->health << 8) / player_character->max_health << 5) / 256;
        if (l_24 != 0)
            func_00144F68(l_20 + 49, 32 - l_24 + 161, 4, l_24, BARS[0]->data + (32 - l_24) * 4);
    }
    if (player_character->fatigue > 0) {
        l_30 = (player_character->attributes[ATTR_STR] + player_character->attributes[ATTR_END]) << 6;
        l_24 = ((player_character->fatigue << 8) / l_30 << 5) >> 8;
        if (l_24 != 0)
            func_00144F68(l_20 + 57, 32 - l_24 + 161, 4, l_24, BARS[1]->data + (32 - l_24) * 4);
    }
    if (player_character->magicka + *(int *)D_001959FC > 0) {
        l_24 = (((player_character->magicka + *(int *)D_001959FC) << 8) / player_character->max_magicka << 5) / 256;
        if (l_24 != 0)
            func_00144F68(l_20 + 65, 32 - l_24 + 161, 4, l_24, BARS[2]->data + (32 - l_24) * 4);
    }
}
