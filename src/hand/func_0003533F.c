/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003533F */
#include "records.h"

extern short xn_gfx_clip_bottom;
extern unsigned char player_environment;
extern char D_00187CA8;
extern struct region regions[];
extern struct image *hud_bar_image;
extern struct settings *game_settings;
extern unsigned char climate_weathers[];
extern unsigned char current_region;
extern int climate_category(void);
extern void xn_sky_draw_snow(void);
extern void xn_sky_draw_rain(void);
#ifdef DAGGER_PORT
extern void port_wide_weather_turn(void);
extern void port_wide_weather(int);
extern int port_forced_weather(void);
#endif

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
    if (regions[current_region].precipitation_override != 0)
        weather = regions[current_region].precipitation_override - 1;
#ifdef DAGGER_PORT
    if (port_forced_weather())          /* PORT_WEATHER (port/host/main.c): a test's rain or snow */
        weather = port_forced_weather();
#endif
    if ((weather & 127) == 5) {
        saved_clip = xn_gfx_clip_bottom;
        xn_gfx_clip_bottom = (game_settings->view_flags & 1) ? 199 : hud_bar_image->y - 2;
#ifdef DAGGER_PORT
        port_wide_weather_turn();       /* the wide frame's sides too (port/engine_gl/wide.c) */
        xn_sky_draw_snow();
        port_wide_weather(5);
#else
        xn_sky_draw_snow();
#endif
        xn_gfx_clip_bottom = saved_clip;
    } else if ((weather & 127) == 4) {
        saved_clip = xn_gfx_clip_bottom;
        xn_gfx_clip_bottom = (game_settings->view_flags & 1) ? 199 : hud_bar_image->y - 2;
        xn_sky_draw_rain();
#ifdef DAGGER_PORT
        port_wide_weather(4);
#endif
        xn_gfx_clip_bottom = saved_clip;
    }
}
