/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006B072 */
#include "records.h"

extern struct record *logbook_object;

void logbook_remove_entry(unsigned char quest_id, int index)
{
    struct logbook *logbook;
    int i;
    int found;

    logbook = &logbook_object->data.logbook;
    found = -1;
    for (i = 0; i < 32; i++) {
        if (quest_id == logbook->quest_ids[i]) {
            found = i;
            break;
        }
    }
    if (found == -1)
        return;
    logbook->message_ids[found][index] = 0;
}
