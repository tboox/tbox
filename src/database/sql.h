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
 * @file		sql.h
 * @defgroup 	database
 */
#ifndef TB_DATABASE_SQL_H
#define TB_DATABASE_SQL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "value.h"

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

	/// stmt init
	tb_handle_t 				(*stmt_init)(struct __tb_database_sql_t* database, tb_char_t const* sql);

	/// stmt exit
	tb_void_t 					(*stmt_exit)(struct __tb_database_sql_t* database, tb_handle_t stmt);

	/// stmt done
	tb_bool_t 					(*stmt_done)(struct __tb_database_sql_t* database, tb_handle_t stmt);

	/// stmt bind
	tb_bool_t 					(*stmt_bind)(struct __tb_database_sql_t* database, tb_handle_t stmt, tb_database_sql_value_t const* list, tb_size_t size);

}tb_database_sql_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init sql database
 *
 * @param url 		the database url
 *                  "sql://localhost/?type=mysql&username=xxxx&password=xxxx"
 *                  "sql://localhost:3306/?type=mysql&username=xxxx&password=xxxx&database=xxxx"
 *                  "sql:///home/file.sqlitedb?type=sqlite3"
 *                  "/home/file.sqlite3"
 *                  "file:///home/file.sqlitedb"
 *                  "C://home/file.sqlite3"
 *
 * @return 			tb_true or tb_false
 */
tb_handle_t 		tb_database_sql_init(tb_char_t const* url);

/*! exit database
 *
 * @param database 	the database handle
 */
tb_void_t 			tb_database_sql_exit(tb_handle_t database);

/*! the database type
 *
 * @param database 	the database handle
 *
 * @return 			the database type
 */
tb_size_t 			tb_database_sql_type(tb_handle_t database);

/*! open database
 *
 * @code
 * tb_handle_t database = tb_database_sql_init("sql://localhost/?type=mysql&username=xxxx&password=xxxx");
 * if (database)
 * {
 *     // open it
 *     if (tb_database_sql_open(database))
 *     {
 *          // done it
 *          // ...
 *
 *          // close it
 *          tb_database_sql_clos(database);
 *     }
 *     tb_database_sql_exit(database);
 * }
 * @endcode
 *
 * @param database 	the database handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_sql_open(tb_handle_t database);

/*! clos database
 *
 * @param database 	the database handle
 */
tb_void_t 			tb_database_sql_clos(tb_handle_t database);

/*! commit the current transaction
 *
 * @param database 	the database handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_sql_commit(tb_handle_t database);

/*! rollback the current transaction
 *
 * @param database 	the database handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_sql_rollback(tb_handle_t database);

/*! the database state
 *
 * TODO: ..
 *
 * @param database 	the database handle
 *
 * @return 			the database state
 */
tb_size_t 			tb_database_sql_state(tb_handle_t database);

/*! done database
 *
 * @code
 *
 * // done sql
 * if (!tb_database_sql_done(database, "select * from table"))
 * {
 *     // trace
 *     tb_trace_e("done sql failed, error: %s", tb_state_cstr(tb_database_sql_state(database)));
 *     return ;
 * }
 *
 * // load result
 * // ..
 *
 * @endcode
 *
 * @param database 	the database handle
 * @param sql 		the sql command
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_sql_done(tb_handle_t database, tb_char_t const* sql);

/*! load the database result
 *
 * @code
 *
 * // done sql
 * // ..
 *
 * // load result
 * tb_iterator_t* result = tb_database_sql_result_load(database, tb_true);
 * if (result)
 * {
 *     // walk result
 *     tb_for_all_if (tb_iterator_t*, row, result, row)
 *     {
 *          // walk values
 *          tb_for_all_if (tb_database_sql_value_t*, value, row, value)
 *          {
 *               tb_trace_i("name: %s, data: %s, at: %lux%lu", tb_database_sql_value_name(value), tb_database_sql_value_text(value), row_itor, item_itor);
 *          }
 *     }
 *
 *     // exit result
 *     tb_database_sql_result_exit(result);
 * }
 * @endcode
 *
 * @param database 	the database handle
 * @param ball 		load all result into memory?
 *
 * @return 			the database result
 */
tb_iterator_t* 		tb_database_sql_result_load(tb_handle_t database, tb_bool_t ball);

/*! exit the database result
 *
 * @param database 	the database handle
 * @param result 	the database result
 */
tb_void_t 			tb_database_sql_result_exit(tb_handle_t database, tb_iterator_t* result);

/*! init the database stmt
 *
 * @param database 	the database handle
 * @param sql 		the sql command
 *
 * @return 			the stmt handle
 */
tb_handle_t 		tb_database_sql_stmt_init(tb_handle_t database, tb_char_t const* sql);

/*! exit the database stmt
 *
 * @param database 	the database handle
 * @param stmt 		the stmt handle
 */
tb_void_t 			tb_database_sql_stmt_exit(tb_handle_t database, tb_handle_t stmt);

/*! done the database stmt
 *
 * @param database 	the database handle
 * @param stmt 		the stmt handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_sql_stmt_done(tb_handle_t database, tb_handle_t stmt);

/*! bind the database stmt argument
 *
 * @param database 	the database handle
 * @param stmt 		the stmt handle
 * @param list 		the argument value list
 * @param size 		the argument value count
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_sql_stmt_bind(tb_handle_t database, tb_handle_t stmt, tb_database_sql_value_t const* list, tb_size_t size);


#endif
