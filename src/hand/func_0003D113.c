/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D113 */
struct skill { short v; short f2; short f4; };
struct player {
    char pad0[157];
    struct skill skills[1];     /* 0x9d */
};
struct pc { char pad0[16]; unsigned char sk[12]; };
extern struct player *D_00195BE0;
extern struct pc *D_00195BEC;

int func_0003D113(void)
{
    int i;
    int sum;
    int max;
    int min;

    for (i = sum = 0; i < 6; i++)
        sum += D_00195BE0->skills[D_00195BEC->sk[i]].v;
    min = D_00195BE0->skills[D_00195BEC->sk[3]].v;
    if (D_00195BE0->skills[D_00195BEC->sk[4]].v < min)
        min = D_00195BE0->skills[D_00195BEC->sk[4]].v;
    if (D_00195BE0->skills[D_00195BEC->sk[5]].v < min)
        min = D_00195BE0->skills[D_00195BEC->sk[5]].v;
    sum -= min;
    max = D_00195BE0->skills[D_00195BEC->sk[6]].v;
    for (i = 7; i < 12; i++)
        if (D_00195BE0->skills[D_00195BEC->sk[i]].v > max)
            max = D_00195BE0->skills[D_00195BEC->sk[i]].v;
    sum += max;
    return sum;
}
