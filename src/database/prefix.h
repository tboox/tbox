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
 * @file		prefix.h
 * @ingroup 	database
 *
 */
#ifndef TB_DATABASE_PREFIX_H
#define TB_DATABASE_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../libc/libc.h"
#include "../network/url.h"
#include "../container/iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the sql database type enum
typedef enum __tb_database_sql_type_e
{
	TB_DATABASE_SQL_TYPE_NONE 				= 0
,	TB_DATABASE_SQL_TYPE_MYSQL 				= 1
,	TB_DATABASE_SQL_TYPE_SQLITE3 			= 2

}tb_database_sql_type_e;

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

/// the database type
typedef struct __tb_database_sql_t
{
	/// the url
	tb_url_t 					url;

	/// the type
	tb_size_t 					type;

	/// the state
	tb_size_t 					state;

	/// is opened?
	tb_bool_t 					bopened;

	/// open
	tb_bool_t 					(*open)(struct __tb_database_sql_t* database);

	/// clos
	tb_void_t 					(*clos)(struct __tb_database_sql_t* database);

	/// exit
	tb_void_t 					(*exit)(struct __tb_database_sql_t* database);

	/// done
	tb_bool_t 					(*done)(struct __tb_database_sql_t* database, tb_char_t const* sql);

	/// load result
	tb_iterator_t* 				(*result_load)(struct __tb_database_sql_t* database, tb_bool_t ball);

	/// exit result
	tb_void_t 					(*result_exit)(struct __tb_database_sql_t* database, tb_iterator_t* result);

	/// the result fields
	tb_iterator_t* 				(*result_fields)(struct __tb_database_sql_t* database, tb_iterator_t* result);

}tb_database_sql_t;


#endif
