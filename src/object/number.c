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
 * @file		number.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 		"object"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the number type
typedef struct __tb_number_t
{
	// the object base
	tb_object_t 		base;

	// the number type
	tb_size_t 			type;

	// the number value
	union
	{
		// the uint8
		tb_uint8_t 		u8;
	
		// the sint8
		tb_sint8_t 		s8;
	
		// the uint16
		tb_uint16_t 	u16;
	
		// the sint16
		tb_sint16_t 	s16;
	
		// the uint32
		tb_uint32_t 	u32;
	
		// the sint32
		tb_sint32_t 	s32;
	
		// the uint64
		tb_uint64_t 	u64;
	
		// the sint64
		tb_sint64_t 	s64;
	
		// the float
		tb_float_t 		f;
	
		// the double
		tb_double_t 	d;
	
	}v;

}tb_number_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_number_t* tb_number_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_NUMBER, tb_null);

	// cast
	return (tb_number_t*)object;
}
static tb_object_t* tb_number_copy(tb_object_t* object)
{
	// check
	tb_number_t* number = (tb_number_t*)object;
	tb_assert_and_check_return_val(number, tb_null);

	// copy
	switch (number->type)
	{
	case TB_NUMBER_TYPE_UINT64:
		return tb_number_init_from_uint64(number->v.u64);
	case TB_NUMBER_TYPE_SINT64:
		return tb_number_init_from_sint64(number->v.s64);
	case TB_NUMBER_TYPE_UINT32:
		return tb_number_init_from_uint32(number->v.u32);
	case TB_NUMBER_TYPE_SINT32:
		return tb_number_init_from_sint32(number->v.s32);
	case TB_NUMBER_TYPE_UINT16:
		return tb_number_init_from_uint16(number->v.u16);
	case TB_NUMBER_TYPE_SINT16:
		return tb_number_init_from_sint16(number->v.s16);
	case TB_NUMBER_TYPE_UINT8:
		return tb_number_init_from_uint8(number->v.u8);
	case TB_NUMBER_TYPE_SINT8:
		return tb_number_init_from_sint8(number->v.s8);
	case TB_NUMBER_TYPE_FLOAT:
		return tb_number_init_from_float(number->v.f);
	case TB_NUMBER_TYPE_DOUBLE:
		return tb_number_init_from_double(number->v.d);
	default:
		break;
	}

	return tb_null;
}
static tb_void_t tb_number_exit(tb_object_t* object)
{
	if (object) tb_opool_del(object);
}
static tb_void_t tb_number_cler(tb_object_t* object)
{
	// check
	tb_number_t* number = (tb_number_t*)object;
	tb_assert_and_check_return(number);

	// cler
	switch (number->type)
	{
	case TB_NUMBER_TYPE_UINT64:
		number->v.u64 = 0;
		break;
	case TB_NUMBER_TYPE_SINT64:
		number->v.s64 = 0;
		break;
	case TB_NUMBER_TYPE_UINT32:
		number->v.u32 = 0;
		break;
	case TB_NUMBER_TYPE_SINT32:
		number->v.s32 = 0;
		break;
	case TB_NUMBER_TYPE_UINT16:
		number->v.u16 = 0;
		break;
	case TB_NUMBER_TYPE_SINT16:
		number->v.s16 = 0;
		break;
	case TB_NUMBER_TYPE_UINT8:
		number->v.u8 = 0;
		break;
	case TB_NUMBER_TYPE_SINT8:
		number->v.s8 = 0;
		break;
	case TB_NUMBER_TYPE_FLOAT:
		number->v.f = 0.;
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		number->v.d = 0.;
		break;
	default:
		break;
	}
}
static tb_number_t* tb_number_init_base()
{
	// make
	tb_number_t* number = tb_opool_get(sizeof(tb_number_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_NUMBER);
	tb_assert_and_check_return_val(number, tb_null);

	// init base
	number->base.copy = tb_number_copy;
	number->base.cler = tb_number_cler;
	number->base.exit = tb_number_exit;

	// ok
	return number;
}
static tb_object_t* tb_number_read_xml(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_number_init_from_uint32(0);

	// walk
	tb_object_t* number = tb_null;
	while (event = tb_xml_reader_next(reader->reader))
	{
		switch (event)
		{
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader->reader);
				tb_assert_and_check_goto(name, end);
				
				// is end?
				if (!tb_stricmp(name, "number")) goto end;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_impl("number: %s", text);

				// has sign? is float?
				tb_size_t s = 0;
				tb_size_t f = 0;
				tb_char_t const* p = text;
				for (; *p; p++)
				{
					if (!s && *p == '-') s = 1;
					if (!f && *p == '.') f = 1;
					if (s && f) break;
				}
				
				// number
				if (f) number = tb_number_init_from_double(tb_atof(text));
				else number = s? tb_number_init_from_sint64(tb_stoi64(text)) : tb_number_init_from_uint64(tb_stou64(text));
				tb_assert_and_check_goto(number, end);
			}
			break;
		default:
			break;
		}
	}

end:

	// ok?
	return number;
}
static tb_bool_t tb_number_writ_xml(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	switch (tb_number_type(object))
	{
	case TB_NUMBER_TYPE_UINT64:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%llu</number>", tb_number_uint64(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT64:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%lld</number>", tb_number_sint64(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT32:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%u</number>", tb_number_uint32(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT32:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%d</number>", tb_number_sint32(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT16:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%u</number>", tb_number_uint16(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT16:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%d</number>", tb_number_sint16(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT8:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%u</number>", tb_number_uint8(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT8:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%d</number>", tb_number_sint8(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_FLOAT:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%f</number>", tb_number_float(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<number>%lf</number>", tb_number_double(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	default:
		break;
	}

	// ok
	return tb_true;
}
static tb_object_t* tb_number_read_bin(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

	// the number type
	tb_size_t number_type = (tb_size_t)size;

	// read number
	tb_object_t* number = tb_null;
	switch (number_type)
	{
	case TB_NUMBER_TYPE_UINT64:
		number = tb_number_init_from_uint64(tb_gstream_bread_u64_be(reader->stream));
		break;
	case TB_NUMBER_TYPE_SINT64:
		number = tb_number_init_from_sint64(tb_gstream_bread_s64_be(reader->stream));
		break;
	case TB_NUMBER_TYPE_UINT32:
		number = tb_number_init_from_uint32(tb_gstream_bread_u32_be(reader->stream));
		break;
	case TB_NUMBER_TYPE_SINT32:
		number = tb_number_init_from_sint32(tb_gstream_bread_s32_be(reader->stream));
		break;
	case TB_NUMBER_TYPE_UINT16:
		number = tb_number_init_from_uint16(tb_gstream_bread_u16_be(reader->stream));
		break;
	case TB_NUMBER_TYPE_SINT16:
		number = tb_number_init_from_sint16(tb_gstream_bread_s16_be(reader->stream));
		break;
	case TB_NUMBER_TYPE_UINT8:
		number = tb_number_init_from_uint8(tb_gstream_bread_u8(reader->stream));
		break;
	case TB_NUMBER_TYPE_SINT8:
		number = tb_number_init_from_sint8(tb_gstream_bread_s8(reader->stream));
		break;
	case TB_NUMBER_TYPE_FLOAT:
		{
			tb_byte_t data[4] = {0};
			if (!tb_gstream_bread(reader->stream, data, 4)) return tb_null;
			number = tb_number_init_from_float(tb_bits_get_float_be(data));
		}
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		{
			tb_byte_t data[8] = {0};
			if (!tb_gstream_bread(reader->stream, data, 8)) return tb_null;
			number = tb_number_init_from_double(tb_bits_get_double_bbe(data));
		}
		break;
	default:
		tb_assert_and_check_return_val(0, tb_null);
		break;
	}

	// ok?
	return number;
}
static tb_bool_t tb_number_writ_bin(tb_object_bin_writer_t* writer, tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

	// the number
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// writ type
	if (!tb_object_writ_bin_type_size(writer->stream, object->type, (tb_uint64_t)tb_number_type(object))) return tb_false;

	// writ number
	switch (tb_number_type(object))
	{
	case TB_NUMBER_TYPE_UINT64:
		if (!tb_gstream_bwrit_u64_be(writer->stream, tb_number_uint64(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT64:
		if (!tb_gstream_bwrit_s64_be(writer->stream, tb_number_sint64(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT32:
		if (!tb_gstream_bwrit_u32_be(writer->stream, tb_number_uint32(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT32:
		if (!tb_gstream_bwrit_s32_be(writer->stream, tb_number_sint32(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT16:
		if (!tb_gstream_bwrit_u16_be(writer->stream, tb_number_uint16(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT16:
		if (!tb_gstream_bwrit_s16_be(writer->stream, tb_number_sint16(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_UINT8:
		if (!tb_gstream_bwrit_u8(writer->stream, tb_number_uint8(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_SINT8:
		if (!tb_gstream_bwrit_s8(writer->stream, tb_number_sint8(object))) return tb_false;
		break;
	case TB_NUMBER_TYPE_FLOAT:
		{
			tb_byte_t data[4];
			tb_bits_set_float_be(data, tb_number_float(object));
			if (!tb_gstream_bwrit(writer->stream, data, 4)) return tb_false;
		}
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		{
			tb_byte_t data[8];
			tb_bits_set_double_bbe(data, tb_number_double(object));
			if (!tb_gstream_bwrit(writer->stream, data, 8)) return tb_false;
		}
		break;
	default:
		tb_assert_and_check_return_val(0, tb_false);
		break;
	}

	// ok
	return tb_true;
}
static tb_object_t* tb_number_read_jsn(tb_object_jsn_reader_t* reader, tb_char_t type)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream, tb_null);

	// init data
	tb_sstring_t 	data;
	tb_char_t 		buff[256];
	if (!tb_sstring_init(&data, buff, 256)) return tb_null;

	// init
	tb_object_t* number = tb_null;

	// append character
	tb_sstring_chrcat(&data, type);

	// walk
	tb_bool_t bs = (type == '-')? tb_true : tb_false;
	tb_bool_t bf = (type == '.')? tb_true : tb_false;
	while (tb_gstream_left(reader->stream)) 
	{
		// need one character
		tb_byte_t* p = tb_null;
		if (!tb_gstream_bneed(reader->stream, &p, 1) && p) goto end;

		// the character
		tb_char_t ch = *p;

		// is float?
		if (!bf && ch == '.') bf = tb_true;
		else if (bf && ch == '.') goto end;

		// append character
		if (tb_isdigit10(ch) || ch == '.' || ch == 'e' || ch == 'E' || ch == '-' || ch == '+') 
			tb_sstring_chrcat(&data, ch);
		else break;

		// skip it
		tb_gstream_bskip(reader->stream, 1);
	}

	// check
	tb_assert_and_check_goto(tb_sstring_size(&data), end);

	// trace
	tb_trace_impl("number: %s", tb_sstring_cstr(&data));

	// init number 
	if (bf) number = tb_number_init_from_float(tb_stof(tb_sstring_cstr(&data)));
	else if (bs) 
	{
		tb_sint64_t value = tb_stoi64(tb_sstring_cstr(&data));
		tb_size_t 	bytes = tb_object_need_bytes(-value);
		switch (bytes)
		{
		case 1: number = tb_number_init_from_sint8((tb_sint8_t)value); break;
		case 2: number = tb_number_init_from_sint16((tb_sint16_t)value); break;
		case 4: number = tb_number_init_from_sint32((tb_sint32_t)value); break;
		case 8: number = tb_number_init_from_sint64((tb_sint64_t)value); break;
		default: break;
		}
		
	}
	else 
	{
		tb_uint64_t value = tb_stou64(tb_sstring_cstr(&data));
		tb_size_t 	bytes = tb_object_need_bytes(value);
		switch (bytes)
		{
		case 1: number = tb_number_init_from_uint8((tb_uint8_t)value); break;
		case 2: number = tb_number_init_from_uint16((tb_uint16_t)value); break;
		case 4: number = tb_number_init_from_uint32((tb_uint32_t)value); break;
		case 8: number = tb_number_init_from_uint64((tb_uint64_t)value); break;
		default: break;
		}
	}

end:

	// exit data
	tb_sstring_exit(&data);

	// ok?
	return number;
}
static tb_bool_t tb_number_writ_jsn(tb_object_jsn_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	switch (tb_number_type(object))
	{
	case TB_NUMBER_TYPE_UINT64:
		tb_gstream_printf(writer->stream, "%llu", tb_number_uint64(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT64:
		tb_gstream_printf(writer->stream, "%lld", tb_number_sint64(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT32:
		tb_gstream_printf(writer->stream, "%u", tb_number_uint32(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT32:
		tb_gstream_printf(writer->stream, "%d", tb_number_sint32(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT16:
		tb_gstream_printf(writer->stream, "%u", tb_number_uint16(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT16:
		tb_gstream_printf(writer->stream, "%d", tb_number_sint16(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_UINT8:
		tb_gstream_printf(writer->stream, "%u", tb_number_uint8(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_SINT8:
		tb_gstream_printf(writer->stream, "%d", tb_number_sint8(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_FLOAT:
		tb_gstream_printf(writer->stream, "%f", tb_number_float(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	case TB_NUMBER_TYPE_DOUBLE:
		tb_gstream_printf(writer->stream, "%lf", tb_number_double(object));
		tb_object_writ_newline(writer->stream, writer->deflate);
		break;
	default:
		break;
	}

	// ok
	return tb_true;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_number_init_reader()
{
	if (!tb_object_set_xml_reader("number", tb_number_read_xml)) return tb_false;
	if (!tb_object_set_bin_reader(TB_OBJECT_TYPE_NUMBER, tb_number_read_bin)) return tb_false;
	if (!tb_object_set_jsn_reader('0', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('1', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('2', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('3', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('4', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('5', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('6', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('7', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('8', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('9', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('.', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('-', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('+', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('e', tb_number_read_jsn)) return tb_false;
	if (!tb_object_set_jsn_reader('E', tb_number_read_jsn)) return tb_false;
	return tb_true;
}
tb_bool_t tb_number_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_NUMBER, tb_number_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_NUMBER, tb_number_writ_bin)) return tb_false;
	if (!tb_object_set_jsn_writer(TB_OBJECT_TYPE_NUMBER, tb_number_writ_jsn)) return tb_false;
	return tb_true;
}
tb_object_t* tb_number_init_from_uint8(tb_uint8_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT8;
	number->v.u8 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_sint8(tb_sint8_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT8;
	number->v.s8 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_uint16(tb_uint16_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT16;
	number->v.u16 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_sint16(tb_sint16_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT16;
	number->v.s16 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_uint32(tb_uint32_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT32;
	number->v.u32 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_sint32(tb_sint32_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT32;
	number->v.s32 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_uint64(tb_uint64_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT64;
	number->v.u64 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_sint64(tb_sint64_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT64;
	number->v.s64 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_float(tb_float_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_FLOAT;
	number->v.f = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_from_double(tb_double_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_DOUBLE;
	number->v.d = value;

	// ok
	return number;
}

tb_size_t tb_number_type(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, TB_NUMBER_TYPE_NONE);

	// type
	return number->type;
}

tb_uint8_t tb_number_uint8(tb_object_t* object)
{
	return (tb_uint8_t)tb_number_uint64(object);
}
tb_sint8_t tb_number_sint8(tb_object_t* object)
{
	return (tb_sint8_t)tb_number_sint64(object);
}
tb_uint16_t tb_number_uint16(tb_object_t* object)
{
	return (tb_uint16_t)tb_number_uint64(object);
}
tb_sint16_t tb_number_sint16(tb_object_t* object)
{
	return (tb_sint16_t)tb_number_sint64(object);
}
tb_uint32_t tb_number_uint32(tb_object_t* object)
{
	return (tb_uint32_t)tb_number_uint64(object);
}
tb_sint32_t tb_number_sint32(tb_object_t* object)
{
	return (tb_sint32_t)tb_number_sint64(object);
}
tb_uint64_t tb_number_uint64(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// uint64
	switch (number->type)
	{
	case TB_NUMBER_TYPE_UINT64:
		return number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return number->v.s64;
	case TB_NUMBER_TYPE_UINT32:
		return number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return number->v.s32;
	case TB_NUMBER_TYPE_UINT16:
		return number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return number->v.s16;
	case TB_NUMBER_TYPE_UINT8:
		return number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return number->v.s8;
	case TB_NUMBER_TYPE_FLOAT:
		return (tb_uint64_t)number->v.f;
	case TB_NUMBER_TYPE_DOUBLE:
		return (tb_uint64_t)number->v.d;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}

tb_sint64_t tb_number_sint64(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// sint64
	switch (number->type)
	{
	case TB_NUMBER_TYPE_UINT64:
		return number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return number->v.s64;
	case TB_NUMBER_TYPE_UINT32:
		return number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return number->v.s32;
	case TB_NUMBER_TYPE_UINT16:
		return number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return number->v.s16;
	case TB_NUMBER_TYPE_UINT8:
		return number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return number->v.s8;
	case TB_NUMBER_TYPE_FLOAT:
		return (tb_sint64_t)number->v.f;
	case TB_NUMBER_TYPE_DOUBLE:
		return (tb_sint64_t)number->v.d;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}
tb_float_t tb_number_float(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// float
	switch (number->type)
	{
	case TB_NUMBER_TYPE_FLOAT:
		return number->v.f;
	case TB_NUMBER_TYPE_DOUBLE:
		return (tb_float_t)number->v.d;
	case TB_NUMBER_TYPE_UINT8:
		return (tb_float_t)number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return (tb_float_t)number->v.s8;
	case TB_NUMBER_TYPE_UINT16:
		return (tb_float_t)number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return (tb_float_t)number->v.s16;
	case TB_NUMBER_TYPE_UINT32:
		return (tb_float_t)number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return (tb_float_t)number->v.s32;
	case TB_NUMBER_TYPE_UINT64:
		return (tb_float_t)number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return (tb_float_t)number->v.s64;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}
tb_double_t tb_number_double(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// double
	switch (number->type)
	{
	case TB_NUMBER_TYPE_DOUBLE:
		return number->v.d;
	case TB_NUMBER_TYPE_FLOAT:
		return (tb_double_t)number->v.f;
	case TB_NUMBER_TYPE_UINT8:
		return (tb_double_t)number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return (tb_double_t)number->v.s8;
	case TB_NUMBER_TYPE_UINT16:
		return (tb_double_t)number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return (tb_double_t)number->v.s16;
	case TB_NUMBER_TYPE_UINT32:
		return (tb_double_t)number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return (tb_double_t)number->v.s32;
	case TB_NUMBER_TYPE_UINT64:
		return (tb_double_t)number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return (tb_double_t)number->v.s64;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}
tb_bool_t tb_number_uint8_set(tb_object_t* object, tb_uint8_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_UINT8;
	number->v.u8 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_sint8_set(tb_object_t* object, tb_sint8_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_SINT8;
	number->v.s8 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_uint16_set(tb_object_t* object, tb_uint16_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_UINT16;
	number->v.u16 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_sint16_set(tb_object_t* object, tb_sint16_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_SINT16;
	number->v.s16 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_uint32_set(tb_object_t* object, tb_uint32_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_UINT32;
	number->v.u32 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_sint32_set(tb_object_t* object, tb_sint32_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_SINT32;
	number->v.s32 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_uint64_set(tb_object_t* object, tb_uint64_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_UINT64;
	number->v.u64 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_sint64_set(tb_object_t* object, tb_sint64_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_SINT64;
	number->v.s64 = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_float_set(tb_object_t* object, tb_float_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_FLOAT;
	number->v.f = value;

	// ok
	return tb_true;
}
tb_bool_t tb_number_double_set(tb_object_t* object, tb_double_t value)
{	
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, tb_false);

	// init value
	number->type = TB_NUMBER_TYPE_DOUBLE;
	number->v.d = value;

	// ok
	return tb_true;
}
