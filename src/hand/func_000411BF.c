/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000411BF */
struct mob {
    char pad0[27];
    unsigned short f27;         /* 0x1b */
    char pad1d[2];
    unsigned int f31;           /* 0x1f */
};
struct snd { char pad0[4]; unsigned short len; };
struct pc { char pad0[64]; unsigned short flags; };
extern char D_00170DC9[];
extern unsigned char D_0017B667[];
extern short D_0017B66D[];
extern short D_0017B69D[];
extern struct pc *D_00195A84;
extern int climate_category(void);
extern struct snd *flats_cfg_find(unsigned short);
extern int disk_open_data(char *);
extern int func_0009DC25(void);
extern void func_0009DC49(int);
extern void func_0009DEA7(int);
extern void func_000A006E(int, int, int);
extern void func_000A00CB(int, char *, int);
extern short *func_000CE45E(short *, short, int);

void person_load_face(struct mob *a1, char *a2)
{
    short *p;
    struct snd *q;
    int v;
    int h;
    short seed;

    seed = func_0009DC25();
    func_0009DC49(a1->f31 | (a1->f31 >> 16));
    p = func_000CE45E(D_0017B66D, a1->f27 >> 7, 24);
    if (p == 0) {
        q = flats_cfg_find(a1->f27);
        if (q != 0 && q->len != 0)
            v = q->len << 12;
        else
            v = (D_0017B69D[D_0017B667[climate_category()] * 8 + (func_0009DC25() & 3) + ((D_00195A84->flags & 1) != 0 ? 4 : 0)] + func_0009DC25() % 10) << 12;
    } else {
        v = (D_0017B69D[((int)p - (int)D_0017B66D) / 2] + func_0009DC25() % 24) << 12;
    }
    h = disk_open_data(D_00170DC9);
    func_000A006E(h, v, 0);
    func_000A00CB(h, a2, 4096);
    func_0009DEA7(h);
    func_0009DC49(seed);
}
