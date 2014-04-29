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

/*! done database
 *
 * @param database 	the database handle
 * @param sql 		the sql command
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_database_done(tb_handle_t database, tb_char_t const* sql);





#endif
