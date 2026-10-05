/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00051B3E */
#pragma pack(1)
struct Snd {
    char pad0[2];
    unsigned short id;          /* 0x02 */
    unsigned short step;        /* 0x04 */
    short count;                /* 0x06 */
    unsigned short delay;       /* 0x08 */
    int data;                   /* 0x0a */
    char pad1[0x2a - 0x0e];
    unsigned char state;        /* 0x2a */
    unsigned char loops;        /* 0x2b */
};
extern unsigned char mouse_buttons;
extern char D_00142307;
extern void func_00051CF9(struct Snd *);
extern int flc_open(int, struct Snd *);
extern void flc_close(struct Snd *);
extern int flc_next_frame(struct Snd *);
extern int func_000A006E(unsigned short, int, int);
extern int func_000CE92C();
extern int func_0012B136();

int pflc_play(int a1, struct Snd *s)
{
    int t0;

    if (flc_open(a1, s) == 0)
        return 1;
    s->state = 255;
    while (mouse_buttons != 0)
        func_0012B136();
    func_000CE92C();
    if (s->step > 256)
        s->step -= 16;
    for (;;) {
        s->count += s->step;
        if (s->loops == 0)
            s->count--;
        while (s->count-- != 0) {
            t0 = *(int *)0x46c;
            if (flc_next_frame(s) != 0)
                break;
            func_00051CF9(s);
            func_0012B136();
            if (D_00142307 != 0 || (int)(unsigned char)(mouse_buttons & 3) != 0)
                goto out;
            while (*(int *)0x46c - t0 < s->delay) {
                func_0012B136();
                if (D_00142307 != 0 || (int)(unsigned char)(mouse_buttons & 3) != 0)
                    goto out;
            }
        }
        if (s->loops == 0)
            goto out;
        if (s->loops != 255) {
            s->loops--;
            if (s->loops == 0)
                goto stop;
        }
        func_000A006E(s->id, s->data, 0);
        s->count = 0;
    }
stop:
    s->state = 255;
    flc_close(s);
    func_000CE92C();
    return 0;
out:
    flc_close(s);
    func_000CE92C();
    return 1;
}
