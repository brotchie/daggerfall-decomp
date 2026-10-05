/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000595DF */
extern char D_00195B74[];
extern char D_00195B80[];
extern char D_00195C44[];
extern char D_001AA600[];
extern int func_000C0700();
extern int func_000CD262();
extern int func_000CD291();
extern int func_00135D00();
extern int func_00135E39();

void paperdoll_draw_item(int a1, int a2, int a3, int a4)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    short l_10;
    short l_C;

    *(int *)D_00195B80 = (int)(*(char **)D_001AA600 + (((int)(unsigned char)*(signed char *)((char *)a1 + 56)) << 8));
    l_24 = func_00135D00(((int)(unsigned short)*(short *)((char *)a1 + 50)) >> 7, (int)(unsigned short)(*(short *)((char *)a1 + 50) & 127), -1);
    if (l_24 != 0) goto L59677;
    func_00135E39();
    l_24 = func_00135D00(((int)(unsigned short)*(short *)((char *)a1 + 50)) >> 7, (int)(unsigned short)(*(short *)((char *)a1 + 50) & 127), -1);
L59677:;
    l_28 = *(int *)((char *)l_24 + 12);
    if (((int)(short)*(short *)((char *)a1 + 67)) != 26) goto L59698;
    if (((int)(short)*(short *)((char *)a1 + 69)) == 6) goto L5969A;
L59698:;
    goto L596FC;
L5969A:;
    func_000C0700(((int)(short)*(short *)((char *)l_28)) + a2, ((int)(short)*(short *)((char *)l_28 + 2)) + a3, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), (int)(unsigned short)(*(short *)((char *)l_28 + 8) | 32768), l_28 + *(int *)((char *)l_28 + 14));
    goto L59732;
L596FC:;
    func_000CD291(((int)(short)*(short *)((char *)l_28)) + a2, ((int)(short)*(short *)((char *)l_28 + 2)) + a3, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), l_28 + *(int *)((char *)l_28 + 14));
L59732:;
    l_10 = *(short *)((char *)l_28) - 192;
    l_C = *(short *)((char *)l_28 + 2) - 1;
    l_2C = (int)(*(char **)D_00195B74 + ((((int)(short)l_C) * 125) + ((int)(short)l_10)));
    func_000CD262(l_2C, (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), *(int *)D_00195C44, a4);
}
