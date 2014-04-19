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
 * @author		ruki
 * @file		ipv4.c 
 * @ingroup 	network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ipv4.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_void_t tb_ipv4_clr(tb_ipv4_t* ipv4)
{
	if (ipv4) ipv4->u32 = 0;
}
tb_uint32_t tb_ipv4_set(tb_ipv4_t* ipv4, tb_char_t const* ip)
{
	// init
	tb_size_t b0, b1, b2, b3;
	tb_char_t const* p = ip;
	tb_check_goto(p, fail);

	// b0
	tb_check_goto(*p && tb_isdigit(*p), fail);
	b0 = tb_stou32(p);
	tb_check_goto(b0 < 256, fail);

	while (*p && *p != '.') p++;
	tb_check_goto(*p, fail);
	p++;

	// b1
	tb_check_goto(*p && tb_isdigit(*p), fail);
	b1 = tb_stou32(p);
	tb_check_goto(b1 < 256, fail);

	while (*p && *p != '.') p++;
	tb_check_goto(*p, fail);
	p++;

	// b2
	tb_check_goto(*p && tb_isdigit(*p), fail);
	b2 = tb_stou32(p);
	tb_check_goto(b2 < 256, fail);

	while (*p && *p != '.') p++;
	tb_check_goto(*p, fail);
	p++;

	// b3
	tb_check_goto(*p && tb_isdigit(*p), fail);
	b3 = tb_stou32(p);
	tb_check_goto(b3 < 256, fail);

	// ok
	if (ipv4) 
	{
		ipv4->u8[0] = (tb_uint8_t)b0;
		ipv4->u8[1] = (tb_uint8_t)b1;
		ipv4->u8[2] = (tb_uint8_t)b2;
		ipv4->u8[3] = (tb_uint8_t)b3;
		return ipv4->u32;
	}
	else
	{
		tb_ipv4_t v4;
		v4.u8[0] = (tb_uint8_t)b0;
		v4.u8[1] = (tb_uint8_t)b1;
		v4.u8[2] = (tb_uint8_t)b2;
		v4.u8[3] = (tb_uint8_t)b3;
		return v4.u32;
	}

fail:
	if (ipv4) ipv4->u32 = 0;
	return 0;
}
tb_char_t const* tb_ipv4_get(tb_ipv4_t const* ipv4, tb_char_t* data, tb_size_t maxn)
{
	tb_assert_and_check_return_val(ipv4 && data && maxn > 15, tb_null);

	// format
	tb_size_t size = tb_snprintf(data, maxn, "%u.%u.%u.%u", ipv4->u8[0], ipv4->u8[1], ipv4->u8[2], ipv4->u8[3]);
	data[size] = '\0';

	// ok
	return data;
}

