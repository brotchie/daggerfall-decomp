/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E854 */
extern char D_00170533[];
extern char D_001967EC[];
extern char D_001967F0[];
extern char D_001967F4[];
extern char D_001967F8[];
extern char D_001967FC[];
extern char D_00196804[];
extern char D_00196A2C[];
extern char D_00196AA4[];
extern char D_00196AA8[];
extern int func_000A0E3B();
extern int func_0014B45B();

#pragma pack(1)
struct E { int f0; int f4; int f8; int fc; int f10; };
struct H { char pad[3]; struct E e[317]; char pad2[17]; char names[10][13]; };
#pragma pack()
#define HP (*(struct H **)D_00196A2C)

void func_0001E854(int a1, int a2)
{
    int l_18;
    int l_14;

    *(int *)D_00196804 = HP->e[a2].f0;
    *(int *)D_001967EC = HP->e[a2].f4;
    *(int *)D_001967F8 = *(int *)D_00196AA4 + HP->e[a2].f8;
    *(int *)D_001967F0 = *(int *)D_00196AA8 - HP->e[a2].fc;
    *(int *)D_001967F4 = func_0014B45B(*(int *)D_001967F8, *(int *)D_001967F0) - 6;
    *(int *)D_001967FC = HP->e[a2].f10;
    if (func_000A0E3B(HP->names[a2], D_00170533) != 0) return;
    *(signed char *)((char *)a1 + 24) = 11;
    *(short *)((char *)a1 + 18) = 414;
}
