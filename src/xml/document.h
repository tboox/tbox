/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		document.h
 *
 */
#ifndef TB_XML_DOCUMENT_H
#define TB_XML_DOCUMENT_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "node.h"
#include "reader.h"
#include "writer.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the xml document type 
typedef struct __tb_xml_document_type_t
{
	// the node base
	tb_xml_node_t 				base;

	// the entities
	struct __tb_xml_nlist_t* 	entities;
	
	// the notations
	struct __tb_xml_nlist_t* 	notations;

}tb_xml_document_type_t;

// the xml document - DOM 
typedef struct __tb_xml_document_t
{
	// the node base
	tb_xml_node_t 				base;

	// the version
	tb_pstring_t 				version;

	// the encoding 
	tb_pstring_t 				encoding;


}tb_xml_document_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_xml_document_t* 	tb_xml_document_init();
tb_void_t 			tb_xml_document_exit(tb_xml_document_t* document);

// load & store
tb_bool_t 			tb_xml_document_load(tb_xml_document_t* document, tb_gstream_t* gst);
tb_bool_t 			tb_xml_document_store(tb_xml_document_t* document, tb_gstream_t* gst);

// modifiors
tb_void_t 			tb_xml_document_clear(tb_xml_document_t* document);

// xml header
tb_pstring_t* 		tb_xml_document_version(tb_xml_document_t* document);
tb_pstring_t* 		tb_xml_document_encoding(tb_xml_document_t* document);

// nodes
tb_xml_node_t* 		tb_xml_document_init_element(tb_xml_document_t* document, tb_char_t const* name);
tb_xml_node_t* 		tb_xml_document_init_text(tb_xml_document_t* document, tb_char_t const* data);
tb_xml_node_t* 		tb_xml_document_init_cdata(tb_xml_document_t* document, tb_char_t const* data);
tb_xml_node_t* 		tb_xml_document_init_comment(tb_xml_document_t* document, tb_char_t const* data);
tb_xml_node_t* 		tb_xml_document_init_attribute(tb_xml_document_t* document, tb_char_t const* name);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
