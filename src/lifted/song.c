/* song.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_001706B6[];
extern char D_001706BD[];
extern char D_001706CA[];
extern char D_001706CD[];
extern char D_00179EA8[];
extern signed char text_buffer[];
extern short D_00190CA2[];
extern int D_00190CD4;
extern int D_00190CD8;
extern int D_00190CDC;
extern signed char D_00190D1F;
extern signed char scratch_190d20;
extern signed char D_00190D21;
extern signed char D_00190D22;
extern char scratch_190d64[];
extern signed char text_rsc_buffer[];
extern struct character *player_character;
extern char scratch_buffer[];

extern int rand_range(int, int);
extern int rand();
extern int srand();
extern int mc_strncpy();
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);
extern int func_000A1054();
extern iptr xn_str_skip_fields();
extern void parse_rsc_text(int, int, int);
extern void object_free_later(struct record *);
#pragma aux mc_set_location parm routine [];

void song_generate(int skip)
{
    int n;
    unsigned char *pattern;

    if (skip != 0) return;
    n = rand();
    srand(n);
    parse_rsc_text(850, 0, 0);
    mc_strncpy((iptr)text_buffer, (iptr)text_rsc_buffer, 160, (iptr)D_001706B6, 42);
    parse_rsc_text(851, 0, 0);
    func_000A1054((iptr)text_buffer, (iptr)text_rsc_buffer, (iptr)D_001706B6, 44, 160);
    mc_set_location(45, (iptr)D_001706B6);
    mc_sprintf((iptr)text_rsc_buffer, (iptr)D_001706BD, (iptr)text_buffer, n);
    mc_strncpy(*(int *)scratch_buffer + 50000, (iptr)text_rsc_buffer, 4, (iptr)D_001706B6, 46);
    for (n = 0; n < 26; n++) {
        *(short *)(scratch_190d64 + (n * 2)) = rand_range(0, 21) + 900;
    }
    n = rand() % 10;
    pattern = (unsigned char *)xn_str_skip_fields(*(int *)D_00179EA8, 33, n);
    while (*pattern != 33) {
        parse_rsc_text((int)(short)D_00190CA2[*pattern++], 0, 0);
        func_000A1054(*(int *)scratch_buffer + 50000, (iptr)text_rsc_buffer, (iptr)D_001706B6, 56, 4);
        func_000A1054(*(int *)scratch_buffer + 50000, (iptr)D_001706CA, (iptr)D_001706B6, 57, 4);
    }
    func_000A1054(*(int *)scratch_buffer + 50000, (iptr)D_001706CD, (iptr)D_001706B6, 59, 4);
}

void song_init_heroes(int player_hero)
{
    {
        int gender;

        D_00190D1F = rand() & -255;
        if (player_hero != 0) {
            D_00190CD4 = 0;
            scratch_190d20 = (signed char)player_character->flags & 1;
        } else {
            D_00190CD4 = rand();
            scratch_190d20 = rand() & -255;
        }
        D_00190CD8 = rand();
        if (scratch_190d20 != 0) {
            gender = 0;
        } else {
            gender = 1;
        }
        D_00190D21 = *(signed char *)&gender;
        D_00190CDC = rand();
        D_00190D22 = 0;
    }
}

void crime_remove_monster(struct record *object)
{
    if (object->type != 18) return;
    object_free_later(object);
}
