/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00078B8A */
extern char D_00176844[];
extern char D_0017685B[];
extern char text_buffer[];
extern char D_00190704[];
extern char D_00195AC8[];
extern int archive_find_record(int, int, int);
extern int archive_read_record(int, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, int);
extern int func_000A0F5C(int, ...);

struct span {
    int f0;
    char *start;
    char *cur;
};

void monster_reload_anim_cb(unsigned char *a1)
{
    unsigned char *l_28;
    struct span *l_24;
    unsigned char *l_20;
    int l_1C;
    int l_18;

    if (*a1 != 18) return;
    l_28 = a1 + 71;
    l_20 = l_28 + 560;
    l_24 = (struct span *)(l_20 + 74);
    l_1C = l_24->cur - l_24->start;
    func_000A0ED9(196, (int)D_00176844);
    func_000A0F5C((int)text_buffer, (int)D_0017685B, l_28[503]);
    l_18 = archive_find_record(*(int *)D_00195AC8, (int)text_buffer, 8);
    ((char **)D_00190704)[l_28[75]] = l_24->start = (char *)archive_read_record(*(int *)D_00195AC8, l_18, 0);
    if (l_24->cur == 0) return;
    l_24->cur = l_24->start + l_1C;
}
