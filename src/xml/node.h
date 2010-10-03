/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
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
, 	TB_XML_NODE_TYPE_CDATA_SECTION 			= 4
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
	tb_char_t const* 			name;

	// the node value
	tb_char_t* 					value;

	// the reference to the document
	void* 						document;

	// the attributes
	struct __tb_xml_nlist_t* 	attributes;

	// the child nodes
	struct __tb_xml_nlist_t* 	childs;

	// the next & prev
	struct __tb_xml_node_t* 	prev;
	struct __tb_xml_node_t* 	next;

	// the parent node
	struct __tb_xml_node_t* 	parent;

}tb_xml_node_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
