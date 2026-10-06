/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009CEC4 */
#include "records.h"
#include "clib.h"

extern char *screen_buffer;
extern char D_0017743D[];
extern unsigned short travel_options;
extern char D_00190CE8;
extern struct character *player_character;
extern struct career *player_class;
extern int travel_ocean_pixels;
extern char *D_001AA690;
extern int D_001AA698;
extern unsigned char D_001AA6A6;
extern int climate_update_at_player(void);
extern void time_pass(int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int travel_pixel_time(int, int);
extern void travel_find_transport(void);
extern void travel_toggle_zoom(void);

int travel_route(int x0, int y0, int x1, int y1, int go)
{
    int dx;
    int dy;
    int n;
    int adx;
    int ady;
    int i;
    int sx;
    int sy;
    int err;
    int sum;
    int v;
    int flag;
    int save;
    int unused;
    struct membership *p;

    flag = 0;
    if (D_001AA698 != 0) {
        D_001AA690 = mc_malloc(64000, D_0017743D, 984);
        mc_memcpy(D_001AA690, screen_buffer, 64000, D_0017743D, 985, 4);
    }
    save = player_character->fatigue;
    x0 = x0 / 32768;
    y0 = y0 / 32768;
    x1 = x1 / 32768;
    y1 = y1 / 32768;
    dx = x1 - x0;
    dy = y1 - y0;
    adx = abs(dx);
    ady = abs(dy);
    n = adx > ady ? adx : ady;
    if (dx < 0)
        sx = -1;
    else
        sx = 1;
    if (dy < 0)
        sy = -1;
    else
        sy = 1;
    travel_find_transport();
    travel_ocean_pixels = sum = err = i = 0;
    for (; i < n; i++) {
        if (n == adx) {
            x0 += sx;
            err += ady;
            if (err > adx) {
                err -= adx;
                y0 += sy;
            }
        } else {
            y0 += sy;
            err += adx;
            if (err > ady) {
                err -= ady;
                x0 += sx;
            }
        }
        v = travel_pixel_time(x0, y0);
        if ((int)(unsigned short)(travel_options & 32) != 0)
            v = v * 300 / 256;
        sum += v;
    }
    if (!(go == 0 || D_001AA6A6 == 100)) {
        if (D_00190CE8 != 0)
            travel_toggle_zoom();
        if ((int)(unsigned short)(travel_options & 3) == 2)
            sum = (sum << 7) / 256;
        time_pass(sum);
        if ((int)(unsigned short)(travel_options & 3) == 2) {
            player_character->fatigue = save;
        } else {
            player_character->fatigue = (player_character->attributes[ATTR_STR] + player_character->attributes[ATTR_END]) << 6;
            player_character->health = player_character->max_health;
            if ((int)(unsigned short)(player_class->flags & 8) == 0)
                player_character->magicka = player_character->max_magicka;
        }
    } else {
        climate_update_at_player();
    }
    if (D_001AA698 != 0) {
        if (!(D_001AA690 == 0 || D_001AA690 == (char *)(iptr)-1751672937)) {
            mc_free(D_001AA690, D_0017743D, 1058);
            D_001AA690 = (char *)(iptr)-1751672937;
        }
    }
    p = guild_find_membership_by_kind(145);
    if (p != 0)
        return sum * (((95 - p->rank) << 8) / 100) / 256;
    return sum;
}
