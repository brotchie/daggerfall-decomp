/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000576FF */
struct pair { unsigned char a, b; };
struct row { struct pair p[5]; };
struct cell { short v; short f2; };
extern short D_00190D64;
extern int D_00190EE4[];
extern char *D_00195C44;
extern struct row D_00199868[];
extern struct cell D_001998E0[];
extern int func_00058890(short, short);
extern void func_0007D3CA(int *);

void func_000576FF(int *a1, short a2)
{
    short i;
    unsigned short c;
    short cnt;
    short k;
    int sv;

    k = c = cnt = 0;
    while (*a1 != 0) {
        for (i = 0; i < 12; i++) {
            if (D_00199868[i].p[0].a == a2 && D_00199868[i].p[0].b == k
                || D_00199868[i].p[1].a == a2 && D_00199868[i].p[1].b == k
                || D_00199868[i].p[2].a == a2 && D_00199868[i].p[2].b == k
                || D_00199868[i].p[3].a == a2 && D_00199868[i].p[3].b == k
                || D_00199868[i].p[4].a == a2 && D_00199868[i].p[4].b == k)
                goto next;
        }
        sv = D_001998E0[D_00190D64].v;
        D_001998E0[D_00190D64].v = 100;
        if (func_00058890(a2, k) != 0)
            D_001998E0[D_00190D64].v = sv;
        else {
            D_001998E0[D_00190D64].v = sv;
            D_00195C44[cnt + 64000] = c;
            D_00190EE4[cnt] = *a1;
            cnt++;
        }
next:
        a1++;
        c++;
        k++;
    }
    D_00190EE4[cnt] = 0;
    func_0007D3CA(D_00190EE4);
}
