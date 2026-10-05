/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00025340 */
#include "records.h"

extern char D_00170738[];        /* __FILE__ */
extern char D_00170765[];
extern signed char text_buffer[];
extern struct career *player_class;
extern struct career *D_00195C44;     /* scratch_buffer: the 18 classes CLASS00-17.CFG */
extern int career_slot_weight(int);
extern int disk_read_file(char *, struct career *);
extern int func_0009DEAC(int);
extern void mc_memset(void *, int, int, char *, int, int);
extern char *memchr(char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

int career_nearest_class(void)
{
    int sc[18];
    struct career *base;
    int j;
    int k;
    int i;
    int s;

    base = D_00195C44;
    mc_memset(sc, 0, 72, D_00170738, 397, 72);
    for (i = 0; i < 18; i++) {
        func_000A0ED9(401, D_00170738);
        mc_sprintf(((char *)text_buffer), D_00170765, i);
        disk_read_file(((char *)text_buffer), &base[i]);
    }
    for (i = 0; i < 12; i++) {
        s = career_slot_weight(i);
        for (j = 0; j < 18; j++) {
            k = memchr((char *)base[j].skills, player_class->skills[i], 12) - (char *)base[j].skills;
            if (k >= 0) {
                if (career_slot_weight(k) == s)
                    sc[j] += s;
                else
                    sc[j] += 3 - func_0009DEAC(career_slot_weight(k) - s);
            }
        }
    }
    for (j = k = i = 0; i < 18; i++) {
        if (sc[i] > j) {
            j = sc[i];
            k = i;
        }
    }
    return k;
}
