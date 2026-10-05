/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000699D8 */
struct msg {
    unsigned char type;
    char pad1[6];
    int a;          /* 7 */
    int b;          /* 11 */
    int c;          /* 15 */
    char pad13[53];
};
struct slot { int used; char pad[264]; };
extern int sound_last_size;
extern struct slot D_001A3BE4[];
extern char sound_enabled;
extern int sound_play_sample(int, int, struct msg *, int);
extern int sound_cache_load(int);

int sound_play_at_point(int a1, int a2, int a3, int a4, int a5)
{
    int h;
    struct msg m;
    int n;

    if (sound_enabled == 0)
        return -1;
    m.type = 0;
    m.a = a2;
    m.b = a3;
    m.c = a4;
    h = sound_cache_load(a1);
    n = sound_play_sample(h, sound_last_size, &m, a5);
    if (n > -1)
        D_001A3BE4[n].used = 0;
    return n;
}
