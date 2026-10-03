/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00040C87 */
#pragma pack(1)
struct Obj { char pad0[3]; short angle; char pad1[2]; int x; int y; int z; };
extern int D_0012AA04;
extern unsigned char D_001789FA;
extern int D_00178A14;
extern struct Obj *D_00195AA4;
extern int D_00195B14;
extern unsigned char D_0019627F;
extern struct Obj *D_00199670[];
extern int D_001996F4;
extern void func_000401F1(struct Obj *);
extern int func_00040B83(struct Obj *);
extern void func_00040E9D(struct Obj *);
extern int func_00062EF7(int, int, int *);
extern int func_0007D6AE(int, int);
extern int func_0009DC25(void);
extern int func_000C808D();

void func_00040C87(int a1)
{
    int i;
    int d;
    int a;
    int cnt;
    int tmp;

    if (D_001789FA == 3)
        return;
    if (D_00195B14 > 10)
        return;
    if (a1 != 0) {
        for (cnt = i = 0; i < D_001996F4; i++) {
            if (D_00199670[i] == 0)
                continue;
            if (func_00040B83(D_00199670[i]) == 0) {
                d = func_000C808D(D_00195AA4->x, D_00195AA4->z, D_00199670[i]->x, D_00199670[i]->z);
                a = func_00062EF7(D_00195AA4->angle, d, &tmp);
                if (a < 600)
                    continue;
            }
            if ((unsigned char)(func_0009DC25() & 3) == 0 || func_00040B83(D_00199670[i]) != 0) {
                cnt++;
                func_00040E9D(D_00199670[i]);
                func_000401F1(D_00199670[i]);
            }
        }
        if (cnt == 0) {
            cnt = func_0007D6AE(2, 5);
            for (i = 0; i < cnt; i++)
                func_00040E9D(0);
        }
        return;
    }
    if ((int)(unsigned char)(D_0019627F & 2) != 0) {
        for (i = 0; i < D_001996F4; i++) {
            if (D_00199670[i] == 0)
                continue;
            if (func_00040B83(D_00199670[i]) != 0) {
                func_00040E9D(D_00199670[i]);
                func_000401F1(D_00199670[i]);
            }
        }
        return;
    }
    if ((int)(unsigned char)(D_0019627F & 1) != 0)
        D_00178A14 = func_0007D6AE(5, 10) * D_0012AA04;
}
