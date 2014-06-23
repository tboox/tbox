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
 * @file        value.c
 * @ingroup     database
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "value"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "value.h"
#include "../stream/stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_object_database_sql_value_size(tb_object_database_sql_value_t const* value)
{
    // check
    tb_assert_and_check_return_val(value, 0);

    // done
    switch (value->type)
    {
    case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
        if (!value->u.text.hint && value->u.text.data)
        {
            ((tb_object_database_sql_value_t*)value)->u.text.hint = tb_strlen(value->u.text.data);
        }
        return value->u.text.hint;
    case TB_DATABASE_SQL_VALUE_TYPE_BLOB32:
    case TB_DATABASE_SQL_VALUE_TYPE_BLOB16:
    case TB_DATABASE_SQL_VALUE_TYPE_BLOB8:
        return value->u.blob.size;
    case TB_DATABASE_SQL_VALUE_TYPE_INT32:
    case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
#endif
        return 4;
    case TB_DATABASE_SQL_VALUE_TYPE_INT64:
    case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
#endif
        return 8;
    case TB_DATABASE_SQL_VALUE_TYPE_INT16:
    case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
        return 2;
    case TB_DATABASE_SQL_VALUE_TYPE_INT8:
    case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
        return 1;
    default:
        tb_trace_e("unknown type: %lu", value->type);
        break;
    }

    return 0;
}
tb_int8_t tb_object_database_sql_value_int8(tb_object_database_sql_value_t const* value)
{
    return (tb_int8_t)tb_object_database_sql_value_int32(value);
}
tb_int16_t tb_object_database_sql_value_int16(tb_object_database_sql_value_t const* value)
{
    return (tb_int16_t)tb_object_database_sql_value_int32(value);
}
tb_int32_t tb_object_database_sql_value_int32(tb_object_database_sql_value_t const* value)
{
    // check
    tb_assert_and_check_return_val(value, 0);

    // done
    switch (value->type)
    {
    case TB_DATABASE_SQL_VALUE_TYPE_INT32:
        return (tb_int32_t)value->u.i32;
    case TB_DATABASE_SQL_VALUE_TYPE_INT64:
        return (tb_int32_t)value->u.i64;
    case TB_DATABASE_SQL_VALUE_TYPE_INT16:
        return (tb_int32_t)value->u.i16;
    case TB_DATABASE_SQL_VALUE_TYPE_INT8:
        return (tb_int32_t)value->u.i8;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
        return (tb_int32_t)value->u.u32;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
        return (tb_int32_t)value->u.u64;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
        return (tb_int32_t)value->u.u16;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
        return (tb_int32_t)value->u.u8;
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
        return (tb_int32_t)value->u.f;
    case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
        return (tb_int32_t)value->u.d;
#endif
    case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
        return value->u.text.data? (tb_int32_t)tb_stoi32(value->u.text.data) : 0;
    default:
        tb_trace_e("unknown number type: %lu", value->type);
        break;
    }

    return 0;
}
tb_int64_t tb_object_database_sql_value_int64(tb_object_database_sql_value_t const* value)
{
    // check
    tb_assert_and_check_return_val(value, 0);

    // done
    switch (value->type)
    {
    case TB_DATABASE_SQL_VALUE_TYPE_INT64:
        return (tb_int64_t)value->u.i64;
    case TB_DATABASE_SQL_VALUE_TYPE_INT32:
        return (tb_int64_t)value->u.i32;
    case TB_DATABASE_SQL_VALUE_TYPE_INT16:
        return (tb_int64_t)value->u.i16;
    case TB_DATABASE_SQL_VALUE_TYPE_INT8:
        return (tb_int64_t)value->u.i8;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
        return (tb_int64_t)value->u.u64;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
        return (tb_int64_t)value->u.u32;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
        return (tb_int64_t)value->u.u16;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
        return (tb_int64_t)value->u.u8;
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
        return (tb_int64_t)value->u.f;
    case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
        return (tb_int64_t)value->u.d;
#endif
    case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
        return value->u.text.data? (tb_int64_t)tb_stoi64(value->u.text.data) : 0;
    default:
        tb_trace_e("unknown number type: %lu", value->type);
        break;
    }

    return 0;
}
tb_uint8_t tb_object_database_sql_value_uint8(tb_object_database_sql_value_t const* value)
{
    return (tb_uint8_t)tb_object_database_sql_value_uint32(value);
}
tb_uint16_t tb_object_database_sql_value_uint16(tb_object_database_sql_value_t const* value)
{
    return (tb_uint16_t)tb_object_database_sql_value_uint32(value);
}
tb_uint32_t tb_object_database_sql_value_uint32(tb_object_database_sql_value_t const* value)
{
    // check
    tb_assert_and_check_return_val(value, 0);

    // done
    switch (value->type)
    {
    case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
        return (tb_uint32_t)value->u.u32;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
        return (tb_uint32_t)value->u.u64;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
        return (tb_uint32_t)value->u.u16;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
        return (tb_uint32_t)value->u.u8;
    case TB_DATABASE_SQL_VALUE_TYPE_INT32:
        return (tb_uint32_t)value->u.i32;
    case TB_DATABASE_SQL_VALUE_TYPE_INT64:
        return (tb_uint32_t)value->u.i64;
    case TB_DATABASE_SQL_VALUE_TYPE_INT16:
        return (tb_uint32_t)value->u.i16;
    case TB_DATABASE_SQL_VALUE_TYPE_INT8:
        return (tb_uint32_t)value->u.i8;
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
        return (tb_uint32_t)value->u.f;
    case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
        return (tb_uint32_t)value->u.d;
#endif
    case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
        return value->u.text.data? (tb_uint32_t)tb_stoi32(value->u.text.data) : 0;
    default:
        tb_trace_e("unknown number type: %lu", value->type);
        break;
    }

    return 0;
}
tb_uint64_t tb_object_database_sql_value_uint64(tb_object_database_sql_value_t const* value)
{
    // check
    tb_assert_and_check_return_val(value, 0);

    // done
    switch (value->type)
    {
    case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
        return (tb_uint64_t)value->u.u64;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
        return (tb_uint64_t)value->u.u32;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
        return (tb_uint64_t)value->u.u16;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
        return (tb_uint64_t)value->u.u8;
    case TB_DATABASE_SQL_VALUE_TYPE_INT64:
        return (tb_uint64_t)value->u.i64;
    case TB_DATABASE_SQL_VALUE_TYPE_INT32:
        return (tb_uint64_t)value->u.i32;
    case TB_DATABASE_SQL_VALUE_TYPE_INT16:
        return (tb_uint64_t)value->u.i16;
    case TB_DATABASE_SQL_VALUE_TYPE_INT8:
        return (tb_uint64_t)value->u.i8;
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
        return (tb_uint64_t)value->u.f;
    case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
        return (tb_uint64_t)value->u.d;
#endif
    case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
        return value->u.text.data? (tb_uint64_t)tb_stou64(value->u.text.data) : 0;
    default:
        tb_trace_e("unknown number type: %lu", value->type);
        break;
    }

    return 0;
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_object_database_sql_value_float(tb_object_database_sql_value_t const* value)
{
    // check
    tb_assert_and_check_return_val(value, 0);

    // done
    switch (value->type)
    {
    case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
        return value->u.f;
    case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
        return (tb_float_t)value->u.d;
    case TB_DATABASE_SQL_VALUE_TYPE_INT64:
        return (tb_float_t)value->u.i64;
    case TB_DATABASE_SQL_VALUE_TYPE_INT32:
        return (tb_float_t)value->u.i32;
    case TB_DATABASE_SQL_VALUE_TYPE_INT16:
        return (tb_float_t)value->u.i16;
    case TB_DATABASE_SQL_VALUE_TYPE_INT8:
        return (tb_float_t)value->u.i8;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
        return (tb_float_t)value->u.u64;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
        return (tb_float_t)value->u.u32;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
        return (tb_float_t)value->u.u16;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
        return (tb_float_t)value->u.u8;
    case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
        return value->u.text.data? tb_stof(value->u.text.data) : 0;
    default:
        tb_trace_e("unknown number type: %lu", value->type);
        break;
    }

    return 0;
}
tb_double_t tb_object_database_sql_value_double(tb_object_database_sql_value_t const* value)
{
    // check
    tb_assert_and_check_return_val(value, 0);

    // done
    switch (value->type)
    {
    case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
        return (tb_double_t)value->u.f;
    case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
        return value->u.d;
    case TB_DATABASE_SQL_VALUE_TYPE_INT64:
        return (tb_double_t)value->u.i64;
    case TB_DATABASE_SQL_VALUE_TYPE_INT32:
        return (tb_double_t)value->u.i32;
    case TB_DATABASE_SQL_VALUE_TYPE_INT16:
        return (tb_double_t)value->u.i16;
    case TB_DATABASE_SQL_VALUE_TYPE_INT8:
        return (tb_double_t)value->u.i8;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT64:
        return (tb_double_t)value->u.u64;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT32:
        return (tb_double_t)value->u.u32;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT16:
        return (tb_double_t)value->u.u16;
    case TB_DATABASE_SQL_VALUE_TYPE_UINT8:
        return (tb_double_t)value->u.u8;
    case TB_DATABASE_SQL_VALUE_TYPE_TEXT:
        return value->u.text.data? tb_stod(value->u.text.data) : 0;
    default:
        tb_trace_e("unknown number type: %lu", value->type);
        break;
    }

    return 0;
}
#endif
tb_void_t tb_object_database_sql_value_set_null(tb_object_database_sql_value_t* value)
{
    // check
    tb_assert_and_check_return(value);

    // init null
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_NULL;
}
tb_void_t tb_object_database_sql_value_set_int8(tb_object_database_sql_value_t* value, tb_int8_t number)
{
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT8;
    value->u.i8         = number;
}
tb_void_t tb_object_database_sql_value_set_int16(tb_object_database_sql_value_t* value, tb_int16_t number)
{
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT16;
    value->u.i16        = number;
}
tb_void_t tb_object_database_sql_value_set_int32(tb_object_database_sql_value_t* value, tb_int32_t number)
{
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT32;
    value->u.i32        = number;
}
tb_void_t tb_object_database_sql_value_set_int64(tb_object_database_sql_value_t* value, tb_int64_t number)
{   
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT64;
    value->u.i64            = number;
}
tb_void_t tb_object_database_sql_value_set_uint8(tb_object_database_sql_value_t* value, tb_uint8_t number)
{
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT8;
    value->u.u8         = number;
}
tb_void_t tb_object_database_sql_value_set_uint16(tb_object_database_sql_value_t* value, tb_uint16_t number)
{
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT16;
    value->u.u16        = number;
}
tb_void_t tb_object_database_sql_value_set_uint32(tb_object_database_sql_value_t* value, tb_uint32_t number)
{
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT32;
    value->u.u32        = number;
}
tb_void_t tb_object_database_sql_value_set_uint64(tb_object_database_sql_value_t* value, tb_uint64_t number)
{   
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_INT64;
    value->u.u64        = number;
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_void_t tb_object_database_sql_value_set_float(tb_object_database_sql_value_t* value, tb_float_t number)
{
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_FLOAT;
    value->u.f          = number;
}
tb_void_t tb_object_database_sql_value_set_double(tb_object_database_sql_value_t* value, tb_double_t number)
{   
    // check
    tb_assert_and_check_return(value);

    // init number
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_DOUBLE;
    value->u.d          = number;
}
#endif
tb_void_t tb_object_database_sql_value_set_text(tb_object_database_sql_value_t* value, tb_char_t const* text, tb_size_t hint)
{
    // check
    tb_assert_and_check_return(value);

    // init text
    value->type         = TB_DATABASE_SQL_VALUE_TYPE_TEXT;
    value->u.text.data  = text;
    value->u.text.hint  = hint;
}
tb_void_t tb_object_database_sql_value_set_blob8(tb_object_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(value);

    // init blob
    value->type             = TB_DATABASE_SQL_VALUE_TYPE_BLOB8;
    value->u.blob.data      = data;
    value->u.blob.size      = size;
    value->u.blob.stream    = tb_object_null;

    // check size
    tb_assert(tb_object_database_sql_value_size(value) <= TB_MAXU8);
}
tb_void_t tb_object_database_sql_value_set_blob16(tb_object_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(value);

    // init blob
    value->type             = TB_DATABASE_SQL_VALUE_TYPE_BLOB16;
    value->u.blob.data      = data;
    value->u.blob.size      = size;
    value->u.blob.stream    = tb_object_null;

    // check size
    tb_assert(tb_object_database_sql_value_size(value) <= TB_MAXU16);
}
tb_void_t tb_object_database_sql_value_set_blob32(tb_object_database_sql_value_t* value, tb_byte_t const* data, tb_size_t size, tb_basic_stream_t* stream)
{
    // check
    tb_assert_and_check_return(value);

    // check stream
    tb_hong_t stream_size = 0;
    if (stream)
    {
        // must be opened 
        tb_assert_and_check_return(tb_stream_is_opened(stream));

        // the stream size
        stream_size = tb_stream_size(stream);
        tb_assert_and_check_return(stream_size >= 0 && stream_size < TB_MAXS32);
    }

    // init blob
    value->type             = TB_DATABASE_SQL_VALUE_TYPE_BLOB32;
    value->u.blob.data      = data;
    value->u.blob.size      = data? size : (tb_size_t)stream_size;
    value->u.blob.stream    = stream;

    // check size
    tb_assert(tb_object_database_sql_value_size(value) <= TB_MAXU32);
}
