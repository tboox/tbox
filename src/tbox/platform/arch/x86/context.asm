;!The Treasure Box Library
; 
; TBox is free software; you can redistribute it and/or modify
; it under the terms of the GNU Lesser General Public License as published by
; the Free Software Foundation; either version 2.1 of the License, or
; (at your option) any later version.
; 
; TBox is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU Lesser General Public License for more details.
; 
; You should have received a copy of the GNU Lesser General Public License
; along with TBox; 
; If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
; 
; Copyright (C) 2009 - 2017, ruki All rights reserved.
;
; @author   ruki
; @file     context.asm
;
;;

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; codes
;;

.386
.model flat
option casemap :none 
.code

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; implementation
;;

; get mcontext
;
; @param mcontext      the mcontext
;
; @return              the error code, ok: 0
;;
tb_context_get_asm proc near

    mov eax, [esp + 4]              ; eax = [esp + 4] = mcontext

    mov word ptr [eax], fs          ; mcontext.mc_fs = fs
    mov word ptr [eax + 4], es      ; mcontext.mc_es = es
    mov word ptr [eax + 8], ds      ; mcontext.mc_ds = ds
    mov word ptr [eax + 12], ss     ; mcontext.mc_ss = ss
    mov [eax + 16], edi             ; mcontext.mc_edi = edi
    mov [eax + 20], esi             ; mcontext.mc_esi = esi
    mov [eax + 24], ebp             ; mcontext.mc_ebp = ebp
    mov [eax + 28], ebx             ; mcontext.mc_ebx = ebx
    mov [eax + 32], edx             ; mcontext.mc_edx = edx
    mov [eax + 36], ecx             ; mcontext.mc_ecx = ecx

    mov dword ptr [eax + 40], 1	    ; mcontext.mc_eax = 1
                                    ; 
                                    ; if (getcontext(ctx) == 0) 
                                    ;      setcontext(ctx);
                                    ;
                                    ; getcontext() will return 1 after calling setcontext()
                                    ;;

    ; esp + 4: ...         => mcontext.mc_esp
    ; esp    : return addr => mcontext.mc_eip
    ;;
    mov ecx, [esp]	                ; mcontext.mc_eip = eip (the return address of tb_context_get())
    mov [eax + 44], ecx

    lea ecx, [esp + 4]	            ; mcontext.mc_esp = esp + 4 (after ret)
    mov [eax + 48], ecx

    mov ecx, [eax + 36]	            ; restore ecx 

    mov eax, 0                      ; return 0
    ret

tb_context_get_asm endp

; set mcontext
;
; @param mcontext      the mcontext
;
; @return              when successful, does not return. 
;;
tb_context_set_asm proc near

    mov eax, [esp + 4]              ; eax = [esp + 4] = mcontext

    mov fs, word ptr [eax]          ; fs = mcontext.mc_fs
    mov es, word ptr [eax + 4]      ; es = mcontext.mc_es
    mov ds, word ptr [eax + 8]      ; ds = mcontext.mc_ds
    mov ss, word ptr [eax + 12]     ; ss = mcontext.mc_ss
    mov edi, [eax + 16]             ; edi = mcontext.mc_edi
    mov esi, [eax + 20]             ; esi = mcontext.mc_esi
    mov ebp, [eax + 24]             ; ebp = mcontext.mc_ebp
    mov ebx, [eax + 28]             ; ebx = mcontext.mc_ebx
    mov edx, [eax + 32]             ; edx = mcontext.mc_edx
    mov ecx, [eax + 36]             ; ecx = mcontext.mc_ecx

    mov esp, [eax + 48]             ; esp = mcontext.mc_esp

    push [eax + 44]	                ; push mcontext.mc_eip to the return address

    mov eax, [eax + 40]             ; eax = mcontext.mc_eax

    ret                             ; return and goto mcontext.mc_eip

tb_context_set_asm endp

end

