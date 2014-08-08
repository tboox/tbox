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
 * @file        xplist.c
 * @ingroup     object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_writer_xplist"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "xplist.h"
#include "../object.h"
#include "../../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_object_xplist_writer_func_date(tb_object_xplist_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // no empty?
    tb_time_t time = tb_object_date_time(object);
    if (time > 0)
    {
        // writ beg
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<date>") < 0) return tb_false;

        // writ date
        tb_tm_t date = {0};
        if (tb_localtime(time, &date))
        {
            if (tb_stream_printf(writer->stream,  "%04ld-%02ld-%02ldT%02ld:%02ld:%02ldZ"
                                ,   date.year
                                ,   date.month
                                ,   date.mday
                                ,   date.hour
                                ,   date.minute
                                ,   date.second) < 0) return tb_false;
        }
                    
        // writ end
        if (tb_stream_printf(writer->stream, "</date>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    else 
    {
        // writ
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<date/>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_object_xplist_writer_func_data(tb_object_xplist_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // no empty?
    if (tb_object_data_size(object))
    {
        // writ beg
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<data>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

        // decode base64 data
        tb_byte_t const*    ib = (tb_byte_t const*)tb_object_data_getp(object);
        tb_size_t           in = tb_object_data_size(object); 
        tb_size_t           on = in << 1;
        tb_char_t*          ob = tb_malloc0_cstr(on);
        tb_assert_and_check_return_val(ob && on, tb_false);
        on = tb_base64_encode(ib, in, ob, on);
        tb_trace_d("base64: %u => %u", in, on);

        // writ data
        tb_char_t const*    p = ob;
        tb_char_t const*    e = ob + on;
        tb_size_t           n = 0;
        for (; p < e && *p; p++, n++)
        {
            if (!(n % 68))
            {
                if (n) if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
                if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
            }
            if (tb_stream_printf(writer->stream, "%c", *p) < 0) return tb_false;
        }
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

        // free it
        tb_free(ob);
                    
        // writ end
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "</data>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    else 
    {
        // writ
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<data>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "</data>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_object_xplist_writer_func_array(tb_object_xplist_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // writ
    if (tb_object_array_size(object))
    {
        // writ beg
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<array>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

        // walk
        tb_for_all (tb_object_ref_t, item, tb_object_array_itor(object))
        {
            // item
            if (item)
            {
                // func
                tb_object_xplist_writer_func_t func = tb_object_xplist_writer_func(item->type);
                tb_assert_and_check_continue(func);

                // writ
                if (!func(writer, item, level + 1)) return tb_false;
            }
        }

        // writ end
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "</array>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    else 
    {
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<array/>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_object_xplist_writer_func_string(tb_object_xplist_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // writ
    if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
    if (tb_object_string_size(object))
    {
        if (tb_stream_printf(writer->stream, "<string>%s</string>", tb_object_string_cstr(object)) < 0) return tb_false;
    }
    else if (tb_stream_printf(writer->stream, "<string/>") < 0) return tb_false;
    if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

    // ok
    return tb_true;
}
static tb_bool_t tb_object_xplist_writer_func_number(tb_object_xplist_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // writ
    switch (tb_object_number_type(object))
    {
    case TB_NUMBER_TYPE_UINT64:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%llu</integer>", tb_object_number_uint64(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT64:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%lld</integer>", tb_object_number_sint64(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_UINT32:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%u</integer>", tb_object_number_uint32(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT32:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%d</integer>", tb_object_number_sint32(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_UINT16:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%u</integer>", tb_object_number_uint16(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT16:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%d</integer>", tb_object_number_sint16(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_UINT8:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%u</integer>", tb_object_number_uint8(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_SINT8:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<integer>%d</integer>", tb_object_number_sint8(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
#ifdef TB_CONFIG_TYPE_FLOAT
    case TB_NUMBER_TYPE_FLOAT:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<real>%f</real>", tb_object_number_float(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_NUMBER_TYPE_DOUBLE:
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<real>%lf</real>", tb_object_number_double(object)) < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
#endif
    default:
        break;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_object_xplist_writer_func_boolean(tb_object_xplist_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // writ
    if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
    if (tb_stream_printf(writer->stream, "<%s/>", tb_object_boolean_bool(object)? "true" : "false") < 0) return tb_false;
    if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

    // ok
    return tb_true;
}
static tb_bool_t tb_object_xplist_writer_func_dictionary(tb_object_xplist_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // writ
    if (tb_object_dictionary_size(object))
    {
        // writ beg
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<dict>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

        // walk
        tb_for_all (tb_object_dictionary_item_t*, item, tb_object_dictionary_itor(object))
        {
            // item
            if (item && item->key && item->val)
            {
                // func
                tb_object_xplist_writer_func_t func = tb_object_xplist_writer_func(item->val->type);
                tb_assert_and_check_continue(func);

                // writ key
                tb_object_writer_tab(writer->stream, writer->deflate, level + 1);
                if (tb_stream_printf(writer->stream, "<key>%s</key>", item->key) < 0) return tb_false;
                if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;

                // writ val
                if (!func(writer, item->val, level + 1)) return tb_false;
            }
        }

        // writ end
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "</dict>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    else 
    {
        if (!tb_object_writer_tab(writer->stream, writer->deflate, level)) return tb_false;
        if (tb_stream_printf(writer->stream, "<dict/>") < 0) return tb_false;
        if (!tb_object_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }

    // ok
    return tb_true;
}
static tb_long_t tb_object_xplist_writer_done(tb_stream_ref_t stream, tb_object_ref_t object, tb_bool_t deflate)
{
    // check
    tb_assert_and_check_return_val(object && stream, -1);
 
    // init writer 
    tb_object_xplist_writer_t writer = {0};
    writer.stream   = stream;
    writer.deflate  = deflate;

    // func
    tb_object_xplist_writer_func_t func = tb_object_xplist_writer_func(object->type);
    tb_assert_and_check_return_val(func, -1);

    // the begin offset
    tb_hize_t bof = tb_stream_offset(stream);

    // writ xplist header
    if (tb_stream_printf(stream, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>") < 0) return -1;
    if (!tb_object_writer_newline(stream, deflate)) return -1;
    if (tb_stream_printf(stream, "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">") < 0) return -1;
    if (!tb_object_writer_newline(stream, deflate)) return -1;
    if (tb_stream_printf(stream, "<plist version=\"1.0\">") < 0) return -1;
    if (!tb_object_writer_newline(stream, deflate)) return -1;

    // writ
    if (!func(&writer, object, 0)) return -1;

    // writ xplist end
    if (tb_stream_printf(stream, "</plist>") < 0) return -1;
    if (!tb_object_writer_newline(stream, deflate)) return -1;

    // sync
    if (!tb_stream_sync(stream, tb_true)) return -1;

    // the end offset
    tb_hize_t eof = tb_stream_offset(stream);

    // ok?
    return eof >= bof? (tb_long_t)(eof - bof) : -1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_writer_t* tb_object_xplist_writer()
{
    // the writer
    static tb_object_writer_t s_writer = {0};
  
    // init writer
    s_writer.writ = tb_object_xplist_writer_done;
 
    // init hooker
    s_writer.hooker = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_uint32(), tb_item_func_ptr(tb_null, tb_null));
    tb_assert_and_check_return_val(s_writer.hooker, tb_null);

    // hook writer 
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATE, tb_object_xplist_writer_func_date);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DATA, tb_object_xplist_writer_func_data);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_ARRAY, tb_object_xplist_writer_func_array);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_STRING, tb_object_xplist_writer_func_string);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NUMBER, tb_object_xplist_writer_func_number);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_BOOLEAN, tb_object_xplist_writer_func_boolean);
    tb_hash_set(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DICTIONARY, tb_object_xplist_writer_func_dictionary);

    // ok
    return &s_writer;
}
tb_bool_t tb_object_xplist_writer_hook(tb_size_t type, tb_object_xplist_writer_func_t func)
{
    // check
    tb_assert_and_check_return_val(func, tb_false);
 
    // the writer
    tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_XPLIST);
    tb_assert_and_check_return_val(writer && writer->hooker, tb_false);

    // hook it
    tb_hash_set(writer->hooker, (tb_pointer_t)type, func);

    // ok
    return tb_true;
}
tb_object_xplist_writer_func_t tb_object_xplist_writer_func(tb_size_t type)
{
    // the writer
    tb_object_writer_t* writer = tb_object_writer_get(TB_OBJECT_FORMAT_XPLIST);
    tb_assert_and_check_return_val(writer && writer->hooker, tb_null);

    // the func
    return (tb_object_xplist_writer_func_t)tb_hash_get(writer->hooker, (tb_pointer_t)type);
}

