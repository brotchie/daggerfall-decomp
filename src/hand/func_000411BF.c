/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000411BF */
#include "records.h"

extern char D_00170DC9[];
extern unsigned char D_0017B667[];
extern short D_0017B66D[];
extern short D_0017B69D[];
extern struct character *text_macro_npc;
extern int climate_category(void);
extern struct flat_cfg *flats_cfg_find(int);
extern int disk_open_data(char *);
extern int rand(void);
extern void srand(int);
extern void close(int);
extern void lseek(int, int, int);
extern void read(int, char *, int);
extern short *xn_str_find_u16(short *, short, int);

void npc_load_face(struct record *npc, char *face)
{
    short *found_sprite;
    struct flat_cfg *flat;
    int offset;
    int file;
    short saved_seed;

    saved_seed = rand();
    srand(npc->id | (npc->id >> 16));
    found_sprite = xn_str_find_u16(D_0017B66D, npc->image >> 7, 24);
    if (found_sprite == 0) {
        flat = flats_cfg_find(npc->image);
        if (flat != 0 && flat->face != 0)
            offset = flat->face << 12;
        else
            offset = (D_0017B69D[D_0017B667[climate_category()] * 8 + (rand() & 3) + ((text_macro_npc->flags & 1) != 0 ? 4 : 0)] + rand() % 10) << 12;
    } else {
        offset = (D_0017B69D[((int)found_sprite - (int)D_0017B66D) / 2] + rand() % 24) << 12;
    }
    file = disk_open_data(D_00170DC9);
    lseek(file, offset, 0);
    read(file, face, 4096);
    close(file);
    srand(saved_seed);
}
