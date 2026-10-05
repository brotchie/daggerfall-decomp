/* main.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001700C0[];
extern char D_001700CC[];
extern int D_0018DC34;
extern char D_0018DC38[];
extern int D_0018DC3C;
extern int D_0018DC7C;
extern int D_0018DD3C;
extern int D_0018DD40;
extern int sos_drum_bank;
extern int sos_melodic_bank;
extern char D_0018DD4C[];
extern char D_0018DD50[];
extern int D_0018DD54;
extern int D_0018DD58;
extern int D_0018DD5C;
extern int D_0018DD60;
extern char D_0018DD64[];
extern signed char D_001A3F5E;

extern int sos_shutdown(void);
extern int sos_load_file(int, ...);
extern int dpmi_lock_region(int, int);
extern int func_0009E1A1();
extern int func_0009E2BB();
extern int func_0009E8FF();
extern int func_0009E95B();
extern int func_0009E9C2();
extern int func_0009EC0A();
extern int func_0009EC82();
extern int func_0009F4DE();
extern int func_0009F9A7();
extern int func_0009FEE5();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_init;

int sos_init(int a1, int a2)
{
    D_0018DC3C = 11025;
    D_0018DC7C = 4096;
    func_0009E1A1(65280, 0);
    func_0009E8FF(0, 0);
    func_0009E9C2(0, 0);
    if (a2 != (-1)) {
        *(int *)D_0018DD64 = a2;
        if (func_0009EC82((int)D_0018DD64, (int)&D_0018DD58) != 0) {
            func_0009F9A7(D_0018DD60, 1, 1);
            func_0009E95B();
            func_0009EC0A();
            return 2;
        }
    }
    if (a1 != (-1)) {
        D_0018DD3C = a1;
        if (func_0009F4DE((int)D_0018DC38, (int)&D_0018DD60) != 0) {
            func_0009E95B();
            return 1;
        }
    }
    if (a1 != (-1)) func_0009E2BB(90, D_0018DD40, (int)D_0018DD50);
    if (a2 == 40962 || (a2 == 40969 && a2 != (-1))) {
        D_001A3F5E = 1;
        if ((sos_melodic_bank = sos_load_file((int)D_001700C0)) == 0) {
            sos_shutdown();
            return 3;
        }
        if ((sos_drum_bank = sos_load_file((int)D_001700CC)) == 0) {
            sos_shutdown();
            return 4;
        }
        if (func_0009FEE5(D_0018DD58, (void __far *)(void *)sos_melodic_bank, 1) != 0) {
            sos_shutdown();
            return 5;
        }
        if (func_0009FEE5(D_0018DD58, (void __far *)(void *)sos_drum_bank, 1) != 0) {
            sos_shutdown();
            return 5;
        }
    }
    dpmi_lock_region((int)D_0018DC38, 268);
    dpmi_lock_region((int)D_0018DD64, 46);
    dpmi_lock_region((int)&D_0018DD5C, 4);
    dpmi_lock_region((int)&D_0018DD60, 4);
    dpmi_lock_region((int)D_0018DD50, 4);
    dpmi_lock_region((int)&D_0018DD54, 4);
    dpmi_lock_region((int)&D_0018DD58, 4);
    dpmi_lock_region((int)D_0018DD4C, 4);
    dpmi_lock_region((int)&D_0018DC34, 4);
    dpmi_lock_region((int)&sos_melodic_bank, 4);
    dpmi_lock_region((int)&sos_drum_bank, 4);
    dpmi_lock_region(sos_melodic_bank, 8192);
    dpmi_lock_region(sos_drum_bank, 8192);
    return 0;
}
