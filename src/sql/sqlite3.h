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
 * @file		sqlite3.h
 * @ingroup 	sql
 */
#ifndef TB_SQL_SQLITE3_H
#define TB_SQL_SQLITE3_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* probe sqlite3 from the url
 *
 * @param url 		the database url
 *
 * @return 			the score
 */
tb_size_t 			tb_sql_sqlite3_probe(tb_url_t const* url);

/* init sqlite3
 *
 * @param url 		the database url
 *
 * @return 			the sql handle
 */
tb_sql_t* 			tb_sql_sqlite3_init(tb_url_t const* url);

#endif
