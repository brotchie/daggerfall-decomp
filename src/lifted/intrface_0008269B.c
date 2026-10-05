/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */



short hex_digit_value(int a1)
{
    if (((int)(unsigned char)*(signed char *)((char *)a1)) >= 48 && ((int)(unsigned char)*(signed char *)((char *)a1)) <= 57) {
        return ((unsigned short)(unsigned char)*(signed char *)((char *)a1)) - 48;
    }
    if (((int)(unsigned char)*(signed char *)((char *)a1)) >= 65 && ((int)(unsigned char)*(signed char *)((char *)a1)) <= 90) {
        return ((unsigned short)(unsigned char)*(signed char *)((char *)a1)) - 55;
    }
    if (((int)(unsigned char)*(signed char *)((char *)a1)) >= 97 && ((int)(unsigned char)*(signed char *)((char *)a1)) <= 97) {
        return ((unsigned short)(unsigned char)*(signed char *)((char *)a1)) - 87;
    }
    return 0;
}
