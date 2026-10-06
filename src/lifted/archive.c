/* archive.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"


extern char D_00170150[];
extern char D_0017015A[];
extern char D_00170172[];
extern iptr lock_text_fail;
extern int lock_text_open;
extern struct record *player_object;
extern struct character *player_character;
extern char *scratch_buffer;
extern iptr archive_directories[];
extern char archive_types[];
extern char archive_record_counts[];
extern char archive_names[];

extern int sound_play(int, struct record *, int);
extern int disk_open_data(char *);
extern int disk_open_rw(char *);
extern iptr hud_message_add(iptr);
extern int rand_range(int, int);
extern void skill_add_uses(int, int);
extern void fatal_error(iptr);
extern void links_trigger(struct record *, int);
extern void guild_count_crime(int, int);
int lockpick_door(struct record *);
#pragma aux mc_set_location parm routine [];

int archive_open(char *name, iptr directory, int writable)
{
    slot16 record_count;
    slot16 handle;
    slot16 dir_size;
    slot16 type;

    if (writable != 0) {
        do {
            *(int *)&handle = disk_open_rw(name);
        } while (*(int *)&handle < 1);
    } else {
        *(int *)&handle = disk_open_data(name);
    }
    if (*(int *)&handle < 1) return *(int *)&handle;
    mc_strncpy((char *)(((iptr)archive_names) + (*(int *)&handle * 13)), name, 13, D_00170150, 32);
    read(*(int *)&handle, &record_count, 2);
    read(*(int *)&handle, &type, 2);
    if (((int)(short)type) == 256) {
        *(int *)&dir_size = ((int)(short)record_count) * 18;
    } else {
        *(int *)&dir_size = ((int)(short)record_count) << 3;
    }
    if (directory == 0) directory = (iptr)mc_malloc(*(int *)&dir_size, D_00170150, 42);
    *(short *)(archive_record_counts + (*(int *)&handle * 2)) = *(int *)&record_count;
    archive_directories[*(int *)&handle] = directory;
    *(short *)(archive_types + (*(int *)&handle * 2)) = *(int *)&type;
    lseek(*(int *)&handle, -*(int *)&dir_size, 2);
    read(*(int *)&handle, (void *)directory, *(int *)&dir_size);
    return *(int *)&handle;
}

void archive_close(int handle)
{
    if (handle == 0) return;
    *(short *)(archive_record_counts + (handle * 2)) = 0;
    if (archive_directories[handle] != 0 && archive_directories[handle] != (-1751672937)) {
        mc_free((void *)archive_directories[handle], D_00170150, 67);
        archive_directories[handle] = -1751672937;
    }
    archive_directories[handle] = 0;
    *(short *)(archive_types + (handle * 2)) = 0;
    close(handle);
}

int archive_find_record(int handle, char *name, int key)
{
    struct bsa_name_entry *named_entry;
    struct bsa_id_entry *entry;
    int record;

    if (((int)(short)*(short *)(archive_types + (handle * 2))) == 256) {
        named_entry = (struct bsa_name_entry *)archive_directories[handle];
        for (record = 0; ((int)(short)*(short *)(archive_record_counts + (handle * 2))) > record; record++, named_entry++) {
            if (strnicmp(name, named_entry->name, key) == 0) return record;
        }
    } else {
        entry = (struct bsa_id_entry *)archive_directories[handle];
        for (record = 0; ((int)(short)*(short *)(archive_record_counts + (handle * 2))) > record; record++, entry++) {
            if (key == entry->id) return record;
        }
    }
    if (((int)(short)*(short *)(archive_types + (handle * 2))) == 256) {
        mc_set_location(105, D_00170150);
        mc_sprintf(scratch_buffer, D_0017015A, name, ((iptr)archive_names) + (handle * 13));
    } else {
        mc_set_location(107, D_00170150);
        mc_sprintf(scratch_buffer, D_00170172, key, ((iptr)archive_names) + (handle * 13));
    }
    fatal_error((iptr)scratch_buffer);
    return 0;
}

int archive_record_size(int handle, int record)
{
    struct bsa_name_entry *named_entry;
    struct bsa_id_entry *entry;

    if (((int)(short)*(short *)(archive_types + (handle * 2))) == 256) {
        named_entry = (struct bsa_name_entry *)archive_directories[handle];
        named_entry += record;
        return named_entry->size;
    }
    entry = (struct bsa_id_entry *)archive_directories[handle];
    entry += record;
    return entry->size;
}

int archive_record_offset(int handle, int record)
{
    struct bsa_name_entry *named_entry;
    struct bsa_id_entry *entry;
    int i;
    int offset;

    offset = 4;
    if (((int)(short)*(short *)(archive_types + (handle * 2))) == 256) {
        named_entry = (struct bsa_name_entry *)archive_directories[handle];
        for (i = 0; i < record; i++, named_entry++) {
            offset += named_entry->size;
        }
        return offset;
    }
    entry = (struct bsa_id_entry *)archive_directories[handle];
    for (i = 0; i < record; i++, entry++) {
        offset += entry->size;
    }
    return offset;
}

iptr archive_read_record(int handle, int record, iptr buffer)
{
    struct bsa_name_entry *named_entry;
    struct bsa_id_entry *entry;
    int i;
    int size;
    int offset;

    offset = 4;
    if (((int)(short)*(short *)(archive_types + (handle * 2))) == 256) {
        named_entry = (struct bsa_name_entry *)archive_directories[handle];
        for (i = 0; i < record; i++, named_entry++) {
            offset += named_entry->size;
        }
        size = named_entry->size;
    } else {
        entry = (struct bsa_id_entry *)archive_directories[handle];
        for (i = 0; i < record; i++, entry++) {
            offset += entry->size;
        }
        size = entry->size;
    }
    if (buffer == 0) buffer = (iptr)mc_malloc(size, D_00170150, 205);
    lseek(handle, offset, 0);
    read(handle, (void *)buffer, size);
    return buffer;
}

void archive_write_record(int handle, int record, iptr data)
{
    struct bsa_name_entry *named_entry;
    struct bsa_id_entry *entry;
    int i;
    int size;
    int offset;

    offset = 4;
    if (((int)(short)*(short *)(archive_types + (handle * 2))) == 256) {
        named_entry = (struct bsa_name_entry *)archive_directories[handle];
        for (i = 0; i < record; i++, named_entry++) {
            offset += named_entry->size;
        }
        size = named_entry->size;
    } else {
        entry = (struct bsa_id_entry *)archive_directories[handle];
        for (i = 0; i < record; i++, entry++) {
            offset += entry->size;
        }
        size = entry->size;
    }
    lseek(handle, offset, 0);
    write(handle, (void *)data, size);
}

void lockpick_door_unused(struct record *door)
{
    int result;

    result = lockpick_door(door);
}

int lockpick_door(struct record *door)
{
    int chance;

    if (door->lockpick_skill_tried == player_character->skills[SKILL_LOCKPICKING].value) return 0;
    if (door->lock_level >= 20) {
        hud_message_add(lock_text_fail);
        links_trigger(door, 4);
        return 0;
    }
    skill_add_uses(13, 1);
    if ((player_character->conditions & 0x40) != 0) {
        chance = (int)(unsigned char)(signed char)player_character->lock_open_chance;
        player_character->conditions &= ~0x40;
    } else {
        chance = (int)(short)player_character->skills[SKILL_LOCKPICKING].value;
    }
    chance += (((int)(unsigned char)(signed char)player_character->level) - door->lock_level) * 5;
    if (chance < 5) {
        chance = 5;
    } else if (chance > 95) {
        chance = 95;
    }
    if (rand_range(0, 100) <= chance) {
        door->flags |= 64;
        hud_message_add(lock_text_open);
        links_trigger(door, 7);
        sound_play(60, door, 100);
        return 1;
    }
    if ((player_character->conditions & 0x40) == 0) {
        door->lockpick_skill_tried = player_character->skills[SKILL_LOCKPICKING].value;
    }
    hud_message_add(lock_text_fail);
    links_trigger(door, 4);
    return 0;
}

int lockpick_action_door(struct building *building, int lock_level, struct record *door)
{
    int chance;

    if (building->access_level >= 10) return 1;
    if (door->lockpick_skill_tried == player_character->skills[SKILL_LOCKPICKING].value) return 0;
    if (lock_level >= 20) {
        hud_message_add(lock_text_fail);
        return 0;
    }
    skill_add_uses(13, 1);
    if ((player_character->conditions & 0x40) != 0) {
        chance = player_character->lock_open_chance;
        player_character->conditions &= ~0x40;
    } else {
        chance = player_character->skills[SKILL_LOCKPICKING].value;
    }
    chance -= lock_level * 5;
    if (chance < 5) {
        chance = 5;
    } else if (chance > 95) {
        chance = 95;
    }
    if (rand_range(0, 100) <= chance) {
        hud_message_add(lock_text_open);
        sound_play(60, player_object, 110);
        guild_count_crime(5, 1);
        return 1;
    }
    if ((player_character->conditions & 0x40) == 0) {
        door->lockpick_skill_tried = player_character->skills[SKILL_LOCKPICKING].value;
    }
    hud_message_add(lock_text_fail);
    return 0;
}
