/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006BF30 */
#include "records.h"

extern struct bank_account *bank_account;
extern void msgbox_show_rsc(int, int);
extern int bank_input_amount(void);
extern void gold_spend(int);
extern int gold_total_alias(void);

void bank_repay_loan(void)
{
    int amount;

    if (bank_account->loan_owed == 0)
        return;
    amount = bank_input_amount();
    if (amount < 1)
        return;
    if (bank_account->balance + gold_total_alias() < amount) {
        msgbox_show_rsc(454, 1);
        return;
    }
    if (amount > bank_account->loan_owed) {
        amount = bank_account->loan_owed;
        msgbox_show_rsc(294, 1);
    }
    bank_account->balance -= amount;
    bank_account->loan_owed -= amount;
    if (bank_account->balance < 0) {
        gold_spend(-bank_account->balance);
        bank_account->balance = 0;
    }
    if (bank_account->loan_owed == 0)
        bank_account->loan_due = 0;
}
