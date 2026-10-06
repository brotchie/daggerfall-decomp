/* sosez.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"
#include "clib.h"

extern char D_001700D5[];
extern char D_00170112[];
extern iptr D_0018DC34;
extern char D_0018DC38[];
extern iptr sos_drum_bank;
extern iptr sos_melodic_bank;
extern char D_0018DD4C[];
extern char D_0018DD50[];
extern int D_0018DD54;
extern int D_0018DD58;
extern int D_0018DD5C;
extern int D_0018DD60;
extern char D_0018DD64[];
extern iptr D_001A3F48;
extern int D_001A3F4C;

extern int dpmi_unlock_region(iptr, int);
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_shutdown;
#pragma aux (sosconv) sos_load_sample;

int sos_shutdown(void)
{
    func_0009E61A(*(int *)D_0018DD50);
    if (D_0018DD5C != (-1)) func_0009F9A7(D_0018DD60, 1, 1);
    if (D_0018DD54 != (-1)) func_0009F253(D_0018DD58, 1);
    func_0009EC0A();
    func_0009E95B();
    func_0009E281(0);
    dpmi_unlock_region((iptr)D_0018DC38, 268);
    dpmi_unlock_region((iptr)D_0018DD64, 46);
    dpmi_unlock_region((iptr)&D_0018DD5C, 4);
    dpmi_unlock_region((iptr)&D_0018DD60, 4);
    dpmi_unlock_region((iptr)D_0018DD50, 4);
    dpmi_unlock_region((iptr)&D_0018DD54, 4);
    dpmi_unlock_region((iptr)&D_0018DD58, 4);
    dpmi_unlock_region((iptr)D_0018DD4C, 4);
    dpmi_unlock_region((iptr)&D_0018DC34, 4);
    dpmi_unlock_region((iptr)&sos_melodic_bank, 4);
    dpmi_unlock_region((iptr)&sos_drum_bank, 4);
    if (sos_melodic_bank != 0) {
        dpmi_unlock_region(sos_melodic_bank, 8192);
        if (sos_melodic_bank != 0 && sos_melodic_bank != (-1751672937)) {
            mc_free((void *)sos_melodic_bank, D_001700D5, 191);
            sos_melodic_bank = -1751672937;
        }
    }
    if (sos_drum_bank != 0) {
        dpmi_unlock_region(sos_drum_bank, 8192);
        if (sos_drum_bank != 0 && sos_drum_bank != (-1751672937)) {
            mc_free((void *)sos_drum_bank, D_001700D5, 197);
            sos_drum_bank = -1751672937;
        }
    }
    if (D_0018DC34 != 0) {
        dpmi_unlock_region(D_001A3F48, D_001A3F4C);
        if (D_001A3F48 != 0 && D_001A3F48 != (-1751672937)) {
            mc_free((void *)D_001A3F48, D_001700D5, 203);
            D_001A3F48 = -1751672937;
        }
    }
    return 1;
}

iptr sos_load_sample(char *path)
{
    int handle;
    int size;
    iptr buffer;
    struct sos_sample *sample;
    struct wav_header *wav;

    handle = open(path, 512);
    if (handle == (-1)) return 0;
    size = lseek(handle, 0, 2);
    lseek(handle, 0, 0);
    buffer = (iptr)mc_malloc(size + 240, D_001700D5, 302);
    if (buffer == 0) {
        close(handle);
        return 0;
    }
    if (read(handle, (void *)(buffer + 240), size) != size) {
        close(handle);
        if (buffer != 0 && buffer != (-1751672937)) {
            mc_free((void *)buffer, D_001700D5, 318);
            buffer = -1751672937;
        }
        return 0;
    }
    close(handle);
    mc_memset((void *)buffer, 0, 240, D_001700D5, 328, 4);
    sample = (struct sos_sample *)buffer;
    if (strncmp((char *)(buffer + 240), D_00170112, 4) == 0) {
        wav = (struct wav_header *)(buffer + 240);
        sample->data = (char *)(buffer + 284);
        sample->length = wav->data_size - 44;
        sample->bits = wav->bits;
        sample->channels = wav->channels;
        if (wav->bits == 8) {
            sample->format = 32768;
        } else {
            sample->format = 0;
        }
        sample->rate = wav->rate;
    } else {
        sample->data = (char *)(buffer + 240);
        sample->length = size;
        sample->bits = 8;
        sample->channels = 1;
        sample->format = 32768;
        sample->rate = 11025;
    }
    sample->pan = 32768;
    sample->volume = 2147450879;
    return (iptr)sample;
}
