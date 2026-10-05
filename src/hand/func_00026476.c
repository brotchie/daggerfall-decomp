/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00026476 */
#include "records.h"

extern unsigned char player_environment;
extern unsigned loan_collectors_next;
extern struct record *bank_accounts;
extern unsigned game_minutes;
extern void loan_spawn_collectors(void);
extern int rand_range(int, int);

void loan_collectors_update(void)
{
    int i;
    struct bank_account *p;

    if (player_environment == 3) return;
    if (loan_collectors_next > game_minutes) return;
    loan_collectors_next = game_minutes + rand_range(1400, 1700);
    p = bank_accounts->data.bank_accounts;
    for (i = 0; i < 62; i++) {
        if (p->loan_due == 0) continue;
        if (p->loan_due < game_minutes)
            loan_spawn_collectors();
    }
}
