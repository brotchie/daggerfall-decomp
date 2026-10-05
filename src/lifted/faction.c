/* faction.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char disk_last_file_size[];
extern char D_00147954[];
extern char D_00170464[];
extern char D_0017048A[];
extern char D_001704A4[];
extern char D_001704BB[];
extern char D_001704C5[];
extern char D_00178630[];
extern char D_0017A25C[];
extern struct record *player_entity;
extern char D_00195B84[];
extern struct location *current_location;
extern struct character *player_character;
extern char game_minutes[];
extern char D_00195C44[];
extern char D_001966BC[];
extern char rumor_file[];
extern char D_00196708[];
extern char faction_count[];
extern struct faction *D_0019671C;
extern struct faction *factions;
extern char D_00196732[];
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

L1BAC5:;
    if (a1 == 0) return;
    l_18 = 0;
L1BAD6:;
    if (l_18 < 3) goto L1BAE6;
    goto L1BB1C;
L1BADE:;
    l_18++;
    goto L1BAD6;
L1BAE6:;
    if (a1->allies[l_18] == 0) goto L1BB1A;
    a1->allies[l_18] = faction_find_r(factions, (int)(short)*(short *)((char *)((l_18 << 2) + (int)a1) + 56));
L1BB1A:;
    goto L1BADE;
L1BB1C:;
    l_18 = 0;
L1BB23:;
    if (l_18 < 3) goto L1BB33;
    goto L1BB69;
L1BB2B:;
    l_18++;
    goto L1BB23;
L1BB33:;
    if (a1->enemies[l_18] == 0) goto L1BB67;
    a1->enemies[l_18] = faction_find_r(factions, (int)(short)*(short *)((char *)((l_18 << 2) + (int)a1) + 68));
L1BB67:;
    goto L1BB2B;
L1BB69:;
    if (a1->child == 0) goto L1BB7D;
    faction_link_relations(a1->child);
L1BB7D:;
    a1 = a1->next;
    goto L1BAC5;
}

void faction_free(void)
{
    if (factions == 0) goto L1BBB8;
    if ((int)factions != (-1751672937)) goto L1BBBA;
L1BBB8:;
    return;
L1BBBA:;
    mc_free((int)factions, (int)D_00170464, 1082);
    factions = (struct faction *)-1751672937;
}

void faction_add_record(struct faction *a1, int a2, struct faction *a3)
{
    int l_14;
    int l_10;

    *(signed char *)D_00196732 = 0;
    mc_memset(((int)D_001966BC) + ((a2 << 2) + 4), 0, (int)&*(signed char *)((char *)((16 - a2) << 2) - 4), (int)D_00170464, 1091, 4);
    l_14 = rand();
    l_14 <<= 16;
    l_14 |= rand();
    a1->seed = l_14;
    a1->politics_factor = rand_range(0, 50) + 20;
    mc_memcpy(a3, a1, 92, (int)D_00170464, 1097, 4);
    if (a2 != 0) goto L1BC91;
    a3->parent = 0;
    goto L1BCE2;
L1BC91:;
    l_10 = a2 - 1;
L1BC98:;
    if (*(int *)(D_001966BC + (l_10 << 2)) != 0) goto L1BCAF;
    l_10--;
    goto L1BC98;
L1BCAF:;
    a3->parent = (struct faction *)(*(int *)(D_001966BC + (l_10 << 2)));
    if (*(int *)(D_001966BC + (a2 << 2)) != 0) goto L1BCE2;
    *(int *)(*(char **)(D_001966BC + (l_10 << 2)) + 84) = (int)a3;
L1BCE2:;
    if (*(int *)(D_001966BC + (a2 << 2)) == 0) goto L1BD03;
    *(int *)(*(char **)(D_001966BC + (a2 << 2)) + 80) = (int)a3;
L1BD03:;
    *(int *)(D_001966BC + (a2 << 2)) = (int)a3;
}

void faction_parse_type(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->type = atoi(l_C);
L1BD48:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1BD74;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1BD76;
L1BD74:;
    goto L1BD85;
L1BD76:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1BD8D;
L1BD85:;
    l_C++;
    goto L1BD48;
L1BD8D:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_name(struct faction *a1, int a2, int a3, int a4)
{
    int l_10;
    int l_C;

    l_C = 0;
    l_10 = *(int *)((char *)a2);
L1BDC2:;
    if (((int)(unsigned char)*(signed char *)((char *)l_10)) == 13) goto L1BDE0;
    if (((int)(unsigned char)*(signed char *)((char *)l_10)) != 10) goto L1BDE2;
L1BDE0:;
    goto L1BDF8;
L1BDE2:;
    a1->name[l_C++] = *(signed char *)((char *)l_10++);
    goto L1BDC2;
L1BDF8:;
    *(int *)((char *)a2) = l_10;
}

void faction_parse_rep(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->reputation = atoi(l_C);
L1BE37:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1BE63;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1BE65;
L1BE63:;
    goto L1BE74;
L1BE65:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1BE7C;
L1BE74:;
    l_C++;
    goto L1BE37;
L1BE7C:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_summon(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
L1BEAA:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1BED6;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1BED8;
L1BED6:;
    goto L1BEE7;
L1BED8:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1BEEF;
L1BEE7:;
    l_C++;
    goto L1BEAA;
L1BEEF:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_region(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->region = atoi(l_C);
    if (a1->region == 255) goto L1BF45;
    a1->region--;
L1BF45:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1BF71;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1BF73;
L1BF71:;
    goto L1BF82;
L1BF73:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1BF8A;
L1BF82:;
    l_C++;
    goto L1BF45;
L1BF8A:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_power(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->power = atoi(l_C);
L1BFC9:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1BFF5;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1BFF7;
L1BFF5:;
    goto L1C006;
L1BFF7:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C00E;
L1C006:;
    l_C++;
    goto L1BFC9;
L1C00E:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_flags(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->flags |= atoi(l_C);
L1C04D:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C079;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C07B;
L1C079:;
    goto L1C08A;
L1C07B:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C092;
L1C08A:;
    l_C++;
    goto L1C04D;
L1C092:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_ally(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    if (a3 != 4) goto L1C0C8;
    fatal_error((int)D_0017048A);
L1C0C8:;
    l_C = *(int *)((char *)a2);
    a1->allies[(*(int *)((char *)a3))++] = (struct faction *)atoi(l_C);
L1C0EF:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C11B;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C11D;
L1C11B:;
    goto L1C12C;
L1C11D:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C134;
L1C12C:;
    l_C++;
    goto L1C0EF;
L1C134:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_enemy(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    if (a4 != 4) goto L1C16A;
    fatal_error((int)D_001704A4);
L1C16A:;
    l_C = *(int *)((char *)a2);
    a1->enemies[(*(int *)((char *)a4))++] = (struct faction *)atoi(l_C);
L1C191:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C1BD;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C1BF;
L1C1BD:;
    goto L1C1CE;
L1C1BF:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C1D6;
L1C1CE:;
    l_C++;
    goto L1C191;
L1C1D6:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_ruler(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->ruler = atoi(l_C);
L1C214:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C240;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C242;
L1C240:;
    goto L1C251;
L1C242:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C259;
L1C251:;
    l_C++;
    goto L1C214;
L1C259:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_face(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->face = atoi(l_C);
L1C298:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) == 13) goto L1C2AF;
    l_C++;
    goto L1C298;
L1C2AF:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_vam(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->vampire_clan = atoi(l_C);
L1C2EE:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) == 13) goto L1C305;
    l_C++;
    goto L1C2EE;
L1C305:;
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
L1C344:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_14) + 1))) & 32)) != 0) goto L1C370;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 45) goto L1C372;
L1C370:;
    goto L1C381;
L1C372:;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 43) goto L1C389;
L1C381:;
    l_14++;
    goto L1C344;
L1C389:;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) > 32) goto L1C3A0;
    l_14++;
    goto L1C389;
L1C3A0:;
    a1->flats[(int)(unsigned char)*(signed char *)D_00196732] = (l_10 << 7) | atoi(l_14);
L1C3C0:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_14) + 1))) & 32)) != 0) goto L1C3EC;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 45) goto L1C3EE;
L1C3EC:;
    goto L1C3FD;
L1C3EE:;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 43) goto L1C405;
L1C3FD:;
    l_14++;
    goto L1C3C0;
L1C405:;
    *(int *)((char *)a2) = l_14;
    if (l_C != (-1)) goto L1C43A;
    a1->flats[(int)(unsigned char)*(signed char *)D_00196732] = ((unsigned short)(unsigned char)*(signed char *)(D_0017A25C + rand_range(0, 9))) + 23296;
L1C43A:;
    if (*(signed char *)D_00196732 != 0) goto L1C451;
    a1->flats[1] = a1->flats[0];
L1C451:;
    *(signed char *)D_00196732 ^= 1;
}

void faction_parse_race(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->race = atoi(l_C);
L1C48E:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C4BA;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C4BC;
L1C4BA:;
    goto L1C4CB;
L1C4BC:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C4D3;
L1C4CB:;
    l_C++;
    goto L1C48E;
L1C4D3:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_sgroup(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->social_group = atoi(l_C);
L1C511:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C53D;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C53F;
L1C53D:;
    goto L1C54E;
L1C53F:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C556;
L1C54E:;
    l_C++;
    goto L1C511;
L1C556:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_ggroup(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
    a1->guild_group = atoi(l_C);
L1C594:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C5C0;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C5C2;
L1C5C0:;
    goto L1C5D1;
L1C5C2:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C5D9;
L1C5D1:;
    l_C++;
    goto L1C594;
L1C5D9:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_minf(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
L1C607:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C633;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C635;
L1C633:;
    goto L1C644;
L1C635:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C64C;
L1C644:;
    l_C++;
    goto L1C607;
L1C64C:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_maxf(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
L1C67A:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_C) + 1))) & 32)) != 0) goto L1C6A6;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 45) goto L1C6A8;
L1C6A6:;
    goto L1C6B7;
L1C6A8:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) != 43) goto L1C6BF;
L1C6B7:;
    l_C++;
    goto L1C67A;
L1C6BF:;
    *(int *)((char *)a2) = l_C;
}

void faction_parse_rank(struct faction *a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = *(int *)((char *)a2);
L1C6ED:;
    if (((int)(unsigned char)*(signed char *)((char *)l_C)) == 13) goto L1C704;
    l_C++;
    goto L1C6ED;
L1C704:;
    *(int *)((char *)a2) = l_C;
}

int func_0001C7D6(int a1, int a2)
{
    struct record *l_1C;
    struct character *l_18;

    l_1C = player_entity->children;
L1C7F4:;
    if (l_1C == 0) goto L1C8CB;
    if (l_1C->type != (a1 + 45)) goto L1C8BD;
    if (a2 >= 1) goto L1C8B7;
    l_18 = &l_1C->data.character;
    mc_memcpy(l_18->skills, player_character->skills, 200, (int)D_00170464, 1352, 210);
    l_18->health = (l_18->max_health = player_character->max_health);
    l_18->magicka = (l_18->max_magicka = player_character->max_magicka);
    l_18->level = player_character->level;
    return (int)l_1C;
L1C8B7:;
    a2--;
L1C8BD:;
    l_1C = l_1C->next;
    goto L1C7F4;
L1C8CB:;
    return 0;
}

int func_0001C8DE(int a1, int a2)
{
    struct record *l_20;
    struct character *l_1C;
    int l_18;

    l_18 = 0;
    l_20 = player_entity->children;
L1C903:;
    if (l_20 == 0) goto L1C94E;
    if (l_20->type != (a2 + 45)) goto L1C943;
    l_1C = &l_20->data.character;
    if (a1 == (-1)) goto L1C93D;
    if (l_1C->career_id != a1) goto L1C943;
L1C93D:;
    l_18++;
L1C943:;
    l_20 = l_20->next;
    goto L1C903;
L1C94E:;
    return l_18;
}

void faction_save(int a1)
{
    int l_1C;
    int l_18;

    write(a1, (int)faction_count, 4);
    faction_save_r(a1, factions);
    faction_link_relations(factions);
}

void faction_save_r(int a1, struct faction *a2)
{
    int l_14;

L1C9B8:;
    if (a2 == 0) return;
    if (a2->child == 0) goto L1C9D9;
    faction_save_r(a1, a2->child);
L1C9D9:;
    l_14 = 0;
L1C9E0:;
    if (l_14 < 3) goto L1C9F0;
    goto L1CA1F;
L1C9E8:;
    l_14++;
    goto L1C9E0;
L1C9F0:;
    if (a2->allies[l_14] == 0) goto L1CA1D;
    a2->allies[l_14] = (struct faction *)(int)a2->allies[l_14]->id;
L1CA1D:;
    goto L1C9E8;
L1CA1F:;
    l_14 = 0;
L1CA26:;
    if (l_14 < 3) goto L1CA36;
    goto L1CA65;
L1CA2E:;
    l_14++;
    goto L1CA26;
L1CA36:;
    if (a2->enemies[l_14] == 0) goto L1CA63;
    a2->enemies[l_14] = (struct faction *)(int)a2->enemies[l_14]->id;
L1CA63:;
    goto L1CA2E;
L1CA65:;
    write(a1, a2, 92);
    a2 = a2->next;
    goto L1C9B8;
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
    l_18 = 0;
L1CABE:;
    if (l_18 < l_1C) goto L1CAD0;
    goto L1CB28;
L1CAC8:;
    l_18++;
    goto L1CABE;
L1CAD0:;
    func_000A00CB(a1, (int)&l_80, 92);
    if (l_80.reputation <= 100) goto L1CAEF;
    l_80.reputation = 100;
L1CAEF:;
    if (l_80.reputation >= (-100)) goto L1CAFE;
    l_80.reputation = 65436;
L1CAFE:;
    l_20 = faction_find(l_80.id);
    mc_memcpy(l_20, (int)&l_80, 80, (int)D_00170464, 1436, 4);
    goto L1CAC8;
L1CB28:;
    faction_link_relations(factions);
}
}

void func_0001CB3C(struct record *a1)
{
    struct faction *l_18;

    if (a1->type != 10) return;
    l_18 = faction_find(a1->data.membership.faction);
    if (l_18 == 0) return;
    if (l_18 == D_0019671C) goto L1CB8E;
    if (faction_has_enemy(l_18, D_0019671C) == 0) goto L1CB90;
L1CB8E:;
    goto L1CBA2;
L1CB90:;
    if (faction_has_ally(l_18, D_0019671C) == 0) return;
L1CBA2:;
    guild_membership = &a1->data.membership;
    (*(int *)D_00195B84)++;
}

int faction_player_related(struct faction *a1)
{
    if (a1 != 0) goto L1CBDD;
    return 0;
L1CBDD:;
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
L1CC34:;
    if (l_20 < 3) goto L1CC44;
    goto L1CC5B;
L1CC3C:;
    l_20++;
    goto L1CC34;
L1CC44:;
    if (a1->allies[l_20] == 0) goto L1CC59;
    l_1C++;
L1CC59:;
    goto L1CC3C;
L1CC5B:;
    return l_1C;
}

int faction_count_enemies(struct faction *a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = l_20;
L1CC8C:;
    if (l_20 < 3) goto L1CC9C;
    goto L1CCB3;
L1CC94:;
    l_20++;
    goto L1CC8C;
L1CC9C:;
    if (a1->enemies[l_20] == 0) goto L1CCB1;
    l_1C++;
L1CCB1:;
    goto L1CC94;
L1CCB3:;
    return l_1C;
}

int faction_make_alliance(struct faction *a1, int a2, struct faction *a3)
{
    if (faction_count_allies(a3) != 3) goto L1CCF1;
    return 0;
L1CCF1:;
    a1->allies[a2] = a3;
    a2 = 0;
L1CD07:;
    if (a3->allies[a2] == 0) goto L1CD1E;
    a2++;
    goto L1CD07;
L1CD1E:;
    a3->allies[a2] = a1;
    rumor_add_faction(a1, a3, 100, 0, 1400);
    return 1;
}

int faction_make_enemies(struct faction *a1, int a2, struct faction *a3)
{
    if (faction_count_enemies(a3) != 3) goto L1CD81;
    return 0;
L1CD81:;
    a1->enemies[a2] = a3;
    a2 = 0;
L1CD97:;
    if (a3->enemies[a2] == 0) goto L1CDAE;
    a2++;
    goto L1CD97;
L1CDAE:;
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
L1CE3F:;
    if (l_18->allies[a2] == a1) goto L1CE56;
    if (a2 < 3) goto L1CE58;
L1CE56:;
    goto L1CE60;
L1CE58:;
    a2++;
    goto L1CE3F;
L1CE60:;
    if (a2 != 3) goto L1CE6F;
    return 1;
L1CE6F:;
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
L1CEEB:;
    if (l_18->enemies[a2] == a1) goto L1CF02;
    if (a2 < 3) goto L1CF04;
L1CF02:;
    goto L1CF0C;
L1CF04:;
    a2++;
    goto L1CEEB;
L1CF0C:;
    if (a2 != 3) goto L1CF1B;
    return 1;
L1CF1B:;
    l_18->enemies[a2] = 0;
    return 1;
}

void rumor_file_open(void)
{
    *(int *)D_00196708 = 0;
    if ((*(int *)rumor_file = disk_open_rw((int)D_001704BB)) >= 0) goto L1D152;
    *(int *)rumor_file = disk_create((int)D_001704BB);
L1D152:;
    if (*(int *)rumor_file < 0) return;
    lseek(*(int *)rumor_file, 0, 2);
}

void rumor_file_close(void)
{
    if (*(int *)rumor_file <= (-1)) return;
    func_0009DEA7(*(int *)rumor_file);
}

int rumor_collect_local(void)
{
    int l_24;
    struct rumor *l_20;
    int l_1C;

    l_24 = *(int *)D_00147954 + 60000;
    func_000A0ED9(1642, (int)D_00170464);
    mc_sprintf(l_24, (int)D_001704C5, (int)current_location);
    l_24 += func_000A0DF4(l_24);
    *(signed char *)((char *)l_24) = 0;
    *(signed char *)((char *)l_24 + 1) = 0;
    if (disk_file_exists((int)D_001704BB) != 0) goto L1D21C;
    return *(int *)D_00147954 + 60000;
L1D21C:;
    disk_read_file((int)D_001704BB, *(int *)D_00147954);
    if (*(int *)disk_last_file_size != 0) goto L1D241;
    return 0;
L1D241:;
    l_1C = (int)(*(char **)D_00147954 + *(int *)disk_last_file_size);
    l_20 = (struct rumor *)*(int *)D_00147954;
L1D259:;
    if (((unsigned)l_20) >= l_1C) goto L1D2C9;
    if (func_0001D54B((int)l_20, 0, 1, 0) == 0) goto L1D2B6;
    mc_memcpy(l_24, (int)l_20 + 34, l_20->text_length, (int)D_00170464, 1658, 4);
    l_24 += l_20->text_length - 1;
    *(signed char *)((char *)l_24 + 1) = 252;
    *(signed char *)((char *)l_24) = *(signed char *)((char *)l_24 + 1);
    l_24 += 2;
L1D2B6:;
    l_20 = (struct rumor *)(((int)l_20 + l_20->text_length) + 34);
    goto L1D259;
L1D2C9:;
    *(signed char *)((char *)l_24) = 0;
    *(signed char *)((char *)l_24 + 1) = 0;
    return *(int *)D_00147954 + 60000;
}

int rumor_pick_news(short a1)
{
    short l_20;
    char l_40[28];
    short l_1C;

    *(int *)((char *)l_40 + 16) = 0;
    *(int *)&l_1C = 0;
    if (disk_file_exists((int)D_001704BB) != 0) goto L1D329;
    return 0;
L1D329:;
    disk_read_file((int)D_001704BB, *(int *)D_00195C44);
    if (*(int *)disk_last_file_size != 0) goto L1D34E;
    return 0;
L1D34E:;
    *(int *)((char *)l_40 + 24) = (int)(*(char **)D_00195C44 + *(int *)disk_last_file_size);
    *(int *)&l_20 = *(int *)D_00195C44;
    *(int *)l_40 = rand_range(1, 100);
    *(int *)((char *)l_40 + 4) = rand_range(1, 100);
    *(int *)((char *)l_40 + 8) = rand_range(1, 100);
    *(int *)((char *)l_40 + 12) = rand_range(1, 100);
    *(int *)((char *)l_40 + 20) = *(int *)D_00195C44 + 40000;
L1D3BB:;
    if (((unsigned)*(int *)&l_20) >= *(int *)((char *)l_40 + 24)) goto L1D40E;
    if (func_0001D54B(*(int *)&l_20, (int)(short)a1, 0, *(int *)((char *)l_40 + ((*(int *)&l_1C & 3) << 2))) == 0) goto L1D3F5;
    *(int *)((char *)(int)(*(char **)((char *)l_40 + 20) + ((*(int *)((char *)l_40 + 16))++ << 2))) = *(int *)&l_20;
L1D3F5:;
    *(int *)&l_20 = (*(int *)&l_20 + *(int *)(*(char **)&l_20 + 26)) + 34;
    (*(int *)&l_1C)++;
    goto L1D3BB;
L1D40E:;
    if (*(int *)((char *)l_40 + 16) != 0) goto L1D41D;
    return 0;
L1D41D:;
    *(int *)&l_20 = *(int *)((char *)((rand_range(0, *(int *)((char *)l_40 + 16) - 1) << 2) + *(int *)((char *)l_40 + 20)));
    mc_memcpy(*(int *)D_00195C44, *(int *)&l_20 + 34, *(int *)(*(char **)&l_20 + 26), (int)D_00170464, 1701, 4);
    return *(int *)D_00195C44;
}

int func_0001D46A(int a1)
{
    struct rumor *l_20;
    int l_1C;

    if (disk_file_exists((int)D_001704BB) != 0) goto L1D495;
    return 0;
L1D495:;
    disk_read_file((int)D_001704BB, *(int *)D_00195C44);
    if (*(int *)disk_last_file_size != 0) goto L1D4BA;
    return 0;
L1D4BA:;
    l_1C = (int)(*(char **)D_00195C44 + *(int *)disk_last_file_size);
    l_20 = (struct rumor *)*(int *)D_00195C44;
L1D4D2:;
    if (((unsigned)l_20) >= l_1C) goto L1D537;
    if (((int)(unsigned char)(l_20->flags & 2)) == 0) goto L1D4F6;
    if (l_20->target == a1) goto L1D4F8;
L1D4F6:;
    goto L1D524;
L1D4F8:;
    mc_memcpy(*(int *)D_00195C44, (int)l_20 + 34, l_20->text_length, (int)D_00170464, 1720, 4);
    return *(int *)D_00195C44;
L1D524:;
    l_20 = (struct rumor *)(((int)l_20 + l_20->text_length) + 34);
    goto L1D4D2;
L1D537:;
    return 0;
}

int func_0001D66C(struct rumor *a1)
{
    int l_1C;

    l_1C = 0;
    if (a1->kind == 10) goto L1D696;
    if (a1->kind != 18) goto L1D698;
L1D696:;
    goto L1D6A1;
L1D698:;
    if (a1->kind != 7) goto L1D6A3;
L1D6A1:;
    goto L1D6AC;
L1D6A3:;
    if (a1->kind != 4) goto L1D6AE;
L1D6AC:;
    goto L1D6B7;
L1D6AE:;
    if (a1->kind != 28) goto L1D6B9;
L1D6B7:;
    goto L1D6C2;
L1D6B9:;
    if (a1->kind != 27) goto L1D6C4;
L1D6C2:;
    goto L1D6CD;
L1D6C4:;
    if (a1->kind != 26) goto L1D6D1;
L1D6CD:;
    l_1C |= 1;
L1D6D1:;
    if (a1->kind == 10) goto L1D6E3;
    if (a1->kind != 18) goto L1D6E5;
L1D6E3:;
    goto L1D6EE;
L1D6E5:;
    if (a1->kind != 7) goto L1D6F0;
L1D6EE:;
    goto L1D6F9;
L1D6F0:;
    if (a1->kind != 4) goto L1D6FB;
L1D6F9:;
    goto L1D704;
L1D6FB:;
    if (a1->kind != 28) goto L1D706;
L1D704:;
    goto L1D70F;
L1D706:;
    if (a1->kind != 27) goto L1D711;
L1D70F:;
    goto L1D71A;
L1D711:;
    if (a1->kind != 26) goto L1D722;
L1D71A:;
    return l_1C;
L1D722:;
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
    disk_read_file((int)D_001704BB, *(int *)D_00147954);
    if (*(int *)disk_last_file_size == 0) return;
    l_28 = (struct rumor *)*(int *)D_00147954;
    l_24 = *(int *)D_00195C44;
    l_20 = (int)(*(char **)D_00147954 + *(int *)disk_last_file_size);
L1D911:;
    if (((unsigned)l_28) >= l_20) goto L1DA0A;
    if (((int)(unsigned char)(l_28->flags & 4)) == 0) goto L1D975;
    l_1C = quest_find_by_id((int)(short)((unsigned short)l_28->quest_id));
    if (l_1C == 0) goto L1D9F4;
    if (stricmp(l_1C->name, (int)l_28 + 11) != 0) goto L1D9F4;
    l_24 = rumor_copy(l_24, l_28);
    goto L1D9F4;
L1D975:;
    if (((unsigned)*(int *)game_minutes) > l_28->expires) goto L1D9F4;
    if (((int)(unsigned char)(l_28->flags & 8)) == 0) goto L1D9A8;
    l_24 = rumor_copy(l_24, l_28);
    goto L1D9F4;
L1D9A8:;
    if (((int)(unsigned char)(l_28->flags & 2)) == 0) goto L1D9F4;
    if (((int)(unsigned char)(l_28->flags & 32)) != 0) goto L1D9F4;
    l_18++;
    l_24 = rumor_copy(l_24, l_28);
    if (l_18 <= 200) goto L1D9F4;
    func_0001DA9C((struct rumor *)*(int *)D_00195C44, l_24);
L1D9F4:;
    l_28 = (struct rumor *)(((int)l_28 + l_28->text_length) + 34);
    goto L1D911;
L1DA0A:;
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
L1DAAF:;
    if (((unsigned)a1) >= a2) return;
    if (((int)(unsigned char)(a1->flags & 2)) == 0) goto L1DAD1;
    a1->flags |= 32;
    return;
L1DAD1:;
    a1 = (struct rumor *)(((int)a1 + a1->text_length) + 34);
    goto L1DAAF;
}
