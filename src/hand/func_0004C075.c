/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004C075 */
#include "records.h"

extern char D_0012B508[];
extern char D_00174F47[];
extern char D_001850D4[];
extern char D_001850E5[];
extern char D_00195D48[];
extern char D_00196271[];
extern char game_mode[];
extern char D_0019629B[];
extern struct quest *current_quest;
extern void msgbox_show_quest_text(struct quest *, short, int);
extern void msgbox_update(void);
extern void palette_restore(void);
extern int flc_play_with_text(int, int, int, int);
extern void info_popup_update(void);
extern void msgbox_yes_no_quest(short);
extern void player_movement_update(void);
extern int mc_memset();
extern int toupper();
extern int strchr();
extern int func_000CDD81();

int quest_offer_prompt(struct quest *a1)
{
    char l_50[44];
    int l_20;
    int l_1C;

    l_1C = 0;
    *(signed char *)D_0012B508 = 146;
    l_20 = strchr((int)D_001850D4, toupper(a1->name[0]));
    if (l_20 != 0) {
        l_1C = flc_play_with_text(*(int *)(D_001850E5 + ((l_20 - ((int)D_001850D4)) << 2)), (int)l_50, 1000, 1) == 1;
        flc_play_with_text(*(int *)(D_001850E5 + ((l_20 - ((int)D_001850D4)) << 2)), (int)l_50, l_1C ? 1002 : 1001, 0);
        mc_memset(655360, 0, 64000, (int)D_00174F47, 277, 4);
        palette_restore();
        *(int *)D_00195D48 = 10000;
        *(signed char *)D_0019629B = 0;
        return l_1C;
    }
    switch (toupper(a1->name[5])) {
    case 'Y':
        msgbox_yes_no_quest(1000);
        while (*(unsigned char *)game_mode == 8) {
            info_popup_update();
            msgbox_update();
            player_movement_update();
            func_000CDD81(1);
        }
        l_1C = *(unsigned char *)D_00196271 == 1;
        *(signed char *)D_0012B508 = 146;
        msgbox_show_quest_text(current_quest, l_1C ? 1002 : 1001, 1);
        break;
    default:
        l_1C = 1;
    }
    return l_1C;
}
