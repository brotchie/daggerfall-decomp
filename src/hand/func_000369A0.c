/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000369A0 */
struct rec { char pad[27]; short f27; short f29; };
extern char D_00170AB4[];
extern int atoi(char *);
extern void func_000A14E8(char *, char *, int, char *, int, int);

void func_000369A0(struct rec *a1, char *a2)
{
    int x;
    char buf[12];

    func_000A14E8(buf, a2, 3, D_00170AB4, 347, 9);
    buf[3] = 0;
    a1->f29 = atoi(buf);
    func_000A14E8(buf, a2 + 3, 2, D_00170AB4, 350, 9);
    buf[2] = 0;
    a1->f27 = atoi(buf);
    func_000A14E8(buf, a2, 8, D_00170AB4, 354, 9);
    buf[8] = 0;
}
