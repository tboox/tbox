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
 * @file        bin.c
 * @ingroup     object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_writer_bin"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bin.h"
#include "../object.h"
#include "../../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_object_bin_writer_func_null(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

    // writ type & null
    return tb_object_writer_bin_type_size(writer->stream, object->type, 0);
}
static tb_bool_t tb_object_bin_writer_func_date(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

    // writ type & time
    return tb_object_writer_bin_type_size(writer->stream, object->type, (tb_uint64_t)tb_object_date_time(object));
}
static tb_bool_t tb_object_bin_writer_func_data(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

    // the data & size
    tb_byte_t const*    data = (tb_byte_t const*)tb_object_data_getp(object);
    tb_size_t           size = tb_object_data_size(object);

    // writ type & size
    if (!tb_object_writer_bin_type_size(writer->stream, object->type, size)) return tb_false;

    // empty?
    tb_check_return_val(size, tb_true);

    // check 
    tb_assert_and_check_return_val(data, tb_false);

    // make the encoder data
    if (!writer->data)
    {
        writer->maxn = tb_max(size, 8192);
        writer->data = tb_malloc0_bytes(writer->maxn);
    }
    else if (writer->maxn < size)
    {
        writer->maxn = size;
        writer->data = (tb_byte_t*)tb_ralloc(writer->data, writer->maxn);
    }
    tb_assert_and_check_return_val(writer->data && size <= writer->maxn, tb_false);

    // copy data to encoder
    tb_memcpy(writer->data, data, size);

    // encode data
    tb_byte_t const*    pb = data;
    tb_byte_t const*    pe = data + size;
    tb_byte_t*          qb = writer->data;
    tb_byte_t*          qe = writer->data + writer->maxn;
    tb_byte_t           xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
    for (; pb < pe && qb < qe; pb++, qb++, xb++) *qb = *pb ^ xb;

    // writ it
    return tb_stream_bwrit(writer->stream, writer->data, size);
}
static tb_bool_t tb_object_bin_writer_func_array(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream && writer->ohash, tb_false);

    // writ type & size
    if (!tb_object_writer_bin_type_size(writer->stream, object->type, tb_object_array_size(object))) return tb_false;

    // walk
    tb_for_all (tb_object_t*, item, tb_object_array_itor(object))
    {
        if (item)
        {
            // exists?
            tb_size_t index = (tb_size_t)tb_hash_get(writer->ohash, item);
            if (index)
            {
                // writ index
                if (!tb_object_writer_bin_type_size(writer->stream, 0, (tb_uint64_t)(index - 1))) return tb_false;
            }
            else
            {
                // the func
                tb_object_bin_writer_func_t func = tb_object_bin_writer_func(item->type);
                tb_assert_and_check_continue(func);

                // writ it
                if (!func(writer, item)) return tb_false;

                // save index
                tb_hash_set(writer->ohash, item, (tb_cpointer_t)(++writer->index));
            }
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_object_bin_writer_func_string(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

    // the data & size
    tb_char_t const*    data = tb_object_string_cstr(object);
    tb_size_t           size = tb_object_string_size(object);

    // writ type & size
    if (!tb_object_writer_bin_type_size(writer->stream, object->type, size)) return tb_false;

    // empty?
    tb_check_return_val(size, tb_true);

    // check
    tb_assert_and_check_return_val(data, tb_false);

    // make the encoder data
    if (!writer->data)
    {
        writer->maxn = tb_max(size, 8192);
        writer->data = tb_malloc0_bytes(writer->maxn);
    }
    else if (writer->maxn < size)
    {
        writer->maxn = size;
        writer->data = (tb_byte_t*)tb_ralloc(writer->data, writer->maxn);
    }
    tb_assert_and_check_return_val(writer->data && size <= writer->maxn, tb_false);

    // copy data to encoder
    tb_memcpy(writer->data, data, size);

    // encode data
    tb_byte_t const*    pb = (tb_byte_t const*)data;
    tb_byte_t const*    pe = (tb_byte_t const*)data + size;
    tb_byte_t*          qb = writer->data;
    tb_byte_t*          qe = writer->data + writer->maxn;
    tb_byte_t           xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
    for (; pb < pe && qb < qe && *pb; pb++, qb++, xb++) *qb = *pb ^ xb;

    // writ it
    return tb_stream_bwrit(writer->stream, writer->data, size);
}
static tb_bool_t tb_object_bin_writer_func_number(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

    // writ type
    if (!tb_object_writer_bin_type_size(writer->stream, object->type, (tb_uint64_t)tb_object_number_type(object))) return tb_false;

    // writ number
    switch (tb_object_number_type(object))
    {
    case TB_NUMBER_TYPE_UINT64:
        if (!tb_stream_bwrit_u64_be(writer->stream, tb_object_number_uint64(object))) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT64:
        if (!tb_stream_bwrit_s64_be(writer->stream, tb_object_number_sint64(object))) return tb_false;
        break;
    case TB_NUMBER_TYPE_UINT32:
        if (!tb_stream_bwrit_u32_be(writer->stream, tb_object_number_uint32(object))) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT32:
        if (!tb_stream_bwrit_s32_be(writer->stream, tb_object_number_sint32(object))) return tb_false;
        break;
    case TB_NUMBER_TYPE_UINT16:
        if (!tb_stream_bwrit_u16_be(writer->stream, tb_object_number_uint16(object))) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT16:
        if (!tb_stream_bwrit_s16_be(writer->stream, tb_object_number_sint16(object))) return tb_false;
        break;
    case TB_NUMBER_TYPE_UINT8:
        if (!tb_stream_bwrit_u8(writer->stream, tb_object_number_uint8(object))) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT8:
        if (!tb_stream_bwrit_s8(writer->stream, tb_object_number_sint8(object))) return tb_false;
        break;
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_NUMBER_TYPE_FLOAT:
        {
            tb_byte_t data[4];
            tb_bits_set_float_be(data, tb_object_number_float(object));
            if (!tb_stream_bwrit(writer->stream, data, 4)) return tb_false;
        }
        break;
    case TB_NUMBER_TYPE_DOUBLE:
        {
            tb_byte_t data[8];
            tb_bits_set_double_bbe(data, tb_object_number_double(object));
            if (!tb_stream_bwrit(writer->stream, data, 8)) return tb_false;
        }
        break;
#endif
    default:
        tb_assert_and_check_return_val(0, tb_false);
        break;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_object_bin_writer_func_boolean(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream, tb_false);

    // writ type & bool
    return tb_object_writer_bin_type_size(writer->stream, object->type, tb_object_boolean_bool(object));
}
static tb_bool_t tb_object_bin_writer_func_dictionary(tb_object_bin_writer_t* writer, tb_object_t* object)
{
    // check
    tb_assert_and_check_return_val(object && writer && writer->stream && writer->ohash, tb_false);

    // writ type & size
    if (!tb_object_writer_bin_type_size(writer->stream, object->type, tb_object_dictionary_size(object))) return tb_false;

    // walk
    tb_for_all (tb_object_dictionary_item_t*, item, tb_object_dictionary_itor(object))
    {
        if (item)
        {
            tb_char_t const*    key = item->key;
            tb_object_t*        val = item->val;
            if (key && val)
            {
                // writ key
                {
                    // exists?
                    tb_size_t index = (tb_size_t)tb_hash_get(writer->shash, key);
                    if (index)
                    {
                        // writ index
                        if (!tb_object_writer_bin_type_size(writer->stream, 0, (tb_uint64_t)(index - 1))) return tb_false;
                    }
                    else
                    {
                        // the func
                        tb_object_bin_writer_func_t func = tb_object_bin_writer_func(TB_OBJECT_TYPE_STRING);
                        tb_assert_and_check_return_val(func, tb_false);

                        // make the key object
                        tb_object_t* okey = tb_object_string_init_from_cstr(key);
                        tb_assert_and_check_return_val(okey, tb_false);

                        // writ it
                        if (!func(writer, okey)) return tb_false;

                        // exit it
                        tb_object_exit(okey);

                        // save index
                        tb_hash_set(writer->shash, key, (tb_cpointer_t)(++writer->index));
                    }
                }

                // writ val
                {
                    // exists?
                    tb_size_t index = (tb_size_t)tb_hash_get(writer->ohash, val);
                    if (index)
                    {
                        // writ index
                        if (!tb_object_writer_bin_type_size(writer->stream, 0, (tb_uint64_t)(index - 1))) return tb_false;
                    }
                    else
                    {
                        // the func
                        tb_object_bin_writer_func_t func = tb_object_bin_writer_func(val->type);
                        tb_assert_and_check_return_val(func, tb_false);

                        // writ it
                        if (!func(writer, val)) return tb_false;

                        // save index
                        tb_hash_set(writer->ohash, val, (tb_cpointer_t)(++writer->index));
                    }
                }
            }
        }
    }

    // ok
    return tb_true;
}
static tb_long_t tb_object_bin_writer_done(tb_stream_ref_t stream, tb_object_t* object, tb_bool_t deflate)
{
    // check
    tb_assert_and_check_return_val(object && stream, -1);

    // the func
    tb_object_bin_writer_func_t func = tb_object_bin_writer_func(object->type);
    tb_assert_and_check_return_val(func, -1);

    // the begin offset
    tb_hize_t bof = tb_stream_offset(stream);

    // writ bin header
    if (!tb_stream_bwrit(stream, (tb_byte_t const*)"tbo00", 5)) return -1;

    // done
    tb_object_bin_writer_t writer = {0};
    do
    {
        // init writer
        writer.stream           = stream;
        writer.ohash            = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_ptr(tb_null, tb_null), tb_item_func_uint32());
        writer.shash            = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_str(tb_true, tb_null), tb_item_func_uint32());
        tb_assert_and_check_break(writer.shash && writer.ohash);

        // writ
        if (!func(&writer, object)) break;

        // sync
        if (!tb_stream_sync(stream, tb_true)) break;

    } while (0);

    // exit the hash
    if (writer.ohash) tb_hash_exit(writer.ohash);
    if (writer.shash) tb_hash_exit(writer.shash);

    // exit the data
    if (writer.data) tb_free(writer.data);

    // the end offset
    tb_hize_t eof = tb_stream_offset(stream);

    // ok?
    return eof >= bof? (tb_long_t)(eof - bof) : -1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_writer_t* tb_object_bin_writer()
{
    // the writer
    static tb_object_writer_t s_writer = {0};
  
    // init writer
    s_writer.writ = tb_object_bin_writer_done;
 
    // init hooker
    s_writer.hooker = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
    tb_assert_and_check_return_val(s_writer.hooker, tb_null);

    // hook writer 
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NULL, tb_object_bin_writer_func_null);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATE, tb_object_bin_writer_func_date);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATA, tb_object_bin_writer_func_data);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_ARRAY, tb_object_bin_writer_func_array);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_STRING, tb_object_bin_writer_func_string);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NUMBER, tb_object_bin_writer_func_number);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_BOOLEAN, tb_object_bin_writer_func_boolean);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DICTIONARY, tb_object_bin_writer_func_dictionary);

    // ok
    return &s_writer;
}
tb_bool_t tb_object_bin_writer_hook(tb_size_t type, tb_object_bin_writer_func_t func)
{
    // check
    tb_assert_and_check_return_val(func, tb_false);
 
    // the writer
    tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_BIN);
    tb_assert_and_check_return_val(writer && writer->hooker, tb_false);

    // hook it
    tb_hash_set(writer->hooker, (tb_pointer_t)type, func);

    // ok
    return tb_true;
}
tb_object_bin_writer_func_t tb_object_bin_writer_func(tb_size_t type)
{
    // the writer
    tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_BIN);
    tb_assert_and_check_return_val(writer && writer->hooker, tb_null);

    // the func
    return (tb_object_bin_writer_func_t)tb_hash_get(writer->hooker, (tb_pointer_t)type);
}

