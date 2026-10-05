/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */



short func_0008269B(int a1)
{
    if (((int)(unsigned char)*(signed char *)((char *)a1)) < 48) goto L826CA;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) <= 57) goto L826CC;
L826CA:;
    goto L826DE;
L826CC:;
    return ((unsigned short)(unsigned char)*(signed char *)((char *)a1)) - 48;
L826DE:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) < 65) goto L826FC;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) <= 90) goto L826FE;
L826FC:;
    goto L8270D;
L826FE:;
    return ((unsigned short)(unsigned char)*(signed char *)((char *)a1)) - 55;
L8270D:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) < 97) goto L8272B;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) <= 97) goto L8272D;
L8272B:;
    goto L8273C;
L8272D:;
    return ((unsigned short)(unsigned char)*(signed char *)((char *)a1)) - 87;
L8273C:;
    return 0;
}
