/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CF09 */
extern signed char mouse_buttons;
extern int xn_gfx_wait_vretrace_start();
extern int xn_gfx_wait_vretrace_end();
extern int xn_mouse_poll_clamped();
extern char xn_kbd_read_key();

int wait_frames_or_input(short a1)
{
    short l_18;
    while (a1--) {
        xn_mouse_poll_clamped();
        if (*((char *)&mouse_buttons) != 0) return 1;
        if (xn_kbd_read_key() != 0) return 1;
        xn_gfx_wait_vretrace_start();
        xn_gfx_wait_vretrace_end();
    }
    return 0;
}
