/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006AED1 */
#include "records.h"

extern char D_00175C86[];
extern struct record *logbook_object;
extern struct location *current_location;
extern int game_minutes;
extern struct quest *quest_find_by_id(int);
extern void logbook_prune_quests(void);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);

void logbook_add_entry(unsigned char quest_id, int message_id, int index)
{
    struct logbook *logbook;
    int i;
    int slot;
    int fresh;
    struct quest *quest;

    logbook = &logbook_object->data.logbook;
    slot = -1;
    fresh = 1;
    logbook_prune_quests();
    for (i = 0; i < 32; i++) {
        if (logbook->quest_ids[i] != 0) {
            quest = quest_find_by_id(logbook->quest_ids[i]);
            if (quest == 0) {
                logbook->quest_ids[i] = 0;
                mc_memset(logbook->message_ids[i], 0, 20, D_00175C86, 301, 20);
            }
        }
        if (logbook->quest_ids[i] == 0 && slot == -1) {
            slot = i;
            continue;
        }
        if (quest_id == logbook->quest_ids[i]) {
            fresh = 0;
            slot = i;
            break;
        }
    }
    if (fresh)
        mc_memset(logbook->message_ids[slot], 0, 20, D_00175C86, 319, 20);
    logbook->quest_ids[slot] = quest_id;
    if (index > 9)
        index %= 10;
    logbook->message_ids[slot][index] = message_id;
    logbook->message_times[slot][index] = game_minutes;
    mc_strncpy(logbook->places[slot], current_location->name, 32, D_00175C86, 325);
}
