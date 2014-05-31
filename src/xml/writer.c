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
 * @file        writer.c
 * @ingroup     xml
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "xml"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "writer.h"
#include "../charset/charset.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
#   define TB_XML_WRITER_ELEMENTS_GROW      (32)
#else
#   define TB_XML_WRITER_ELEMENTS_GROW      (64)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the xml writer type
typedef struct __tb_xml_writer_t
{
    // stream
    tb_basic_stream_t*      wstream;

    // is format?
    tb_bool_t               bformat;
    
    // the block pool
    tb_handle_t             pool;

    // stack
    tb_stack_t*             elements;

    // attributes
    tb_handle_t             attributes;

}tb_xml_writer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_xml_writer_init(tb_basic_stream_t* wstream, tb_bool_t bformat)
{
    // check
    tb_assert_and_check_return_val(wstream, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_xml_writer_t*    writer = tb_null;
    do
    {
        // make writer
        writer = tb_malloc0(sizeof(tb_xml_writer_t));
        tb_assert_and_check_break(writer);

        // init writer
        writer->wstream     = wstream;
        writer->bformat     = bformat;

        // init pool
        writer->pool        = tb_block_pool_init(TB_BLOCK_POOL_GROW_SMALL, 0);
        tb_assert_and_check_break(writer->pool);
        
        // init elements
        writer->elements    = tb_stack_init(TB_XML_WRITER_ELEMENTS_GROW, tb_item_func_str(tb_false, writer->pool));
        tb_assert_and_check_break(writer->elements);

        // init attributes
        writer->attributes  = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, tb_item_func_str(tb_false, writer->pool), tb_item_func_str(tb_false, writer->pool));
        tb_assert_and_check_break(writer->attributes);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (writer) tb_xml_writer_exit(writer);
        writer = tb_null;
    }

    // ok?
    return writer;
}
tb_void_t tb_xml_writer_exit(tb_handle_t writer)
{
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    if (xwriter)
    {
        // exit attributes
        if (xwriter->attributes) tb_hash_exit(xwriter->attributes);
        xwriter->attributes = tb_null;

        // exit elements
        if (xwriter->elements) tb_stack_exit(xwriter->elements);
        xwriter->elements = tb_null;

        // exit pool
        if (xwriter->pool) tb_block_pool_exit(xwriter->pool);
        xwriter->pool = tb_null;

        // free it
        tb_free(xwriter);
    }
}
tb_void_t tb_xml_writer_save(tb_handle_t writer, tb_xml_node_t const* node)
{
    // check
    tb_assert_and_check_return(writer && node);

    // done
    switch (node->type)
    {
    case TB_XML_NODE_TYPE_DOCUMENT:
        {
            // document
            tb_xml_document_t* document = (tb_xml_document_t*)node;
            tb_xml_writer_document(writer, tb_scoped_string_cstr(&document->version), tb_scoped_string_cstr(&document->charset));

            // childs
            tb_xml_node_t* next = node->chead;
            while (next)
            {
                // save
                tb_xml_writer_save(writer, next);

                // next
                next = next->next;
            }
        }
        break;
    case TB_XML_NODE_TYPE_DOCUMENT_TYPE:
        {
            // document type
            tb_xml_document_type_t* doctype = (tb_xml_document_type_t*)node;
            tb_xml_writer_document_type(writer, tb_scoped_string_cstr(&doctype->type));
        }
        break;
    case TB_XML_NODE_TYPE_ELEMENT:
        {
            // attributes
            tb_xml_node_t* attr = node->ahead;
            while (attr)
            {
                // save
                tb_xml_writer_attributes_cstr(writer, tb_scoped_string_cstr(&attr->name), tb_scoped_string_cstr(&attr->data));

                // next
                attr = attr->next;
            }

            // childs
            tb_xml_node_t* next = node->chead;
            if (next)
            {
                // enter
                tb_xml_writer_element_enter(writer, tb_scoped_string_cstr(&node->name));

                // init
                while (next)
                {
                    // save
                    tb_xml_writer_save(writer, next);

                    // next
                    next = next->next;
                }

                // leave
                tb_xml_writer_element_leave(writer);
            }
            else tb_xml_writer_element_empty(writer, tb_scoped_string_cstr(&node->name));
        }
        break;
    case TB_XML_NODE_TYPE_COMMENT:
        tb_xml_writer_comment(writer, tb_scoped_string_cstr(&node->data));
        break;
    case TB_XML_NODE_TYPE_CDATA:
        tb_xml_writer_cdata(writer, tb_scoped_string_cstr(&node->data));
        break;
    case TB_XML_NODE_TYPE_TEXT:
        tb_xml_writer_text(writer, tb_scoped_string_cstr(&node->data));
        break;
    default:
        break;
    }
}
tb_void_t tb_xml_writer_document(tb_handle_t writer, tb_char_t const* version, tb_char_t const* charset)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream);

    tb_basic_stream_printf(xwriter->wstream, "<?xml version=\"%s\" encoding=\"%s\"?>", version? version : "2.0", charset? charset : "utf-8");
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");
}
tb_void_t tb_xml_writer_document_type(tb_handle_t writer, tb_char_t const* type)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream);

    tb_basic_stream_printf(xwriter->wstream, "<!DOCTYPE %s>", type? type : "");
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");
}
tb_void_t tb_xml_writer_cdata(tb_handle_t writer, tb_char_t const* data)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream && data);

    // writ tabs
    if (xwriter->bformat)
    {
        tb_size_t t = tb_stack_size(xwriter->elements);
        while (t--) tb_basic_stream_printf(xwriter->wstream, "\t");
    }

    tb_basic_stream_printf(xwriter->wstream, "<![CDATA[%s]]>", data);
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");
}
tb_void_t tb_xml_writer_text(tb_handle_t writer, tb_char_t const* text)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream && text);

    // writ tabs
    if (xwriter->bformat)
    {
        tb_size_t t = tb_stack_size(xwriter->elements);
        while (t--) tb_basic_stream_printf(xwriter->wstream, "\t");
    }

    tb_basic_stream_printf(xwriter->wstream, "%s", text);
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");
}
tb_void_t tb_xml_writer_comment(tb_handle_t writer, tb_char_t const* comment)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream && comment);

    // writ tabs
    if (xwriter->bformat)
    {
        tb_size_t t = tb_stack_size(xwriter->elements);
        while (t--) tb_basic_stream_printf(xwriter->wstream, "\t");
    }

    tb_basic_stream_printf(xwriter->wstream, "<!--%s-->", comment);
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");
}
tb_void_t tb_xml_writer_element_empty(tb_handle_t writer, tb_char_t const* name)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream && xwriter->attributes && name);

    // writ tabs
    if (xwriter->bformat)
    {
        tb_size_t t = tb_stack_size(xwriter->elements);
        while (t--) tb_basic_stream_printf(xwriter->wstream, "\t");
    }

    // writ name
    tb_basic_stream_printf(xwriter->wstream, "<%s", name);

    // writ attributes
    if (tb_hash_size(xwriter->attributes))
    {
        tb_for_all (tb_hash_item_t*, item, xwriter->attributes)
        {
            if (item && item->name && item->data)
                tb_basic_stream_printf(xwriter->wstream, " %s=\"%s\"", item->name, item->data);
        }
        tb_hash_clear(xwriter->attributes);
    }

    // writ end
    tb_basic_stream_printf(xwriter->wstream, "/>");
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");
}
tb_void_t tb_xml_writer_element_enter(tb_handle_t writer, tb_char_t const* name)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream && xwriter->elements && xwriter->attributes && name);

    // writ tabs
    if (xwriter->bformat)
    {
        tb_size_t t = tb_stack_size(xwriter->elements);
        while (t--) tb_basic_stream_printf(xwriter->wstream, "\t");
    }

    // writ name
    tb_basic_stream_printf(xwriter->wstream, "<%s", name);

    // writ attributes
    if (tb_hash_size(xwriter->attributes))
    {
        tb_for_all (tb_hash_item_t*, item, xwriter->attributes)
        {
            if (item && item->name && item->data)
                tb_basic_stream_printf(xwriter->wstream, " %s=\"%s\"", item->name, item->data);
        }
        tb_hash_clear(xwriter->attributes);
    }

    // writ end
    tb_basic_stream_printf(xwriter->wstream, ">");
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");

    // put name
    tb_stack_put(xwriter->elements, name);
}
tb_void_t tb_xml_writer_element_leave(tb_handle_t writer)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->wstream && xwriter->elements && xwriter->attributes);

    // writ tabs
    if (xwriter->bformat)
    {
        tb_size_t t = tb_stack_size(xwriter->elements);
        if (t) t--;
        while (t--) tb_basic_stream_printf(xwriter->wstream, "\t");
    }

    // writ name
    tb_char_t const* name = tb_stack_top(xwriter->elements);
    tb_assert_and_check_return(name);

    tb_basic_stream_printf(xwriter->wstream, "</%s>", name);
    if (xwriter->bformat) tb_basic_stream_printf(xwriter->wstream, "\n");

    // pop name
    tb_stack_pop(xwriter->elements);
}
tb_void_t tb_xml_writer_attributes_long(tb_handle_t writer, tb_char_t const* name, tb_long_t value)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->attributes && name);

    tb_char_t data[64] = {0};
    tb_snprintf(data, 64, "%ld", value);
    tb_hash_set(xwriter->attributes, name, data);
}
tb_void_t tb_xml_writer_attributes_bool(tb_handle_t writer, tb_char_t const* name, tb_bool_t value)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->attributes && name);

    tb_char_t data[64] = {0};
    tb_snprintf(data, 64, "%s", value? "true" : "false");
    tb_hash_set(xwriter->attributes, name, data);
}
tb_void_t tb_xml_writer_attributes_cstr(tb_handle_t writer, tb_char_t const* name, tb_char_t const* value)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->attributes && name && value);

    tb_hash_set(xwriter->attributes, name, value);
}
tb_void_t tb_xml_writer_attributes_format(tb_handle_t writer, tb_char_t const* name, tb_char_t const* format, ...)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->attributes && name && format);

    tb_size_t size = 0;
    tb_char_t data[8192] = {0};
    tb_vsnprintf_format(data, 8192, format, &size);
    tb_hash_set(xwriter->attributes, name, data);
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_void_t tb_xml_writer_attributes_float(tb_handle_t writer, tb_char_t const* name, tb_float_t value)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->attributes && name);

    tb_char_t data[64] = {0};
    tb_snprintf(data, 64, "%f", value);
    tb_hash_set(xwriter->attributes, name, data);
}
tb_void_t tb_xml_writer_attributes_double(tb_handle_t writer, tb_char_t const* name, tb_double_t value)
{
    // check
    tb_xml_writer_t* xwriter = (tb_xml_writer_t*)writer;
    tb_assert_and_check_return(xwriter && xwriter->attributes && name);

    tb_char_t data[64] = {0};
    tb_snprintf(data, 64, "%lf", value);
    tb_hash_set(xwriter->attributes, name, data);
}
#endif

