/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006B072 */
#include "records.h"

struct logbook_messages { short ids[10]; };
struct logbook {                /* the logbook record's data (type 24), up to the message ids */
    short quest_ids[32];
    struct logbook_messages message_ids[32];
};
extern struct record *logbook_object;

void logbook_remove_entry(unsigned char quest_id, int index)
{
    struct logbook *logbook;
    int i;
    int found;

    logbook = (struct logbook *)RECORD_DATA(logbook_object);
    found = -1;
    for (i = 0; i < 32; i++) {
        if (quest_id == logbook->quest_ids[i]) {
            found = i;
            break;
        }
    }
    if (found == -1)
        return;
    logbook->message_ids[found].ids[index] = 0;
}
