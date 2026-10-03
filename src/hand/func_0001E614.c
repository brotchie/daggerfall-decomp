/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E614 */
struct rec13 { char pad[4]; char c4, c5, c6, c7; char pad2[5]; };
struct bits { unsigned char lo:4; unsigned char b4:1; unsigned char b5:2; };
extern char D_001704CC[];
extern char D_00170530[];
extern char D_00179E10[];
extern struct rec13 D_0018437F[];
extern char D_001903A4[];
extern char D_001968BA;
extern unsigned char D_001968BD[];
extern unsigned char D_001968FD[];
extern struct bits D_0019693D[];
extern int D_00196A2C;
extern int D_00196AA0;
extern char D_001A5B38[];
extern char D_001A5C26;
extern int func_00012FCE(int, char *, int);
extern void func_00013260(int, int, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern char *func_000A0DD9(int, char *, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern void func_000A0F5C(char *, char *, ...);

void func_0001E614(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    char l_18[4];

    l_24 = D_001968BD[a1];
    if (D_0019693D[a1].b4)
        D_0018437F[l_24].c4 = D_001968BA;
    else
        D_0018437F[l_24].c4 = 'A';
    D_0018437F[l_24].c5 = D_00179E10[D_0019693D[a1].b5];
    func_000A0DD9(D_001968FD[a1], l_18, 10);
    if (l_24 == 13 || l_24 == 14) {
        D_0018437F[l_24].c6 = D_0019693D[a1].lo + 'A';
        D_0018437F[l_24].c7 = l_18[0];
    } else if (D_001968FD[a1] < 10) {
        D_0018437F[l_24].c6 = '0';
        D_0018437F[l_24].c7 = l_18[0];
    } else {
        D_0018437F[l_24].c6 = l_18[0];
        D_0018437F[l_24].c7 = l_18[1];
    }
    if (D_001A5C26 == 0) {
        func_000A0ED9(476, D_001704CC);
        func_000A0F5C(D_001903A4, D_00170530, &D_0018437F[l_24]);
    } else {
        func_000A0AD9(D_001903A4, D_001A5B38, 160, D_001704CC, 478);
    }
    l_1C = func_00012FCE(D_00196AA0, D_001903A4, 8);
    func_00013260(D_00196AA0, l_1C, D_00196A2C);
}
