/* dload.c: XnGine's initial data from the user's FALL.EXE (xndata.h; docs/engine/data.md).
   FALL.EXE is a CauseWay MZ stub followed by a Watcom LE executable; object 2 is XnGine's code
   and data, which the loader maps at 0xC0000. The initial symbols' table is dtable.c. */
#include "xndata.h"

/* a little-endian dword */
static u32 dword(const u8 *p)
{
    return (u32)p[0] | (u32)p[1] << 8 | (u32)p[2] << 16 | (u32)p[3] << 24;
}

static void copy(u8 *dst, const u8 *src, u32 n)
{
    u32 k;

    for (k = 0; k < n; k++)
        dst[k] = src[k];
}

int xn_data_object2(const u8 *exe, u32 exe_size, u8 *image)
{
    u32 le, page_size, objects, map, pages, first, count, k, at, n, page, off;
    const u8 *obj;

    if (exe_size < 0x40 || exe[0] != 'M' || exe[1] != 'Z')
        return 0;
    le = dword(exe + 0x3C);                     /* the LE header (the stub's e_lfanew) */
    if (le > exe_size - 0x84 || exe[le] != 'L' || exe[le + 1] != 'E')
        return 0;
    page_size = dword(exe + le + 0x28);
    objects = le + dword(exe + le + 0x40);      /* the object table: 24 bytes an object */
    map = le + dword(exe + le + 0x48);          /* the page map: 4 bytes a page */
    pages = dword(exe + le + 0x80);             /* the pages (from the file's start: the
                                                   stub is at 0) */
    if (dword(exe + le + 0x44) < 2 || objects > exe_size - 48 || page_size == 0)
        return 0;
    obj = exe + objects + 24;                   /* object 2: size, base, flags, first page,
                                                   pages */
    if (dword(obj) != XN_DATA_OBJECT2_SIZE || dword(obj + 4) != XN_DATA_OBJECT2_BASE)
        return 0;
    first = dword(obj + 12);
    count = dword(obj + 16);
    for (k = 0, at = 0; k < count && at < XN_DATA_OBJECT2_SIZE; k++, at += page_size) {
        if (map + 4 * (first + k) > exe_size)
            return 0;
        /* a page map entry: the page's number in the file (3 bytes, high first), flags */
        page = (u32)exe[map + 4 * (first - 1 + k)] << 16 |
               (u32)exe[map + 4 * (first - 1 + k) + 1] << 8 | exe[map + 4 * (first - 1 + k) + 2];
        if (exe[map + 4 * (first - 1 + k) + 3] != 0 || page == 0)
            return 0;                           /* not a plain page */
        off = pages + (page - 1) * page_size;
        n = XN_DATA_OBJECT2_SIZE - at < page_size ? XN_DATA_OBJECT2_SIZE - at : page_size;
        if (off > exe_size || n > exe_size - off)
            return 0;
        copy(image + at, exe + off, n);
    }
    return at >= XN_DATA_OBJECT2_SIZE;
}

void xn_data_load(const u8 *object2_image)
{
    const xn_data_item *it;
    u32 k;

    for (k = 0; k < xn_data_item_count; k++) {
        it = &xn_data_items[k];
        copy((u8 *)it->at, object2_image + (it->address - XN_DATA_OBJECT2_BASE), it->size);
    }
}
