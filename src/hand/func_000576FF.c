/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000576FF */
#include "records.h"
extern short scratch_190d64;
extern int scratch_190ee4[];
extern char *scratch_buffer;
extern struct magic_enchantment D_00199868[][5];   /* itemmaker_slot_exclusions */
extern struct enchantment itemmaker_slots[];
extern int itemmaker_param_excluded(int, int);
extern void list_popup_open(int *);

void itemmaker_show_param_list(int *names, short type)
{
    short i;
    unsigned short index;
    short count;
    short param;
    int saved;

    param = index = count = 0;
    while (*names != 0) {
        for (i = 0; i < 12; i++) {
            if (D_00199868[i][0].type == type && D_00199868[i][0].param == param
                || D_00199868[i][1].type == type && D_00199868[i][1].param == param
                || D_00199868[i][2].type == type && D_00199868[i][2].param == param
                || D_00199868[i][3].type == type && D_00199868[i][3].param == param
                || D_00199868[i][4].type == type && D_00199868[i][4].param == param)
                goto next;
        }
        saved = itemmaker_slots[scratch_190d64].type;
        itemmaker_slots[scratch_190d64].type = 100;
        if (itemmaker_param_excluded(type, param) != 0)
            itemmaker_slots[scratch_190d64].type = saved;
        else {
            itemmaker_slots[scratch_190d64].type = saved;
            scratch_buffer[count + 64000] = index;
            scratch_190ee4[count] = *names;
            count++;
        }
next:
        names++;
        index++;
        param++;
    }
    scratch_190ee4[count] = 0;
    list_popup_open(scratch_190ee4);
}
