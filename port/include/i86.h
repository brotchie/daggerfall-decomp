/* i86.h for the native build (docs/port.md): the parts of Watcom 10.0a's <i86.h> the game
   uses, with Watcom's 32-bit layouts (int is 32 bits on both). int386 and int386x run the few
   DOS, DPMI and mouse services the game asks for (port/shim/dos.c).

   Far pointers: a flat 32-bit DOS program has one data selector and 32-bit offsets. A native
   pointer has 64 bits, so here a selector stands for the top 32 bits of an address (its 4 GB
   window): FP_SEG gives the selector of a pointer's window, FP_OFF its low 32 bits, and MK_FP
   puts the two together again, so MK_FP(FP_SEG(p), FP_OFF(p)) == p. The flat selector is the
   window of the program's own data and stack; segread gives it for DS, ES and SS, and
   FP_SEG gives it for a near pointer that arrives as a 32-bit value. A DPMI DOS memory block
   has a selector of its own, based at the block (port/shim/dos.c). */
#ifndef PORT_I86_H
#define PORT_I86_H

struct DWORDREGS {
    unsigned int eax, ebx, ecx, edx, esi, edi, cflag;
};

struct WORDREGS {
    unsigned short ax, _1, bx, _2, cx, _3, dx, _4, si, _5, di, _6;
    unsigned int cflag;
};

struct BYTEREGS {
    unsigned char al, ah;
    unsigned short _1;
    unsigned char bl, bh;
    unsigned short _2;
    unsigned char cl, ch;
    unsigned short _3;
    unsigned char dl, dh;
    unsigned short _4;
};

union REGS {
    struct DWORDREGS x;
    struct WORDREGS w;
    struct BYTEREGS h;
};

struct SREGS {
    unsigned short es, cs, ss, ds, fs, gs;
};

int port_int386(int, union REGS *, union REGS *);
int port_int386x(int, union REGS *, union REGS *, struct SREGS *);
void port_segread(struct SREGS *);

/* the selector of p's 4 GB window, and selector:offset as a pointer (port/shim/dos.c) */
unsigned short port_fp_seg(const volatile void *p);
void *port_mk_fp(unsigned short sel, unsigned int off);

#define FP_SEG(p) port_fp_seg((const volatile void *)(p))
#define FP_OFF(p) ((unsigned)(unsigned long)(p))
#define MK_FP(s, o) port_mk_fp((unsigned short)(s), (unsigned)(o))

#endif
