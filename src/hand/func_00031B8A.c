/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00031B8A */
#include "records.h"

#pragma pack(1)
struct Kind { char pad[6]; unsigned char flags; };
struct Ent6 { unsigned short a; unsigned short b; unsigned short c; };
struct Req {                    /* a QBN person (struct qbn_person), its +0x02 read as a u16 */
    char pad0[2];
    short flags;                /* 2 */
    short kind;                 /* 4 */
    short arg;                  /* 6 */
    char pad1[4];
    struct record *obj;         /* 12 */
};
#pragma pack()
/* a quest NPC's data (types 41, 65): a copy of its building record, then 4 bytes of the
 * town name at +0x1A */
#define NPC_SITE(r) ((struct building *)&(r)->data)
extern char D_00170A64[];
extern unsigned char D_0017A25C[];
extern short D_0017A270[];
extern struct record *nonworld_root;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern char *D_00195C44;
extern struct record *D_00195CE8;
extern unsigned char current_region;
extern int loaded_location_door_count;
extern struct Ent6 *loaded_location_doors;
extern char D_001970C8[];
extern int D_001970CC;
extern struct Ent6 *D_001970D0;
extern struct record *D_001970D4;
extern struct location *D_001970D8;
extern char D_001970DC;
extern unsigned char D_001970DD;
extern struct quest *current_quest;
extern struct faction *faction_find_type_in_region(short, int);
extern struct faction *faction_find(short);
extern struct faction *faction_random_of_type(unsigned char);
extern struct qbn_person *quest_record(struct quest *, int, int);
extern int func_000339B2(struct Ent6 *, struct Req *, struct building *, int);
extern struct faction *func_0003445C(struct faction **);
extern int func_000344D3(void);
extern int quest_object_in_use(int);
extern struct Kind *flats_cfg_find(unsigned short);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern void location_free(char *);
extern void location_pick_random_town(char *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int rand(void);
extern int mc_strncpy(char *, char *, int, char *, int);
extern int mc_memcpy(void *, void *, int, char *, int, int);

int quest_init_person(struct Req *r)
{
    struct Ent6 *list;
    struct Ent6 *e;
    int n;
    int found;
    int i;
    int any;
    int tries;
    int npc;
    struct record *src;
    struct record *it;
    struct record *it2;
    struct location *rec;
    int *buf;
    struct building *sub;
    struct Kind *kind;
    struct faction *np;
    struct qbn_person *slot;

    npc = 0;
    r->flags &= 0x0fff;
    if (r->kind == 21) {
        if (D_00195CE8 == 0 || D_00195CE8->twin != 0)
            return 0;
        kind = flats_cfg_find(D_00195CE8->image);
        it = object_create_child(nonworld_root, 0, 58);
        sub = object_building(player_object);
        if (sub != 0) {
            if (sub->faction_id == 65535)
                sub->faction_id = 510;
            if (sub->faction_id == 0)
                sub->faction_id = faction_find_type_in_region(current_region, 15)->id;
            mc_memcpy(&it->data, sub, 26, D_00170A64, 64, 4);
        } else if (*(short *)RECORD_DATA(D_00195CE8) != 0) {
            NPC_SITE(it)->faction_id = *(short *)RECORD_DATA(D_00195CE8);
        }
        if (*(short *)RECORD_DATA(D_00195CE8) != 0)
            it->pad19 = *(short *)RECORD_DATA(D_00195CE8);
        else
            it->pad19 = sub->faction_id;
        mc_strncpy(RECORD_DATA(it) + 26, (char *)current_location, 4, D_00170A64, 76);
        mc_memcpy(&it->x, &D_00195CE8->x, 12, D_00170A64, 77, 4);
        it->type = 41;
        it->image = D_00195CE8->image;
        r->obj = it;
        it->flags = 514;
        it->flags |= (int)(unsigned char)(kind->flags & 1) != 0 ? 4 : 0;
        it->owner = (unsigned short)current_region;
        it->quest_id = current_quest->id;
        it->id = D_00195CE8->id;
        it->repair_due = it->id;
        if ((int)(unsigned short)(it->flags & 4) != 0)
            *(RECORD_DATA(it) + 2) |= 16;
        else
            *(RECORD_DATA(it) + 2) &= ~16;
        return 1;
    }
    if (r->kind == -8) {
        it = object_create_child(nonworld_root, 0, 0);
        it->type = 65;
        it->image = 0;
        r->obj = it;
        it->repair_due = 0;
        it->id = object_new_id(800);
        it->flags = 514;
        it->pad19 = r->arg;
        it->flags |= (int)(unsigned char)(flats_cfg_find(it->image)->flags & 1) != 0 ? 4 : 0;
        it->owner = (unsigned short)current_region;
        it->quest_id = current_quest->id;
        return 1;
    }
    if (!(r->kind != -1 || (int)(short)(r->flags & 0x100) != 0)) {
        np = faction_find(r->arg);
        if (np->type == 4) {
            kind = flats_cfg_find(np->flats[0]);
            it = object_create_child(nonworld_root, 0, 58);
            it->pad19 = np->id;
            it->type = 41;
            it->image = np->flats[0];
            it->flags = 514;
            it->flags |= (int)(unsigned char)(kind->flags & 1) != 0 ? 4 : 0;
            it->owner = (unsigned short)current_region;
            it->quest_id = current_quest->id;
            r->obj = it;
            it->repair_due = it->id = object_new_id(800);
            NPC_SITE(it)->faction_id = r->arg;
            return 1;
        }
    }
    if (r->kind == -2) {
        if (r->arg == 10)
            npc = D_0017A270[rand_range(0, 9)];
        else
            npc = faction_random_of_type(r->arg)->id;
        r->kind = -1;
    }
    if (!(r->kind != -4 || r->arg != 10000)) {
        i = func_000344D3();
        if (i == 0) {
            r->kind = -6;
        } else {
            r->kind = -1;
            r->arg = i;
        }
    }
    if (r->kind >= -5 && r->kind <= -3 || r->kind == -7) {
        slot = quest_record(current_quest, 3, r->arg);
        if (slot == 0) {
            r->obj = 0;
            D_001970DC++;
            return 1;
        }
        it = slot->object;
        if (it == 0) {
            r->obj = 0;
            D_001970DC++;
            return 1;
        }
        switch ((unsigned short)(r->kind + 7)) {
        case 0:
            ((unsigned char *)&r->flags)[1] &= 0xf9;
            r->flags |= (int)(unsigned short)(it->flags & 4) != 0 ? 1024 : 512;
            r->kind = -6;
            break;
        case 2:
            r->arg = NPC_SITE(it)->faction_id;
            r->kind = -1;
            break;
        case 3:
            np = faction_find(NPC_SITE(it)->faction_id);
            if (np != 0) {
                np = func_0003445C(np->enemies);
            } else {
                r->obj = 0;
                r->kind = -5;
                D_001970DC++;
                return 1;
            }
            if (np != 0)
                r->arg = np->id;
            else
                r->arg = 510;
            r->kind = -1;
            break;
        case 4:
            np = faction_find(NPC_SITE(it)->faction_id);
            if (np != 0) {
                np = func_0003445C(np->allies);
            } else {
                r->obj = 0;
                r->kind = -5;
                D_001970DC++;
                return 1;
            }
            if (np != 0)
                r->arg = np->id;
            else
                r->arg = 510;
            r->kind = -1;
            break;
        }
    }
    if ((int)(short)(r->flags & 0xff) == 255)
        r->flags = (r->flags & 0xff00) + (rand() & 1);
    tries = 0;
retry:
    if ((int)(short)(r->flags & 0xff) == 0) {
        list = loaded_location_doors;
        n = loaded_location_door_count;
        src = D_00195AC4;
        rec = current_location;
    } else {
        location_free(D_001970C8);
        location_pick_random_town(D_001970C8);
        n = D_001970CC;
        list = D_001970D0;
        src = D_001970D4;
        rec = D_001970D8;
    }
    buf = (int *)D_00195C44;
    found = 0;
    any = 0;
    if ((int)(short)(r->flags & 0x600) != 0)
        D_001970DD = (int)(short)(r->flags & 0x200) != 0 ? 1 : 0;
    else
        D_001970DD = rand() & 1;
    if ((int)(short)(r->flags & 0x100) == 0) {
        for (found = i = 0, e = list; i < n; i++, e++) {
            if (e->a != 65535 && func_000339B2(e, r, &rec->buildings[e->a], 0) != 0)
                buf[found++] = (e->a << 16) + e->c;
        }
    }
    if (r->kind != -6 && found == 0 || (int)(short)(r->flags & 0x100) != 0) {
        any = 1;
        for (i = 0, e = list; i < n; i++, e++) {
            if ((int)(short)(r->flags & 0x100) != 0 && func_000339B2(e, r, &rec->buildings[e->a], 1) != 0)
                buf[found++] = (e->a << 16) + e->c;
            else if (e->a != 65535 && func_000339B2(e, r, &rec->buildings[e->a], 1) != 0)
                buf[found++] = (e->a << 16) + e->c;
        }
    }
    if (found == 0 && (int)(short)(r->flags & 0xff) == 0) {
        if (r->kind == -1)
            npc = r->arg;
        if (r->kind == -2)
            npc = faction_random_of_type(r->arg)->id;
        r->kind = -6;
    }
    if (found == 0 && r->kind != -6) {
        if (r->kind == -1)
            npc = r->arg;
        if (r->kind == -2)
            npc = faction_random_of_type(r->arg)->id;
        if (tries < 50)
            goto retry;
        r->kind = -6;
        tries = 0;
        goto retry;
    }
    if (found == 0)
        return 0;
    i = rand() % found;
    if (any != 0 && quest_object_in_use((src->id & 0xffff0000) + (buf[i] & 0xffff)) != 0 && ++tries < 100)
        goto retry;
    it = object_create_child(nonworld_root, 0, 58);
    mc_memcpy(&it->x, &src->x, 12, D_00170A64, 307, 4);
    mc_memcpy(&it->data, &rec->buildings[(unsigned)buf[i] >> 16], 26, D_00170A64, 308, 4);
    mc_strncpy(RECORD_DATA(it) + 26, (char *)rec, 4, D_00170A64, 309);
    if (NPC_SITE(it)->faction_id == 0)
        NPC_SITE(it)->faction_id = faction_find_type_in_region(current_region, 15)->id;
    if (npc != 0)
        NPC_SITE(it)->faction_id = npc;
    else if (r->kind == -1)
        NPC_SITE(it)->faction_id = r->arg;
    it->type = 41;
    it->image = 0;
    r->obj = it;
    if (any != 0 && (int)(short)(r->flags & 0x100) != 0) {
        it->id = object_new_id((unsigned)src->id >> 16);
        it->repair_due = it->id;
    } else {
        it->id = (src->id & 0xffff0000) + (buf[i] & 0xffff);
        it->repair_due = it->id;
    }
    it->flags = 514;
    if (npc != 0) {
        it->image = faction_find(npc)->flats[D_001970DD];
        it->pad19 = npc;
    } else if (r->kind == -1) {
        it->image = faction_find(r->arg)->flats[D_001970DD];
        it->pad19 = r->arg;
    } else if ((short)NPC_SITE(it)->faction_id > 0) {
        it->image = faction_find(NPC_SITE(it)->faction_id)->flats[D_001970DD];
        it->pad19 = NPC_SITE(it)->faction_id;
    } else {
        it->image = D_0017A25C[D_001970DD * 10 + rand_range(0, 9)] + 23296;
        it->pad19 = 510;
    }
    kind = flats_cfg_find(it->image);
    it->flags |= (int)(unsigned char)(kind->flags & 1) != 0 ? 4 : 0;
    it->owner = (unsigned short)current_region;
    it->quest_id = current_quest->id;
    if (any != 0 && (int)(short)(r->flags & 0x100) == 0) {
        it2 = object_create_child(nonworld_root, 0, 26);
        it2->type = 40;
        it2->id = it->id;
        it2->quest_id = current_quest->id;
        object_reparent(it2, it);
        it->id = object_new_id((unsigned)it->id >> 16);
    }
    location_free(D_001970C8);
    return 1;
}
