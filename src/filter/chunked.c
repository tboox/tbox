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
 * @file		chunked.c
 * @ingroup 	filter
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"chunked"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "filter.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the chunked filter type
typedef struct __tb_filter_chunked_t
{
	// the filter base
	tb_filter_t 			base;

	// the chunked size
	tb_size_t 				size;

	// the chunked read
	tb_size_t 				read;

	// the cache line
	tb_pstring_t 			line;

}tb_filter_chunked_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_filter_chunked_t* tb_filter_chunked_cast(tb_filter_t* filter)
{
	// check
	tb_assert_and_check_return_val(filter && filter->type == TB_FILTER_TYPE_CHUNKED, tb_null);
	return (tb_filter_chunked_t*)filter;
}
/* chunked_data
 *
 *   head     data   tail
 * ea5\r\n ..........\r\n e65\r\n..............\r\n 0\r\n\r\n
 * ---------------------- ------------------------- ---------
 *        chunk0                  chunk1               end
 */
static tb_long_t tb_filter_chunked_spak(tb_filter_t* filter, tb_bstream_t* istream, tb_bstream_t* ostream, tb_long_t sync)
{
	// check
	tb_filter_chunked_t* cfilter = tb_filter_chunked_cast(filter);
	tb_assert_and_check_return_val(cfilter && istream && ostream, -1);
	tb_assert_and_check_return_val(tb_bstream_valid(istream) && tb_bstream_valid(ostream), -1);

	// no data? continue or end
	if (!tb_bstream_left(istream)) return sync < 0? -1 : 0;

	// the idata
	tb_byte_t const* 	ip = tb_bstream_pos(istream);
	tb_byte_t const* 	ie = tb_bstream_end(istream);

	// the odata
	tb_byte_t* 			ob = (tb_byte_t*)tb_bstream_beg(ostream);
	tb_byte_t* 			op = (tb_byte_t*)tb_bstream_pos(ostream);
	tb_byte_t* 			oe = (tb_byte_t*)tb_bstream_end(ostream);

	// parse chunked head and chunked tail
	if (!cfilter->size || cfilter->read >= cfilter->size)
	{
		// walk
		while (ip < ie)
		{
			// the charactor
			tb_char_t ch = *ip++;

			// check
			tb_assert_and_check_return_val(ch, -1);
		
			// append char to line
			if (ch != '\n') tb_pstring_chrcat(&cfilter->line, ch);
			// is line end?
			else
			{
				// check
				tb_char_t const* 	pb = tb_pstring_cstr(&cfilter->line);
				tb_size_t 			pn = tb_pstring_size(&cfilter->line);
				tb_assert_and_check_return_val(pb, -1);

				// trace
				tb_trace_impl("line: %s", tb_pstring_cstr(&cfilter->line));

				// strip '\r' if exists
				if (pb[pn - 1] == '\r') tb_pstring_strip(&cfilter->line, pn - 1);

				// is chunked tail? only "\r\n"
				if (!tb_pstring_size(&cfilter->line)) 
				{
					// reset size
					cfilter->read = 0;
					cfilter->size = 0;

					// trace
					tb_trace_impl("tail");

					// end
					goto end;
				}
				// is chunked head? parse size
				else
				{
					// parse size
					cfilter->size = tb_s16tou32(pb);

					// trace
					tb_trace_impl("size: %lu", cfilter->size);

					// is file end? "0\r\n\r\n"
					tb_check_return_val(cfilter->size , -1);

					// clear data
					tb_pstring_clear(&cfilter->line);

					// ok
					break;
				}
			}
		}
	}

	// check
	tb_assert_and_check_return_val(cfilter->read < cfilter->size, -1);

	// read chunked data
	tb_size_t size = tb_min3(ie - ip, oe - op, cfilter->size - cfilter->read);
	if (size) 
	{
		// copy data
		tb_memcpy((tb_byte_t*)op, ip, size);
		ip += size;
		op += size;

		// update read
		cfilter->read += size;
	}

	// trace
	tb_trace_impl("read: %lu", cfilter->read);

end:

	// update stream
	tb_bstream_goto(istream, (tb_byte_t*)ip);
	tb_bstream_goto(ostream, (tb_byte_t*)op);

	// ok
	return (op - ob);
}
static tb_void_t tb_filter_chunked_cler(tb_filter_t* filter)
{
	// check
	tb_filter_chunked_t* cfilter = tb_filter_chunked_cast(filter);
	tb_assert_and_check_return(cfilter);

	// clear size
	cfilter->size = 0;

	// clear read
	cfilter->read = 0;

	// clear line
	tb_pstring_clear(&cfilter->line);
}
static tb_void_t tb_filter_chunked_exit(tb_filter_t* filter)
{
	// check
	tb_filter_chunked_t* cfilter = tb_filter_chunked_cast(filter);
	tb_assert_and_check_return(cfilter);

	// exit line
	tb_pstring_exit(&cfilter->line);
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_filter_t* tb_filter_init_from_chunked(tb_bool_t dechunked)
{
	// done
	tb_bool_t 				ok = tb_false;
	tb_filter_chunked_t* 	filter = tb_null;
	do
	{
		// noimpl for encoding chunked
		if (!dechunked)
		{
			tb_trace_noimpl();
			break;
		}

		// make filter
		filter = (tb_filter_chunked_t*)tb_malloc0(sizeof(tb_filter_chunked_t));
		tb_assert_and_check_break(filter);

		// init filter 
		if (!tb_filter_init((tb_filter_t*)filter, TB_FILTER_TYPE_CHUNKED)) break;
		filter->base.spak = tb_filter_chunked_spak;
		filter->base.cler = tb_filter_chunked_cler;
		filter->base.exit = tb_filter_chunked_exit;

		// init line
		if (!tb_pstring_init(&filter->line)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit filter
		tb_filter_exit((tb_filter_t*)filter);
		filter = tb_null;
	}

	// ok?
	return (tb_filter_t*)filter;
}

