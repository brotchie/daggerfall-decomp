/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char quest_faces[];
extern char D_00195A15[];
extern char D_00195A16[];
extern char D_00195A1A[];
extern int D_00195D04[];
extern int D_00195D14;
extern struct quest *current_quest;

extern int rand_range(int, int);

void quest_face_add(struct record *a1, int a2, int a3, int a4)
{
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    short l_C;

    if (a1->type == 18) {
        *(int *)&l_C = (int)&a1->data.character;
        a3 = ((((int)(unsigned short)(*(short *)(*(char **)&l_C + 64) & 1)) != 0) ? 1 : 0);
    } else if (a3 != 0) {
        a3 = 1;
    }
    l_1C = 0;
    while (*(int *)(D_00195A16 + (l_1C * 10)) != 0 && l_1C < 10) l_1C++;
    if (l_1C >= 10) return;
    *(int *)(D_00195A16 + (l_1C * 10)) = a4;
    *(signed char *)(D_00195A15 + (l_1C * 10)) = (signed char)current_quest->id;
    if (a1->type != 18 && a1->data.building.faction_id == 514) {
        l_18 = a3 + (a2 * 2);
        quest_faces[l_1C * 10] = (((*(signed char *)&a3 << 7) + (*(signed char *)&a2 << 6)) + *(signed char *)&l_18) | 16;
        l_10 = D_00195D14;
    } else {
        quest_faces[l_1C * 10] = ((*(signed char *)&a3 << 7) + (*(signed char *)&a2 << 6)) + rand_range(0, 9);
        l_10 = D_00195D04[((a3 * 2) + a2)];
    }
    l_14 = 0;
    l_18 = (int)(unsigned char)(quest_faces[l_1C * 10] & 15);
    while (l_14 < l_18) {
        l_10 = (((int)(unsigned short)*(short *)((char *)l_10 + 10)) + l_10) + 12;
        l_14++;
    }
    *(int *)(D_00195A1A + (l_1C * 10)) = l_10;
}
