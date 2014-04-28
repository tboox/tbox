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
 * @defgroup 	sql
 */
#ifndef TB_SQL_H
#define TB_SQL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init sql
 *
 * @param url 		the database url
 *                  "sql://localhost/?source=mysql&user=xxxx&pwd=xxxx"
 *                  "sql://localhost:3306/?source=mysql&user=xxxx&pwd=xxxx&database=xxxx"
 *                  "sql:///home/file.sqlite3?source=sqlite3"
 *                  "/home/file.sqlite3"
 *                  "file:///home/file.sqlite3"
 *                  "C://home/file.sqlite3"
 *
 * @return 			tb_true or tb_false
 */
tb_handle_t 		tb_sql_init(tb_char_t const* url);

/*! exit sql
 *
 * @param sql 		the sql handle
 */
tb_void_t 			tb_sql_exit(tb_handle_t sql);

/*! open sql
 *
 * @param sql 		the sql handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_sql_open(tb_handle_t sql);

/*! clos sql
 *
 * @param sql 		the sql handle
 */
tb_void_t 			tb_sql_clos(tb_handle_t sql);




#endif
