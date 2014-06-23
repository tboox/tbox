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
 * @file        sql.c
 * @defgroup    database
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "database"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sql.h"
#include "mysql.h"
#include "sqlite3.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_object_database_sql_init(tb_char_t const* url)
{
    // check
    tb_assert_and_check_return_val(url, tb_object_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_object_database_sql_t*  database = tb_object_null;
    tb_url_t            database_url;
    do
    {
        // trace
        tb_trace_d("init: %s: ..", url);

        // init url
        if (!tb_url_init(&database_url)) break;

        // make url
        if (!tb_url_set(&database_url, url)) break;

        // check protocol
        tb_size_t protocol = tb_url_protocol_get(&database_url);
        tb_assert_and_check_break(protocol == TB_URL_PROTOCOL_SQL || protocol == TB_URL_PROTOCOL_FILE);

        // the probe func
        static tb_size_t (*s_probe[])(tb_url_t const*) = 
        {
            tb_object_null
#ifdef TB_CONFIG_THIRD_HAVE_MYSQL
        ,   tb_object_database_mysql_probe
#endif
#ifdef TB_CONFIG_THIRD_HAVE_SQLITE3
        ,   tb_object_database_sqlite3_probe
#endif
        };

        // the init func
        static tb_object_database_sql_t* (*s_init[])(tb_url_t const*) = 
        {
            tb_object_null
#ifdef TB_CONFIG_THIRD_HAVE_MYSQL
        ,   tb_object_database_mysql_init
#endif
#ifdef TB_CONFIG_THIRD_HAVE_SQLITE3
        ,   tb_object_database_sqlite3_init
#endif
        };

        // probe the database type
        tb_size_t i = 1;
        tb_size_t n = tb_object_arrayn(s_probe);
        tb_size_t s = 0;
        tb_size_t m = 0;
        for (; i < n; i++)
        {
            if (s_probe[i]) 
            {
                // probe it
                tb_size_t score = s_probe[i](&database_url);
                if (score > s) 
                {
                    // save the max score
                    s = score;
                    m = i;

                    // ok?
                    if (score == 100) break;
                }
            }
        }
        tb_check_break(m < n && s_init[m]);

        // init it
        database = s_init[m](&database_url);
        tb_assert_and_check_break(database);

        // trace
        tb_trace_d("init: %s: ok", url);

        // ok
        ok = tb_true;

    } while (0);

    // exit url
    tb_url_exit(&database_url);

    // failed?
    if (!ok)
    {
        // trace
        tb_trace_d("init: %s: no", url);

        // exit database
        if (database) tb_object_database_sql_exit((tb_handle_t)database);
        database = tb_object_null;
    }

    // ok?
    return database;
}
tb_void_t tb_object_database_sql_exit(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return(database);
        
    // trace
    tb_trace_d("exit: ..");

    // exit it
    if (database->exit) database->exit(database);

    // trace
    tb_trace_d("exit: ok");
}
tb_size_t tb_object_database_sql_type(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database, TB_DATABASE_SQL_TYPE_NONE);

    // the database type
    return database->type;
}
tb_bool_t tb_object_database_sql_open(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->open, tb_false);
    
    // opened?
    tb_check_return_val(!database->bopened, tb_true);
    
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;

    // open it
    database->bopened = database->open(database);

    // save state
    if (database->bopened) database->state = TB_STATE_OK;

    // ok?
    return database->bopened;
}
tb_void_t tb_object_database_sql_clos(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return(database);
            
    // opened?
    tb_check_return(database->bopened);

    // clos it
    if (database->clos) database->clos(database);
    
    // closed
    database->bopened = tb_false;
    
    // clear state
    database->state = TB_STATE_OK;
}
tb_size_t tb_object_database_sql_state(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database, TB_STATE_UNKNOWN_ERROR);
    
    // the state
    return database->state;
}
tb_bool_t tb_object_database_sql_begin(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->commit, tb_false);
    
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;
        
    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_false);

    // begin it
    tb_bool_t ok = database->begin(database);

    // save state
    if (ok) database->state = TB_STATE_OK;

    // ok?
    return ok;
}
tb_bool_t tb_object_database_sql_commit(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->commit, tb_false);
    
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;
        
    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_false);

    // commit it
    tb_bool_t ok = database->commit(database);

    // save state
    if (ok) database->state = TB_STATE_OK;

    // ok?
    return ok;
}
tb_bool_t tb_object_database_sql_rollback(tb_handle_t handle)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->rollback, tb_false);
    
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;
        
    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_false);

    // rollback it
    tb_bool_t ok = database->rollback(database);

    // save state
    if (ok) database->state = TB_STATE_OK;

    // ok?
    return ok;
}
tb_bool_t tb_object_database_sql_done(tb_handle_t handle, tb_char_t const* sql)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->done && sql, tb_false);
    
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;
        
    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_false);

    // done it
    tb_bool_t ok = database->done(database, sql);

    // trace
    tb_trace_d("done: sql: %s: %s", sql, ok? "ok" : "no");

    // save state
    if (ok) database->state = TB_STATE_OK;

    // ok?
    return ok;
}
tb_iterator_t* tb_object_database_sql_result_load(tb_handle_t handle, tb_bool_t ball)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->result_load, tb_object_null);
        
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;

    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_object_null);

    // load it
    tb_iterator_t* result = database->result_load(database, ball);

    // save state
    if (result) database->state = TB_STATE_OK;

    // ok?
    return result;
}
tb_void_t tb_object_database_sql_result_exit(tb_handle_t handle, tb_iterator_t* result)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return(database && database->result_exit && result);
            
    // opened?
    tb_assert_and_check_return(database->bopened);

    // exit it
    database->result_exit(database, result);

    // clear state
    database->state = TB_STATE_OK;
}
tb_handle_t tb_object_database_sql_statement_init(tb_handle_t handle, tb_char_t const* sql)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->statement_init && sql, tb_object_null);
        
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;

    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_object_null);

    // init stmt
    tb_handle_t stmt = database->statement_init(database, sql);
    
    // save state
    if (stmt) database->state = TB_STATE_OK;

    // ok?
    return stmt;
}
tb_void_t tb_object_database_sql_statement_exit(tb_handle_t handle, tb_handle_t stmt)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return(database && database->statement_done && stmt);
    
    // opened?
    tb_assert_and_check_return(database->bopened);

    // exit stmt
    database->statement_exit(database, stmt);

    // clear state
    database->state = TB_STATE_OK;
}
tb_bool_t tb_object_database_sql_statement_done(tb_handle_t handle, tb_handle_t stmt)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->statement_done && stmt, tb_false);
        
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;

    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_false);

    // done stmt
    tb_bool_t ok = database->statement_done(database, stmt);

    // save state
    if (ok) database->state = TB_STATE_OK;

    // ok?
    return ok;
}
tb_bool_t tb_object_database_sql_statement_bind(tb_handle_t handle, tb_handle_t stmt, tb_object_database_sql_value_t const* list, tb_size_t size)
{
    // check
    tb_object_database_sql_t* database = (tb_object_database_sql_t*)handle;
    tb_assert_and_check_return_val(database && database->statement_bind && stmt && list && size, tb_false);
        
    // init state
    database->state = TB_STATE_DATABASE_UNKNOWN_ERROR;

    // opened?
    tb_assert_and_check_return_val(database->bopened, tb_false);

    // bind stmt argument
    tb_bool_t ok = database->statement_bind(database, stmt, list, size);
    
    // save state
    if (ok) database->state = TB_STATE_OK;

    // ok?
    return ok;
}
