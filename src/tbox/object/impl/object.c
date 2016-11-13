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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        object.c
 * @ingroup     object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "reader/reader.h"
#include "writer/writer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_object_init_env()
{
    // register reader
    if (!tb_oc_reader_set(TB_OBJECT_FORMAT_BIN, tb_oc_bin_reader())) return tb_false;
    if (!tb_oc_reader_set(TB_OBJECT_FORMAT_JSON, tb_oc_json_reader())) return tb_false;
    if (!tb_oc_reader_set(TB_OBJECT_FORMAT_BPLIST, tb_oc_bplist_reader())) return tb_false;
 
    // register writer
    if (!tb_oc_writer_set(TB_OBJECT_FORMAT_BIN, tb_oc_bin_writer())) return tb_false;
    if (!tb_oc_writer_set(TB_OBJECT_FORMAT_JSON, tb_oc_json_writer())) return tb_false;
    if (!tb_oc_writer_set(TB_OBJECT_FORMAT_BPLIST, tb_oc_bplist_writer())) return tb_false;

    // register reader and writer for xml
#ifdef TB_CONFIG_MODULE_HAVE_XML
    if (!tb_oc_reader_set(TB_OBJECT_FORMAT_XML, tb_oc_xml_reader())) return tb_false;
    if (!tb_oc_writer_set(TB_OBJECT_FORMAT_XML, tb_oc_xml_writer())) return tb_false;
    if (!tb_oc_reader_set(TB_OBJECT_FORMAT_XPLIST, tb_oc_xplist_reader())) return tb_false;
    if (!tb_oc_writer_set(TB_OBJECT_FORMAT_XPLIST, tb_oc_xplist_writer())) return tb_false;
#endif

    // ok
    return tb_true;
}
tb_void_t tb_object_exit_env()
{
    // remove reader
    tb_oc_reader_remove(TB_OBJECT_FORMAT_BIN);
    tb_oc_reader_remove(TB_OBJECT_FORMAT_JSON);
    tb_oc_reader_remove(TB_OBJECT_FORMAT_BPLIST);

    // remove writer
    tb_oc_writer_remove(TB_OBJECT_FORMAT_BIN);
    tb_oc_writer_remove(TB_OBJECT_FORMAT_JSON);
    tb_oc_writer_remove(TB_OBJECT_FORMAT_BPLIST);

    // remove reader and writer for xml
#ifdef TB_CONFIG_MODULE_HAVE_XML
    tb_oc_reader_remove(TB_OBJECT_FORMAT_XML);
    tb_oc_writer_remove(TB_OBJECT_FORMAT_XML);
    tb_oc_reader_remove(TB_OBJECT_FORMAT_XPLIST);
    tb_oc_writer_remove(TB_OBJECT_FORMAT_XPLIST);
#endif
}

