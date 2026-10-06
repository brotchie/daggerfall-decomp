; lightp_t.asm: the test shim of xn_light_add_point's asm entry (15BF75), built only by
; tools/xn_rc.py. Canonical C has no such entry: xn_light_add_point (light.c) tells its caller
; whether a point light reaches the face, and xn_light_setup_poly counts the lights and builds
; the shader. The asm's handler instead dropped its own return address when the third slot
; filled and jumped into the three-light builder, which returned to the light setup's caller.
; The records of 15BF75 see that return, so its shim makes it: the route stub calls this with
; EAX = the stub's saved registers (pushfd; pushad); xc_light_add_point (light_t.c) does the
; work on them and returns 1 for the frame skip.
;
; Assembled by Watcom's WASM (10.0a).
.386p
.model flat

extrn xc_light_add_point_:near          ; int (xn_regs *)

public xn_light_add_point_r_

.code

xn_light_add_point_r_:
    call xc_light_add_point_
    test eax, eax
    jne skip
    ret                                 ; to the stub: popad; popfd; ret
skip:
    add esp, 4                          ; the stub's own return address
    popad                               ; the registers as xc_light_add_point left them
    popfd
    add esp, 4                          ; the dispatch loop's return address (the asm's pop eax)
    ret                                 ; to the light setup's caller, as the builder's ret

end
