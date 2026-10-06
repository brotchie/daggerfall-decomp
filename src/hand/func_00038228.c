/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00038228 */
#include "records.h"
#include "clib.h"

extern char D_00170B13[];
extern struct spell *selected_spell;
extern char spell_effect_settings[];
extern char spell_effect_costs[];
extern char spell_effect_cost_index[];
extern char *spell_effect_subtype_names[][12];
extern iptr list_popup_callback;
extern char *scratch_buffer;
extern short spell_effect_slot;
extern char spell_effect_cost_current[];
extern short D_00199628;
extern char spellmaker_settings_kind[];
extern int func_00037AB7(void);
extern int spellmaker_allowed_targets(void);
extern void spellmaker_pick_subtype_cb(int);
extern int spellmaker_find_effect(short);
extern void list_popup_open_strings(char *);

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
    if ((iptr)spell_effect_subtype_names[effect_type][0] == 0) {
        mc_memcpy(spell_effect_cost_current, spell_effect_costs + (*(unsigned char *)(spell_effect_cost_index + effect_type * 12) << 3), 8, D_00170B13, 1058, 8);
        D_00199628 = 0;
        *(char *)spellmaker_settings_kind = *(char *)(spell_effect_settings + effect_type * 12);
    } else {
        char *s;

        s = scratch_buffer;
        *s = 0;
        j = 0;
        while ((iptr)spell_effect_subtype_names[effect_type][j] != 0) {
            *(scratch_buffer + j + 32000) = j;
            mc_strncpy(s, spell_effect_subtype_names[effect_type][j++], 4, D_00170B13, 1071);
            s = s + strlen(s) + 1;
        }
        *s = 0;
        list_popup_callback = (iptr)spellmaker_pick_subtype_cb;
        list_popup_open_strings(scratch_buffer);
    }
}
