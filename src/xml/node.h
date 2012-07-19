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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		node.h
 * @ingroup 	xml
 *
 */
#ifndef TB_XML_NODE_H
#define TB_XML_NODE_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*!the xml node type
 *
 * @note see http://www.w3.org/TR/REC-DOM-Level-1/
 *
 */
typedef enum __tb_xml_node_type_t
{
	TB_XML_NODE_TYPE_NONE 					= 0
,	TB_XML_NODE_TYPE_ELEMENT 				= 1
, 	TB_XML_NODE_TYPE_ATTRIBUTE 				= 2
, 	TB_XML_NODE_TYPE_TEXT 					= 3
, 	TB_XML_NODE_TYPE_CDATA 					= 4
, 	TB_XML_NODE_TYPE_ENTITY_REFERENCE 		= 5
, 	TB_XML_NODE_TYPE_ENTITY 				= 6
, 	TB_XML_NODE_TYPE_PROCESSING_INSTRUCTION	= 7
, 	TB_XML_NODE_TYPE_COMMENT				= 8
, 	TB_XML_NODE_TYPE_DOCUMENT				= 9
, 	TB_XML_NODE_TYPE_DOCUMENT_TYPE			= 10
, 	TB_XML_NODE_TYPE_DOCUMENT_FRAGMENT 		= 11
, 	TB_XML_NODE_TYPE_NOTATION				= 12

}tb_xml_node_type_t;

/// the xml node 
typedef struct __tb_xml_node_t
{
	// the node type
	tb_size_t 					type;

	// the node name
	tb_pstring_t 				name;

	// the node data
	tb_pstring_t 				data;

	// the next
	struct __tb_xml_node_t* 	next;

	// the childs
	struct __tb_xml_node_t* 	chead;
	struct __tb_xml_node_t* 	ctail;

	// the attributes
	struct __tb_xml_node_t* 	ahead;
	struct __tb_xml_node_t* 	atail;

	// the parent
	struct __tb_xml_node_t* 	parent;

}tb_xml_node_t;

/// the xml element type
typedef struct __tb_xml_element_t
{
	// the node base
	tb_xml_node_t 				base;

}tb_xml_element_t;

/// the xml text type
typedef struct __tb_xml_text_t
{
	// the node base
	tb_xml_node_t 				base;

}tb_xml_text_t;

/// the xml cdata type
typedef struct __tb_xml_cdata_t
{
	// the node base
	tb_xml_node_t 				base;

}tb_xml_cdata_t;

/// the xml comment type
typedef struct __tb_xml_comment_t
{
	// the node base
	tb_xml_node_t 				base;

}tb_xml_comment_t;

/*!the xml attribute type
 *
 * <pre>
 * inherit node, 
 * but since they are not actually child nodes of the element they describe, 
 * the DOM does not consider them part of the document tree.
 * </pre>
 */
typedef struct __tb_xml_attribute_t
{
	// the node base
	tb_xml_node_t 				base;

}tb_xml_attribute_t;

/// the xml document type 
typedef struct __tb_xml_document_t
{
	// the node base
	tb_xml_node_t 				base;

	// the version
	tb_pstring_t 				version;

	// the encoding 
	tb_pstring_t 				encoding;

}tb_xml_document_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// init element 
tb_xml_node_t* 		tb_xml_node_init_element(tb_char_t const* name);

/// init text 
tb_xml_node_t* 		tb_xml_node_init_text(tb_char_t const* data);

/// init cdata 
tb_xml_node_t* 		tb_xml_node_init_cdata(tb_char_t const* cdata);

/// init comment 
tb_xml_node_t* 		tb_xml_node_init_comment(tb_char_t const* comment);

/// init attribute 
tb_xml_node_t* 		tb_xml_node_init_attribute(tb_char_t const* name, tb_char_t const* data);

/// init document 
tb_xml_node_t* 		tb_xml_node_init_document(tb_char_t const* version, tb_char_t const* encoding);

/// exit node 
tb_void_t 			tb_xml_node_exit(tb_xml_node_t* node);

/// insert the next node
tb_void_t 			tb_xml_node_insert_next(tb_xml_node_t* node, tb_xml_node_t* next);

/// remove the next node
tb_void_t 			tb_xml_node_remove_next(tb_xml_node_t* node);

/// append the node to the chead
tb_void_t 			tb_xml_node_append_chead(tb_xml_node_t* node, tb_xml_node_t* child);

/// append the node to the ctail
tb_void_t 			tb_xml_node_append_ctail(tb_xml_node_t* node, tb_xml_node_t* child);

/// append the node to the ahead
tb_void_t 			tb_xml_node_append_ahead(tb_xml_node_t* node, tb_xml_node_t* attribute);

/// append the node to the atail
tb_void_t 			tb_xml_node_append_atail(tb_xml_node_t* node, tb_xml_node_t* attribute);


#endif
