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
 * @file		prefix.h
 * @ingroup 	object
 *
 */
#ifndef TB_OBJECT_PREFIX_H
#define TB_OBJECT_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../xml/xml.h"
#include "../stream/stream.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// writ tab
#define tb_object_writ_tab(gst, deflate, tab) 	if (!(deflate)) { tb_size_t t = (tab); while (t--) tb_gstream_printf((gst), "\t"); } 

// writ '\n'
#define tb_object_writ_newline(gst, deflate) 	if (!(deflate)) tb_gstream_printf((gst), "\n"); 

// bytes
#define tb_object_need_bytes(x) 				\
												(((tb_uint64_t)(x)) < (1ull << 8) ? 1 : \
												(((tb_uint64_t)(x)) < (1ull << 16) ? 2 : \
												(((tb_uint64_t)(x)) < (1ull << 24) ? 3 : \
												(((tb_uint64_t)(x)) < (1ull << 32) ? 4 : 8))))

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the object type enum
typedef enum __tb_object_type_e
{
	TB_OBJECT_TYPE_NONE 		= 0
,	TB_OBJECT_TYPE_DATA 		= 1
,	TB_OBJECT_TYPE_DATE 		= 2
,	TB_OBJECT_TYPE_ARRAY 		= 3
,	TB_OBJECT_TYPE_STRING 		= 4
,	TB_OBJECT_TYPE_NUMBER 		= 5
,	TB_OBJECT_TYPE_BOOLEAN 		= 6
,	TB_OBJECT_TYPE_DICTIONARY 	= 7
,	TB_OBJECT_TYPE_NULL 		= 8
,	TB_OBJECT_TYPE_USER 		= 9 //!< the user defined type, ...

}tb_object_type_e;

/// the object flag enum
typedef enum __tb_object_flag_e
{
	TB_OBJECT_FLAG_NONE 		= 0
,	TB_OBJECT_FLAG_READONLY 	= 1
,	TB_OBJECT_FLAG_SINGLETON 	= 2

}tb_object_flag_e;

/// the object format enum
typedef enum __tb_object_format_e
{
	TB_OBJECT_FORMAT_NONE 		= 0x0000 	//!< none
,	TB_OBJECT_FORMAT_XML 		= 0x0001 	//!< the xml format
,	TB_OBJECT_FORMAT_BIN 		= 0x0002 	//!< the tbox format
,	TB_OBJECT_FORMAT_JSN 		= 0x0003 	//!< the json format
,	TB_OBJECT_FORMAT_DEFLATE 	= 0x0100 	//!< deflate?

}tb_object_format_e;

/// the object type
typedef struct __tb_object_t
{
	/// the object flag
	tb_size_t 				flag 	: 1;

	/// the object type
	tb_size_t 				type 	: 31;

	/// the object reference count
	tb_size_t 				refn;

	/// the object private data
	tb_cpointer_t 			priv;

	/// the copy func
	struct __tb_object_t* 	(*copy)(struct __tb_object_t* object);

	/// the cler func
	tb_void_t 				(*cler)(struct __tb_object_t* object);

	/// the exit func
	tb_void_t 				(*exit)(struct __tb_object_t* object);

}tb_object_t;

/// the object xml reader type
typedef struct __tb_object_xml_reader_t
{
	// the xml reader
	tb_handle_t 			reader;

}tb_object_xml_reader_t;

/// the object xml writer type
typedef struct __tb_object_xml_writer_t
{
	// the stream
	tb_gstream_t* 			stream;

	// is deflate?
	tb_bool_t 				deflate;

}tb_object_xml_writer_t;

/// the object bin reader type
typedef struct __tb_object_bin_reader_t
{
	// the stream
	tb_gstream_t* 			stream;

	// the object list
	tb_vector_t* 			list;

}tb_object_bin_reader_t;

/// the object bin writer type
typedef struct __tb_object_bin_writer_t
{
	// the stream
	tb_gstream_t* 			stream;

	// the object hash
	tb_hash_t* 				hash;

	// the object index
	tb_size_t 				index;

	// the encoder data
	tb_byte_t* 				data;

	// the encoder maxn
	tb_size_t 				maxn;

}tb_object_bin_writer_t;

/// the object jsn reader type
typedef struct __tb_object_jsn_reader_t
{
	// the stream
	tb_gstream_t* 			stream;

}tb_object_jsn_reader_t;

/// the object jsn writer type
typedef struct __tb_object_jsn_writer_t
{
	// the stream
	tb_gstream_t* 			stream;

	// is deflate?
	tb_bool_t 				deflate;

}tb_object_jsn_writer_t;

/// the xml reader func type
typedef tb_object_t* 		(*tb_object_xml_reader_func_t)(tb_object_xml_reader_t* reader, tb_size_t event);

/// the xml writer func type
typedef tb_bool_t 			(*tb_object_xml_writer_func_t)(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level);

/// the bin reader func type
typedef tb_object_t* 		(*tb_object_bin_reader_func_t)(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size);

/// the bin writer func type
typedef tb_bool_t 			(*tb_object_bin_writer_func_t)(tb_object_bin_writer_t* writer, tb_object_t* object);

/// the jsn reader func type
typedef tb_object_t* 		(*tb_object_jsn_reader_func_t)(tb_object_jsn_reader_t* reader, tb_char_t type);

/// the jsn writer func type
typedef tb_bool_t 			(*tb_object_jsn_writer_func_t)(tb_object_jsn_writer_t* writer, tb_object_t* object, tb_size_t level);

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_object_writ_bin_type_size(tb_gstream_t* gst, tb_size_t type, tb_uint64_t size)
{
	// check
	tb_assert_and_check_return_val(type <= 0xff, tb_false);

	// byte for size < 64bits
	tb_size_t sizeb = tb_object_need_bytes(size);
	tb_assert_and_check_return_val(sizeb <= 8, tb_false);

	// flag for size
	tb_size_t sizef = 0;
	switch (sizeb)
	{
	case 1: sizef = 0xc; break;
	case 2: sizef = 0xd; break;
	case 4: sizef = 0xe; break;
	case 8: sizef = 0xf; break;
	default: break;
	}
	tb_assert_and_check_return_val(sizef, tb_false);

	// writ flag 
	tb_uint8_t flag = ((type < 0xf? (tb_uint8_t)type : 0xf) << 4) | (size < 0xc? (tb_uint8_t)size : sizef);
	if (!tb_gstream_bwrit_u8(gst, flag)) return tb_false;

	// trace
//	tb_trace("writ: type: %lu, size: %llu", type, size);

	// writ type
	if (type >= 0xf) if (!tb_gstream_bwrit_u8(gst, (tb_uint8_t)type)) return tb_false;

	// writ size
	if (size >= 0xc)
	{
		switch (sizeb)
		{
		case 1:
			if (!tb_gstream_bwrit_u8(gst, (tb_uint8_t)size)) return tb_false;
			break;
		case 2:
			if (!tb_gstream_bwrit_u16_be(gst, (tb_uint16_t)size)) return tb_false;
			break;
		case 4:
			if (!tb_gstream_bwrit_u32_be(gst, (tb_uint32_t)size)) return tb_false;
			break;
		case 8:
			if (!tb_gstream_bwrit_u64_be(gst, (tb_uint64_t)size)) return tb_false;
			break;
		default:
			tb_assert_and_check_return_val(0, tb_false);
			break;
		}
	}

	// ok
	return tb_true;
}
static __tb_inline__ tb_void_t tb_object_read_bin_type_size(tb_gstream_t* gst, tb_size_t* ptype, tb_uint64_t* psize)
{
	// the flag
	tb_uint8_t flag = tb_gstream_bread_u8(gst);

	// the type & size
	tb_size_t 	type = flag >> 4;
	tb_uint64_t size = flag & 0x0f;
	if (type == 0xf) type = tb_gstream_bread_u8(gst);
	switch (size)
	{
	case 0xc:
		size = tb_gstream_bread_u8(gst);
		break;
	case 0xd:
		size = tb_gstream_bread_u16_be(gst);
		break;
	case 0xe:
		size = tb_gstream_bread_u32_be(gst);
		break;
	case 0xf:
		size = tb_gstream_bread_u64_be(gst);
		break;
	default:
		break;
	}

	// trace
//	tb_trace("type: %lu, size: %llu", type, size);

	// save
	if (ptype) *ptype = type;
	if (psize) *psize = size;
}

#endif
