/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004EAF4 */
#include "structs.h"

unsigned char *note_page_walk(unsigned char *entry, int (*text_cb)(unsigned char *), int (*line_cb)(unsigned char *))
{
    char unused1[12];
    char unused2[12];

    while (*entry != 0) {
        if (*entry == 1) {
            if (text_cb != 0)
                if (text_cb(entry) != 0)
                    return entry;
            entry += 91;
        } else {
            if (line_cb != 0)
                if (line_cb(entry) != 0)
                    return entry;
            (*(struct note_line **)&entry)++;
        }
    }
    return 0;
}
