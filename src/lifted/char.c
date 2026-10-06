/* char.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern char D_0017573C[];
extern signed char D_001841E3[];
extern signed char current_region;
extern char npc_record_buffer[];
extern char D_0019995C[];
extern signed char D_0019995F;
extern short D_001999AD[];
extern short D_00199B43;

extern struct faction *faction_find_type_in_region(short, short);
extern iptr faction_find(short);
extern iptr flats_cfg_find(int);
extern iptr npc_display_name(struct record *);

iptr npc_talk_record_build(struct record *npc)
{
    char *npc_data;
    int i;
    int saved_seed;
    struct flat_cfg *flat_cfg;
    struct faction *faction;

    saved_seed = rand();
    flat_cfg = (struct flat_cfg *)flats_cfg_find(npc->image);
    npc_data = RECORD_DATA(npc);
    mc_memset(npc_record_buffer, 0, 560, D_0017573C, 226, 4);
    if (npc->type == 53) {
        *(short *)D_0019995C = ((int)(unsigned short)*(short *)D_0019995C) | ((((int)(unsigned short)(npc->npc_flags & 16384)) != 0) ? 1 : 0);
    } else if (flat_cfg != 0 && (flat_cfg->flags & 1) != 0) {
        *(signed char *)D_0019995C |= 1;
    } else if (flat_cfg == 0 && ((int)(unsigned char)(*(signed char *)(npc_data + 2) & 16)) != 0) {
        *(signed char *)D_0019995C |= 1;
    }
    D_0019995F = D_001841E3[(int)(unsigned char)current_region];
    if (npc->type == 8) {
        D_00199B43 = *(short *)npc_data;
    } else {
        D_00199B43 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15)->id;
    }
    if (D_00199B43 == 0) {
        D_00199B43 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15)->id;
    }
    faction = (struct faction *)faction_find((int)(short)D_00199B43);
    if (faction != 0 && faction->type == 4) {
        mc_strncpy(npc_record_buffer, faction->name, 32, D_0017573C, 245);
    } else {
        mc_strncpy(npc_record_buffer, (char *)npc_display_name(npc), 32, D_0017573C, 247);
    }
    srand(npc->id);
    for (i = 1; i < 5; i++) {
        D_001999AD[i] = rand();
    }
    srand(saved_seed);
    return (iptr)npc_record_buffer;
}
