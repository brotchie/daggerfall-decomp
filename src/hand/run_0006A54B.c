/* matched by the real Watcom C32 10.0a (-d2): a run of jmem.c from 0x0006A0D4 to 0x0006A54B, kept together for its switch table's alignment */
#include "records.h"

struct msg { int a; short b; char pad[10]; };
extern char *screen_buffer;
extern char D_00175AD4[];       /* __FILE__ */
extern char D_00175B02[];
extern char D_00175B1A[];
extern char D_00175B39[];
extern char D_00175B51[];
extern char D_00175B63[];
extern char D_00175B79[];
extern char D_00175B91[];
extern char D_00175BA8[];
extern char D_00175BC1[];
extern char D_00175BDC[];
extern char D_00175BFA[];
extern char D_00175C18[];
extern int mem_check_level;
extern int frame_checkpoint;
extern int engine_running;
extern struct record *nonworld_root;
extern struct record *quest_root;
extern struct record *location_object;
extern struct mem_block *object_heap_blocks;
extern int object_heap_size;
extern void debug_checkpoint(int);
extern struct qbn_place *quest_section(struct quest *, int);
extern void fatal_error(char *);
extern void object_foreach(struct record *, void (*)(struct record *));
extern struct record *object_find_by_id(struct record *, int);
extern int mc_memset();
extern int func_000A2A2B(void);
extern int func_000A2A76(struct msg *);
extern int xn_sys_zero_page_check();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
#pragma aux func_000A29BA parm routine [];
extern int func_000A29BA(char *);
extern int func_000A148C(char *, ...);

int mem_pool_release(char *data)
{
    struct mem_block *block;
    struct mem_block *neighbour;
    int size;

    block = (struct mem_block *)(data - 18);
    if (block->magic != 1768515945 || ((short)block->flags & ~1) != 0)
        fatal_error(D_00175B02);
    *(unsigned char *)&block->flags &= 254;
    size = block->size;
    mc_memset(data, 150, size, D_00175AD4, 182, 4);
    if (block->next != 0) {
        if (!((short)block->next->flags & 1)) {
            neighbour = block->next;
            block->size += neighbour->size + 18;
            block->next = neighbour->next;
            if (block->next != 0)
                block->next->prev = block;
        }
    }
    if (block->prev != 0) {
        if (!((short)block->prev->flags & 1)) {
            neighbour = block->prev;
            neighbour->size += block->size + 18;
            neighbour->next = block->next;
            if (neighbour->next != 0)
                neighbour->next->prev = neighbour;
        }
    }
    return size;
}

void mem_check_quest_object_cb(struct record *object)
{
    int id;

    if (object->twin != 0 && object->type != 2) {
        if (object_find_by_id(location_object, object->twin->id) == 0)
            fatal_error(D_00175B1A);
    }
    id = object->id;
    object->id = 0;
    if (object_find_by_id(nonworld_root, id) != 0)
        fatal_error(D_00175B39);
    object->id = id;
}

void mem_check_quest_ids_cb(struct record *object)
{
    struct qbn_place *place;
    struct quest *quest;
    int i;

    if (object->type != 14)
        return;
    quest = &object->data.quest;
    place = quest_section(quest, 4);
    for (i = 0; quest->section_counts[4] > i; i++, place++) {
        if (place->object != 0 && object->quest_id != place->object->quest_id)
            fatal_error(D_00175B51);
    }
}

void mem_check_heap(int checkpoint)
{
    struct mem_block *block;
    struct mem_block *prev;

    frame_checkpoint = checkpoint;
    if (mem_check_level == 0)
        return;
    mc_set_location(280, D_00175AD4);
    if (func_000A29BA(screen_buffer) != 0)
        fatal_error(D_00175B63);
    debug_checkpoint(checkpoint);
    if (engine_running != 0)
        xn_sys_zero_page_check(checkpoint);
    object_foreach(nonworld_root, mem_check_quest_object_cb);
    object_foreach(quest_root->children, mem_check_quest_ids_cb);
    prev = block = object_heap_blocks;
    while (block != 0) {
        if (block->magic != 1768515945) {
            mc_set_location(300, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175B91);
        }
        if (block->next != 0 && (char *)block + 18 + block->size != (char *)block->next) {
            mc_set_location(306, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)block)[18]);
            fatal_error(D_00175BA8);
        }
        if (block->size == 0 || (int)block->size > object_heap_size) {
            mc_set_location(312, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175BC1);
        }
        if (block < object_heap_blocks || (int)object_heap_blocks + object_heap_size < (int)block) {
            mc_set_location(318, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175BDC);
        }
        prev = block;
        block = block->next;
        if (block != 0 && block->prev != prev) {
            mc_set_location(327, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175BFA);
        }
    }
}

void mem_check_crt_heap(int checkpoint)
{
    int status;
    struct msg info;

    status = 0;
    if (mem_check_level == 0)
        return;
    mc_set_location(349, D_00175AD4);
    func_000A2A2B();
    info.b = 0;
    info.a = 0;
    while (status == 0)
        status = func_000A2A76(&info);
    switch (status) {
    case 4:
        break;
    case 1:
        break;
    case 2:
        fatal_error(D_00175C18);
        break;
    case 5:
        fatal_error(D_00175C18);
        break;
    case 3:
        fatal_error(D_00175C18);
        break;
    }
}
