/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005209D */
#pragma pack(1)
struct anim {
    unsigned char flags;
    char pad1;
    unsigned short handle;
    short frame_count;
    short frames_left;
    short ticks_per_frame;
    int loop_offset;
    short x;
    short y;
    short w;
    short h;
    char *buf0;
    char *buf1;
    char *buf2;
};
struct hdr {
    short type;
    short pad2;
    short x;
    short y;
    short pad8[2];
};
struct info {
    char pad0[6];
    short frames;
    short width;
    short height;
    char pad12[4];
    unsigned speed;
    char pad20[60];
    int frame1_offset;
    int frame2_offset;
    char pad88[40];
};
#pragma pack()
extern char D_00175404[];
extern unsigned short disk_open_data(char *);
extern int mc_memset();
extern int lseek(int, int, int);
extern char *mc_malloc(int, char *, int);
extern int read(int, void *, int);

int flc_open(char *name, struct anim *anim)
{
    struct info header;
    struct hdr chunk;

    anim->handle = disk_open_data(name);
    if (anim->handle < 1)
        return 0;
    read(anim->handle, &header, 128);
    anim->frame_count = header.frames;
    anim->ticks_per_frame = header.speed / 55 + 1;
    anim->w = header.width;
    anim->h = header.height;
    anim->y = anim->x = 0;
    anim->loop_offset = header.frame2_offset;
    if (anim->buf0 == 0) {
        anim->flags |= 1;
        anim->buf0 = mc_malloc(anim->w * anim->h, D_00175404, 225);
    }
    if (anim->buf1 == 0) {
        anim->flags |= 2;
        anim->buf1 = mc_malloc(2050, D_00175404, 231);
    }
    mc_memset(anim->buf1, 0, 2050, D_00175404, 233, 4);
    read(anim->handle, &chunk, 10);
    if ((unsigned short)chunk.x == 0xF100) {
        read(anim->handle, &chunk, 8);
        if (chunk.type == 3) {
            anim->x = chunk.x - (anim->w >> 1);
            anim->y = chunk.y - (anim->h >> 1);
        }
    }
    lseek(anim->handle, header.frame1_offset, 0);
    anim->frames_left = 1;
    if (anim->buf2 == 0) {
        anim->flags |= 64;
        anim->buf2 = mc_malloc(anim->w * anim->h, D_00175404, 255);
    }
    return 1;
}
