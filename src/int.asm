; int.asm: DPMI helpers written in assembly in the original (no stack frame), assembled
; with the Watcom 10.0a assembler (WASM). The original assembler encoded register-to-register
; moves as 8b /r where WASM writes 89 /r, so those few are spelled out as bytes.
.386p
_TEXT segment dword public use32 'CODE'

; free a DOS memory block: the selector arrives in eax, edx is split into cx:dx
; (DPMI 0007h sets a segment's base address from cx:dx)
public func_00099648_
func_00099648_:
        push    edx
        pop     dx
        pop     cx
        db      66h, 8bh, 0d8h          ; mov bx, ax (the 8b form, as TASM/MASM encode it)
        mov     ax, 7
        int     31h
        ret

; allocate eax bytes of DOS memory (DPMI 0100h, paragraphs), then return the block's
; linear base address (DPMI 0006h on the selector in dx)
public func_00099662_
func_00099662_:
        push    ebx
        push    ecx
        push    edx
        add     eax, 0fh
        shr     eax, 4
        db      8bh, 0d8h               ; mov ebx, eax
        mov     ax, 100h
        int     31h
        db      66h, 8bh, 0dah          ; mov bx, dx
        mov     ax, 6
        int     31h
        db      66h, 8bh, 0c1h          ; mov ax, cx
        shl     eax, 10h
        db      66h, 8bh, 0c2h          ; mov ax, dx
        pop     edx
        pop     ecx
        pop     ebx
        ret

_TEXT ends
        end
