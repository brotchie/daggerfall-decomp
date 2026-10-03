/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001B06F */
struct Node {
    char pad[0x50];
    struct Node *next;          /* 0x50 */
    struct Node *child;         /* 0x54 */
    struct Node *parent;        /* 0x58 */
};
extern int D_00195B84;
extern struct Node *D_0019670C;
extern struct Node *D_0019671C;
extern unsigned char func_0001AFD5(struct Node *);

int func_0001B06F(struct Node *a, struct Node *b)
{
    struct Node *save;

    save = a;
    D_0019671C = b;
    D_0019670C = a->parent;
    D_00195B84 = 0;
    if ((unsigned char)(func_0001AFD5(b->child) & 1))
        return 3;
    if (a->parent != 0) {
        a = a->parent->child;
        while (a != 0) {
            if (a == b)
                return 2;
            a = a->next;
        }
    }
    a = save;
    if (a->parent != 0) {
        a = a->parent;
        while (a != 0) {
            if (a == b)
                return 1;
            a = a->parent;
        }
    }
    return 0;
}
