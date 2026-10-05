/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char player_entity[];

extern void damage_spawn_splash(int, int, int);
extern void cast_spell_on(int, int, int);
extern void cast_creature_missile(int, int, int);

int cast_creature_spell_at(int a1, int a2, int a3)
{
    int l_14;

__dagger_tbl5B11F:;
    l_14 = a1 + 71;
    *(int *)((char *)a1 + 47) = a2;
    switch (*(unsigned char *)((char *)l_14 + 7)) {
case 0:
    cast_spell_on(a1, a2, 0);
    if (a2 == *(int *)player_entity) goto L5B191;
    damage_spawn_splash(a2, 3, 3);
L5B191:;
    return 1;
case 1:
    cast_spell_on(a1, a3, 0);
    return 1;
case 2:
    cast_creature_missile(a1, a2, a3);
    return 0;
case 3:
    cast_spell_on(a1, a3, 0);
    return 1;
case 4:
    cast_creature_missile(a1, a2, a3);
    return 0;
default:
    return 1;
}
}
