/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00057147 */
struct rec { char type, param, type2, param2, type3, param3; char pad[4]; };
extern struct rec D_00199868[];

void func_00057147(short slot, short type, short param, short type2, char param2, char type3, char param3)
{
    D_00199868[slot].type = type;
    D_00199868[slot].param = param;
    D_00199868[slot].type2 = type2;
    D_00199868[slot].param2 = param2;
    D_00199868[slot].type3 = type3;
    D_00199868[slot].param3 = param3;
}
