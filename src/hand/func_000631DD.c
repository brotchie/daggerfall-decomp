/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000631DD */
struct mob { char pad[137]; int flags; };
extern struct mob *D_00195BE0;
extern int func_000631AA(int);
extern int func_0007D6AE(int, int);

int func_000631DD(int a1)
{
    int chance;

    if ((D_00195BE0->flags & 0x3004) == 0)
        return 1;
    if (D_00195BE0->flags & 4)
        return func_000631AA(a1);
    if (D_00195BE0->flags & 0x2000)
        chance = 8;
    else
        chance = 4;
    if (func_000631AA(a1))
        return 1;
    return func_0007D6AE(1, 100) <= chance;
}
