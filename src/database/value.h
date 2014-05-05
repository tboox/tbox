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
 * @file		value.h
 * @ingroup 	database
 *
 */
#ifndef TB_DATABASE_SQL_VALUE_H
#define TB_DATABASE_SQL_VALUE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the sql database value type enum
typedef enum __tb_database_sql_value_type_e
{
	TB_DATABASE_SQL_VALUE_TYPE_NULL 		= 0
,	TB_DATABASE_SQL_VALUE_TYPE_INT8 		= 1
,	TB_DATABASE_SQL_VALUE_TYPE_INT16 		= 2
,	TB_DATABASE_SQL_VALUE_TYPE_INT32 		= 3
,	TB_DATABASE_SQL_VALUE_TYPE_INT64 		= 4
,	TB_DATABASE_SQL_VALUE_TYPE_BLOB8 		= 5
,	TB_DATABASE_SQL_VALUE_TYPE_BLOB16 		= 6
,	TB_DATABASE_SQL_VALUE_TYPE_BLOB32 		= 7
,	TB_DATABASE_SQL_VALUE_TYPE_TEXT8 		= 8
,	TB_DATABASE_SQL_VALUE_TYPE_TEXT16 		= 9
,	TB_DATABASE_SQL_VALUE_TYPE_TEXT32 		= 10
#ifdef TB_CONFIG_TYPE_FLOAT
,	TB_DATABASE_SQL_VALUE_TYPE_FLOAT 		= 11
,	TB_DATABASE_SQL_VALUE_TYPE_DOUBLE 		= 12
#endif

}tb_database_sql_value_type_e;

/// the sql database value type
typedef struct __tb_database_sql_value_t
{
	/// the type
	tb_size_t 					type;

	/// the name
	tb_char_t const* 			name;

	/// the data
	union 
	{
		// int
		tb_int8_t 				i8;
		tb_int16_t 				i16;
		tb_int32_t 				i32;
		tb_int64_t 				i64;

		// float
#ifdef TB_CONFIG_TYPE_FLOAT
		tb_float_t 				f;
		tb_double_t 			d;
#endif

		// blob
		struct 
		{
			tb_byte_t const* 	data;
			tb_size_t 			size;

		} 						blob;

		// text
		struct 
		{
			tb_char_t const* 	data;
			tb_size_t 			hint;

		} 						text;
	};	

}tb_database_sql_value_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the value data size
 *
 * @param value 	the value
 *
 * @return 			the value data size
 */
tb_size_t 			tb_database_sql_value_size(tb_database_sql_value_t const* value);

/*! the int8 value
 *
 * @param value 	the value
 *
 * @return 			the int8 value
 */
tb_int8_t 			tb_database_sql_value_int8(tb_database_sql_value_t const* value);

/*! the int16 value
 *
 * @param value 	the value
 *
 * @return 			the int16 value
 */
tb_int16_t 			tb_database_sql_value_int16(tb_database_sql_value_t const* value);

/*! the int32 value
 *
 * @param value 	the value
 *
 * @return 			the int32 value
 */
tb_int32_t 			tb_database_sql_value_int32(tb_database_sql_value_t const* value);

/*! the int64 value
 *
 * @param value 	the value
 *
 * @return 			the int64 value
 */
tb_int64_t 			tb_database_sql_value_int64(tb_database_sql_value_t const* value);

#ifdef TB_CONFIG_TYPE_FLOAT
/*! the float value
 *
 * @param value 	the value
 *
 * @return 			the float value
 */
tb_float_t 			tb_database_sql_value_float(tb_database_sql_value_t const* value);

/*! the double value
 *
 * @param value 	the value
 *
 * @return 			the double value
 */
tb_double_t 		tb_database_sql_value_double(tb_database_sql_value_t const* value);
#endif

/*! set the int8 value
 *
 * @param value 	the value
 * @param number 	the number
 */
tb_void_t 			tb_database_sql_value_set_int8(tb_database_sql_value_t* value, tb_int8_t number);

/*! set the int16 value
 *
 * @param value 	the value
 * @param number 	the number
 */
tb_void_t 			tb_database_sql_value_set_int16(tb_database_sql_value_t* value, tb_int16_t number);

/*! set the int32 value
 *
 * @param value 	the value
 * @param number 	the number
 */
tb_void_t 			tb_database_sql_value_set_int32(tb_database_sql_value_t* value, tb_int32_t number);

/*! set the int64 value
 *
 * @param value 	the value
 * @param number 	the number
 */
tb_void_t 			tb_database_sql_value_set_int64(tb_database_sql_value_t* value, tb_int64_t number);

#ifdef TB_CONFIG_TYPE_FLOAT
/*! set the float value
 *
 * @param value 	the value
 * @param number 	the number
 */
tb_void_t 			tb_database_sql_value_set_float(tb_database_sql_value_t* value, tb_float_t number);

/*! set the double value
 *
 * @param value 	the value
 * @param number 	the number
 */
tb_void_t 			tb_database_sql_value_set_double(tb_database_sql_value_t* value, tb_double_t number);
#endif

/*! set the text8 value
 *
 * @param value 	the value
 * @param text 		the text data
 * @param hint 		the text size hint
 */
tb_void_t 			tb_database_sql_value_set_text8(tb_database_sql_value_t* value, tb_char_t const* text, tb_size_t hint);

/*! set the text16 value
 *
 * @param value 	the value
 * @param text 		the text data
 * @param hint 		the text size hint
 */
tb_void_t 			tb_database_sql_value_set_text16(tb_database_sql_value_t* value, tb_char_t const* text, tb_size_t hint);

/*! set the text32 value
 *
 * @param value 	the value
 * @param text 		the text data
 * @param hint 		the text size hint
 */
tb_void_t 			tb_database_sql_value_set_text32(tb_database_sql_value_t* value, tb_char_t const* text, tb_size_t hint);

/*! set the blob8 value
 *
 * @param value 	the value
 * @param data 		the blob data
 * @param size 		the blob size
 */
tb_void_t 			tb_database_sql_value_set_blob8(tb_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size);

/*! set the blob16 value
 *
 * @param value 	the value
 * @param data 		the blob data
 * @param size 		the blob size
 */
tb_void_t 			tb_database_sql_value_set_blob16(tb_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size);

/*! set the blob32 value
 *
 * @param value 	the value
 * @param data 		the blob data
 * @param size 		the blob size
 */
tb_void_t 			tb_database_sql_value_set_blob32(tb_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

/// the value is text?
static __tb_inline_force__ tb_bool_t tb_database_sql_value_is_text(tb_database_sql_value_t const* value)
{
	return ( 	value 
			&& 	( 	value->type == TB_DATABASE_SQL_VALUE_TYPE_TEXT32
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_TEXT16
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_TEXT8))? tb_true : tb_false;
}

/// the value is blob?
static __tb_inline_force__ tb_bool_t tb_database_sql_value_is_blob(tb_database_sql_value_t const* value)
{
	return ( 	value 
			&& 	( 	value->type == TB_DATABASE_SQL_VALUE_TYPE_BLOB32
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_BLOB16
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_BLOB8))? tb_true : tb_false;
}

/// the value is integer?
static __tb_inline_force__ tb_bool_t tb_database_sql_value_is_integer(tb_database_sql_value_t const* value)
{
	return ( 	value 
			&& 	( 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT32
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT64
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT16
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT8))? tb_true : tb_false;
}

/// the value is float?
#ifdef TB_CONFIG_TYPE_FLOAT
static __tb_inline_force__ tb_bool_t tb_database_sql_value_is_float(tb_database_sql_value_t const* value)
{
	return ( 	value 
			&& 	( 	value->type == TB_DATABASE_SQL_VALUE_TYPE_FLOAT
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_DOUBLE))? tb_true : tb_false;
}
#endif

/// the value is number?
static __tb_inline_force__ tb_bool_t tb_database_sql_value_is_number(tb_database_sql_value_t const* value)
{
	return ( 	value 
			&& 	( 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT32
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT64
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT16
#ifdef TB_CONFIG_TYPE_FLOAT
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_FLOAT
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_DOUBLE
#endif
				|| 	value->type == TB_DATABASE_SQL_VALUE_TYPE_INT8))? tb_true : tb_false;
}

/// the value name
static __tb_inline_force__ tb_char_t const* tb_database_sql_value_name(tb_database_sql_value_t const* value)
{
	// check
	tb_assert_and_check_return_val(value, tb_null);

	// the name
	return value->name;
}

/// the value text data
static __tb_inline_force__ tb_char_t const* tb_database_sql_value_text(tb_database_sql_value_t const* value)
{
	// check
	tb_assert_and_check_return_val(tb_database_sql_value_is_text(value), tb_null);

	// the text data
	return value->text.data;
}

/// the value blob data
static __tb_inline_force__ tb_byte_t const* tb_database_sql_value_blob(tb_database_sql_value_t const* value)
{
	// check
	tb_assert_and_check_return_val(tb_database_sql_value_is_blob(value), tb_null);

	// the blob data
	return value->blob.data;
}

/// set the value name
static __tb_inline_force__ tb_void_t tb_database_sql_value_name_set(tb_database_sql_value_t* value, tb_char_t const* name)
{
	// check
	tb_assert_and_check_return(value);

	// set the name
	value->name = name;
}


#endif
