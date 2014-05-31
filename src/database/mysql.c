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
 * @file        mysql.c
 * @ingroup     database
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "mysql"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sql.h"
#include "mysql.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../stream/stream.h"
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include <mysql/mysqld_error.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mysql result row type
typedef struct __tb_database_mysql_result_row_t
{
    // the iterator
    tb_iterator_t                       itor;

    // the row
    MYSQL_ROW                           row;

    // the lengths
    tb_ulong_t*                         lengths;

    // the col count
    tb_size_t                           count;

    // the col value
    tb_database_sql_value_t             value;

}tb_database_mysql_result_row_t;

// the mysql result type
typedef struct __tb_database_mysql_result_t
{
    // the iterator
    tb_iterator_t                       itor;

    // the stmt
    MYSQL_STMT*                         stmt;

    // the result
    MYSQL_RES*                          result;

    // the fields
    MYSQL_FIELD*                        fields;

    // the metadata 
    MYSQL_RES*                          metadata;

    // the row count
    tb_size_t                           count;

    // try loading all?
    tb_bool_t                           try_all;

    // the row
    tb_database_mysql_result_row_t      row;

    // the stream
    tb_basic_stream_t*                  stream;

}tb_database_mysql_result_t;

// the mysql stream type
typedef struct __tb_database_mysql_stream_t
{
    // the base
    tb_basic_stream_t                   base;

    // the stmt
    MYSQL_STMT*                         stmt;

    // the result
    MYSQL_BIND*                         result;

    // the offset
    tb_size_t                           offset;

    // the column
    tb_size_t                           column;

}tb_database_mysql_stream_t;

// the mysql type
typedef struct __tb_database_mysql_t
{
    // the base
    tb_database_sql_t                   base;

    // the result
    tb_database_mysql_result_t          result;

    // the database
    MYSQL*                              database;

    // the bind list
    MYSQL_BIND*                         bind_list;

    // the bind maxn
    tb_size_t                           bind_maxn;

    // the bind data
    tb_scoped_buffer_t                  bind_data;

}tb_database_mysql_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
static tb_void_t tb_database_mysql_result_exit(tb_database_sql_t* database, tb_iterator_t* result);

/* //////////////////////////////////////////////////////////////////////////////////////
 * library implementation
 */
static tb_handle_t tb_database_mysql_library_init(tb_cpointer_t* ppriv)
{
    // init it
    if (mysql_library_init(0, tb_null, tb_null))
    {
        // trace
        tb_trace_e("init: mysql library failed!");
        return tb_null;
    }

    // ok
    return ppriv;
}
static tb_void_t tb_database_mysql_library_exit(tb_handle_t handle, tb_cpointer_t priv)
{
    // exit it
    mysql_library_end();
}
static tb_handle_t tb_database_mysql_library_load()
{
    return tb_singleton_instance(TB_SINGLETON_TYPE_LIBRARY_MYSQL, tb_database_mysql_library_init, tb_database_mysql_library_exit, tb_null);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * state implementation
 */
static tb_size_t tb_database_mysql_state_from_errno(tb_size_t errno)
{
    // done
    tb_size_t state = TB_STATE_DATABASE_UNKNOWN_ERROR;
    switch (errno)
    {
    case ER_NO_DB_ERROR:
    case ER_BAD_DB_ERROR:
        state = TB_STATE_DATABASE_NO_SUCH_DATABASE;
        break;
    case ER_NO_SUCH_TABLE:
        state = TB_STATE_DATABASE_NO_SUCH_TABLE;
        break;
    case ER_BAD_FIELD_ERROR:
        state = TB_STATE_DATABASE_NO_SUCH_FIELD;
        break;
    case ER_ACCESS_DENIED_ERROR:
        state = TB_STATE_DATABASE_ACCESS_DENIED;
        break;
    case ER_PARSE_ERROR:
        state = TB_STATE_DATABASE_PARSE_ERROR;
        break;
    case ER_WRONG_VALUE_COUNT_ON_ROW:
        state = TB_STATE_DATABASE_VALUE_COUNT_ERROR;
        break;
    case CR_UNKNOWN_HOST:
        state = TB_STATE_DATABASE_UNKNOWN_HOST;
        break;
    case ER_UNKNOWN_ERROR:
        break;
    default:
        tb_trace_e("unknown errno: %lu", errno);
        break;
    }

    // ok?
    return state;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * stream implementation
 */
static __tb_inline__ tb_database_mysql_stream_t* tb_database_mysql_stream_cast(tb_handle_t stream)
{
    tb_basic_stream_t* bstream = (tb_basic_stream_t*)stream;
    tb_assert_and_check_return_val(bstream && bstream->base.type == TB_STREAM_TYPE_NONE, tb_null);
    return (tb_database_mysql_stream_t*)bstream;
}
static tb_bool_t tb_database_mysql_stream_open(tb_handle_t stream)
{
    // check
    tb_database_mysql_stream_t* mstream = tb_database_mysql_stream_cast(stream);
    tb_assert_and_check_return_val(mstream && mstream->stmt, tb_false);

    // check result
    tb_assert_and_check_return_val(mstream->result && mstream->result->buffer && mstream->result->buffer_length, tb_false);

    // ok
    return tb_true;
}
static tb_bool_t tb_database_mysql_stream_clos(tb_handle_t stream)
{
    // check
    tb_database_mysql_stream_t* mstream = tb_database_mysql_stream_cast(stream);
    tb_assert_and_check_return_val(mstream, tb_false);
    
    // ok
    return tb_true;
}
static tb_long_t tb_database_mysql_stream_read(tb_handle_t stream, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_database_mysql_stream_t* mstream = tb_database_mysql_stream_cast(stream);
    tb_assert_and_check_return_val(mstream && mstream->stmt, -1);

    // check data and size
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // check result
    tb_assert_and_check_return_val(mstream->result && mstream->result->buffer && mstream->result->buffer_length, -1);

    // the length
    tb_size_t length = (tb_size_t)*mstream->result->length;

    // end?
    tb_check_return_val(length && mstream->offset < length, -1);

    // read data
    size = tb_min3(size, (tb_size_t)mstream->result->buffer_length, length - mstream->offset);
    if (size) tb_memcpy(data, mstream->result->buffer, size);
    
    // update offset
    mstream->offset += size;
    
    // fetch column
    if (mysql_stmt_fetch_column(mstream->stmt, mstream->result, mstream->column, mstream->offset))
    {
        // trace
        tb_trace_e("stream: fetch failed at: %lu, error[%d]: %s", mstream->column, mysql_stmt_errno(mstream->stmt), mysql_stmt_error(mstream->stmt));
        return -1;
    }

    // trace
//  tb_trace_d("stream: read: %lu", size);

    // ok?
    return (tb_long_t)(size);
}
static tb_long_t tb_database_mysql_stream_wait(tb_handle_t stream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_database_mysql_stream_t* mstream = tb_database_mysql_stream_cast(stream);
    tb_assert_and_check_return_val(mstream, -1);

    // ok?
    return wait;
}
static tb_bool_t tb_database_mysql_stream_ctrl(tb_handle_t stream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_database_mysql_stream_t* mstream = tb_database_mysql_stream_cast(stream);
    tb_assert_and_check_return_val(mstream, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // the psize
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize && mstream->result, tb_false);

            // get size
            *psize = (tb_hong_t)*mstream->result->length;

            // ok
            return tb_true;
        }   
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = mstream->offset;

            // ok
            return tb_true;
        }
    default:
        break;
    }
    return tb_false;
}
static tb_basic_stream_t* tb_database_mysql_stream_init(MYSQL_STMT* stmt, MYSQL_BIND* result, tb_size_t column)
{
    // check
    tb_assert_and_check_return_val(stmt && result, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_database_mysql_stream_t* stream = tb_null;
    do
    {
        // make stream
        stream = (tb_database_mysql_stream_t*)tb_malloc0(sizeof(tb_database_mysql_stream_t));
        tb_assert_and_check_break(stream);

        // init stream
        if (!tb_basic_stream_init((tb_basic_stream_t*)stream, TB_STREAM_TYPE_NONE, 0)) break;
        stream->base.open       = tb_database_mysql_stream_open;
        stream->base.clos       = tb_database_mysql_stream_clos;
        stream->base.read       = tb_database_mysql_stream_read;
        stream->base.wait       = tb_database_mysql_stream_wait;
        stream->base.base.ctrl  = tb_database_mysql_stream_ctrl;
        stream->stmt            = stmt;
        stream->result          = result;
        stream->column          = column;

        // ok
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok)
    {
        // exit it
        if (stream) tb_basic_stream_exit((tb_basic_stream_t*)stream);
        stream = tb_null;
    }

    // ok?
    return (tb_basic_stream_t*)stream;
}
static tb_bool_t tb_database_mysql_stream_set_value(tb_database_sql_value_t* value, tb_database_mysql_t* mysql, MYSQL_BIND* result, tb_size_t column)
{
    // check
    tb_assert_and_check_return_val(value && mysql && mysql->result.stmt && result, tb_false);
    
    // done
    tb_bool_t ok = tb_false;
    do
    {
        // exit the last stream first
        if (mysql->result.stream) tb_basic_stream_exit(mysql->result.stream);
        mysql->result.stream = tb_null;

        // init stream
        mysql->result.stream = tb_database_mysql_stream_init(mysql->result.stmt, result, column);
        tb_assert_and_check_break(mysql->result.stream);

        // open stream
        if (!tb_basic_stream_open(mysql->result.stream)) break;

        // set blob32
        tb_database_sql_value_set_blob32(value, tb_null, 0, mysql->result.stream);

        // ok 
        ok = tb_true;

    } while (0);

    // failed? 
    if (!ok) 
    {
        // exit it
        if (mysql->result.stream) tb_basic_stream_exit(mysql->result.stream);
        mysql->result.stream = tb_null;
    }

    // ok?
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator implementation
 */
static tb_size_t tb_database_mysql_result_row_iterator_size(tb_iterator_t* iterator)
{
    // check
    tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
    tb_assert_return_val(result, 0);

    // size
    return result->count;
}
static tb_size_t tb_database_mysql_result_row_iterator_head(tb_iterator_t* iterator)
{
    // head
    return 0;
}
static tb_size_t tb_database_mysql_result_row_iterator_tail(tb_iterator_t* iterator)
{
    // check
    tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
    tb_assert_return_val(result, 0);

    // tail
    return result->count;
}
static tb_size_t tb_database_mysql_result_row_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
    tb_assert_return_val(result, 0);
    tb_assert_and_check_return_val(itor && itor <= result->count, result->count);

    // load all? 
    tb_assert_and_check_return_val(result->try_all, result->count);

    // prev
    return itor - 1;
}
static tb_size_t tb_database_mysql_result_row_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
    tb_assert_return_val(result, 0);
    tb_assert_and_check_return_val(itor < result->count, result->count);

    // not load all? try fetching it
    if (!result->try_all)
    {
        // fetch stmt
        if (result->stmt)
        {
            // fetch the row
            tb_int_t ok = 0;
            if ((ok = mysql_stmt_fetch(result->stmt)))
            {
                // end or error?
                if (ok != MYSQL_DATA_TRUNCATED)
                {
                    // error?
                    if (ok != MYSQL_NO_DATA)
                    {
                        // the mysql
                        tb_database_mysql_t* mysql = (tb_database_mysql_t*)iterator->priv;

                        // save state
                        if (mysql) mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(result->stmt));

                        // trace
                        tb_trace_e("stmt: fetch row %lu failed, error[%d]: %s", itor, mysql_stmt_errno(result->stmt), mysql_stmt_error(result->stmt));
                    }

                    // end
                    return result->count;
                }
            }
        }
        // fetch result
        else 
        {
            // check
            tb_assert_and_check_return_val(result->result, result->count);
            
            // fetch the row
            result->row.row = mysql_fetch_row(result->result);
            tb_check_return_val(result->row.row, result->count);

            // fetch the lengths
            result->row.lengths = mysql_fetch_lengths(result->result);
            tb_assert_and_check_return_val(result->row.lengths, result->count);
        }
    }

    // next
    return itor + 1;
}
static tb_pointer_t tb_database_mysql_result_row_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_database_mysql_result_t* result = (tb_database_mysql_result_t*)iterator;
    tb_assert_and_check_return_val(result && itor < result->count, tb_null);

    // load all?
    if (result->try_all)
    {
        // load stmt row
        if (result->stmt)
        {
            // seek to the row number
            mysql_stmt_data_seek(result->stmt, itor);

            // fetch the row
            tb_int_t ok = 0;
            if ((ok = mysql_stmt_fetch(result->stmt)))
            {
                // end or error?
                if (ok != MYSQL_DATA_TRUNCATED)
                {
                    // error?
                    if (ok != MYSQL_NO_DATA)
                    {   
                        // the mysql
                        tb_database_mysql_t* mysql = (tb_database_mysql_t*)iterator->priv;

                        // save state
                        if (mysql) mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(result->stmt));

                        // trace
                        tb_trace_e("stmt: fetch row %lu failed, error[%d]: %s", itor, mysql_stmt_errno(result->stmt), mysql_stmt_error(result->stmt));
                    }
                    return tb_null;
                }
            }
        }
        // load result row
        else
        {
            // check
            tb_assert_and_check_return_val(result->result, tb_null);

            // seek to the row number
            mysql_data_seek(result->result, itor);
            
            // fetch the row
            result->row.row = mysql_fetch_row(result->result);
            tb_assert_and_check_return_val(result->row.row, tb_null);

            // fetch the lengths
            result->row.lengths = mysql_fetch_lengths(result->result);
            tb_assert_and_check_return_val(result->row.lengths, tb_null);
        }
    }

    // the row iterator
    return (tb_pointer_t)&result->row;
}
static tb_size_t tb_database_mysql_result_col_iterator_size(tb_iterator_t* iterator)
{
    // check
    tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
    tb_assert_and_check_return_val(row, 0);

    // size
    return row->count;
}
static tb_size_t tb_database_mysql_result_col_iterator_head(tb_iterator_t* iterator)
{
    // check
    tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
    tb_assert_and_check_return_val(row, 0);

    // head
    return 0;
}
static tb_size_t tb_database_mysql_result_col_iterator_tail(tb_iterator_t* iterator)
{
    // check
    tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
    tb_assert_and_check_return_val(row, 0);

    // tail
    return row->count;
}
static tb_size_t tb_database_mysql_result_col_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
    tb_assert_and_check_return_val(row && itor && itor <= row->count, 0);

    // prev
    return itor - 1;
}
static tb_size_t tb_database_mysql_result_col_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
    tb_assert_and_check_return_val(row && itor < row->count, row->count);

    // next
    return itor + 1;
}
static tb_pointer_t tb_database_mysql_result_col_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_database_mysql_result_row_t* row = (tb_database_mysql_result_row_t*)iterator;
    tb_assert_and_check_return_val(row && itor < row->count, tb_null);

    // the mysql
    tb_database_mysql_t* mysql = (tb_database_mysql_t*)iterator->priv;
    tb_assert_and_check_return_val(mysql && mysql->result.fields, tb_null);

    // the field
    MYSQL_FIELD* field = &mysql->result.fields[itor];

    // fetch column from stmt
    if (mysql->result.stmt)
    {
        // check
        tb_assert_and_check_return_val(mysql->bind_list && itor < mysql->bind_maxn, tb_null);

        // the result
        MYSQL_BIND* result = &mysql->bind_list[itor];

        // fetch column
        if (mysql_stmt_fetch_column(mysql->result.stmt, result, itor, 0))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(mysql->result.stmt));

            // trace
            tb_trace_e("stmt: fetch result failed at: %lu, field_type: %d, error[%d]: %s", itor, field->type, mysql_stmt_errno(mysql->result.stmt), mysql_stmt_error(mysql->result.stmt));
            return tb_null;
        }

        // init value
        tb_database_sql_value_name_set(&row->value, (tb_char_t const*)field->name);
        switch (result->buffer_type)
        {
        case MYSQL_TYPE_STRING:
            tb_database_sql_value_set_text(&row->value, (tb_char_t const*)result->buffer, (tb_size_t)*result->length);
            break;
        case MYSQL_TYPE_LONG:
            tb_database_sql_value_set_int32(&row->value, *((tb_int32_t const*)result->buffer));
            break;
        case MYSQL_TYPE_LONGLONG:
            tb_database_sql_value_set_int64(&row->value, *((tb_int64_t const*)result->buffer));
            break;
        case MYSQL_TYPE_SHORT:
            tb_database_sql_value_set_int16(&row->value, *((tb_int16_t const*)result->buffer));
            break;
        case MYSQL_TYPE_TINY:
            tb_database_sql_value_set_int8(&row->value, *((tb_int8_t const*)result->buffer));
            break;
        case MYSQL_TYPE_INT24:
            tb_database_sql_value_set_int32(&row->value, tb_bits_get_s24_ne((tb_byte_t const*)result->buffer));
            break;
            // note: the field type of text, tinyblob, blob and longblob always be blob
        case MYSQL_TYPE_BLOB:
            {
                // text?
                if (field->charsetnr != 63)
                {
                    tb_database_sql_value_set_text(&row->value, (tb_char_t const*)result->buffer, (tb_size_t)*result->length);
                }
                // blob?
                else
                {
                    // blob8?
                    if ((tb_size_t)*result->length <= TB_MAXU8)
                    {
                        tb_database_sql_value_set_blob8(&row->value, (tb_byte_t const*)result->buffer, (tb_size_t)*result->length);
                    }
                    // blob16?
                    else if ((tb_size_t)*result->length <= TB_MAXU16)
                    {
                        tb_database_sql_value_set_blob16(&row->value, (tb_byte_t const*)result->buffer, (tb_size_t)*result->length);
                    }
                    // blob32?
                    else
                    {
                        tb_database_mysql_stream_set_value(&row->value, mysql, result, itor);
                    }
                }
            }
            break;  
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
            tb_database_mysql_stream_set_value(&row->value, mysql, result, itor);
            break;
        case MYSQL_TYPE_TINY_BLOB:
            tb_database_sql_value_set_blob8(&row->value, (tb_byte_t const*)result->buffer, (tb_size_t)*result->length);
            break;
#ifdef TB_CONFIG_TYPE_FLOAT
        case MYSQL_TYPE_FLOAT:
            tb_database_sql_value_set_float(&row->value, *((tb_float_t const*)result->buffer));
            break;
        case MYSQL_TYPE_DOUBLE:
            tb_database_sql_value_set_double(&row->value, *((tb_double_t const*)result->buffer));
            break;
#endif
        case MYSQL_TYPE_NULL:
            tb_database_sql_value_set_null(&row->value);
            break;
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_ENUM:
            tb_trace_e("stmt: fetch result: not supported buffer type: %d", result->buffer_type);
            return tb_null;
        default:
            tb_trace_e("stmt: fetch result: unknown buffer type: %d", result->buffer_type);
            return tb_null;
        }
    }
    // fetch column from result
    else
    {
        // check
        tb_assert_and_check_return_val(row->row && row->lengths, tb_null);

        // init value
        tb_database_sql_value_name_set(&row->value, (tb_char_t const*)field->name);
        tb_database_sql_value_set_text(&row->value, (tb_char_t const*)row->row[itor], (tb_size_t)row->lengths[itor]);
    }

    // the col item
    return (tb_pointer_t)&row->value;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_database_mysql_t* tb_database_mysql_cast(tb_database_sql_t* database)
{
    // check
    tb_assert_and_check_return_val(database && database->type == TB_DATABASE_SQL_TYPE_MYSQL, tb_null);

    // cast
    return (tb_database_mysql_t*)database;
}
static tb_bool_t tb_database_mysql_open(tb_database_sql_t* database)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql, tb_false);

    // done
    tb_bool_t           ok = tb_false;
    tb_char_t const*    host = tb_null;
    tb_size_t           port = 0;
    tb_char_t           username[64] = {0};
    tb_char_t           password[64] = {0};
    tb_char_t           database_sql_name[64] = {0};
    do
    {
        // the database host
        host = tb_url_host_get(&database->url);
        tb_assert_and_check_break(host);

        // the database port
        port = tb_url_port_get(&database->url);

        // the database args
        tb_char_t const* args = tb_url_args_get(&database->url);
        if (args)
        {
            // the args size
            tb_size_t argn = tb_strlen(args);

            // the database username
            tb_char_t const* p = tb_stristr(args, "username=");
            if (p)
            {
                // skip to value
                p += 9;

                // the value end
                tb_char_t const* e = tb_strchr(p, '&');
                if (!e) e = args + argn;

                // save username
                if (p < e) tb_strlcpy(username, p, tb_min(e - p, sizeof(username) - 1));
            }
    
            // the database password
            p = tb_stristr(args, "password=");
            if (p)
            {
                // skip to value
                p += 9;

                // the value end
                tb_char_t const* e = tb_strchr(p, '&');
                if (!e) e = args + argn;

                // save password
                if (p < e) tb_strlcpy(password, p, tb_min(e - p, sizeof(password) - 1));
            }
    
            // the database name
            p = tb_stristr(args, "database=");
            if (p)
            {
                // skip to value
                p += 9;

                // the value end
                tb_char_t const* e = tb_strchr(p, '&');
                if (!e) e = args + argn;

                // save database name
                if (p < e) tb_strlcpy(database_sql_name, p, tb_min(e - p, sizeof(database_sql_name) - 1));
            }
        }

        // load mysql library
        if (!tb_database_mysql_library_load()) break;

        // init mysql database
        mysql->database = mysql_init(tb_null);
        tb_assert_and_check_break(mysql->database);

        // connect it
        if (!mysql_real_connect(mysql->database, host, username[0]? username : tb_null, password[0]? password : tb_null, database_sql_name[0]? database_sql_name : tb_null, (tb_uint_t)port, tb_null, 0))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_errno(mysql->database));

            // trace
            tb_trace_e("open: host: %s failed, error[%d]: %s", host, mysql_errno(mysql->database), mysql_error(mysql->database));
            break;
        }

        // disable auto commit
        if (mysql_autocommit(mysql->database, 0))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_errno(mysql->database));

            // trace
            tb_trace_e("open: disable auto commit failed, error[%d]: %s", mysql_errno(mysql->database), mysql_error(mysql->database));
            break;
        }

        // ok
        ok = tb_true;

    } while (0);

    // trace
    tb_trace_d("open: host: %s, port: %lu, username: %s, password: %s, database: %s : %s", host, port, username, password, database_sql_name, ok? "ok" : "no");

    // ok?
    return ok;
}
static tb_void_t tb_database_mysql_clos(tb_database_sql_t* database)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return(mysql);

    // clear bind data
    tb_scoped_buffer_clear(&mysql->bind_data);

    // clear bind list
    if (mysql->bind_list && mysql->bind_maxn) 
        tb_memset(mysql->bind_list, 0, mysql->bind_maxn * sizeof(MYSQL_BIND));

    // close database
    if (mysql->database) mysql_close(mysql->database);
    mysql->database = tb_null;
}
static tb_void_t tb_database_mysql_exit(tb_database_sql_t* database)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return(mysql);

    // close it first
    tb_database_mysql_clos(database);

    // exit bind data
    tb_scoped_buffer_exit(&mysql->bind_data);

    // exit bind list
    if (mysql->bind_list) tb_free(mysql->bind_list);
    mysql->bind_list = tb_null;
    mysql->bind_maxn = 0;

    // exit url
    tb_url_exit(&database->url);

    // exit it
    tb_free(mysql);
}
/* begin mysql transaction
 *
 * @note
 * the default storage engine MyIASM do not support transaction
 * need enable InnoDB engine and set autocommit=0 if you want to use it
 */
static tb_bool_t tb_database_mysql_begin(tb_database_sql_t* database)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database, tb_false);

    // done begin
    if (mysql_query(mysql->database, "begin;"))
    {
        // save state
        mysql->base.state = tb_database_mysql_state_from_errno(mysql_errno(mysql->database));

        // trace
        tb_trace_e("begin: failed, error[%d]: %s", mysql_errno(mysql->database), mysql_error(mysql->database));
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_database_mysql_commit(tb_database_sql_t* database)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database, tb_false);

    // done commit
    if (mysql_commit(mysql->database))
    {
        // save state
        mysql->base.state = tb_database_mysql_state_from_errno(mysql_errno(mysql->database));

        // trace
        tb_trace_e("commit: failed, error[%d]: %s", mysql_errno(mysql->database), mysql_error(mysql->database));
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_database_mysql_rollback(tb_database_sql_t* database)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database, tb_false);

    // done rollback
    if (mysql_rollback(mysql->database))
    {
        // save state
        mysql->base.state = tb_database_mysql_state_from_errno(mysql_errno(mysql->database));

        // trace
        tb_trace_e("rollback: failed, error[%d]: %s", mysql_errno(mysql->database), mysql_error(mysql->database));
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_database_mysql_done(tb_database_sql_t* database, tb_char_t const* sql)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database && sql, tb_false);

    // exit the last result first
    tb_database_mysql_result_exit(database, (tb_iterator_t*)&mysql->result);

    // done query
    if (mysql_query(mysql->database, sql))
    {
        // save state
        mysql->base.state = tb_database_mysql_state_from_errno(mysql_errno(mysql->database));

        // trace
        tb_trace_e("done: sql: %s failed, error[%d]: %s", sql, mysql_errno(mysql->database), mysql_error(mysql->database));
        return tb_false;
    }

    // trace
    tb_trace_d("done: sql: %s: ok", sql);

    // ok
    return tb_true;
}
static tb_void_t tb_database_mysql_result_exit(tb_database_sql_t* database, tb_iterator_t* result)
{
    // check
    tb_database_mysql_result_t* mysql_result = (tb_database_mysql_result_t*)result;
    tb_assert_and_check_return(mysql_result);

    // exit stream
    if (mysql_result->stream) tb_basic_stream_exit(mysql_result->stream);
    mysql_result->stream = tb_null;

    // exit result
    if (mysql_result->result) mysql_free_result(mysql_result->result);
    mysql_result->result = tb_null;
    mysql_result->fields = tb_null;

    // clear result
    mysql_result->count = 0;
    mysql_result->row.count = 0;

    // exit metadata
    if (mysql_result->metadata) mysql_free_result(mysql_result->metadata);
    mysql_result->metadata = tb_null;

    // clear stmt
    if (mysql_result->stmt && mysql_result->try_all)
        mysql_stmt_free_result(mysql_result->stmt);
    mysql_result->stmt = tb_null;

    // reset try all
    mysql_result->try_all = tb_false;
}
static tb_size_t tb_database_mysql_result_type_size(tb_size_t type)
{
    // done
    tb_size_t size = 0;
    switch (type)
    {
    case MYSQL_TYPE_STRING:         size = 8192;    break;
    case MYSQL_TYPE_LONG:           size = 4;       break;
    case MYSQL_TYPE_LONGLONG:       size = 8;       break;
    case MYSQL_TYPE_SHORT:          size = 2;       break;
    case MYSQL_TYPE_TINY:           size = 1;       break;
    case MYSQL_TYPE_INT24:          size = 3;       break;
    // TODO: for text and tinyblob
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:      size = 65536;   break;
    case MYSQL_TYPE_TINY_BLOB:      size = 256;     break;
#ifdef TB_CONFIG_TYPE_FLOAT
    case MYSQL_TYPE_FLOAT:          size = 4;       break;
    case MYSQL_TYPE_DOUBLE:         size = 8;       break;
#endif
    case MYSQL_TYPE_NULL:                           break;
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_TIMESTAMP:
    case MYSQL_TYPE_DATE:
    case MYSQL_TYPE_TIME:
    case MYSQL_TYPE_DATETIME:
    case MYSQL_TYPE_YEAR:
    case MYSQL_TYPE_SET:
    case MYSQL_TYPE_ENUM:
        tb_trace_e("not supported field type: %d", type);
        break;
    default:
        tb_trace_e("unknown field type: %d", type);
        break;
    }

    // ok?
    return size;
}
static tb_size_t tb_database_mysql_result_bind_maxn(tb_database_mysql_t* mysql)
{
    // check
    tb_assert_and_check_return_val(mysql && mysql->result.stmt && mysql->result.fields, 0);
    
    // walk
    tb_size_t i = 0;
    tb_size_t m = 0;
    tb_size_t n = mysql->result.row.count;
    for (i = 0; i < n; i++)
    {
        // += buffer
        m += tb_database_mysql_result_type_size(mysql->result.fields[i].type);

        // += length
        m += sizeof(tb_ulong_t);

        // += is_null
        m += sizeof(my_bool);
    }

    // ok?
    return m;
}
static tb_bool_t tb_database_mysql_result_bind_data(tb_database_mysql_t* mysql)
{
    // check
    tb_assert_and_check_return_val(mysql && mysql->result.stmt && mysql->result.fields, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check
        tb_assert_and_check_break(mysql->bind_list && mysql->result.row.count <= mysql->bind_maxn);

        // the bind data and maxn
        tb_byte_t*  bind_data = tb_scoped_buffer_data(&mysql->bind_data);
        tb_size_t   bind_maxn = tb_scoped_buffer_maxn(&mysql->bind_data);
        tb_assert_and_check_break(bind_data && bind_maxn);

        // clear data
        tb_memset(bind_data, 0, bind_maxn);
    
        // bind data
        tb_size_t   i = 0;
        tb_size_t   n = mysql->result.row.count;
        tb_byte_t*  p = bind_data;
        tb_byte_t*  e = bind_data + bind_maxn;
        for (i = 0; i < n; i++)
        {
            // the bind
            MYSQL_BIND* bind = &mysql->bind_list[i];

            // bind type
            bind->buffer_type = mysql->result.fields[i].type;

            // bind buffer length   
            bind->buffer_length = (tb_ulong_t)tb_database_mysql_result_type_size(bind->buffer_type);

            // bind buffer
            tb_assert_and_check_break(p + bind->buffer_length < e);
            bind->buffer = bind->buffer_length? (tb_char_t*)p : tb_null;
            p += bind->buffer_length;

            // bind is_unsigned
            bind->is_unsigned = (mysql->result.fields[i].flags & UNSIGNED_FLAG)? 1 : 0;
            
            // bind length
            tb_assert_and_check_break(p + sizeof(tb_ulong_t) < e);
            bind->length = (tb_ulong_t*)p;
            p += sizeof(tb_ulong_t);
    
            // bind is_null
            tb_assert_and_check_break(p + sizeof(my_bool) < e);
            bind->is_null = (my_bool*)p;
            p += sizeof(my_bool);
        }

        // check
        tb_assert_and_check_break(i == n);

        // bind result
        if (mysql_stmt_bind_result(mysql->result.stmt, mysql->bind_list))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(mysql->result.stmt));

            // trace
            tb_trace_e("stmt: bind result failed, error[%d]: %s", mysql_stmt_errno(mysql->result.stmt), mysql_stmt_error(mysql->result.stmt));
            break;
        }

        // ok 
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
static tb_bool_t tb_database_mysql_result_bind(tb_database_mysql_t* mysql, tb_bool_t try_all)
{
    // check
    tb_assert_and_check_return_val(mysql && mysql->result.stmt, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // load the field infos
        mysql->result.metadata = mysql_stmt_result_metadata(mysql->result.stmt);
        tb_check_break(mysql->result.metadata);

        // save result col count
        mysql->result.row.count = (tb_size_t)mysql_num_fields(mysql->result.metadata);
        tb_assert_and_check_break(mysql->result.row.count);

        // load result fields
        mysql->result.fields = mysql_fetch_fields(mysql->result.metadata);
        tb_assert_and_check_break(mysql->result.fields);

        // make bind list
        if (!mysql->bind_list)
        {
            mysql->bind_maxn = mysql->result.row.count + 16;
            mysql->bind_list = (MYSQL_BIND*)tb_nalloc(mysql->bind_maxn, sizeof(MYSQL_BIND));
        }
        // grow bind list
        else if (mysql->result.row.count > mysql->bind_maxn)
        {
            mysql->bind_maxn = mysql->result.row.count + 16;
            mysql->bind_list = (MYSQL_BIND*)tb_ralloc(mysql->bind_list, mysql->bind_maxn * sizeof(MYSQL_BIND));
        }

        // check
        tb_assert_and_check_break(mysql->bind_list && mysql->result.row.count <= mysql->bind_maxn);

        // clear bind list
        tb_memset(mysql->bind_list, 0, mysql->bind_maxn * sizeof(MYSQL_BIND));

        // compute bind maxn
        tb_size_t bind_maxn = tb_database_mysql_result_bind_maxn(mysql);
        tb_assert_and_check_break(bind_maxn);

        // resize bind data
        if (!tb_scoped_buffer_resize(&mysql->bind_data, bind_maxn)) break;

        // bind result data
        if (!tb_database_mysql_result_bind_data(mysql)) break;

        // load all?
        if (try_all && mysql_stmt_store_result(mysql->result.stmt))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(mysql->result.stmt));

            // trace
            tb_trace_e("stmt: load all result failed, error[%d]: %s", mysql_stmt_errno(mysql->result.stmt), mysql_stmt_error(mysql->result.stmt));
            break;
        }

        // try loading all?
        mysql->result.try_all = try_all;

        // save result row count
        mysql->result.count = try_all? (tb_size_t)mysql_stmt_num_rows(mysql->result.stmt) : -1;

        // init mode
        mysql->result.itor.mode = (try_all? TB_ITERATOR_MODE_RACCESS : TB_ITERATOR_MODE_FORWARD) | TB_ITERATOR_MODE_READONLY;

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
static tb_iterator_t* tb_database_mysql_result_load(tb_database_sql_t* database, tb_bool_t try_all)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database, tb_null);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // load result from stmt
        if (mysql->result.stmt)
        {
            // bind result
            if (!tb_database_mysql_result_bind(mysql, try_all)) break;
    
            // try fetching the first result
            if (!try_all)
            {
                // fetch the first row
                tb_int_t ok = 0;
                if ((ok = mysql_stmt_fetch(mysql->result.stmt)))
                {
                    // end or error?
                    if (ok != MYSQL_DATA_TRUNCATED)
                    {
                        // error?
                        if (ok != MYSQL_NO_DATA)
                        {
                            // save state
                            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(mysql->result.stmt));

                            // trace
                            tb_trace_e("stmt: fetch row head failed, error[%d]: %s", mysql_stmt_errno(mysql->result.stmt), mysql_stmt_error(mysql->result.stmt));
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            // load result
            mysql->result.result = try_all? mysql_store_result(mysql->database) : mysql_use_result(mysql->database);
            tb_check_break(mysql->result.result);

            // try fetching the first result
            if (!try_all)
            {
                // fetch the first row
                mysql->result.row.row = mysql_fetch_row(mysql->result.result);
                tb_check_break(mysql->result.row.row);

                // fetch the first lengths
                mysql->result.row.lengths = mysql_fetch_lengths(mysql->result.result);
                tb_assert_and_check_break(mysql->result.row.lengths);
            }

            // load result fields
            mysql->result.fields = mysql_fetch_fields(mysql->result.result);
            tb_assert_and_check_break(mysql->result.fields);

            // save result row count
            mysql->result.count = try_all? (tb_size_t)mysql_num_rows(mysql->result.result) : -1;

            // save result col count
            mysql->result.row.count = (tb_size_t)mysql_num_fields(mysql->result.result);

            // try loading all?
            mysql->result.try_all = try_all;

            // init mode
            mysql->result.itor.mode = (try_all? TB_ITERATOR_MODE_RACCESS : TB_ITERATOR_MODE_FORWARD) | TB_ITERATOR_MODE_READONLY;
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit result
        tb_database_mysql_result_exit(database, (tb_iterator_t*)&mysql->result);
    }

    // ok?
    return ok? (tb_iterator_t*)&mysql->result : tb_null;
}
static tb_handle_t tb_database_mysql_statement_init(tb_database_sql_t* database, tb_char_t const* sql)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database && sql, tb_null);

    // done
    tb_bool_t   ok = tb_false;
    MYSQL_STMT* stmt = tb_null;
    do
    {
        // init stmt
        stmt = mysql_stmt_init(mysql->database);
        if (!stmt)
        {   
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_errno(mysql->database));

            // trace
            tb_trace_e("stmt: init: %s failed, error[%d]: %s", sql, mysql_errno(mysql->database), mysql_error(mysql->database));
            break;
        }

        // prepare stmt
        if (mysql_stmt_prepare(stmt, sql, tb_strlen(sql)))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(stmt));

            // trace
            tb_trace_e("stmt: prepare: %s failed, error[%d]: %s", sql, mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
            break;
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stmt) mysql_stmt_close(stmt);
        stmt = tb_null;
    }

    // ok?
    return (tb_handle_t)stmt;
}
static tb_void_t tb_database_mysql_statement_exit(tb_database_sql_t* database, tb_handle_t stmt)
{
    // exit it
    if (stmt) mysql_stmt_close((MYSQL_STMT*)stmt);
}
static tb_bool_t tb_database_mysql_statement_done(tb_database_sql_t* database, tb_handle_t stmt)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database && stmt, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // exit the last result first
        tb_database_mysql_result_exit(database, (tb_iterator_t*)&mysql->result);

        // done stmt
        if (mysql_stmt_execute((MYSQL_STMT*)stmt))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno(stmt));

            // trace
            tb_trace_e("stmt: done failed, error[%d]: %s", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
            break;
        }

        // save stmt
        mysql->result.stmt = (MYSQL_STMT*)stmt;

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
static tb_bool_t tb_database_mysql_statement_bind(tb_database_sql_t* database, tb_handle_t stmt, tb_database_sql_value_t const* list, tb_size_t size)
{
    // check
    tb_database_mysql_t* mysql = tb_database_mysql_cast(database);
    tb_assert_and_check_return_val(mysql && mysql->database && stmt && list && size, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check the param count
        tb_size_t param_count = mysql_stmt_param_count((MYSQL_STMT*)stmt);
        tb_assert_and_check_break(size == param_count);

        // make bind list
        if (!mysql->bind_list)
        {
            mysql->bind_maxn = size + 16;
            mysql->bind_list = (MYSQL_BIND*)tb_nalloc(mysql->bind_maxn, sizeof(MYSQL_BIND));
        }
        // grow bind list
        else if (size > mysql->bind_maxn)
        {
            mysql->bind_maxn = size + 16;
            mysql->bind_list = (MYSQL_BIND*)tb_ralloc(mysql->bind_list, mysql->bind_maxn * sizeof(MYSQL_BIND));
        }

        // check
        tb_assert_and_check_break(mysql->bind_list && size <= mysql->bind_maxn);

        // clear bind list
        tb_memset(mysql->bind_list, 0, mysql->bind_maxn * sizeof(MYSQL_BIND));

        // init bind list
        tb_size_t i = 0;
        for (i = 0; i < size; i++)
        {
            // the value
            tb_database_sql_value_t const* value = &list[i];
            switch (value->type)
            {
            case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_STRING;   
                mysql->bind_list[i].buffer          = (tb_char_t*)tb_database_sql_value_text(value);   
                mysql->bind_list[i].buffer_length   = tb_database_sql_value_size(value) + 1;   
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_INT64:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_LONGLONG;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.i64;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_INT32:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_LONG;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.i32;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_INT16:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_SHORT;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.i16;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_INT8:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_TINY;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.i8;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_LONGLONG;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.u64;
                mysql->bind_list[i].is_unsigned     = 1;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_LONG;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.u32;
                mysql->bind_list[i].is_unsigned     = 1;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_SHORT;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.u16;
                mysql->bind_list[i].is_unsigned     = 1;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_TINY;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.u8;
                mysql->bind_list[i].is_unsigned     = 1;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_BLOB32:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_LONG_BLOB;   
                mysql->bind_list[i].buffer          = (tb_char_t*)tb_database_sql_value_blob(value);   
                mysql->bind_list[i].buffer_length   = tb_database_sql_value_size(value);   
                mysql->bind_list[i].length          = &mysql->bind_list[i].buffer_length; 
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_BLOB16:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_BLOB;   
                mysql->bind_list[i].buffer          = (tb_char_t*)tb_database_sql_value_blob(value);   
                mysql->bind_list[i].buffer_length   = tb_database_sql_value_size(value);   
                mysql->bind_list[i].length          = &mysql->bind_list[i].buffer_length; 
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_BLOB8:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_TINY_BLOB;   
                mysql->bind_list[i].buffer          = (tb_char_t*)tb_database_sql_value_blob(value);   
                mysql->bind_list[i].buffer_length   = tb_database_sql_value_size(value);   
                mysql->bind_list[i].length          = &mysql->bind_list[i].buffer_length; 
                break;
#ifdef TB_CONFIG_TYPE_FLOAT
            case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_FLOAT;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.f;
                break;
            case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_DOUBLE;   
                mysql->bind_list[i].buffer          = (tb_char_t*)&value->u.d;
                break;
#endif
            case TB_DATABASE_SQL_VALUE_TYPE_NULL:
                mysql->bind_list[i].buffer_type     = MYSQL_TYPE_NULL;   
                break;
            default:
                tb_trace_e("stmt: bind: unknown value type: %lu", value->type);
                break;
            }
        }

        // bind it
        if (mysql_stmt_bind_param((MYSQL_STMT*)stmt, mysql->bind_list))
        {
            // save state
            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno((MYSQL_STMT*)stmt));

            // trace
            tb_trace_e("stmt: bind failed, error[%d]: %s", mysql_stmt_errno((MYSQL_STMT*)stmt), mysql_stmt_error((MYSQL_STMT*)stmt));
            break;
        }
        
        // send blob32 data
        tb_byte_t data[TB_BASIC_STREAM_BLOCK_MAXN];
        for (i = 0; i < size; i++)
        {
            // the value
            tb_database_sql_value_t const* value = &list[i];
            if (value->type == TB_DATABASE_SQL_VALUE_TYPE_BLOB32 && value->u.blob.stream)
            {
                // trace
                tb_trace_d("stmt: bind: send: blob: %lld: ..", tb_stream_size(value->u.blob.stream));

                // done
                while (!tb_stream_beof(value->u.blob.stream))
                {
                    // read it
                    tb_long_t real = tb_basic_stream_read(value->u.blob.stream, data, sizeof(data));
                    if (real > 0)
                    {
                        // send it
                        if (mysql_stmt_send_long_data((MYSQL_STMT*)stmt, i, (tb_char_t const*)data, real))
                        {
                            // save state
                            mysql->base.state = tb_database_mysql_state_from_errno(mysql_stmt_errno((MYSQL_STMT*)stmt));

                            // trace
                            tb_trace_e("stmt: bind: send blob data failed, error[%d]: %s", mysql_stmt_errno((MYSQL_STMT*)stmt), mysql_stmt_error((MYSQL_STMT*)stmt));
                            break;
                        }
                    }
                    else if (!real)
                    {
                        // wait 
                        tb_long_t wait = tb_basic_stream_wait(value->u.blob.stream, TB_BASIC_STREAM_WAIT_READ, tb_stream_timeout(value->u.blob.stream));
                        tb_assert_and_check_break(wait > 0);
                    }
                    else break;
                }
            }
        }

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_size_t tb_database_mysql_probe(tb_url_t const* url)
{
    // check
    tb_assert_and_check_return_val(url, 0);

    // done
    tb_size_t score = 0;
    do
    {
        // the url arguments
        tb_char_t const* args = tb_url_args_get(url);
        if (args)
        {
            // find the database type
            tb_char_t const* ptype = tb_stristr(args, "type=");
            if (ptype && !tb_strnicmp(ptype + 5, "mysql", 5))
            {
                // ok
                score = 100;
                break;
            }
        }

        // the database port, the default port: 3306 
        if (tb_url_port_get(url) == 3306) score += 20;

        // is sql url? 
        if (tb_url_protocol_get(url) == TB_URL_PROTOCOL_SQL) 
            score += 5;

    } while (0);

    // trace
    tb_trace_d("probe: %s, score: %lu", tb_url_get((tb_url_t*)url), score);

    // ok?
    return score;
}
tb_database_sql_t* tb_database_mysql_init(tb_url_t const* url)
{
    // check
    tb_assert_and_check_return_val(url, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_database_mysql_t*    mysql = tb_null;
    do
    {
        // make database
        mysql = tb_malloc0(sizeof(tb_database_mysql_t));
        tb_assert_and_check_break(mysql);

        // init database
        mysql->base.type            = TB_DATABASE_SQL_TYPE_MYSQL;
        mysql->base.open            = tb_database_mysql_open;
        mysql->base.clos            = tb_database_mysql_clos;
        mysql->base.exit            = tb_database_mysql_exit;
        mysql->base.done            = tb_database_mysql_done;
        mysql->base.begin           = tb_database_mysql_begin;
        mysql->base.commit          = tb_database_mysql_commit;
        mysql->base.rollback        = tb_database_mysql_rollback;
        mysql->base.result_load     = tb_database_mysql_result_load;
        mysql->base.result_exit     = tb_database_mysql_result_exit;
        mysql->base.statement_init  = tb_database_mysql_statement_init;
        mysql->base.statement_exit  = tb_database_mysql_statement_exit;
        mysql->base.statement_done  = tb_database_mysql_statement_done;
        mysql->base.statement_bind  = tb_database_mysql_statement_bind;

        // init result row iterator
        mysql->result.itor.mode     = TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_READONLY;
        mysql->result.itor.priv     = (tb_pointer_t)mysql;
        mysql->result.itor.step     = 0;
        mysql->result.itor.size     = tb_database_mysql_result_row_iterator_size;
        mysql->result.itor.head     = tb_database_mysql_result_row_iterator_head;
        mysql->result.itor.tail     = tb_database_mysql_result_row_iterator_tail;
        mysql->result.itor.prev     = tb_database_mysql_result_row_iterator_prev;
        mysql->result.itor.next     = tb_database_mysql_result_row_iterator_next;
        mysql->result.itor.item     = tb_database_mysql_result_row_iterator_item;
        mysql->result.itor.copy     = tb_null;
        mysql->result.itor.comp     = tb_null;

        // init result col iterator
        mysql->result.row.itor.mode = TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_READONLY;
        mysql->result.row.itor.priv = (tb_pointer_t)mysql;
        mysql->result.row.itor.step = 0;
        mysql->result.row.itor.size = tb_database_mysql_result_col_iterator_size;
        mysql->result.row.itor.head = tb_database_mysql_result_col_iterator_head;
        mysql->result.row.itor.tail = tb_database_mysql_result_col_iterator_tail;
        mysql->result.row.itor.prev = tb_database_mysql_result_col_iterator_prev;
        mysql->result.row.itor.next = tb_database_mysql_result_col_iterator_next;
        mysql->result.row.itor.item = tb_database_mysql_result_col_iterator_item;
        mysql->result.row.itor.copy = tb_null;
        mysql->result.row.itor.comp = tb_null;

        // init url
        if (!tb_url_init(&mysql->base.url)) break;

        // copy url
        tb_url_copy(&mysql->base.url, url);

        // init state
        mysql->base.state = TB_STATE_OK;

        // init bind 
        if (!tb_scoped_buffer_init(&mysql->bind_data)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok) 
    {
        // exit database
        if (mysql) tb_database_mysql_exit((tb_database_sql_t*)mysql);
        mysql = tb_null;
    }

    // ok?
    return (tb_database_sql_t*)mysql;
}

