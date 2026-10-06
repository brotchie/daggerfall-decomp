/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CCC8 */
#include "records.h"
#include "clib.h"

#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
#pragma pack()
extern char D_00176E38[];
extern struct bits8 D_001940D8;
extern void picklist_update_thumb(struct picklist *);

void picklist_add(struct picklist *l, char *name, short flags)
{
    int unused;
    short i;

    if (D_001940D8.b0) {
        if (l->count == 0) {
            mc_strncpy(l->entries->text, name, 40, D_00176E38, 70);
            l->entries->flags = flags;
            l->entries->index = l->count;
        } else {
            for (i = 0; i < l->count; i++) {
                if (stricmp(l->entries[i].text, name) >= 0) {
                    mc_memmove(&l->entries[i + 1], &l->entries[i], (l->count - i) * 44, D_00176E38, 80, 4);
                    mc_strncpy(l->entries[i].text, name, 40, D_00176E38, 81);
                    l->entries[i].flags = flags;
                    l->entries[i].index = l->count;
                    goto done;
                }
            }
            mc_strncpy(l->entries[l->count].text, name, 40, D_00176E38, 87);
            l->entries[l->count].flags = flags;
            l->entries[l->count].index = l->count;
        }
    } else {
        mc_strncpy(l->entries[l->count].text, name, 40, D_00176E38, 94);
        l->entries[l->count].flags = flags;
        l->entries[l->count].index = l->count;
    }
done:
    l->count++;
    picklist_update_thumb(l);
}
