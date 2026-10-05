/* fs2df.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170AB4[];
extern char D_00170AEC[];
extern char D_00170AF9[];
extern char D_00170B06[];
extern signed char D_001940D8;
extern int magic_window_image;
extern int window_image;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern char D_001985D4[];
extern char D_001985D8[];
extern struct link *D_001995E4;
extern char *D_001995E8;
extern struct link *D_001995EC;
extern char D_001995F0[];
extern char D_001995F4[];
extern char D_00199600[];
extern char D_00199604[];
extern char D_0019960C[];
extern int D_00199614;
extern short D_00199618;
extern int spellmaker_settings_image;
extern char links[];
extern int link_count;

extern int spellmaker_new(void);
extern int disk_read_file(int, int);
extern int mc_memset();
short rdb_object_id_by_offset(int);
void func_000361B7(int);
void rdb_build_action_chain(int);
void action_record_add(int, int, int, unsigned char);
void action_record_add_chained(int, int, int, unsigned char);
void action_axis_to_translation(struct link *);

void func_000361B7(int model)
{
    int entry;
    int key;

    key = 0;
    entry = (int)(D_001995E8 + *(int *)(*(char **)D_00199604 + 9020));
    while (((int)(short)*(short *)((char *)entry + 4)) != key) {
        entry = (int)(D_001995E8 + *(int *)((char *)entry));
    }
    *(int *)((char *)model + 19) = *(int *)((char *)entry + 6);
    *(short *)((char *)model + 14) = *(short *)((char *)entry + 10);
    *(signed char *)((char *)model + 18) = *(signed char *)((char *)entry + 12);
}

void rdb_link_actions(struct record *quarter, int rdb_object, int block_index)
{
    int unused1;
    int unused2;
    int offset;

    do {
        offset = rdb_object - (int)D_001995E8;
        switch ((unsigned char)(*(signed char *)((char *)rdb_object + 20) & 63)) {
        case 1:
            if (*(int *)((char *)(*(int *)D_0019960C = (int)(D_001995E8 + *(int *)((char *)rdb_object + 21))) + 19) < 0) {
                func_000361B7(*(int *)D_0019960C);
            }
            if (*(short *)(*(char **)D_0019960C + 14) != 0) {
                *(int *)D_00199600 = (int)(D_001995E8 + *(int *)(*(char **)D_0019960C + 19));
                D_00199618 = rdb_object_id_by_offset(offset);
                rdb_build_action_chain((int)(unsigned char)(*(signed char *)((char *)rdb_object + 20) & 63));
            }
            break;
        case 2:
            *(int *)D_001995F4 = (int)(D_001995E8 + *(int *)((char *)rdb_object + 21));
            break;
        case 3:
            if ((*(short *)((char *)(*(int *)D_001995F0 = (int)(D_001995E8 + *(int *)((char *)rdb_object + 21))) + 2) != 0 && ((int)(unsigned short)*(short *)(*(char **)D_001995F0)) != 25482) || (((int)(unsigned short)*(short *)(*(char **)D_001995F0)) == 25490 && ((int)(unsigned char)*(signed char *)(*(char **)D_001995F0 + 4)) == 70 && *(int *)(*(char **)D_001995F0 + 6) == 16747 && ((int)(unsigned char)*(signed char *)(*(char **)D_001995F0 + 10)) == 5)) {
                if (((int)(unsigned short)*(short *)(*(char **)D_001995F0)) != 25488) {
                    D_00199618 = rdb_object_id_by_offset(offset);
                    rdb_build_action_chain((int)(unsigned char)(*(signed char *)((char *)rdb_object + 20) & 63));
                }
            }
        }
        rdb_object = (int)(D_001995E8 + *(int *)((char *)rdb_object));
    } while ((rdb_object - (int)D_001995E8) > 0);
}

short rdb_object_id_by_offset(int offset)
{
    int i;

    for (i = 0; i < D_00199614; i++) {
        if (*(int *)(D_001985D4 + (i << 3)) == offset) return *(short *)(D_001985D8 + (i << 3));
    }
    return 0;
}

void rdb_build_action_chain(int resource_type)
{
    int rdb_object;
    int offset;

    switch ((unsigned)resource_type) {
    case 1:
        action_record_add(*(int *)D_0019960C, *(int *)D_00199600, 0, 0);
        offset = *(int *)(*(char **)D_00199600 + 5);
        break;
    case 2:
        action_record_add(0, 0, 0, (int)(unsigned char)*(signed char *)(*(char **)D_001995F4 + 3));
        offset = *(int *)(*(char **)D_001995F4 + 4);
        break;
    case 3:
        action_record_add(0, 0, *(int *)D_001995F0, (int)(unsigned char)*(signed char *)(*(char **)D_001995F0 + 10));
        offset = *(int *)(*(char **)D_001995F0 + 6);
    }
    while (offset > 0) {
        D_00199618 = rdb_object_id_by_offset(offset);
        rdb_object = (int)(D_001995E8 + offset);
        switch ((unsigned char)(*(signed char *)((char *)rdb_object + 20) & 63)) {
        case 1:
            if (*(int *)((char *)(*(int *)D_0019960C = (int)(D_001995E8 + *(int *)((char *)rdb_object + 21))) + 19) < 0) {
                func_000361B7(*(int *)D_0019960C);
            }
            *(int *)D_00199600 = (int)(D_001995E8 + *(int *)(*(char **)D_0019960C + 19));
            action_record_add_chained(*(int *)D_0019960C, *(int *)D_00199600, 0, 0);
            offset = *(int *)(*(char **)D_00199600 + 5);
            break;
        case 2:
            action_record_add_chained(0, 0, 0, (int)(unsigned char)*(signed char *)((char *)(*(int *)D_001995F4 = (int)(D_001995E8 + *(int *)((char *)rdb_object + 21))) + 3));
            offset = *(int *)(*(char **)D_001995F4 + 4);
            break;
        case 3:
            action_record_add_chained(0, 0, *(int *)D_001995F0, (int)(unsigned char)*(signed char *)((char *)(*(int *)D_001995F0 = (int)(D_001995E8 + *(int *)((char *)rdb_object + 21))) + 10));
            offset = *(int *)(*(char **)D_001995F0 + 6);
        }
    }
}

void action_record_add(int model, int model_action, int flat, unsigned char action)
{
    D_001995EC = (D_001995E4 = (struct link *)(((int)links) + (link_count++ * 39)));
    mc_memset((int)D_001995EC, 0, 39, (int)D_00170AB4, 447, 4);
    D_001995EC->object_id = D_00199618;
    if (model_action != 0) {
        D_001995EC->trigger = *(signed char *)((char *)model + 14);
        D_001995EC->param = *(signed char *)((char *)model + 18);
        D_001995EC->axis = *(signed char *)((char *)model_action);
        D_001995EC->duration = *(short *)((char *)model_action + 1);
        D_001995EC->magnitude = *(short *)((char *)model_action + 3);
        D_001995EC->action = *(signed char *)((char *)model_action + 9);
    } else if (flat != 0) {
        D_001995EC->trigger = *(signed char *)((char *)flat + 2);
        D_001995EC->param = *(signed char *)((char *)flat + 5);
        D_001995EC->axis = *(signed char *)((char *)flat + 4);
        D_001995EC->action = action;
    } else {
        D_001995EC->action = action;
    }
    if (D_001995EC->action > 1 && D_001995EC->action < 8) action_axis_to_translation(D_001995EC);
    D_001995EC->chain_count = 0;
}

void action_record_add_chained(int model, int model_action, int flat, unsigned char action)
{
    D_001995E4->chain_count++;
    D_001995EC = (struct link *)(((int)links) + (link_count++ * 39));
    mc_memset((int)D_001995EC, 0, 39, (int)D_00170AB4, 490, 4);
    D_001995EC->object_id = D_00199618;
    if (model_action != 0) {
        D_001995EC->trigger = *(signed char *)((char *)model + 14);
        D_001995EC->param = *(signed char *)((char *)model + 18);
        D_001995EC->axis = *(signed char *)((char *)model_action);
        D_001995EC->duration = *(short *)((char *)model_action + 1);
        D_001995EC->magnitude = *(short *)((char *)model_action + 3);
        D_001995EC->action = *(signed char *)((char *)model_action + 9);
    } else if (flat != 0) {
        D_001995EC->trigger = *(signed char *)((char *)flat + 2);
        D_001995EC->param = *(signed char *)((char *)flat + 5);
        D_001995EC->axis = *(signed char *)((char *)flat + 4);
        D_001995EC->action = action;
    } else {
        D_001995EC->action = action;
    }
    if (D_001995EC->action <= 1 || D_001995EC->action >= 8) return;
    action_axis_to_translation(D_001995EC);
}

void action_axis_to_translation(struct link *link)
{
    link->magnitude = link->axis << 3;
    link->axis = ((link->action - 2) ^ 1) + 1;
    link->duration = 50;
    link->action = 1;
}

int spellmaker_open(int opening)
{
    if (((int)D_0019626F) == 2) return 1;
    if (opening != 0) {
        game_mode = 2;
        window_image = disk_read_file((int)D_00170AEC, 0);
        magic_window_image = disk_read_file((int)D_00170AF9, 0);
        spellmaker_settings_image = disk_read_file((int)D_00170B06, 0);
        D_00196272 = 1;
        D_001940D8 |= 1;
        spellmaker_new();
    }
    return ((((int)(unsigned char)game_mode) == 2) ? 1 : 0);
}
