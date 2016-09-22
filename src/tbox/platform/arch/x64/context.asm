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

option casemap :none 
.code

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; implementation
;;

; make context
;
; @param stackdata     the stack data (esp + 4)
; @param stacksize     the stack size (esp + 8)
; @param func          the entry function (esp + 12)
;
; @return              the context pointer (eax)
;;
tb_context_make proc 


    ret
tb_context_make endp 

; jump context
;
; @param context       the to-context (esp + 4)
; @param priv          the passed user private data (esp + 8)
;
; @return              the from-context (context: eax, priv: edx)
;;
tb_context_jump proc 

    ret
tb_context_jump endp 

end
