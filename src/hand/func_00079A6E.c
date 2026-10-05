/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00079A6E */
#include "records.h"

extern char D_00176884[];        /* __FILE__ */
extern char D_0017688F[];
extern char *D_00195C44;
extern int save_file_handle;
extern void fatal_error(char *);
extern void save_unlink_character(struct record *);
extern int write(int, void *, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(void *, char *, ...);

int savetree_write_record(struct record *a1)
{
    struct record *buf;
    int len;
    int unused1;
    int unused2;

    buf = (struct record *)D_00195C44;
    len = *(int *)((char *)a1 - 6);     /* the heap block's size */
    if (len == 0) {
        func_000A0ED9(84, D_00176884);
        mc_sprintf(D_00195C44, D_0017688F);
        fatal_error(D_00195C44);
    }
    write(save_file_handle, &len, 4);
    mc_memcpy(buf, a1, len, D_00176884, 90, 4);
    if (buf->twin != 0) {
        if (buf->quest_id != 0)
            buf->twin = (struct record *)buf->twin->id;
        else
            buf->twin = 0;
    }
    switch (a1->type) {
    case 3:
    case 18:
    case 44:
        save_unlink_character(buf);
        break;
    case 9:
        if (a1->parent->type == 3 || a1->parent->type == 18)
            buf->caster = (struct record *)buf->caster->id;
        break;
    }
    if (a1->parent != 0) {
        *(int *)&buf->parent = a1->parent->type;
        buf->parent_id = a1->parent->id;
    }
    return write(save_file_handle, buf, len) != len ? 1 : 0;
}
