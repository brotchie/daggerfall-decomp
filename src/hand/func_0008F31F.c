/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008F31F */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char key_down_esc;
extern char D_00176E94[];
extern char potionmaker_buttons[];
extern char D_00187FE2[];
extern char D_00187FE4[];
extern char D_00187FE6[];
extern char D_00187FE8[];
extern signed char text_buffer[];
extern char D_00190BE4[];
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

struct img {
    short f0;
    short f2;
    unsigned short w;
    unsigned short h;
    short f8;
    short f10;
    short f12;
    int data;
};

struct hotspot {
    short x0;
    short y0;
    short x1;
    short y1;
    void (*fn)(void);
};

#define COUNT (potion_cauldron_count)
#define MOUSE_X (mouse_x)
#define MOUSE_Y (mouse_y)

void potionmaker_update(void)
{
    char buf[112];      /* never used: it only sizes the frame */
    struct img *l_24;
    struct item *l_20;
    short l_1C;
    short l_18;

    if (potionmaker_open(0) == 0) return;
    xn_draw_fullscreen_overlay_shaded(window_image);
    xn_font_select(4);
    text_draw_coloured(potion_name, 31, 185, 145, 156);
    text_draw_coloured(itoa(player_character->gold, (int)text_buffer, 10), 235, 185, 145, 156);
    xn_font_select(3);
    *(int *)potion_ingredient_count = 0;
    mc_memset((int)potion_ingredients, 0, 2048, (int)D_00176E94, 182, 2048);
    object_foreach(player_entity->children, (int)potionmaker_ingredient_cb);
    for (COUNT = l_1C = 0; l_1C < 8; l_1C++) {
        if (((int *)potion_cauldron)[l_1C] != 0) {
            l_20 = &((struct record **)potion_cauldron)[l_1C]->data.item;
            l_24 = *(struct img **)((char *)xn_tex_cache_lookup(l_20->inventory_image >> 7, l_20->inventory_image & 127, -1) + 12);
            xn_draw_image_drop_shadow((COUNT & 1) * 56 + 233 - (l_24->w >> 1), (COUNT >> 1) * 38 + 42 - (l_24->h >> 1), l_24->w, l_24->h, (char *)l_24 + l_24->data);
            text_draw_centred_coloured((int)l_20->name, (short)((COUNT & 1) * 56 + 236), (short)((COUNT >> 1) * 40 + 48), 145, 156);
            ((short *)D_001A9B9C)[COUNT++] = l_1C;
        }
    }
    if (*(int *)potion_ingredient_count == 0 && D_001AA3E0 == 0 && COUNT == 0) {
        msgbox_show_rsc(34, 1);
        potionmaker_close();
    }
    if (((struct bf8_2_1 *)&D_001940D4)->f && (l_1C = list_popup_poll()) > -1)
        potion_make(((struct item **)D_00190BE4)[l_1C]);
    if ((char)key_down_esc != 0)
        potionmaker_close();
    if ((char)mouse_buttons == 0 || ((char)mouse_buttons != 0 && (char)mouse_buttons_prev != 0)) return;
    for (l_1C = 0; l_1C < 5; l_1C++) {
        if (MOUSE_X > ((struct hotspot *)potionmaker_buttons)[l_1C].x0 && MOUSE_X < ((struct hotspot *)potionmaker_buttons)[l_1C].x1
         && MOUSE_Y > ((struct hotspot *)potionmaker_buttons)[l_1C].y0 && MOUSE_Y < ((struct hotspot *)potionmaker_buttons)[l_1C].y1) {
            sound_play(203, player_object, 100);
            ((struct hotspot *)potionmaker_buttons)[l_1C].fn();
        }
    }
    if (MOUSE_X > 221 && MOUSE_X < 304 && MOUSE_Y > 30 && MOUSE_Y < 171) {
        l_1C = (MOUSE_X - 221) % 56;
        if (l_1C > 27) return;
        l_1C = (MOUSE_X - 221) / 56;
        l_18 = (MOUSE_Y - 30) % 38;
        if (l_18 > 24) return;
        l_18 = (MOUSE_Y - 30) / 38;
        l_1C += l_18 + l_18;
        ((int *)potion_cauldron)[((short *)D_001A9B9C)[l_1C]] = 0;
        ((unsigned char *)D_001A9BB4)[((short *)D_001A9B9C)[l_1C]] = 254;
        return;
    }
    if (MOUSE_X < 16 || MOUSE_X > 155 || MOUSE_Y < 30 || MOUSE_Y > 171) return;
    l_1C = (MOUSE_X - 16) % 56;
    if (l_1C > 27) return;
    l_1C = (MOUSE_X - 16) / 56;
    l_18 = (MOUSE_Y - 30) % 38;
    if (l_18 > 27) return;
    l_18 = (MOUSE_Y - 30) / 38;
    l_1C += l_18 * 3;
    l_20 = &((struct record **)potion_ingredients)[l_1C + *(int *)potion_ingredient_scroll]->data.item;
    if (((int *)potion_ingredients)[l_1C + *(int *)potion_ingredient_scroll] != 0 && COUNT != 8 && potionmaker_in_cauldron(l_20->group, l_20->index) == 0)
        potionmaker_add_ingredient(l_1C + *(int *)potion_ingredient_scroll);
}
