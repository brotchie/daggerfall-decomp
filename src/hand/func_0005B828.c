/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005B828 */
struct ent { char pad[137]; int flags; };
struct spell { int id; int mask; };
extern struct spell D_00185C3F[];
extern int guild_npc_object;
extern short spell_effect_slot;
extern char *spell_find_active_effect(char *, int, int *, int *);
extern void spell_end(int);
extern void spfx_effect_end(char *, int, char *);

void spell_break_concealment(char *a1)
{
    char *l_24;
    struct ent *l_20;
    int l_1C;
    int l_18;

    l_20 = (struct ent *)(a1 + 71);
    if ((l_20->flags & 0x3004) == 0) return;
    for (l_18 = 0; l_18 < 3; l_18++) {
        l_24 = spell_find_active_effect(a1, D_00185C3F[l_18].id, &l_1C, &l_1C);
        if (l_24 == 0) {
            l_20->flags &= ~D_00185C3F[l_18].mask;
            continue;
        }
        if (l_24[spell_effect_slot * 2 + 1] == 0) {
            spfx_effect_end(l_24, spell_effect_slot, a1);
            if (spell_effect_slot == 0 && *(unsigned char *)(l_24 + 2) == 255)
                spell_end(guild_npc_object);
        }
    }
}
