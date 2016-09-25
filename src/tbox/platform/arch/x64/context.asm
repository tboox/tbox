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
; declaration
;;

; exit(value)
extern _exit:proc

;  ----------------------------------------------------------------------------------
;  |     0   |     1   |     2    |     3   |     4   |     5   |     6   |     7   |
;  ----------------------------------------------------------------------------------
;  |    0x0  |    0x4  |    0x8   |    0xc  |   0x10  |   0x14  |   0x18  |   0x1c  |
;  ----------------------------------------------------------------------------------
;  |      fbr_strg     |      fc_dealloc    |       limit       |        base       |
;  ----------------------------------------------------------------------------------
;  ----------------------------------------------------------------------------------
;  |     8   |    9    |    10    |    11   |    12   |    13   |    14   |    15   |
;  ----------------------------------------------------------------------------------
;  |   0x20  |  0x24   |   0x28   |   0x2c  |   0x30  |   0x34  |   0x38  |   0x3c  |
;  ----------------------------------------------------------------------------------
;  |        R12        |         R13        |        R14        |        R15        |
;  ----------------------------------------------------------------------------------
;  ----------------------------------------------------------------------------------
;  |    16   |    17   |    18   |    19    |    20   |    21   |    22   |    23   |
;  ----------------------------------------------------------------------------------
;  |   0xe40  |   0x44 |   0x48  |   0x4c   |   0x50  |   0x54  |   0x58  |   0x5c  |
;  ----------------------------------------------------------------------------------
;  |        RDI         |       RSI         |        RBX        |        RBP        |
;  ----------------------------------------------------------------------------------
;  ----------------------------------------------------------------------------------
;  |    24   |   25    |    26    |   27    |    28   |    29   |    30   |    31   |
;  ----------------------------------------------------------------------------------
;  |   0x60  |   0x64  |   0x68   |   0x6c  |   0x70  |   0x74  |   0x78  |   0x7c  |
;  ----------------------------------------------------------------------------------
;  |        hidden     |         RIP        |       EXIT        |   parameter area  |
;  ----------------------------------------------------------------------------------
;  ----------------------------------------------------------------------------------
;  |    32   |   32    |    33    |   34    |    35   |    36   |    37   |    38   |
;  ----------------------------------------------------------------------------------
;  |   0x80  |   0x84  |   0x88   |   0x8c  |   0x90  |   0x94  |   0x98  |   0x9c  |
;  ----------------------------------------------------------------------------------
;  |                       parameter area                       |        FCTX       |
;  ----------------------------------------------------------------------------------
;  ----------------------------------------------------------------------------------
;  |    39   |   40    |    41    |   42    |    43   |    44   |    45   |    46   |
;  ----------------------------------------------------------------------------------
;  |   0xa0  |   0xa4  |   0xa8   |   0xac  |   0xb0  |   0xb4  |   0xb8  |   0xbc  |
;  ----------------------------------------------------------------------------------
;  |       DATA        |                    |                   |                   |
;  ----------------------------------------------------------------------------------

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; implementation
;;
.code

; make context (refer to boost.context)
;
; 
;             -----------------------------------------------------------------------------------------
; stackdata: |                                                          |         context        |||||||
;             -----------------------------------------------------------------------------------|-----
;                                                                                           (16-align)
;
;
;             ---------------------------------------
; context:   |  fiber  | dealloc |  limit  |  base   | 
;             ---------------------------------------   
;            0         8         16         24
;                                                                                                          
;             ---------------------------------------
;            |   r12   |   r13   |   r14   |   r15   | 
;             ---------------------------------------   
;            32        40        48        56                 --------------------------------------------------------
;                                                            |                                                        |
;                                                            |          func     __end     arguments             retval(from)
;             -----------------------------------------------------------------------------------------------------------------------------------
;            |   rdi   |   rsi   |   rbx   |   rbp   | retval(saved) |   rip   |   end   |  unused  | context(unused) |  priv(unused)  | padding |
;             -----------------------------------------------------------------------------------------------------------------------------------
;            64        72        80        88        96              104       112       120        128               136                          
;                                                                              |         |
;                                                                              |      16-align
;                                                                              |
;                                                                    esp when jump to function
;
;
; @param stackdata     the stack data (rcx)
; @param stacksize     the stack size (rdx)
; @param func          the entry function (r8)
;
; @return              the context pointer (rax)
;;
tb_context_make proc frame

    ; .xdata for a function's structured exception handling unwind behavior
    .endprolog

    ; save the stack top to rax
    mov rax, rcx
    add rax, rdx

    ; reserve space for first argument(from) and retval(from) item of context-function
    ; 3 * 8 = 24
    sub rax, 24

    ; 16-align of the stack top address 
    and rax, -16

    ; reserve space for context-data on context-stack
    sub rax, 120

    ; context.rip = func
    mov [rax + 104], r8

    ; save bottom address of context stack as 'limit'
    mov [rax + 16], rcx

    ; save address of context stack limit as 'dealloction stack'
    mov [rax + 8], rcx

    ; save top address of context stack as 'base'
    add rcx, rdx
    mov [rax + 24], rcx

    ; init context.retval(saved) = a writeable space (unused)
    ;
    ; it will write context (unused) and priv (unused) when jump to a new context function entry first
    ;;
    lea rcx, [rax + 152]
    mov [rax + 96], rcx

    ; context.end = the address of label __end
    lea rcx, __end
    mov [rax + 112], rcx

    ; return pointer to context-data
    ret 

__end:
    ; exit(0)
    xor rcx, rcx
    call _exit
    hlt

tb_context_make endp 

; jump context (refer to boost.context)
;
; @param retval        the from-context (rcx)
; @param context       the to-context (rdx)
; @param priv          the passed user private data (r8)
;
; @return              the from-context (retval: rcx)
;;
tb_context_jump proc frame

    ; .xdata for a function's structured exception handling unwind behavior
    .endprolog

    ; save the hidden argument: retval (from-context)
    push rcx

    ; save registers and construct the current context
    push rbp
    push rbx
    push rsi
    push rdi
    push r15
    push r14 
    push r13
    push r12

    ; load TIB
    mov r10, gs:[030h]

    ; save current stack base
    mov rax, [r10 + 08h]
    push rax

    ; save current stack limit
    mov rax, [r10 + 010h]
    push rax
    
    ; save current deallocation stack
    mov rax, [r10 + 01478h]
    push rax
    
    ; save fiber local storage
    mov rax, [r10 + 018h]
    push rax

    ; save the old context(esp) to r9
    mov r9, rsp

    ; switch to the new context(esp) and stack
    mov rsp, rdx

    ; load TIB
    mov r10, gs:[030h]

    ; restore fiber local storage
    pop rax
    mov [r10 + 018h], rax

    ; restore deallocation stack
    pop rax
    mov [r10 + 01478h], rax
    
    ; restore stack limit
    pop rax
    mov [r10 + 010h], rax
    
    ; restore stack base
    pop rax
    mov [r10 + 08h], rax

    ; restore registers of the new context
    pop r12
    pop r13
    pop r14
    pop r15
    pop rdi
    pop rsi
    pop rbx
    pop rbp

    ; restore retval (saved) to rax
    pop rax 

    ; restore the return or function address(r10)
    pop r10

    ; return from-context(retval: [rcx](context: r9, priv: r8)) from jump 
    ;
    ; it will write context (unused) and priv (unused) when jump to a new context function entry first
    ;;
    mov [rax], r9
    mov [rax + 8], r8

    ; pass old-context(ecx(context: r9, priv: r8)) arguments to the context function
    ;
    ; tb_context_from_t from;
    ; func(from)
    ;
    ; lea ecx, address of from
    ; ecx.context = r9
    ; ecx.priv = r8
    ; call func
    ;;
    mov rcx, rax

    ; jump to the return or function address(rip)
    ;
    ;
    ;                                     
    ;              -----------------------------------------------------------------
    ; context: .. |   end   |  unused  | context(unused) |  priv(unused)  | padding |
    ;              -----------------------------------------------------------------
    ;             0         8 arguments 
    ;             |         |
    ;            esp     16-align 
    ;           (now)
    ;;
    jmp r10

tb_context_jump endp 

end
