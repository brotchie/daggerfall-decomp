/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00038228 */
extern char D_00170B13[];
extern char D_00178A0A[];
extern char D_0017A94B[];
extern char D_0017AE13[];
extern char D_0017AF6B[];
extern char D_00182752[];
extern char D_00195B7C[];
extern char D_00195C44[];
extern char D_00195F30[];
extern char D_0019961C[];
extern char D_00199628[];
extern char D_0019962E[];
extern int func_00037AB7(void);
extern short func_00037D5A(void);
extern void func_0003817F(int);
extern short func_0003853C(short);
extern void func_0007D24F(char *);
extern int func_000A0AD9();
extern int func_000A0DF4();
extern int func_000A1023();

void func_00038228(short a1)
{
    short j;

    *(char *)(*(char **)D_00178A0A + (*(short *)D_00195F30 = func_0003853C(255)) * 2) = a1;
    if (func_00037AB7() == 0)
        *(char *)(*(char **)D_00178A0A + 6) = 4;
    j = func_00037D5A();
    if (j != 2) {
        if (j == 0 && *(char *)(*(char **)D_00178A0A + 7) != 0)
            *(char *)(*(char **)D_00178A0A + 7) = 0;
        if (j == 1 && *(char *)(*(char **)D_00178A0A + 7) == 0)
            (*(char *)(*(char **)D_00178A0A + 7))++;
    }
    if (*(int *)(D_00182752 + a1 * 48) == 0) {
        func_000A1023(D_0019961C, D_0017AE13 + (*(unsigned char *)(D_0017AF6B + a1 * 12) << 3), 8, D_00170B13, 1058, 8);
        *(short *)D_00199628 = 0;
        *(char *)D_0019962E = *(char *)(D_0017A94B + a1 * 12);
    } else {
        char *s;

        s = *(char **)D_00195C44;
        *s = 0;
        j = 0;
        while (*(int *)(D_00182752 + a1 * 48 + j * 4) != 0) {
            *(*(char **)D_00195C44 + j + 32000) = j;
            func_000A0AD9(s, *(int *)(D_00182752 + a1 * 48 + j++ * 4), 4, D_00170B13, 1071);
            s = s + func_000A0DF4(s) + 1;
        }
        *s = 0;
        *(int *)D_00195B7C = (int)func_0003817F;
        func_0007D24F(*(char **)D_00195C44);
    }
}
