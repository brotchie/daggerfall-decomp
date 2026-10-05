/* sosez.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001700D5[];
extern char D_00170112[];
extern int D_0018DC34;
extern char D_0018DC38[];
extern int sos_drum_bank;
extern int sos_melodic_bank;
extern char D_0018DD4C[];
extern char D_0018DD50[];
extern int D_0018DD54;
extern int D_0018DD58;
extern int D_0018DD5C;
extern int D_0018DD60;
extern char D_0018DD64[];
extern int D_001A3F48;
extern int D_001A3F4C;

extern int dpmi_unlock_region(int, int);
extern int open(int, ...);
extern int func_0009DEA7();
extern int func_0009E281();
extern int func_0009E61A();
extern int func_0009E95B();
extern int func_0009EC0A();
extern int func_0009F253();
extern int func_0009F9A7();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int mc_malloc();
extern int func_000A00CB();
extern int strncmp();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_shutdown;
#pragma aux (sosconv) func_0001167F;

int sos_shutdown(void)
{
    func_0009E61A(*(int *)D_0018DD50);
    if (D_0018DD5C != (-1)) func_0009F9A7(D_0018DD60, 1, 1);
    if (D_0018DD54 != (-1)) func_0009F253(D_0018DD58, 1);
    func_0009EC0A();
    func_0009E95B();
    func_0009E281(0);
    dpmi_unlock_region((int)D_0018DC38, 268);
    dpmi_unlock_region((int)D_0018DD64, 46);
    dpmi_unlock_region((int)&D_0018DD5C, 4);
    dpmi_unlock_region((int)&D_0018DD60, 4);
    dpmi_unlock_region((int)D_0018DD50, 4);
    dpmi_unlock_region((int)&D_0018DD54, 4);
    dpmi_unlock_region((int)&D_0018DD58, 4);
    dpmi_unlock_region((int)D_0018DD4C, 4);
    dpmi_unlock_region((int)&D_0018DC34, 4);
    dpmi_unlock_region((int)&sos_melodic_bank, 4);
    dpmi_unlock_region((int)&sos_drum_bank, 4);
    if (sos_melodic_bank != 0) {
        dpmi_unlock_region(sos_melodic_bank, 8192);
        if (sos_melodic_bank != 0 && sos_melodic_bank != (-1751672937)) {
            mc_free(sos_melodic_bank, (int)D_001700D5, 191);
            sos_melodic_bank = -1751672937;
        }
    }
    if (sos_drum_bank != 0) {
        dpmi_unlock_region(sos_drum_bank, 8192);
        if (sos_drum_bank != 0 && sos_drum_bank != (-1751672937)) {
            mc_free(sos_drum_bank, (int)D_001700D5, 197);
            sos_drum_bank = -1751672937;
        }
    }
    if (D_0018DC34 != 0) {
        dpmi_unlock_region(D_001A3F48, D_001A3F4C);
        if (D_001A3F48 != 0 && D_001A3F48 != (-1751672937)) {
            mc_free(D_001A3F48, (int)D_001700D5, 203);
            D_001A3F48 = -1751672937;
        }
    }
    return 1;
}

int func_0001167F(int a1)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_20 = open(a1, 512);
    if (l_20 == (-1)) return 0;
    l_1C = lseek(l_20, 0, 2);
    lseek(l_20, 0, 0);
    l_18 = mc_malloc(l_1C + 240, (int)D_001700D5, 302);
    if (l_18 == 0) {
        func_0009DEA7(l_20);
        return 0;
    }
    if (func_000A00CB(l_20, l_18 + 240, l_1C) != l_1C) {
        func_0009DEA7(l_20);
        if (l_18 != 0 && l_18 != (-1751672937)) {
            mc_free(l_18, (int)D_001700D5, 318);
            l_18 = -1751672937;
        }
        return 0;
    }
    func_0009DEA7(l_20);
    mc_memset(l_18, 0, 240, (int)D_001700D5, 328, 4);
    l_14 = l_18;
    if (strncmp(l_18 + 240, (int)D_00170112, 4) == 0) {
        l_10 = l_18 + 240;
        *(int *)((char *)l_14) = l_18 + 284;
        *(int *)((char *)l_14 + 12) = *(int *)((char *)l_10 + 40) - 44;
        *(int *)((char *)l_14 + 56) = (int)(short)*(short *)((char *)l_10 + 34);
        *(int *)((char *)l_14 + 60) = (int)(short)*(short *)((char *)l_10 + 22);
        if (((int)(short)*(short *)((char *)l_10 + 34)) == 8) {
            *(int *)((char *)l_14 + 64) = 32768;
        } else {
            *(int *)((char *)l_14 + 64) = 0;
        }
        *(int *)((char *)l_14 + 52) = *(int *)((char *)l_10 + 24);
    } else {
        *(int *)((char *)l_14) = l_18 + 240;
        *(int *)((char *)l_14 + 12) = l_1C;
        *(int *)((char *)l_14 + 56) = 8;
        *(int *)((char *)l_14 + 60) = 1;
        *(int *)((char *)l_14 + 64) = 32768;
        *(int *)((char *)l_14 + 52) = 11025;
    }
    *(int *)((char *)l_14 + 68) = 32768;
    *(int *)((char *)l_14 + 44) = 2147450879;
    return l_14;
}
