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

;.386
;;.model flat
option casemap :none 
.code

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; implementation
;;

; get mcontext
;
; @param mcontext      rcx
;
; @return              the error code, ok: 0
;;
tb_context_get_asm proc 

    mov qword ptr [rcx], rdi            ; mcontext.mc_rdi = rdi 
    mov qword ptr [rcx + 8], rsi        ; mcontext.mc_rsi = rsi
    mov qword ptr [rcx + 16], rdx       ; mcontext.mc_rdx = rdx 
    mov qword ptr [rcx + 24], rcx       ; mcontext.mc_rcx = rcx 
    mov qword ptr [rcx + 32], r8        ; mcontext.mc_r8  = r8 
    mov qword ptr [rcx + 40], r9        ; mcontext.mc_r9  = r9 
    mov qword ptr [rcx + 48], 1         ; mcontext.mc_rax = 1
                                        ; 
                                        ; if (getcontext(ctx) == 0) 
                                        ;      setcontext(ctx);
                                        ;
                                        ; getcontext() will return 1 after calling setcontext()
                                        ;;
    mov qword ptr [rcx + 56], rbx       ; mcontext.mc_rbx = rbx 
    mov qword ptr [rcx + 64], rbp       ; mcontext.mc_rbp = rbp
    mov qword ptr [rcx + 72], r10       ; mcontext.mc_r10 = r10 
    mov qword ptr [rcx + 80], r11       ; mcontext.mc_r11 = r11 
    mov qword ptr [rcx + 88], r12       ; mcontext.mc_r12 = r12 
    mov qword ptr [rcx + 96], r13       ; mcontext.mc_r13 = r13 
    mov qword ptr [rcx + 104], r14      ; mcontext.mc_r14 = r14 
    mov qword ptr [rcx + 112], r15      ; mcontext.mc_r15 = r15 

    ; rsp + 8: ...         => mcontext.mc_rsp
    ; rsp    : return addr => mcontext.mc_rip
    ;;
    mov rax, qword ptr [rsp]            ; mcontext.mc_rip = eip (the return address of tb_context_get())
    mov qword ptr [rcx + 120], rax

    lea rax, [rsp + 8]                  ; mcontext.mc_rsp = rsp + 8 (after ret)
    mov qword ptr [rcx + 128], rax
    
    mov rax, 0                          ; return 0
    ret
tb_context_get_asm endp 

; set mcontext
;
; @param mcontext      rcx
;
; @return              when successful, does not return. 
;;
tb_context_set_asm proc 

    mov rdi, qword ptr [rcx]            ; rdi = mcontext.mc_rdi 
    mov rsi, qword ptr [rcx + 8]        ; rsi = mcontext.mc_rsi 
    mov rdx, qword ptr [rcx + 16]       ; rdx = mcontext.mc_rdx 
    mov r8, qword ptr [rcx + 32]        ; r8 = mcontext.mc_r8
    mov r9, qword ptr [rcx + 40]        ; r9 = mcontext.mc_r9 
    mov rax, qword ptr [rcx + 48]       ; rax = mcontext.mc_rax
    mov rbx, qword ptr [rcx + 56]       ; rbx = mcontext.mc_rbx 
    mov rbp, qword ptr [rcx + 64]       ; rbp = mcontext.mc_rbp 
    mov r10, qword ptr [rcx + 72]       ; r10 = mcontext.mc_r10 
    mov r11, qword ptr [rcx + 80]       ; r11 = mcontext.mc_r11 
    mov r12, qword ptr [rcx + 88]       ; r12 = mcontext.mc_r12 
    mov r13, qword ptr [rcx + 96]       ; r13 = mcontext.mc_r13 
    mov r14, qword ptr [rcx + 104]      ; r14 = mcontext.mc_r14 
    mov r15, qword ptr [rcx + 112]      ; r15 = mcontext.mc_r15 
    mov rsp, qword ptr [rcx + 128]      ; rsp = mcontext.mc_rsp 

    push qword ptr [rcx + 120]          ; push mcontext.mc_rip to the return address

    mov rcx, qword ptr [rcx + 24]       ; rcx = mcontext.mc_rcx 

    ret                                 ; return and goto mcontext.mc_rip
tb_context_set_asm endp 

end

