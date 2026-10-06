/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005978F */
#include "records.h"
#include "clib.h"

extern char D_0017573C[];
extern struct character *player_character;
extern unsigned char *paperdoll_slots;
extern unsigned char **paperdoll_items;

/* item +0x42 (the top byte of `message`) is the paperdoll drawing order here */
void paperdoll_add_item(struct item *item, int slot)
{
    int pos;
    int image_file;

    pos = 0;
    if (item->group == 3) {
        image_file = item->inventory_image >> 7;
        if (image_file == 432 || image_file == 433) {
        } else if (player_character->flags & 1) {
            item->inventory_image -= 128;
        }
        if (slot == 19) {
            if (item->item_flags & 4)
                item->inventory_image++;
        } else if (slot == 21) {
            if (item->item_flags & 4)
                ((unsigned char *)item)[66] += 5;
        }
    }
    while (paperdoll_items[pos] != 0 && ((unsigned char *)item)[66] > paperdoll_items[pos][66])
        pos++;
    mc_memmove(&paperdoll_slots[pos + 1], &paperdoll_slots[pos], 27, D_0017573C, 201, 4);
    mc_memmove(&paperdoll_items[pos + 1], &paperdoll_items[pos], 108, D_0017573C, 202, 4);
    paperdoll_items[pos] = (unsigned char *)item;
    paperdoll_slots[pos] = slot;
}
