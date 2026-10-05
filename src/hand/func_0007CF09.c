/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CF09 */
extern signed char mouse_buttons;
extern int func_000CD308();
extern int func_000CD31A();
extern int func_0012B136();
extern char func_001427A8();

int func_0007CF09(short a1)
{
    short l_18;
    while (a1--) {
        func_0012B136();
        if (*((char *)&mouse_buttons) != 0) return 1;
        if (func_001427A8() != 0) return 1;
        func_000CD308();
        func_000CD31A();
    }
    return 0;
}
