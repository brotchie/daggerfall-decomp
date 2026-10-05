/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00026476 */
#include "records.h"

extern unsigned char player_environment;
extern unsigned D_0019599C;
extern struct record *bank_accounts;
extern unsigned game_minutes;
extern void func_00026508(void);
extern int rand_range(int, int);

void loan_collectors_update(void)
{
    int i;
    struct bank_account *p;

    if (player_environment == 3) return;
    if (D_0019599C > game_minutes) return;
    D_0019599C = game_minutes + rand_range(1400, 1700);
    p = bank_accounts->data.bank_accounts;
    for (i = 0; i < 62; i++) {
        if (p->loan_due == 0) continue;
        if (p->loan_due < game_minutes)
            func_00026508();
    }
}
