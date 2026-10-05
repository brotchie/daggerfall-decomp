/* steal.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0012B508[];
extern char D_0017018C[];
extern char D_00178A08[];
extern char D_00183258[];
extern char D_0018325C[];
extern char D_00183260[];
extern char D_00183264[];
extern char D_0018328C[];
extern char D_0018333C[];
extern char D_00183340[];
extern char text_buffer[];
extern char D_00190BE4[];
extern char D_00195AF4[];
extern char player_character[];
extern char crime_current[];

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

void pickpocket_attempt(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    l_1C = (int)(short)*(short *)(*(char **)player_character + 247);
    skill_add_uses(15, 1);
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 18) goto L137B7;
    l_18 = a1 + 71;
    l_1C += (((int)(unsigned char)*(signed char *)((char *)*(int *)player_character + 129)) - ((int)(unsigned char)*(signed char *)((char *)l_18 + 129))) * 5;
L137B7:;
    if (l_1C >= 5) goto L137C6;
    l_1C = 5;
    goto L137D3;
L137C6:;
    if (l_1C <= 95) goto L137D3;
    l_1C = 95;
L137D3:;
    if ((rand() % 101) <= l_1C) goto L13835;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 18) goto L137FF;
    func_0002FBCC();
L137FF:;
    hud_message_add(*(int *)D_0018333C);
    *(signed char *)crime_current = 12;
    guards_summon(1);
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 53) goto L13830;
    *(signed char *)((char *)a1 + 26) |= 128;
L13830:;
    return;
L13835:;
    if ((rand() % 101) >= 33) goto L1385E;
    msgbox_show_rsc(8999, 1);
    return;
L1385E:;
    *(short *)D_00178A08 = 250;
    *(signed char *)D_0012B508 = 145;
    l_20 = (rand() % 5) + 1;
    *(int *)(*(char **)player_character + 133) += l_20;
    func_000A0ED9(155, (int)D_0017018C);
    mc_sprintf((int)text_buffer, *(int *)D_00183340, l_20);
    msgbox_show_string((int)text_buffer, 1);
    guild_count_crime(5, 1);
}

void lock_show_difficulty(int a1)
{
    int l_18;

    if (a1 < 20) goto L1390A;
    hud_status_set(*(int *)D_0018328C);
    return;
L1390A:;
    l_18 = ((int)(short)*(short *)(*(char **)player_character + 235)) - (a1 * 5);
    if (l_18 >= 30) goto L13933;
    hud_status_set(*(int *)D_00183258);
    return;
L13933:;
    if (l_18 >= 35) goto L13945;
    hud_status_set(*(int *)D_0018325C);
    return;
L13945:;
    if (l_18 >= 45) goto L13957;
    hud_status_set(*(int *)D_00183260);
    return;
L13957:;
    hud_status_set(*(int *)(D_00183264 + (((l_18 - 45) / 5) << 2)));
}

void func_00013981(int a1)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    l_18 = a1 + 71;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 32)) != 27) goto L139CC;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 34)) == 3) goto L139CE;
L139CC:;
    return;
L139CE:;
    if (*(int *)((char *)l_18 + 36) != *(int *)D_00190BE4) goto L139EC;
    if (*(short *)((char *)l_18 + 63) == *(short *)D_00190BE4) goto L139EE;
L139EC:;
    return;
L139EE:;
    *(int *)D_00195AF4 = a1;
}
