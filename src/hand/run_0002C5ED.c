/* matched by the real Watcom C32 10.0a (-d2): a run of qcom.c from 0x0002C4FA to 0x0002C5ED, kept together for its switch table's alignment */
#include "records.h"

/* a timer (struct qbn_timer) as this code types it: signed flags, an unsigned delay and the
 * links as object pointers (records.h has int link1/link2 and int delay) */
struct task {
    char pad0[2];
    short flags;
    unsigned char type;
    int arg1;
    int arg2;
    int start;
    unsigned int delay;
    struct record *who;
    struct record *whom;
    char pad29[4];
};
extern short travel_options;
extern char D_0017A13C[];
extern int game_minutes;
extern struct quest *current_quest;
extern void quest_timer_update(struct quest *, struct task *, short);
extern struct record *func_0002C96B(struct quest *, struct record *, short);
extern void quest_timer_expire(struct quest *, struct task *);
extern void func_0002CAB0(struct task *);
extern unsigned int quest_travel_minutes(struct quest *, struct record *, struct record *);
extern struct task *quest_section(struct quest *, int);
extern int rand_range(int, int);
extern int memchr(char *, int, int);

void quest_timers_update(struct quest *a1)
{
    int i;
    struct task *t;

    t = quest_section(a1, 6);
    for (i = 0; a1->section_counts[6] > i; i++, t++) {
        if (t->flags & 2) {
            t->flags &= ~128;
            func_0002CAB0(t);
        }
        if (t->flags & 64)
            quest_timer_update(a1, t, 0);
    }
}

void quest_timers_start_all(void)
{
    struct task *t;
    int i;

    t = quest_section(current_quest, 6);
    for (i = 0; current_quest->section_counts[6] > i; i++, t++)
        quest_timer_update(current_quest, t, 1);
}

void quest_timer_update(struct quest *a1, struct task *a2, short a3)
{
    int l_18;
    short saved;

    saved = travel_options;
    if (a3) {
        travel_options = 537;
        if (!(a2->flags & 1024)) {
            a2->flags |= 1024;
            switch (a2->type) {
            case 2:
            case 4:
                a2->who = func_0002C96B(a1, a2->who, a2->flags & 256);
                a2->whom = 0;
                break;
            case 3:
            case 5:
                a2->who = func_0002C96B(a1, a2->who, a2->flags & 256);
                a2->whom = func_0002C96B(a1, a2->whom, a2->flags & 512);
                break;
            default:
                a2->who = a2->whom = 0;
                break;
            }
        }
        a2->start = game_minutes;
        switch (a2->type) {
        case 0:
            a2->delay = rand_range(a2->arg1, a2->arg2);
            break;
        case 1:
            a2->delay = a2->arg1;
            break;
        case 2:
            a2->delay = quest_travel_minutes(a1, 0, a2->who) * 384 >> 8;
            if (memchr(D_0017A13C, a2->who->link_flag, 5))
                a2->delay += 10080;
            break;
        case 3:
            a2->delay = quest_travel_minutes(a1, a2->who, a2->whom) * 384 >> 8;
            if (memchr(D_0017A13C, a2->who->link_flag, 5) || memchr(D_0017A13C, a2->whom->link_flag, 5))
                a2->delay += 10080;
            break;
        case 4:
            a2->delay = quest_travel_minutes(a1, 0, a2->who) * 384 >> 8;
            if (memchr(D_0017A13C, a2->who->link_flag, 5))
                a2->delay += 10080;
            break;
        case 5:
            a2->delay = quest_travel_minutes(a1, 0, a2->who) * 384 >> 8;
            a2->delay += quest_travel_minutes(a1, a2->who, a2->whom) * 384 >> 8;
            if (memchr(D_0017A13C, a2->who->link_flag, 5))
                a2->delay += 10080;
            if (memchr(D_0017A13C, a2->whom->link_flag, 5))
                a2->delay += 10080;
            break;
        }
        if (a2->flags & 16)
            a2->delay <<= 1;
        travel_options = saved;
    } else if (game_minutes - a2->start > a2->delay) {
        quest_timer_expire(a1, a2);
    }
}
