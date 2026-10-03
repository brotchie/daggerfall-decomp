; Shared by the XnGine modules (src/xngine/xn_*.asm).
;
; XnGine was not assembled by Watcom's WASM. For an ALU or mov between two registers its
; assembler wrote the `reg, r/m` form (8B /r for mov edi, esi: 8B FE), the form MASM and TASM
; write; WASM 10.0a writes the `r/m, reg` form (89 F7). `mov@ edi, esi` is `mov edi, esi` in
; the original's encoding, and likewise add@, or@, adc@, sbb@, and@, sub@, xor@ and cmp@.

; ?r_<reg> = register number, ?w_<reg> = size in bytes
?k = 0
irp x, <eax, ecx, edx, ebx, esp, ebp, esi, edi>
    ?r_&x = ?k
    ?w_&x = 4
    ?k = ?k + 1
endm
?k = 0
irp x, <ax, cx, dx, bx, sp, bp, si, di>
    ?r_&x = ?k
    ?w_&x = 2
    ?k = ?k + 1
endm
?k = 0
irp x, <al, cl, dl, bl, ah, ch, dh, bh>
    ?r_&x = ?k
    ?w_&x = 1
    ?k = ?k + 1
endm

; op d, s with opcode opc (its byte form, direction bit set): opc+1 for words and dwords
?rr macro opc, d, s
    if ?w_&d eq 2
        db 066h
    endif
    if ?w_&d eq 1
        db opc, 0C0h + ?r_&d * 8 + ?r_&s
    else
        db opc + 1, 0C0h + ?r_&d * 8 + ?r_&s
    endif
endm

add@ macro d, s
    ?rr 002h, d, s
endm
or@ macro d, s
    ?rr 00Ah, d, s
endm
adc@ macro d, s
    ?rr 012h, d, s
endm
sbb@ macro d, s
    ?rr 01Ah, d, s
endm
and@ macro d, s
    ?rr 022h, d, s
endm
sub@ macro d, s
    ?rr 02Ah, d, s
endm
xor@ macro d, s
    ?rr 032h, d, s
endm
cmp@ macro d, s
    ?rr 03Ah, d, s
endm
mov@ macro d, s
    ?rr 08Ah, d, s
endm
