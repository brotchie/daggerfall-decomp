/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002BD1E */
#include "records.h"

extern char D_00170878[];
extern char D_0017088A[];
extern char D_0017089B[];
extern char D_001708B7[];
extern char D_001708D1[];
extern struct record *nonworld_root;
extern char qbn_opcode_arg_counts[];
extern void fatal_error(char *);
extern struct record *object_find_by_id(struct record *, int);

/* a resource's object pointer back to the object's id, checking it belongs to this quest */
#define FIX(o, msg) \
    if (o) { \
        if ((short)o->quest_id != a1->id) fatal_error(msg); \
        o = (struct record *)o->id; \
    }

void quest_unlink_for_save(struct quest *a1)
{
    struct qbn_op *l_48;
    struct qbn_arg *l_44;
    int l_40;
    int l_3C;
    struct qbn_place *l_38;
    struct qbn_person *l_34;
    struct qbn_item *l_30;
    struct qbn_foe *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct qbn_timer *l_18;

    l_48 = (struct qbn_op *)((char *)a1 + a1->section_offsets[8]);
    for (l_1C = 0; l_1C < a1->section_counts[8]; l_1C++, l_48++) {
        l_44 = l_48->args;
        l_48->arg_count = (*(unsigned char **)qbn_opcode_arg_counts)[l_48->opcode] - '0';
        for (l_20 = 0; l_20 < l_48->arg_count; l_20++, l_44++) {
            if (l_44->record) l_44->record -= (int)a1;
            if (l_44->object) l_44->object = (struct record *)l_44->object->id;
        }
    }
    l_34 = (struct qbn_person *)((char *)a1 + a1->section_offsets[3]);
    for (l_1C = 0; l_1C < a1->section_counts[3]; l_1C++, l_34++) {
        FIX(l_34->object, D_00170878)
    }
    l_38 = (struct qbn_place *)((char *)a1 + a1->section_offsets[4]);
    for (l_1C = 0; l_1C < a1->section_counts[4]; l_1C++, l_38++) {
        if (l_38->object) {
            if ((short)l_38->object->quest_id != a1->id) fatal_error(D_0017088A);
            l_38->object = (struct record *)l_38->object->id;
            if (object_find_by_id(nonworld_root, (int)l_38->object) == 0) fatal_error(D_0017089B);
            if (l_38->object == 0) fatal_error(D_001708B7);
            /* +0x03 the place's type (10: a fixed object), +0x04/+0x06 the object id's halves */
            if (l_38->scope == 10) {
                if ((((unsigned short)l_38->p2 & 0xffff) | (l_38->p1 << 16)) != (int)l_38->object)
                    fatal_error(D_001708D1);
            }
        }
    }
    l_30 = (struct qbn_item *)((char *)a1 + a1->section_offsets[0]);
    for (l_1C = 0; l_1C < a1->section_counts[0]; l_1C++, l_30++) {
        FIX(l_30->object, D_00170878)
    }
    l_2C = (struct qbn_foe *)((char *)a1 + a1->section_offsets[7]);
    for (l_1C = 0; l_1C < a1->section_counts[7]; l_1C++, l_2C++) {
        FIX(l_2C->object, D_00170878)
    }
    l_18 = (struct qbn_timer *)((char *)a1 + a1->section_offsets[6]);
    for (l_1C = 0; l_1C < a1->section_counts[6]; l_1C++, l_18++) {
        FIX(l_18->link1, D_00170878)
        FIX(l_18->link2, D_00170878)
    }
}
