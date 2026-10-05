/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000202C5 */
extern char D_00170604[];
extern char D_00170634[];
extern char D_0017063D[];
extern char D_00179E60[];
extern char D_00179E66[];
extern unsigned char D_00179E7F[];
extern unsigned char D_00179E90[];
extern unsigned char D_00179E94[];
extern unsigned char D_001850D4[];
extern int D_001850E5[];
extern char text_buffer[];
extern unsigned game_minutes;
extern unsigned short *game_settings;
extern int D_00195D30;
extern unsigned char climate_weathers[];
extern char D_001961F5[];
extern unsigned char D_00196271;
extern unsigned char *D_0019671C;
extern int current_quest;
extern unsigned char *faction_find(short);
extern int climate_category(void);
extern void msgbox_show_string(char *, int);
extern void msgbox_show_rsc(int, int);
extern int flc_play_with_text(int, char *, int, int);
extern unsigned char *monster_summon_near_player(int);
extern int rand_range(int, int);
extern void msgbox_yes_no_rsc(int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern int rand(void);
extern int srand(int);
extern int mc_memset();
extern int mc_strncpy();
extern char *func_000CE45E(char *, short, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int mc_sprintf(char *, ...);

#define REGION(o) D_001850D4[D_00179E94[*(unsigned short *)((o) + 33)]]

void daedra_summon(unsigned char *a1)
{
    char l_70[44];
    unsigned char *l_40;
    unsigned char *l_3C;
    char *l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    unsigned char *l_24;
    unsigned char *l_20;
    unsigned char l_1C;
    unsigned char l_18;

    l_18 = 48;
    l_20 = a1 + 71;
    l_40 = faction_find(*(short *)l_20);
    while (*(unsigned char **)(l_40 + 88) != 0)
        l_40 = *(unsigned char **)(l_40 + 88);
    if (*(unsigned short *)(l_40 + 33) == 40 || *l_40 == 8) {
    } else {
        l_40 = *(unsigned char **)(l_40 + 84);
    }
    switch (*(unsigned short *)(l_40 + 33)) {
    case 40:
        l_38 = func_000CE45E(D_00179E60, game_minutes % 518400 / 1440, 16);
        if (l_38 == 0 || D_00179E66 == l_38) {
            msgbox_show_rsc(480, 1);
            return;
        }
        l_34 = (l_38 - D_00179E60) / 2 + 1;
        l_3C = faction_find(l_34);
        D_0019671C = l_3C;
        if (REGION(l_3C) == 56) {
            if (*game_settings & 4) {
                msgbox_show_rsc(400, 1);
                return;
            }
            l_18 = 120;
        }
        D_00195D30 = (100 - *(short *)(l_40 + 29)) * 1000 + 100000;
        msgbox_yes_no_rsc(481);
        if (D_00196271 == 2) return;
        l_30 = 30;
        break;
    case 21:
    case 22:
    case 24:
    case 26:
    case 27:
    case 29:
    case 33:
    case 35:
    case 36:
    case 82:
    case 84:
    case 88:
    case 92:
    case 94:
    case 98:
    case 106:
        l_38 = func_000CE45E(D_00179E60, game_minutes % 518400 / 1440, 16);
        if (l_38 == 0 || D_00179E66 == l_38) {
            msgbox_show_rsc(480, 1);
            return;
        }
        l_34 = (l_38 - D_00179E60) / 2 + 1;
        for (l_2C = 0; l_2C < 3; l_2C++) {
            if (*(unsigned char **)(l_40 + l_2C * 4 + 68) != 0 && *(unsigned short *)(*(unsigned char **)(l_40 + l_2C * 4 + 68) + 33) == l_34) {
                msgbox_show_string(D_00170604, 1);
                return;
            }
        }
        l_3C = faction_find(l_34);
        D_0019671C = l_3C;
        if (REGION(l_3C) == 56) {
            if (*game_settings & 4) {
                msgbox_show_rsc(400, 1);
                return;
            }
            l_18 = 120;
        }
        D_00195D30 = (100 - *(short *)(l_40 + 29)) * 1000 + 100000;
        msgbox_yes_no_rsc(481);
        if (D_00196271 == 2) return;
        l_30 = 30;
        break;
    default:
        l_28 = rand();
        srand(game_minutes / 1440);
        l_34 = 4;
        if (*(unsigned short *)(l_40 + 33) != 419) {
            while (l_34 == 4)
                l_34 = rand_range(1, 16);
        }
        l_3C = faction_find(l_34);
        D_0019671C = l_3C;
        if (REGION(l_3C) == 56) {
            if (*game_settings & 4) {
                msgbox_show_rsc(400, 1);
                return;
            }
            l_18 = 120;
        }
        D_00195D30 = (100 - *(short *)(l_40 + 29)) * 1000 + 100000;
        msgbox_yes_no_rsc(481);
        if (D_00196271 == 2) return;
        l_30 = 30;
        srand(l_28);
        break;
    }
    if (D_00195D30 < 0)
        D_00195D30 = -D_00195D30;
    if (D_00195D30 > 200000)
        D_00195D30 = 200000;
    if (gold_can_afford(D_00195D30) == 0) {
        msgbox_show_rsc(454, 1);
        return;
    }
    l_1C = climate_weathers[climate_category()];
    l_2C = 5;
    if (l_1C == 6)
        l_2C = 15;
    if (rand() % 101 <= l_2C) {
        l_34 = 9;
        l_3C = faction_find(l_34);
    }
    l_30 += *(short *)(l_3C + 29);
    if (D_00179E7F[l_34] == 100 || l_1C == D_00179E7F[l_34])
        l_30 += 30;
    gold_spend(D_00195D30);
    if (rand_range(1, 100) > l_30) {
        msgbox_show_rsc(484, 1);
        return;
    }
    if (*(unsigned short *)(l_3C + 37) & 64) {
        l_24 = &REGION(l_3C);
        mc_memset(l_70, 0, 44, D_00170634, 189, 4);
        current_quest = 0;
        flc_play_with_text(D_001850E5[l_24 - D_001850D4], l_70, 482, 0);
        a1 = monster_summon_near_player(D_00179E90[rand_range(0, 4)]);
        if (a1 != 0)
            a1[624] = 1;
        return;
    }
    *(unsigned short *)(l_3C + 37) |= 64;
    func_000A0ED9(200, D_00170634);
    mc_sprintf(text_buffer, D_0017063D, REGION(l_3C), l_18);
    mc_strncpy(D_001961F5, text_buffer, 13, D_00170634, 201);
}
