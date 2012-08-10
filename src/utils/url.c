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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 * *
 * @author		ruki
 * @file		url.c
 * @ingroup 	utils
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "url.h"
#include "../libc/libc.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_url_encode(tb_char_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on)
{
	// init
	tb_char_t const* 	ip = ib;
	tb_char_t* 			op = ob;
	tb_char_t const* 	ie = ib + in;
	tb_char_t const* 	oe = ob + on;
	static tb_char_t 	ht[] = "0123456789ABCDEF";

	// walk
	while (ip < ie && op < oe) 
	{
		// character
		tb_byte_t c = *ip++;

		// space?
		if (c == ' ') *op++ = '+';
		// %xx?
		else if ((c < '0' && c != '-' && c != '.') ||(c < 'A' && c > '9') ||(c > 'Z' && c < 'a' && c != '_') || (c > 'z'))
		{
			op[0] = '%';
			op[1] = ht[c >> 4];
			op[2] = ht[c & 15];
			op += 3;
		} 
		else *op++ = c;
	}

	// end
	*op = '\0';

	// ok
	return op - ob;
}
tb_size_t tb_url_decode(tb_char_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on)
{
	// init
	tb_char_t const* 	ip = ib;
	tb_char_t* 			op = ob;
	tb_char_t const* 	ie = ib + in;
	tb_char_t const* 	oe = ob + on;

	// walk
	while (ip < ie && op < oe) 
	{
		// space?
		if (*ip == '+') *op = ' ';
		// %xx?
		else if (*ip == '%' && ip + 2 < ie && tb_isdigit16(ip[1]) && tb_isdigit16(ip[2]))
		{
			*op = (tb_char_t)tb_s16tou32(&ip[1]);
			ip += 2;
		}
		else *op = *ip;

		// next
		ip++;
		op++;
	}

	// end
	*op = '\0';

	// ok
	return op - ob;
}
