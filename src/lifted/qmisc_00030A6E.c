/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char quest_faces[];
extern char D_00195A15[];
extern char D_00195A16[];
extern char D_00195A1A[];
extern char D_00195D04[];
extern char D_00195D14[];
extern struct quest *current_quest;

extern int rand_range(int, int);

void quest_face_add(struct record *a1, int a2, int a3, int a4)
{
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    short l_C;

    if (a1->type != 18) goto L30ACA;
    *(int *)&l_C = (int)&a1->data.character;
    a3 = ((((int)(unsigned short)(*(short *)(*(char **)&l_C + 64) & 1)) != 0) ? 1 : 0);
    goto L30AD7;
L30ACA:;
    if (a3 == 0) goto L30AD7;
    a3 = 1;
L30AD7:;
    l_1C = 0;
L30ADE:;
    if (*(int *)(D_00195A16 + (l_1C * 10)) == 0) goto L30AF1;
    if (l_1C < 10) goto L30AF3;
L30AF1:;
    goto L30AFB;
L30AF3:;
    l_1C++;
    goto L30ADE;
L30AFB:;
    if (l_1C >= 10) return;
    *(int *)(D_00195A16 + (l_1C * 10)) = a4;
    *(signed char *)(D_00195A15 + (l_1C * 10)) = (signed char)current_quest->id;
    if (a1->type == 18) goto L30B46;
    if (((int)(unsigned short)*(short *)((char *)a1 + 89)) == 514) goto L30B48;
L30B46:;
    goto L30B7E;
L30B48:;
    l_18 = a3 + (a2 * 2);
    *(signed char *)(quest_faces + (l_1C * 10)) = (((*(signed char *)&a3 << 7) + (*(signed char *)&a2 << 6)) + *(signed char *)&l_18) | 16;
    l_10 = *(int *)D_00195D14;
    goto L30BBC;
L30B7E:;
    *(signed char *)(quest_faces + (l_1C * 10)) = ((*(signed char *)&a3 << 7) + (*(signed char *)&a2 << 6)) + rand_range(0, 9);
    l_10 = *(int *)(D_00195D04 + (((a3 * 2) + a2) << 2));
L30BBC:;
    l_14 = 0;
    l_18 = (int)(unsigned char)(*(signed char *)(quest_faces + (l_1C * 10)) & 15);
L30BD7:;
    if (l_14 >= l_18) goto L30BFC;
    l_10 = (((int)(unsigned short)*(short *)((char *)l_10 + 10)) + l_10) + 12;
    l_14++;
    goto L30BD7;
L30BFC:;
    *(int *)(D_00195A1A + (l_1C * 10)) = l_10;
}
