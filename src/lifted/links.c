/* links.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern iptr D_00147954;
extern char D_00175962[];
extern signed char text_rsc_buffer[];
extern signed char D_00190FE5[];
extern struct record *player_entity;
extern struct record *location_object;
extern struct spell *spell_records;
extern struct character *player_character;
extern char *scratch_buffer;
extern struct link links[];
extern iptr active_links[];
extern int link_count;
extern int active_link_count;

extern int func_000658CA(int, int);
extern iptr hud_message_add(char *);
extern int spfx_damage(struct record *, int, struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_find_by_id(struct record *, iptr);
extern int xn_str_count_nonzero(char *, unsigned);
extern void parse_rsc_text(int, int, int);
extern void parse_rsc_text_copy(int, iptr);

void links_save(int handle)
{
    int i;
    struct link *copy;
    int *indexes;

    if (active_link_count < 0) active_link_count = 0;
    mc_memcpy((void *)D_00147954, links, link_count * 39, D_00175962, 78, 4);
    copy = (struct link *)D_00147954;
    for (i = 0; i < link_count; i++) {
        if (copy[i].object != 0) {
            copy[i].object = (struct record *)(uptr)copy[i].object->id;
        }
    }
    write(handle, &link_count, 4);
    write(handle, copy, link_count * 39);
    write(handle, &active_link_count, 4);
    indexes = (int *)D_00147954;
    for (i = 0; i < active_link_count; i++) {
        indexes[i] = ((unsigned)(active_links[i] - ((iptr)links))) / 39;
    }
    write(handle, indexes, active_link_count << 2);
}

void links_load(int handle)
{
    int i;

    read(handle, &link_count, 4);
    read(handle, links, link_count * 39);
    for (i = 0; i < link_count; i++) {
        if (links[i].param == 108) {
            links[i].param = 100;
        }
        if (links[i].object != 0) {
            links[i].object = object_find_by_id(location_object, (iptr)links[i].object);
        }
    }
    read(handle, &active_link_count, 4);
    if (active_link_count < 0) active_link_count = 0;
    read(handle, active_links, active_link_count << 2);
    for (i = 0; i < active_link_count; i++) {
        active_links[i] = ((iptr)links) + (active_links[i] * 39);
    }
}

void link_show_text(int text_id)
{
    char *line;
    char *end;

    parse_rsc_text_copy(text_id, (iptr)scratch_buffer);
    line = scratch_buffer;
    while (*line != 0) {
        end = line;
        while (*end != 0 && *end != 252 && *end != 253) {
            end++;
        }
        *end = 0;
        hud_message_add(line);
        line = end + 1;
    }
}

int link_answer_matches(int text_id, iptr answer)
{
    int i;
    int length;

    parse_rsc_text(text_id, 0, 0);
    length = strlen((char *)text_rsc_buffer);
    for (i = 1; i < length; i++) {
        if (((int)(unsigned char)text_rsc_buffer[i]) == 44 || ((int)(unsigned char)text_rsc_buffer[i]) == 34) {
            text_rsc_buffer[i] = 0;
        }
    }
    D_00190FE5[i] = 0;
    i = 1;
    while (text_rsc_buffer[i] != 0) {
        if (stricmp((char *)(((iptr)text_rsc_buffer) + i), (char *)answer) == 0) return 1;
        i += strlen((char *)(((iptr)text_rsc_buffer) + i)) + 1;
    }
    return 0;
}

void link_hurt_player(int element, int magnitude)
{
    struct record *spell_object;
    struct spell *spell;

    spell_object = object_create_child(location_object, 0, 89);
    spell = &spell_object->data.spell;
    spell->cast_magnitudes[0] = magnitude * ((unsigned short)player_character->level);
    spell->effects[0].subtype = 0;
    spell->element = *(signed char *)&element;
    spfx_damage(spell_object, 0, player_entity);
    object_delete(spell_object);
}

void link_start(struct link *link)
{
    int axis;
    int *ticks_addr;
    int *ticks_addr2;

    link->flags &= 250;
    switch (link->action) {
    case 1:
        if (link->duration != 0) {
            link->speed = (link->magnitude << 16) / link->duration;
        } else {
            link->speed = link->magnitude << 16;
        }
        if (((int)(unsigned char)(link->flags & 2)) != 0) link->speed = -link->speed;
        ticks_addr = (int *)1132;
        link->start_tick = *ticks_addr;
        switch (link->axis - 1) {
        case 0:
        case 1:
            link->start = link->object->x;
            break;
        case 2:
        case 3:
            link->start = link->object->y;
            break;
        case 4:
        case 5:
            link->start = link->object->z;
        }
        return;
    case 8:
        if (link->duration != 0) {
            link->speed = (link->magnitude << 16) / link->duration;
        } else {
            link->speed = link->magnitude << 16;
        }
        if (((int)(unsigned char)(link->flags & 2)) != 0) link->speed = -link->speed;
        ticks_addr2 = (int *)1132;
        link->start_tick = *ticks_addr2;
        if (((int)(unsigned char)(link->flags & 32)) == 0 && (axis = func_000658CA(link->object->image2, link->object->image)) != 0) {
            link->flags |= 32;
            if (((int)(unsigned char)(link->axis & 1)) != 0) axis++;
            link->axis = *(signed char *)&axis;
        }
        switch (link->axis - 1) {
        case 0:
        case 1:
            link->start = link->object->angle_x;
            break;
        case 2:
        case 3:
            link->start = link->object->yaw;
            break;
        case 4:
        case 5:
            link->start = link->object->angle_z;
        }
        return;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        if (((int)(unsigned char)(link->flags & 2)) == 0) return;
        link->flags |= 1;
    default:;
    }
}

void links_set_reverse(short object_id, int flags)
{
    int i;

    for (i = 0; i < link_count; i++) {
        if (links[i].object_id == (short)object_id) {
            links[i].flags &= 253;
            links[i].flags |= *(signed char *)&flags & 2;
        }
    }
}

iptr links_object_motion(iptr object)
{
    int i;
    int n;
    int j;
    struct link *link;

    for (i = 0; i < active_link_count; i++) {
        link = (struct link *)active_links[i];
        n = link->chain_count + 1;
        for (j = 0; j < n; j++, link++) {
            if ((iptr)link->object == object && xn_str_count_nonzero(link->delta, 6) != 0) {
                return (iptr)link->delta;
            }
        }
    }
    return 0;
}

struct spell *link_find_spell(int id)
{
    int i;

    i = 0;
    while (spell_records[i].name[0] == 0 || spell_records[i].id != id) i++;
    return &spell_records[i];
}
