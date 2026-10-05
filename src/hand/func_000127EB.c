/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000127EB */
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_delete_item;
extern char D_00170129[];
extern int mc_memmove();
struct stream;
extern int profile_find_item(struct stream *, ...);

#pragma pack(1)
struct stream {
    unsigned char f0;
    unsigned char flags;
    char pad2[130];
    char *buffer;                   /* +0x84 */
    int length;                     /* +0x88 */
    char pad8c[16];
    char *item;                     /* +0x9C: the found item's line */
};
#pragma pack()

int profile_delete_item(struct stream *profile, int item)
{
    char *line;
    int length;

    length = 0;
    if ((short)profile_find_item(profile, item) == 0) return 0;
    line = profile->item;
    while (line[length] != 10) length++;
    length++;
    mc_memmove(line, line + length, profile->buffer + profile->length - (line + length), D_00170129, 1119, 4);
    profile->length -= length;
    profile->flags |= 128;
    return 1;
}
