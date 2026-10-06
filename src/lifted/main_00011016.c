/* main.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"

extern char D_001700C0[];
extern char D_001700CC[];
extern iptr D_0018DC34;
extern char D_0018DC38[];
extern int D_0018DC3C;
extern int D_0018DC7C;
extern int D_0018DD3C;
extern iptr D_0018DD40;
extern iptr sos_drum_bank;
extern iptr sos_melodic_bank;
extern char D_0018DD4C[];
extern char D_0018DD50[];
extern int D_0018DD54;
extern int D_0018DD58;
extern int D_0018DD5C;
extern int D_0018DD60;
extern char D_0018DD64[];
extern signed char music_uses_fm;

extern int sos_shutdown(void);
extern iptr sos_load_file(char *);
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_load_file;
extern int dpmi_lock_region(iptr, int);
#pragma aux (sosconv) sos_init;

int sos_init(int digi_device, int midi_device)
{
    D_0018DC3C = 11025;
    D_0018DC7C = 4096;
    func_0009E1A1(65280, 0);
    func_0009E8FF(0, 0);
    func_0009E9C2(0, 0);
    if (midi_device != (-1)) {
        *(int *)D_0018DD64 = midi_device;
        if (func_0009EC82(D_0018DD64, &D_0018DD58) != 0) {
            func_0009F9A7(D_0018DD60, 1, 1);
            func_0009E95B();
            func_0009EC0A();
            return 2;
        }
    }
    if (digi_device != (-1)) {
        D_0018DD3C = digi_device;
        if (func_0009F4DE(D_0018DC38, &D_0018DD60) != 0) {
            func_0009E95B();
            return 1;
        }
    }
    if (digi_device != (-1)) func_0009E2BB(90, (void (*)(void))D_0018DD40, (int *)D_0018DD50);
    if (midi_device == 40962 || (midi_device == 40969 && midi_device != (-1))) {
        music_uses_fm = 1;
        if ((sos_melodic_bank = sos_load_file(D_001700C0)) == 0) {
            sos_shutdown();
            return 3;
        }
        if ((sos_drum_bank = sos_load_file(D_001700CC)) == 0) {
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
    dpmi_lock_region((iptr)D_0018DC38, 268);
    dpmi_lock_region((iptr)D_0018DD64, 46);
    dpmi_lock_region((iptr)&D_0018DD5C, 4);
    dpmi_lock_region((iptr)&D_0018DD60, 4);
    dpmi_lock_region((iptr)D_0018DD50, 4);
    dpmi_lock_region((iptr)&D_0018DD54, 4);
    dpmi_lock_region((iptr)&D_0018DD58, 4);
    dpmi_lock_region((iptr)D_0018DD4C, 4);
    dpmi_lock_region((iptr)&D_0018DC34, 4);
    dpmi_lock_region((iptr)&sos_melodic_bank, 4);
    dpmi_lock_region((iptr)&sos_drum_bank, 4);
    dpmi_lock_region(sos_melodic_bank, 8192);
    dpmi_lock_region(sos_drum_bank, 8192);
    return 0;
}
