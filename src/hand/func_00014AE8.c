/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00014AE8 */
struct rec { unsigned char type; char pad1[32]; unsigned short f33; char pad35[19]; unsigned char f54; };
struct who { char pad[31]; int f31; };
extern char D_001703C9[];
extern char D_001703D6[];
extern char D_001703E3[];
extern char D_001703F0[];
extern char D_00187CA8;
extern char D_00190D0F;
extern char D_00190D10;
extern int window_image;
extern char *D_00195C44;
extern char *D_00195D28;
extern int talk_disposition;
extern unsigned char D_0019626F;
extern unsigned char D_00196272;
extern unsigned char game_mode;
extern unsigned char crime_current;
extern unsigned char D_0019627F;
extern void *talk_saved_screen;
extern struct rec *talk_npc_own_faction;
extern struct rec *talk_npc_faction;
extern char *talk_place_topics;
extern int talk_npc_speech_style;
extern int talk_npc_attitude;
extern int D_001965C8;
extern int talk_attitude_cache;
extern int D_001965D0;
extern int D_001965D4;
extern struct who *talk_npc_object;
extern int D_001965E4;
extern int talk_selected_row;
extern int talk_face_image;
extern int talk_list_bottom;
extern int talk_list_top;
extern int D_001965F8;
extern char D_001965FC[];
extern short D_001966A4;
extern unsigned char talk_flags;
extern short D_001966AC;
extern char talk_question_mode;
extern char talk_showing_categories;
extern char talk_topic_tab;
extern char talk_tone;
extern char talk_redraw;
extern char talk_news_asked;
extern char D_001966B7;
extern char talk_prostitute_offer;
extern char D_001966BA;
extern char D_001966BB;
extern void talk_close(void);
extern void talk_say_text(int);
extern void talk_say_string(int);
extern void talk_init_text(void);
extern void talk_build_place_topics(void);
extern void func_00018339(void);
extern int talk_faction_greeting(int);
extern int talk_roll_attitude(void);
extern int func_0001D46A(int);
extern void msgbox_show_rsc(short, int);
extern void guards_summon(int);
extern void person_load_face(struct who *, int);
extern int func_00041347(void);
extern int disk_read_file(char *, int);
extern int rand(void);
extern void *mc_malloc(int, char *, int);
extern void func_0012DB50(int);

int talk_open(struct who *a1)
{
    int err;
    int msg;

    if (D_0019626F == 12 && game_mode == 8)
        return 1;
    if (a1 != 0) {
        if (talk_npc_faction->f54 == 4 && (func_00041347() & 2)) {
            D_0019627F &= 2;
            crime_current = 7;
            guards_summon(0);
            return 0;
        }
        talk_news_asked = 0;
        D_001966BA = 0;
        talk_prostitute_offer = 0;
        talk_npc_object = a1;
        func_0012DB50(4);
        game_mode = 12;
        window_image = disk_read_file(D_001703C9, 0);
        D_001965E4 = disk_read_file(D_001703D6, 0);
        D_001965F8 = disk_read_file(D_001703E3, 0);
        talk_saved_screen = mc_malloc(64000, D_001703F0, 225);
        D_00196272 = 1;
        D_001966BB = 0;
        talk_tone = 1;
        talk_question_mode = 1;
        talk_topic_tab = 0;
        talk_list_top = 0;
        talk_list_bottom = 13;
        talk_selected_row = 0;
        talk_redraw = 1;
        D_001966A4 = 0;
        D_001966B7 = 0;
        D_001966AC = 0;
        talk_attitude_cache = D_001965D0 = D_001965D4 = 0;
        D_00195D28 = D_001965FC;
        talk_place_topics = D_00195C44 + 64768;
        func_00018339();
        talk_showing_categories = 1;
        talk_build_place_topics();
        talk_flags |= 2;
        D_00187CA8 = 0;
        D_00190D0F = 0;
        talk_init_text();
        D_001965C8 = rand();
        if (D_00190D10 == 0)
            person_load_face(talk_npc_object, talk_face_image);
        err = func_0001D46A(talk_npc_object->f31);
        if (talk_npc_own_faction->type == 15 || talk_npc_own_faction->type == 14)
            msg = talk_npc_own_faction->f33;
        else
            msg = talk_npc_faction->f33;
        if (talk_npc_own_faction->type == 15 || talk_npc_own_faction->type == 14) {
            if (talk_npc_own_faction->f54 < 5)
                talk_npc_speech_style = talk_npc_own_faction->f54;
            else
                talk_npc_speech_style = 1;
        } else {
            if (talk_npc_faction->f54 < 5)
                talk_npc_speech_style = talk_npc_faction->f54;
            else
                talk_npc_speech_style = 1;
        }
        if (talk_npc_speech_style == 1)
            talk_npc_speech_style = 0;
        else if (talk_npc_speech_style == 0)
            talk_npc_speech_style = 1;
        talk_npc_attitude = talk_roll_attitude();
        if (err != 0) {
            talk_say_string(err);
        } else {
            msg = talk_faction_greeting(msg);
            if (msg != 0) {
                if (D_001966BA) {
                    talk_close();
                    msgbox_show_rsc(msg, 1);
                    return 0;
                }
                talk_say_text(msg);
            } else if (talk_disposition < 0) {
                talk_say_text(7206);
            } else if (talk_disposition < 10) {
                talk_say_text(7207);
            } else if (talk_disposition >= 10 && talk_disposition < 30) {
                talk_say_text(7208);
            } else {
                talk_say_text(7209);
            }
        }
    }
    return game_mode == 12 ? 1 : 0;
}
