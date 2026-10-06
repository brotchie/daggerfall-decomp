/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005CE17 */
#include "records.h"

extern char xn_gfx_row_offset[];
extern iptr screen_buffer;
extern char D_00175898[];
extern char D_00185CDC[];
extern int spell_points_bonus;
extern char hud_bar_image[];
extern iptr hud_mode_icons;
extern char hud_portrait[];
extern struct character *player_character;
extern struct settings *game_settings;
extern int hud_vital_bar_images;
extern int D_00195C80;
extern int D_00195C84;
extern iptr hud_portrait_overlays;
extern unsigned char interaction_mode;
extern void hud_draw_compass(void);
extern int hud_portrait_overlay_index(void);
extern int mc_memcpy();
extern int xn_draw_put_rect();
extern int xn_draw_image();
extern int xn_draw_image_transparent();


#define IMG(g) (*(struct image **)(g))
#define BARS ((struct image **)&hud_vital_bar_images)

void hud_draw(void)
{
    unsigned max_fatigue;
    int portrait_x;
    int portrait_y;
    int bar_height;
    int bar_x;
    int overlay;
    struct image *image;

    if ((game_settings->view_flags & 1) == 0) {
        mc_memcpy(*(char **)&screen_buffer + ((int *)xn_gfx_row_offset)[IMG(hud_bar_image)->y], IMG(hud_bar_image)->pixels, IMG(hud_bar_image)->data_size, D_00175898, 124, 4);
        xn_draw_put_rect(131, 154, 47, 22, *(char **)&hud_mode_icons + ((int *)D_00185CDC)[interaction_mode], 0);
        overlay = hud_portrait_overlay_index();
        if (overlay != -1) {
            image = ((struct image *)hud_portrait_overlays);
            while (overlay != 0) {
                image = (struct image *)((char *)image + image->data_size + 12);
                overlay--;
            }
            xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
        }
        portrait_x = 23 - IMG(hud_portrait)->width / 2;
        portrait_y = 176 - IMG(hud_portrait)->height / 2;
        xn_draw_image_transparent(portrait_x, portrait_y, IMG(hud_portrait)->width, IMG(hud_portrait)->height, IMG(hud_portrait)->pixels);
        hud_draw_compass();
        bar_x = 0;
    } else {
        bar_x = -40;
    }
    if (player_character->health > 0) {
        bar_height = ((player_character->health << 8) / player_character->max_health << 5) / 256;
        if (bar_height != 0)
            xn_draw_image(bar_x + 49, 32 - bar_height + 161, 4, bar_height, BARS[0]->pixels + (32 - bar_height) * 4);
    }
    if (player_character->fatigue > 0) {
        max_fatigue = (player_character->attributes[ATTR_STR] + player_character->attributes[ATTR_END]) << 6;
        bar_height = ((player_character->fatigue << 8) / max_fatigue << 5) >> 8;
        if (bar_height != 0)
            xn_draw_image(bar_x + 57, 32 - bar_height + 161, 4, bar_height, BARS[1]->pixels + (32 - bar_height) * 4);
    }
    if (player_character->magicka + spell_points_bonus > 0) {
        bar_height = (((player_character->magicka + spell_points_bonus) << 8) / player_character->max_magicka << 5) / 256;
        if (bar_height != 0)
            xn_draw_image(bar_x + 65, 32 - bar_height + 161, 4, bar_height, BARS[2]->pixels + (32 - bar_height) * 4);
    }
}
