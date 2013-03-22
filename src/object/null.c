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
 * @file		null.c
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
 * implementation
 */
static tb_object_t* tb_null_copy(tb_object_t* object)
{
	return object;
}
static tb_object_t* tb_null_read_xml(tb_object_xml_reader_t* reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && reader->reader && event, tb_null);

	// ok
	return tb_null_init();
}
static tb_bool_t tb_null_writ_xml(tb_object_xml_writer_t* writer, tb_object_t* object, tb_size_t level)
{
	// check
	tb_assert_and_check_return_val(writer && writer->stream, tb_false);

	// writ
	tb_object_writ_tab(writer->stream, writer->deflate, level);
	tb_gstream_printf(writer->stream, "<null/>");
	tb_object_writ_newline(writer->stream, writer->deflate);

	// ok
	return tb_true;
}
static tb_object_t* tb_null_read_bin(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
	// check
	tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

	// ok
	return tb_null_init();
}
static tb_bool_t tb_null_writ_bin(tb_object_bin_writer_t* writer, tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

	// writ type & null
	return tb_object_writ_bin_type_size(writer->stream, object->type, 0);
}
/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// null
static tb_object_t const g_null = 
{
	TB_OBJECT_FLAG_READONLY | TB_OBJECT_FLAG_SINGLETON
,	TB_OBJECT_TYPE_NULL
, 	1
, 	tb_null
, 	tb_null_copy
, 	tb_null
, 	tb_null

};

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_null_init_reader()
{
	if (!tb_object_set_xml_reader("null", tb_null_read_xml)) return tb_false;
	if (!tb_object_set_bin_reader(TB_OBJECT_TYPE_NULL, tb_null_read_bin)) return tb_false;
	return tb_true;
}
tb_bool_t tb_null_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_NULL, tb_null_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_NULL, tb_null_writ_bin)) return tb_false;
	return tb_true;
}
tb_object_t const* tb_null_init()
{
	return &g_null;
}

