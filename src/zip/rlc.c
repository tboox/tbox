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
 *
 * \author		ruki
 * \file		rlc.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rlc.h"

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */
static __tb_inline__ tb_zip_rlc_t* tb_zip_rlc_cast(tb_zip_t* zip)
{
	tb_assert_and_check_return_val(zip && zip->algo == TB_ZIP_ALGO_RLC, TB_NULL);
	return (tb_zip_rlc_t*)zip;
}
static tb_long_t tb_zip_rlc_spak_deflate(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost, tb_bool_t sync)
{
	tb_zip_rlc_t* rlc = tb_zip_rlc_cast(zip);
	tb_assert_and_check_return_val(rlc && ist && ost, -1);

	// the input stream
	tb_byte_t* ip = ist->p;
	tb_byte_t* ie = ist->e;
	tb_check_return_val(ip && ip < ie, -1);

	// the output stream
	tb_byte_t* op = ost->p;
	tb_byte_t* oe = ost->e;
	tb_assert_and_check_return_val(op && oe, -1);

	// get vlc
	tb_zip_vlc_t* vlc = rlc->vlc;
	tb_assert_and_check_return_val(vlc && vlc->set, -1);

	// vlc callback
	tb_zip_vlc_set_t vlc_set = vlc->set;
	
	// get last & repeat
	tb_byte_t last = 0;
	tb_size_t repeat = 1;
	if (!rlc->repeat)
	{
		last = *ip++;
		repeat = 1;
	}
	else 
	{
		last = rlc->last;
		repeat = rlc->repeat;
	}

	// deflate 
	for (; ip <= ie; ip++)
	{
		// update repeat
		if (*ip != last || ip == ie) 
		{
			// fill output, [FIXME]: vlc < 24?
			if (tb_bstream_left_bits(ost) > 32) 
			{
				if (repeat > 1)
				{
					// set flag
					tb_bstream_set_u1(ost, 1);

					// set repeat
					vlc_set(vlc, repeat, ost);
					
					// set value
					tb_bstream_set_ubits32(ost, last, 8);

					//tb_trace("repeat(0x%02x): %d", last, repeat);
				}
				else
				{
					tb_assert(repeat == 1);

					// set flag
					tb_bstream_set_u1(ost, 0);

					// set value
					tb_bstream_set_ubits32(ost, last, 8);
				}
			}

			// input is end?
			if (ip == ie) break;

			// reset
			last = *ip;
			repeat = 1;
		}
		else repeat++;
	}

	// sync 
	//tb_bstream_sync(ost);

	// update 
	ist->p = ip;
	rlc->last = last;
	rlc->repeat = repeat;

	// ok
	return (ost->p - op);
}
static tb_long_t tb_zip_rlc_spak_inflate(tb_zip_t* zip, tb_bstream_t* ist, tb_bstream_t* ost, tb_bool_t sync)
{
	tb_zip_rlc_t* rlc = tb_zip_rlc_cast(zip);
	tb_assert_and_check_return_val(rlc && ist && ost, -1);

	// the input stream
	tb_byte_t* ip = ist->p;
	tb_byte_t* ie = ist->e;
	tb_assert_and_check_return_val(ip && ie, -1);

	// the output stream
	tb_byte_t* ob = ost->p;
	tb_byte_t* op = ost->p;
	tb_byte_t* oe = ost->e;
	tb_assert_and_check_return_val(op && oe, -1);

	// get vlc
	tb_zip_vlc_t* vlc = rlc->vlc;
	tb_assert_and_check_return_val(vlc && vlc->get, -1);

	// vlc callback
	tb_zip_vlc_get_t vlc_get = vlc->get;

	// fill the left bytes
	tb_int_t 	repeat = rlc->repeat;
	tb_byte_t 	last = rlc->last;
	while (repeat-- > 0 && op < oe) *op++ = last;

	// inflate, [FIXME]: vlc < 24?
	while (tb_bstream_left_bits(ist) > 32 && op < oe)
	{
		// get flag
		if (tb_bstream_get_u1(ist))
		{
			// get repeat
			repeat = vlc_get(vlc, ist);

			// get value
			last = tb_bstream_get_ubits32(ist, 8);

			//tb_trace("repeat(0x%02x): %d", last, repeat);

			// fill bytes
			while (repeat-- > 0 && op < oe) *op++ = last;
	
		}
		else if (op < oe) *op++ = tb_bstream_get_ubits32(ist, 8);
	}

	// update 
	ost->p = op;
	rlc->last = last;
	rlc->repeat = repeat;

	// ok
	return (op - ob);
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_t* tb_zip_rlc_init(tb_size_t action)
{
	// alloc
	tb_zip_t* zip = (tb_zip_t*)tb_malloc0(sizeof(tb_zip_rlc_t));
	tb_assert_and_check_return_val(zip, TB_NULL);
	
	// init zip
	zip->algo 		= TB_ZIP_ALGO_RLC;
	zip->action 	= action;
	zip->spak 		= (action == TB_ZIP_ACTION_INFLATE)? tb_zip_rlc_spak_inflate : tb_zip_rlc_spak_deflate;

	// open vlc
#if TB_ZIP_RLC_VLC_TYPE_GOLOMB
	((tb_zip_rlc_t*)zip)->vlc = tb_zip_vlc_golomb_open(&(zip->vlc), 4);
#elif TB_ZIP_RLC_VLC_TYPE_GAMMA
	((tb_zip_rlc_t*)zip)->vlc = tb_zip_vlc_gamma_open(&(zip->vlc));
#else
	((tb_zip_rlc_t*)zip)->vlc = tb_zip_vlc_fixed_open(&(zip->vlc), 16);
#endif

	// check vlc
	tb_assert_and_check_goto(((tb_zip_rlc_t*)zip)->vlc, fail);

	// ok
	return zip;

fail:
	if (zip) tb_free(zip);
	return TB_NULL;
}
tb_void_t tb_zip_rlc_exit(tb_zip_t* zip)
{
	tb_zip_rlc_t* rlc = tb_zip_rlc_cast(zip);
	if (rlc) 
	{
		// close vlc
		if (rlc->vlc && rlc->vlc->close) rlc->vlc->close(rlc->vlc); 

		// free it
		tb_free(rlc);
	}
}

