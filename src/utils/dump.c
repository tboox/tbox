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
 * @file		dump.c
 * @ingroup 	utils
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dump.h"
#include "../asio/asio.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_dump_data(tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(data && size);

	// init stream
	tb_basic_stream_t* gst = tb_basic_stream_init_from_data(data, size);
	if (gst)
	{
		// open stream
		if (tb_basic_stream_open(gst))
		{
			// dump stream
			tb_dump_data_from_stream(gst);
		}
	
		// exit stream
		tb_basic_stream_exit(gst);
	}
}
tb_void_t tb_dump_data_from_url(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return(url);

	// init stream
	tb_basic_stream_t* gst = tb_basic_stream_init_from_url(url);
	if (gst)
	{
		// open stream
		if (tb_basic_stream_open(gst))
		{
			// dump stream
			tb_dump_data_from_stream(gst);
		}
	
		// exit stream
		tb_basic_stream_exit(gst);
	}
}
tb_void_t tb_dump_data_from_stream(tb_basic_stream_t* gst)
{
	// check
	tb_assert_and_check_return(gst);

	// init 
	tb_size_t offset = 0;

	// dump head
	tb_tracef_i("");
	tb_size_t i = 0;
	tb_size_t n = 147;
	for (i = 0; i < n; i++) tb_tracet_i("=");
	tb_tracet_i(__tb_newline__);

	// walk
	while (!tb_stream_beof(gst))
	{
		// read line
		tb_long_t read = 0;
		tb_byte_t line[0x20];
		while (read < 0x20)
		{
			// read data
			tb_long_t real = tb_basic_stream_read(gst, line + read, 0x20 - read);
			// has data?
			if (real > 0) read += real;
			// no data?
			else if (!real)
			{
				// wait
				tb_long_t e = tb_basic_stream_wait(gst, TB_AIOE_CODE_RECV, tb_stream_timeout(gst));
				tb_assert_and_check_break(e >= 0);

				// timeout?
				tb_check_break(e);

				// has read?
				tb_assert_and_check_break(e & TB_AIOE_CODE_RECV);
			}
			else break;
		}

		// full line?
		if (read == 0x20)
		{
			// dump offset
			tb_tracef_i("");
			tb_tracet_i("%08X ", offset);

			// dump data
			for (i = 0; i < 0x20; i++)
			{
				if (!(i & 3)) tb_tracet_i(" ");
				tb_tracet_i(" %02X", line[i]);
			}

			// dump spaces
			tb_tracet_i("  ");

			// dump characters
			for (i = 0; i < 0x20; i++)
			{
				tb_tracet_i("%c", tb_isgraph(line[i])? line[i] : '.');
			}

			// dump new line
			tb_tracet_i(__tb_newline__);

			// update offset
			offset += 0x20;
		}
		// has left?
		else if (read)
		{
			// init padding
			tb_size_t padding = n - 0x20;

			// dump offset
			tb_tracef_i("");
			tb_tracet_i("%08X ", offset); 
			if (padding >= 9) padding -= 9;

			// dump data
			for (i = 0; i < read; i++)
			{
				if (!(i & 3)) 
				{
					tb_tracet_i(" ");
					if (padding) padding--;
				}

				tb_tracet_i(" %02X", line[i]);
				if (padding >= 3) padding -= 3;
			}

			// dump spaces
			while (padding--) tb_tracet_i(" ");
				
			// dump characters
			for (i = 0; i < read; i++)
			{
				tb_tracet_i("%c", tb_isgraph(line[i])? line[i] : '.');
			}

			// dump new line
			tb_tracet_i(__tb_newline__);

			// update offset
			offset += read;
		}
		// end
		else break;
	}
}
