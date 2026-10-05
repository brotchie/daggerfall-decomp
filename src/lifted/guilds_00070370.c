/* guilds.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char player_character[];
extern char game_minutes[];

extern int disease_is_lycanthrope(void);

void guild_count_crime(int a1, int a2)
{
    if (a1 != 5) goto L703F6;
    if (*(int *)(*(char **)player_character + 529) != 0) return;
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 546)) == 100) return;
    *(signed char *)(*(char **)player_character + 546) += *(signed char *)&a2;
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 546)) < 6) goto L703F1;
    *(int *)(*(char **)player_character + 529) = *(int *)game_minutes + 4320;
L703F1:;
    return;
L703F6:;
    if (*(int *)(*(char **)player_character + 92) == 0) goto L7040A;
    if (disease_is_lycanthrope() != 0) goto L7040C;
L7040A:;
    goto L7042D;
L7040C:;
    *(int *)(*(char **)player_character + 96) = *(int *)game_minutes;
    *(short *)(*(char **)player_character + 126) = *(short *)(*(char **)player_character + 92);
L7042D:;
    if (*(int *)(*(char **)player_character + 533) != 0) return;
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 543)) == 100) return;
    *(signed char *)(*(char **)player_character + 543) += *(signed char *)&a2;
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 543)) < 15) return;
    *(int *)(*(char **)player_character + 533) = *(int *)game_minutes + 4320;
}
