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
 * This is a modified version based on adler32.c from the zlib library.
 * Copyright (C) 1995 - 2011 Mark adler
 *
 * @author      ruki
 * @file        adler32.c
 * @ingroup     utils
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "adler32.h"
#ifdef TB_CONFIG_PACKAGE_HAVE_ZLIB
#   include "zlib/zlib.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// largest prime smaller than 65536
#define BASE (65521)

// NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
#define NMAX (5552)

// dot
#define DO1(data, i)    {adler += (data)[i]; sum2 += adler;}
#define DO2(data, i)    DO1(data, i); DO1(data, i + 1);
#define DO4(data, i)    DO2(data, i); DO2(data, i + 2);
#define DO8(data, i)    DO4(data, i); DO4(data, i + 4);
#define DO16(data)      DO8(data, 0); DO8(data, 8);

// mod
#define MOD(a)          (a) %= BASE
#define MOD28(a)        (a) %= BASE
#define MOD63(a)        (a) %= BASE

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_uint32_t tb_adler32_encode(tb_uint32_t adler, tb_byte_t const* data, tb_size_t size)
{
#ifdef TB_CONFIG_PACKAGE_HAVE_ZLIB
    return adler32(adler, data, size);
#else
    // split adler-32 into component sums 
    tb_size_t sum2 = (adler >> 16) & 0xffff; adler &= 0xffff;

    // in case user likes doing a byte at a time, keep it fast 
    if (size == 1) 
    {
        adler += data[0];
        if (adler >= BASE) adler -= BASE;

        sum2 += adler;
        if (sum2 >= BASE) sum2 -= BASE;

        return adler | (sum2 << 16);
    }

    // initial adler-32 value (deferred check for size == 1 speed) 
    tb_check_return_val(data, 1);

    // in case short lengths are provided, keep it somewhat fast 
    if (size < 16) 
    {
        // done
        while (size--)
        {
            adler += *data++;
            sum2 += adler;
        }
        if (adler >= BASE) adler -= BASE;

        // only added so many BASE's 
        MOD28(sum2);            
        return adler | (sum2 << 16);
    }

    // do length NMAX blocks -- requires just one modulo operation 
    tb_size_t n;
    while (size >= NMAX)
    {
        size -= NMAX;

        // NMAX is divisible by 16 
        n = NMAX / 16;          
        do
        {
            // 16 sums unrolled 
            DO16(data);          
            data += 16;

        } while (--n);

        MOD(adler);
        MOD(sum2);
    }

    // do remaining bytes (less than NMAX, still just one modulo) 
    if (size) 
    {                  
        // avoid modulos if none remaining 
        while (size >= 16)
        {
            size -= 16;
            DO16(data);
            data += 16;
        }
        while (size--)
        {
            adler += *data++;
            sum2 += adler;
        }
        MOD(adler);
        MOD(sum2);
    }

    // return recombined sums 
    return adler | (sum2 << 16);
#endif
}
