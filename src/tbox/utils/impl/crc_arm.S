/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        crc_arm.S
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../prefix/prefix.S"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifndef TB_ARCH_ARM64

    /* tb_uint32_t tb_crc32_encode_asm(tb_uint32_t crc, tb_byte_t const* data, tb_size_t size, tb_uint32_t const* table);
     * 
     * @param crc   r0
     * @param data  r1
     * @param size  r2
     * @param table r3
     *
     * @return      crc, r0
     */
function tb_crc32_encode_asm, export=1

    // enter
    mov ip, sp
    stmfd sp!, {r4-r9, sl, fp, ip, lr, pc}
    sub fp, ip, #4

    // calc crc for the odd address
label odd                               // while (1) { 
    ands r4, r1, #3                     // if (!(ib & 0x3)) goto even; 
    beq even
    subs r2, r2, #1                     // if (--in < 0) goto end; 
    blt end
                                        // calc: crc = table[(*ib++ ^ crc) & 0xff] ^ (crc >> 8) 
    ldrb r4, [r1], #1                   // r4 = *ib++ 
    eor r4, r0, r4                      // r4 ^= crc 
    and r4, r4, #255                    // r4 &= 0xff 
    ldr r4, [r3, r4, lsl #2]            // r4 = table[r4] 
    eor r0, r4, r0, lsr #8              // crc = r4 ^ (crc >> 8) 
    b odd                               // } 

label evenloop
    // ib[0-7] => r4-r9, sl, ip, ib += 8 * sizeof(uint32) 
    ldmia r1!, {r4 - r9, sl, ip}        

    // calc: for r4
    eor lr, r0, r4                      // calc: crc = table[((r4 >> 0) ^ crc) & 0xff] ^ (crc >> 8) 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2] 
    eor r0, lr, r0, lsr #8  

    eor lr, r0, r4, lsr #8              // calc: crc = table[((r4 >> 8) ^ crc) & 0xff] ^ (crc >> 8) 
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r4, lsr #16             // calc: crc = table[((r4 >> 16) ^ crc) & 0xff] ^ (crc >> 8) 
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r4, lsr #24             // calc: crc = table[((r4 >> 24) ^ crc) & 0xff] ^ (crc >> 8) 
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    // calc: for r5 
    eor lr, r0, r5                      
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r5, lsr #8
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r5, lsr #16
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r5, lsr #24
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    // calc: for r6 
    eor lr, r0, r6                      
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r6, lsr #8
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r6, lsr #16
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r6, lsr #24
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    // calc: for r7 
    eor lr, r0, r7                      
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r7, lsr #8
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r7, lsr #16
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, r7, lsr #24
    and lr, lr, #255
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    // calc: for r8 
    eor lr, r0 ,r8                      
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0 ,r8, lsr #8  
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0 ,r8, lsr #16 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0 ,r8, lsr #24 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    // calc: for r9 
    eor lr, r0 ,r9                      
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0 ,r9, lsr #8  
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0 ,r9, lsr #16 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0 ,r9, lsr #24 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    // calc: for sl 
    eor lr, r0, sl                      
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, sl, lsr #8  
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, sl, lsr #16 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, sl, lsr #24 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    // calc: for ip 
    eor lr, r0, ip                      
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, ip, lsr #8  
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, ip, lsr #16 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

    eor lr, r0, ip, lsr #24 
    and lr, lr, #255    
    ldr lr, [r3, lr, lsl #2]
    eor r0, lr, r0, lsr #8

label even
    // in -= 32; if (in >= 0) goto evenloop; 
    subs r2, r2, #32                    
    bge evenloop

    // left: in += 32; if (in < 0) goto end; 
    adds r2, r2, #32                    
    ble end

    // calc: the left data crc 
label left                              // do { 
    ldrb r4, [r1], #1                   // calc: crc = table[(*ib++ ^ crc) & 0xff] ^ (crc >> 8) 
    eor r4, r0, r4                  
    and r4, r4, #255    
    ldr r4, [r3, r4, lsl #2]
    eor r0, r4, r0, lsr #8

    subs r2, r2, #1                     // } while (--in) 
    bne left

label end
    // leave
    ldmea fp, {r4 - r9, sl, fp, sp, pc}

endfunc

#endif
