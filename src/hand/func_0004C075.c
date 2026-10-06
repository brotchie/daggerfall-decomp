/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004C075 */
#include "records.h"

extern signed char D_0012B508;
extern char D_00174F47[];
extern char D_001850D4[];
extern int D_001850E5[];
extern int sky_loaded_frame;
extern unsigned char D_00196271;
extern signed char game_mode;
extern signed char night_sky_loaded;
extern struct quest *current_quest;
extern void msgbox_show_quest_text(struct quest *, short, int);
extern void msgbox_update(void);
extern void palette_restore(void);
extern int flc_play_with_text(int, iptr, int, int);
extern void info_popup_update(void);
extern void msgbox_yes_no_quest(short);
extern void player_movement_update(void);
extern int mc_memset();
extern int toupper();
extern char *strchr();
extern int xn_gfx_present_inclusive();

int quest_offer_prompt(struct quest *quest)
{
    char anim[44];
    char *found_letter;
    int accepted;

    accepted = 0;
    D_0012B508 = 146;
    found_letter = strchr(D_001850D4, toupper(quest->name[0]));
    if (found_letter != 0) {
        accepted = flc_play_with_text(D_001850E5[found_letter - D_001850D4], (iptr)anim, 1000, 1) == 1;
        flc_play_with_text(D_001850E5[found_letter - D_001850D4], (iptr)anim, accepted ? 1002 : 1001, 0);
        mc_memset(655360, 0, 64000, (iptr)D_00174F47, 277, 4);
        palette_restore();
        sky_loaded_frame = 10000;
        night_sky_loaded = 0;
        return accepted;
    }
    switch (toupper(quest->name[5])) {
    case 'Y':
        msgbox_yes_no_quest(1000);
        while ((unsigned char)game_mode == 8) {
            info_popup_update();
            msgbox_update();
            player_movement_update();
            xn_gfx_present_inclusive(1);
        }
        accepted = D_00196271 == 1;
        D_0012B508 = 146;
        msgbox_show_quest_text(current_quest, accepted ? 1002 : 1001, 1);
        break;
    default:
        accepted = 1;
    }
    return accepted;
}
