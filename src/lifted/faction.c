/* faction.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"
#include "portio.h"

extern char disk_last_file_size[];
extern iptr D_00147954;
extern char D_00170464[];
extern char D_0017048A[];
extern char D_001704A4[];
extern char D_001704BB[];
extern char D_001704C5[];
extern signed char D_00178630[];
extern signed char D_0017A25C[];
extern struct record *player_entity;
extern char D_00195B84[];
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern char *scratch_buffer;
extern iptr D_001966BC[16];
extern int rumor_file;
extern int D_00196708;
extern int faction_count;
extern struct faction *D_0019671C;
extern struct faction *factions;
extern signed char D_00196732;
extern struct membership *guild_membership;

extern struct faction *faction_find(short);
extern struct faction *faction_find_r(struct faction *, short);
extern int faction_has_enemy(struct faction *, struct faction *);
extern int faction_has_ally(struct faction *, struct faction *);
extern int rumor_is_eligible(iptr, int, int, int);
extern struct quest *quest_find_by_id(int);
extern iptr disk_read_file(char *, iptr);
extern int disk_write_arena2_file(char *, iptr, int);
extern int disk_open_rw(char *);
extern int disk_create(char *);
extern int disk_file_exists(char *);
extern int rand_range(int, int);
extern void faction_load_file(void);
extern void rumor_add_faction(struct faction *, struct faction *, int, unsigned char, int);
extern void msgbox_show_string(iptr, int);
extern void fatal_error(char *);
extern void object_foreach(struct record *, void (*)());
int faction_count_allies(struct faction *);
int faction_count_enemies(struct faction *);
iptr rumor_collect_local(void);
iptr rumor_copy(iptr, struct rumor *);
void faction_link_relations(struct faction *);
void faction_free(void);
void faction_save_r(int, struct faction *);
void func_0001CB3C(struct record *);
void func_0001DA9C(struct rumor *, iptr);
#pragma aux mc_set_location parm routine [];

void faction_link_relations(struct faction *faction)
{
    int i;

    while (faction != 0) {
        for (i = 0; i < 3; i++) {
            if (faction->allies[i] != 0) {
                faction->allies[i] = faction_find_r(factions, (int)(short)*(short *)&faction->allies[i]);
            }
        }
        for (i = 0; i < 3; i++) {
            if (faction->enemies[i] != 0) {
                faction->enemies[i] = faction_find_r(factions, (int)(short)*(short *)&faction->enemies[i]);
            }
        }
        if (faction->child != 0) faction_link_relations(faction->child);
        faction = faction->next;
    }
}

void faction_free(void)
{
    if (factions == 0 || (iptr)factions == (-1751672937)) return;
    mc_free(factions, D_00170464, 1082);
    factions = (struct faction *)(iptr)-1751672937;
#ifdef DAGGER_PORT
    /* faction_add_record's last record at each depth still points into the array just
       freed, and the next load links its first record through it: a write into freed memory,
       which DOS's heap took and the host's does not (its checks trap) */
    mc_memset(D_001966BC, 0, 16 * PTR_SIZE, D_00170464, 1082, 4);
#endif
}

void faction_add_record(struct faction *parsed, int depth, struct faction *added)
{
    int seed;
    int parent_depth;

    D_00196732 = 0;
    mc_memset((void *)(((iptr)(char *)D_001966BC) + ((depth << PTR_SHIFT) + PTR_SIZE)), 0, (int)(iptr)&*(signed char *)((char *)(iptr)((16 - depth) << PTR_SHIFT) - PTR_SIZE), D_00170464, 1091, 4);
    seed = rand();
    seed <<= 16;
    seed |= rand();
    parsed->seed = seed;
    parsed->politics_factor = rand_range(0, 50) + 20;
    mc_memcpy(added, parsed, REC_SIZEOF(struct faction), D_00170464, 1097, 4);
    if (depth == 0) {
        added->parent = 0;
    } else {
        parent_depth = depth - 1;
        while (D_001966BC[parent_depth] == 0) parent_depth--;
        added->parent = (struct faction *)(D_001966BC[parent_depth]);
        if (D_001966BC[depth] == 0) {
            *(iptr *)((char *)D_001966BC[parent_depth] + REC_OFFSETOF(struct faction, child)) = (iptr)added;
        }
    }
    if (D_001966BC[depth] != 0) {
        *(iptr *)((char *)D_001966BC[depth] + REC_OFFSETOF(struct faction, next)) = (iptr)added;
    }
    D_001966BC[depth] = (iptr)added;
}

void faction_parse_type(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->type = atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_name(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;
    int i;

    i = 0;
    text = *cursor;
    while (((int)(unsigned char)*text) != 13 && ((int)(unsigned char)*text) != 10) {
        faction->name[i++] = *text++;
    }
    *cursor = text;
}

void faction_parse_rep(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->reputation = atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_summon(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_region(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->region = atoi(text);
    if (faction->region != 255) faction->region--;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_power(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->power = atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_flags(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->flags |= atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_ally(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    if ((iptr)ally_count == 4) fatal_error(D_0017048A);
    text = *cursor;
    faction->allies[(*ally_count)++] = (struct faction *)(iptr)atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_enemy(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    if ((iptr)enemy_count == 4) fatal_error(D_001704A4);
    text = *cursor;
    faction->enemies[(*enemy_count)++] = (struct faction *)(iptr)atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_ruler(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->ruler = atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_face(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->face = atoi(text);
    while (((int)(unsigned char)*text) != 13) text++;
    *cursor = text;
}

void faction_parse_vam(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->vampire_clan = atoi(text);
    while (((int)(unsigned char)*text) != 13) text++;
    *cursor = text;
}

void faction_parse_flat(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;
    int archive;
    int archive_value;

    text = *cursor;
    archive = atoi(text);
    archive_value = archive;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    while (((int)(unsigned char)*text) <= 32) text++;
    faction->flats[(int)(unsigned char)D_00196732] = (archive << 7) | atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
    if (archive_value == (-1)) {
        faction->flats[(int)(unsigned char)D_00196732] = ((unsigned short)(unsigned char)D_0017A25C[rand_range(0, 9)]) + 23296;
    }
    if (D_00196732 == 0) faction->flats[1] = faction->flats[0];
    D_00196732 ^= 1;
}

void faction_parse_race(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->race = atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_sgroup(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->social_group = atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_ggroup(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    faction->guild_group = atoi(text);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_minf(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_maxf(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*text + 1)] & 32)) != 0 || ((int)(unsigned char)*text) == 45 || ((int)(unsigned char)*text) == 43) {
        text++;
    }
    *cursor = text;
}

void faction_parse_rank(struct faction *faction, signed char **cursor, int *ally_count, int *enemy_count)
{
    signed char *text;

    text = *cursor;
    while (((int)(unsigned char)*text) != 13) text++;
    *cursor = text;
}

iptr func_0001C7D6(int kind, int index)
{
    struct record *object;
    struct character *character;

    object = player_entity->children;
    while (object != 0) {
        if (object->type == (kind + 45)) {
            if (index < 1) {
                character = &object->data.character;
                mc_memcpy(character->skills, player_character->skills, 200, D_00170464, 1352, 210);
                character->health = (character->max_health = player_character->max_health);
                character->magicka = (character->max_magicka = player_character->max_magicka);
                character->level = player_character->level;
                return (iptr)object;
            }
            index--;
        }
        object = object->next;
    }
    return 0;
}

int func_0001C8DE(int career_id, int kind)
{
    struct record *object;
    struct character *character;
    int count;

    count = 0;
    object = player_entity->children;
    while (object != 0) {
        if (object->type == (kind + 45)) {
            character = &object->data.character;
            if (career_id == (-1) || character->career_id == career_id) count++;
        }
        object = object->next;
    }
    return count;
}

void faction_save(int file)
{
    int unused1;
    int unused2;

    write(file, &faction_count, 4);
    faction_save_r(file, factions);
    faction_link_relations(factions);
}

void faction_save_r(int file, struct faction *faction)
{
    int i;

    while (faction != 0) {
        if (faction->child != 0) faction_save_r(file, faction->child);
        for (i = 0; i < 3; i++) {
            if (faction->allies[i] != 0) {
                faction->allies[i] = (struct faction *)(iptr)(int)faction->allies[i]->id;
            }
        }
        for (i = 0; i < 3; i++) {
            if (faction->enemies[i] != 0) {
                faction->enemies[i] = (struct faction *)(iptr)(int)faction->enemies[i]->id;
            }
        }
        PORT_WRITE_FACTION(file, faction);
        faction = faction->next;
    }
}

void faction_load(int file)
{
    struct faction *faction;
    int count;
    int i;
    {
        struct faction loaded;

        faction_free();
        faction_load_file();
        read(file, &count, 4);
        for (i = 0; i < count; i++) {
            PORT_READ_FACTION(file, &loaded);
            if (loaded.reputation > 100) loaded.reputation = 100;
            if (loaded.reputation < (-100)) loaded.reputation = 65436;
            faction = faction_find(loaded.id);
            mc_memcpy(faction, &loaded, REC_OFFSETOF(struct faction, next), D_00170464, 1436, 4);
        }
        faction_link_relations(factions);
    }
}

void func_0001CB3C(struct record *object)
{
    struct faction *faction;

    if (object->type != 10) return;
    faction = faction_find(object->data.membership.faction);
    if (faction == 0) return;
    if (faction != D_0019671C && faction_has_enemy(faction, D_0019671C) == 0) {
        if (faction_has_ally(faction, D_0019671C) == 0) return;
    }
    guild_membership = &object->data.membership;
    (*(int *)D_00195B84)++;
}

int faction_player_related(struct faction *faction)
{
    if (faction == 0) return 0;
    D_0019671C = faction;
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, func_0001CB3C);
    return *(int *)D_00195B84;
}

int faction_count_allies(struct faction *faction)
{
    int i;
    int count;

    i = 0;
    count = i;
    for (; i < 3; i++) {
        if (faction->allies[i] != 0) count++;
    }
    return count;
}

int faction_count_enemies(struct faction *faction)
{
    int i;
    int count;

    i = 0;
    count = i;
    for (; i < 3; i++) {
        if (faction->enemies[i] != 0) count++;
    }
    return count;
}

int faction_make_alliance(struct faction *faction, int slot, struct faction *other)
{
    if (faction_count_allies(other) == 3) return 0;
    faction->allies[slot] = other;
    slot = 0;
    while (other->allies[slot] != 0) slot++;
    other->allies[slot] = faction;
    rumor_add_faction(faction, other, 100, 0, 1400);
    return 1;
}

int faction_make_enemies(struct faction *faction, int slot, struct faction *other)
{
    if (faction_count_enemies(other) == 3) return 0;
    faction->enemies[slot] = other;
    slot = 0;
    while (other->enemies[slot] != 0) slot++;
    other->enemies[slot] = faction;
    rumor_add_faction(faction, other, 100, 0, 1401);
    return 1;
}

int faction_break_alliance(struct faction *faction, int slot)
{
    struct faction *other;

    rumor_add_faction(faction, faction->allies[slot], 100, 0, 1402);
    other = faction->allies[slot];
    faction->allies[slot] = 0;
    slot = 0;
    while (other->allies[slot] != faction && slot < 3) slot++;
    if (slot == 3) return 1;
    other->allies[slot] = 0;
    return 1;
}

int faction_make_peace(struct faction *faction, int slot)
{
    struct faction *other;

    rumor_add_faction(faction, faction->enemies[slot], 100, 0, 1403);
    other = faction->enemies[slot];
    faction->enemies[slot] = 0;
    slot = 0;
    while (other->enemies[slot] != faction && slot < 3) slot++;
    if (slot == 3) return 1;
    other->enemies[slot] = 0;
    return 1;
}

void rumor_file_open(void)
{
    D_00196708 = 0;
    if ((rumor_file = disk_open_rw(D_001704BB)) < 0) {
        rumor_file = disk_create(D_001704BB);
    }
    if (rumor_file < 0) return;
    lseek(rumor_file, 0, 2);
}

void rumor_file_close(void)
{
    if (rumor_file <= (-1)) return;
    close(rumor_file);
}

iptr rumor_collect_local(void)
{
    char *out;
    struct rumor *rumor;
    iptr end;

    out = (char *)(D_00147954 + 60000);
    mc_set_location(1642, D_00170464);
    mc_sprintf(out, D_001704C5, (iptr)current_location);
    out += strlen(out);
    *out = 0;
    out[1] = 0;
    if (disk_file_exists(D_001704BB) == 0) return D_00147954 + 60000;
    disk_read_file(D_001704BB, D_00147954);
    if (*(int *)disk_last_file_size == 0) return 0;
    end = (iptr)(*(char **)&D_00147954 + *(int *)disk_last_file_size);
    rumor = (struct rumor *)D_00147954;
    while (((uptr)rumor) < end) {
        if (rumor_is_eligible((iptr)rumor, 0, 1, 0) != 0) {
            mc_memcpy(out, (void *)((iptr)rumor + 34), rumor->text_length, D_00170464, 1658, 4);
            out += rumor->text_length - 1;
            out[1] = 252;
            *out = out[1];
            out += 2;
        }
        rumor = (struct rumor *)(((iptr)rumor + rumor->text_length) + 34);
    }
    *out = 0;
    out[1] = 0;
    return D_00147954 + 60000;
}

iptr rumor_pick_news(short faction_id)
{
    pslot16 rumor;
    struct {
        int rolls[4];               /* +0x00: rumor_is_eligible's roll, by index & 3 */
        int count;                  /* +0x10 */
        iptr *found;                /* +0x14: the eligible rumors */
        iptr end;                    /* +0x18 */
    } state;
    pslot16 index;

    state.count = 0;
    *(int *)&index = 0;
    if (disk_file_exists(D_001704BB) == 0) return 0;
    disk_read_file(D_001704BB, (iptr)scratch_buffer);
    if (*(int *)disk_last_file_size == 0) return 0;
    state.end = (iptr)(scratch_buffer + *(int *)disk_last_file_size);
    *(iptr *)&rumor = (iptr)scratch_buffer;
    state.rolls[0] = rand_range(1, 100);
    state.rolls[1] = rand_range(1, 100);
    state.rolls[2] = rand_range(1, 100);
    state.rolls[3] = rand_range(1, 100);
    state.found = (iptr *)((iptr)scratch_buffer + 40000);
    while (((uptr)*(iptr *)&rumor) < state.end) {
        if (rumor_is_eligible(*(iptr *)&rumor, (int)(short)faction_id, 0, state.rolls[*(int *)&index & 3]) != 0) {
            state.found[state.count++] = *(iptr *)&rumor;
        }
        *(iptr *)&rumor = (*(iptr *)&rumor + (*(struct rumor **)&rumor)->text_length) + 34;
        (*(int *)&index)++;
    }
    if (state.count == 0) return 0;
    *(iptr *)&rumor = state.found[rand_range(0, state.count - 1)];
    mc_memcpy((void *)scratch_buffer, (void *)(*(iptr *)&rumor + 34), (*(struct rumor **)&rumor)->text_length, D_00170464, 1701, 4);
    return (iptr)scratch_buffer;
}

iptr func_0001D46A(int target)
{
    struct rumor *rumor;
    iptr end;

    if (disk_file_exists(D_001704BB) == 0) return 0;
    disk_read_file(D_001704BB, (iptr)scratch_buffer);
    if (*(int *)disk_last_file_size == 0) return 0;
    end = (iptr)(scratch_buffer + *(int *)disk_last_file_size);
    rumor = (struct rumor *)scratch_buffer;
    while (((uptr)rumor) < end) {
        if (((int)(unsigned char)(rumor->flags & 2)) != 0 && rumor->target == target) {
            mc_memcpy((void *)scratch_buffer, (void *)((iptr)rumor + 34), rumor->text_length, D_00170464, 1720, 4);
            return (iptr)scratch_buffer;
        }
        rumor = (struct rumor *)(((iptr)rumor + rumor->text_length) + 34);
    }
    return 0;
}

int func_0001D66C(struct rumor *rumor)
{
    int flags;

    flags = 0;
    if (rumor->kind == 10 || rumor->kind == 18 || rumor->kind == 7 || rumor->kind == 4 || rumor->kind == 28 || rumor->kind == 27 || rumor->kind == 26) {
        flags |= 1;
    }
    if (rumor->kind == 10 || rumor->kind == 18 || rumor->kind == 7 || rumor->kind == 4 || rumor->kind == 28 || rumor->kind == 27 || rumor->kind == 26) {
        return flags;
    }
    flags |= 8;
    return flags;
}

void rumor_show_local(void)
{
    iptr text;

    text = rumor_collect_local();
    msgbox_show_string(text, 1);
}

void rumor_file_purge(void)
{
    struct rumor *rumor;
    iptr out;
    iptr end;
    struct quest *quest;
    int npc_count;

    npc_count = 0;
    if (disk_file_exists(D_001704BB) == 0) return;
    disk_read_file(D_001704BB, D_00147954);
    if (*(int *)disk_last_file_size == 0) return;
    rumor = (struct rumor *)D_00147954;
    out = (iptr)scratch_buffer;
    end = (iptr)(*(char **)&D_00147954 + *(int *)disk_last_file_size);
    while (((uptr)rumor) < end) {
        if (((int)(unsigned char)(rumor->flags & 4)) != 0) {
            quest = quest_find_by_id((int)(short)((unsigned short)rumor->quest_id));
            if (quest == 0) goto L1D9F4;
            if (stricmp(quest->name, (char *)((iptr)rumor + 11)) != 0) goto L1D9F4;
            out = rumor_copy(out, rumor);
        } else if (((unsigned)game_minutes) <= rumor->expires) {
            if (((int)(unsigned char)(rumor->flags & 8)) != 0) {
                out = rumor_copy(out, rumor);
            } else if (((int)(unsigned char)(rumor->flags & 2)) != 0) {
                if (((int)(unsigned char)(rumor->flags & 32)) == 0) {
                    npc_count++;
                    out = rumor_copy(out, rumor);
                    if (npc_count > 200) func_0001DA9C((struct rumor *)scratch_buffer, out);
                }
            }
        }
L1D9F4:;
        rumor = (struct rumor *)(((iptr)rumor + rumor->text_length) + 34);
    }
    disk_write_arena2_file(D_001704BB, (iptr)scratch_buffer, (int)(out - (iptr)scratch_buffer));
}

iptr rumor_copy(iptr out, struct rumor *rumor)
{
    mc_memcpy((void *)out, rumor, 34, D_00170464, 1873, 4);
    mc_memcpy((void *)(out + 34), (void *)((iptr)rumor + 34), rumor->text_length, D_00170464, 1874, 4);
    return (out + 34) + rumor->text_length;
}

void func_0001DA9C(struct rumor *rumor, iptr end)
{
    while (((uptr)rumor) < end) {
        if (((int)(unsigned char)(rumor->flags & 2)) != 0) {
            rumor->flags |= 32;
            return;
        }
        rumor = (struct rumor *)(((iptr)rumor + rumor->text_length) + 34);
    }
}
