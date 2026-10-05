/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D412 */
#pragma pack(1)
struct ent { short id; int off; };
#pragma pack()
extern char D_00170D55[];
extern char D_00170D5C[];
extern struct ent *D_00195C44;
extern int D_00195D6C;
extern char D_00196295;
extern char *text_expand_wrap(unsigned short, short, unsigned char *, char *, char *);
extern int rand(void);
extern int lseek(int, int, int);
extern char *mc_malloc(int, char *, int);
extern int func_000A00CB(int, void *, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int mc_sprintf(char *, ...);

char *text_rsc_load(short a1, unsigned short a2, short a3)
{
    short n;
    short i;
    short j;
    int l_34;
    unsigned char *l_30;
    char *l_2C;
    char *l_28;

    lseek(D_00195D6C, 0, 0);
    func_000A00CB(D_00195D6C, &n, 2);
    func_000A00CB(D_00195D6C, D_00195C44, n);
    n /= 6;
    for (i = 0; i < n; i++) {
        if (D_00195C44[i].id == a1)
            break;
    }
    if (i >= n) {
        if (D_00196295) {
            D_00196295 = 0;
            return 0;
        }
        l_30 = mc_malloc(1024, D_00170D55, 65);
        func_000A0ED9(66, D_00170D55);
        mc_sprintf(l_30, D_00170D5C, a1);
        return l_30;
    }
    l_34 = D_00195C44[i + 1].off - D_00195C44[i].off;
    lseek(D_00195D6C, D_00195C44[i].off, 0);
    l_30 = mc_malloc(l_34 + 16, D_00170D55, 73);
    l_2C = mc_malloc(l_34 < 4096 ? 8192 : l_34 * 2, D_00170D55, 74);
    l_28 = mc_malloc(l_34 < 4096 ? 8192 : l_34 * 2, D_00170D55, 75);
    func_000A00CB(D_00195D6C, l_30, l_34 + 8);
    j = 0;
    i = 1;
    while (l_30[j] != 254) {
        if (l_30[j] == 255 && l_30[j + 1] != 254)
            i++;
        j++;
    }
    if (i != 1)
        i = rand() % i;
    else
        i--;
    j = 0;
    while (i) {
        while (l_30[j++] != 255)
            ;
        i--;
    }
    while (l_30[j] < 254)
        l_30[i++] = l_30[j++];
    l_30[i] = 0;
    return text_expand_wrap(a2, a3, l_30, l_2C, l_28);
}
