/* char.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_0017573C[];
extern signed char D_001841E3[];
extern signed char current_region;
extern char npc_record_buffer[];
extern char D_0019995C[];
extern signed char D_0019995F;
extern short D_001999AD[];
extern short D_00199B43;

extern struct faction *faction_find_type_in_region(int, short);
extern int faction_find(short);
extern int flats_cfg_find(int);
extern int npc_display_name(struct record *);
extern int rand();
extern int srand();
extern int mc_memset();
extern int mc_strncpy();

int npc_talk_record_build(struct record *a1)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    struct faction *l_1C;

    l_24 = rand();
    l_20 = flats_cfg_find(a1->image);
    l_2C = (int)RECORD_DATA(a1);
    mc_memset((int)npc_record_buffer, 0, 560, (int)D_0017573C, 226, 4);
    if (a1->type == 53) {
        *(short *)D_0019995C = ((int)(unsigned short)*(short *)D_0019995C) | ((((int)(unsigned short)(a1->npc_flags & 16384)) != 0) ? 1 : 0);
    } else if (l_20 != 0 && ((int)(unsigned char)(*(signed char *)((char *)l_20 + 6) & 1)) != 0) {
        *(signed char *)D_0019995C |= 1;
    } else if (l_20 == 0 && ((int)(unsigned char)(*(signed char *)((char *)l_2C + 2) & 16)) != 0) {
        *(signed char *)D_0019995C |= 1;
    }
    D_0019995F = D_001841E3[(int)(unsigned char)current_region];
    if (a1->type == 8) {
        D_00199B43 = *(short *)((char *)l_2C);
    } else {
        D_00199B43 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15)->id;
    }
    if (D_00199B43 == 0) {
        D_00199B43 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15)->id;
    }
    l_1C = (struct faction *)faction_find((int)(short)D_00199B43);
    if (l_1C != 0 && l_1C->type == 4) {
        mc_strncpy((int)npc_record_buffer, (int)l_1C->name, 32, (int)D_0017573C, 245);
    } else {
        mc_strncpy((int)npc_record_buffer, npc_display_name(a1), 32, (int)D_0017573C, 247);
    }
    srand(a1->id);
    for (l_28 = 1; l_28 < 5; l_28++) {
        D_001999AD[l_28] = rand();
    }
    srand(l_24);
    return (int)npc_record_buffer;
}
