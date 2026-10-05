/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00038228 */
#include "records.h"

extern char D_00170B13[];
extern struct spell *selected_spell;
extern char spell_effect_settings[];
extern char spell_effect_costs[];
extern char spell_effect_cost_index[];
extern char spell_effect_subtype_names[];
extern int list_popup_callback;
extern char scratch_buffer[];
extern short spell_effect_slot;
extern char spell_effect_cost_current[];
extern short D_00199628;
extern char spellmaker_settings_kind[];
extern int func_00037AB7(void);
extern int spellmaker_allowed_targets(void);
extern void spellmaker_pick_subtype_cb(int);
extern int spellmaker_find_effect(short);
extern void list_popup_open_strings(char *);
extern int mc_strncpy();
extern int strlen();
extern int mc_memcpy();

void spellmaker_pick_effect_cb(short effect_type)
{
    short j;

    selected_spell->effects[spell_effect_slot = spellmaker_find_effect(255)].type = effect_type;
    if (func_00037AB7() == 0)
        selected_spell->element = 4;
    j = spellmaker_allowed_targets();
    if (j != 2) {
        if (j == 0 && selected_spell->target != 0)
            selected_spell->target = 0;
        if (j == 1 && selected_spell->target == 0)
            (selected_spell->target)++;
    }
    if (*(int *)(spell_effect_subtype_names + effect_type * 48) == 0) {
        mc_memcpy(spell_effect_cost_current, spell_effect_costs + (*(unsigned char *)(spell_effect_cost_index + effect_type * 12) << 3), 8, D_00170B13, 1058, 8);
        D_00199628 = 0;
        *(char *)spellmaker_settings_kind = *(char *)(spell_effect_settings + effect_type * 12);
    } else {
        char *s;

        s = *(char **)scratch_buffer;
        *s = 0;
        j = 0;
        while (*(int *)(spell_effect_subtype_names + effect_type * 48 + j * 4) != 0) {
            *(*(char **)scratch_buffer + j + 32000) = j;
            mc_strncpy(s, *(int *)(spell_effect_subtype_names + effect_type * 48 + j++ * 4), 4, D_00170B13, 1071);
            s = s + strlen(s) + 1;
        }
        *s = 0;
        list_popup_callback = (int)spellmaker_pick_subtype_cb;
        list_popup_open_strings(*(char **)scratch_buffer);
    }
}
