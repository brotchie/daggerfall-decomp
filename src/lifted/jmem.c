/* jmem.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char key_down_enter[];
extern char D_00175AD4[];
extern char D_00175ADB[];
extern char D_00175C2D[];
extern char D_00175C79[];
extern char D_00187CA8[];
extern char mem_check_level[];
extern char frame_checkpoint[];
extern struct record *player_object;
extern char window_image[];
extern char D_0019626F[];
extern char D_00196272[];
extern char game_mode[];
extern char logbook_first_entry[];
extern char logbook_show_notes[];

extern int key_action_held(int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int func_0009DA1C(int, int);
extern int printf(int, ...);
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int func_000A2D9E();
extern int func_00143700();
extern void fatal_error(int);
extern void func_00068B1B(void);
extern void mem_check_heap(int);
extern void logbook_build_entries(void);
#pragma aux func_0009DA1C parm routine [];
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) crash_screen;

void mem_pool_init(struct mem_pool *a1, int a2)
{
    struct mem_block *l_14;

    mc_memset((int)a1, 0, 4, (int)D_00175AD4, 60, 4);
    a1->first = (struct mem_block *)mc_malloc(a2, (int)D_00175AD4, 62);
    l_14 = a1->first;
    if (l_14 != 0) goto L69EFC;
    fatal_error((int)D_00175ADB);
L69EFC:;
    a1->size = a2;
    l_14->size = a2 - 18;
    l_14->prev = 0;
    l_14->next = l_14->prev;
    l_14->flags = 0;
}

void mem_pool_free(struct mem_pool *a1)
{
    if (a1->first == 0) goto L69F5F;
    if ((int)a1->first != (-1751672937)) goto L69F61;
L69F5F:;
    return;
L69F61:;
    mc_free((int)a1->first, (int)D_00175AD4, 84);
    a1->first = (struct mem_block *)-1751672937;
}

int mem_pool_alloc(struct mem_pool *a1, int a2)
{
    struct mem_block *l_20;
    struct mem_block *l_1C;
    int l_18;

    l_18 = (a2 + 1) & -2;
    l_18 += 18;
    l_20 = a1->first;
L69FB3:;
    if (l_20 == 0) goto L69FEA;
    if ((int)l_20->size < l_18) goto L69FD5;
    if (((int)(short)((short)l_20->flags & 1)) == 0) goto L69FE6;
L69FD5:;
    l_1C = l_20;
    l_20 = l_20->next;
    goto L69FE8;
L69FE6:;
    goto L69FEA;
L69FE8:;
    goto L69FB3;
L69FEA:;
    if (l_20 != 0) goto L69FFC;
    return 0;
L69FFC:;
    if ((int)l_20->size == l_18) goto L6A05D;
    l_1C = (struct mem_block *)((int)l_20 + l_18);
    l_1C->next = l_20->next;
    l_1C->prev = l_20;
    l_1C->size = (int)l_20->size - l_18;
    l_1C->flags = 0;
    l_1C->magic = 1768515945;
    if (l_1C->next == 0) goto L6A05B;
    l_1C->next->prev = l_1C;
L6A05B:;
    goto L6A06A;
L6A05D:;
    l_1C = l_20->next;
    l_18 += 18;
L6A06A:;
    l_20->next = l_1C;
    l_20->size = l_18 - 18;
    l_20->flags |= 1;
    l_20->magic = 1768515945;
    return (int)l_20 + 18;
}

int mem_block_size(int a1)
{
    struct mem_block *l_1C;

    l_1C = (struct mem_block *)(a1 - 18);
    return l_1C->size;
}

void crash_screen(void)
{
    func_00143700();
    func_0009DA1C(394, (int)D_00175AD4);
    printf((int)D_00175C2D, *(int *)frame_checkpoint);
L6A63A:;
    if (*(signed char *)key_down_enter == 0) goto L6A63A;
    func_00068B1B();
    func_000A2D9E();
}

void mem_check_now(int a1)
{
    (*(int *)mem_check_level)++;
    mem_check_heap(a1);
    (*(int *)mem_check_level)--;
}

int logbook_open(int a1)
{
    if (((int)(unsigned char)*(signed char *)D_0019626F) != 14) goto L6A6AC;
    if (((int)(unsigned char)*(signed char *)game_mode) == 8) goto L6A6AE;
L6A6AC:;
    goto L6A6BA;
L6A6AE:;
    return 1;
L6A6BA:;
    if (a1 != 0) goto L6A6D9;
    if (*(signed char *)game_mode != 0) goto L6A6D7;
    if (key_action_held(24) != 0) goto L6A6D9;
L6A6D7:;
    goto L6A72D;
L6A6D9:;
    *(signed char *)game_mode = 14;
    *(signed char *)D_00196272 = 1;
    *(int *)window_image = disk_read_file((int)D_00175C79, 0);
    *(int *)logbook_show_notes = (*(int *)logbook_first_entry = 0);
    *(signed char *)D_00187CA8 = 0;
    logbook_build_entries();
    sound_play(237, (int)player_object, 100);
L6A72D:;
    return ((((int)(unsigned char)*(signed char *)game_mode) == 14) ? 1 : 0);
}
