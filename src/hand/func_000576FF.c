/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000576FF */
struct pair { unsigned char a, b; };
struct row { struct pair p[5]; };
struct cell { short v; short f2; };
extern short scratch_190d64;
extern int scratch_190ee4[];
extern char *scratch_buffer;
extern struct row D_00199868[];
extern struct cell itemmaker_slots[];
extern int itemmaker_param_excluded(short, short);
extern void list_popup_open(int *);

void itemmaker_show_param_list(int *names, short type)
{
    short i;
    unsigned short index;
    short count;
    short param;
    int saved;

    param = index = count = 0;
    while (*names != 0) {
        for (i = 0; i < 12; i++) {
            if (D_00199868[i].p[0].a == type && D_00199868[i].p[0].b == param
                || D_00199868[i].p[1].a == type && D_00199868[i].p[1].b == param
                || D_00199868[i].p[2].a == type && D_00199868[i].p[2].b == param
                || D_00199868[i].p[3].a == type && D_00199868[i].p[3].b == param
                || D_00199868[i].p[4].a == type && D_00199868[i].p[4].b == param)
                goto next;
        }
        saved = itemmaker_slots[scratch_190d64].v;
        itemmaker_slots[scratch_190d64].v = 100;
        if (itemmaker_param_excluded(type, param) != 0)
            itemmaker_slots[scratch_190d64].v = saved;
        else {
            itemmaker_slots[scratch_190d64].v = saved;
            scratch_buffer[count + 64000] = index;
            scratch_190ee4[count] = *names;
            count++;
        }
next:
        names++;
        index++;
        param++;
    }
    scratch_190ee4[count] = 0;
    list_popup_open(scratch_190ee4);
}
