/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002829B */
extern char D_001707AE[];
extern char scratch_190ce5;
extern short scratch_190d68;
extern short scratch_190d6a;
extern char *scratch_buffer;
extern int town_notes_size(void);
extern void mc_strncpy(char *, int, int, char *, int);

void town_note_add(int a1)
{
    int l_18;

    scratch_190ce5 = 1;
    l_18 = town_notes_size();
    *(short *)(scratch_buffer + l_18) = scratch_190d68;
    *(short *)(scratch_buffer + l_18 + 2) = scratch_190d6a;
    mc_strncpy(scratch_buffer + (l_18 + 4), a1, 4, D_001707AE, 823);
}
