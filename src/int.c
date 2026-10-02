/* int.c: DPMI helpers written in assembly in the original (no stack frame); kept as naked
 * functions with the same instructions. */

/* free a DOS memory block: the selector arrives in eax, edx is split into cx:dx (DPMI 0007h
 * sets a segment's base address from cx:dx) */
void __declspec(naked) func_00099648(void)
{
    _asm {
        push edx
        pop dx
        pop cx
        mov bx, ax
        mov ax, 7
        int 31h
        ret
    }
}

/* allocate eax bytes of DOS memory (DPMI 0100h, paragraphs), then return the block's linear
 * base address (DPMI 0006h on the selector in dx) */
void __declspec(naked) func_00099662(void)
{
    _asm {
        push ebx
        push ecx
        push edx
        add eax, 0fh
        shr eax, 4
        mov ebx, eax
        mov ax, 100h
        int 31h
        mov bx, dx
        mov ax, 6
        int 31h
        mov ax, cx
        shl eax, 10h
        mov ax, dx
        pop edx
        pop ecx
        pop ebx
        ret
    }
}
