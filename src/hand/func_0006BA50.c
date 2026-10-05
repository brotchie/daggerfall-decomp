/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006BA50 */
#include "records.h"

extern char *screen_buffer;
extern char D_00175CC4[];        /* __FILE__ */
extern char D_00175CCB[];
extern char D_00190B44[];
extern signed char text_rsc_buffer[];
extern struct image *D_00195B5C;
extern struct image *window_image;
extern int game_minutes;
extern struct house_for_sale bank_houses_for_sale[];
extern char *bank_saved_screen;
extern struct ship_for_sale bank_ships_for_sale[];
extern struct bank_account *bank_account;
extern unsigned char bank_screen;
extern unsigned char bank_selected;
extern void parse_expand(char *, char *);
extern void bank_draw_preview(int, void *);
extern void bank_draw_house_list(void);
extern void bank_draw_ship_list(void);
extern void text_draw_coloured(char *, short, short, int, unsigned char);
extern int gold_total(void);
extern char *itoa(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern int xn_draw_image();

void bank_draw(void)
{
    int saved_minutes;
    struct image *image;

    image = window_image;
    mc_memcpy(screen_buffer, bank_saved_screen, 64000, D_00175CC4, 216, 4);
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    switch (bank_screen) {
    case 0:
        text_draw_coloured(itoa(bank_account->balance, ((char *)text_rsc_buffer), 10), 197, 19, 145, 156);
        text_draw_coloured(itoa(gold_total(), ((char *)text_rsc_buffer), 10), 203, 29, 145, 156);
        if (bank_account->loan_due != 0) {
            text_draw_coloured(itoa(bank_account->loan_owed, ((char *)text_rsc_buffer), 10), 143, 39, 145, 156);
            saved_minutes = game_minutes;
            game_minutes = bank_account->loan_due;
            parse_expand(D_00175CCB, D_00190B44);
            text_draw_coloured(D_00190B44, 119, 49, 145, 156);
            game_minutes = saved_minutes;
        }
        break;
    case 1:
        xn_draw_image(D_00195B5C->x, D_00195B5C->y, D_00195B5C->width, D_00195B5C->height, D_00195B5C->pixels);
        bank_draw_house_list();
        bank_draw_preview(bank_houses_for_sale[bank_selected].block->model_count, bank_houses_for_sale[bank_selected].block->models);
        break;
    case 2:
        xn_draw_image(D_00195B5C->x, D_00195B5C->y, D_00195B5C->width, D_00195B5C->height, D_00195B5C->pixels);
        bank_draw_ship_list();
        bank_draw_preview(1, &bank_ships_for_sale[bank_selected].model);
        break;
    }
}
