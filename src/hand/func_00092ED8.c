/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00092ED8 */
#include "records.h"
#include "bitfield.h"
#include "clib.h"
#pragma pack(1)
extern char *screen_buffer;
extern char D_0017704C[];
extern struct rect inv_mode_buttons[][7];   /* 5 trade modes */
extern struct rect inv_buttons[];
extern signed char text_buffer[];
extern unsigned char player_motion_flags;
extern iptr wagon_container;
extern char *D_00195B64;
extern struct character *player_character;
extern int game_minutes;
extern int trade_total;
extern int trade_mode;
extern int inventory_action;
extern unsigned char current_region;
extern unsigned char inv_right_icon;
extern char *inventory_images;
extern char *D_001AA420;
extern struct image *D_001AA434;
extern struct image *D_001AA438;
extern struct image *D_001AA43C;
extern iptr inv_left_container;
extern unsigned char D_001AA5F8;
extern unsigned char inv_tab;
extern int holiday_today(int, int);
extern void paperdoll_draw(int, int);
extern void text_draw_coloured(char *, short, short, int, unsigned char);
extern int gold_total_alias(void);
extern void inv_blit_rect_from_image(int, char *);
extern void inv_draw_container_icon(int, int);
extern int inv_draw_item_cell(struct record *, short, struct rect *);
extern void inv_draw_left_list(struct rect *);
extern void inv_draw_right_list(struct rect *);
extern void trade_total_buy(void);
extern int trade_total_repair(void);
extern void inv_draw_scroll_arrows(void);
extern int trade_total_sell(void);
extern void trade_total_identify(void);
extern void inv_draw_armor_values(void);
extern int xn_draw_copy_rect_stride_bytes(char *, char *, int, int, int);
extern void xn_draw_image(int, int, int, int, char *);

void inventory_draw(void)
{
    int mode;
    struct rect *rect;
    char *src;
    int y;
    int i;
    int unused1;
    int unused2;
    int holiday;
    int shown_total;
    int unused3;

    mc_memcpy(screen_buffer, inventory_images, 64000, D_0017704C, 553, 4);
    if (trade_mode != 0)
        xn_draw_image(D_001AA434->x, D_001AA434->y, D_001AA434->width, D_001AA434->height, D_001AA434->pixels);
    rect = &inv_mode_buttons[trade_mode][inventory_action];
    if (trade_mode == 0) {
        for (y = rect->y0; rect->y1 >= y; y++)
            mc_memcpy(rect->x0 + (screen_buffer + y * 320), D_001AA420 + y * 320 + rect->x0, rect->x1 - rect->x0 + 1, D_0017704C, 566, 4);
    } else {
        src = D_001AA438->pixels;
        for (y = rect->y0; rect->y1 >= y; y++)
            mc_memcpy(rect->x0 + (screen_buffer + y * 320), (rect->x0 - (unsigned short)(short)D_001AA438->x) + (src + D_001AA438->width * (y - D_001AA438->y)), rect->x1 - rect->x0 + 1, D_0017704C, 574, 4);
    }
    inv_blit_rect_from_image(inv_tab + 41, D_001AA420);
    paperdoll_draw(-147, 0);
    xn_draw_copy_rect_stride_bytes(D_00195B64 + 1008, screen_buffer + 4209, 111, 184, 125);
    if (trade_mode != 0) {
        xn_draw_image(D_001AA43C->x, D_001AA43C->y, D_001AA43C->width, D_001AA43C->height, D_001AA43C->pixels);
        holiday = holiday_today(game_minutes, current_region);
        if (holiday == 43 || ((struct bf8_7_1 *)&player_motion_flags)->f)
            shown_total = 0;
        else
            shown_total = trade_total;
        text_draw_coloured(itoa(trade_total, ((char *)text_buffer), 10), 77, 15, 145, 156);
        text_draw_coloured(itoa(gold_total_alias(), ((char *)text_buffer), 10), 107, 15, 145, 156);
    }
    inv_draw_armor_values();
    inv_draw_left_list(&inv_buttons[27]);
    inv_draw_right_list(&inv_buttons[32]);
    for (i = 0; i <= 11; i++) {
        if (player_character->equipped[i] != 0)
            inv_draw_item_cell(player_character->equipped[i], i, inv_buttons);
    }
    inv_draw_scroll_arrows();
    if (inv_left_container == wagon_container)
        inv_draw_container_icon(32, D_001AA5F8);
    else
        inv_draw_container_icon(32, inv_right_icon);
    mode = trade_mode;
    switch (mode) {
    case 0:
        break;
    case 3:
        trade_total_repair();
        break;
    case 1:
        trade_total_buy();
        break;
    case 2:
        trade_total_sell();
        break;
    case 4:
        trade_total_identify();
        break;
    }
}
