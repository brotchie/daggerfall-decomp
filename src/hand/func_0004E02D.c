/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004E02D */
#pragma pack(1)
struct note_line {                 /* a notebook page's line entry (type 2, 11 bytes) */
    unsigned char type;
    short x1, y1, x2, y2;
    char colour;
    char flags;
};
#pragma pack()
extern char D_00174FAC[];
extern char note_colour;
extern unsigned char D_001940D5;
extern struct note_line *note_page;
extern int note_page_backup;
extern short note_page_free;
extern void msgbox_show_rsc(int, int);
extern int mc_memcpy();

void note_add_line(short x1, short y1, short x2, short y2)
{
    struct note_line *entry;

    if ((unsigned)note_page_free < 11) {
        msgbox_show_rsc(1700, 1);
        return;
    }
    mc_memcpy(note_page_backup, note_page, 3640, D_00174FAC, 399, 4);
    D_001940D5 |= 16;
    entry = note_page;
    while (entry->type != 0) {
        if (entry->type == 1)
            entry = (struct note_line *)((char *)entry + 91);
        else
            entry++;
    }
    entry->type = 2;
    entry->x1 = x1;
    entry->y1 = y1;
    entry->x2 = x2;
    entry->y2 = y2;
    entry->colour = note_colour;
    entry++;
    entry->type = 0;
}
