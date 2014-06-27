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
#define TB_TRACE_MODULE_NAME        "object_reader_bin"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bin.h"
#include "../object.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the array grow
#ifdef __tb_small__
#   define TB_OBJECT_BIN_READER_ARRAY_GROW          (64)
#else
#   define TB_OBJECT_BIN_READER_ARRAY_GROW          (256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_object_ref_t tb_object_bin_reader_func_null(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // ok
    return tb_object_null_init();
}
static tb_object_ref_t tb_object_bin_reader_func_date(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // ok
    return tb_object_date_init_from_time((tb_time_t)size);
}
static tb_object_ref_t tb_object_bin_reader_func_data(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // empty?
    if (!size) return tb_object_data_init_from_data(tb_null, 0);

    // make data
    tb_char_t* data = tb_malloc0_cstr((tb_size_t)size);
    tb_assert_and_check_return_val(data, tb_null);

    // read data
    if (!tb_stream_bread(reader->stream, (tb_byte_t*)data, (tb_size_t)size)) 
    {
        tb_free(data);
        return tb_null;
    }

    // decode data
    {
        tb_byte_t*  pb = (tb_byte_t*)data;
        tb_byte_t*  pe = (tb_byte_t*)data + size;
        tb_byte_t   xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
        for (; pb < pe; pb++, xb++) *pb ^= xb;
    }

    // make the data object
    tb_object_ref_t object = tb_object_data_init_from_data(data, (tb_size_t)size); 

    // exit data
    tb_free(data);

    // ok?
    return object;
}
static tb_object_ref_t tb_object_bin_reader_func_array(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // empty?
    if (!size) return tb_object_array_init(TB_OBJECT_BIN_READER_ARRAY_GROW, tb_false);

    // init array
    tb_object_ref_t array = tb_object_array_init(TB_OBJECT_BIN_READER_ARRAY_GROW, tb_false);
    tb_assert_and_check_return_val(array, tb_null);

    // walk
    tb_size_t i = 0;
    tb_size_t n = (tb_size_t)size;
    for (i = 0; i < n; i++)
    {
        // the type & size
        tb_size_t               type = 0;
        tb_uint64_t             size = 0;
        tb_object_reader_bin_type_size(reader->stream, &type, &size);

        // trace
        tb_trace_d("item: type: %lu, size: %llu", type, size);

        // is index?
        tb_object_ref_t item = tb_null;
        if (!type)
        {
            // the object index
            tb_size_t index = (tb_size_t)size;
        
            // check
            tb_assert_and_check_break(index < tb_vector_size(reader->list));

            // the item
            item = (tb_object_ref_t)tb_iterator_item(reader->list, index);

            // refn++
            if (item) tb_object_inc(item);
        }
        else
        {
            // the reader func
            tb_object_bin_reader_func_t func = tb_object_bin_reader_func(type);
            tb_assert_and_check_break(func);

            // read it
            item = func(reader, type, size);

            // save it
            tb_vector_insert_tail(reader->list, item);
        }

        // check
        tb_assert_and_check_break(item);

        // append item
        tb_object_array_append(array, item);
    }

    // failed?
    if (i != n)
    {
        if (array) tb_object_exit(array);
        array = tb_null;
    }

    // ok?
    return array;
}
static tb_object_ref_t tb_object_bin_reader_func_string(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // empty?
    if (!size) return tb_object_string_init_from_cstr(tb_null);

    // make data
    tb_char_t* data = tb_malloc0_cstr((tb_size_t)size + 1);
    tb_assert_and_check_return_val(data, tb_null);

    // read data
    if (!tb_stream_bread(reader->stream, (tb_byte_t*)data, (tb_size_t)size)) 
    {
        tb_free(data);
        return tb_null;
    }

    // decode string
    {
        tb_byte_t*  pb = (tb_byte_t*)data;
        tb_byte_t*  pe = (tb_byte_t*)data + size;
        tb_byte_t   xb = (tb_byte_t)(((size >> 8) & 0xff) | (size & 0xff));
        for (; pb < pe; pb++, xb++) *pb ^= xb;
    }

    // make string
    tb_object_ref_t string = tb_object_string_init_from_cstr(data); 

    // exit data
    tb_free(data);

    // ok?
    return string;
}
static tb_object_ref_t tb_object_bin_reader_func_number(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // the number type
    tb_size_t number_type = (tb_size_t)size;

    // read number
    tb_object_ref_t number = tb_null;
    switch (number_type)
    {
    case TB_NUMBER_TYPE_UINT64:
        number = tb_object_number_init_from_uint64(tb_stream_bread_u64_be(reader->stream));
        break;
    case TB_NUMBER_TYPE_SINT64:
        number = tb_object_number_init_from_sint64(tb_stream_bread_s64_be(reader->stream));
        break;
    case TB_NUMBER_TYPE_UINT32:
        number = tb_object_number_init_from_uint32(tb_stream_bread_u32_be(reader->stream));
        break;
    case TB_NUMBER_TYPE_SINT32:
        number = tb_object_number_init_from_sint32(tb_stream_bread_s32_be(reader->stream));
        break;
    case TB_NUMBER_TYPE_UINT16:
        number = tb_object_number_init_from_uint16(tb_stream_bread_u16_be(reader->stream));
        break;
    case TB_NUMBER_TYPE_SINT16:
        number = tb_object_number_init_from_sint16(tb_stream_bread_s16_be(reader->stream));
        break;
    case TB_NUMBER_TYPE_UINT8:
        number = tb_object_number_init_from_uint8(tb_stream_bread_u8(reader->stream));
        break;
    case TB_NUMBER_TYPE_SINT8:
        number = tb_object_number_init_from_sint8(tb_stream_bread_s8(reader->stream));
        break;
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_NUMBER_TYPE_FLOAT:
        {
            tb_byte_t data[4] = {0};
            if (!tb_stream_bread(reader->stream, data, 4)) return tb_null;
            number = tb_object_number_init_from_float(tb_bits_get_float_be(data));
        }
        break;
    case TB_NUMBER_TYPE_DOUBLE:
        {
            tb_byte_t data[8] = {0};
            if (!tb_stream_bread(reader->stream, data, 8)) return tb_null;
            number = tb_object_number_init_from_double(tb_bits_get_double_bbe(data));
        }
        break;
#endif
    default:
        tb_assert_and_check_return_val(0, tb_null);
        break;
    }

    // ok?
    return number;
}
static tb_object_ref_t tb_object_bin_reader_func_boolean(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // ok?
    return tb_object_boolean_init(size? tb_true : tb_false);
}
static tb_object_ref_t tb_object_bin_reader_func_dictionary(tb_object_bin_reader_t* reader, tb_size_t type, tb_uint64_t size)
{
    // check
    tb_assert_and_check_return_val(reader && reader->stream && reader->list, tb_null);

    // empty?
    if (!size) return tb_object_dictionary_init(TB_OBJECT_DICTIONARY_SIZE_MICRO, tb_false);

    // init dictionary
    tb_object_ref_t dictionary = tb_object_dictionary_init(0, tb_false);
    tb_assert_and_check_return_val(dictionary, tb_null);

    // walk
    tb_size_t       i = 0;
    tb_size_t       n = (tb_size_t)size;
    for (i = 0; i < n; i++)
    {
        // read key
        tb_object_ref_t key = tb_null;
        do
        {
            // the type & size
            tb_size_t               type = 0;
            tb_uint64_t             size = 0;
            tb_object_reader_bin_type_size(reader->stream, &type, &size);

            // trace
            tb_trace_d("key: type: %lu, size: %llu", type, size);

            // is index?
            if (!type)
            {
                // the object index
                tb_size_t index = (tb_size_t)size;
            
                // check
                tb_assert_and_check_break(index < tb_vector_size(reader->list));

                // the item
                key = (tb_object_ref_t)tb_iterator_item(reader->list, index);
            }
            else
            {
                // check
                tb_assert_and_check_break(type == TB_OBJECT_TYPE_STRING);

                // the reader func
                tb_object_bin_reader_func_t func = tb_object_bin_reader_func(type);
                tb_assert_and_check_break(func);

                // read it
                key = func(reader, type, size);
                tb_assert_and_check_break(key);

                // save it
                tb_vector_insert_tail(reader->list, key);

                // refn--
                tb_object_dec(key);
            }

        } while (0);

        // check
        tb_assert_and_check_break(key && tb_object_type(key) == TB_OBJECT_TYPE_STRING);
        tb_assert_and_check_break(tb_object_string_size(key) && tb_object_string_cstr(key));
        
        // read val
        tb_object_ref_t val = tb_null;
        do
        {
            // the type & size
            tb_size_t               type = 0;
            tb_uint64_t             size = 0;
            tb_object_reader_bin_type_size(reader->stream, &type, &size);

            // trace
            tb_trace_d("val: type: %lu, size: %llu", type, size);

            // is index?
            if (!type)
            {
                // the object index
                tb_size_t index = (tb_size_t)size;
            
                // check
                tb_assert_and_check_break(index < tb_vector_size(reader->list));

                // the item
                val = (tb_object_ref_t)tb_iterator_item(reader->list, index);

                // refn++
                if (val) tb_object_inc(val);
            }
            else
            {
                // the reader func
                tb_object_bin_reader_func_t func = tb_object_bin_reader_func(type);
                tb_assert_and_check_break(func);

                // read it
                val = func(reader, type, size);

                // save it
                if (val) tb_vector_insert_tail(reader->list, val);
            }
        
        } while (0);

        // check
        tb_assert_and_check_break(val);

        // set key => val
        tb_object_dictionary_set(dictionary, tb_object_string_cstr(key), val);
    }

    // failed?
    if (i != n)
    {
        if (dictionary) tb_object_exit(dictionary);
        dictionary = tb_null;
    }

    // ok?
    return dictionary;
}
static tb_object_ref_t tb_object_bin_reader_done(tb_stream_ref_t stream)
{
    // read bin header
    tb_byte_t data[32] = {0};
    if (!tb_stream_bread(stream, data, 5)) return tb_null;

    // check 
    if (tb_strnicmp((tb_char_t const*)data, "tbo00", 5)) return tb_null;

    // init
    tb_object_ref_t            object = tb_null;
    tb_object_bin_reader_t  reader = {0};

    // init reader
    reader.stream           = stream;
    reader.list             = tb_vector_init(256, tb_item_func_obj());
    tb_assert_and_check_return_val(reader.list, tb_null);

    // the type & size
    tb_size_t               type = 0;
    tb_uint64_t             size = 0;
    tb_object_reader_bin_type_size(stream, &type, &size);

    // trace
    tb_trace_d("root: type: %lu, size: %llu", type, size);

    // the func
    tb_object_bin_reader_func_t func = tb_object_bin_reader_func(type);
    tb_assert_and_check_goto(func, end);

    // read it
    object = func(&reader, type, size);

end:

    // exit the list
    if (reader.list) tb_vector_exit(reader.list);

    // ok?
    return object;
}
static tb_size_t tb_object_bin_reader_probe(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, 0);

    // need it
    tb_byte_t* p = tb_null;
    if (!tb_stream_need(stream, &p, 3)) return 0;
    tb_assert_and_check_return_val(p, 0);

    // ok?
    return !tb_strnicmp((tb_char_t const*)p, "tbo", 3)? 80 : 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_reader_t* tb_object_bin_reader()
{
    // the reader
    static tb_object_reader_t s_reader = {0};

    // init reader
    s_reader.read   = tb_object_bin_reader_done;
    s_reader.probe  = tb_object_bin_reader_probe;

    // init hooker
    s_reader.hooker = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
    tb_assert_and_check_return_val(s_reader.hooker, tb_null);

    // hook reader 
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NULL, tb_object_bin_reader_func_null);
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATE, tb_object_bin_reader_func_date);
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATA, tb_object_bin_reader_func_data);
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_ARRAY, tb_object_bin_reader_func_array);
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_STRING, tb_object_bin_reader_func_string);
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NUMBER, tb_object_bin_reader_func_number);
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_BOOLEAN, tb_object_bin_reader_func_boolean);
    tb_hash_set(s_reader.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DICTIONARY, tb_object_bin_reader_func_dictionary);

    // ok
    return &s_reader;
}
tb_bool_t tb_object_bin_reader_hook(tb_size_t type, tb_object_bin_reader_func_t func)
{
    // check
    tb_assert_and_check_return_val(type && func, tb_false);

    // the reader
    tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_BIN);
    tb_assert_and_check_return_val(reader && reader->hooker, tb_false);

    // hook it
    tb_hash_set(reader->hooker, (tb_pointer_t)type, func);

    // ok
    return tb_true;
}
tb_object_bin_reader_func_t tb_object_bin_reader_func(tb_size_t type)
{
    // check
    tb_assert_and_check_return_val(type, tb_null);

    // the reader
    tb_object_reader_t* reader = tb_object_reader_get(TB_OBJECT_FORMAT_BIN);
    tb_assert_and_check_return_val(reader && reader->hooker, tb_null);

    // the func
    return (tb_object_bin_reader_func_t)tb_hash_get(reader->hooker, (tb_pointer_t)type);
}

