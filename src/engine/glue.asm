; glue.asm: calling asm and DOS from the readable C with a register file (see xngine.h).
;
; xn_asmcall(target, r): loads EAX EBX ECX EDX ESI EDI EBP from the xn_regs at r, calls
; target, and stores them back with EFLAGS. For an asm function whose results are in more
; than one register (a pragma can only name one). The callee runs on the C's stack.
;
; xn_intNN(r): the same around `int NNh`: DOS (21h), DPMI (31h), the mouse (33h), the video
; BIOS (10h), the keyboard (16h), 15h and 2Fh. The emulator logs and replays them like the
; asm's own.
;
; Watcom's register convention: arguments in EAX and EDX; EBX ECX ESI EDI EBP preserved.
; Assembled by Watcom's WASM (10.0a).
.386p
.model flat

public xn_asmcall_
public xn_int10_, xn_int15_, xn_int16_, xn_int21_, xn_int2f_, xn_int31_, xn_int33_

R_EDI   equ 0
R_ESI   equ 4
R_EBP   equ 8
R_ESP   equ 12
R_EBX   equ 16
R_EDX   equ 20
R_ECX   equ 24
R_EAX   equ 28
R_EFL   equ 32

; save the C's registers, keep r (EDX) on the stack, load the register file
ENTER_REGS macro
    push ebx
    push ecx
    push esi
    push edi
    push ebp
    push edx
    mov ebp, edx
    mov eax, dword ptr [ebp+R_EAX]
    mov ebx, dword ptr [ebp+R_EBX]
    mov ecx, dword ptr [ebp+R_ECX]
    mov edx, dword ptr [ebp+R_EDX]
    mov esi, dword ptr [ebp+R_ESI]
    mov edi, dword ptr [ebp+R_EDI]
    mov ebp, dword ptr [ebp+R_EBP]
endm

; store the registers and EFLAGS in the register file, restore the C's registers
LEAVE_REGS macro
    xchg ebp, dword ptr [esp]
    mov dword ptr [ebp+R_EAX], eax
    mov dword ptr [ebp+R_EBX], ebx
    mov dword ptr [ebp+R_ECX], ecx
    mov dword ptr [ebp+R_EDX], edx
    mov dword ptr [ebp+R_ESI], esi
    mov dword ptr [ebp+R_EDI], edi
    pushfd
    pop dword ptr [ebp+R_EFL]
    pop dword ptr [ebp+R_EBP]
    pop ebp
    pop edi
    pop esi
    pop ecx
    pop ebx
    ret
endm

.code

xn_asmcall_:
    push ebx
    push ecx
    push esi
    push edi
    push ebp
    push edx                    ; r
    push eax                    ; the target (on the stack: an interrupt may call in here)
    mov ebp, edx
    mov eax, dword ptr [ebp+R_EAX]
    mov ebx, dword ptr [ebp+R_EBX]
    mov ecx, dword ptr [ebp+R_ECX]
    mov edx, dword ptr [ebp+R_EDX]
    mov esi, dword ptr [ebp+R_ESI]
    mov edi, dword ptr [ebp+R_EDI]
    mov ebp, dword ptr [ebp+R_EBP]
    call dword ptr [esp]
    lea esp, [esp+4]            ; (the flags as the callee left them)
    LEAVE_REGS

INT_GLUE macro name, vector
name:
    ENTER_REGS
    int vector
    LEAVE_REGS
endm

INT_GLUE xn_int10_, 10h
INT_GLUE xn_int15_, 15h
INT_GLUE xn_int16_, 16h
INT_GLUE xn_int21_, 21h
INT_GLUE xn_int2f_, 2Fh
INT_GLUE xn_int31_, 31h
INT_GLUE xn_int33_, 33h

end
