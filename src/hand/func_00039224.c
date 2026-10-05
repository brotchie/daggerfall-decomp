/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039224 */
struct pair { unsigned char a; unsigned char b; };
extern struct pair *selected_spell;
extern short spell_effect_text_base[];

int spell_effect_text_index(short a1)
{
    short s;
    short t;

    s = spell_effect_text_base[selected_spell[a1].a];
    t = selected_spell[a1].b;
    if (t != 255 && selected_spell[a1].a != 29)
        s += t;
    return s;
}
