/* archive.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern char D_00170150[];
extern char D_0017015A[];
extern char D_00170172[];
extern int lock_text_fail;
extern int lock_text_open;
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

    if (a3 != 0) {
        do {
            *(int *)&l_1C = disk_open_rw(a1);
        } while (*(int *)&l_1C < 1);
    } else {
        *(int *)&l_1C = disk_open_data(a1);
    }
    if (*(int *)&l_1C < 1) return *(int *)&l_1C;
    mc_strncpy(((int)archive_names) + (*(int *)&l_1C * 13), a1, 13, (int)D_00170150, 32);
    func_000A00CB(*(int *)&l_1C, (int)&l_10, 2);
    func_000A00CB(*(int *)&l_1C, (int)&l_14, 2);
    if (((int)(short)l_14) == 256) {
        *(int *)&l_18 = ((int)(short)l_10) * 18;
    } else {
        *(int *)&l_18 = ((int)(short)l_10) << 3;
    }
    if (a2 == 0) a2 = mc_malloc(*(int *)&l_18, (int)D_00170150, 42);
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
    if (*(int *)(archive_directories + (a1 << 2)) != 0 && *(int *)(archive_directories + (a1 << 2)) != (-1751672937)) {
        mc_free(*(int *)(archive_directories + (a1 << 2)), (int)D_00170150, 67);
        *(int *)(archive_directories + (a1 << 2)) = -1751672937;
    }
    *(int *)(archive_directories + (a1 << 2)) = 0;
    *(short *)(archive_types + (a1 * 2)) = 0;
    func_0009DEA7(a1);
}

int archive_find_record(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    if (((int)(short)*(short *)(archive_types + (a1 * 2))) == 256) {
        l_1C = *(int *)(archive_directories + (a1 << 2));
        for (l_14 = 0; ((int)(short)*(short *)(archive_record_counts + (a1 * 2))) > l_14; l_14++, (*(char (**)[18])&l_1C)++) {
            if (strnicmp(a2, l_1C, a3) == 0) return l_14;
        }
    } else {
        l_18 = *(int *)(archive_directories + (a1 << 2));
        for (l_14 = 0; ((int)(short)*(short *)(archive_record_counts + (a1 * 2))) > l_14; l_14++, (*(char (**)[8])&l_18)++) {
            if (a3 == *(int *)((char *)l_18)) return l_14;
        }
    }
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) == 256) {
        func_000A0ED9(105, (int)D_00170150);
        mc_sprintf(*(int *)D_00195C44, (int)D_0017015A, a2, ((int)archive_names) + (a1 * 13));
    } else {
        func_000A0ED9(107, (int)D_00170150);
        mc_sprintf(*(int *)D_00195C44, (int)D_00170172, a3, ((int)archive_names) + (a1 * 13));
    }
    fatal_error(*(int *)D_00195C44);
    return 0;
}

int archive_record_size(int a1, int a2)
{
    int l_1C;
    int l_18;

    if (((int)(short)*(short *)(archive_types + (a1 * 2))) == 256) {
        l_1C = *(int *)(archive_directories + (a1 << 2));
        l_1C += a2 * 18;
        return *(int *)((char *)l_1C + 14);
    }
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
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) == 256) {
        l_24 = *(int *)(archive_directories + (a1 << 2));
        for (l_1C = 0; l_1C < a2; l_1C++, (*(char (**)[18])&l_24)++) {
            l_18 += *(int *)((char *)l_24 + 14);
        }
        return l_18;
    }
    l_20 = *(int *)(archive_directories + (a1 << 2));
    for (l_1C = 0; l_1C < a2; l_1C++, (*(char (**)[8])&l_20)++) {
        l_18 += *(int *)((char *)l_20 + 4);
    }
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
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) == 256) {
        l_24 = *(int *)(archive_directories + (a1 << 2));
        for (l_1C = 0; l_1C < a2; l_1C++, (*(char (**)[18])&l_24)++) {
            l_14 += *(int *)((char *)l_24 + 14);
        }
        l_18 = *(int *)((char *)l_24 + 14);
    } else {
        l_20 = *(int *)(archive_directories + (a1 << 2));
        for (l_1C = 0; l_1C < a2; l_1C++, (*(char (**)[8])&l_20)++) {
            l_14 += *(int *)((char *)l_20 + 4);
        }
        l_18 = *(int *)((char *)l_20 + 4);
    }
    if (a3 == 0) a3 = mc_malloc(l_18, (int)D_00170150, 205);
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
    if (((int)(short)*(short *)(archive_types + (a1 * 2))) == 256) {
        l_20 = *(int *)(archive_directories + (a1 << 2));
        for (l_18 = 0; l_18 < a2; l_18++, (*(char (**)[18])&l_20)++) {
            l_10 += *(int *)((char *)l_20 + 14);
        }
        l_14 = *(int *)((char *)l_20 + 14);
    } else {
        l_1C = *(int *)(archive_directories + (a1 << 2));
        for (l_18 = 0; l_18 < a2; l_18++, (*(char (**)[8])&l_1C)++) {
            l_10 += *(int *)((char *)l_1C + 4);
        }
        l_14 = *(int *)((char *)l_1C + 4);
    }
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

    if (a1->lockpick_skill_tried == player_character->skills[SKILL_LOCKPICKING].value) return 0;
    if (a1->lock_level >= 20) {
        hud_message_add(lock_text_fail);
        links_trigger(a1, 4);
        return 0;
    }
    skill_add_uses(13, 1);
    if ((player_character->conditions & 0x40) != 0) {
        l_1C = (int)(unsigned char)(signed char)player_character->lock_open_chance;
        player_character->conditions &= ~0x40;
    } else {
        l_1C = (int)(short)player_character->skills[SKILL_LOCKPICKING].value;
    }
    l_1C += (((int)(unsigned char)(signed char)player_character->level) - a1->lock_level) * 5;
    if (l_1C < 5) {
        l_1C = 5;
    } else if (l_1C > 95) {
        l_1C = 95;
    }
    if (rand_range(0, 100) <= l_1C) {
        a1->flags |= 64;
        hud_message_add(lock_text_open);
        links_trigger(a1, 7);
        sound_play(60, a1, 100);
        return 1;
    }
    if ((player_character->conditions & 0x40) == 0) {
        a1->lockpick_skill_tried = player_character->skills[SKILL_LOCKPICKING].value;
    }
    hud_message_add(lock_text_fail);
    links_trigger(a1, 4);
    return 0;
}

int lockpick_action_door(int a1, int a2, struct record *a3)
{
    int l_14;

    if (((int)(unsigned char)*(signed char *)((char *)a1 + 8)) >= 10) return 1;
    if (a3->lockpick_skill_tried == player_character->skills[SKILL_LOCKPICKING].value) return 0;
    if (a2 >= 20) {
        hud_message_add(lock_text_fail);
        return 0;
    }
    skill_add_uses(13, 1);
    if ((player_character->conditions & 0x40) != 0) {
        l_14 = player_character->lock_open_chance;
        player_character->conditions &= ~0x40;
    } else {
        l_14 = player_character->skills[SKILL_LOCKPICKING].value;
    }
    l_14 -= a2 * 5;
    if (l_14 < 5) {
        l_14 = 5;
    } else if (l_14 > 95) {
        l_14 = 95;
    }
    if (rand_range(0, 100) <= l_14) {
        hud_message_add(lock_text_open);
        sound_play(60, player_object, 110);
        guild_count_crime(5, 1);
        return 1;
    }
    if ((player_character->conditions & 0x40) == 0) {
        a3->lockpick_skill_tried = player_character->skills[SKILL_LOCKPICKING].value;
    }
    hud_message_add(lock_text_fail);
    return 0;
}
