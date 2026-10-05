/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00025340 */
struct rec { char pad0[16]; char name[58]; };      /* 74 bytes */
struct pc { char pad0[16]; unsigned char f16[12]; };
extern char D_00170738[];        /* __FILE__ */
extern char D_00170765[];
extern char text_buffer[];
extern struct pc *player_class;
extern struct rec *D_00195C44;
extern int career_slot_weight(int);
extern int disk_read_file(char *, struct rec *);
extern int func_0009DEAC(int);
extern void mc_memset(void *, int, int, char *, int, int);
extern char *memchr(char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

int career_nearest_class(void)
{
    int sc[18];
    struct rec *base;
    int j;
    int k;
    int i;
    int s;

    base = D_00195C44;
    mc_memset(sc, 0, 72, D_00170738, 397, 72);
    for (i = 0; i < 18; i++) {
        func_000A0ED9(401, D_00170738);
        mc_sprintf(text_buffer, D_00170765, i);
        disk_read_file(text_buffer, &base[i]);
    }
    for (i = 0; i < 12; i++) {
        s = career_slot_weight(i);
        for (j = 0; j < 18; j++) {
            k = memchr(base[j].name, player_class->f16[i], 12) - base[j].name;
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
