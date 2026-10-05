/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088732 */
#include "records.h"
extern int D_000C2893[];
extern char *D_000C28BC;
extern char *D_000C28C0;
extern int D_00187F30[];
extern char text_rsc_buffer[];
extern struct record *player_object;
extern char D_00196289;
extern struct map_location *location_here;
extern int D_001A94A0[];
extern int D_001A94B0[];
extern int D_001A94C0;
extern void parse_rsc_text(int, int, int);
extern int hud_message_add(char *);
extern struct map_location *region_find_location(int);
extern void func_00088281(char *, char *);
extern int func_000C2D81();
extern int func_000C3A60();
extern int func_000C3FCB();

void terrain_update_cells(void)
{
    int i;
    int cur;
    struct map_location *saved;

    cur = func_000C2D81(player_object->x, player_object->z);
    saved = location_here;
    D_001A94C0 = 4;
    for (i = 0; i < 4; i++) {
        if (D_001A94A0[i] != D_000C2893[i]) {
            D_001A94A0[i] = D_000C2893[i];
            D_001A94B0[i] = 1;
            D_001A94C0 = i;
            if ((location_here = region_find_location(D_001A94A0[i])) != 0) {
                if (D_00196289 == 0 && cur == D_001A94A0[i]) {
                    switch ((location_here->x_type_flags << 2) >> 27) {
                    case 4:
                    case 7:
                    case 10:
                    case 12:
                        parse_rsc_text(location_here->dungeon_type + 500, 0, 0);
                        hud_message_add(text_rsc_buffer);
                    }
                }
                func_00088281(D_000C28BC + D_00187F30[D_001A94C0], D_000C28C0 + D_00187F30[D_001A94C0]);
                func_000C3FCB();
            }
            func_000C3A60(i);
        }
    }
    location_here = saved;
}
