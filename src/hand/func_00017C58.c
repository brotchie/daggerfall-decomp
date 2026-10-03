/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00017C58 */
struct rec26 {
    char pad0[18];
    unsigned short id;          /* 0x12 */
    char pad14[4];
    unsigned char kind;         /* 0x18 */
    char pad19;
};
struct hdr {
    char pad0[41];
    unsigned short count;       /* 0x29 */
    struct rec26 *recs;         /* 0x2b */
};
extern struct hdr *D_00195BDC;

int func_00017C58(short a1, int a2)
{
    struct rec26 *p;
    int i;

    p = D_00195BDC->recs;
    for (i = 0; i < D_00195BDC->count; i++, p++) {
        if (a2 == 0) {
            if (p->id == a1)
                return 1;
        } else {
            if (p->kind == a1)
                return 1;
        }
    }
    return 0;
}
