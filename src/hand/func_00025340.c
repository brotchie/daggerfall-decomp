/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00025340 */
#include "records.h"

extern char D_00170738[];        /* __FILE__ */
extern char D_00170765[];
extern signed char text_buffer[];
extern struct career *player_class;
extern struct career *scratch_buffer;     /* scratch_buffer: the 18 classes CLASS00-17.CFG */
extern int career_slot_weight(int);
extern iptr disk_read_file(char *, struct career *);
extern int abs(int);
extern void mc_memset(void *, int, int, char *, int, int);
extern char *memchr(char *, int, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

int career_nearest_class(void)
{
    int scores[18];
    struct career *classes;
    int j;
    int k;
    int i;
    int weight;

    classes = scratch_buffer;
    mc_memset(scores, 0, 72, D_00170738, 397, 72);
    for (i = 0; i < 18; i++) {
        mc_set_location(401, D_00170738);
        mc_sprintf(((char *)text_buffer), D_00170765, i);
        disk_read_file(((char *)text_buffer), &classes[i]);
    }
    for (i = 0; i < 12; i++) {
        weight = career_slot_weight(i);
        for (j = 0; j < 18; j++) {
            k = (int)(memchr((char *)classes[j].skills, player_class->skills[i], 12) - (char *)classes[j].skills);
            if (k >= 0) {
                if (career_slot_weight(k) == weight)
                    scores[j] += weight;
                else
                    scores[j] += 3 - abs(career_slot_weight(k) - weight);
            }
        }
    }
    for (j = k = i = 0; i < 18; i++) {
        if (scores[i] > j) {
            j = scores[i];
            k = i;
        }
    }
    return k;
}
