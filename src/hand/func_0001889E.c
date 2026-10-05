/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001889E */
#include "records.h"

extern char *D_00147954;
extern char D_001703F0[];        /* __FILE__ */
extern char D_00170431[];
extern short talk_organisations[];
extern short *talk_topics;
extern int talk_list_count;
extern int talk_selected_row;
extern int talk_list_bottom;
extern int talk_list_top;
extern char talk_key_text[][65];
extern short D_001966A2;
extern unsigned char D_001966B8;
extern void talk_list_draw_item(char *, int, int, int, int);
extern void func_0001839C(void);
extern struct faction *faction_find(short);
extern void mc_strncpy(char *, char *, int, char *, int);

void talk_draw_tell_list(void)
{
    int i;
    int color;
    char *str;

    talk_topics = (short *)(D_00147954 + 16384);
    talk_list_count = 0;
    D_001966A2 = 1;
    talk_topics[0] = talk_topics[1] = talk_topics[2] = 0;
    if (talk_list_count >= talk_list_top && talk_list_count <= talk_list_bottom) {
        if (talk_selected_row == talk_list_count) {
            color = 244;
            mc_strncpy(talk_key_text[D_001966B8], D_00170431, 4, D_001703F0, 1884);
        } else {
            color = 145;
        }
        talk_list_draw_item(D_00170431, 6, (talk_list_count - talk_list_top) * 7 + 71, color, 156);
    }
    talk_list_count++;
    func_0001839C();
    for (i = 0; i < 34; i++) {
        talk_topics[D_001966A2 * 3] = i > 7 ? i + 861 : i + 860;
        talk_topics[D_001966A2 * 3 + 1] = 0;
        talk_topics[D_001966A2 * 3 + 2] = 0;
        D_001966A2++;
        if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
            talk_list_count++;
            continue;
        }
        str = faction_find(talk_organisations[i])->name;
        if (talk_selected_row == talk_list_count) {
            color = 244;
            mc_strncpy(talk_key_text[D_001966B8], str, 4, D_001703F0, 1914);
        } else {
            color = 145;
        }
        talk_list_draw_item(str, 6, (talk_list_count - talk_list_top) * 7 + 71, color, 156);
        talk_list_count++;
    }
}
