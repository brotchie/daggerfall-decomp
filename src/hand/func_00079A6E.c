/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00079A6E */
#include "records.h"
#include "clib.h"

extern char D_00176884[];        /* __FILE__ */
extern char D_0017688F[];
extern char *scratch_buffer;
extern int save_file_handle;
extern void fatal_error(char *);
extern void save_unlink_character(struct record *);
#pragma aux mc_set_location parm routine [];

int savetree_write_record(struct record *object)
{
    struct record *buf;
    int len;
    int unused1;
    int unused2;

    buf = (struct record *)scratch_buffer;
    len = *(int *)((char *)object - 6);     /* the heap block's size */
    if (len == 0) {
        mc_set_location(84, D_00176884);
        mc_sprintf(scratch_buffer, D_0017688F);
        fatal_error(scratch_buffer);
    }
    write(save_file_handle, &len, 4);
    mc_memcpy(buf, object, len, D_00176884, 90, 4);
    if (buf->twin != 0) {
        if (buf->quest_id != 0)
            buf->twin = (struct record *)(uptr)buf->twin->id;
        else
            buf->twin = 0;
    }
    switch (object->type) {
    case 3:
    case 18:
    case 44:
        save_unlink_character(buf);
        break;
    case 9:
        if (object->parent->type == 3 || object->parent->type == 18)
            buf->caster = (struct record *)(uptr)buf->caster->id;
        break;
    }
    if (object->parent != 0) {
        *(int *)&buf->parent = object->parent->type;
        buf->parent_id = object->parent->id;
    }
    return write(save_file_handle, buf, len) != len ? 1 : 0;
}
