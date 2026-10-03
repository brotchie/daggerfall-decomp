/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00059909 */
extern unsigned char *D_00199B4C;
extern char **D_00199B50;
extern void func_000595DF(char *, int, int, int);

void func_00059909(int a1, int a2)
{
    int i;

    i = 0;
    while (D_00199B50[i] != 0) {
        func_000595DF(D_00199B50[i], a1, a2, D_00199B4C[i] + 64);
        i++;
    }
}
