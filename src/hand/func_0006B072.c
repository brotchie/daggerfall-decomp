/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006B072 */
struct slot { short v[10]; };
struct tab {
    short id[32];
    struct slot s[32];
};
extern char *logbook_object;

void logbook_remove_entry(unsigned char a1, int a2)
{
    struct tab *p;
    int i;
    int found;

    p = (struct tab *)(logbook_object + 71);
    found = -1;
    for (i = 0; i < 32; i++) {
        if (a1 == p->id[i]) {
            found = i;
            break;
        }
    }
    if (found == -1)
        return;
    p->s[found].v[a2] = 0;
}
