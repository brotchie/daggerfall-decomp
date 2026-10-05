/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008C2D7 */
extern unsigned char D_0012B508;
extern short D_0012DA44;
extern short D_00142928;
extern short D_0014292C;
extern char D_00176E2C[];        /* __FILE__ */
extern char D_00190B44[];
extern char *inpstr_text;
extern short inpstr_max_length;
extern short inpstr_cursor;
extern void text_draw(char *, short, short);
extern int inpstr_read_key(void);
extern int inpstr_handle_key(unsigned char);
extern int inpstr_text_width(char *, short);
extern void mc_strncpy(char *, char *, int, char *, int);
extern short func_000A0DF4(char *);
extern void func_000CD308(void);
extern void func_000CD31A(void);
extern void func_000CDD81(int);
extern void func_0012B2EB(void);
extern void func_00142790(void);
extern void func_00144D00(short, short, short, short);
extern void func_001531F0(short, short, short, short);

int inpstr_edit(char *a1, short a2, short a3, short a4, short a5, short a6)
{
    short key;
    unsigned char old;
    int r;

    func_0012B2EB();
    func_00142790();
    inpstr_text = a1;
    mc_strncpy(D_00190B44, inpstr_text, 160, D_00176E2C, 56);
    inpstr_cursor = func_000A0DF4(inpstr_text);
    inpstr_max_length = a6;
    for (;;) {
        key = inpstr_read_key();
        if (key == 0) {
            old = D_0012B508;
            D_0012B508 = 0;
            func_00144D00(a2, a3, a4, a5);
            D_0012B508 = 12;
            D_00142928 = a2 + inpstr_text_width(inpstr_text, inpstr_cursor);
            D_0014292C = a3;
            if (*(int *)0x46c & 32)
                func_001531F0(D_00142928, D_0014292C, D_00142928, D_0014292C + D_0012DA44 - 1);
            D_0012B508 = old;
            text_draw(inpstr_text, a2, a3);
            func_000CDD81(1);
            func_000CD308();
            func_000CD31A();
            continue;
        }
        r = inpstr_handle_key(key);
        if (r == 32768) {
            mc_strncpy(inpstr_text, D_00190B44, 4, D_00176E2C, 83);
            return 0;
        }
        if (r != 0x87654321)
            return r;
    }
}
