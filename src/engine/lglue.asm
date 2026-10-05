; lglue.asm: the asm interface of xn_light_add_point (15BF75), which C cannot have.
;
; It is a handler of the light setup's dispatch table (xn_light_dispatch, called from
; xn_light_setup_poly's loop with ebp = the point slots used * 4, esi = the light ref,
; edi = the polygon). When the third slot fills, the asm drops its own return address and
; jumps to xn_light_build_shader_3, whose ret goes straight back to the light setup's caller:
; a frame skip no route stub can make. The work is C (xn_light_point_reaches, light.c); this
; keeps the control flow. Once xn_light_setup_poly is C (it calls xn_light_point_reaches
; itself), only this function's own records reach it.
;
; Assembled by Watcom's WASM (10.0a).
.386p
.model flat

extrn xn_light_point_reaches_:near      ; int (struct xn_poly *, xn_light_ref *, int slot)
extrn asm_xn_light_build_shader_3:near  ; the asm entry (routed to its C when converted)

public xn_light_add_point_

.code

xn_light_add_point_:
    mov eax, edi                ; Watcom: eax, edx, ebx; it keeps ecx esi edi ebp
    mov edx, esi
    mov ebx, ebp
    shr ebx, 2
    call xn_light_point_reaches_
    test eax, eax
    je not_lit
    add ebp, 4                  ; the slot is filled
    cmp ebp, 0Ch
    je full
not_lit:
    clc
    ret
full:
    pop eax                     ; the dispatch loop's return address
    jmp asm_xn_light_build_shader_3

end
