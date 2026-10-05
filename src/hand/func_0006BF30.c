/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006BF30 */
struct acct { int f0; int f4; int f8; };
extern struct acct *bank_account;
extern void msgbox_show_rsc(int, int);
extern int bank_input_amount(void);
extern void gold_spend(int);
extern int gold_total_alias(void);

void bank_repay_loan(void)
{
    int n;

    if (bank_account->f4 == 0)
        return;
    n = bank_input_amount();
    if (n < 1)
        return;
    if (bank_account->f0 + gold_total_alias() < n) {
        msgbox_show_rsc(454, 1);
        return;
    }
    if (n > bank_account->f4) {
        n = bank_account->f4;
        msgbox_show_rsc(294, 1);
    }
    bank_account->f0 -= n;
    bank_account->f4 -= n;
    if (bank_account->f0 < 0) {
        gold_spend(-bank_account->f0);
        bank_account->f0 = 0;
    }
    if (bank_account->f4 == 0)
        bank_account->f8 = 0;
}
