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
 * @file		encoding.h
 * @defgroup 	encoding
 * @ingroup 	encoding
 *
 */
#ifndef TB_ENCODING_H
#define TB_ENCODING_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the encoding type
typedef enum __tb_encoding_t
{
	TB_ENCODING_ASCII 		= 0
, 	TB_ENCODING_GB2312 		= 1
, 	TB_ENCODING_GBK 		= 2
, 	TB_ENCODING_UTF8 		= 3
, 	TB_ENCODING_UNICODE 	= 4

}tb_encoding_t;

// the encoding encoder type
typedef struct __tb_encoder_t
{
	// the encoding type
	tb_uint8_t 			encoding;

	// the character range of unicode
	tb_uint32_t 		minchar;
	tb_uint32_t 		maxchar;

	// get unicode from the encoding string
	tb_size_t 			(*get)(tb_uint32_t* ch, tb_byte_t const** data, tb_size_t size);

	// set unicode to the encoding string
	tb_size_t 			(*set)(tb_uint32_t ch, tb_byte_t** data, tb_size_t size);

	// from unicode to the encoding character, not support utf-x
	tb_uint32_t 		(*from)(tb_uint32_t ch);

	// from the encoding character to unicode, not support utf-x
	tb_uint32_t 		(*to)(tb_uint32_t ch);

}tb_encoder_t;


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// get the encoder
tb_encoder_t const* 	tb_encoding_get_encoder(tb_size_t encoding);

// convert string
tb_size_t 				tb_encoding_convert_string(tb_size_t src_e, tb_size_t dst_e, tb_byte_t const* src_s, tb_size_t src_n, tb_byte_t* dst_s, tb_size_t dst_n);


#endif

