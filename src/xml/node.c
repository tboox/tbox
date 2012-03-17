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
 * \author		ruki
 * \file		node.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "node.h"
#include "nlist.h"
#include "document.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * details
 */
static tb_xml_node_t* tb_xml_node_childs_select_node(tb_xml_node_t* node, tb_sstring_t* parent, tb_char_t const* path)
{
	tb_assert_and_check_return_val(node && node->childs, TB_NULL);
	
	// init
	tb_xml_node_t* 	r = TB_NULL;

	// init string
	tb_sstring_t 	s;
	tb_char_t 		d[4096];
	if (!tb_sstring_init(&s, d, 4096)) return TB_NULL;

	// find it
	tb_xml_node_t* head = (tb_xml_node_t*)node->childs;
	tb_xml_node_t* item = head->next;
	while (item && item != head)
	{
		// element?
		if (item->type == TB_XML_NODE_TYPE_ELEMENT 
			&& tb_pstring_size(&item->name))
		{
			// append path
			tb_sstring_clear(&s);
			if (parent) tb_sstring_strcat(&s, parent);
			tb_sstring_strfcat(&s, "/%s", tb_pstring_cstr(&item->name));

			// is this?
			if (!tb_sstring_cstrcmp(&s, path))
			{
				r = item;
				break;
			}
			// select from next childs
			else
			{
				r = tb_xml_node_childs_select_node(item, &s, path);
				if (r) break;
			}
		}
		item = item->next;
	}

	// exit string
	tb_sstring_exit(&s);

	return r;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_xml_node_t* tb_xml_node_init(tb_pointer_t document, tb_size_t type)
{
	// the node size
	static tb_size_t sizes[] = 
	{
		0
	,	sizeof(tb_xml_element_t)
	,	sizeof(tb_xml_attribute_t)
	,	sizeof(tb_xml_text_t)
	,	sizeof(tb_xml_cdata_t)
	,	0
	,	0
	, 	0
	,	sizeof(tb_xml_comment_t)
	,	sizeof(tb_xml_document_t)
	,	0
	, 	0
	, 	0
	
	};
	tb_assert_and_check_return_val(type < tb_arrayn(sizes) && sizes[type], TB_NULL);

	// alloc node
	tb_xml_node_t* node = tb_nalloc0(1, sizes[type]);
	tb_assert_and_check_return_val(node, TB_NULL);

	// init node
	node->type = type;
	node->document = document? document : node;
	node->prev = node;
	node->next = node;
	tb_pstring_init(&node->name);
	tb_pstring_init(&node->value);

	// ok
	return node;
}
tb_void_t tb_xml_node_exit(tb_xml_node_t* node)
{
	if (node)
	{
		// free specificed data
		if (node->free) node->free(node);

		// free name & value
		tb_pstring_exit(&node->name);
		tb_pstring_exit(&node->value);

		// free childs
		if (node->childs) tb_xml_nlist_exit(node->childs);
		node->childs = TB_NULL;

		// free attributes
		if (node->attributes) tb_xml_nlist_exit(node->attributes);
		node->attributes = TB_NULL;

		// free it
		tb_free(node);
	}
}

tb_void_t tb_xml_node_childs_append(tb_xml_node_t* node, tb_xml_node_t* child)
{
	tb_assert(node && child);
	if (!node || !child) return ;

	if (!node->childs) node->childs = tb_xml_nlist_init();
	if (node->childs) 
	{
		tb_xml_nlist_add(node->childs, child);
		child->parent = node;
	}
}
tb_void_t tb_xml_node_childs_remove(tb_xml_node_t* node, tb_xml_node_t* child)
{
	tb_assert(node && child);
	if (!node || !child) return ;

	if (node->childs) 
	{
		tb_xml_nlist_det(node->childs, child);
		child->parent = TB_NULL;
	}
}
tb_xml_node_t* tb_xml_node_childs_head(tb_xml_node_t* node)
{
	if (node && node->childs) return node->childs->base.next;
	return TB_NULL;
}
tb_xml_node_t* tb_xml_node_childs_tail(tb_xml_node_t* node)
{
	if (node && node->childs) return node->childs->base.prev;
	return TB_NULL;
}

tb_void_t tb_xml_node_attributes_clear(tb_xml_node_t* node)
{
	if (node)
	{
		if (node->attributes) tb_xml_nlist_exit(node->attributes);
		node->attributes = TB_NULL;
	}
}
tb_xml_node_t* tb_xml_node_attributes_add_string(tb_xml_node_t* node, tb_char_t const* name, tb_pstring_t const* value)
{
	tb_assert(node && name && value);
	if (!node || !name || !value) return TB_NULL;

	// init attribute
	tb_xml_node_t* attribute = (tb_xml_node_t*)tb_xml_document_init_attribute(node->document, name);
	if (!attribute) return TB_NULL;

	// init attribute
	tb_pstring_strcpy(&attribute->value, value);

	// init attributes
	if (!node->attributes) node->attributes = tb_xml_nlist_init();
	if (!node->attributes) goto fail;

	// add attribute
	tb_xml_nlist_add(node->attributes, attribute);

	return attribute;
fail:
	if (attribute) tb_xml_node_exit(attribute);
	return TB_NULL;
}
tb_xml_node_t* tb_xml_node_attributes_add_c_string(tb_xml_node_t* node, tb_char_t const* name, tb_char_t const* value)
{
	tb_assert(node && name && value);
	if (!node || !name || !value) return TB_NULL;

	// init attribute
	tb_xml_node_t* attribute = (tb_xml_node_t*)tb_xml_document_init_attribute(node->document, name);
	if (!attribute) return TB_NULL;

	// init attribute
	tb_pstring_cstrcpy(&attribute->value, value);

	// init attributes
	if (!node->attributes) node->attributes = tb_xml_nlist_init();
	if (!node->attributes) goto fail;

	// add attribute
	tb_xml_nlist_add(node->attributes, attribute);

	return attribute;
fail:
	if (attribute) tb_xml_node_exit(attribute);
	return TB_NULL;
}
tb_xml_node_t* tb_xml_node_attributes_add_int(tb_xml_node_t* node, tb_char_t const* name, tb_int_t value)
{
	tb_assert(node && name);
	if (!node || !name) return TB_NULL;

	// init attribute
	tb_xml_node_t* attribute = (tb_xml_node_t*)tb_xml_document_init_attribute(node->document, name);
	if (!attribute) return TB_NULL;

	// init attribute
	tb_pstring_cstrfcpy(&attribute->value, "%d", value);

	// init attributes
	if (!node->attributes) node->attributes = tb_xml_nlist_init();
	if (!node->attributes) goto fail;

	// add attribute
	tb_xml_nlist_add(node->attributes, attribute);

	return attribute;
fail:
	if (attribute) tb_xml_node_exit(attribute);
	return TB_NULL;
}

#ifdef TB_CONFIG_TYPE_FLOAT
tb_xml_node_t* tb_xml_node_attributes_add_float(tb_xml_node_t* node, tb_char_t const* name, tb_float_t value)
{
	tb_assert(node && name);
	if (!node || !name) return TB_NULL;

	// init attribute
	tb_xml_node_t* attribute = (tb_xml_node_t*)tb_xml_document_init_attribute(node->document, name);
	if (!attribute) return TB_NULL;

	// init attribute
	tb_pstring_cstrfcpy(&attribute->value, "%g", value);

	// init attributes
	if (!node->attributes) node->attributes = tb_xml_nlist_init();
	if (!node->attributes) goto fail;

	// add attribute
	tb_xml_nlist_add(node->attributes, attribute);

	return attribute;
fail:
	if (attribute) tb_xml_node_exit(attribute);
	return TB_NULL;
}
#endif
tb_xml_node_t* tb_xml_node_attributes_add_bool(tb_xml_node_t* node, tb_char_t const* name, tb_bool_t value)
{
	tb_assert(node && name);
	if (!node || !name) return TB_NULL;

	// init attribute
	tb_xml_node_t* attribute = tb_xml_document_init_attribute(node->document, name);
	if (!attribute) return TB_NULL;

	// init attribute
	tb_pstring_cstrfcpy(&attribute->value, value == TB_TRUE? "true" : "false");

	// init attributes
	if (!node->attributes) node->attributes = tb_xml_nlist_init();
	if (!node->attributes) goto fail;

	// add attribute
	tb_xml_nlist_add(node->attributes, attribute);

	return attribute;
fail:
	if (attribute) tb_xml_node_exit(attribute);
	return TB_NULL;
}
tb_xml_node_t* tb_xml_node_attributes_add_format(tb_xml_node_t* node, tb_char_t const* name, tb_char_t const* fmt, ...)
{
	tb_assert(node && name && fmt);
	if (!node || !name || !fmt) return TB_NULL;

	// format text
	tb_char_t text[4096];
	tb_size_t size = 0;
	tb_va_format(text, 4096, fmt, &size);
	if (!size) return TB_NULL;

	// init attribute
	tb_xml_node_t* attribute = tb_xml_document_init_attribute(node->document, name);
	if (!attribute) return TB_NULL;

	// init attribute
	tb_pstring_cstrfcpy(&attribute->value, text);

	// init attributes
	if (!node->attributes) node->attributes = tb_xml_nlist_init();
	if (!node->attributes) goto fail;

	// add attribute
	tb_xml_nlist_add(node->attributes, attribute);

	return attribute;
fail:
	if (attribute) tb_xml_node_exit(attribute);
	return TB_NULL;
}

tb_xml_node_t* tb_xml_node_add_element(tb_xml_node_t* node, tb_char_t const* name)
{
	tb_assert(node && name);
	if (!node || !name) return TB_NULL;

	// init element
	tb_xml_node_t* element = tb_xml_document_init_element(node->document, name);
	if (!element) return TB_NULL;

	// add element
	tb_xml_node_childs_append(node, element);

	return element;
}
tb_xml_node_t* tb_xml_node_add_text(tb_xml_node_t* node, tb_char_t const* data)
{
	tb_assert(node && data);
	if (!node || !data) return TB_NULL;

	// init text
	tb_xml_node_t* text = tb_xml_document_init_text(node->document, data);
	if (!text) return TB_NULL;

	// add text
	tb_xml_node_childs_append(node, text);

	return text;
}
tb_xml_node_t* tb_xml_node_add_cdata(tb_xml_node_t* node, tb_char_t const* data)
{
	tb_assert(node && data);
	if (!node || !data) return TB_NULL;

	// init cdata
	tb_xml_node_t* cdata = tb_xml_document_init_cdata(node->document, data);
	if (!cdata) return TB_NULL;

	// add cdata
	tb_xml_node_childs_append(node, cdata);

	return cdata;
}
tb_xml_node_t* tb_xml_node_add_comment(tb_xml_node_t* node, tb_char_t const* data)
{
	tb_assert(node && data);
	if (!node || !data) return TB_NULL;

	// init comment
	tb_xml_node_t* comment = tb_xml_document_init_comment(node->document, data);
	if (!comment) return TB_NULL;

	// add comment
	tb_xml_node_childs_append(node, comment);

	return comment;
}
tb_xml_node_t* tb_xml_node_add_attribute(tb_xml_node_t* node, tb_char_t const* name)
{
	tb_assert(node && name);
	if (!node || !name) return TB_NULL;

	// init attribute
	tb_xml_node_t* attribute = tb_xml_document_init_attribute(node->document, name);
	if (!attribute) return TB_NULL;

	// add attribute
	tb_xml_node_childs_append(node, attribute);

	return attribute;
}

tb_xml_node_t* tb_xml_node_childs_select(tb_xml_node_t* node, tb_char_t const* path)
{
	tb_assert(node && node->childs && path);
	if (!node || !node->childs || !path) return TB_NULL;

	return tb_xml_node_childs_select_node(node, TB_NULL, path);
}
