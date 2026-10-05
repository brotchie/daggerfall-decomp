/* question.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char mouse_buttons[];
extern char mouse_y[];
extern char D_0012DA44[];
extern char key_down_a[];
extern char key_down_c[];
extern char key_down_b[];
extern char screen_buffer[];
extern char D_0017539B[];
extern char D_001753DB[];
extern char D_001753E7[];
extern char class_answer_cels[];
extern char text_buffer[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00190D68[];
extern char frame_counter[];
extern char D_00195C44[];
extern char mouse_buttons_prev[];
extern char D_001997B5[];
extern char class_questions_asked[];
extern char D_00199816[];
extern char D_00199817[];
extern char D_00199818[];
extern char class_answer_counts[];
extern char D_0019981A[];
extern char D_0019981B[];

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
L514B7:;
    if (memchr((int)class_questions_asked, (int)(short)*(short *)&l_1C, 10) == 0) goto L514E7;
    l_1C = (rand() % 40) + 1;
    goto L514B7;
L514E7:;
    func_000CE49E((int)class_questions_asked, (int)(short)*(short *)&l_1C);
    l_28 = text_rsc_load(9000, 8, 0);
    l_2C = l_28;
    *(short *)D_00190D68 = l_1C;
L51518:;
    l_28 = memchr(l_28, 123, 32000) + 1;
    func_000CE46D((int)text_buffer, l_28, 46);
    if (atoi((int)text_buffer) != ((int)(short)*(short *)&l_1C)) goto L51518;
    l_28 += 2;
    *(signed char *)((char *)memchr(l_28, 123, 32000)) = 0;
    l_20 = mc_malloc(64000, (int)D_0017539B, 176);
    mc_memcpy(l_20, *(int *)screen_buffer, 64000, (int)D_0017539B, 178, 4);
    mc_memset(*(int *)screen_buffer, 0, 64000, (int)D_0017539B, 179, 4);
L515BE:;
    if (*(signed char *)((char *)l_28) == 0) goto L515CF;
    if (*(signed char *)((char *)l_28 - 1) != 0) goto L515D1;
L515CF:;
    goto L51643;
L515D1:;
    func_000CE46D((int)text_buffer, l_28, 252);
    l_28 += func_000A0DF4((int)text_buffer) + 1;
    l_30 = strchr((int)text_buffer, 41);
    if (l_30 == 0) goto L5161A;
    *(signed char *)(D_001997B5 + ((int)(unsigned char)*(signed char *)((char *)l_30 - 1))) = *(signed char *)&l_18;
L5161A:;
    text_draw((int)text_buffer, 20, ((int)(short)*(short *)D_0012DA44) * ((int)(short)*(short *)&l_18));
    (*(short *)&l_18)++;
    goto L515BE;
L51643:;
    mc_memcpy(*(int *)D_00195C44, *(int *)screen_buffer, 64000, (int)D_0017539B, 193, 4);
    mc_memcpy(*(int *)screen_buffer, l_20, 64000, (int)D_0017539B, 194, 4);
    if (l_20 == 0) goto L51691;
    if (l_20 != (-1751672937)) goto L51693;
L51691:;
    goto L516AC;
L51693:;
    mc_free(l_20, (int)D_0017539B, 195);
    l_20 = -1751672937;
L516AC:;
    *(short *)D_00190D66 = (*(short *)D_0012DA44 * l_18) - 52;
    class_question_scroll(a1, 0);
    if (l_2C == 0) goto L516D8;
    if (l_2C != (-1751672937)) goto L516DA;
L516D8:;
    return;
L516DA:;
    mc_free(l_2C, (int)D_0017539B, 199);
    l_2C = -1751672937;
}

void func_0005175B(int a1, short a2)
{
    if (a2 >= 0) goto L5177F;
    if (*(short *)D_00190D64 != 0) goto L51781;
L5177F:;
    goto L5178D;
L51781:;
    *(short *)D_00190D64 += *(int *)&a2;
    goto L517AF;
L5178D:;
    if (a2 <= 0) goto L517A3;
    if (*(short *)D_00190D64 < *(short *)D_00190D66) goto L517A5;
L517A3:;
    goto L517AF;
L517A5:;
    *(short *)D_00190D64 += *(int *)&a2;
L517AF:;
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
    pflc_play(*(int *)(class_answer_cels + (((int)(short)a1) << 2)), (int)l_44);
}
}

int class_question_get_answer(void)
{
    short l_1C;
    short l_18;

    if (*(signed char *)key_down_a == 0) goto L5185E;
    return 1;
L5185E:;
    if (*(signed char *)key_down_b == 0) goto L51873;
    return 2;
L51873:;
    if (*(signed char *)key_down_c == 0) goto L51888;
    return 3;
L51888:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L518A5;
    if (*(signed char *)mouse_buttons_prev != *(signed char *)mouse_buttons) goto L518AA;
L518A5:;
    goto L5197D;
L518AA:;
    if (func_00051991((int)(short)((int)(unsigned char)*(signed char *)D_00199816), (int)(short)((int)(unsigned char)*(signed char *)D_00199817), (int)&l_1C, (int)&l_18) == 0) goto L518F1;
    if (*(short *)mouse_y <= l_1C) goto L518E3;
    if (*(short *)mouse_y < l_18) goto L518E5;
L518E3:;
    goto L518F1;
L518E5:;
    return 1;
L518F1:;
    if (func_00051991((int)(short)((int)(unsigned char)*(signed char *)D_00199817), (int)(short)((int)(unsigned char)*(signed char *)D_00199818), (int)&l_1C, (int)&l_18) == 0) goto L51935;
    if (*(short *)mouse_y <= l_1C) goto L5192A;
    if (*(short *)mouse_y < l_18) goto L5192C;
L5192A:;
    goto L51935;
L5192C:;
    return 2;
L51935:;
    if (func_00051991((int)(short)((int)(unsigned char)*(signed char *)D_00199818), (int)(short)(*(short *)D_00190D66 + 48), (int)&l_1C, (int)&l_18) == 0) goto L5197D;
    if (*(short *)mouse_y <= l_1C) goto L51972;
    if (*(short *)mouse_y < l_18) goto L51974;
L51972:;
    goto L5197D;
L51974:;
    return 3;
L5197D:;
    return 0;
}

int func_00051991(short a1, short a2, int a3, int a4)
{
    a1 = *(int *)&a1 * *(short *)D_0012DA44;
    a2 = *(int *)&a2 * *(short *)D_0012DA44;
    if (((int)(short)a1) >= (((int)(short)*(short *)D_00190D64) + 48)) goto L519E2;
    if ((short)(short)*(int *)&a2 > *(short *)D_00190D64) goto L519EB;
L519E2:;
    return 0;
L519EB:;
    if ((short)(short)*(int *)&a1 >= *(short *)D_00190D64) goto L51A01;
    *(short *)((char *)a3) = 135;
    goto L51A17;
L51A01:;
    *(short *)((char *)a3) = (*(int *)&a1 - *(short *)D_00190D64) + 135;
L51A17:;
    if (((int)(short)a2) <= (((int)(short)*(short *)D_00190D64) + 48)) goto L51A33;
    *(short *)((char *)a4) = 183;
    goto L51A49;
L51A33:;
    *(short *)((char *)a4) = (*(int *)&a2 - *(short *)D_00190D64) + 135;
L51A49:;
    return 1;
}

int class_question_pick_class(void)
{
    short l_1C;
    int l_24;
    short l_18;

    disk_read_file((int)D_001753DB, *(int *)D_00195C44);
    *(int *)&l_1C = *(int *)D_00195C44 + 18;
    l_24 = ((((int)(unsigned char)*(signed char *)D_0019981B) << 16) | (((int)(unsigned char)*(signed char *)D_0019981A) << 8)) | ((int)(unsigned char)*(signed char *)class_answer_counts);
    *(int *)&l_18 = 0;
L51AAE:;
    if (((int)(short)l_18) < 48) goto L51AC1;
    goto L51AE2;
L51AB9:;
    (*(int *)&l_18)++;
    goto L51AAE;
L51AC1:;
    if ((*(int *)(*(char **)&l_1C) & 16777215) != l_24) goto L51ADC;
    return ((int)(short)l_18) >> 2;
L51ADC:;
    *(int *)&l_1C += 3;
    goto L51AB9;
L51AE2:;
    *(int *)&l_18 = 0;
L51AE9:;
    if (((int)(short)l_18) < 18) goto L51AFC;
    goto L51B20;
L51AF4:;
    (*(int *)&l_18)++;
    goto L51AE9;
L51AFC:;
    if ((*(int *)(*(char **)&l_1C) & 16777215) != l_24) goto L51B1A;
    return (((int)(short)l_18) >> 2) + 12;
L51B1A:;
    *(int *)&l_1C += 3;
    goto L51AF4;
L51B20:;
    fatal_error((int)D_001753E7);
    return 0;
}
