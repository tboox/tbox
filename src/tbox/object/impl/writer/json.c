/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        json.c
 * @ingroup     object
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "oc_writer_json"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "json.h"
#include "writer.h"
#include "../../../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_char_t const* tb_oc_json_escape_cstr(tb_char_t* data, tb_size_t maxn, tb_object_ref_t object)
{
    tb_char_t const* sp = tb_oc_string_cstr(object);
    tb_char_t const* se = sp + tb_oc_string_size(object);
    tb_char_t*       dp = data;
    tb_char_t const* de = data + maxn;
    tb_char_t        ch;
    while (sp < se && dp < de)
    {
        ch = *sp++;
        if (dp + 1 < de)
        {
            if (ch == '\n')
            {
                *dp++ = '\\';
                *dp++ = 'n';
            }
            else if (ch == '\t')
            {
                *dp++ = '\\';
                *dp++ = 't';
            }
            else if (ch == '"')
            {
                *dp++ = '\\';
                *dp++ = '"';
            }
            else if (ch == '\\')
            {
                *dp++ = '\\';
                *dp++ = '\\';
            }
            else
            {
                *dp++ = ch;
                continue;
            }
        }
        else *dp++ = ch;
    }
    if (dp < de) *dp = '\0';
    return data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_oc_json_writer_func_null(tb_oc_json_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // write
    if (tb_stream_printf(writer->stream, "null") < 0) return tb_false;
    if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
    return tb_true;
}
static tb_bool_t tb_oc_json_writer_func_array(tb_oc_json_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // write array
    if (tb_oc_array_size(object))
    {
        // write begin
        if (tb_stream_printf(writer->stream, "[") < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;

        // walk
        tb_for_all (tb_object_ref_t, item, tb_oc_array_itor(object))
        {
            // item
            if (item)
            {
                // func
                tb_oc_json_writer_func_t func = tb_oc_json_writer_func(item->type);
                tb_assert_and_check_continue(func);

                // write tab
                if (item_itor != item_head)
                {
                    if (!tb_oc_writer_spaces(writer->stream, writer->deflate, level, 2)) return tb_false;
                    if (tb_stream_printf(writer->stream, ",") < 0) return tb_false;
                    if (!tb_oc_writer_spaces(writer->stream, writer->deflate, 1, 1)) return tb_false;
                }
                else if (!tb_oc_writer_spaces(writer->stream, writer->deflate, level + 1, 2)) return tb_false;

                // write
                if (!func(writer, item, level + 1)) return tb_false;
            }
        }

        // write end
        if (!tb_oc_writer_spaces(writer->stream, writer->deflate, level, 2)) return tb_false;
        if (tb_stream_printf(writer->stream, "]") < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    else
    {
        if (tb_stream_printf(writer->stream, "[]") < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    return tb_true;
}
static tb_bool_t tb_oc_json_writer_func_string(tb_oc_json_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    tb_size_t size = tb_oc_string_size(object);
    if (size)
    {
        tb_size_t maxn = (size * 3 / 2) + 16;
        if (maxn > 8192)
        {
            tb_char_t* data = (tb_char_t*)tb_malloc(maxn);
            tb_char_t const* cstr = data? tb_oc_json_escape_cstr(data, maxn, object) : tb_null;
            if (cstr && tb_stream_printf(writer->stream, "\"%s\"", cstr) < 0) return tb_false;
            tb_free(data);
        }
        else
        {
            tb_char_t data[8192];
            tb_char_t const* cstr = tb_oc_json_escape_cstr(data, maxn, object);
            if (cstr && tb_stream_printf(writer->stream, "\"%s\"", cstr) < 0) return tb_false;
        }
    }
    else if (tb_stream_printf(writer->stream, "\"\"") < 0) return tb_false;
    if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
    return tb_true;
}
static tb_bool_t tb_oc_json_writer_func_number(tb_oc_json_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // write
    switch (tb_oc_number_type(object))
    {
    case TB_OC_NUMBER_TYPE_UINT64:
        if (tb_stream_printf(writer->stream, "%llu", tb_oc_number_uint64(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_SINT64:
        if (tb_stream_printf(writer->stream, "%lld", tb_oc_number_sint64(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_UINT32:
        if (tb_stream_printf(writer->stream, "%u", tb_oc_number_uint32(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_SINT32:
        if (tb_stream_printf(writer->stream, "%d", tb_oc_number_sint32(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_UINT16:
        if (tb_stream_printf(writer->stream, "%u", tb_oc_number_uint16(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_SINT16:
        if (tb_stream_printf(writer->stream, "%d", tb_oc_number_sint16(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_UINT8:
        if (tb_stream_printf(writer->stream, "%u", tb_oc_number_uint8(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_SINT8:
        if (tb_stream_printf(writer->stream, "%d", tb_oc_number_sint8(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
    case TB_OC_NUMBER_TYPE_FLOAT:
        if (tb_stream_printf(writer->stream, "%f", tb_oc_number_float(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
    case TB_OC_NUMBER_TYPE_DOUBLE:
        if (tb_stream_printf(writer->stream, "%lf", tb_oc_number_double(object)) < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
        break;
#endif
    default:
        break;
    }
    return tb_true;
}
static tb_bool_t tb_oc_json_writer_func_boolean(tb_oc_json_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // write
    if (tb_stream_printf(writer->stream, "%s", tb_oc_boolean_bool(object)? "true" : "false") < 0) return tb_false;
    if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
    return tb_true;
}
static tb_bool_t tb_oc_json_writer_func_dictionary(tb_oc_json_writer_t* writer, tb_object_ref_t object, tb_size_t level)
{
    // check
    tb_assert_and_check_return_val(writer && writer->stream, tb_false);

    // write
    if (tb_oc_dictionary_size(object))
    {
        // write beg
        if (tb_stream_printf(writer->stream, "{") < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;

        // walk
        tb_for_all (tb_oc_dictionary_item_t*, item, tb_oc_dictionary_itor(object))
        {
            // item
            if (item && item->key && item->val)
            {
                // func
                tb_oc_json_writer_func_t func = tb_oc_json_writer_func(item->val->type);
                tb_assert_and_check_continue(func);

                // write tab
                if (item_itor != item_head)
                {
                    if (!tb_oc_writer_spaces(writer->stream, writer->deflate, level, 2)) return tb_false;
                    if (tb_stream_printf(writer->stream, ",") < 0) return tb_false;
                    if (!tb_oc_writer_spaces(writer->stream, writer->deflate, 1, 1)) return tb_false;
                }
                else if (!tb_oc_writer_spaces(writer->stream, writer->deflate, level + 1, 2)) return tb_false;

                // write key
                if (tb_stream_printf(writer->stream, "\"%s\":", item->key) < 0) return tb_false;

                // write spaces
                if (!writer->deflate) if (tb_stream_printf(writer->stream, " ") < 0) return tb_false;
                if (item->val->type == TB_OBJECT_TYPE_DICTIONARY || item->val->type == TB_OBJECT_TYPE_ARRAY)
                {
                    if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
                    if (!tb_oc_writer_spaces(writer->stream, writer->deflate, level + 1, 2)) return tb_false;
                }

                // write value
                if (!func(writer, item->val, level + 1)) return tb_false;
            }
        }

        // write end
        if (!tb_oc_writer_spaces(writer->stream, writer->deflate, level, 2)) return tb_false;
        if (tb_stream_printf(writer->stream, "}") < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    else
    {
        if (tb_stream_printf(writer->stream, "{}") < 0) return tb_false;
        if (!tb_oc_writer_newline(writer->stream, writer->deflate)) return tb_false;
    }
    return tb_true;
}
static tb_long_t tb_oc_json_writer_done(tb_stream_ref_t stream, tb_object_ref_t object, tb_bool_t deflate)
{
    // check
    tb_assert_and_check_return_val(object && stream, -1);

    // init writer
    tb_oc_json_writer_t writer = {0};
    writer.stream   = stream;
    writer.deflate  = deflate;

    // func
    tb_oc_json_writer_func_t func = tb_oc_json_writer_func(object->type);
    tb_assert_and_check_return_val(func, tb_false);

    // the begin offset
    tb_hize_t bof = tb_stream_offset(stream);

    // write
    if (!func(&writer, object, 0)) return -1;

    // sync
    if (!tb_stream_sync(stream, tb_true)) return -1;

    // the end offset
    tb_hize_t eof = tb_stream_offset(stream);
    return eof >= bof? (tb_long_t)(eof - bof) : -1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_oc_writer_t* tb_oc_json_writer()
{
    // the writer
    static tb_oc_writer_t s_writer = {0};

    // init writer
    s_writer.writ = tb_oc_json_writer_done;

    // init hooker
    s_writer.hooker = tb_hash_map_init(TB_HASH_MAP_BUCKET_SIZE_MICRO, tb_element_uint32(), tb_element_ptr(tb_null, tb_null));
    tb_assert_and_check_return_val(s_writer.hooker, tb_null);

    // hook writer
    tb_hash_map_insert(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NULL, tb_oc_json_writer_func_null);
    tb_hash_map_insert(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_ARRAY, tb_oc_json_writer_func_array);
    tb_hash_map_insert(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_STRING, tb_oc_json_writer_func_string);
    tb_hash_map_insert(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_NUMBER, tb_oc_json_writer_func_number);
    tb_hash_map_insert(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_BOOLEAN, tb_oc_json_writer_func_boolean);
    tb_hash_map_insert(s_writer.hooker, (tb_pointer_t)TB_OBJECT_TYPE_DICTIONARY, tb_oc_json_writer_func_dictionary);
    return &s_writer;
}
tb_bool_t tb_oc_json_writer_hook(tb_size_t type, tb_oc_json_writer_func_t func)
{
    tb_assert_and_check_return_val(func, tb_false);

    tb_oc_writer_t* writer = tb_oc_writer_get(TB_OBJECT_FORMAT_JSON);
    tb_assert_and_check_return_val(writer && writer->hooker, tb_false);

    tb_hash_map_insert(writer->hooker, (tb_pointer_t)type, func);
    return tb_true;
}
tb_oc_json_writer_func_t tb_oc_json_writer_func(tb_size_t type)
{
    tb_oc_writer_t* writer = tb_oc_writer_get(TB_OBJECT_FORMAT_JSON);
    tb_assert_and_check_return_val(writer && writer->hooker, tb_null);

    return (tb_oc_json_writer_func_t)tb_hash_map_get(writer->hooker, (tb_pointer_t)type);
}

