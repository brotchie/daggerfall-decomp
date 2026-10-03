/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003EEE0 */
struct save { char hdr[6]; char name[54]; };
extern char D_0012AC00;
extern char *D_00143550;
extern char D_00170D55[];        /* __FILE__ */
extern char D_00196270;
extern char D_00196271;
extern char D_00196272;
extern char D_00196274;
extern char D_00196279;
extern char D_00199654[];
extern char *D_0019965C;
extern int func_0003EAB4(struct save *, short, char *, int);
extern void func_0003F25E(void);
extern void func_0007D0DA(void);
extern void func_000A0040(void *, int, int, char *, int, int);
extern void *func_000A00AF(int, char *, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern void func_0012B136(void);
extern void func_0012DB50(int);

void func_0003EEE0(char *a1, short a2, short a3)
{
    struct save s;
    int rc;

    if (D_00196270 != 0)
        return;
    D_0019965C = func_000A00AF(64000, D_00170D55, 765);
    func_000A1023(D_0019965C, D_00143550, 64000, D_00170D55, 766, 4);
    func_000A0040(&s, 0, 60, D_00170D55, 768, 4);
    func_000A0AD9(s.name, a1, 9, D_00170D55, 769);
    func_0012DB50(4);
    if (a3 == 5) {
        D_00196271 = 0;
        rc = func_0003EAB4(&s, a2, D_00199654, 4);
    } else
        rc = func_0003EAB4(&s, a2, D_00199654, 0);
    if (rc == 0)
        return;
    D_00196270 = a3;
    func_0007D0DA();
    D_00196274 = 8;
    D_00196272 = 1;
    D_00196279 = D_0012AC00;
    func_0012B136();
    func_0003F25E();
}
