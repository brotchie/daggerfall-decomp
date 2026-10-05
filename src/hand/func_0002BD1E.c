/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002BD1E */
#include "records.h"

extern char D_00170878[];
extern char D_0017088A[];
extern char D_0017089B[];
extern char D_001708B7[];
extern char D_001708D1[];
extern struct record *nonworld_root;
extern int qbn_opcode_arg_counts;
extern void fatal_error(char *);
extern struct record *object_find_by_id(struct record *, int);

/* a resource's object pointer back to the object's id, checking it belongs to this quest */
#define FIX(o, msg) \
    if (o) { \
        if ((short)o->quest_id != quest->id) fatal_error(msg); \
        o = (struct record *)o->id; \
    }

void quest_unlink_for_save(struct quest *quest)
{
    struct qbn_op *op;
    struct qbn_arg *arg;
    int unused1;
    int unused2;
    struct qbn_place *qbn_place;
    struct qbn_person *qbn_person;
    struct qbn_item *qbn_item;
    struct qbn_foe *foe;
    int unused3;
    int unused4;
    int j;
    int i;
    struct qbn_timer *timer;

    op = (struct qbn_op *)((char *)quest + quest->section_offsets[8]);
    for (i = 0; i < quest->section_counts[8]; i++, op++) {
        arg = op->args;
        op->arg_count = (*(unsigned char **)&qbn_opcode_arg_counts)[op->opcode] - '0';
        for (j = 0; j < op->arg_count; j++, arg++) {
            if (arg->record) arg->record -= (int)quest;
            if (arg->object) arg->object = (struct record *)arg->object->id;
        }
    }
    qbn_person = (struct qbn_person *)((char *)quest + quest->section_offsets[3]);
    for (i = 0; i < quest->section_counts[3]; i++, qbn_person++) {
        FIX(qbn_person->object, D_00170878)
    }
    qbn_place = (struct qbn_place *)((char *)quest + quest->section_offsets[4]);
    for (i = 0; i < quest->section_counts[4]; i++, qbn_place++) {
        if (qbn_place->object) {
            if ((short)qbn_place->object->quest_id != quest->id) fatal_error(D_0017088A);
            qbn_place->object = (struct record *)qbn_place->object->id;
            if (object_find_by_id(nonworld_root, (int)qbn_place->object) == 0) fatal_error(D_0017089B);
            if (qbn_place->object == 0) fatal_error(D_001708B7);
            /* +0x03 the place's type (10: a fixed object), +0x04/+0x06 the object id's halves */
            if (qbn_place->scope == 10) {
                if ((((unsigned short)qbn_place->p2 & 0xffff) | (qbn_place->p1 << 16)) != (int)qbn_place->object)
                    fatal_error(D_001708D1);
            }
        }
    }
    qbn_item = (struct qbn_item *)((char *)quest + quest->section_offsets[0]);
    for (i = 0; i < quest->section_counts[0]; i++, qbn_item++) {
        FIX(qbn_item->object, D_00170878)
    }
    foe = (struct qbn_foe *)((char *)quest + quest->section_offsets[7]);
    for (i = 0; i < quest->section_counts[7]; i++, foe++) {
        FIX(foe->object, D_00170878)
    }
    timer = (struct qbn_timer *)((char *)quest + quest->section_offsets[6]);
    for (i = 0; i < quest->section_counts[6]; i++, timer++) {
        FIX(timer->link1, D_00170878)
        FIX(timer->link2, D_00170878)
    }
}
