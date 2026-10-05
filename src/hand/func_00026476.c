/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00026476 */
extern unsigned char player_environment;
extern unsigned D_0019599C;
extern char *bank_accounts;
extern unsigned game_minutes;
extern void func_00026508(void);
extern int rand_range(int, int);

void loan_collectors_update(void)
{
    int i;
    char *p;

    if (player_environment == 3) return;
    if (D_0019599C > game_minutes) return;
    D_0019599C = game_minutes + rand_range(1400, 1700);
    p = bank_accounts + 71;
    for (i = 0; i < 62; i++) {
        if (*(unsigned *)(p + 8) == 0) continue;
        if (*(unsigned *)(p + 8) < game_minutes)
            func_00026508();
    }
}
