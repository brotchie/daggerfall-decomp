/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004A0C0 */
#pragma pack(1)
struct rec { char pad0[24]; unsigned char kind; char pad19; };
#pragma pack()
extern int text_blank;
extern char *current_location;
extern int rand_range(int, int);
extern int building_name(struct rec *);

int parse_town_building_name(short a1)
{
    struct rec *p;
    short i;
    short c;

    p = *(struct rec **)(current_location + 43);
    for (c = i = 0; i < *(unsigned short *)(current_location + 41); i++, p++)
        if (p->kind == a1) c++;
    if (c == 0) return text_blank;
    if (c == 1)
        c = 0;
    else
        c = rand_range(0, c - 1) + 1;
    p = *(struct rec **)(current_location + 43);
    while (c != 0) {
        while (p->kind != a1) p++;
        c--;
    }
    return building_name(p);
}
