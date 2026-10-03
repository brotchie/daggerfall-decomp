/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00069E3C */
extern char D_001A3F60[];
extern int D_001A3F74;
extern int D_001A3F78;
extern int D_001A3F7C;
extern int D_001A3F90;
extern int D_001A3F94;
extern int D_001A3F98;
extern int D_001A3F9C;
extern void func_00099490(char *);

void func_00069E3C(void)
{
    func_00099490(D_001A3F60);
    D_001A3F98 = (D_001A3F78 - D_001A3F74) << 2;
    D_001A3F90 = (D_001A3F7C << 2) + D_001A3F98;
    D_001A3F94 = D_001A3F78 << 2;
    D_001A3F9C = D_001A3F98;
}
