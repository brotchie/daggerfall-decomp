/* question.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char mouse_buttons;
extern short mouse_y;
extern short D_0012DA44;
extern signed char key_down_a;
extern signed char key_down_c;
extern signed char key_down_b;
extern int screen_buffer;
extern char D_0017539B[];
extern char D_001753DB[];
extern char D_001753E7[];
extern int class_answer_cels[];
extern signed char text_buffer[];
extern char D_00190D64[];
extern char D_00190D66[];
extern short D_00190D68;
extern char frame_counter[];
extern int D_00195C44;
extern signed char mouse_buttons_prev;
extern signed char D_001997B5[];
extern char class_questions_asked[];
extern signed char D_00199816;
extern signed char D_00199817;
extern signed char D_00199818;
extern signed char class_answer_counts;
extern signed char D_0019981A;
extern signed char D_0019981B;

extern int text_rsc_load(int, int, int);
extern int pflc_play(int, int);
extern int sound_play_ui(int);
extern int disk_read_file(int, int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int atoi();
extern int func_000A0DF4();
extern int mc_memcpy();
extern int memchr();
extern int strchr();
extern int func_000A1944();
extern int func_000CD126();
extern int func_000CE46D();
extern int func_000CE49E();
extern void fatal_error(int);
extern void class_question_scroll(int, int);
extern void text_draw(int, int, int);
int func_00051991(short, short, int, int);

void class_question_show(int a1)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 0;
    l_1C = l_18;
    *(short *)D_00190D64 = 0;
    while (memchr((int)class_questions_asked, (int)(short)*(short *)&l_1C, 10) != 0) {
        l_1C = (rand() % 40) + 1;
    }
    func_000CE49E((int)class_questions_asked, (int)(short)*(short *)&l_1C);
    l_28 = text_rsc_load(9000, 8, 0);
    l_2C = l_28;
    D_00190D68 = l_1C;
    do {
        l_28 = memchr(l_28, 123, 32000) + 1;
        func_000CE46D((int)text_buffer, l_28, 46);
    } while (atoi((int)text_buffer) != ((int)(short)*(short *)&l_1C));
    l_28 += 2;
    *(signed char *)((char *)memchr(l_28, 123, 32000)) = 0;
    l_20 = mc_malloc(64000, (int)D_0017539B, 176);
    mc_memcpy(l_20, screen_buffer, 64000, (int)D_0017539B, 178, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_0017539B, 179, 4);
    while (*(signed char *)((char *)l_28) != 0 && *(signed char *)((char *)l_28 - 1) != 0) {
        func_000CE46D((int)text_buffer, l_28, 252);
        l_28 += func_000A0DF4((int)text_buffer) + 1;
        l_30 = strchr((int)text_buffer, 41);
        if (l_30 != 0) {
            D_001997B5[(int)(unsigned char)*(signed char *)((char *)l_30 - 1)] = *(signed char *)&l_18;
        }
        text_draw((int)text_buffer, 20, ((int)(short)D_0012DA44) * ((int)(short)*(short *)&l_18));
        (*(short *)&l_18)++;
    }
    mc_memcpy(D_00195C44, screen_buffer, 64000, (int)D_0017539B, 193, 4);
    mc_memcpy(screen_buffer, l_20, 64000, (int)D_0017539B, 194, 4);
    if (l_20 != 0 && l_20 != (-1751672937)) {
        mc_free(l_20, (int)D_0017539B, 195);
        l_20 = -1751672937;
    }
    *(short *)D_00190D66 = (D_0012DA44 * l_18) - 52;
    class_question_scroll(a1, 0);
    if (l_2C == 0 || l_2C == (-1751672937)) return;
    mc_free(l_2C, (int)D_0017539B, 199);
    l_2C = -1751672937;
}

void func_0005175B(int a1, short a2)
{
    if (a2 < 0 && *(short *)D_00190D64 != 0) {
        *(short *)D_00190D64 += *(int *)&a2;
    } else if (a2 > 0 && *(short *)D_00190D64 < *(short *)D_00190D66) {
        *(short *)D_00190D64 += *(int *)&a2;
    }
    *(int *)frame_counter = (int)(short)(*(short *)D_00190D64 & 7);
    func_000CD126(*(int *)((char *)(((((int)(short)(*(short *)D_00190D64 & 15)) >> 3) << 2) + a1)), 0, 119);
}

void class_question_answer_anim(short a1)
{
    {
        char l_44[44];

        func_000A1944((int)l_44, 0, 44);
        *(short *)l_44 = 16;
        sound_play_ui(18);
        pflc_play(class_answer_cels[((int)(short)a1)], (int)l_44);
    }
}

int class_question_get_answer(void)
{
    short l_1C;
    short l_18;

    if (key_down_a != 0) return 1;
    if (key_down_b != 0) return 2;
    if (key_down_c != 0) return 3;
    if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && mouse_buttons_prev != mouse_buttons) {
        if (func_00051991((int)(short)((int)(unsigned char)D_00199816), (int)(short)((int)(unsigned char)D_00199817), (int)&l_1C, (int)&l_18) != 0) {
            if (mouse_y > l_1C && mouse_y < l_18) return 1;
        }
        if (func_00051991((int)(short)((int)(unsigned char)D_00199817), (int)(short)((int)(unsigned char)D_00199818), (int)&l_1C, (int)&l_18) != 0) {
            if (mouse_y > l_1C && mouse_y < l_18) return 2;
        }
        if (func_00051991((int)(short)((int)(unsigned char)D_00199818), (int)(short)(*(short *)D_00190D66 + 48), (int)&l_1C, (int)&l_18) != 0) {
            if (mouse_y > l_1C && mouse_y < l_18) return 3;
        }
    }
    return 0;
}

int func_00051991(short a1, short a2, int a3, int a4)
{
    a1 = *(int *)&a1 * D_0012DA44;
    a2 = *(int *)&a2 * D_0012DA44;
    if (((int)(short)a1) >= (((int)(short)*(short *)D_00190D64) + 48) || (short)(short)*(int *)&a2 <= *(short *)D_00190D64) {
        return 0;
    }
    if ((short)(short)*(int *)&a1 < *(short *)D_00190D64) {
        *(short *)((char *)a3) = 135;
    } else {
        *(short *)((char *)a3) = (*(int *)&a1 - *(short *)D_00190D64) + 135;
    }
    if (((int)(short)a2) > (((int)(short)*(short *)D_00190D64) + 48)) {
        *(short *)((char *)a4) = 183;
    } else {
        *(short *)((char *)a4) = (*(int *)&a2 - *(short *)D_00190D64) + 135;
    }
    return 1;
}

int class_question_pick_class(void)
{
    short l_1C;
    int l_24;
    short l_18;

    disk_read_file((int)D_001753DB, D_00195C44);
    *(int *)&l_1C = D_00195C44 + 18;
    l_24 = ((((int)(unsigned char)D_0019981B) << 16) | (((int)(unsigned char)D_0019981A) << 8)) | ((int)(unsigned char)class_answer_counts);
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 48; (*(int *)&l_18)++) {
        if ((*(int *)(*(char **)&l_1C) & 16777215) == l_24) return ((int)(short)l_18) >> 2;
        *(int *)&l_1C += 3;
    }
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 18; (*(int *)&l_18)++) {
        if ((*(int *)(*(char **)&l_1C) & 16777215) == l_24) return (((int)(short)l_18) >> 2) + 12;
        *(int *)&l_1C += 3;
    }
    fatal_error((int)D_001753E7);
    return 0;
}
