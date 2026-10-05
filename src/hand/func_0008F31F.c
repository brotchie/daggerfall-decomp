/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008F31F */
#include "records.h"
#include "bitfield.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char key_down_esc;
extern char D_00176E94[];
extern struct rect potionmaker_buttons[];
extern char D_00187FE2[];
extern char D_00187FE4[];
extern char D_00187FE6[];
extern char D_00187FE8[];
extern signed char text_buffer[];
extern char scratch_190be4[];
extern signed char D_001940D4;
extern struct record *player_entity;
extern struct record *player_object;
extern struct character *player_character;
extern int window_image;
extern signed char mouse_buttons_prev;
extern char D_001A9B9C[];
extern signed char D_001A9BB4[];
extern char potion_cauldron[];
extern char potion_ingredients[];
extern char potion_ingredient_scroll[];
extern int D_001AA3E0;
extern char potion_ingredient_count[];
extern int potion_name;
extern short potion_cauldron_count;
extern int list_popup_poll(void);
extern void msgbox_show_rsc(int, int);
extern int sound_play(int, struct record *, int);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern void object_foreach(struct record *, int);
extern int potionmaker_open(int);
extern void potionmaker_add_ingredient(int);
extern int potionmaker_in_cauldron(unsigned short, unsigned short);
extern void potionmaker_ingredient_cb(struct record *);
extern int potionmaker_close(void);
extern void potion_make(struct item *);
extern int mc_memset();
extern int itoa();
extern int xn_draw_fullscreen_overlay_shaded();
extern int xn_draw_image_drop_shadow();
extern int xn_font_select();
extern int xn_tex_cache_lookup();



#define COUNT (potion_cauldron_count)
#define MOUSE_X (mouse_x)
#define MOUSE_Y (mouse_y)

void potionmaker_update(void)
{
    char buf[112];      /* never used: it only sizes the frame */
    struct texture_header *image;
    struct item *item;
    short i;
    short row;

    if (potionmaker_open(0) == 0) return;
    xn_draw_fullscreen_overlay_shaded(window_image);
    xn_font_select(4);
    text_draw_coloured(potion_name, 31, 185, 145, 156);
    text_draw_coloured(itoa(player_character->gold, (int)text_buffer, 10), 235, 185, 145, 156);
    xn_font_select(3);
    *(int *)potion_ingredient_count = 0;
    mc_memset((int)potion_ingredients, 0, 2048, (int)D_00176E94, 182, 2048);
    object_foreach(player_entity->children, (int)potionmaker_ingredient_cb);
    for (COUNT = i = 0; i < 8; i++) {
        if (((int *)potion_cauldron)[i] != 0) {
            item = &((struct record **)potion_cauldron)[i]->data.item;
            image = *(struct texture_header **)((char *)xn_tex_cache_lookup(item->inventory_image >> 7, item->inventory_image & 127, -1) + 12);
            xn_draw_image_drop_shadow((COUNT & 1) * 56 + 233 - (image->width >> 1), (COUNT >> 1) * 38 + 42 - (image->height >> 1), image->width, image->height, (char *)image + image->data_offset);
            text_draw_centred_coloured((int)item->name, (short)((COUNT & 1) * 56 + 236), (short)((COUNT >> 1) * 40 + 48), 145, 156);
            ((short *)D_001A9B9C)[COUNT++] = i;
        }
    }
    if (*(int *)potion_ingredient_count == 0 && D_001AA3E0 == 0 && COUNT == 0) {
        msgbox_show_rsc(34, 1);
        potionmaker_close();
    }
    if (((struct bf8_2_1 *)&D_001940D4)->f && (i = list_popup_poll()) > -1)
        potion_make(((struct item **)scratch_190be4)[i]);
    if ((char)key_down_esc != 0)
        potionmaker_close();
    if ((char)mouse_buttons == 0 || ((char)mouse_buttons != 0 && (char)mouse_buttons_prev != 0)) return;
    for (i = 0; i < 5; i++) {
        if (MOUSE_X > potionmaker_buttons[i].x0 && MOUSE_X < potionmaker_buttons[i].x1
         && MOUSE_Y > potionmaker_buttons[i].y0 && MOUSE_Y < potionmaker_buttons[i].y1) {
            sound_play(203, player_object, 100);
            potionmaker_buttons[i].handler();
        }
    }
    if (MOUSE_X > 221 && MOUSE_X < 304 && MOUSE_Y > 30 && MOUSE_Y < 171) {
        i = (MOUSE_X - 221) % 56;
        if (i > 27) return;
        i = (MOUSE_X - 221) / 56;
        row = (MOUSE_Y - 30) % 38;
        if (row > 24) return;
        row = (MOUSE_Y - 30) / 38;
        i += row + row;
        ((int *)potion_cauldron)[((short *)D_001A9B9C)[i]] = 0;
        ((unsigned char *)D_001A9BB4)[((short *)D_001A9B9C)[i]] = 254;
        return;
    }
    if (MOUSE_X < 16 || MOUSE_X > 155 || MOUSE_Y < 30 || MOUSE_Y > 171) return;
    i = (MOUSE_X - 16) % 56;
    if (i > 27) return;
    i = (MOUSE_X - 16) / 56;
    row = (MOUSE_Y - 30) % 38;
    if (row > 27) return;
    row = (MOUSE_Y - 30) / 38;
    i += row * 3;
    item = &((struct record **)potion_ingredients)[i + *(int *)potion_ingredient_scroll]->data.item;
    if (((int *)potion_ingredients)[i + *(int *)potion_ingredient_scroll] != 0 && COUNT != 8 && potionmaker_in_cauldron(item->group, item->index) == 0)
        potionmaker_add_ingredient(i + *(int *)potion_ingredient_scroll);
}
