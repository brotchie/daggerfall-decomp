/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006BF30 */
#include "records.h"

extern struct bank_account *bank_account;
extern void msgbox_show_rsc(int, int);
extern int bank_input_amount(void);
extern void gold_spend(int);
extern int gold_total_alias(void);

void bank_repay_loan(void)
{
    int n;

    if (bank_account->loan_owed == 0)
        return;
    n = bank_input_amount();
    if (n < 1)
        return;
    if (bank_account->balance + gold_total_alias() < n) {
        msgbox_show_rsc(454, 1);
        return;
    }
    if (n > bank_account->loan_owed) {
        n = bank_account->loan_owed;
        msgbox_show_rsc(294, 1);
    }
    bank_account->balance -= n;
    bank_account->loan_owed -= n;
    if (bank_account->balance < 0) {
        gold_spend(-bank_account->balance);
        bank_account->balance = 0;
    }
    if (bank_account->loan_owed == 0)
        bank_account->loan_due = 0;
}
