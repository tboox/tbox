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
 * @file		boolean.c
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

// the boolean type
typedef struct __tb_boolean_t
{
	// the object base
	tb_object_t 		base;

	// the boolean value
	tb_bool_t 			value;

}tb_boolean_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_boolean_t const* tb_boolean_cast(tb_object_t const* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_BOOLEAN, tb_null);

	// cast
	return (tb_boolean_t const*)object;
}

static tb_object_t* tb_boolean_copy(tb_object_t* object)
{
	// check
	tb_boolean_t* boolean = (tb_boolean_t*)object;
	tb_assert_and_check_return_val(boolean, tb_null);

	// copy
	return (tb_object_t*)tb_boolean_init(boolean->value);
}
static tb_object_t* tb_boolean_read_xml(tb_handle_t reader, tb_size_t event)
{
	// check
	tb_assert_and_check_return_val(reader && event, tb_null);

	// name
	tb_char_t const* name = tb_xml_reader_element(reader);
	tb_assert_and_check_return_val(name, tb_null);
	tb_trace_impl("boolean: %s", name);

	// the boolean value
	tb_bool_t val = tb_false;
	if (!tb_stricmp(name, "true")) val = tb_true;
	else if (!tb_stricmp(name, "false")) val = tb_false;
	else return tb_null;

	// ok?
	return tb_boolean_init(val);
}
static tb_bool_t tb_boolean_writ_xml(tb_object_t* object, tb_gstream_t* gst, tb_bool_t deflate, tb_size_t level)
{
	// writ
	tb_object_writ_tab(gst, deflate, level);
	tb_gstream_printf(gst, "<%s/>", tb_boolean_bool(object)? "true" : "false");
	tb_object_writ_newline(gst, deflate);

	// ok
	return tb_true;
}
static tb_object_t* tb_boolean_read_bin(tb_gstream_t* gst, tb_size_t type, tb_size_t size)
{
	tb_trace_noimpl();
	return tb_null;
}
static tb_bool_t tb_boolean_writ_bin(tb_object_t* object, tb_gstream_t* gst)
{
	// writ type & size
	if (!tb_object_writ_bin_type_size(gst, object->type, 0xf)) return tb_false;

	// ok
	return tb_true;
}
/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// true
static tb_boolean_t const g_boolean_true = 
{
	TB_OBJECT_FLAG_READONLY | TB_OBJECT_FLAG_SINGLETON
,	TB_OBJECT_TYPE_BOOLEAN
, 	1
, 	tb_null
, 	tb_boolean_copy
, 	tb_null
, 	tb_null
, 	tb_true

};

// false
static tb_boolean_t const g_boolean_false = 
{
	TB_OBJECT_FLAG_READONLY | TB_OBJECT_FLAG_SINGLETON
,	TB_OBJECT_TYPE_BOOLEAN
, 	1
, 	tb_null
, 	tb_boolean_copy
, 	tb_null
, 	tb_null
, 	tb_false

};

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_boolean_init_reader()
{
	if (!tb_object_set_xml_reader("true", tb_boolean_read_xml)) return tb_false;
	if (!tb_object_set_xml_reader("false", tb_boolean_read_xml)) return tb_false;
	if (!tb_object_set_bin_reader(TB_OBJECT_TYPE_BOOLEAN, tb_boolean_read_bin)) return tb_false;
	return tb_true;
}
tb_bool_t tb_boolean_init_writer()
{
	if (!tb_object_set_xml_writer(TB_OBJECT_TYPE_BOOLEAN, tb_boolean_writ_xml)) return tb_false;
	if (!tb_object_set_bin_writer(TB_OBJECT_TYPE_BOOLEAN, tb_boolean_writ_bin)) return tb_false;
	return tb_true;
}
tb_object_t const* tb_boolean_init(tb_bool_t value)
{
	return value? tb_boolean_true() : tb_boolean_false();
}
tb_object_t const* tb_boolean_true()
{
	return (tb_object_t const*)&g_boolean_true;
}
tb_object_t const* tb_boolean_false()
{
	return (tb_object_t const*)&g_boolean_false;
}
tb_bool_t tb_boolean_bool(tb_object_t const* object)
{
	tb_boolean_t const* boolean = tb_boolean_cast(object);
	tb_assert_and_check_return_val(boolean, tb_false);

	return boolean->value;
}

