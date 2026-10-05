/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00071606 */
struct mob {
    char pad0[0x28];
    short f28;
    char pad2a[0x43 - 0x2a];
    unsigned char f43;
    char pad44[0x7c - 0x44];
    short hp;               /* 0x7c */
    short maxhp;            /* 0x7e */
    char pad80[0x8d - 0x80];
    short f8d;
    short f8f;
};
struct ext { char pad[4]; unsigned short f4; unsigned char f6; };
struct pc {
    char pad0[0x30];
    short f30;
    char pad32[6];
    short f38;
    char pad3a[0x9d - 0x3a];
    short f9d;
    char pad9f[0x1fd - 0x9f];
    int f1fd;
};
extern struct pc *player_character;
extern int game_minutes;
extern void skill_add_uses(int, int);
extern int player_in_daylight(void);
extern void fatigue_add(int);

void rest_recover(char *a1)
{
    struct mob *m;
    struct ext *e;
    int v;

    m = (struct mob *)(a1 + 71);
    e = (struct ext *)((char *)m + 560);
    if (m->f43 == 8 && (unsigned)(game_minutes - player_character->f1fd) > 960)
        return;
    v = 60;
    if (e->f6 != 0) {
        if (e->f6 & 4)
            v += 40;
        else if ((e->f6 & 1) && player_in_daylight() != 0)
            v += 40;
        else if ((e->f6 & 2) && player_in_daylight() == 0)
            v += 40;
    }
    v += player_character->f9d;
    skill_add_uses(0, 1);
    v = m->maxhp * v / 1000 + (m->f28 - 50) / 10;
    if (v < 1)
        v = 1;
    m->hp += v;
    if (m->hp > m->maxhp)
        m->hp = m->maxhp;
    fatigue_add((player_character->f30 + player_character->f38) << 6 >> 3);
    if (!(e->f4 & 8) && m->f8d < m->f8f) {
        m->f8d += m->f8f >> 3;
        if (m->f8d > m->f8f)
            m->f8d = m->f8f;
    }
}
