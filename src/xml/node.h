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
 * \file		node.h
 *
 */
#ifndef TB_XML_NODE_H
#define TB_XML_NODE_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* the xml node type
 *
 * see http://www.w3.org/TR/REC-DOM-Level-1/
 *
 */
typedef enum __tb_xml_node_type_t
{
	TB_XML_NODE_TYPE_NULL 					= 0
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

// the xml node 
struct __tb_xml_nlist_t;
typedef struct __tb_xml_node_t
{
	// the node type
	tb_size_t 					type;

	// the node name
	tb_string_t 				name;

	// the node value
	tb_string_t 				value;

	// the reference to the document
	tb_pointer_t 						document;

	// the next & prev
	struct __tb_xml_node_t* 	prev;
	struct __tb_xml_node_t* 	next;

	// the parent node
	struct __tb_xml_node_t* 	parent;

	// the child nodes
	struct __tb_xml_nlist_t* 	childs;

	// the attributes
	struct __tb_xml_nlist_t* 	attributes;

	// the callback
	tb_void_t 						(*free)(struct __tb_xml_node_t* node);

}tb_xml_node_t;

// the xml element type
typedef struct __tb_xml_element_t
{
	// the node base
	tb_xml_node_t 			base;

}tb_xml_element_t;

// the xml text type
typedef struct __tb_xml_text_t
{
	// the node base
	tb_xml_node_t 			base;

}tb_xml_text_t;

// the xml cdata type
typedef struct __tb_xml_cdata_t
{
	// the node base
	tb_xml_node_t 			base;

}tb_xml_cdata_t;

// the xml comment type
typedef struct __tb_xml_comment_t
{
	// the node base
	tb_xml_node_t 			base;

}tb_xml_comment_t;

/* the xml attribute type
 *
 * inherit node, 
 * but since they are not actually child nodes of the element they describe, 
 * the DOM does not consider them part of the document tree.
 */
typedef struct __tb_xml_attribute_t
{
	// the node base
	tb_xml_node_t 			base;

}tb_xml_attribute_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & uninit
tb_void_t 			tb_xml_node_init(tb_xml_node_t* node, tb_pointer_t document, tb_size_t type);
tb_void_t 			tb_xml_node_uninit(tb_xml_node_t* node);

// create & destroy
tb_xml_node_t* 	tb_xml_node_create(tb_pointer_t document, tb_size_t type);
tb_void_t 			tb_xml_node_destroy(tb_xml_node_t* node);

// childs
tb_void_t 			tb_xml_node_childs_append(tb_xml_node_t* node, tb_xml_node_t* child);
tb_void_t 			tb_xml_node_childs_remove(tb_xml_node_t* node, tb_xml_node_t* child);
tb_xml_node_t* 	tb_xml_node_childs_head(tb_xml_node_t* node);
tb_xml_node_t* 	tb_xml_node_childs_tail(tb_xml_node_t* node);
tb_xml_node_t* 	tb_xml_node_childs_select(tb_xml_node_t* node, tb_char_t const* path);

// nodes
tb_xml_node_t* 	tb_xml_node_add_element(tb_xml_node_t* node, tb_char_t const* name);
tb_xml_node_t* 	tb_xml_node_add_text(tb_xml_node_t* node, tb_char_t const* data);
tb_xml_node_t* 	tb_xml_node_add_cdata(tb_xml_node_t* node, tb_char_t const* data);
tb_xml_node_t* 	tb_xml_node_add_comment(tb_xml_node_t* node, tb_char_t const* data);
tb_xml_node_t* 	tb_xml_node_add_attribute(tb_xml_node_t* node, tb_char_t const* name);

// attributes
tb_void_t 			tb_xml_node_attributes_clear(tb_xml_node_t* node);
tb_xml_node_t* 	tb_xml_node_attributes_add_string(tb_xml_node_t* node, tb_char_t const* name, tb_string_t const* value);
tb_xml_node_t* 	tb_xml_node_attributes_add_c_string(tb_xml_node_t* node, tb_char_t const* name, tb_char_t const* value);
tb_xml_node_t* 	tb_xml_node_attributes_add_int(tb_xml_node_t* node, tb_char_t const* name, tb_int_t value);
tb_xml_node_t* 	tb_xml_node_attributes_add_bool(tb_xml_node_t* node, tb_char_t const* name, tb_bool_t value);
tb_xml_node_t* 	tb_xml_node_attributes_add_format(tb_xml_node_t* node, tb_char_t const* name, tb_char_t const* fmt, ...);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_xml_node_t* 	tb_xml_node_attributes_add_float(tb_xml_node_t* node, tb_char_t const* name, tb_float_t value);
#endif


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
