/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000849E4 */
extern int func_000844FB(int, int);

void func_000849E4(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_18 = ((int)(unsigned short)(*(short *)((char *)a2 + 12) & 31)) - 2; switch (l_18) {
    case 8:
    case 9:
    case 13:
    case 14:
    case 16:
    case 17:
        l_20 = func_000844FB(a1, (int)(unsigned short)*(short *)((char *)a2 + 12));
        *(int *)((char *)l_20 + 7) = *(int *)((char *)a2);
        *(int *)((char *)l_20 + 15) = *(int *)((char *)a2 + 8);
        *(int *)((char *)l_20 + 11) = *(int *)((char *)a2 + 4);
        break;
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
    case 10: case 11: case 12: case 15:
        break;
    }
}
