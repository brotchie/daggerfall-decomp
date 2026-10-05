/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001B69D */
struct rec { char pad[33]; short id; char pad2[57]; };     /* 92 bytes */
struct cmd { unsigned short hash; void (*fn)(struct rec *, char **, int *, int *); };
extern char *D_00147954;
extern char D_0017043C[];
extern char D_00170448[];
extern char D_00170464[];
extern char D_0017046E[];
extern unsigned char D_00178630[];
extern struct cmd faction_keywords[];
extern char text_buffer[];
extern int faction_count;
extern struct rec *factions;
extern unsigned char D_00196732;
extern void faction_link_relations(struct rec *);
extern void faction_add_record(struct rec *, int, struct rec *);
extern void fatal_error(char *);
extern int disk_open_data(char *);
extern void func_0009DEA7(int);
extern void func_000A0024(struct rec *, char *, int);
extern void func_000A0040(void *, int, int, char *, int, int);
extern struct rec *func_000A00AF(int, char *, int);
extern int func_000A00CB(int, char *, int);
extern short func_000A0D13(char *);
extern int func_000A1097(int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern void func_000A0F5C(char *, char *, ...);

void faction_load_file(void)
{
    struct rec rec;
    int fh;
    int i;
    int found;
    int line;
    int len;
    int l_34;
    int l_30;
    int indent;
    int hash;
    int prev;
    char *p;
    int have;
    struct rec *cur;

    line = 1;
    have = 0;
    l_34 = 0;
    l_30 = 0;
    indent = 0;
    prev = 0;
    D_00196732 = 0;
    fh = disk_open_data(D_0017043C);
    if (fh < 1)
        fatal_error(D_00170448);
    len = func_000A00CB(fh, D_00147954, 90000);
    func_0009DEA7(fh);
    for (faction_count = i = 0; i < len; i++)
        if (D_00147954[i] == '#')
            faction_count++;
    if (factions != 0) {
        if (factions != 0 && factions != (struct rec *)0x97979797) {
            func_000A0024(factions, D_00170464, 973);
            factions = (struct rec *)0x97979797;
        }
    }
    cur = factions = func_000A00AF(faction_count * 92, D_00170464, 975);
    func_000A0040(cur, 0, faction_count * 92, D_00170464, 976, 4);
    func_000A0040(&rec, 0, 92, D_00170464, 977, 4);
    p = D_00147954;
    p[len] = 0;
    while (*p != 0) {
        switch (*p) {
        case 13:
        case ' ':
            p++;
            break;
        case 10:
            line++;
            p++;
            indent = 0;
            break;
        case 9:
            indent++;
            p++;
            break;
        case '#':
            if (have) {
                faction_add_record(&rec, prev, cur++);
                prev = indent;
                indent = 0;
                func_000A0040(&rec, 0, 92, D_00170464, 1012, 4);
                l_30 = l_34 = 0;
            }
            have = 1;
            p++;
            rec.id = func_000A0D13(p);
            while (D_00178630[(unsigned char)(*p + 1)] & 32)
                p++;
            break;
        case ';':
            while (*p != 13 && *p != 10)
                p++;
            break;
        case ':':
            p++;
            break;
        default:
            hash = 0;
            while (*p > ' ' && *p != ':') {
                hash <<= 1;
                hash += func_000A1097(*p++);
            }
            for (found = i = 0; i < 19; i++) {
                if (faction_keywords[i].hash != hash) continue;
                found = 1;
                while (*p <= ' ' || *p == ':')
                    p++;
                faction_keywords[i].fn(&rec, &p, &l_30, &l_34);
                break;
            }
            if (!found) {
                func_000A0ED9(1049, D_00170464);
                func_000A0F5C(text_buffer, D_0017046E, line);
                fatal_error(text_buffer);
            }
            break;
        }
    }
    if (have)
        faction_add_record(&rec, prev, cur);
    faction_link_relations(factions);
}
