/* faction.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char disk_last_file_size[];
extern int D_00147954;
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
extern char D_00195C44[];
extern char D_001966BC[];
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
extern int func_0001D54B(int, int, int, int);
extern struct quest *quest_find_by_id(int);
extern int disk_read_file(int, int);
extern int disk_write_arena2_file(int, int, int);
extern int disk_open_rw(int);
extern int disk_create(int);
extern int disk_file_exists(int);
extern int rand_range(int, int);
extern int rand();
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int func_000A00CB();
extern int write();
extern int atoi();
extern int func_000A0DF4();
extern int stricmp();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern void faction_load_file(void);
extern void rumor_add_faction(struct faction *, struct faction *, int, unsigned char, int);
extern void msgbox_show_string(int, int);
extern void fatal_error(int);
extern void object_foreach(struct record *, int);
int faction_count_allies(struct faction *);
int faction_count_enemies(struct faction *);
int rumor_collect_local(void);
int rumor_copy(int, struct rumor *);
void faction_link_relations(struct faction *);
void faction_free(void);
void faction_save_r(int, struct faction *);
void func_0001CB3C(struct record *);
void func_0001DA9C(struct rumor *, int);
#pragma aux func_000A0ED9 parm routine [];

void faction_link_relations(struct faction *a1)
{
    int l_18;

    while (a1 != 0) {
        for (l_18 = 0; l_18 < 3; l_18++) {
            if (a1->allies[l_18] != 0) {
                a1->allies[l_18] = faction_find_r(factions, (int)(short)*(short *)((char *)((l_18 << 2) + (int)a1) + 56));
            }
        }
        for (l_18 = 0; l_18 < 3; l_18++) {
            if (a1->enemies[l_18] != 0) {
                a1->enemies[l_18] = faction_find_r(factions, (int)(short)*(short *)((char *)((l_18 << 2) + (int)a1) + 68));
            }
        }
        if (a1->child != 0) faction_link_relations(a1->child);
        a1 = a1->next;
    }
}

void faction_free(void)
{
    if (factions == 0 || (int)factions == (-1751672937)) return;
    mc_free((int)factions, (int)D_00170464, 1082);
    factions = (struct faction *)-1751672937;
}

void faction_add_record(struct faction *a1, int a2, struct faction *a3)
{
    int l_14;
    int l_10;

    D_00196732 = 0;
    mc_memset(((int)D_001966BC) + ((a2 << 2) + 4), 0, (int)&*(signed char *)((char *)((16 - a2) << 2) - 4), (int)D_00170464, 1091, 4);
    l_14 = rand();
    l_14 <<= 16;
    l_14 |= rand();
    a1->seed = l_14;
    a1->politics_factor = rand_range(0, 50) + 20;
    mc_memcpy(a3, a1, 92, (int)D_00170464, 1097, 4);
    if (a2 == 0) {
        a3->parent = 0;
    } else {
        l_10 = a2 - 1;
        while (*(int *)(D_001966BC + (l_10 << 2)) == 0) l_10--;
        a3->parent = (struct faction *)(*(int *)(D_001966BC + (l_10 << 2)));
        if (*(int *)(D_001966BC + (a2 << 2)) == 0) {
            *(int *)(*(char **)(D_001966BC + (l_10 << 2)) + 84) = (int)a3;
        }
    }
    if (*(int *)(D_001966BC + (a2 << 2)) != 0) {
        *(int *)(*(char **)(D_001966BC + (a2 << 2)) + 80) = (int)a3;
    }
    *(int *)(D_001966BC + (a2 << 2)) = (int)a3;
}

void faction_parse_type(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->type = atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_name(struct faction *a1, int a2, int a3, int a4)
{
    int l_10;
    int l_C;

    l_C = 0;
    l_10 = *(int *)((char *)a2);
    while (((int)(unsigned char)*(signed char *)((char *)l_10)) != 13 && ((int)(unsigned char)*(signed char *)((char *)l_10)) != 10) {
        a1->name[l_C++] = *(signed char *)((char *)l_10++);
    }
    *(int *)((char *)a2) = l_10;
}

void faction_parse_rep(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->reputation = atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_summon(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_region(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->region = atoi(l_C);
    if (a1->region != 255) a1->region--;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_power(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->power = atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_flags(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->flags |= atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_ally(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    if (a3 == 4) fatal_error((int)D_0017048A);
    l_C = *(int *)((char *)a2);
    a1->allies[(*(int *)((char *)a3))++] = (struct faction *)atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_enemy(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    if (a4 == 4) fatal_error((int)D_001704A4);
    l_C = *(int *)((char *)a2);
    a1->enemies[(*(int *)((char *)a4))++] = (struct faction *)atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_ruler(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->ruler = atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_face(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->face = atoi(l_C);
    while (((int)(unsigned char)*(signed char *)((char *)l_C)) != 13) l_C++;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_vam(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->vampire_clan = atoi(l_C);
    while (((int)(unsigned char)*(signed char *)((char *)l_C)) != 13) l_C++;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_flat(struct faction *a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    int l_C;

    l_14 = *(int *)((char *)a2);
    l_10 = atoi(l_14);
    l_C = l_10;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_14) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_14)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_14)) == 43) {
        l_14++;
    }
    while (((int)(unsigned char)*(signed char *)((char *)l_14)) <= 32) l_14++;
    a1->flats[(int)(unsigned char)D_00196732] = (l_10 << 7) | atoi(l_14);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_14) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_14)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_14)) == 43) {
        l_14++;
    }
    *(int *)((char *)a2) = l_14;
    if (l_C == (-1)) {
        a1->flats[(int)(unsigned char)D_00196732] = ((unsigned short)(unsigned char)D_0017A25C[rand_range(0, 9)]) + 23296;
    }
    if (D_00196732 == 0) a1->flats[1] = a1->flats[0];
    D_00196732 ^= 1;
}

void faction_parse_race(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->race = atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_sgroup(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->social_group = atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_ggroup(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->guild_group = atoi(l_C);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_minf(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_maxf(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_C) + 1)] & 32)) != 0 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 45 || ((int)(unsigned char)*(signed char *)((char *)l_C)) == 43) {
        l_C++;
    }
    *(int *)((char *)a2) = l_C;
}

void faction_parse_rank(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    while (((int)(unsigned char)*(signed char *)((char *)l_C)) != 13) l_C++;
    *(int *)((char *)a2) = l_C;
}

int func_0001C7D6(int a1, int a2)
{
    struct record *l_1C;
    struct character *l_18;

    l_1C = player_entity->children;
    while (l_1C != 0) {
        if (l_1C->type == (a1 + 45)) {
            if (a2 < 1) {
                l_18 = &l_1C->data.character;
                mc_memcpy(l_18->skills, player_character->skills, 200, (int)D_00170464, 1352, 210);
                l_18->health = (l_18->max_health = player_character->max_health);
                l_18->magicka = (l_18->max_magicka = player_character->max_magicka);
                l_18->level = player_character->level;
                return (int)l_1C;
            }
            a2--;
        }
        l_1C = l_1C->next;
    }
    return 0;
}

int func_0001C8DE(int a1, int a2)
{
    struct record *l_20;
    struct character *l_1C;
    int l_18;

    l_18 = 0;
    l_20 = player_entity->children;
    while (l_20 != 0) {
        if (l_20->type == (a2 + 45)) {
            l_1C = &l_20->data.character;
            if (a1 == (-1) || l_1C->career_id == a1) l_18++;
        }
        l_20 = l_20->next;
    }
    return l_18;
}

void faction_save(int a1)
{
    int l_1C;
    int l_18;

    write(a1, (int)&faction_count, 4);
    faction_save_r(a1, factions);
    faction_link_relations(factions);
}

void faction_save_r(int a1, struct faction *a2)
{
    int l_14;

    while (a2 != 0) {
        if (a2->child != 0) faction_save_r(a1, a2->child);
        for (l_14 = 0; l_14 < 3; l_14++) {
            if (a2->allies[l_14] != 0) {
                a2->allies[l_14] = (struct faction *)(int)a2->allies[l_14]->id;
            }
        }
        for (l_14 = 0; l_14 < 3; l_14++) {
            if (a2->enemies[l_14] != 0) {
                a2->enemies[l_14] = (struct faction *)(int)a2->enemies[l_14]->id;
            }
        }
        write(a1, a2, 92);
        a2 = a2->next;
    }
}

void faction_load(int a1)
{
    struct faction *l_20;
    int l_1C;
    int l_18;
    {
        struct faction l_80;

        faction_free();
        faction_load_file();
        func_000A00CB(a1, (int)&l_1C, 4);
        for (l_18 = 0; l_18 < l_1C; l_18++) {
            func_000A00CB(a1, (int)&l_80, 92);
            if (l_80.reputation > 100) l_80.reputation = 100;
            if (l_80.reputation < (-100)) l_80.reputation = 65436;
            l_20 = faction_find(l_80.id);
            mc_memcpy(l_20, (int)&l_80, 80, (int)D_00170464, 1436, 4);
        }
        faction_link_relations(factions);
    }
}

void func_0001CB3C(struct record *a1)
{
    struct faction *l_18;

    if (a1->type != 10) return;
    l_18 = faction_find(a1->data.membership.faction);
    if (l_18 == 0) return;
    if (l_18 != D_0019671C && faction_has_enemy(l_18, D_0019671C) == 0) {
        if (faction_has_ally(l_18, D_0019671C) == 0) return;
    }
    guild_membership = &a1->data.membership;
    (*(int *)D_00195B84)++;
}

int faction_player_related(struct faction *a1)
{
    if (a1 == 0) return 0;
    D_0019671C = a1;
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, (int)func_0001CB3C);
    return *(int *)D_00195B84;
}

int faction_count_allies(struct faction *a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = l_20;
    for (; l_20 < 3; l_20++) {
        if (a1->allies[l_20] != 0) l_1C++;
    }
    return l_1C;
}

int faction_count_enemies(struct faction *a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = l_20;
    for (; l_20 < 3; l_20++) {
        if (a1->enemies[l_20] != 0) l_1C++;
    }
    return l_1C;
}

int faction_make_alliance(struct faction *a1, int a2, struct faction *a3)
{
    if (faction_count_allies(a3) == 3) return 0;
    a1->allies[a2] = a3;
    a2 = 0;
    while (a3->allies[a2] != 0) a2++;
    a3->allies[a2] = a1;
    rumor_add_faction(a1, a3, 100, 0, 1400);
    return 1;
}

int faction_make_enemies(struct faction *a1, int a2, struct faction *a3)
{
    if (faction_count_enemies(a3) == 3) return 0;
    a1->enemies[a2] = a3;
    a2 = 0;
    while (a3->enemies[a2] != 0) a2++;
    a3->enemies[a2] = a1;
    rumor_add_faction(a1, a3, 100, 0, 1401);
    return 1;
}

int faction_break_alliance(struct faction *a1, int a2)
{
    struct faction *l_18;

    rumor_add_faction(a1, a1->allies[a2], 100, 0, 1402);
    l_18 = a1->allies[a2];
    a1->allies[a2] = 0;
    a2 = 0;
    while (l_18->allies[a2] != a1 && a2 < 3) a2++;
    if (a2 == 3) return 1;
    l_18->allies[a2] = 0;
    return 1;
}

int faction_make_peace(struct faction *a1, int a2)
{
    struct faction *l_18;

    rumor_add_faction(a1, a1->enemies[a2], 100, 0, 1403);
    l_18 = a1->enemies[a2];
    a1->enemies[a2] = 0;
    a2 = 0;
    while (l_18->enemies[a2] != a1 && a2 < 3) a2++;
    if (a2 == 3) return 1;
    l_18->enemies[a2] = 0;
    return 1;
}

void rumor_file_open(void)
{
    D_00196708 = 0;
    if ((rumor_file = disk_open_rw((int)D_001704BB)) < 0) {
        rumor_file = disk_create((int)D_001704BB);
    }
    if (rumor_file < 0) return;
    lseek(rumor_file, 0, 2);
}

void rumor_file_close(void)
{
    if (rumor_file <= (-1)) return;
    func_0009DEA7(rumor_file);
}

int rumor_collect_local(void)
{
    int l_24;
    struct rumor *l_20;
    int l_1C;

    l_24 = D_00147954 + 60000;
    func_000A0ED9(1642, (int)D_00170464);
    mc_sprintf(l_24, (int)D_001704C5, (int)current_location);
    l_24 += func_000A0DF4(l_24);
    *(signed char *)((char *)l_24) = 0;
    *(signed char *)((char *)l_24 + 1) = 0;
    if (disk_file_exists((int)D_001704BB) == 0) return D_00147954 + 60000;
    disk_read_file((int)D_001704BB, D_00147954);
    if (*(int *)disk_last_file_size == 0) return 0;
    l_1C = (int)(*(char **)&D_00147954 + *(int *)disk_last_file_size);
    l_20 = (struct rumor *)D_00147954;
    while (((unsigned)l_20) < l_1C) {
        if (func_0001D54B((int)l_20, 0, 1, 0) != 0) {
            mc_memcpy(l_24, (int)l_20 + 34, l_20->text_length, (int)D_00170464, 1658, 4);
            l_24 += l_20->text_length - 1;
            *(signed char *)((char *)l_24 + 1) = 252;
            *(signed char *)((char *)l_24) = *(signed char *)((char *)l_24 + 1);
            l_24 += 2;
        }
        l_20 = (struct rumor *)(((int)l_20 + l_20->text_length) + 34);
    }
    *(signed char *)((char *)l_24) = 0;
    *(signed char *)((char *)l_24 + 1) = 0;
    return D_00147954 + 60000;
}

int rumor_pick_news(short a1)
{
    short l_20;
    char l_40[28];
    short l_1C;

    *(int *)((char *)l_40 + 16) = 0;
    *(int *)&l_1C = 0;
    if (disk_file_exists((int)D_001704BB) == 0) return 0;
    disk_read_file((int)D_001704BB, *(int *)D_00195C44);
    if (*(int *)disk_last_file_size == 0) return 0;
    *(int *)((char *)l_40 + 24) = (int)(*(char **)D_00195C44 + *(int *)disk_last_file_size);
    *(int *)&l_20 = *(int *)D_00195C44;
    *(int *)l_40 = rand_range(1, 100);
    *(int *)((char *)l_40 + 4) = rand_range(1, 100);
    *(int *)((char *)l_40 + 8) = rand_range(1, 100);
    *(int *)((char *)l_40 + 12) = rand_range(1, 100);
    *(int *)((char *)l_40 + 20) = *(int *)D_00195C44 + 40000;
    while (((unsigned)*(int *)&l_20) < *(int *)((char *)l_40 + 24)) {
        if (func_0001D54B(*(int *)&l_20, (int)(short)a1, 0, *(int *)((char *)l_40 + ((*(int *)&l_1C & 3) << 2))) != 0) {
            *(int *)((char *)(int)(*(char **)((char *)l_40 + 20) + ((*(int *)((char *)l_40 + 16))++ << 2))) = *(int *)&l_20;
        }
        *(int *)&l_20 = (*(int *)&l_20 + *(int *)(*(char **)&l_20 + 26)) + 34;
        (*(int *)&l_1C)++;
    }
    if (*(int *)((char *)l_40 + 16) == 0) return 0;
    *(int *)&l_20 = *(int *)((char *)((rand_range(0, *(int *)((char *)l_40 + 16) - 1) << 2) + *(int *)((char *)l_40 + 20)));
    mc_memcpy(*(int *)D_00195C44, *(int *)&l_20 + 34, *(int *)(*(char **)&l_20 + 26), (int)D_00170464, 1701, 4);
    return *(int *)D_00195C44;
}

int func_0001D46A(int a1)
{
    struct rumor *l_20;
    int l_1C;

    if (disk_file_exists((int)D_001704BB) == 0) return 0;
    disk_read_file((int)D_001704BB, *(int *)D_00195C44);
    if (*(int *)disk_last_file_size == 0) return 0;
    l_1C = (int)(*(char **)D_00195C44 + *(int *)disk_last_file_size);
    l_20 = (struct rumor *)*(int *)D_00195C44;
    while (((unsigned)l_20) < l_1C) {
        if (((int)(unsigned char)(l_20->flags & 2)) != 0 && l_20->target == a1) {
            mc_memcpy(*(int *)D_00195C44, (int)l_20 + 34, l_20->text_length, (int)D_00170464, 1720, 4);
            return *(int *)D_00195C44;
        }
        l_20 = (struct rumor *)(((int)l_20 + l_20->text_length) + 34);
    }
    return 0;
}

int func_0001D66C(struct rumor *a1)
{
    int l_1C;

    l_1C = 0;
    if (a1->kind == 10 || a1->kind == 18 || a1->kind == 7 || a1->kind == 4 || a1->kind == 28 || a1->kind == 27 || a1->kind == 26) {
        l_1C |= 1;
    }
    if (a1->kind == 10 || a1->kind == 18 || a1->kind == 7 || a1->kind == 4 || a1->kind == 28 || a1->kind == 27 || a1->kind == 26) {
        return l_1C;
    }
    l_1C |= 8;
    return l_1C;
}

void func_0001D739(void)
{
    int l_18;

    l_18 = rumor_collect_local();
    msgbox_show_string(l_18, 1);
}

void rumor_file_purge(void)
{
    struct rumor *l_28;
    int l_24;
    int l_20;
    struct quest *l_1C;
    int l_18;

    l_18 = 0;
    if (disk_file_exists((int)D_001704BB) == 0) return;
    disk_read_file((int)D_001704BB, D_00147954);
    if (*(int *)disk_last_file_size == 0) return;
    l_28 = (struct rumor *)D_00147954;
    l_24 = *(int *)D_00195C44;
    l_20 = (int)(*(char **)&D_00147954 + *(int *)disk_last_file_size);
    while (((unsigned)l_28) < l_20) {
        if (((int)(unsigned char)(l_28->flags & 4)) != 0) {
            l_1C = quest_find_by_id((int)(short)((unsigned short)l_28->quest_id));
            if (l_1C == 0) goto L1D9F4;
            if (stricmp(l_1C->name, (int)l_28 + 11) != 0) goto L1D9F4;
            l_24 = rumor_copy(l_24, l_28);
        } else if (((unsigned)game_minutes) <= l_28->expires) {
            if (((int)(unsigned char)(l_28->flags & 8)) != 0) {
                l_24 = rumor_copy(l_24, l_28);
            } else if (((int)(unsigned char)(l_28->flags & 2)) != 0) {
                if (((int)(unsigned char)(l_28->flags & 32)) == 0) {
                    l_18++;
                    l_24 = rumor_copy(l_24, l_28);
                    if (l_18 > 200) func_0001DA9C((struct rumor *)*(int *)D_00195C44, l_24);
                }
            }
        }
L1D9F4:;
        l_28 = (struct rumor *)(((int)l_28 + l_28->text_length) + 34);
    }
    disk_write_arena2_file((int)D_001704BB, *(int *)D_00195C44, l_24 - *(int *)D_00195C44);
}

int rumor_copy(int a1, struct rumor *a2)
{
    mc_memcpy(a1, (int)a2, 34, (int)D_00170464, 1873, 4);
    mc_memcpy(a1 + 34, (int)a2 + 34, a2->text_length, (int)D_00170464, 1874, 4);
    return (a1 + 34) + a2->text_length;
}

void func_0001DA9C(struct rumor *a1, int a2)
{
    while (((unsigned)a1) < a2) {
        if (((int)(unsigned char)(a1->flags & 2)) != 0) {
            a1->flags |= 32;
            return;
        }
        a1 = (struct rumor *)(((int)a1 + a1->text_length) + 34);
    }
}
