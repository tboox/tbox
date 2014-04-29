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
#include "../network/url.h"
#include "../container/iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the sql database type enum
typedef enum __tb_database_database_type_e
{
	TB_DATABASE_TYPE_NONE 		= 0
,	TB_DATABASE_TYPE_MYSQL 		= 1
,	TB_DATABASE_TYPE_SQLITE3 	= 2

}tb_database_database_type_e;

/// the database type
typedef struct __tb_database_t
{
	/// the type
	tb_size_t 			type;

	/// the url
	tb_url_t 			url;

	/// is opened?
	tb_bool_t 			bopened;

	/// open
	tb_bool_t 			(*open)(struct __tb_database_t* database);

	/// clos
	tb_void_t 			(*clos)(struct __tb_database_t* database);

	/// exit
	tb_void_t 			(*exit)(struct __tb_database_t* database);

}tb_database_t;

#endif
