/* packed.c: what Daggerfall's installer unpacks (port_host.h). The CD keeps ARCH3D.BSA (the 3D
   models) and DAGGER.SND (the sound effects) in ARENA2/PACKED.DAT, and an install made from
   Bethesda's free release of the CD lacks them. The native build unpacks them into the
   overlay's ARENA2 when the game folder has neither, as tools/fallemu.py's prepare_overlay
   does for the emulator.

   PACKED.DAT: 8 bytes, then 256 KB blocks, each a 36-byte header (nine u32s: [3] the block's
   compressed size, [5] its unpacked size) and a PKWARE DCL ("implode") stream; a file's last
   block is short; the names close the file. The decompressor follows Mark Adler's blast.c
   (zlib contrib/blast; tools/blast.py). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "port_host.h"

#define MAXBITS 13

struct huffman {
    short count[MAXBITS + 1];
    short symbol[256];
};

struct stream {
    const unsigned char *in;
    size_t len, pos;
    unsigned int bitbuf, bitcnt;
    unsigned char *out;
    size_t outlen, outcap;
    int error;
};

static unsigned int bits(struct stream *s, unsigned int n)
{
    unsigned int v;

    while (s->bitcnt < n) {
        if (s->pos >= s->len) {
            s->error = 1;
            return 0;
        }
        s->bitbuf |= (unsigned int)s->in[s->pos++] << s->bitcnt;
        s->bitcnt += 8;
    }
    v = s->bitbuf & ((1u << n) - 1);
    s->bitbuf >>= n;
    s->bitcnt -= n;
    return v;
}

/* canonical codes, stored with their bits inverted */
static int decode(struct stream *s, const struct huffman *h)
{
    int len, code = 0, first = 0, index = 0;

    for (len = 1; len <= MAXBITS; len++) {
        int count = h->count[len];
        code |= (int)(bits(s, 1) ^ 1);
        if (code - first < count)
            return h->symbol[index + (code - first)];
        index += count;
        first = (first + count) << 1;
        code <<= 1;
    }
    s->error = 1;
    return 0;
}

static void construct(struct huffman *h, const unsigned char *rep, int n)
{
    short length[256], offs[MAXBITS + 2];
    int symbol = 0, len, s;

    do {
        int left;
        len = *rep++;
        left = (len >> 4) + 1;
        len &= 15;
        do
            length[symbol++] = (short)len;
        while (--left);
    } while (--n);
    memset(h->count, 0, sizeof h->count);
    for (s = 0; s < symbol; s++)
        h->count[length[s]]++;
    offs[1] = 0;
    for (len = 1; len < MAXBITS; len++)
        offs[len + 1] = (short)(offs[len] + h->count[len]);
    for (s = 0; s < symbol; s++)
        if (length[s] != 0)
            h->symbol[offs[length[s]]++] = (short)s;
}

static void put(struct stream *s, unsigned char c)
{
    if (s->outlen == s->outcap) {
        size_t cap = s->outcap ? s->outcap * 2 : 1 << 20;
        unsigned char *p = realloc(s->out, cap);
        if (p == NULL) {
            s->error = 1;
            return;
        }
        s->out = p;
        s->outcap = cap;
    }
    s->out[s->outlen++] = c;
}

/* one DCL stream from in[pos], appended to s->out */
static int explode(struct stream *s)
{
    static const unsigned char litlen[] = {
        11, 124, 8, 7, 28, 7, 188, 13, 76, 4, 10, 8, 12, 10, 12, 10, 8, 23, 8,
        9, 7, 6, 7, 8, 7, 6, 55, 8, 23, 24, 12, 11, 7, 9, 11, 12, 6, 7, 22, 5,
        7, 24, 6, 11, 9, 6, 7, 22, 7, 11, 38, 7, 9, 8, 25, 11, 8, 11, 9, 12,
        8, 12, 5, 38, 5, 38, 5, 11, 7, 5, 6, 21, 6, 10, 53, 8, 7, 24, 10, 27,
        44, 253, 253, 253, 252, 252, 252, 13, 12, 45, 12, 45, 12, 61, 12, 45,
        44, 173};
    static const unsigned char lenlen[] = {2, 35, 36, 53, 38, 23};
    static const unsigned char distlen[] = {2, 20, 53, 230, 247, 151, 248};
    static const short base[16] = {3, 2, 4, 5, 6, 7, 8, 9, 10, 12, 16, 24, 40, 72, 136, 264};
    static const char extra[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8};
    static struct huffman lit, len, dist;
    static int built;
    unsigned int coded, dict;
    size_t start = s->outlen;

    if (!built) {
        construct(&lit, litlen, sizeof litlen);
        construct(&len, lenlen, sizeof lenlen);
        construct(&dist, distlen, sizeof distlen);
        built = 1;
    }
    s->bitbuf = s->bitcnt = 0;
    coded = bits(s, 8);
    dict = bits(s, 8);
    if (coded > 1 || dict < 4 || dict > 6)
        return -1;
    while (!s->error) {
        if (bits(s, 1)) {
            int sym = decode(s, &len);
            unsigned int length = (unsigned int)base[sym] + bits(s, (unsigned int)extra[sym]);
            unsigned int shift, d;
            size_t from, k;
            if (length == 519)
                break;
            shift = length == 2 ? 2 : dict;
            d = ((unsigned int)decode(s, &dist) << shift) + bits(s, shift) + 1;
            if (d > s->outlen - start)
                return -1;
            from = s->outlen - d;
            for (k = 0; k < length; k++)
                put(s, s->out[from + k]);
        } else {
            put(s, (unsigned char)(coded ? decode(s, &lit) : (int)bits(s, 8)));
        }
    }
    return s->error ? -1 : 0;
}

static int write_file(const char *path, const unsigned char *data, size_t n)
{
    char tmp[1100];
    FILE *f;

    snprintf(tmp, sizeof tmp, "%s.tmp", path);
    f = fopen(tmp, "wb");
    if (f == NULL)
        return -1;
    if (fwrite(data, 1, n, f) != n) {
        fclose(f);
        remove(tmp);
        return -1;
    }
    fclose(f);
    return rename(tmp, path);
}

int port_unpack_packed(void)
{
    static const char *const names[] = {"ARCH3D.BSA", "DAGGER.SND"};
    char path[1024], dst[1024];
    unsigned char *d;
    struct stream s;
    int need = 0, i, k = 0;
    long size;
    FILE *f;

    for (i = 0; i < 2; i++) {
        char dos[32];
        snprintf(dos, sizeof dos, "ARENA2\\%s", names[i]);
        if (!dos_host_path(dos, 0, path, sizeof path))
            need |= 1 << i;
    }
    if (need == 0)
        return 0;
    if (!dos_host_path("ARENA2\\PACKED.DAT", 0, path, sizeof path) ||
        (f = fopen(path, "rb")) == NULL) {
        fprintf(stderr, "port: no ARENA2\\PACKED.DAT to unpack ARCH3D.BSA and DAGGER.SND from\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    d = malloc((size_t)size);
    if (d == NULL || fread(d, 1, (size_t)size, f) != (size_t)size) {
        fclose(f);
        free(d);
        return -1;
    }
    fclose(f);
    fprintf(stderr, "port: unpacking ARCH3D.BSA and DAGGER.SND from PACKED.DAT (once)\n");
    memset(&s, 0, sizeof s);
    s.in = d;
    s.len = (size_t)size;
    s.pos = 8;
    while (k < 2 && s.pos + 36 <= s.len) {
        uint32_t h[9];
        size_t block = s.pos;
        memcpy(h, d + block, sizeof h);
        s.pos = block + 36;
        if (explode(&s) != 0) {
            fprintf(stderr, "port: PACKED.DAT: bad block at %zu\n", block);
            break;
        }
        s.pos = block + 36 + h[3];
        if (h[5] < 0x40000) {           /* the file's last block */
            if (need & (1 << k)) {
                char dos[32];
                snprintf(dos, sizeof dos, "ARENA2\\%s", names[k]);
                if (!dos_host_path(dos, 1, dst, sizeof dst) ||
                    write_file(dst, s.out, s.outlen) != 0)
                    fprintf(stderr, "port: cannot write %s\n", names[k]);
            }
            s.outlen = 0;
            k++;
        }
    }
    free(s.out);
    free(d);
    return k == 2 ? 0 : -1;
}
