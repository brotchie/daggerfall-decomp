/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001B69D */
#include "records.h"
#include "clib.h"

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
#pragma aux mc_set_location parm routine [];

void faction_load_file(void)
{
    struct faction parsed;
    int file;
    int i;
    int found;
    int line;
    int len;
    int enemy_count;
    int ally_count;
    int indent;
    int hash;
    int prev;
    char *text;
    int have_record;
    struct faction *dest;

    line = 1;
    have_record = 0;
    enemy_count = 0;
    ally_count = 0;
    indent = 0;
    prev = 0;
    D_00196732 = 0;
    file = disk_open_data(D_0017043C);
    if (file < 1)
        fatal_error(D_00170448);
    len = read(file, D_00147954, 90000);
    close(file);
    for (faction_count = i = 0; i < len; i++)
        if (D_00147954[i] == '#')
            faction_count++;
    if (factions != 0) {
        if (factions != 0 && factions != (struct faction *)(iptr)-1751672937) {
            mc_free(factions, D_00170464, 973);
            factions = (struct faction *)(iptr)-1751672937;
        }
    }
    dest = factions = mc_malloc(faction_count * REC_SIZEOF(struct faction), D_00170464, 975);
    mc_memset(dest, 0, faction_count * REC_SIZEOF(struct faction), D_00170464, 976, 4);
    mc_memset(&parsed, 0, sizeof(parsed), D_00170464, 977, 4);
    text = D_00147954;
    text[len] = 0;
    while (*text != 0) {
        switch (*text) {
        case 13:
        case ' ':
            text++;
            break;
        case 10:
            line++;
            text++;
            indent = 0;
            break;
        case 9:
            indent++;
            text++;
            break;
        case '#':
            if (have_record) {
                faction_add_record(&parsed, prev, dest++);
                prev = indent;
                indent = 0;
                mc_memset(&parsed, 0, sizeof(parsed), D_00170464, 1012, 4);
                ally_count = enemy_count = 0;
            }
            have_record = 1;
            text++;
            parsed.id = atoi(text);
            while (D_00178630[(unsigned char)(*text + 1)] & 32)
                text++;
            break;
        case ';':
            while (*text != 13 && *text != 10)
                text++;
            break;
        case ':':
            text++;
            break;
        default:
            hash = 0;
            while (*text > ' ' && *text != ':') {
                hash <<= 1;
                hash += tolower(*text++);
            }
            for (found = i = 0; i < 19; i++) {
                if (faction_keywords[i].hash != hash) continue;
                found = 1;
                while (*text <= ' ' || *text == ':')
                    text++;
                faction_keywords[i].fn(&parsed, &text, &ally_count, &enemy_count);
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
    if (have_record)
        faction_add_record(&parsed, prev, dest);
    faction_link_relations(factions);
}
