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
 * \file		gamma.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "gamma.h"
#include "../../../math/math.h"

/* /////////////////////////////////////////////////////////
 * details
 */

static void tb_zstream_vlc_gamma_set(tb_zstream_vlc_t* vlc, tb_uint32_t val, tb_bstream_t* bst)
{
	TB_ASSERT(vlc && val);

	// compute q & r
	tb_uint32_t q = TB_MATH_ILOG2I(val);
	tb_uint32_t r = val - (1 << q);
	TB_ASSERT(q < 32);

	//TB_DBG("x: %d, q: %d, r: %d", val, q, r);

	// store
	tb_int_t i = 0;
	for (i = 0; i < q; i++) tb_bstream_set_u1(bst, 1);
	tb_bstream_set_u1(bst, 0);
	for (i = 0; i < q; i++, r >>= 1) tb_bstream_set_u1(bst, r & 0x1);
}
static tb_uint32_t tb_zstream_vlc_gamma_get(tb_zstream_vlc_t* vlc, tb_bstream_t const* bst)
{
	TB_ASSERT(vlc);

	// get q
	tb_uint32_t q = 0;
	while (tb_bstream_get_u1(bst)) q++;
	TB_ASSERT(q < 32);

	// get r
	tb_uint32_t i = 0;
	tb_uint32_t r = 0;
	for (i = 0; i < q; i++) r |= tb_bstream_get_u1(bst) << i;

	//TB_DBG("x: %d, q: %d, r: %d", r + (1 << q), q, r);

	return (r + (1 << q));
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_zstream_vlc_t* tb_zstream_vlc_gamma_open(tb_zstream_vlc_gamma_t* gamma)
{
	// init
	memset(gamma, 0, sizeof(tb_zstream_vlc_gamma_t));
	((tb_zstream_vlc_t*)gamma)->type = TB_ZSTREAM_VLC_TYPE_GAMMA;
	((tb_zstream_vlc_t*)gamma)->set = tb_zstream_vlc_gamma_set;
	((tb_zstream_vlc_t*)gamma)->get = tb_zstream_vlc_gamma_get;
	((tb_zstream_vlc_t*)gamma)->close = TB_NULL;

	/* gamma coding
	 *
	 * x = [1, ...)
	 * q = (int)log2(x)
	 * r = x - (1 << q)
	 *
	 * x      code
	 * --------------------
	 * 1       0
	 * 2      10 0
	 * 3      10 1
	 * 4     110 00
	 * 5     110 01
	 *
	 * code: 11111 ...  0    r
	 *        q-bits        q-bits
	 */
#if 0

	// make it
	tb_int_t 		x = 0;
	tb_int_t 		i = 0;
	tb_bstream_t 	bst;
	tb_byte_t 		d[4];
	for (x = 1; x <= 65535; x++)
	{
		tb_int_t q = TB_MATH_ILOG2I(x);
		tb_int_t r = x - (1 << q);

		// is out?
		if ((q << 1) + 1 > 16) break;

		tb_bstream_attach(&bst, d, 4);
		for (i = 0; i < q; i++) tb_bstream_set_u1(&bst, 1);
		tb_bstream_set_u1(&bst, 0);
		for (i = 0; i < q; i++, r >>= 1) tb_bstream_set_u1(&bst, r & 0x1);

		tplat_printf("x = 0x%04x, q = %d: ", x, q);
		tb_bstream_attach(&bst, d, 4);
		for (q = 0; tb_bstream_get_u1(&bst); q++) tplat_printf("1");
		tplat_printf("0 ");
		for (i = 0; i < q; i++) tplat_printf("%d", tb_bstream_get_u1(&bst));
		tplat_printf("\n");
	}

#endif

	return (tb_zstream_vlc_t*)gamma;
}
