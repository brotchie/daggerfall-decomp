/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00037AB7 */
extern unsigned char *selected_spell;
extern unsigned char D_0017A8B2[];

int func_00037AB7(void)
{
    short l_18;

    l_18 = D_0017A8B2[selected_spell[0]] > D_0017A8B2[selected_spell[2]] ? D_0017A8B2[selected_spell[0]] : D_0017A8B2[selected_spell[2]];
    return D_0017A8B2[selected_spell[4]] > l_18 ? D_0017A8B2[selected_spell[4]] : l_18;
}
