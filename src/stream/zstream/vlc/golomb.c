/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		golomb.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "golomb.h"
#include "../../../math/math.h"

/* /////////////////////////////////////////////////////////
 * details
 */

static void tb_zstream_vlc_golomb_set(tb_zstream_vlc_t* vlc, tb_uint32_t val, tb_bstream_t* bst)
{
	TB_ASSERT(vlc && val);

#if 1
	tb_size_t avg = 0;
	if (((tb_zstream_vlc_golomb_t*)vlc)->count)
		avg = ((tb_zstream_vlc_golomb_t*)vlc)->total / ((tb_zstream_vlc_golomb_t*)vlc)->count;
	//TB_DBG("%d %d", avg, ((tb_zstream_vlc_golomb_t*)vlc)->count);

	((tb_zstream_vlc_golomb_t*)vlc)->defm = TB_MATH_IRLOG2I(avg);
#endif

	// compute q & r
	tb_int_t m = ((tb_zstream_vlc_golomb_t*)vlc)->defm;
	tb_int_t b = 1 << m;
	tb_int_t q = (tb_int_t)((val - 1) / b);
	tb_int_t r = val - 1 - q * b;

	//TB_DBG("x: %d, q: %d, m: %d, r: %d", val, q, m, r);

	// store
	tb_int_t i = 0;
	for (i = 0; i < q; i++) tb_bstream_set_u1(bst, 1);
	tb_bstream_set_u1(bst, 0);
	for (i = 0; i < m; i++, r >>= 1) tb_bstream_set_u1(bst, r & 0x1);

#if 1
	((tb_zstream_vlc_golomb_t*)vlc)->total += val;
	((tb_zstream_vlc_golomb_t*)vlc)->count++;
#endif
}
static tb_uint32_t tb_zstream_vlc_golomb_get(tb_zstream_vlc_t* vlc, tb_bstream_t const* bst)
{
	TB_ASSERT(vlc);

#if 1
	tb_size_t avg = 0;
	if (((tb_zstream_vlc_golomb_t*)vlc)->count)
		avg = ((tb_zstream_vlc_golomb_t*)vlc)->total / ((tb_zstream_vlc_golomb_t*)vlc)->count;
	//TB_DBG("%d %d", avg, ((tb_zstream_vlc_golomb_t*)vlc)->count);

	((tb_zstream_vlc_golomb_t*)vlc)->defm = TB_MATH_IRLOG2I(avg);
#endif

	// get q
	tb_uint32_t q = 0;
	while (tb_bstream_get_u1(bst)) q++;

	// get b
	tb_int_t m = ((tb_zstream_vlc_golomb_t*)vlc)->defm;
	tb_int_t b = 1 << m;

	// get r
	tb_uint32_t i = 0;
	tb_uint32_t r = 0;
	for (i = 0; i < m; i++) r |= tb_bstream_get_u1(bst) << i;

#if 1
	((tb_zstream_vlc_golomb_t*)vlc)->total += (r + q * b + 1);
	((tb_zstream_vlc_golomb_t*)vlc)->count++;
#endif

	//TB_DBG("x: %d, q: %d, m: %d, r: %d", (r + q * b + 1), q, m, r);

	return (r + q * b + 1);
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_zstream_vlc_t* tb_zstream_vlc_golomb_open(tb_zstream_vlc_golomb_t* golomb, tb_size_t defm)
{
	// init
	memset(golomb, 0, sizeof(tb_zstream_vlc_golomb_t));
	((tb_zstream_vlc_t*)golomb)->type = TB_ZSTREAM_VLC_TYPE_GOLOMB;
	((tb_zstream_vlc_t*)golomb)->set = tb_zstream_vlc_golomb_set;
	((tb_zstream_vlc_t*)golomb)->get = tb_zstream_vlc_golomb_get;
	((tb_zstream_vlc_t*)golomb)->close = TB_NULL;
	golomb->defm = defm;
	golomb->total = 0;
	golomb->count = 0;

	/* golomb coding
	 *
	 * x = [1, ...)
	 * b = 1 << m
	 * q = (int)((x - 1) / b)
	 * r = x - 1 - q * b
	 *
	 *     x          m = 0       m = 1       m = 2       m = 3
	 * -------------------------------------------------------------
	 *     1             0         0 0        0 00        0 000
	 *     2            10         0 1        0 01        0 001
	 *     3           110        10 0        0 10        0 010
	 *     4          1110        10 1        0 11        0 011
	 *     5         11110       110 0       10 00        0 100
	 *
	 * code: 11111.... 0     r
	 *        q-bits       m-bits
	 *
	 */
#if 0

	// make it
	tb_int_t 		x = 0;
	tb_int_t 		i = 0;
	tb_int_t 		m = defm;
	tb_bstream_t 	bst;
	tb_byte_t 		d[8];
	for (x = 1; x <= 65535; x++)
	{
		tb_int_t b = 1 << m;
		tb_int_t q = (tb_int_t)((x - 1) / b);
		tb_int_t r = x - 1 - q * b;

		// is out?
		if (q + 1 + m > (8 << 3)) break;

		tb_bstream_attach(&bst, d, 8);
		for (i = 0; i < q; i++) tb_bstream_set_u1(&bst, 1);
		tb_bstream_set_u1(&bst, 0);
		for (i = 0; i < m; i++, r >>= 1) tb_bstream_set_u1(&bst, r & 0x1);

		tplat_printf("x = 0x%04x, q = %d, m = %d: ", x, q, m);
		tb_bstream_attach(&bst, d, 8);
		while (tb_bstream_get_u1(&bst)) tplat_printf("1");
		tplat_printf("0 ");
		for (i = 0; i < m; i++) tplat_printf("%d", tb_bstream_get_u1(&bst));
		tplat_printf("\n");
	}

#endif

	return (tb_zstream_vlc_t*)golomb;
}
