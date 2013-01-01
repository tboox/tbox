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
 * trace
 */
//#define TB_TRACE_IMPL_TAG 		"xml"

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
		if (node->chead)
		{
			tb_xml_node_t* save = TB_NULL;
			tb_xml_node_t* next = node->chead;
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
		if (node->ahead)
		{
			tb_xml_node_t* save = TB_NULL;
			tb_xml_node_t* next = node->ahead;
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
tb_void_t tb_xml_node_insert_next(tb_xml_node_t* node, tb_xml_node_t* next)
{
	// check
	tb_assert_and_check_return(node && next);

	// init
	next->parent = node->parent;
	next->next = node->next;

	// next
	node->next = next;
}
tb_void_t tb_xml_node_remove_next(tb_xml_node_t* node)
{
	// check
	tb_assert_and_check_return(node);

	// next
	tb_xml_node_t* next = node->next;

	// save
	tb_xml_node_t* save = next? next->next : TB_NULL;

	// exit
	if (next) tb_xml_node_exit(next);

	// next
	node->next = save;
}
tb_void_t tb_xml_node_append_chead(tb_xml_node_t* node, tb_xml_node_t* child)
{
	// check
	tb_assert_and_check_return(node && child);

	// init
	child->parent = node;

	// append
	if (node->chead) 
	{
		child->next = node->chead;
		node->chead = child;
	}
	else
	{
		tb_assert(!node->ctail);
		node->ctail = node->chead = child;
	}
}
tb_void_t tb_xml_node_append_ctail(tb_xml_node_t* node, tb_xml_node_t* child)
{
	// check
	tb_assert_and_check_return(node && child);

	// init
	child->parent = node;
	child->next = TB_NULL;

	// append
	if (node->ctail) 
	{
		node->ctail->next = child;
		node->ctail = child;
	}
	else
	{
		tb_assert(!node->chead);
		node->ctail = node->chead = child;
	}
}
tb_void_t tb_xml_node_append_ahead(tb_xml_node_t* node, tb_xml_node_t* attribute)
{
	// check
	tb_assert_and_check_return(node && attribute);

	// init
	attribute->parent = node;

	// append
	if (node->ahead) 
	{
		attribute->next = node->ahead;
		node->ahead = attribute;
	}
	else
	{
		tb_assert(!node->atail);
		node->atail = node->ahead = attribute;
	}
}
tb_void_t tb_xml_node_append_atail(tb_xml_node_t* node, tb_xml_node_t* attribute)
{
	// check
	tb_assert_and_check_return(node && attribute);

	// init
	attribute->parent = node;
	attribute->next = TB_NULL;

	// append
	if (node->atail) 
	{
		node->atail->next = attribute;
		node->atail = attribute;
	}
	else
	{
		tb_assert(!node->ahead);
		node->atail = node->ahead = attribute;
	}
}

tb_xml_node_t* tb_xml_node_goto(tb_xml_node_t* node, tb_char_t const* path)
{
	tb_assert_and_check_return_val(node && path, TB_NULL);
	tb_trace_impl("root: %s goto: %s", tb_pstring_cstr(&node->name), path);

	// skip '/'
	tb_char_t const* p = path; while (*p && *p == '/') p++;

	// is self?
	if (!*p) return node;

	// size
	tb_size_t n = tb_strlen(p);

	// walk the child nodes
	tb_xml_node_t* root = node;
	tb_xml_node_t* head = node->chead;
	tb_xml_node_t* tail = node->ctail;
	for (node = head; node; node = node->next)
	{
		if (node->type == TB_XML_NODE_TYPE_ELEMENT)
		{
			// size
			tb_size_t m = tb_pstring_size(&node->name);

			// trace
			tb_trace_impl("%s", tb_pstring_cstr(&node->name));

			// has it?
			if (!tb_pstring_cstrncmp(&node->name, p, m))
			{
				// is it?
				if (m == n) return node;
				else if (m < n)
				{
					// skip this node
					tb_char_t const* q = p + m;	

					// is root?
					if (*q == '/')
					{
						// goto the child node
						tb_xml_node_t* c = tb_xml_node_goto(node, q);
						if (c) return c;
					}
				}
			}
		}
	}

	// no
	return TB_NULL;
}

