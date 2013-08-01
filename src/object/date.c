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
 * @file		date.c
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
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the date type
typedef struct __tb_date_t
{
	// the object base
	tb_object_t 		base;

	// the date time
	tb_time_t 			time;

}tb_date_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_date_t* tb_date_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_DATE, tb_null);

	// cast
	return (tb_date_t*)object;
}
static tb_object_t* tb_date_copy(tb_object_t* object)
{
	return tb_date_init_from_time(tb_date_time(object));
}
static tb_void_t tb_date_exit(tb_object_t* object)
{
	tb_date_t* date = tb_date_cast(object);
	if (date) tb_opool_del(date);
}
static tb_void_t tb_date_cler(tb_object_t* object)
{
	tb_date_t* date = tb_date_cast(object);
	if (date) date->time = 0;
}
static tb_date_t* tb_date_init_base()
{
	// make
	tb_date_t* date = tb_opool_get(sizeof(tb_date_t), TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DATE);
	tb_assert_and_check_return_val(date, tb_null);

	// init base
	date->base.copy = tb_date_copy;
	date->base.cler = tb_date_cler;
	date->base.exit = tb_date_exit;

	// ok
	return date;
}
static tb_object_t* tb_date_read_xml(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// empty?
	if (event == TB_XML_READER_EVENT_ELEMENT_EMPTY) 
		return tb_date_init_from_time(0);

	// walk
	tb_object_t* 	date 	= tb_null;
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
				if (!tb_stricmp(name, "date"))
				{
					// empty?
					if (!date) date = tb_date_init_from_time(0);
					goto end;
				}
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// text
				tb_char_t const* text = tb_xml_reader_text(reader->reader);
				tb_assert_and_check_goto(text, end);
				tb_trace_impl("date: %s", text);

				// done date: %04ld-%02ld-%02ld %02ld:%02ld:%02ld
				tb_tm_t tm = {0};
				tb_char_t const* p = text;
				tb_char_t const* e = text + tb_strlen(text);

				// init year
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.year = tb_atoi(p);

				// init month
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.month = tb_atoi(p);
				
				// init day
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.mday = tb_atoi(p);
				
				// init hour
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.hour = tb_atoi(p);
						
				// init minute
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.minute = tb_atoi(p);
				
				// init second
				while (p < e && *p && tb_isdigit(*p)) p++;
				while (p < e && *p && !tb_isdigit(*p)) p++;
				tb_assert_and_check_goto(p < e, end);
				tm.second = tb_atoi(p);
			
				// time
				tb_time_t time = tb_mktime(&tm);
				tb_assert_and_check_goto(time >= 0, end);

				// date
				date = tb_date_init_from_time(time);
			}
			break;
		default:
			break;
		}
	}

end:

	// ok?
	return date;
}
static tb_bool_t tb_date_writ_xml(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// no empty?
	tb_time_t time = tb_date_time(object);
	if (time > 0)
	{
		// writ beg
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<date>");

		// writ date
		tb_tm_t date = {0};
		if (tb_localtime(time, &date))
		{
			tb_gstream_printf(writer->stream, 	"%04ld-%02ld-%02ld %02ld:%02ld:%02ld"
								, 	date.year
								, 	date.month
								, 	date.mday
								, 	date.hour
								, 	date.minute
								, 	date.second);
		}
					
		// writ end
		tb_gstream_printf(writer->stream, "</date>");
		tb_object_writ_newline(writer->stream, writer->deflate);
	}
	else 
	{
		// writ
		tb_object_writ_tab(writer->stream, writer->deflate, level);
		tb_gstream_printf(writer->stream, "<date/>");
		tb_object_writ_newline(writer->stream, writer->deflate);
	}

	// ok
	return tb_true;
}
static tb_object_t* tb_date_read_bin(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

	// ok
	return tb_date_init_from_time((tb_time_t)size);
}
static tb_bool_t tb_date_writ_bin(tb_object_bin_writer_t* writer, tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

	// writ type & time
	return tb_object_writ_bin_type_size(writer->stream, object->type, (tb_uint64_t)tb_date_time(object));
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_date_init_reader()
{
	if (!tb_object_set_xml_reader("date", tb_date_read_xml)) return tb_false;
	if (!tb_object_set_bin_reader(TB_OBJECT_TYPE_DATE, tb_date_read_bin)) return tb_false;
	return tb_true;
}
tb_bool_t tb_date_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_DATE, tb_date_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_DATE, tb_date_writ_bin)) return tb_false;
	return tb_true;
}
tb_object_t* tb_date_init_from_now()
{
	// make
	tb_date_t* date = tb_date_init_base();
	tb_assert_and_check_return_val(date, tb_null);

	// init time
	date->time = tb_time();

	// ok
	return date;
}
tb_object_t* tb_date_init_from_time(tb_time_t time)
{
	// make
	tb_date_t* date = tb_date_init_base();
	tb_assert_and_check_return_val(date, tb_null);

	// init time
	if (time > 0) date->time = time;

	// ok
	return date;
}
tb_time_t tb_date_time(tb_object_t* object)
{
	// check
	tb_date_t* date = tb_date_cast(object);
	tb_assert_and_check_return_val(date, -1);

	// time
	return date->time;
}
tb_bool_t tb_date_time_set(tb_object_t* object, tb_time_t time)
{
	// check
	tb_date_t* date = tb_date_cast(object);
	tb_assert_and_check_return_val(date, tb_false);

	// set time
	date->time = time;

	// ok
	return tb_true;
}
tb_bool_t tb_date_time_set_now(tb_object_t* object)
{
	// check
	tb_date_t* date = tb_date_cast(object);
	tb_assert_and_check_return_val(date, tb_false);

	// set time
	date->time = tb_time();

	// ok
	return tb_true;
}
