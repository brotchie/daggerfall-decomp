/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039E5F */
#pragma pack(1)
struct Ent {
    char pad[47];
    char name[26];              /* 47 */
    char key;                   /* 73 */
    char pad2[15];
};
extern char D_00170B13[];
extern char D_00170B69[];
extern char *D_00195C44;
extern void disk_read_file(char *, char *);
extern void mc_strncpy(char *, char *, int, char *, int);
extern int func_000A0DF4(char *);
extern char *memchr(char *, int, int);

char **spells_std_names_for_ids(char *keys)
{
    char **list;
    char *str;
    struct Ent *tbl;
    short i;
    int j;
    short n;
    short cnt;

    n = memchr(keys, 255, 1000) - keys;
    list = (char **)(D_00195C44 + 20000);
    str = D_00195C44 + 21000;
    tbl = (struct Ent *)D_00195C44;
    disk_read_file(D_00170B69, D_00195C44);
    for (cnt = i = 0; i < n; i++) {
        j = 0;
        while (j < 128) {
            if (tbl[j].name[0] != 0 && tbl[j].key == keys[i])
                break;
            j++;
        }
        mc_strncpy(str, tbl[j].name, 4, D_00170B13, 1625);
        list[cnt++] = str;
        str += func_000A0DF4(str) + 1;
    }
    list[cnt] = 0;
    if (cnt == 0)
        return 0;
    return list;
}
