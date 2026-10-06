/* jmem.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern signed char key_down_enter;
extern char D_00175AD4[];
extern char D_00175ADB[];
extern char D_00175C2D[];
extern char D_00175C79[];
extern signed char D_00187CA8;
extern int mem_check_level;
extern int frame_checkpoint;
extern struct record *player_object;
extern iptr window_image;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern int logbook_first_entry;
extern char logbook_show_notes[];

extern int key_action_held(int);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern void xn_gfx_restore_mode(void);
extern void fatal_error(char *);
extern void sound_shutdown_music(void);
extern void mem_check_heap(int);
extern void logbook_build_entries(void);
#pragma aux func_0009DA1C parm routine [];
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) crash_screen;

void mem_pool_init(struct mem_pool *pool, int size)
{
    struct mem_block *block;

    mc_memset(pool, 0, 4, D_00175AD4, 60, 4);
    pool->first = (struct mem_block *)mc_malloc(size, D_00175AD4, 62);
    block = pool->first;
    if (block == 0) fatal_error(D_00175ADB);
    pool->size = size;
    block->size = size - 18;
    block->prev = 0;
    block->next = block->prev;
    block->flags = 0;
}

void mem_pool_free(struct mem_pool *pool)
{
    if (pool->first == 0 || (iptr)pool->first == (-1751672937)) return;
    mc_free(pool->first, D_00175AD4, 84);
    pool->first = (struct mem_block *)(iptr)-1751672937;
}

iptr mem_pool_alloc(struct mem_pool *pool, int size)
{
    struct mem_block *block;
    struct mem_block *next;
    int block_size;

    block_size = (size + 1) & -2;
    block_size += 18;
    block = pool->first;
    while (block != 0) {
        if ((int)block->size < block_size || ((int)(short)((short)block->flags & 1)) != 0) {
            next = block;
            block = block->next;
        } else {
            break;
        }
    }
    if (block == 0) return 0;
    if ((int)block->size != block_size) {
        next = (struct mem_block *)((iptr)block + block_size);
        next->next = block->next;
        next->prev = block;
        next->size = (int)block->size - block_size;
        next->flags = 0;
        next->magic = 1768515945;
        if (next->next != 0) next->next->prev = next;
    } else {
        next = block->next;
        block_size += 18;
    }
    block->next = next;
    block->size = block_size - 18;
    block->flags |= 1;
    block->magic = 1768515945;
    return (iptr)block + 18;
}

int mem_block_size(char *data)
{
    struct mem_block *block;

    block = (struct mem_block *)(data - 18);
    return block->size;
}

void crash_screen(void)
{
    xn_gfx_restore_mode();
    func_0009DA1C(394, D_00175AD4);
    printf(D_00175C2D, frame_checkpoint);
    while (key_down_enter == 0);
    sound_shutdown_music();
    func_000A2D9E();
}

void mem_check_now(int checkpoint)
{
    mem_check_level++;
    mem_check_heap(checkpoint);
    mem_check_level--;
}

int logbook_open(int force)
{
    if (((int)D_0019626F) == 14 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (force != 0 || (game_mode == 0 && key_action_held(24) != 0)) {
        game_mode = 14;
        D_00196272 = 1;
        window_image = disk_read_file(D_00175C79, 0);
        *(int *)logbook_show_notes = (logbook_first_entry = 0);
        D_00187CA8 = 0;
        logbook_build_entries();
        sound_play(237, player_object, 100);
    }
    return ((((int)(unsigned char)game_mode) == 14) ? 1 : 0);
}
