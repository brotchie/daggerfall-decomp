/* archive.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern char D_00170150[];
extern char D_0017015A[];
extern char D_00170172[];
extern char lock_text_fail[];
extern char lock_text_open[];
extern struct record *player_object;
extern struct character *player_character;
extern char D_00195C44[];
extern char archive_directories[];
extern char archive_types[];
extern char archive_record_counts[];
extern char archive_names[];

extern int sound_play(int, struct record *, int);
extern int disk_open_data(int);
extern int disk_open_rw(int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int func_0009DEA7();
extern int mc_free();
extern int lseek();
extern int mc_malloc();
extern int func_000A00CB();
extern int mc_strncpy();
extern int write();
extern int strnicmp();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern void skill_add_uses(int, int);
extern void fatal_error(int);
extern void links_trigger(struct record *, int);
extern void guild_count_crime(int, unsigned char);
int lockpick_door(struct record *);
#pragma aux func_000A0ED9 parm routine [];

int archive_open(int a1, int a2, int a3)
{
    short l_10;
    short l_1C;
    short l_18;
    short l_14;

    if (a3 == 0) goto L12E32;
L12E1F:;
    *(int *)&l_1C = disk_open_rw(a1);
    if (*(int *)&l_1C < 1) goto L12E1F;
    goto L12E3D;
L12E32:;
    *(int *)&l_1C = disk_open_data(a1);
L12E3D:;
    if (*(int *)&l_1C >= 1) goto L12E4E;
    return *(int *)&l_1C;
L12E4E:;
    mc_strncpy(((int)archive_names) + (*(int *)&l_1C * 13), a1, 13, (int)D_00170150, 32);
    func_000A00CB(*(int *)&l_1C, (int)&l_10, 2);
    func_000A00CB(*(int *)&l_1C, (int)&l_14, 2);
    if (((int)(short)l_14) != 256) goto L12EA4;
    *(int *)&l_18 = ((int)(short)l_10) * 18;
    goto L12EAE;
L12EA4:;
    *(int *)&l_18 = ((int)(short)l_10) << 3;
L12EAE:;
    if (a2 != 0) goto L12EC9;
    a2 = mc_malloc(*(int *)&l_18, (int)D_00170150, 42);
L12EC9:;
    *(short *)(archive_record_counts + (*(int *)&l_1C * 2)) = *(int *)&l_10;
    *(int *)(archive_directories + (*(int *)&l_1C << 2)) = a2;
    *(short *)(archive_types + (*(int *)&l_1C * 2)) = *(int *)&l_14;
    lseek(*(int *)&l_1C, -*(int *)&l_18, 2);
    func_000A00CB(*(int *)&l_1C, a2, *(int *)&l_18);
    return *(int *)&l_1C;
}

void archive_close(int a1)
{
    if (a1 == 0) return;
    *(short *)(archive_record_counts + (a1 * 2)) = 0;
    if (*(int *)(archive_directories + (a1 << 2)) == 0) goto L12F71;
    if (*(int *)(archive_directories + (a1 << 2)) != (-1751672937)) goto L12F73;
L12F71:;
    goto L12F9E;
L12F73:;
    mc_free(*(int *)(archive_directories + (a1 << 2)), (int)D_00170150, 67);
    *(int *)(archive_directories + (a1 << 2)) = -1751672937;
L12F9E:;
    *(int *)(archive_directories + (a1 << 2)) = 0;
    *(short *)(archive_types + (a1 * 2)) = 0;
    func_0009DEA7(a1);
}

int archive_find_record(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    if (((int)(short)*(short *)(archive_types + (a1 * 2))) != 256) goto L1304F;
    l_1C = *(int *)(archive_directories + (a1 << 2));
    l_14 = 0;
L1300C:;
    if (((int)(short)*(short *)(archive_record_counts + (a1 * 2))) > l_14) goto L1302E;
    goto L1304D;
L1301F:;
    l_14++;
    (*(char (**)[18])&l_1C)++;
    goto L1300C;
L1302E:;
    if (strnicmp(a2, l_1C, a3) != 0) goto L1304B;
    return l_14;
L1304B:;
    goto L1301F;
L1304D:;
    goto L1309E;
L1304F:;
    l_18 = *(int *)(archive_directories + (a1 << 2));
    l_14 = 0;
L13065:;
    if (((int)(short)*(short *)(archive_record_counts + (a1 * 2))) > l_14) goto L13087;
    goto L1309E;
L13078:;
    l_14++;
    (*(char (**)[8])&l_18)++;
    goto L13065;
L13087:;
    if (a3 != *(int *)((char *)l_18)) goto L1309C;
    return l_14;
L1309C:;
    goto L13078;
L1309E:;
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) != 256) goto L130E4;
    func_000A0ED9(105, (int)D_00170150);
    mc_sprintf(*(int *)D_00195C44, (int)D_0017015A, a2, ((int)archive_names) + (a1 * 13));
    goto L13115;
L130E4:;
    func_000A0ED9(107, (int)D_00170150);
    mc_sprintf(*(int *)D_00195C44, (int)D_00170172, a3, ((int)archive_names) + (a1 * 13));
L13115:;
    fatal_error(*(int *)D_00195C44);
    return 0;
}

int archive_record_size(int a1, int a2)
{
    int l_1C;
    int l_18;

    if (((int)(short)*(short *)(archive_types + (a1 * 2))) != 256) goto L13178;
    l_1C = *(int *)(archive_directories + (a1 << 2));
    l_1C += a2 * 18;
    return *(int *)((char *)l_1C + 14);
L13178:;
    l_18 = *(int *)(archive_directories + (a1 << 2));
    l_18 += a2 << 3;
    return *(int *)((char *)l_18 + 4);
}

int archive_record_offset(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 4;
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) != 256) goto L13214;
    l_24 = *(int *)(archive_directories + (a1 << 2));
    l_1C = 0;
L131E8:;
    if (l_1C < a2) goto L13201;
    goto L1320C;
L131F2:;
    l_1C++;
    (*(char (**)[18])&l_24)++;
    goto L131E8;
L13201:;
    l_18 += *(int *)((char *)l_24 + 14);
    goto L131F2;
L1320C:;
    return l_18;
L13214:;
    l_20 = *(int *)(archive_directories + (a1 << 2));
    l_1C = 0;
L1322A:;
    if (l_1C < a2) goto L13243;
    goto L1324E;
L13234:;
    l_1C++;
    (*(char (**)[8])&l_20)++;
    goto L1322A;
L13243:;
    l_18 += *(int *)((char *)l_20 + 4);
    goto L13234;
L1324E:;
    return l_18;
}

int archive_read_record(int a1, int a2, int a3)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_14 = 4;
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) != 256) goto L132D4;
    l_24 = *(int *)(archive_directories + (a1 << 2));
    l_1C = 0;
L132A5:;
    if (l_1C < a2) goto L132BE;
    goto L132C9;
L132AF:;
    l_1C++;
    (*(char (**)[18])&l_24)++;
    goto L132A5;
L132BE:;
    l_14 += *(int *)((char *)l_24 + 14);
    goto L132AF;
L132C9:;
    l_18 = *(int *)((char *)l_24 + 14);
    goto L13317;
L132D4:;
    l_20 = *(int *)(archive_directories + (a1 << 2));
    l_1C = 0;
L132EA:;
    if (l_1C < a2) goto L13303;
    goto L1330E;
L132F4:;
    l_1C++;
    (*(char (**)[8])&l_20)++;
    goto L132EA;
L13303:;
    l_14 += *(int *)((char *)l_20 + 4);
    goto L132F4;
L1330E:;
    l_18 = *(int *)((char *)l_20 + 4);
L13317:;
    if (a3 != 0) goto L13332;
    a3 = mc_malloc(l_18, (int)D_00170150, 205);
L13332:;
    lseek(a1, l_14, 0);
    func_000A00CB(a1, a3, l_18);
    return a3;
}

void archive_write_record(int a1, int a2, int a3)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_10 = 4;
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) != 256) goto L133D2;
    l_20 = *(int *)(archive_directories + (a1 << 2));
    l_18 = 0;
L133A3:;
    if (l_18 < a2) goto L133BC;
    goto L133C7;
L133AD:;
    l_18++;
    (*(char (**)[18])&l_20)++;
    goto L133A3;
L133BC:;
    l_10 += *(int *)((char *)l_20 + 14);
    goto L133AD;
L133C7:;
    l_14 = *(int *)((char *)l_20 + 14);
    goto L13415;
L133D2:;
    l_1C = *(int *)(archive_directories + (a1 << 2));
    l_18 = 0;
L133E8:;
    if (l_18 < a2) goto L13401;
    goto L1340C;
L133F2:;
    l_18++;
    (*(char (**)[8])&l_1C)++;
    goto L133E8;
L13401:;
    l_10 += *(int *)((char *)l_1C + 4);
    goto L133F2;
L1340C:;
    l_14 = *(int *)((char *)l_1C + 4);
L13415:;
    lseek(a1, l_10, 0);
    write(a1, a3, l_14);
}

void func_00013438(struct record *a1)
{
    int l_18;

    l_18 = lockpick_door(a1);
}

int lockpick_door(struct record *a1)
{
    int l_1C;

    if (a1->lockpick_skill_tried != player_character->skills[SKILL_LOCKPICKING].value) goto L13494;
    return 0;
L13494:;
    if (a1->lock_level < 20) goto L134C8;
    hud_message_add(*(int *)lock_text_fail);
    links_trigger(a1, 4);
    return 0;
L134C8:;
    skill_add_uses(13, 1);
    if ((player_character->conditions & 0x40) == 0) goto L13504;
    l_1C = (int)(unsigned char)(signed char)player_character->lock_open_chance;
    player_character->conditions &= ~0x40;
    goto L13513;
L13504:;
    l_1C = (int)(short)player_character->skills[SKILL_LOCKPICKING].value;
L13513:;
    l_1C += (((int)(unsigned char)(signed char)player_character->level) - a1->lock_level) * 5;
    if (l_1C >= 5) goto L13543;
    l_1C = 5;
    goto L13550;
L13543:;
    if (l_1C <= 95) goto L13550;
    l_1C = 95;
L13550:;
    if (rand_range(0, 100) > l_1C) goto L1359A;
    a1->flags |= 64;
    hud_message_add(*(int *)lock_text_open);
    links_trigger(a1, 7);
    sound_play(60, a1, 100);
    return 1;
L1359A:;
    if ((player_character->conditions & 0x40) != 0) goto L135BB;
    a1->lockpick_skill_tried = player_character->skills[SKILL_LOCKPICKING].value;
L135BB:;
    hud_message_add(*(int *)lock_text_fail);
    links_trigger(a1, 4);
    return 0;
}

int lockpick_action_door(int a1, int a2, struct record *a3)
{
    int l_14;

    if (((int)(unsigned char)*(signed char *)((char *)a1 + 8)) < 10) goto L13617;
    return 1;
L13617:;
    if (a3->lockpick_skill_tried != player_character->skills[SKILL_LOCKPICKING].value) goto L1363C;
    return 0;
L1363C:;
    if (a2 < 20) goto L13658;
    hud_message_add(*(int *)lock_text_fail);
    return 0;
L13658:;
    skill_add_uses(13, 1);
    if ((player_character->conditions & 0x40) == 0) goto L13694;
    l_14 = player_character->lock_open_chance;
    player_character->conditions &= ~0x40;
    goto L136A3;
L13694:;
    l_14 = player_character->skills[SKILL_LOCKPICKING].value;
L136A3:;
    l_14 -= a2 * 5;
    if (l_14 >= 5) goto L136BB;
    l_14 = 5;
    goto L136C8;
L136BB:;
    if (l_14 <= 95) goto L136C8;
    l_14 = 95;
L136C8:;
    if (rand_range(0, 100) > l_14) goto L13710;
    hud_message_add(*(int *)lock_text_open);
    sound_play(60, player_object, 110);
    guild_count_crime(5, 1);
    return 1;
L13710:;
    if ((player_character->conditions & 0x40) != 0) goto L13731;
    a3->lockpick_skill_tried = player_character->skills[SKILL_LOCKPICKING].value;
L13731:;
    hud_message_add(*(int *)lock_text_fail);
    return 0;
}
