/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004E02D */
#pragma pack(1)
struct ev {
    unsigned char type;
    short a, b, c, d;
    char e;
    char pad;
};
#pragma pack()
extern char D_00174FAC[];
extern char D_001851FF;
extern unsigned char D_001940D5;
extern struct ev *note_page;
extern int D_001997D8;
extern short note_page_free;
extern void msgbox_show_rsc(int, int);
extern int mc_memcpy();

void note_add_line(short a1, short a2, short a3, short a4)
{
    struct ev *l_1C;

    if ((unsigned)note_page_free < 11) {
        msgbox_show_rsc(1700, 1);
        return;
    }
    mc_memcpy(D_001997D8, note_page, 3640, D_00174FAC, 399, 4);
    D_001940D5 |= 16;
    l_1C = note_page;
    while (l_1C->type != 0) {
        if (l_1C->type == 1)
            l_1C = (struct ev *)((char *)l_1C + 91);
        else
            l_1C++;
    }
    l_1C->type = 2;
    l_1C->a = a1;
    l_1C->b = a2;
    l_1C->c = a3;
    l_1C->d = a4;
    l_1C->e = D_001851FF;
    l_1C++;
    l_1C->type = 0;
}
