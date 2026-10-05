/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003C610 */
#include "records.h"

extern char D_00195B84[];
extern char scratch_buffer[];
extern signed char D_001962A2;
extern char D_001962A7;

void health_status_add(struct record *object)
{
    struct disease *disease;

    if (object->type != 11) return;
    disease = &object->data.disease;
    if (disease->id >= 128 && D_001962A7 == 0 && disease->stage != 0) {
        D_001962A2 = 1;
        (*(char **)scratch_buffer)[*(int *)D_00195B84 + 60000] = 117;
        (*(int *)D_00195B84)++;
        D_001962A7 = 1;
    } else if (disease->id < 100) {
        D_001962A2 = 1;
        if ((object->flags & 0x8000) && disease->stage != 0) {
            (*(char **)scratch_buffer)[*(int *)D_00195B84 + 60000] = disease->id + 100;
            (*(int *)D_00195B84)++;
        }
    }
}
