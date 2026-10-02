#!/usr/bin/env python3
"""Decompress PKWARE Data Compression Library ("implode") data, after Mark Adler's blast.c
(zlib contrib/blast). Daggerfall's installer packs ARCH3D.BSA and DAGGER.SND this way in
ARENA2/PACKED.DAT.

usage: blast.py IN OUT [offset]
"""
import sys

MAXBITS = 13
LITLEN = bytes([
    11, 124, 8, 7, 28, 7, 188, 13, 76, 4, 10, 8, 12, 10, 12, 10, 8, 23, 8,
    9, 7, 6, 7, 8, 7, 6, 55, 8, 23, 24, 12, 11, 7, 9, 11, 12, 6, 7, 22, 5,
    7, 24, 6, 11, 9, 6, 7, 22, 7, 11, 38, 7, 9, 8, 25, 11, 8, 11, 9, 12,
    8, 12, 5, 38, 5, 38, 5, 11, 7, 5, 6, 21, 6, 10, 53, 8, 7, 24, 10, 27,
    44, 253, 253, 253, 252, 252, 252, 13, 12, 45, 12, 45, 12, 61, 12, 45,
    44, 173])
LENLEN = bytes([2, 35, 36, 53, 38, 23])
DISTLEN = bytes([2, 20, 53, 230, 247, 151, 248])
BASE = [3, 2, 4, 5, 6, 7, 8, 9, 10, 12, 16, 24, 40, 72, 136, 264]
EXTRA = [0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8]


def construct(rep):
    """Huffman table (count per length, symbols in code order) from compact lengths. The
    decoder walks it bit by bit; codes are canonical with inverted bits in the stream."""
    lengths = []
    for b in rep:
        lengths += [b & 15] * ((b >> 4) + 1)
    count = [0] * (MAXBITS + 1)
    for n in lengths:
        count[n] += 1
    offs = [0] * (MAXBITS + 2)
    for n in range(1, MAXBITS + 1):
        offs[n + 1] = offs[n] + count[n]
    symbol = [0] * len(lengths)
    for s, n in enumerate(lengths):
        if n:
            symbol[offs[n]] = s
            offs[n] += 1
    # a lookup by (length, code) -> symbol, built from the canonical order
    table = {}
    code = first = index = 0
    for n in range(1, MAXBITS + 1):
        for k in range(count[n]):
            table[(n, first + k)] = symbol[index + k]
        index += count[n]
        first = (first + count[n]) << 1
    return table


LIT, LEN, DIST = construct(LITLEN), construct(LENLEN), construct(DISTLEN)


def explode(data, pos=0):
    """Decompress one stream starting at data[pos]. Returns (output, end position)."""
    bitbuf = 0
    bitcnt = 0
    p = pos
    out = bytearray()

    def bits(n):
        nonlocal bitbuf, bitcnt, p
        while bitcnt < n:
            bitbuf |= data[p] << bitcnt
            p += 1
            bitcnt += 8
        v = bitbuf & ((1 << n) - 1)
        bitbuf >>= n
        bitcnt -= n
        return v

    def decode(table):
        code = 0
        for n in range(1, MAXBITS + 1):
            code |= bits(1) ^ 1
            s = table.get((n, code))
            if s is not None:
                return s
            code <<= 1
        raise ValueError("bad code at %d" % p)

    lit, dict_bits = bits(8), bits(8)
    if lit > 1 or not 4 <= dict_bits <= 6:
        raise ValueError("not a PKWARE DCL stream at %d (%d, %d)" % (pos, lit, dict_bits))
    while True:
        if bits(1):
            sym = decode(LEN)
            length = BASE[sym] + bits(EXTRA[sym])
            if length == 519:
                break
            shift = 2 if length == 2 else dict_bits
            dist = (decode(DIST) << shift) + bits(shift) + 1
            if dist > len(out):
                raise ValueError("distance too far back at %d" % p)
            start = len(out) - dist
            for k in range(length):
                out.append(out[start + k])
        else:
            out.append(decode(LIT) if lit else bits(8))
    return bytes(out), p


if __name__ == "__main__":
    src = open(sys.argv[1], "rb").read()
    off = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0
    out, end = explode(src, off)
    open(sys.argv[2], "wb").write(out)
    print("%d bytes in -> %d bytes out (stream ends at %#x)" % (end - off, len(out), end))
