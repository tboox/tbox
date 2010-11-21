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
 * \file		zstream.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "zstream.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_zstream_algo_t tb_zstream_algo(tb_zstream_t* zst)
{
	if (zst) return zst->algo;
	else return TB_ZSTREAM_ALGO_NULL;
}
tb_zstream_action_t tb_zstream_action(tb_zstream_t* zst)
{
	if (zst) return zst->action;
	else return TB_ZSTREAM_ACTION_NULL;
}
tb_char_t const* tb_zstream_name(tb_zstream_t* zst)
{
	TB_ASSERT(zst);
	if (!zst) return "";
	tb_char_t const* names[] =
	{
		""
	, 	"huffman"
	, 	"lz77"
	, 	"lzw"
	, 	"zlib"
	
	};

	tb_size_t n = TB_STATIC_ARRAY_SIZE(names);
	tb_size_t i = zst->algo;
	if (i < n) return names[i];
	else return "";
}

