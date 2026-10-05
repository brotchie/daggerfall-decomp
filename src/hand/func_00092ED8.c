/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00092ED8 */
#pragma pack(1)
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct Rect { short x0; short y0; short x1; short y1; char pad[4]; };
struct Ply { char pad[367]; int items[12]; };
extern char *screen_buffer;
extern char D_0017704C[];
extern char inv_mode_buttons[];
extern char inv_buttons[];
extern char D_00188569[];
extern char D_001885A5[];
extern signed char text_buffer[];
extern unsigned char player_motion_flags;
extern int wagon_container;
extern char *D_00195B64;
extern struct Ply *player_character;
extern int game_minutes;
extern int D_00195D2C;
extern int trade_mode;
extern int inventory_action;
extern unsigned char current_region;
extern unsigned char inv_right_icon;
extern char *inventory_images;
extern char *D_001AA420;
extern struct Img *D_001AA434;
extern struct Img *D_001AA438;
extern struct Img *D_001AA43C;
extern int inv_left_container;
extern unsigned char D_001AA5F8;
extern unsigned char inv_tab;
extern int holiday_today(int, int);
extern void paperdoll_draw(int, int);
extern void text_draw_colored(char *, short, short, int, unsigned char);
extern int gold_total_alias(void);
extern void inv_blit_rect_from_image(int, char *);
extern void inv_draw_container_icon(int, int);
extern int inv_draw_item_cell(int, short, char *);
extern void inv_draw_left_list(char *);
extern void inv_draw_right_list(char *);
extern void func_00096997(void);
extern int func_00096A14(void);
extern void inv_draw_scroll_arrows(void);
extern int func_00097764(void);
extern void func_000984E0(void);
extern void inv_draw_armor_values(void);
extern char *func_000A0DD9(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern int func_000CE31C();
extern int func_00144F68();

void inventory_draw(void)
{
    int sw;
    struct Rect *r;
    char *src;
    int y;
    int i;
    int unused1;
    int unused2;
    int key;
    int val;
    int unused3;

    mc_memcpy(screen_buffer, inventory_images, 64000, D_0017704C, 553, 4);
    if (trade_mode != 0)
        func_00144F68(D_001AA434->x, D_001AA434->y, D_001AA434->w, D_001AA434->h, D_001AA434->data);
    r = (struct Rect *)(((int)inv_mode_buttons + trade_mode * 84) + inventory_action * 12);
    if (trade_mode == 0) {
        for (y = r->y0; r->y1 >= y; y++)
            mc_memcpy(r->x0 + (screen_buffer + y * 320), D_001AA420 + y * 320 + r->x0, r->x1 - r->x0 + 1, D_0017704C, 566, 4);
    } else {
        src = D_001AA438->data;
        for (y = r->y0; r->y1 >= y; y++)
            mc_memcpy(r->x0 + (screen_buffer + y * 320), (r->x0 - (unsigned short)*(short *)D_001AA438) + (src + D_001AA438->w * (y - D_001AA438->y)), r->x1 - r->x0 + 1, D_0017704C, 574, 4);
    }
    inv_blit_rect_from_image(inv_tab + 41, D_001AA420);
    paperdoll_draw(-147, 0);
    func_000CE31C(D_00195B64 + 1008, screen_buffer + 4209, 111, 184, 125);
    if (trade_mode != 0) {
        func_00144F68(D_001AA43C->x, D_001AA43C->y, D_001AA43C->w, D_001AA43C->h, D_001AA43C->data);
        key = holiday_today(game_minutes, current_region);
        if (key == 43 || ((struct bf8_7_1 *)&player_motion_flags)->f)
            val = 0;
        else
            val = D_00195D2C;
        text_draw_colored(func_000A0DD9(D_00195D2C, ((char *)text_buffer), 10), 77, 15, 145, 156);
        text_draw_colored(func_000A0DD9(gold_total_alias(), ((char *)text_buffer), 10), 107, 15, 145, 156);
    }
    inv_draw_armor_values();
    inv_draw_left_list(D_00188569);
    inv_draw_right_list(D_001885A5);
    for (i = 0; i <= 11; i++) {
        if (player_character->items[i] != 0)
            inv_draw_item_cell(player_character->items[i], i, inv_buttons);
    }
    inv_draw_scroll_arrows();
    if (inv_left_container == wagon_container)
        inv_draw_container_icon(32, D_001AA5F8);
    else
        inv_draw_container_icon(32, inv_right_icon);
    sw = trade_mode;
    switch (sw) {
    case 0:
        break;
    case 3:
        func_00096A14();
        break;
    case 1:
        func_00096997();
        break;
    case 2:
        func_00097764();
        break;
    case 4:
        func_000984E0();
        break;
    }
}
