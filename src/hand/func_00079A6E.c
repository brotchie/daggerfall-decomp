/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00079A6E */
struct thing {
    unsigned char type;
    char pad1[30];
    int id;                     /* 31 */
    char pad23[3];
    char f38;                   /* 38 */
    int f39;                    /* 39 */
    char pad2b[4];
    struct thing *f47;          /* 47 */
    struct thing *f51;          /* 51 */
    char pad37[12];
    struct thing *f67;          /* 67 */
};
extern char D_00176884[];        /* __FILE__ */
extern char D_0017688F[];
extern struct thing *D_00195C44;
extern int save_file_handle;
extern void fatal_error(struct thing *);
extern void save_unlink_character(struct thing *);
extern int write(int, void *, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(void *, char *, ...);

int savetree_write_record(struct thing *a1)
{
    struct thing *buf;
    int len;
    int unused1;
    int unused2;

    buf = D_00195C44;
    len = *(int *)((char *)a1 - 6);
    if (len == 0) {
        func_000A0ED9(84, D_00176884);
        mc_sprintf(D_00195C44, D_0017688F);
        fatal_error(D_00195C44);
    }
    write(save_file_handle, &len, 4);
    mc_memcpy(buf, a1, len, D_00176884, 90, 4);
    if (buf->f51 != 0) {
        if (buf->f38 != 0)
            buf->f51 = (struct thing *)buf->f51->id;
        else
            buf->f51 = 0;
    }
    switch (a1->type) {
    case 3:
    case 18:
    case 44:
        save_unlink_character(buf);
        break;
    case 9:
        if (a1->f67->type == 3 || a1->f67->type == 18)
            buf->f47 = (struct thing *)buf->f47->id;
        break;
    }
    if (a1->f67 != 0) {
        *(int *)&buf->f67 = a1->f67->type;
        buf->f39 = a1->f67->id;
    }
    return write(save_file_handle, buf, len) != len ? 1 : 0;
}
