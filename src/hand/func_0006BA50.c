/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006BA50 */
#include "records.h"

struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct ent { unsigned char f0; char pad1[4]; int f5; };
struct slot { struct ent *e; char pad[16]; };      /* 20 bytes */
struct rec { char pad[74]; };
extern char *screen_buffer;
extern char D_00175CC4[];        /* __FILE__ */
extern char D_00175CCB[];
extern char D_00190B44[];
extern signed char text_rsc_buffer[];
extern struct Img *D_00195B5C;
extern struct Img *window_image;
extern int game_minutes;
extern struct slot bank_houses_for_sale[];
extern char *bank_saved_screen;
extern struct rec bank_ships_for_sale[];
extern struct bank_account *bank_account;
extern unsigned char bank_screen;
extern unsigned char bank_selected;
extern void parse_expand(char *, char *);
extern void bank_draw_preview(int, void *);
extern void bank_draw_house_list(void);
extern void bank_draw_ship_list(void);
extern void text_draw_colored(char *, short, short, int, unsigned char);
extern int gold_total(void);
extern char *func_000A0DD9(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern int func_00144F68();

void bank_draw(void)
{
    int saved;
    struct Img *img;

    img = window_image;
    mc_memcpy(screen_buffer, bank_saved_screen, 64000, D_00175CC4, 216, 4);
    func_00144F68(img->x, img->y, img->w, img->h, img->data);
    switch (bank_screen) {
    case 0:
        text_draw_colored(func_000A0DD9(bank_account->balance, ((char *)text_rsc_buffer), 10), 197, 19, 145, 156);
        text_draw_colored(func_000A0DD9(gold_total(), ((char *)text_rsc_buffer), 10), 203, 29, 145, 156);
        if (bank_account->loan_due != 0) {
            text_draw_colored(func_000A0DD9(bank_account->loan_owed, ((char *)text_rsc_buffer), 10), 143, 39, 145, 156);
            saved = game_minutes;
            game_minutes = bank_account->loan_due;
            parse_expand(D_00175CCB, D_00190B44);
            text_draw_colored(D_00190B44, 119, 49, 145, 156);
            game_minutes = saved;
        }
        break;
    case 1:
        func_00144F68(D_00195B5C->x, D_00195B5C->y, D_00195B5C->w, D_00195B5C->h, D_00195B5C->data);
        bank_draw_house_list();
        bank_draw_preview(bank_houses_for_sale[bank_selected].e->f0, (void *)bank_houses_for_sale[bank_selected].e->f5);
        break;
    case 2:
        func_00144F68(D_00195B5C->x, D_00195B5C->y, D_00195B5C->w, D_00195B5C->h, D_00195B5C->data);
        bank_draw_ship_list();
        bank_draw_preview(1, &bank_ships_for_sale[bank_selected]);
        break;
    }
}
