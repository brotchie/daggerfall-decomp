/* spells.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */



void spellmaker_adjust_value(int a1, int a2, int a3, short a4)
{
    *(signed char *)((char *)a1) += *(signed char *)&a2;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) < 1) *(signed char *)((char *)a1) = 1;
    if ((short)((unsigned short)(unsigned char)*(signed char *)((char *)a1)) > *(short *)&a3) {
        *(signed char *)((char *)a1) = *(signed char *)&a3;
    }
    if (a4 < 0 && *(unsigned char *)((char *)(((int)(short)a4) + a1)) > *(unsigned char *)((char *)a1)) {
        *(signed char *)((char *)(((int)(short)a4) + a1)) = *(signed char *)((char *)a1);
    }
    if (a4 <= 0 || *(unsigned char *)((char *)(((int)(short)a4) + a1)) >= *(unsigned char *)((char *)a1)) {
        return;
    }
    *(signed char *)((char *)(((int)(short)a4) + a1)) = *(signed char *)((char *)a1);
}
