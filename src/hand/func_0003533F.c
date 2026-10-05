/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003533F */
#include "records.h"

struct dun { unsigned char f0; char pad[79]; };
extern short xn_gfx_clip_bottom;
extern unsigned char player_environment;
extern char D_00187CA8;
extern struct dun region_precipitation_override[];
extern struct image *hud_bar_image;
extern struct settings *game_settings;
extern unsigned char climate_weathers[];
extern unsigned char current_region;
extern int climate_category(void);
extern void xn_sky_draw_snow(void);
extern void xn_sky_draw_rain(void);

void weather_draw_precipitation(void)
{
    int climate;
    int weather;
    int saved_clip;

    climate = climate_category();
    if (D_00187CA8 == 0)
        return;
    if (player_environment != 1)
        return;
    weather = climate_weathers[climate];
    if (region_precipitation_override[current_region].f0 != 0)
        weather = region_precipitation_override[current_region].f0 - 1;
    if ((weather & 127) == 5) {
        saved_clip = xn_gfx_clip_bottom;
        xn_gfx_clip_bottom = (game_settings->view_flags & 1) ? 199 : hud_bar_image->y - 2;
        xn_sky_draw_snow();
        xn_gfx_clip_bottom = saved_clip;
    } else if ((weather & 127) == 4) {
        saved_clip = xn_gfx_clip_bottom;
        xn_gfx_clip_bottom = (game_settings->view_flags & 1) ? 199 : hud_bar_image->y - 2;
        xn_sky_draw_rain();
        xn_gfx_clip_bottom = saved_clip;
    }
}
