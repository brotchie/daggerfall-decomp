/* dos.c: the DOS, DPMI and mouse services the game asks for through int386 and int386x
   (docs/port.md). Each service the game uses gets a host version as the port reaches it;
   until then a call logs its interrupt and function and fails (carry set). */
#include <stdio.h>
#include <string.h>

#include "i86.h"

static int service(int intno, union REGS *in, union REGS *out, struct SREGS *sr)
{
    (void)sr;
    if (out != in)
        *out = *in;
    fprintf(stderr, "port: int %02Xh ax=%04X not implemented\n", intno, in->w.ax);
    out->x.cflag = 1;
    return (int)out->x.eax;
}

int port_int386(int intno, union REGS *in, union REGS *out)
{
    return service(intno, in, out, NULL);
}

int port_int386x(int intno, union REGS *in, union REGS *out, struct SREGS *sr)
{
    return service(intno, in, out, sr);
}

void port_segread(struct SREGS *sr)
{
    memset(sr, 0, sizeof *sr);
}

/* int.c's own FP_SEG (it undefines <i86.h>'s): there are no segments */
unsigned short _FP_SEG(const volatile void *p)
{
    (void)p;
    return 0;
}
