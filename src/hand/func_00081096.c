/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00081096 */
struct mob { char pad[137]; int flags; };
struct cam { char pad; short yaw; short pitch; };
extern int D_000C5400;
extern int D_001940D4;
extern int D_001940DA;
extern int D_001940DB;
extern int D_001959B8;
extern int D_001959BC;
extern struct cam *D_00195A98;
extern struct cam *D_00195AA4;
extern struct mob *D_00195BE0;
extern unsigned char D_00195E7A;
extern char D_00195E7B;
extern char D_00195E7F;
extern short D_00195F64;
extern short D_00195F66;
extern char D_00196274;
extern char D_00196277;
extern char D_0019627D;
extern int func_00042F0F(int);
extern void func_0007EED8(void);

void func_00081096(void)
{
    short dy;
    short dx;

    if (D_00196274 || (D_001940D4 & 0x24))
        return;
    if (D_00195E7A == 1) {
        if (!D_00195E7F && !func_00042F0F(33)) {
            func_0007EED8();
            if (!D_00196274 && (D_00196277 || D_0019627D || (D_001940DB & 0x20) || (D_00195BE0->flags & 8) ? 1 : 0)) {
                dy = D_00195F64;
                dx = D_00195F66;
                if ((unsigned char)D_00195E7B & 0x80)
                    dx = -dx;
                D_00195AA4->yaw += dx;
                D_00195AA4->pitch += dy;
                if (dy > 2)
                    D_000C5400 = -8;
                else if (dy < -2)
                    D_000C5400 = 8;
                if (D_00195AA4->yaw < -256)
                    D_00195AA4->yaw = -256;
                else if (D_00195AA4->yaw > 256)
                    D_00195AA4->yaw = 256;
                D_00195A98->yaw = D_00195AA4->yaw;
                D_00195A98->pitch = D_00195AA4->pitch;
            }
        }
    } else if (D_001940DA & 0x40) {
        dy = D_00195F64;
        dx = D_00195F66;
        D_001959B8 += dx;
        D_001959BC += dy;
        if (D_001959B8 < -256)
            D_001959B8 = -256;
        else if (D_001959B8 > 256)
            D_001959B8 = 256;
        if (D_001959BC < -512)
            D_001959BC = -512;
        else if (D_001959BC > 512)
            D_001959BC = 512;
    }
}
