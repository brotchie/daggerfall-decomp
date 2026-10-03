/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005DB95 */
#pragma pack(1)
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct Obj { char pad0[3]; short angle; char pad1[2]; int x; int y; int z; };
extern int D_000C23BC;
extern char *D_00143550;
extern char D_00175898[];
extern struct Obj *D_00195A7C;
extern struct Obj *D_00195AA4;
extern unsigned short *D_00195BF8;
extern char *D_00195D88;
extern struct Img *D_00195D9C;
extern char D_00196274;
extern int func_00062EF7(int, int, int *);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern int func_000C808D();
extern int func_00144F68();

void func_0005DB95(int a1)
{
    int i;
    int off;
    int dir;
    int dist;
    int pos;

    if (a1 == 0) {
        if ((int)(unsigned short)(*D_00195BF8 & 1) == 0)
            return;
        if (D_00196274 != 0)
            return;
        func_00144F68(D_00195D9C->x, D_00195D9C->y, D_00195D9C->w, D_00195D9C->h, D_00195D9C->data);
        off = ((short)(D_00195AA4->angle & 0x7ff) << 5) / 256;
        for (i = 185; i <= 197; i++)
            func_000A1023(D_00143550 + (i * 320 + 253), (i - 185) * 322 + (D_00195D88 + off), 65, D_00175898, 440, 4);
        if (D_00195A7C != 0) {
            dir = func_00062EF7(D_00195AA4->angle, func_000C808D(D_00195AA4->x, D_00195AA4->z, D_00195A7C->x, D_00195A7C->z), &dist);
            dir = (dir << 5) / 256;
            if (dir > 32)
                dir = 32;
            pos = dir * dist + 57885;
            D_00143550[pos - 2] = D_00143550[pos - 1] = D_00143550[pos] = D_00143550[pos + 1] = D_00143550[pos + 2] = 245;
            pos += 320;
            D_00143550[pos - 1] = D_00143550[pos] = D_00143550[pos + 1] = 245;
            pos += 320;
            D_00143550[pos] = 245;
        }
        return;
    }
    func_00144F68(D_00195D9C->x - 250, D_00195D9C->y - 11, D_00195D9C->w, D_00195D9C->h, D_00195D9C->data);
    off = ((D_000C23BC & 2047) << 5) / 256;
    for (i = 174; i <= 186; i++)
        func_000A1023(D_00143550 + (i * 320 + 3), (i - 174) * 322 + (D_00195D88 + off), 65, D_00175898, 460, 4);
}
