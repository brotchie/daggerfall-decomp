/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00038228 */
extern char D_00170B13[];
extern char selected_spell[];
extern char spell_effect_settings[];
extern char spell_effect_costs[];
extern char spell_effect_cost_index[];
extern char spell_effect_subtype_names[];
extern char list_popup_callback[];
extern char D_00195C44[];
extern char spell_effect_slot[];
extern char spell_effect_cost_current[];
extern char D_00199628[];
extern char spellmaker_settings_kind[];
extern int func_00037AB7(void);
extern short func_00037D5A(void);
extern void spellmaker_pick_subtype_cb(int);
extern short spellmaker_find_effect(short);
extern void picklist_open_strings(char *);
extern int mc_strncpy();
extern int func_000A0DF4();
extern int mc_memcpy();

void spellmaker_pick_effect_cb(short a1)
{
    short j;

    *(char *)(*(char **)selected_spell + (*(short *)spell_effect_slot = spellmaker_find_effect(255)) * 2) = a1;
    if (func_00037AB7() == 0)
        *(char *)(*(char **)selected_spell + 6) = 4;
    j = func_00037D5A();
    if (j != 2) {
        if (j == 0 && *(char *)(*(char **)selected_spell + 7) != 0)
            *(char *)(*(char **)selected_spell + 7) = 0;
        if (j == 1 && *(char *)(*(char **)selected_spell + 7) == 0)
            (*(char *)(*(char **)selected_spell + 7))++;
    }
    if (*(int *)(spell_effect_subtype_names + a1 * 48) == 0) {
        mc_memcpy(spell_effect_cost_current, spell_effect_costs + (*(unsigned char *)(spell_effect_cost_index + a1 * 12) << 3), 8, D_00170B13, 1058, 8);
        *(short *)D_00199628 = 0;
        *(char *)spellmaker_settings_kind = *(char *)(spell_effect_settings + a1 * 12);
    } else {
        char *s;

        s = *(char **)D_00195C44;
        *s = 0;
        j = 0;
        while (*(int *)(spell_effect_subtype_names + a1 * 48 + j * 4) != 0) {
            *(*(char **)D_00195C44 + j + 32000) = j;
            mc_strncpy(s, *(int *)(spell_effect_subtype_names + a1 * 48 + j++ * 4), 4, D_00170B13, 1071);
            s = s + func_000A0DF4(s) + 1;
        }
        *s = 0;
        *(int *)list_popup_callback = (int)spellmaker_pick_subtype_cb;
        picklist_open_strings(*(char **)D_00195C44);
    }
}
