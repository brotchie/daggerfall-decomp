/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000369A0 */
#include "records.h"

extern char D_00170AB4[];
extern int atoi(char *);
extern void func_000A14E8(char *, char *, int, char *, int, int);

void rdb_model_id_from_name(struct record *object, char *name)
{
    int x;
    char buf[12];

    func_000A14E8(buf, name, 3, D_00170AB4, 347, 9);
    buf[3] = 0;
    object->image2 = atoi(buf);
    func_000A14E8(buf, name + 3, 2, D_00170AB4, 350, 9);
    buf[2] = 0;
    object->image = atoi(buf);
    func_000A14E8(buf, name, 8, D_00170AB4, 354, 9);
    buf[8] = 0;
}
