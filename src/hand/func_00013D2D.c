/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00013D2D */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
extern char D_001702CC[];        /* __FILE__ */
extern char *D_00178E54;
extern struct bits8 D_001940DA;
extern int D_00196470;
extern char *D_00196474;
extern void func_000A0024(void *, char *, int);

void func_00013D2D(char *a1)
{
    if (D_00178E54 != 0 && D_00178E54 != (char *)0x97979797) {
        func_000A0024(D_00178E54, D_001702CC, 52);
        D_00178E54 = (char *)0x97979797;
    }
    D_00178E54 = D_001940DA.b3 ? a1 : 0;
    D_00196474 = a1;
    D_00196470 = 320;
    D_001940DA.b3 = 0;
}
