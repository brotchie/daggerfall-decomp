/* i86.h for the native build (docs/port.md): the parts of Watcom 10.0a's <i86.h> the game
   uses, with Watcom's 32-bit layouts (int is 32 bits on both). int386 and int386x run the few
   DOS, DPMI and mouse services the game asks for (port/shim/dos.c). */
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

/* far pointers: there are no segments; a selector is 0 and an offset is the address */
#define FP_SEG(p) ((unsigned short)0)
#define FP_OFF(p) ((unsigned)(unsigned long)(p))
#define MK_FP(s, o) ((void *)(unsigned long)(o))

#endif
