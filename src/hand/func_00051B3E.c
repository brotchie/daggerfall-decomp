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
extern char xn_kbd_last_scancode;
extern void flc_show_frame(struct Snd *);
extern int flc_open(int, struct Snd *);
extern void flc_close(struct Snd *);
extern int flc_next_frame(struct Snd *);
extern int lseek(unsigned short, int, int);
extern int xn_kbd_wait_all_released();
extern int xn_mouse_poll_clamped();

int pflc_play(int file_name, struct Snd *s)
{
    int t0;

    if (flc_open(file_name, s) == 0)
        return 1;
    s->state = 255;
    while (mouse_buttons != 0)
        xn_mouse_poll_clamped();
    xn_kbd_wait_all_released();
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
            flc_show_frame(s);
            xn_mouse_poll_clamped();
            if (xn_kbd_last_scancode != 0 || (int)(unsigned char)(mouse_buttons & 3) != 0)
                goto out;
            while (*(int *)0x46c - t0 < s->delay) {
                xn_mouse_poll_clamped();
                if (xn_kbd_last_scancode != 0 || (int)(unsigned char)(mouse_buttons & 3) != 0)
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
        lseek(s->id, s->data, 0);
        s->count = 0;
    }
stop:
    s->state = 255;
    flc_close(s);
    xn_kbd_wait_all_released();
    return 0;
out:
    flc_close(s);
    xn_kbd_wait_all_released();
    return 1;
}
