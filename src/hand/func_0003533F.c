/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003533F */
#include "records.h"

struct w2 { unsigned short f0; unsigned short f2; };
struct dun { unsigned char f0; char pad[79]; };
extern short D_0014294C;
extern unsigned char player_environment;
extern char D_00187CA8;
extern struct dun region_precipitation_override[];
extern struct w2 *hud_bar_image;
extern struct settings *game_settings;
extern unsigned char climate_weathers[];
extern unsigned char current_region;
extern int climate_category(void);
extern void func_000C9A89(void);
extern void func_000C9CB9(void);

void weather_draw_precipitation(void)
{
    int n;
    int kind;
    int old;

    n = climate_category();
    if (D_00187CA8 == 0)
        return;
    if (player_environment != 1)
        return;
    kind = climate_weathers[n];
    if (region_precipitation_override[current_region].f0 != 0)
        kind = region_precipitation_override[current_region].f0 - 1;
    if ((kind & 127) == 5) {
        old = D_0014294C;
        D_0014294C = (*(unsigned short *)game_settings & 1) ? 199 : hud_bar_image->f2 - 2;
        func_000C9A89();
        D_0014294C = old;
    } else if ((kind & 127) == 4) {
        old = D_0014294C;
        D_0014294C = (*(unsigned short *)game_settings & 1) ? 199 : hud_bar_image->f2 - 2;
        func_000C9CB9();
        D_0014294C = old;
    }
}
