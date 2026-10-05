/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */



short hex_digit_value(char *digit)
{
    if (((int)(unsigned char)*digit) >= 48 && ((int)(unsigned char)*digit) <= 57) {
        return ((unsigned short)(unsigned char)*digit) - 48;
    }
    if (((int)(unsigned char)*digit) >= 65 && ((int)(unsigned char)*digit) <= 90) {
        return ((unsigned short)(unsigned char)*digit) - 55;
    }
    if (((int)(unsigned char)*digit) >= 97 && ((int)(unsigned char)*digit) <= 97) {
        return ((unsigned short)(unsigned char)*digit) - 87;
    }
    return 0;
}
