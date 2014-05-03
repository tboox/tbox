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
 * @file		value.c
 * @ingroup 	database
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "value.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_database_sql_value_size(tb_database_sql_value_t const* value)
{
	// check
	tb_assert_and_check_return_val(value, 0);

	// done
	switch (value->type)
	{
	case TB_DATABASE_SQL_VALUE_TYPE_TEXT32:
	case TB_DATABASE_SQL_VALUE_TYPE_TEXT16:
	case TB_DATABASE_SQL_VALUE_TYPE_TEXT8:
		return value->text.hint? value->text.hint : (value->text.data? tb_strlen(value->text.data) : 0);
	case TB_DATABASE_SQL_VALUE_TYPE_BLOB32:
	case TB_DATABASE_SQL_VALUE_TYPE_BLOB16:
	case TB_DATABASE_SQL_VALUE_TYPE_BLOB8:
		return value->blob.size;
	case TB_DATABASE_SQL_VALUE_TYPE_INT32:
#ifdef TB_CONFIG_TYPE_FLOAT
	case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
#endif
		return 4;
	case TB_DATABASE_SQL_VALUE_TYPE_INT64:
#ifdef TB_CONFIG_TYPE_FLOAT
	case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
#endif
		return 8;
	case TB_DATABASE_SQL_VALUE_TYPE_INT16:
		return 2;
	case TB_DATABASE_SQL_VALUE_TYPE_INT8:
		return 1;
	default:
		tb_trace_e("unknown value type: %llu", value->type);
		break;
	}

	return 0;
}
tb_int8_t tb_database_sql_value_int8(tb_database_sql_value_t const* value)
{
	return 0;
}
tb_int16_t tb_database_sql_value_int16(tb_database_sql_value_t const* value)
{
	return 0;
}
tb_int32_t tb_database_sql_value_int32(tb_database_sql_value_t const* value)
{
	return 0;
}
tb_int64_t tb_database_sql_value_int64(tb_database_sql_value_t const* value)
{
	return 0;
}
tb_float_t tb_database_sql_value_float(tb_database_sql_value_t const* value)
{
	return 0;
}
tb_double_t tb_database_sql_value_double(tb_database_sql_value_t const* value)
{
	return 0;
}
tb_void_t tb_database_sql_value_int8_set(tb_database_sql_value_t* value, tb_int8_t number)
{
}
tb_void_t tb_database_sql_value_int16_set(tb_database_sql_value_t* value, tb_int16_t number)
{
}
tb_void_t tb_database_sql_value_int32_set(tb_database_sql_value_t* value, tb_int32_t number)
{
}
tb_void_t tb_database_sql_value_int64_set(tb_database_sql_value_t* value, tb_int64_t number)
{
}
tb_void_t tb_database_sql_value_float_set(tb_database_sql_value_t* value, tb_float_t number)
{
}
tb_void_t tb_database_sql_value_double_set(tb_database_sql_value_t* value, tb_double_t number)
{
}
tb_void_t tb_database_sql_value_text8_set(tb_database_sql_value_t* value, tb_char_t const* text, tb_size_t hint)
{
	// check
	tb_assert_and_check_return(value && text);

	// init text
	value->type 		= TB_DATABASE_SQL_VALUE_TYPE_TEXT8;
	value->text.data 	= text;
	value->text.hint 	= hint;

	// check size
	tb_assert(tb_database_sql_value_size(value) <= TB_MAXU8);
}
tb_void_t tb_database_sql_value_text16_set(tb_database_sql_value_t* value, tb_char_t const* text, tb_size_t hint)
{
	// check
	tb_assert_and_check_return(value && text);

	// init text
	value->type 		= TB_DATABASE_SQL_VALUE_TYPE_TEXT16;
	value->text.data 	= text;
	value->text.hint 	= hint;

	// check size
	tb_assert(tb_database_sql_value_size(value) <= TB_MAXU16);
}
tb_void_t tb_database_sql_value_text32_set(tb_database_sql_value_t* value, tb_char_t const* text, tb_size_t hint)
{
	// check
	tb_assert_and_check_return(value && text);

	// init text
	value->type 		= TB_DATABASE_SQL_VALUE_TYPE_TEXT32;
	value->text.data 	= text;
	value->text.hint 	= hint;
}
tb_void_t tb_database_sql_value_blob8_set(tb_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(value && data);

	// init blob
	value->type 		= TB_DATABASE_SQL_VALUE_TYPE_BLOB8;
	value->blob.data 	= data;
	value->blob.size 	= size;

	// check size
	tb_assert(tb_database_sql_value_size(value) <= TB_MAXU8);
}
tb_void_t tb_database_sql_value_blob16_set(tb_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(value && data);

	// init blob
	value->type 		= TB_DATABASE_SQL_VALUE_TYPE_BLOB16;
	value->blob.data 	= data;
	value->blob.size 	= size;

	// check size
	tb_assert(tb_database_sql_value_size(value) <= TB_MAXU16);
}
tb_void_t tb_database_sql_value_blob32_set(tb_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(value && data);

	// init blob
	value->type 		= TB_DATABASE_SQL_VALUE_TYPE_BLOB32;
	value->blob.data 	= data;
	value->blob.size 	= size;

	// check size
	tb_assert(tb_database_sql_value_size(value) <= TB_MAXU32);
}
