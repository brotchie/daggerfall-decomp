/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00091247 */
struct rect { short x, y, w, h; };
struct img { struct rect r; char pad[4]; char data[1]; };
struct region { short x1, y1, x2, y2; void (*fn)(void); };   /* mouse hit box */
extern unsigned char D_0012B508;
extern short D_0012DA44;
extern char D_00176FC3[];
extern char D_00176FC8[];
extern char D_00176FCD[];
extern char D_00176FD2[];
extern char D_00176FD7[];
extern char D_00176FDC[];
extern struct region D_0018801C[];
extern short D_0018810C;
extern short D_00188110;
extern char D_001903A4[];
extern char D_00190B44[];
extern short D_00190D64;
extern short D_00190D6A;
extern struct img *D_00195B5C;
extern char *D_00195BE0;
extern char *D_00195BEC;
extern unsigned char D_001AA41A;
extern void func_0004633F(char *, char *);
extern void func_00050344(char *, char *);
extern void func_0007CA1F(char *, int, int, int, unsigned char);
extern void func_0007CA85(char *, int, int, int, unsigned char);
extern char *func_000A0DD9(int, char *, int);
extern void func_0012DB50(int);
extern void func_00144FB4(int, int, int, int, char *);

void func_00091247(void)
{
    int i;
    short x;

    func_00144FB4(44, D_00190D6A, (unsigned short)D_00195B5C->r.w, (unsigned short)D_00195B5C->r.h, D_00195B5C->data);
    D_0012B508 = 146;
    x = (D_0018810C + D_00188110) >> 1;
    func_0012DB50(4);
    for (i = 0; i < 8; i++) {
        func_0007CA85(func_000A0DD9(((short *)(D_00195BE0 + 32))[i], D_001903A4, 10), x, (short)(D_0018801C[i + 20].y2 - D_0012DA44 + 1), 145, 141);
    }
    func_0007CA85(func_000A0DD9(D_00190D64, D_001903A4, 10), 51, (short)(D_00190D6A + 13 - D_0012DA44 + 1), 145, 141);
    func_00050344(D_00195BE0, D_00195BEC);
    if (D_001AA41A == 255)
        return;
    func_0004633F(D_00176FC3, D_00190B44);
    func_0007CA1F(D_00190B44, 83, 22, 145, 141);
    func_0004633F(D_00176FC8, D_00190B44);
    func_0007CA1F(D_00190B44, 103, 32, 145, 141);
    func_0004633F(D_00176FCD, D_00190B44);
    func_0007CA1F(D_00190B44, 112, 49, 145, 141);
    func_0004633F(D_00176FD2, D_00190B44);
    func_0007CA1F(D_00190B44, 121, 71, 145, 141);
    func_0004633F(D_00176FD7, D_00190B44);
    func_0007CA1F(D_00190B44, 97, 93, 145, 141);
    func_0004633F(D_00176FDC, D_00190B44);
    func_0007CA1F(D_00190B44, 101, 110, 145, 141);
    func_0007CA1F(D_00190B44, 122, 120, 145, 141);
}
