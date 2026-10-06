/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00011A6B */
#include "structs.h"
#include "clib.h"
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_open;
extern char D_00170129[];        /* __FILE__ */

int profile_open(struct profile *r, char *name)
{
    int fd;

    mc_strncpy(r->path, name, 4, D_00170129, 77);
    fd = open(name, 512);
    if (fd == -1)
        return 0;
    r->length = lseek(fd, 0, 2);
    r->capacity = r->length + 1024;
    lseek(fd, 0, 0);
    if ((r->buffer = mc_malloc(r->capacity, D_00170129, 94)) == 0) {
        close(fd);
        return 0;
    }
    if (read(fd, r->buffer, r->length) != r->length) {
        close(fd);
        if (r->buffer != 0 && r->buffer != (char *)0x97979797) {
            mc_free(r->buffer, D_00170129, 110);
            r->buffer = (char *)0x97979797;
        }
        return 0;
    }
    close(fd);
    r->section = r->buffer;
    r->section_offset = 0;
    r->value = 0;
    r->next_value = 0;
    r->item = 0;
    r->line = 0;
    r->flags &= 127;
    return 1;
}
