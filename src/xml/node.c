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
 * @file		node.c
 * @ingroup 	xml
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "node.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_xml_node_t* tb_xml_node_init_element(tb_char_t const* name)
{
	// check
	tb_assert_and_check_return_val(name, TB_NULL);

	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_element_t));
	tb_assert_and_check_return_val(node, TB_NULL);

	// init 
	node->type = TB_XML_NODE_TYPE_ELEMENT;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	tb_pstring_cstrcpy(&node->name, name);

	// ok
	return node;
}
tb_xml_node_t* tb_xml_node_init_text(tb_char_t const* data)
{
	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_text_t));
	tb_assert_and_check_return_val(node, TB_NULL);

	// init 
	node->type = TB_XML_NODE_TYPE_TEXT;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	tb_pstring_cstrcpy(&node->name, "#text");
	if (data) tb_pstring_cstrcpy(&node->data, data);

	// ok
	return node;
}
tb_xml_node_t* tb_xml_node_init_cdata(tb_char_t const* cdata)
{
	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_cdata_t));
	tb_assert_and_check_return_val(node, TB_NULL);

	// init 
	node->type = TB_XML_NODE_TYPE_CDATA;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	tb_pstring_cstrcpy(&node->name, "#cdata");
	if (cdata) tb_pstring_cstrcpy(&node->data, cdata);

	// ok
	return cdata;
}
tb_xml_node_t* tb_xml_node_init_comment(tb_char_t const* comment)
{
	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_comment_t));
	tb_assert_and_check_return_val(node, TB_NULL);

	// init 
	node->type = TB_XML_NODE_TYPE_COMMENT;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	tb_pstring_cstrcpy(&node->name, "#comment");
	if (comment) tb_pstring_cstrcpy(&node->data, comment);

	// ok
	return node;
}
tb_xml_node_t* tb_xml_node_init_attribute(tb_char_t const* name, tb_char_t const* data)
{
	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_attribute_t));
	tb_assert_and_check_return_val(node, TB_NULL);

	// init 
	node->type = TB_XML_NODE_TYPE_ATTRIBUTE;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	if (name) tb_pstring_cstrcpy(&node->name, name);
	if (data) tb_pstring_cstrcpy(&node->data, data);

	// ok
	return node;
}
tb_xml_node_t* tb_xml_node_init_document(tb_char_t const* version, tb_char_t const* encoding)
{
	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_document_t));
	tb_assert_and_check_return_val(node, TB_NULL);

	// init 
	node->type = TB_XML_NODE_TYPE_DOCUMENT;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	tb_pstring_init(&((tb_xml_document_t*)node)->version);
	tb_pstring_init(&((tb_xml_document_t*)node)->encoding);
	tb_pstring_cstrcpy(&node->name, "#document");
	tb_pstring_cstrcpy(&((tb_xml_document_t*)node)->version, version? version : "2.0");
	tb_pstring_cstrcpy(&((tb_xml_document_t*)node)->encoding, encoding? encoding : "utf-8");

	// ok
	return node;
}
tb_void_t tb_xml_node_exit(tb_xml_node_t* node)
{
	if (node)
	{
		// free name & data
		tb_pstring_exit(&node->name);
		tb_pstring_exit(&node->data);

		// free version & encoding for document
		if (node->type == TB_XML_NODE_TYPE_DOCUMENT)
		{
			tb_pstring_exit(&((tb_xml_document_t*)node)->version);
			tb_pstring_exit(&((tb_xml_document_t*)node)->encoding);
		}

		// free childs
		if (node->childs)
		{
			tb_xml_node_t* save = TB_NULL;
			tb_xml_node_t* next = node->childs;
			while (next)
			{
				// save
				save = next->next;
				
				// exit
				tb_xml_node_exit(node);

				// next
				next = save;
			}
		}

		// free attributes
		if (node->attributes)
		{
			tb_xml_node_t* save = TB_NULL;
			tb_xml_node_t* next = node->attributes;
			while (next)
			{
				// save
				save = next->next;
				
				// exit
				tb_xml_node_exit(node);

				// next
				next = save;
			}
		}

		// free it
		tb_free(node);
	}
}

