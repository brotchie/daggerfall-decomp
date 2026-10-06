/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005209D */
#include "structs.h"
#include "clib.h"
#pragma pack(1)
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
extern int disk_open_data(char *);

int flc_open(char *name, struct flc_player *anim)
{
    struct info header;
    struct hdr chunk;

    anim->handle = disk_open_data(name);
    if (anim->handle < 1)
        return 0;
    read(anim->handle, &header, 128);
    anim->frame_count = header.frames;
    anim->ticks_per_frame = header.speed / 55 + 1;
    anim->width = header.width;
    anim->height = header.height;
    anim->y = anim->x = 0;
    anim->loop_offset = header.frame2_offset;
    if (anim->chunk == 0) {
        anim->flags |= 1;
        anim->chunk = mc_malloc(anim->width * anim->height, D_00175404, 225);
    }
    if (anim->palette == 0) {
        anim->flags |= 2;
        anim->palette = mc_malloc(2050, D_00175404, 231);
    }
    mc_memset(anim->palette, 0, 2050, D_00175404, 233, 4);
    read(anim->handle, &chunk, 10);
    if ((unsigned short)chunk.x == 0xF100) {
        read(anim->handle, &chunk, 8);
        if (chunk.type == 3) {
            anim->x = chunk.x - (anim->width >> 1);
            anim->y = chunk.y - (anim->height >> 1);
        }
    }
    lseek(anim->handle, header.frame1_offset, 0);
    anim->frames_left = 1;
    if (anim->image == 0) {
        anim->flags |= 64;
        anim->image = mc_malloc(anim->width * anim->height, D_00175404, 255);
    }
    return 1;
}
