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
 * @file		database.h
 * @defgroup 	database
 */
#ifndef TB_DATABASE_H
#define TB_DATABASE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init database
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
tb_handle_t 		tb_database_init(tb_char_t const* url);

/*! exit database
 *
 * @param database 	the database handle
 */
tb_void_t 			tb_database_exit(tb_handle_t database);

/*! the database type
 *
 * @param database 	the database handle
 *
 * @return 			the database type
 */
tb_size_t 			tb_database_type(tb_handle_t database);

/*! open database
 *
 * @code
 * tb_handle_t database = tb_database_init("sql://localhost/?type=mysql&username=xxxx&password=xxxx");
 * if (database)
 * {
 *     // open it
 *     if (tb_database_open(database))
 *     {
 *          // done it
 *          // ...
 *
 *          // close it
 *          tb_database_clos(database);
 *     }
 *     tb_database_exit(database);
 * }
 * @endcode
 *
 * @param database 	the database handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_open(tb_handle_t database);

/*! clos database
 *
 * @param database 	the database handle
 */
tb_void_t 			tb_database_clos(tb_handle_t database);

/*! the database state
 *
 * @param database 	the database handle
 *
 * @return 			the database state
 */
tb_size_t 			tb_database_state(tb_handle_t database);

/*! done database
 *
 * @code
 *
 * // done sql
 * if (!tb_database_done(database, "select * from table"))
 * {
 *     // trace
 *     tb_trace_e("done sql failed, error: %s", tb_state_cstr(tb_database_state(database)));
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
tb_bool_t 			tb_database_done(tb_handle_t database, tb_char_t const* sql);

/*! load the database result
 *
 * @code
 *
 * // done sql
 * // ..
 *
 * // load result
 * tb_iterator_t* result = tb_database_result_load(database);
 * if (result)
 * {
 *     // walk result
 *     tb_for_all_if (tb_iterator_t*, row, result, row)
 *     {
 *          // walk items
 *          tb_for_all_if (tb_database_result_item_t*, item, row, item)
 *          {
 *               tb_trace_i("name: %s, data: %s, size: %lu, at: %lux%lu", item->name, item->data, item->size, row_itor, item_itor);
 *          }
 *     }
 *
 *     // or
 *     tb_size_t row_itor = tb_iterator_head(result);
 *     tb_size_t row_tail = tb_iterator_tail(result);
 *     for (; row_itor != row_tail; row_itor = tb_iterator_next(result, row_itor))
 *     {
 *          tb_handle_t row = (tb_handle_t)tb_iterator_item(result, row_itor);
 *          if (row)
 *          {
 *              tb_size_t item_itor = 0;
 *              tb_size_t item_size = tb_iterator_size(row);
 *              for (item_itor = 0; item_itor < item_size; item_itor++)
 *              {
 *                   tb_database_result_item_t* item = (tb_database_result_item_t*)tb_iterator_item(row, item_itor);
 *                   if (item)
 *                   {
 *                       tb_trace_i("name: %s, data: %s, size: %lu, at: %lux%lu", item->name, item->data, item->size, row_itor, item_itor);
 *                   }
 *              }
 *          }
 *     }
 *
 *     // exit result
 *     tb_database_result_exit(result);
 * }
 * @endcode
 *
 * @param database 	the database handle
 *
 * @return 			the database result
 */
tb_iterator_t* 		tb_database_result_load(tb_handle_t database);

/*! exit the database result
 *
 * @param database 	the database handle
 * @param result 	the database result
 */
tb_void_t 			tb_database_result_exit(tb_handle_t database, tb_iterator_t* result);




#endif
