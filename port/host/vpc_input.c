/* vpc_input.c: the virtual PC's keyboard and mouse (port/include/port_vpc.h).

   Keyboard: each SDL key becomes the PC's scan code set 1 bytes (make; break = make | 80h;
   E0h before the extended keys), queued by the main thread and handed to the int 9 vector
   one byte an interrupt through port 60h, as the 8042 does. The default int 9 is the BIOS's:
   it keeps the shift flags at 0x417 and puts (scan code << 8 | ASCII) in the BIOS keyboard
   buffer that int 16h reads. XnGine installs its own int 9 (xn_kbd_install) and may chain to
   this one.

   Mouse: int 33h as a DOS mouse driver gives it. Motion is counted in mickeys (8 per 8
   pixels across, 16 down, the drivers' default); the position is in the driver's units, 640
   across mode 13h's 320 pixels, clamped to the ranges set by 07h and 08h. The host's motion is
   taken relative to the picture: crossing the 320 pixels moves 320 game pixels. A click in the
   window captures the mouse; Ctrl+G releases it. */
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_vpc.h"

/* ---- scan code set 1 ----------------------------------------------------------------------- */

/* SDL scancode -> set-1 make code; 0x100 marks an E0-prefixed key */
static unsigned short set1(SDL_Scancode s)
{
    if (s >= SDL_SCANCODE_A && s <= SDL_SCANCODE_Z) {
        static const unsigned char letters[26] = {
            0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
            0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C};
        return letters[s - SDL_SCANCODE_A];
    }
    if (s >= SDL_SCANCODE_1 && s <= SDL_SCANCODE_0)
        return (unsigned short)(0x02 + (s - SDL_SCANCODE_1));
    if (s >= SDL_SCANCODE_F1 && s <= SDL_SCANCODE_F10)
        return (unsigned short)(0x3B + (s - SDL_SCANCODE_F1));
    switch (s) {
    case SDL_SCANCODE_RETURN: return 0x1C;
    case SDL_SCANCODE_ESCAPE: return 0x01;
    case SDL_SCANCODE_BACKSPACE: return 0x0E;
    case SDL_SCANCODE_TAB: return 0x0F;
    case SDL_SCANCODE_SPACE: return 0x39;
    case SDL_SCANCODE_MINUS: return 0x0C;
    case SDL_SCANCODE_EQUALS: return 0x0D;
    case SDL_SCANCODE_LEFTBRACKET: return 0x1A;
    case SDL_SCANCODE_RIGHTBRACKET: return 0x1B;
    case SDL_SCANCODE_BACKSLASH: return 0x2B;
    case SDL_SCANCODE_SEMICOLON: return 0x27;
    case SDL_SCANCODE_APOSTROPHE: return 0x28;
    case SDL_SCANCODE_GRAVE: return 0x29;
    case SDL_SCANCODE_COMMA: return 0x33;
    case SDL_SCANCODE_PERIOD: return 0x34;
    case SDL_SCANCODE_SLASH: return 0x35;
    case SDL_SCANCODE_CAPSLOCK: return 0x3A;
    case SDL_SCANCODE_F11: return 0x57;
    case SDL_SCANCODE_F12: return 0x58;
    case SDL_SCANCODE_SCROLLLOCK: return 0x46;
    case SDL_SCANCODE_NUMLOCKCLEAR: return 0x45;
    case SDL_SCANCODE_LCTRL: return 0x1D;
    case SDL_SCANCODE_LSHIFT: return 0x2A;
    case SDL_SCANCODE_LALT: return 0x38;
    case SDL_SCANCODE_RSHIFT: return 0x36;
    case SDL_SCANCODE_RCTRL: return 0x11D;
    case SDL_SCANCODE_RALT: return 0x138;
    case SDL_SCANCODE_INSERT: return 0x152;
    case SDL_SCANCODE_DELETE: return 0x153;
    case SDL_SCANCODE_HOME: return 0x147;
    case SDL_SCANCODE_END: return 0x14F;
    case SDL_SCANCODE_PAGEUP: return 0x149;
    case SDL_SCANCODE_PAGEDOWN: return 0x151;
    case SDL_SCANCODE_UP: return 0x148;
    case SDL_SCANCODE_DOWN: return 0x150;
    case SDL_SCANCODE_LEFT: return 0x14B;
    case SDL_SCANCODE_RIGHT: return 0x14D;
    case SDL_SCANCODE_KP_DIVIDE: return 0x135;
    case SDL_SCANCODE_KP_ENTER: return 0x11C;
    case SDL_SCANCODE_KP_MULTIPLY: return 0x37;
    case SDL_SCANCODE_KP_MINUS: return 0x4A;
    case SDL_SCANCODE_KP_PLUS: return 0x4E;
    case SDL_SCANCODE_KP_PERIOD: return 0x53;
    case SDL_SCANCODE_KP_0: return 0x52;
    case SDL_SCANCODE_KP_1: return 0x4F;
    case SDL_SCANCODE_KP_2: return 0x50;
    case SDL_SCANCODE_KP_3: return 0x51;
    case SDL_SCANCODE_KP_4: return 0x4B;
    case SDL_SCANCODE_KP_5: return 0x4C;
    case SDL_SCANCODE_KP_6: return 0x4D;
    case SDL_SCANCODE_KP_7: return 0x47;
    case SDL_SCANCODE_KP_8: return 0x48;
    case SDL_SCANCODE_KP_9: return 0x49;
    default: return 0;
    }
}

/* the scan codes waiting for the 8042: the main thread puts, the interrupt thread takes */
#define KQ 256
static unsigned char kq[KQ];
static SDL_AtomicInt kq_head, kq_tail;
static volatile unsigned char kbd_data, port61;

static void kq_put(unsigned char b)
{
    int h = SDL_GetAtomicInt(&kq_head);
    if (((h + 1) & (KQ - 1)) == (SDL_GetAtomicInt(&kq_tail) & (KQ - 1)))
        return;                 /* full: the 8042 drops it too */
    kq[h & (KQ - 1)] = b;
    SDL_SetAtomicInt(&kq_head, (h + 1) & (KQ - 1));
}

static int kq_take(void)
{
    int t = SDL_GetAtomicInt(&kq_tail);
    unsigned char b;
    if (t == SDL_GetAtomicInt(&kq_head))
        return -1;
    b = kq[t];
    SDL_SetAtomicInt(&kq_tail, (t + 1) & (KQ - 1));
    return b;
}

void vpc_kbd_irq(void)
{
    int b = kq_take();
    vpc_handler h;

    if (b < 0)
        return;
    kbd_data = (unsigned char)b;
    h = vpc_getvect(9);
    if (h != NULL)
        h();
}

unsigned char vpc_kbd_inb(unsigned int port)
{
    if (port == 0x60)
        return kbd_data;
    if (port == 0x61)
        return port61;
    return SDL_GetAtomicInt(&kq_head) != SDL_GetAtomicInt(&kq_tail) ? 0x1D : 0x1C;    /* 64h */
}

void vpc_kbd_outb(unsigned int port, unsigned char v)
{
    if (port == 0x61)
        port61 = v;
}

/* ---- the BIOS's int 9 and keyboard buffer ----------------------------------------------------- */

#define BQ 16
static unsigned short bq[BQ];
static int bq_head, bq_tail;            /* under the interrupt lock */
static int e0_prefix;

static unsigned char ascii(unsigned char scan, int shift, int caps, int ctrl)
{
    static const char lower[] = "\0\0331234567890-=\b\tqwertyuiop[]\r\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 ";
    static const char upper[] = "\0\033!@#$%^&*()_+\b\tQWERTYUIOP{}\r\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";
    char c;

    if (scan >= sizeof lower - 1)
        return 0;
    c = shift ? upper[scan] : lower[scan];
    if (caps && c >= 'a' && c <= 'z')
        c -= 'a' - 'A';
    else if (caps && c >= 'A' && c <= 'Z')
        c += 'a' - 'A';
    if (ctrl && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
        c = (char)((c | 0x20) - 'a' + 1);
    return (unsigned char)c;
}

void vpc_bios_int9(void)
{
    unsigned char b = kbd_data, scan = b & 0x7F;
    unsigned char *flags = port_low_memory + 0x417;
    int brk = b & 0x80, ext = e0_prefix;

    if (b == 0xE0) {
        e0_prefix = 1;
        return;
    }
    e0_prefix = 0;
    switch (scan) {
    case 0x36: *flags = brk ? *flags & ~0x01 : *flags | 0x01; return;     /* right shift */
    case 0x2A: *flags = brk ? *flags & ~0x02 : *flags | 0x02; return;     /* left shift */
    case 0x1D: *flags = brk ? *flags & ~0x04 : *flags | 0x04; return;     /* ctrl */
    case 0x38: *flags = brk ? *flags & ~0x08 : *flags | 0x08; return;     /* alt */
    case 0x46: if (!brk) *flags ^= 0x10; return;                         /* scroll lock */
    case 0x45: if (!brk) *flags ^= 0x20; return;                         /* num lock */
    case 0x3A: if (!brk) *flags ^= 0x40; return;                         /* caps lock */
    }
    if (brk)
        return;
    {
        unsigned short w = (unsigned short)(scan << 8 |
            (ext ? 0 : ascii(scan, *flags & 0x03, *flags & 0x40, *flags & 0x04)));
        int next = (bq_head + 1) % BQ;
        if (next != bq_tail) {
            bq[bq_head] = w;
            bq_head = next;
        }
    }
}

int vpc_bios_key(int remove)
{
    int k = -1;

    vpc_irq_lock();
    if (bq_tail != bq_head) {
        k = bq[bq_tail];
        if (remove)
            bq_tail = (bq_tail + 1) % BQ;
    }
    vpc_irq_unlock();
    return k;
}

/* ---- the mouse driver ---------------------------------------------------------------------------- */

static struct {
    double x, y;                        /* driver units */
    double mx, my;                      /* mickeys not yet read */
    int buttons;
    int minx, maxx, miny, maxy;
    int ratio_x, ratio_y;               /* mickeys per 8 pixels */
    int sens_x, sens_y, threshold;
    int press_count[3], release_count[3];
    int press_x[3], press_y[3], release_x[3], release_y[3];
    int shown;
} mouse;

static int captured;
/* the mouse's state: the main thread's events change it, the game's int 33h reads it */
static SDL_Mutex *mouse_lock;

/* driver units across a mode 13h pixel: 2 in the default 0-639 range; 1 once the program
   sets a range of a screen's width in pixels (XnGine's 0-310) */
static int units_x(void)
{
    return mouse.maxx - mouse.minx >= 400 ? 2 : 1;
}

static void mouse_reset(void)
{
    memset(&mouse, 0, sizeof mouse);
    mouse.maxx = 639;
    mouse.maxy = 199;
    mouse.x = 320;
    mouse.y = 100;
    mouse.ratio_x = 8;
    mouse.ratio_y = 16;
    mouse.sens_x = mouse.sens_y = 50;
    mouse.threshold = 50;
    mouse.shown = -1;
}

void vpc_input_init(void)
{
    if (mouse_lock == NULL)
        mouse_lock = SDL_CreateMutex();
    mouse_reset();
}

static double clampd(double v, int lo, int hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

/* the window's size of the picture, to turn the host's motion into game pixels */
static void picture_scale(SDL_Window *w, double *sx, double *sy)
{
    int ww = 960, wh = 720;
    double pw, ph;

    if (w != NULL)
        SDL_GetWindowSize(w, &ww, &wh);
    /* 4:3, letterboxed */
    if (ww * 3 > wh * 4) {
        ph = wh;
        pw = wh * 4.0 / 3.0;
    } else {
        pw = ww;
        ph = ww * 3.0 / 4.0;
    }
    *sx = 320.0 / pw;
    *sy = 200.0 / ph;
}

/* relative mode where the window allows it; the clicks go to the game either way */
static void set_capture(SDL_Window *w, int on)
{
    if (w != NULL)
        SDL_SetWindowRelativeMouseMode(w, on);
    captured = on;
}

static int handle_event(const void *ev);

int vpc_input_handle_event(const void *ev)
{
    int r;

    SDL_LockMutex(mouse_lock);
    r = handle_event(ev);
    SDL_UnlockMutex(mouse_lock);
    return r;
}

static int handle_event(const void *ev)
{
    const SDL_Event *e = ev;
    SDL_Window *w;

    switch (e->type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
        unsigned short code;
        if (e->type == SDL_EVENT_KEY_DOWN && e->key.scancode == SDL_SCANCODE_G &&
            (e->key.mod & SDL_KMOD_CTRL)) {
            set_capture(SDL_GetWindowFromID(e->key.windowID), 0);
            return 1;
        }
        code = set1(e->key.scancode);
        if (code == 0)
            return 1;
        if (code & 0x100)
            kq_put(0xE0);
        kq_put((unsigned char)((code & 0x7F) | (e->type == SDL_EVENT_KEY_UP ? 0x80 : 0)));
        return 1;
    }
    case SDL_EVENT_MOUSE_MOTION: {
        double sx, sy, gx, gy;
        w = SDL_GetWindowFromID(e->motion.windowID);
        picture_scale(w, &sx, &sy);
        gx = e->motion.xrel * sx;               /* game pixels */
        gy = e->motion.yrel * sy;
        /* the driver's units (units_x); mickeys at the drivers' ratio */
        mouse.mx += gx * units_x() * mouse.ratio_x / 8.0;
        mouse.my += gy * mouse.ratio_y / 8.0;
        mouse.x = clampd(mouse.x + gx * units_x(), mouse.minx, mouse.maxx);
        mouse.y = clampd(mouse.y + gy, mouse.miny, mouse.maxy);
        return 1;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        int b = e->button.button == SDL_BUTTON_LEFT ? 0 : e->button.button == SDL_BUTTON_RIGHT ? 1
              : e->button.button == SDL_BUTTON_MIDDLE ? 2 : -1;
        w = SDL_GetWindowFromID(e->button.windowID);
        if (!captured && e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            set_capture(w, 1);              /* the first click takes the mouse */
            return 1;
        }
        if (b < 0)
            return 1;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            mouse.buttons |= 1 << b;
            mouse.press_count[b]++;
            mouse.press_x[b] = (int)mouse.x;
            mouse.press_y[b] = (int)mouse.y;
        } else {
            mouse.buttons &= ~(1 << b);
            mouse.release_count[b]++;
            mouse.release_x[b] = (int)mouse.x;
            mouse.release_y[b] = (int)mouse.y;
        }
        return 1;
    }
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        set_capture(SDL_GetWindowFromID(e->window.windowID), 0);
        return 1;
    }
    return 0;
}

#define AXr(r) ((unsigned int)(r)->eax & 0xFFFF)
#define BXr(r) ((unsigned int)(r)->ebx & 0xFFFF)
#define CXr(r) ((int)(short)((r)->ecx & 0xFFFF))
#define DXr(r) ((int)(short)((r)->edx & 0xFFFF))
#define SET(reg, v) ((reg) = ((reg) & ~0xFFFFul) | ((unsigned long)(v) & 0xFFFF))

static void mouse_service(struct vpc_regs *r);

void vpc_mouse_service(struct vpc_regs *r)
{
    SDL_LockMutex(mouse_lock);
    mouse_service(r);
    SDL_UnlockMutex(mouse_lock);
}

static void mouse_service(struct vpc_regs *r)
{
    int b;

    switch (AXr(r)) {
    case 0x0000:                /* reset: AX FFFFh installed, BX buttons */
    case 0x0021:
        mouse_reset();
        SET(r->eax, 0xFFFF);
        SET(r->ebx, 3);
        break;
    case 0x0001:                /* show / hide the driver's cursor (XnGine draws its own) */
        if (mouse.shown < 0)
            mouse.shown++;
        break;
    case 0x0002:
        mouse.shown--;
        break;
    case 0x0003:                /* BX buttons, CX x, DX y */
        SET(r->ebx, mouse.buttons);
        SET(r->ecx, (int)mouse.x);
        SET(r->edx, (int)mouse.y);
        break;
    case 0x0004:                /* set the position: CX, DX */
        mouse.x = clampd(CXr(r), mouse.minx, mouse.maxx);
        mouse.y = clampd(DXr(r), mouse.miny, mouse.maxy);
        break;
    case 0x0005:                /* button BX's presses since the last call: AX the buttons,
                                   BX the count, CX DX where the last one was */
        b = (int)BXr(r) > 2 ? 0 : (int)BXr(r);
        SET(r->eax, mouse.buttons);
        SET(r->ebx, mouse.press_count[b]);
        SET(r->ecx, mouse.press_x[b]);
        SET(r->edx, mouse.press_y[b]);
        mouse.press_count[b] = 0;
        break;
    case 0x0006:                /* the same for releases */
        b = (int)BXr(r) > 2 ? 0 : (int)BXr(r);
        SET(r->eax, mouse.buttons);
        SET(r->ebx, mouse.release_count[b]);
        SET(r->ecx, mouse.release_x[b]);
        SET(r->edx, mouse.release_y[b]);
        mouse.release_count[b] = 0;
        break;
    case 0x0007:                /* the x range: CX..DX */
        mouse.minx = CXr(r) < DXr(r) ? CXr(r) : DXr(r);
        mouse.maxx = CXr(r) < DXr(r) ? DXr(r) : CXr(r);
        mouse.x = clampd(mouse.x, mouse.minx, mouse.maxx);
        break;
    case 0x0008:                /* the y range */
        mouse.miny = CXr(r) < DXr(r) ? CXr(r) : DXr(r);
        mouse.maxy = CXr(r) < DXr(r) ? DXr(r) : CXr(r);
        mouse.y = clampd(mouse.y, mouse.miny, mouse.maxy);
        break;
    case 0x000B: {              /* mickeys since the last call: CX across, DX down */
        int mx = (int)mouse.mx, my = (int)mouse.my;
        mouse.mx -= mx;
        mouse.my -= my;
        SET(r->ecx, mx);
        SET(r->edx, my);
        break;
    }
    case 0x000F:                /* mickeys per 8 pixels: CX, DX */
        if (CXr(r) > 0)
            mouse.ratio_x = CXr(r);
        if (DXr(r) > 0)
            mouse.ratio_y = DXr(r);
        break;
    case 0x001A:                /* sensitivity: BX, CX, DX (kept and given back) */
        mouse.sens_x = (int)BXr(r);
        mouse.sens_y = CXr(r);
        mouse.threshold = DXr(r);
        break;
    case 0x001B:
        SET(r->ebx, mouse.sens_x);
        SET(r->ecx, mouse.sens_y);
        SET(r->edx, mouse.threshold);
        break;
    case 0x0024:                /* version: BX 6.26, CH type (PS/2), CL IRQ */
        SET(r->ebx, 0x0626);
        SET(r->ecx, 0x0400);
        break;
    default:
        fprintf(stderr, "port: int 33h ax=%04X not implemented\n", AXr(r));
    }
}

/* ---- the scripted driver's input (vpc_script.c) ------------------------------------------- */

void vpc_input_key(int sdl_scancode, int down)
{
    unsigned short code = set1((SDL_Scancode)sdl_scancode);

    if (code == 0)
        return;
    if (code & 0x100)
        kq_put(0xE0);
    kq_put((unsigned char)((code & 0x7F) | (down ? 0 : 0x80)));
}

/* the pointer to (x, y) in mode 13h pixels; button b (0 left, 1 right) down or up */
void vpc_input_mouse_to(int x, int y)
{
    SDL_LockMutex(mouse_lock);
    double dx = x * units_x() - mouse.x, dy = y - mouse.y;

    mouse.mx += dx * mouse.ratio_x / 8.0;
    mouse.my += dy * mouse.ratio_y / 8.0;
    mouse.x = clampd(x * units_x(), mouse.minx, mouse.maxx);
    mouse.y = clampd(y, mouse.miny, mouse.maxy);
    SDL_UnlockMutex(mouse_lock);
}

void vpc_input_mouse_button(int b, int down)
{
    SDL_LockMutex(mouse_lock);
    if (down) {
        mouse.buttons |= 1 << b;
        mouse.press_count[b]++;
        mouse.press_x[b] = (int)mouse.x;
        mouse.press_y[b] = (int)mouse.y;
    } else {
        mouse.buttons &= ~(1 << b);
        mouse.release_count[b]++;
        mouse.release_x[b] = (int)mouse.x;
        mouse.release_y[b] = (int)mouse.y;
    }
    SDL_UnlockMutex(mouse_lock);
}
