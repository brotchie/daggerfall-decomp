/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001B69D */
struct rec { char pad[33]; short id; char pad2[57]; };     /* 92 bytes */
struct cmd { unsigned short hash; void (*fn)(struct rec *, char **, int *, int *); };
extern char *D_00147954;
extern char D_0017043C[];
extern char D_00170448[];
extern char D_00170464[];
extern char D_0017046E[];
extern unsigned char D_00178630[];
extern struct cmd D_00179D9C[];
extern char D_001903A4[];
extern int D_00196710;
extern struct rec *D_0019672C;
extern unsigned char D_00196732;
extern void func_0001BAB4(struct rec *);
extern void func_0001BBE2(struct rec *, int, struct rec *);
extern void func_00050069(char *);
extern int func_0006CD6E(char *);
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

void func_0001B69D(void)
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
    fh = func_0006CD6E(D_0017043C);
    if (fh < 1)
        func_00050069(D_00170448);
    len = func_000A00CB(fh, D_00147954, 90000);
    func_0009DEA7(fh);
    for (D_00196710 = i = 0; i < len; i++)
        if (D_00147954[i] == '#')
            D_00196710++;
    if (D_0019672C != 0) {
        if (D_0019672C != 0 && D_0019672C != (struct rec *)0x97979797) {
            func_000A0024(D_0019672C, D_00170464, 973);
            D_0019672C = (struct rec *)0x97979797;
        }
    }
    cur = D_0019672C = func_000A00AF(D_00196710 * 92, D_00170464, 975);
    func_000A0040(cur, 0, D_00196710 * 92, D_00170464, 976, 4);
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
                func_0001BBE2(&rec, prev, cur++);
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
                if (D_00179D9C[i].hash != hash) continue;
                found = 1;
                while (*p <= ' ' || *p == ':')
                    p++;
                D_00179D9C[i].fn(&rec, &p, &l_30, &l_34);
                break;
            }
            if (!found) {
                func_000A0ED9(1049, D_00170464);
                func_000A0F5C(D_001903A4, D_0017046E, line);
                func_00050069(D_001903A4);
            }
            break;
        }
    }
    if (have)
        func_0001BBE2(&rec, prev, cur);
    func_0001BAB4(D_0019672C);
}
