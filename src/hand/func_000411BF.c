/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000411BF */
#include "records.h"

struct snd { char pad0[4]; unsigned short len; };
extern char D_00170DC9[];
extern unsigned char D_0017B667[];
extern short D_0017B66D[];
extern short D_0017B69D[];
extern struct character *text_macro_npc;
extern int climate_category(void);
extern struct snd *flats_cfg_find(unsigned short);
extern int disk_open_data(char *);
extern int rand(void);
extern void srand(int);
extern void close(int);
extern void lseek(int, int, int);
extern void read(int, char *, int);
extern short *xn_str_find_u16(short *, short, int);

void npc_load_face(struct record *a1, char *a2)
{
    short *p;
    struct snd *q;
    int v;
    int h;
    short seed;

    seed = rand();
    srand(a1->id | (a1->id >> 16));
    p = xn_str_find_u16(D_0017B66D, a1->image >> 7, 24);
    if (p == 0) {
        q = flats_cfg_find(a1->image);
        if (q != 0 && q->len != 0)
            v = q->len << 12;
        else
            v = (D_0017B69D[D_0017B667[climate_category()] * 8 + (rand() & 3) + ((text_macro_npc->flags & 1) != 0 ? 4 : 0)] + rand() % 10) << 12;
    } else {
        v = (D_0017B69D[((int)p - (int)D_0017B66D) / 2] + rand() % 24) << 12;
    }
    h = disk_open_data(D_00170DC9);
    lseek(h, v, 0);
    read(h, a2, 4096);
    close(h);
    srand(seed);
}
