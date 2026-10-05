/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E854 */
#include "records.h"
extern char D_00170533[];
extern int D_001967EC;
extern int D_001967F0;
extern int D_001967F4;
extern int D_001967F8;
extern char D_001967FC[];
extern int D_00196804;
extern char rmb_block[];
extern int block_origin_x;
extern int block_origin_z;
extern int stricmp();
extern int func_0014B45B();

#pragma pack(1)
struct E { int f0; int f4; int f8; int fc; int f10; };
struct H { char pad[3]; struct E e[317]; char pad2[17]; char names[10][13]; };
#pragma pack()
#define HP (*(struct H **)rmb_block)

void func_0001E854(struct building *a1, int a2)
{
    int l_18;
    int l_14;

    D_00196804 = HP->e[a2].f0;
    D_001967EC = HP->e[a2].f4;
    D_001967F8 = block_origin_x + HP->e[a2].f8;
    D_001967F0 = block_origin_z - HP->e[a2].fc;
    D_001967F4 = func_0014B45B(D_001967F8, D_001967F0) - 6;
    *(int *)D_001967FC = HP->e[a2].f10;
    if (stricmp(HP->names[a2], D_00170533) != 0) return;
    a1->type = 11;
    a1->faction_id = 414;
}
