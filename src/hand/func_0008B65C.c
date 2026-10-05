/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008B65C */
extern char D_00176D98[];
extern char D_00176DA2[];
extern char namegen_part_name[];
extern char namegen_syllable[];
extern short namegen_file;
extern void namegen_read_part(short, short);
extern int func_000A1054();

char *name_generate_surname(unsigned char bank, unsigned char female)
{
    short part;

    namegen_part_name[0] = 0;
    switch (bank) {
    case 1:
    case 8:
    case 9:
    case 10:
        break;
    case 2:
        part = 0;
        namegen_read_part(namegen_file, part);
        func_000A1054(namegen_part_name, namegen_syllable, D_00176D98, 126, 30);
        namegen_read_part(namegen_file, part + 1);
        func_000A1054(namegen_part_name, namegen_syllable, D_00176D98, 128, 30);
        func_000A1054(namegen_part_name, D_00176DA2, D_00176D98, 129, 30);
        break;
    default:
        part = 4;
        namegen_read_part(namegen_file, part);
        func_000A1054(namegen_part_name, namegen_syllable, D_00176D98, 134, 30);
        namegen_read_part(namegen_file, part + 1);
        func_000A1054(namegen_part_name, namegen_syllable, D_00176D98, 136, 30);
        break;
    }
    return namegen_part_name;
}
