/* steal.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char D_0012B508;
extern char D_0017018C[];
extern short D_00178A08;
extern int D_00183258;
extern int D_0018325C;
extern int D_00183260;
extern int D_00183264[];
extern int D_0018328C;
extern int D_0018333C;
extern int D_00183340;
extern signed char text_buffer[];
extern char D_00190BE4[];
extern struct record *D_00195AF4;
extern struct character *player_character;
extern signed char crime_current;

extern int hud_message_add(int);
extern int rand();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern void func_0002FBCC(void);
extern void skill_add_uses(int, int);
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void guards_summon(int);
extern void guild_count_crime(int, unsigned char);
extern void hud_status_set(int);
#pragma aux func_000A0ED9 parm routine [];

void pickpocket_attempt(struct record *a1)
{
    int l_20;
    int l_1C;
    struct character *l_18;

    l_1C = (int)(short)player_character->skills[15].value;
    skill_add_uses(15, 1);
    if (a1->type == 18) {
        l_18 = &a1->data.character;
        l_1C += (player_character->level - l_18->level) * 5;
    }
    if (l_1C < 5) {
        l_1C = 5;
    } else if (l_1C > 95) {
        l_1C = 95;
    }
    if ((rand() % 101) > l_1C) {
        if (a1->type == 18) func_0002FBCC();
        hud_message_add(D_0018333C);
        crime_current = 12;
        guards_summon(1);
        if (a1->type == 53) a1->npc_flags |= 0x8000;
        return;
    }
    if ((rand() % 101) < 33) {
        msgbox_show_rsc(8999, 1);
        return;
    }
    D_00178A08 = 250;
    D_0012B508 = 145;
    l_20 = (rand() % 5) + 1;
    player_character->gold += l_20;
    func_000A0ED9(155, (int)D_0017018C);
    mc_sprintf((int)text_buffer, D_00183340, l_20);
    msgbox_show_string((int)text_buffer, 1);
    guild_count_crime(5, 1);
}

void lock_show_difficulty(int a1)
{
    int l_18;

    if (a1 >= 20) {
        hud_status_set(D_0018328C);
        return;
    }
    l_18 = player_character->skills[13].value - (a1 * 5);
    if (l_18 < 30) {
        hud_status_set(D_00183258);
        return;
    }
    if (l_18 < 35) {
        hud_status_set(D_0018325C);
        return;
    }
    if (l_18 < 45) {
        hud_status_set(D_00183260);
        return;
    }
    hud_status_set(D_00183264[((l_18 - 45) / 5)]);
}

void func_00013981(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->group != 27 || l_18->index != 3) return;
    if (l_18->value != *(int *)D_00190BE4 || (short)l_18->message != *(short *)D_00190BE4) return;
    D_00195AF4 = a1;
}
