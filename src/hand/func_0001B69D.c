/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001B69D */
#include "records.h"

struct cmd { unsigned short hash; void (*fn)(struct faction *, char **, int *, int *); };
extern char *D_00147954;
extern char D_0017043C[];
extern char D_00170448[];
extern char D_00170464[];
extern char D_0017046E[];
extern unsigned char D_00178630[];
extern struct cmd faction_keywords[];
extern signed char text_buffer[];
extern int faction_count;
extern struct faction *factions;
extern unsigned char D_00196732;
extern void faction_link_relations(struct faction *);
extern void faction_add_record(struct faction *, int, struct faction *);
extern void fatal_error(char *);
extern int disk_open_data(char *);
extern void close(int);
extern void mc_free(struct faction *, char *, int);
extern void mc_memset(void *, int, int, char *, int, int);
extern struct faction *mc_malloc(int, char *, int);
extern int read(int, char *, int);
extern short atoi(char *);
extern int tolower(int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern void mc_sprintf(char *, char *, ...);

void faction_load_file(void)
{
    struct faction rec;
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
    struct faction *cur;

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
    len = read(fh, D_00147954, 90000);
    close(fh);
    for (faction_count = i = 0; i < len; i++)
        if (D_00147954[i] == '#')
            faction_count++;
    if (factions != 0) {
        if (factions != 0 && factions != (struct faction *)0x97979797) {
            mc_free(factions, D_00170464, 973);
            factions = (struct faction *)0x97979797;
        }
    }
    cur = factions = mc_malloc(faction_count * 92, D_00170464, 975);
    mc_memset(cur, 0, faction_count * 92, D_00170464, 976, 4);
    mc_memset(&rec, 0, 92, D_00170464, 977, 4);
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
                mc_memset(&rec, 0, 92, D_00170464, 1012, 4);
                l_30 = l_34 = 0;
            }
            have = 1;
            p++;
            rec.id = atoi(p);
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
                hash += tolower(*p++);
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
                mc_set_location(1049, D_00170464);
                mc_sprintf(((char *)text_buffer), D_0017046E, line);
                fatal_error(((char *)text_buffer));
            }
            break;
        }
    }
    if (have)
        faction_add_record(&rec, prev, cur);
    faction_link_relations(factions);
}
