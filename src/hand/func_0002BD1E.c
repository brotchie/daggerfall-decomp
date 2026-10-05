/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002BD1E */
extern char D_00170878[];
extern char D_0017088A[];
extern char D_0017089B[];
extern char D_001708B7[];
extern char D_001708D1[];
extern char nonworld_root[];
extern char qbn_opcode_arg_counts[];
extern void fatal_error(char *);
extern int object_find_by_id(int, char *);

#pragma pack(1)
struct Obj { char pad[31]; char *next; char pad2[3]; unsigned char owner; };
struct E15 { char b0; int base; char pad[6]; struct Obj *obj; };
struct E87 { short kind; short pad; short n; struct E15 e[5]; char pad2[6]; };
struct E20 { char pad[12]; struct Obj *obj; char pad2[4]; };
struct E24 { char pad[3]; signed char type; unsigned short hi; unsigned short lo; char pad2[8]; struct Obj *obj; char pad3[4]; };
struct E19 { char pad[11]; struct Obj *obj; char pad2[4]; };
struct E14 { char pad[10]; struct Obj *obj; };
struct E33 { char pad[21]; struct Obj *obj1; struct Obj *obj2; char pad2[4]; };
struct Hdr {
    short id; char pad[14];
    short n19; char pad2[4]; short n20; short n24; char pad3[2]; short n33; short n14; short n87;
    char pad4[2]; short off19; char pad5[4]; short off20; short off24; char pad6[2]; short off33;
    short off14; short off87;
};
#pragma pack()

#define FIX(o, msg) \
    if (o) { \
        if ((short)o->owner != a1->id) fatal_error(msg); \
        o = (struct Obj *)o->next; \
    }

void quest_unlink_for_save(struct Hdr *a1)
{
    struct E87 *l_48;
    struct E15 *l_44;
    int l_40;
    int l_3C;
    struct E24 *l_38;
    struct E20 *l_34;
    struct E19 *l_30;
    struct E14 *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct E33 *l_18;

    l_48 = (struct E87 *)((char *)a1 + a1->off87);
    for (l_1C = 0; l_1C < a1->n87; l_1C++, l_48++) {
        l_44 = l_48->e;
        l_48->n = (*(unsigned char **)qbn_opcode_arg_counts)[l_48->kind] - '0';
        for (l_20 = 0; l_20 < l_48->n; l_20++, l_44++) {
            if (l_44->base) l_44->base -= (int)a1;
            if (l_44->obj) l_44->obj = (struct Obj *)l_44->obj->next;
        }
    }
    l_34 = (struct E20 *)((char *)a1 + a1->off20);
    for (l_1C = 0; l_1C < a1->n20; l_1C++, l_34++) {
        FIX(l_34->obj, D_00170878)
    }
    l_38 = (struct E24 *)((char *)a1 + a1->off24);
    for (l_1C = 0; l_1C < a1->n24; l_1C++, l_38++) {
        if (l_38->obj) {
            if ((short)l_38->obj->owner != a1->id) fatal_error(D_0017088A);
            l_38->obj = (struct Obj *)l_38->obj->next;
            if (object_find_by_id(*(int *)nonworld_root, (char *)l_38->obj) == 0) fatal_error(D_0017089B);
            if (l_38->obj == 0) fatal_error(D_001708B7);
            if (l_38->type == 10) {
                if (((l_38->lo & 0xffff) | (l_38->hi << 16)) != (int)l_38->obj)
                    fatal_error(D_001708D1);
            }
        }
    }
    l_30 = (struct E19 *)((char *)a1 + a1->off19);
    for (l_1C = 0; l_1C < a1->n19; l_1C++, l_30++) {
        FIX(l_30->obj, D_00170878)
    }
    l_2C = (struct E14 *)((char *)a1 + a1->off14);
    for (l_1C = 0; l_1C < a1->n14; l_1C++, l_2C++) {
        FIX(l_2C->obj, D_00170878)
    }
    l_18 = (struct E33 *)((char *)a1 + a1->off33);
    for (l_1C = 0; l_1C < a1->n33; l_1C++, l_18++) {
        FIX(l_18->obj1, D_00170878)
        FIX(l_18->obj2, D_00170878)
    }
}
