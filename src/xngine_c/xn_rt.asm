; xn_rt.asm: the paths between XnGine's asm and its C translation (see runtime.h).
;
; Entering C (xn_thunk): a translated function's asm entry is sent (by the loader) to a stub
; that sets xn_target to the C function and jumps here. The CPU registers go to R, the C
; runs on its own stack (xn_cesp) with interrupts off, and when it returns, R goes back to
; the CPU and execution continues at xn_ret: the return address its `ret` popped, or wherever
; it jumps (another function, or the asm itself for code the C does not handle).
;
; Leaving C for asm (xn_call, xn_int, xn_divfault): R goes to the CPU, the asm runs on its own
; stack (R.esp), and the CPU comes back to R. xn_call puts the address of xn_back in the
; return slot the caller filled, so the callee's `ret` comes back here, and puts the caller's
; value back after. Each saves the C stack pointer in xn_cesp (nested entries run below it).
;
; Assembled by Watcom's WASM (10.0a).
.386p
.model flat

extrn _R:dword
extrn _F:dword
extrn xn_eflags_:near

public xn_thunk, xn_call, xn_int, xn_divfault
public _xn_target, _xn_ret, _xn_retcs, _xn_far, _xn_cesp, _xn_depth, _xn_back_k
public _xn_backtab, xn_back, xn_thunk_c, xn_back_c

R_EAX   equ 0
R_ECX   equ 4
R_EDX   equ 8
R_EBX   equ 12
R_ESP   equ 16
R_EBP   equ 20
R_ESI   equ 24
R_EDI   equ 28
R_EFL   equ 32
R_ES    equ 36
R_CS    equ 40
R_SS    equ 44
R_DS    equ 48
R_FS    equ 52
R_GS    equ 56

.data
_xn_target  dd 0
_xn_ret     dd 0        ; with _xn_retcs: the far pointer of a far return
_xn_retcs   dd 0
_xn_far     dd 0
_xn_cesp    dd 0
xn_flagslot dd 0
xn_calltgt  dd 0
xn_leave_to dd 0
xn_zero     dd 0
_xn_depth   dd 0
_xn_back_k  dd 0
_xn_backtab dd 256 dup (0)

.code

; CPU -> R (all but EFLAGS)
SAVE_CPU macro
    mov dword ptr _R+R_EAX, eax
    mov dword ptr _R+R_ECX, ecx
    mov dword ptr _R+R_EDX, edx
    mov dword ptr _R+R_EBX, ebx
    mov dword ptr _R+R_ESP, esp
    mov dword ptr _R+R_EBP, ebp
    mov dword ptr _R+R_ESI, esi
    mov dword ptr _R+R_EDI, edi
    mov word ptr _R+R_ES, es
    mov word ptr _R+R_CS, cs
    mov word ptr _R+R_SS, ss
    mov word ptr _R+R_DS, ds
    mov word ptr _R+R_FS, fs
    mov word ptr _R+R_GS, gs
endm

; EFLAGS -> R, interrupts off, then onto the C stack with DF clear. The flags are pushed on
; the asm stack (an interrupt must never find ESP on the C stack while they are still on), and
; the dead word they overwrite there is put back. The segment registers stay as they are (the
; C uses no string instructions, so ES is never its concern).
ENTER_FLAGS macro
    mov eax, dword ptr [esp-4]
    pushfd
    cli
    pop dword ptr _R+R_EFL
    mov dword ptr [esp-4], eax
endm

ENTER_STACK macro
    mov esp, _xn_cesp
    cld
    mov dword ptr _F, 0
endm

ENTER_C macro
    ENTER_FLAGS
    ENTER_STACK
endm

; a segment register from R, only if the C changed it: loading a selector sets the accessed bit
; of its descriptor (a write to the descriptor table) and reloads its base and limit
LOAD_SEG macro sreg, off
    local same
    mov ax, sreg
    cmp ax, word ptr _R+off
    je same
    mov sreg, word ptr _R+off
same:
endm

LOAD_REGS macro
    mov eax, dword ptr _R+R_EAX
    mov ecx, dword ptr _R+R_ECX
    mov edx, dword ptr _R+R_EDX
    mov ebx, dword ptr _R+R_EBX
    mov ebp, dword ptr _R+R_EBP
    mov esi, dword ptr _R+R_ESI
    mov edi, dword ptr _R+R_EDI
    mov esp, offset xn_flagslot
    popfd
    mov esp, dword ptr _R+R_ESP
endm

; R -> CPU, EFLAGS last, onto the asm stack (R.esp), and jump to xn_leave_to. EFLAGS is loaded
; with IF clear, and an `sti` (whose one-instruction shadow covers the jump) turns interrupts
; on only once ESP is the asm's: an interrupt must never find ESP in the C's data. The
; selectors are flat, so DS can be loaded before the last reads of R.
LEAVE_C macro
    local noif
    call xn_eflags_
    mov edx, eax
    and eax, not 0200h
    mov xn_flagslot, eax
    LOAD_SEG es, R_ES
    LOAD_SEG fs, R_FS
    LOAD_SEG gs, R_GS
    LOAD_SEG ds, R_DS
    test edx, 0200h
    je noif
    LOAD_REGS
    sti
    jmp dword ptr xn_leave_to
noif:
    LOAD_REGS
    jmp dword ptr xn_leave_to
endm

; The loader's stubs do SAVE_CPU and ENTER_FLAGS themselves and only then (interrupts off)
; set xn_target and come in at xn_thunk_c: an interrupt taken on the way in would run handlers
; whose own stubs set xn_target too. The trampolines do the same for xn_back_k (xn_back_c).
xn_thunk:
    SAVE_CPU
    ENTER_FLAGS
xn_thunk_c:
    ENTER_STACK
    ; C calls whose return slot the asm has since popped (it left the callee some other way,
    ; as the span routines leave the row loop) never return: drop their frames, and put the
    ; slot back if the asm has not reused it. (A slot 64 KB or more below ESP is taken for
    ; another stack's, such as an interrupt handler's, and left alone.)
    mov ecx, _xn_cesp
thunk_unwind:
    cmp dword ptr _xn_depth, 0
    je thunk_go
    mov eax, dword ptr [ecx]            ; the frame's depth (-1: not a call)
    cmp eax, -1
    je thunk_go
    mov ebx, dword ptr [ecx+4]          ; its return slot
    mov edx, dword ptr _R+R_ESP
    sub edx, ebx
    jbe thunk_go                        ; the slot is still on the stack: live
    cmp edx, 10000h
    jae thunk_go                        ; far above: another stack (an interrupt's)
    mov edx, dword ptr _xn_backtab[eax*4]
    cmp dword ptr [ebx], edx
    jne thunk_reused
    mov edx, dword ptr [ecx+8]
    mov dword ptr [ebx], edx
thunk_reused:
    mov _xn_depth, eax
    mov ecx, dword ptr [ecx+12]
    mov _xn_cesp, ecx
    jmp thunk_unwind
thunk_go:
    mov esp, _xn_cesp
    mov dword ptr _xn_far, 0
    call dword ptr _xn_target
    mov eax, _xn_ret
    cmp dword ptr _xn_far, 0
    je thunk_near
    mov dword ptr _xn_far, 0            ; used up: a C caller's own return is near
    mov eax, offset thunk_farjmp
thunk_near:
    mov xn_leave_to, eax
    LEAVE_C
thunk_farjmp:
    jmp fword ptr cs:_xn_ret

; void xn_call(u32 target in eax)
;
; Its frame on the C stack, at xn_cesp: depth, slot, the slot's value, the previous xn_cesp,
; then the saved registers. The callee returns through the slot to the trampoline for this
; depth (xn_backtab, written by the loader: `mov xn_back_k, depth; jmp xn_back`), so a return
; that skips frames (asm that pops its own return address and returns to its caller's caller)
; still comes back to the right C: xn_back unwinds the frames in between, putting their slots
; back as the asm left them.
xn_call:
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    push dword ptr _xn_cesp
    mov xn_calltgt, eax
    mov ebx, dword ptr _R+R_ESP
    push dword ptr [ebx]                ; the return address the caller pushed
    push ebx                            ; and where
    mov eax, _xn_depth
    push eax
    mov eax, dword ptr _xn_backtab[eax*4]
    mov dword ptr [ebx], eax
    inc dword ptr _xn_depth
    mov _xn_cesp, esp
    mov eax, xn_calltgt
    mov xn_leave_to, eax
    LEAVE_C
xn_back:
    SAVE_CPU
    ENTER_FLAGS
xn_back_c:
    ENTER_STACK
    mov ecx, _xn_back_k
back_find:
    cmp dword ptr [esp], ecx
    je back_found
    mov ebx, dword ptr [esp+4]          ; a frame the return skipped
    test ebx, ebx
    je back_next
    mov eax, dword ptr [esp+8]
    mov dword ptr [ebx], eax
back_next:
    mov esp, dword ptr [esp+12]
    jmp back_find
back_found:
    pop eax
    mov _xn_depth, eax
    pop ebx
    pop eax
    mov dword ptr [ebx], eax
    mov _xn_ret, eax
    pop dword ptr _xn_cesp
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

; void xn_int(u32 vector in eax)
xn_int:
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    push dword ptr _xn_cesp
    push 0
    push 0
    push -1
    mov byte ptr xn_int_insn+1, al
    mov _xn_cesp, esp
    mov xn_leave_to, offset xn_int_insn
    LEAVE_C
xn_int_insn:
    db 0CDh, 0                          ; int n, n written above
    SAVE_CPU
    ENTER_C
    add esp, 12
    pop dword ptr _xn_cesp
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

; void xn_divfault(void): a divide error with R's registers and stack. The handler skips the
; 6-byte div (ModRM 35h) and returns with EAX = EDX = 0.
xn_divfault:
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    push dword ptr _xn_cesp
    push 0
    push 0
    push -1
    mov _xn_cesp, esp
    mov xn_leave_to, offset xn_div_insn
    LEAVE_C
xn_div_insn:
    div dword ptr xn_zero
    SAVE_CPU
    ENTER_C
    add esp, 12
    pop dword ptr _xn_cesp
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

end
