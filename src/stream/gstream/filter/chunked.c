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
 * @author		ruki
 * @file		kstream.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"chunked"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "filter.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the chunked stream type
typedef struct __tb_gstream_filter_chunked_t
{
	// the stream base
	tb_gstream_filter_t 			base;

	// the chunked size
	tb_size_t 				size;

	// the chunked read
	tb_size_t 				read;

	// the cache line
	tb_pstring_t 			line;

}tb_gstream_filter_chunked_t;

/* ///////////////////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_gstream_filter_chunked_t* tb_gstream_filter_chunked_cast(tb_gstream_t* gst)
{
	// check
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(filter && filter->type == TB_GSTREAM_FLTR_TYPE_CHUNKED, tb_null);
	return (tb_gstream_filter_chunked_t*)filter;
}
static tb_long_t tb_gstream_filter_chunked_aopen(tb_gstream_t* gst)
{
	// check
	tb_gstream_filter_chunked_t* kst = tb_gstream_filter_chunked_cast(gst);
	tb_assert_and_check_return_val(kst, -1);

	// init 
	kst->size = 0;
	kst->read = 0;
	if (!tb_pstring_init(&kst->line)) return -1;

	// open filter
	return tb_gstream_filter_aopen(gst);
}
static tb_long_t tb_gstream_filter_chunked_aclose(tb_gstream_t* gst)
{
	// check
	tb_gstream_filter_chunked_t* kst = tb_gstream_filter_chunked_cast(gst);
	tb_assert_and_check_return_val(kst, -1);

	// reset 
	kst->size = 0;
	kst->read = 0;
	tb_pstring_exit(&kst->line);

	// close filter
	return tb_gstream_filter_aclose(gst);
}
/* chunked_data
 *
 *   head     data   tail
 * ea5\r\n ..........\r\n e65\r\n..............\r\n 0\r\n\r\n
 * ---------------------- ------------------------- ---------
 *        chunk0                  chunk1               end
 */
static tb_long_t tb_gstream_filter_chunked_spak(tb_gstream_t* gst, tb_long_t sync)
{
	// check
	tb_gstream_filter_chunked_t* kst = tb_gstream_filter_chunked_cast(gst);
	tb_gstream_filter_t* filter = tb_gstream_filter_cast(gst);
	tb_assert_and_check_return_val(kst && filter, -1);

	// the input
	tb_assert_and_check_return_val(filter->ip, -1);
	tb_byte_t const* 	ib = filter->ip;
	tb_byte_t const* 	ip = filter->ip;
	tb_byte_t const* 	ie = ip + filter->in;
	tb_check_return_val(ip < ie, 0);

	// the output
	tb_assert_and_check_return_val(filter->op, -1);
	tb_byte_t* 			ob = filter->op;
	tb_byte_t* 			op = filter->op;
	tb_byte_t const* 	oe = filter->ob + TB_GSTREAM_FLTR_CACHE_MAXN;
	tb_check_return_val(op < oe, 0);

	// parse chunked head and chunked tail
	if (!kst->size || kst->read >= kst->size)
	{
		// walk
		while (ip < ie)
		{
			// the charactor
			tb_char_t ch = *ip++;

			// check
			tb_assert_and_check_return_val(ch, -1);
		
			// append char to line
			if (ch != '\n') tb_pstring_chrcat(&kst->line, ch);
			// is line end?
			else
			{
				// check
				tb_char_t const* 	pb = tb_pstring_cstr(&kst->line);
				tb_size_t 			pn = tb_pstring_size(&kst->line);
				tb_assert_and_check_return_val(pb, -1);

				// trace
//				tb_trace_impl("line: %s", tb_pstring_cstr(&kst->line));

				// strip '\r' if exists
				if (pb[pn - 1] == '\r') tb_pstring_strip(&kst->line, pn - 1);

				// is chunked tail? only "\r\n"
				if (!tb_pstring_size(&kst->line)) 
				{
					// reset size
					kst->read = 0;
					kst->size = 0;

					// trace
					tb_trace_impl("tail");

					// continue
					goto end;
				}
				// is chunked head? parse size
				else
				{
					// parse size
					kst->size = tb_s16tou32(pb);

					// trace
					tb_trace_impl("size: %lu", kst->size);

					// is file end? "0\r\n\r\n"
					tb_check_return_val(kst->size , -1);

					// clear data
					tb_pstring_clear(&kst->line);

					// ok
					break;
				}
			}
		}
	}

	// check
	tb_assert_and_check_return_val(kst->read < kst->size, -1);

	// read chunked data
	tb_size_t size = tb_min3(ie - ip, oe - op, kst->size - kst->read);
	if (size) 
	{
		// copy data
		tb_memcpy(op, ip, size);
		ip += size;
		op += size;

		// update read
		kst->read += size;
	}

	// trace
	tb_trace_impl("read: %lu", kst->read);

end:

	// check
	tb_assert_and_check_return_val(ip >= ib && ip <= ie, -1);
	tb_assert_and_check_return_val(op >= ob && op <= oe, -1);

	// update input
	filter->in -= ip - ib;
	filter->ip = ip;

	// update output
	filter->on += op - ob;

	// ok
	return (op - ob);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_init_filter_chunked()
{
	tb_gstream_t* gst = (tb_gstream_t*)tb_malloc0(sizeof(tb_gstream_filter_chunked_t));
	tb_assert_and_check_return_val(gst, tb_null);

	// init base
	if (!tb_gstream_init(gst)) goto fail;

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_FLTR;
	gst->aopen 	= tb_gstream_filter_chunked_aopen;
	gst->aread 	= tb_gstream_filter_aread;
	gst->aclose	= tb_gstream_filter_chunked_aclose;
	gst->wait	= tb_gstream_filter_wait;
	gst->ctrl	= tb_gstream_filter_ctrl;

	// init filter
	((tb_gstream_filter_t*)gst)->type 	= TB_GSTREAM_FLTR_TYPE_CHUNKED;
	((tb_gstream_filter_t*)gst)->spak = tb_gstream_filter_chunked_spak;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_exit(gst);
	return tb_null;
}

tb_gstream_t* tb_gstream_init_filter_from_chunked(tb_gstream_t* gst)
{
	// check
	tb_assert_and_check_return_val(gst, tb_null);

	// init the chunked stream
	tb_gstream_t* kst = tb_gstream_init_filter_chunked();
	tb_assert_and_check_return_val(kst, tb_null);

	// set gstream
	if (!tb_gstream_ctrl(kst, TB_GSTREAM_CTRL_FLTR_SET_GSTREAM, gst)) goto fail;
	
	// ok
	return kst;

fail:
	if (kst) tb_gstream_exit(kst);
	return tb_null;
}
