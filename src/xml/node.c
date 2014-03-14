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
 * @author		ruki
 * @file		node.c
 * @ingroup 	xml
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 		"xml"

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
	tb_assert_and_check_return_val(name, tb_null);

	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_element_t));
	tb_assert_and_check_return_val(node, tb_null);

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
	tb_assert_and_check_return_val(node, tb_null);

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
	tb_assert_and_check_return_val(node, tb_null);

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
	tb_assert_and_check_return_val(node, tb_null);

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
	tb_assert_and_check_return_val(node, tb_null);

	// init 
	node->type = TB_XML_NODE_TYPE_ATTRIBUTE;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	if (name) tb_pstring_cstrcpy(&node->name, name);
	if (data) tb_pstring_cstrcpy(&node->data, data);

	// ok
	return node;
}
tb_xml_node_t* tb_xml_node_init_document(tb_char_t const* version, tb_char_t const* charset)
{
	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_document_t));
	tb_assert_and_check_return_val(node, tb_null);

	// init 
	node->type = TB_XML_NODE_TYPE_DOCUMENT;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	tb_pstring_init(&((tb_xml_document_t*)node)->version);
	tb_pstring_init(&((tb_xml_document_t*)node)->charset);
	tb_pstring_cstrcpy(&node->name, "#document");
	tb_pstring_cstrcpy(&((tb_xml_document_t*)node)->version, version? version : "2.0");
	tb_pstring_cstrcpy(&((tb_xml_document_t*)node)->charset, charset? charset : "utf-8");

	// ok
	return node;
}
tb_xml_node_t* tb_xml_node_init_document_type(tb_char_t const* type)
{
	// alloc
	tb_xml_node_t* node = tb_malloc0(sizeof(tb_xml_document_type_t));
	tb_assert_and_check_return_val(node, tb_null);

	// init 
	node->type = TB_XML_NODE_TYPE_DOCUMENT_TYPE;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->data);
	tb_pstring_init(&((tb_xml_document_type_t*)node)->type);
	tb_pstring_cstrcpy(&node->name, "#doctype");
	tb_pstring_cstrcpy(&((tb_xml_document_type_t*)node)->type, type? type : "");

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

		// free version & charset for document
		if (node->type == TB_XML_NODE_TYPE_DOCUMENT)
		{
			tb_pstring_exit(&((tb_xml_document_t*)node)->version);
			tb_pstring_exit(&((tb_xml_document_t*)node)->charset);
		}

		// free type
		if (node->type == TB_XML_NODE_TYPE_DOCUMENT_TYPE)
			tb_pstring_exit(&((tb_xml_document_type_t*)node)->type);

		// free childs
		if (node->chead)
		{
			tb_xml_node_t* save = tb_null;
			tb_xml_node_t* next = node->chead;
			while (next)
			{
				// save
				save = next->next;
				
				// exit
				tb_xml_node_exit(next);

				// next
				next = save;
			}
		}

		// free attributes
		if (node->ahead)
		{
			tb_xml_node_t* save = tb_null;
			tb_xml_node_t* next = node->ahead;
			while (next)
			{
				// save
				save = next->next;
				
				// exit
				tb_xml_node_exit(next);

				// next
				next = save;
			}
		}

		// free it
		tb_free(node);
	}
}

tb_xml_node_t* tb_xml_node_chead(tb_xml_node_t const* node)
{
	tb_assert_and_check_return_val(node, tb_null);
	return node->chead;
}
tb_size_t tb_xml_node_csize(tb_xml_node_t const* node)
{
	tb_assert_and_check_return_val(node, 0);
	return node->csize;
}
tb_xml_node_t* tb_xml_node_ahead(tb_xml_node_t const* node)
{
	tb_assert_and_check_return_val(node, tb_null);
	return node->ahead;
}
tb_size_t tb_xml_node_asize(tb_xml_node_t const* node)
{
	tb_assert_and_check_return_val(node, 0);
	return node->asize;
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
	tb_xml_node_t* save = next? next->next : tb_null;

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
		node->csize++;
	}
	else
	{
		tb_assert(!node->ctail);
		node->ctail = node->chead = child;
		node->csize = 1;
	}
}
tb_void_t tb_xml_node_append_ctail(tb_xml_node_t* node, tb_xml_node_t* child)
{
	// check
	tb_assert_and_check_return(node && child);

	// init
	child->parent = node;
	child->next = tb_null;

	// append
	if (node->ctail) 
	{
		node->ctail->next = child;
		node->ctail = child;
		node->csize++;
	}
	else
	{
		tb_assert(!node->chead);
		node->ctail = node->chead = child;
		node->csize = 1;
	}
}
tb_void_t tb_xml_node_remove_chead(tb_xml_node_t* node)
{
	// check
	tb_assert_and_check_return(node);

	// null?
	tb_check_return(node->chead);

	// remove
	if (node->chead != node->ctail) 
	{
		// save
		tb_xml_node_t* save = node->chead;

		// remove
		node->chead = save->next;

		// exit
		tb_xml_node_exit(save);

		// size--
		node->csize--;
	}
	else
	{
		// save
		tb_xml_node_t* save = node->chead;

		// remove
		node->chead = tb_null;
		node->ctail = tb_null;

		// exit
		tb_xml_node_exit(save);

		// size--
		node->csize--;
	}
}
tb_void_t tb_xml_node_remove_ctail(tb_xml_node_t* node)
{
	tb_trace_noimpl();
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
		node->asize++;
	}
	else
	{
		tb_assert(!node->atail);
		node->atail = node->ahead = attribute;
		node->asize = 1;
	}
}
tb_void_t tb_xml_node_append_atail(tb_xml_node_t* node, tb_xml_node_t* attribute)
{
	// check
	tb_assert_and_check_return(node && attribute);

	// init
	attribute->parent = node;
	attribute->next = tb_null;

	// append
	if (node->atail) 
	{
		node->atail->next = attribute;
		node->atail = attribute;
		node->asize++;
	}
	else
	{
		tb_assert(!node->ahead);
		node->atail = node->ahead = attribute;
		node->asize = 1;
	}
}

tb_xml_node_t* tb_xml_node_goto(tb_xml_node_t* node, tb_char_t const* path)
{
	tb_assert_and_check_return_val(node && path, tb_null);
	tb_trace_d("root: %s goto: %s", tb_pstring_cstr(&node->name), path);

	// skip '/'
	tb_char_t const* p = path; while (*p && *p == '/') p++;

	// is self?
	if (!*p) return node;

	// size
	tb_size_t n = tb_strlen(p);

	// walk the child nodes
	tb_xml_node_t* head = node->chead;
	for (node = head; node; node = node->next)
	{
		if (node->type == TB_XML_NODE_TYPE_ELEMENT)
		{
			// size
			tb_size_t m = tb_pstring_size(&node->name);

			// trace
			tb_trace_d("%s", tb_pstring_cstr(&node->name));

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
	return tb_null;
}

