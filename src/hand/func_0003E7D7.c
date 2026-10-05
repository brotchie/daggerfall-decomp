/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003E7D7 */
extern char D_00170D55[];
extern char D_00170DA7[];
extern signed char text_buffer[];
extern char qrc_name[];
extern char D_001910EC[];
extern int text_rsc_file;
extern char *text_rsc_load(int, int, int);
extern int disk_open_data(char *);
extern void close(int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, ...);

char *text_qrc_load(char *name, short message_id, short unused, short width)
{
    short saved_file;
    char *text;

    saved_file = text_rsc_file;
    mc_memcpy(qrc_name, name, 8, D_00170D55, 568, 2048);
    *D_001910EC = 0;
    mc_set_location(570, D_00170D55);
    mc_sprintf(((char *)text_buffer), D_00170DA7, qrc_name);
    if ((text_rsc_file = disk_open_data(((char *)text_buffer))) > 0) {
        text = text_rsc_load(message_id, 0, width);
        close(text_rsc_file);
        text_rsc_file = saved_file;
        return text;
    }
    text_rsc_file = saved_file;
    return 0;
}
