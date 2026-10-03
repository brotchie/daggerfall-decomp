/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009190C */
struct pc { char pad[0x20]; short stats[8]; };
extern unsigned D_00190BE4;
extern signed char D_00190CE4[];
extern short D_00190D64;
extern short D_00190D70;
extern struct pc *D_00195BE0;

void func_0009190C(int a1)
{
    if (*(unsigned *)0x46c - D_00190BE4 < 6)
        return;
    D_00190BE4 = *(unsigned *)0x46c;
    if (a1 == 30) {
        if (D_00190D64 != 0) {
            if (D_00195BE0->stats[D_00190D70] == 100)
                return;
            D_00190D64--;
            D_00195BE0->stats[D_00190D70]++;
        }
        return;
    }
    if (D_00195BE0->stats[D_00190D70] <= D_00190CE4[D_00190D70])
        return;
    if (D_00195BE0->stats[D_00190D70] == 10)
        return;
    D_00190D64++;
    D_00195BE0->stats[D_00190D70]--;
}
