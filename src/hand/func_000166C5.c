/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000166C5 */
#include "records.h"

extern char player_environment[];
extern char D_00190D11[];
extern char D_00195A90[];
extern char D_00195A94[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char D_00195D28[];
extern char D_00196488[];
extern char talk_topics[];
extern struct faction *talk_npc_own_faction;
extern char D_001965DC[];
extern char talk_selected_row[];
extern char talk_topic_tab[];
extern char talk_news_asked[];
extern int talk_find_regional(int);
extern void town_map_note_building(struct record *, int);
extern int rand_range(int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int func_000C7FD9();


int talk_hint_text_id(int a1)
{
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    struct record *l_1C;

    l_38 = 0;
    if (*(unsigned char *)talk_topic_tab == 3) {}
    if (*(int *)D_001965DC == 2) {
        if (*(short *)(*(char **)D_00195D28 + 4) == 0) {
            if (*(int *)(*(char **)D_00195D28 + 18) == 0) {
                if (talk_find_regional(*(unsigned char *)(D_00196488 + *(int *)talk_selected_row)) != 0)
                    return 10;
                return 11;
            }
            l_1C = object_find_by_id(D_00195AC4, *(int *)(*(char **)(*(char **)D_00195D28 + 18) + 20));
            *(int *)D_00195A90 = l_1C->x;
            *(int *)D_00195A94 = l_1C->z;
            if (*(unsigned char *)player_environment == 1 && (func_000C7FD9(l_1C->x, l_1C->z, player_object->x, player_object->z) < 2048 || rand_range(1, 100) <= 25)) {
                town_map_note_building(l_1C, *(int *)(*(char **)D_00195D28 + 18));
                return 7332;
            }
            return 7333;
        }
    } else {
        if (talk_npc_own_faction->id != 806 && talk_npc_own_faction->id != 842)
            *(char *)talk_news_asked = 1;
        if ((*(short **)talk_topics)[*(int *)talk_selected_row * 3 + 2] != 0) {
            *(char *)D_00190D11 = (*(short **)talk_topics)[*(int *)talk_selected_row * 3 + 2];
            l_30 = 32768;
        } else
            l_30 = 0;
        if (a1 != 0 && (*(short **)talk_topics)[*(int *)talk_selected_row * 3 + 1] != 0)
            return l_30 | (*(short **)talk_topics)[*(int *)talk_selected_row * 3 + 1];
        if ((*(short **)talk_topics)[*(int *)talk_selected_row * 3] != 0)
            return l_30 | (*(short **)talk_topics)[*(int *)talk_selected_row * 3];
        return l_30 | (*(short **)talk_topics)[*(int *)talk_selected_row * 3 + 1];
    }
    return 100;
}
