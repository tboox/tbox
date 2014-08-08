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
 * @author      ruki
 * @file        prefix.h
 *
 */
#ifndef TB_DATABASE_IMPL_PREFIX_H
#define TB_DATABASE_IMPL_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../sql.h"
#include "sqlite3.h"
#include "mysql.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the database sql impl type
typedef struct __tb_database_sql_impl_t
{
    // the url
    tb_url_t                        url;

    // the type
    tb_size_t                       type;

    // the state
    tb_size_t                       state;

    // is opened?
    tb_bool_t                       bopened;

    // open
    tb_bool_t                       (*open)(struct __tb_database_sql_impl_t* database);

    // clos
    tb_void_t                       (*clos)(struct __tb_database_sql_impl_t* database);

    // exit
    tb_void_t                       (*exit)(struct __tb_database_sql_impl_t* database);

    // done
    tb_bool_t                       (*done)(struct __tb_database_sql_impl_t* database, tb_char_t const* sql);

    // begin
    tb_bool_t                       (*begin)(struct __tb_database_sql_impl_t* database);

    // commit
    tb_bool_t                       (*commit)(struct __tb_database_sql_impl_t* database);

    // rollback
    tb_bool_t                       (*rollback)(struct __tb_database_sql_impl_t* database);

    // load result
    tb_iterator_ref_t               (*result_load)(struct __tb_database_sql_impl_t* database, tb_bool_t try_all);

    // exit result
    tb_void_t                       (*result_exit)(struct __tb_database_sql_impl_t* database, tb_iterator_ref_t result);

    // statement init
    tb_database_sql_statement_ref_t (*statement_init)(struct __tb_database_sql_impl_t* database, tb_char_t const* sql);

    // statement exit
    tb_void_t                       (*statement_exit)(struct __tb_database_sql_impl_t* database, tb_database_sql_statement_ref_t statement);

    // statement done
    tb_bool_t                       (*statement_done)(struct __tb_database_sql_impl_t* database, tb_database_sql_statement_ref_t statement);

    // statement bind
    tb_bool_t                       (*statement_bind)(struct __tb_database_sql_impl_t* database, tb_database_sql_statement_ref_t statement, tb_database_sql_value_t const* list, tb_size_t size);

}tb_database_sql_impl_t;


#endif
