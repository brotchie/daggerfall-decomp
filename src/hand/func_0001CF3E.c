/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001CF3E */
#include "records.h"

extern char D_00170464[];        /* __FILE__ */
extern unsigned char scratch_190d16;
extern unsigned char scratch_190d17;
extern signed char text_rsc_buffer[];
extern int D_00195B84;
extern int game_minutes;
extern unsigned char D_00196269;
extern unsigned char D_001962A5;
extern int rumor_file;
extern int D_00196708;
extern struct faction *D_0019670C;
extern struct faction *D_0019671C;
extern int faction_player_related(struct faction *);
extern unsigned char func_0001D66C(struct rumor *);
extern void parse_rsc_text(int, int, int);
extern int rand_range(int, int);
extern void mc_memset(char *, int, int, char *, int, int);
extern int write(int, void *, int);
extern int strlen(char *);

void rumor_add_faction(struct faction *a1, struct faction *a2, int a3, unsigned char a4, int a5)
{
    int unused;
    struct rumor m;
    int saved;

    saved = D_00195B84;
    if (D_001962A5 == 0)
        return;
    if (a5 != 455 && a5 != 456)
        if (a3 == 100 && faction_player_related(a1) == 0 && faction_player_related(a2) == 0) {
            D_00195B84 = saved;
            return;
        }
    D_0019671C = a1;
    D_0019670C = a2;
    scratch_190d16 = a3;
    scratch_190d17 = a4;
    D_00196708++;
    if (a4 != 0)
        D_00196269 = a4;
    else if (a1 != 0 && a1->region != 255)
        D_00196269 = a1->region;
    else if (a2 != 0 && a2->region != 255)
        D_00196269 = a2->region;
    else
        D_00196269 = rand_range(0, 61);
    parse_rsc_text(a5, 0, 0);
    if (a1 != 0)
        m.faction1 = a1->id;
    else
        m.faction1 = 0;
    if (a2 != 0)
        m.faction2 = a2->id;
    else
        m.faction2 = 0;
    m.kind = a3;
    m.region = a4;
    m.flags = func_0001D66C(&m);
    m.quest_id = 0;
    mc_memset(m.quest_name, 0, 9, D_00170464, 1606, 9);
    m.message = 0;
    m.target = 0;
    m.text_length = strlen(((char *)text_rsc_buffer)) + 1;
    m.expires = game_minutes + 43140;
    write(rumor_file, &m, 34);
    write(rumor_file, ((char *)text_rsc_buffer), m.text_length);
    D_00195B84 = saved;
}
