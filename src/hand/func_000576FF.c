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

void itemmaker_show_param_list(int *a1, short a2)
{
    short i;
    unsigned short c;
    short cnt;
    short k;
    int sv;

    k = c = cnt = 0;
    while (*a1 != 0) {
        for (i = 0; i < 12; i++) {
            if (D_00199868[i].p[0].a == a2 && D_00199868[i].p[0].b == k
                || D_00199868[i].p[1].a == a2 && D_00199868[i].p[1].b == k
                || D_00199868[i].p[2].a == a2 && D_00199868[i].p[2].b == k
                || D_00199868[i].p[3].a == a2 && D_00199868[i].p[3].b == k
                || D_00199868[i].p[4].a == a2 && D_00199868[i].p[4].b == k)
                goto next;
        }
        sv = itemmaker_slots[scratch_190d64].v;
        itemmaker_slots[scratch_190d64].v = 100;
        if (itemmaker_param_excluded(a2, k) != 0)
            itemmaker_slots[scratch_190d64].v = sv;
        else {
            itemmaker_slots[scratch_190d64].v = sv;
            scratch_buffer[cnt + 64000] = c;
            scratch_190ee4[cnt] = *a1;
            cnt++;
        }
next:
        a1++;
        c++;
        k++;
    }
    scratch_190ee4[cnt] = 0;
    list_popup_open(scratch_190ee4);
}
