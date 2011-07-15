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
 * \file		document.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "document.h"
#include "nlist.h"

/* /////////////////////////////////////////////////////////
 * details
 */
static void tb_xml_document_free(tb_xml_node_t* node)
{
	tb_xml_document_t* document = (tb_xml_document_t*)node;
	if (document)
	{
		// clear it
		tb_xml_document_clear(document);

		// free it
		tb_string_uninit(&document->version);
		tb_string_uninit(&document->encoding);
	}
}
static void tb_xml_document_store_childs(tb_xml_writer_t* writer, tb_xml_nlist_t* childs, tb_bool_t* ret)
{
	if (!writer || !childs) return ;

	// get head
	tb_xml_node_t* head = (tb_xml_node_t*)childs;

	// strore childs
	tb_xml_node_t* node = head->next;
	while (node && node != head)
	{
		// is childs?
		if (node->childs) 
		{
			// write attributes
			if (node->attributes)
			{
				tb_xml_node_t* ahead = (tb_xml_node_t*)node->attributes;
				tb_xml_node_t* anode = ahead->next;
				while (anode && anode != ahead)
				{
					tb_xml_writer_attributes_add_string(writer, tb_string_c_string(&anode->name), &anode->value);
					anode = anode->next;
				}
			}

			// write element
			tb_xml_writer_element_beg(writer, tb_string_c_string(&node->name));
			tb_xml_document_store_childs(writer, node->childs, ret);
			tb_xml_writer_element_end(writer, tb_string_c_string(&node->name));

			if (ret && *ret == TB_FALSE) goto fail;
		}
		else
		{
			// store node
			switch (node->type)
			{
			case TB_XML_NODE_TYPE_ELEMENT:
				{
					// write attributes
					if (node->attributes)
					{
						tb_xml_node_t* ahead = (tb_xml_node_t*)node->attributes;
						tb_xml_node_t* anode = ahead->next;
						while (anode && anode != ahead)
						{
							tb_xml_writer_attributes_add_string(writer, tb_string_c_string(&anode->name), &anode->value);
							anode = anode->next;
						}
					}

					// writer element
					tb_xml_writer_element_empty(writer, tb_string_c_string(&node->name));
				}
				break;
			case TB_XML_NODE_TYPE_TEXT:
				tb_xml_writer_text(writer, tb_string_c_string(&node->value));
				break;
			case TB_XML_NODE_TYPE_CDATA:
				tb_xml_writer_cdata(writer, tb_string_c_string(&node->value));
				break;
			case TB_XML_NODE_TYPE_COMMENT:
				tb_xml_writer_comment(writer, tb_string_c_string(&node->value));
				break;
			default: goto fail;
			}
		}

		// next node
		node = node->next;
	}

	return ;
fail:
	if (ret) *ret = TB_FALSE;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_xml_document_t* tb_xml_document_create()
{
	// alloc document
	tb_xml_document_t* document = (tb_xml_document_t*)tb_malloc(sizeof(tb_xml_document_t));
	if (!document) return TB_NULL;

	// init node
	tb_xml_node_init(&document->base, document, TB_XML_NODE_TYPE_DOCUMENT);
	document->base.free = tb_xml_document_free;
	tb_string_assign_c_string_by_ref(&document->base.name, "#document");

	// init document
	tb_string_init(&document->version);
	tb_string_init(&document->encoding);
	tb_string_assign_c_string_by_ref(&document->version, "2.0");
	tb_string_assign_c_string_by_ref(&document->encoding, "utf-8");

	return document;
}
void tb_xml_document_destroy(tb_xml_document_t* document)
{
	if (document) tb_xml_node_destroy((tb_xml_node_t*)document);
}

tb_bool_t tb_xml_document_load(tb_xml_document_t* document, tb_gstream_t* gst)
{
	TB_ASSERT(document);
	if (!document) return TB_FALSE;

	// clear document
	tb_xml_document_clear(document);

	// open reader
	tb_xml_reader_t* reader = tb_xml_reader_open(gst);
	TB_ASSERT(reader);
	if (!reader) return TB_FALSE;

	// the parent node
	tb_xml_node_t* parent = (tb_xml_node_t*)document;

	// has event?
	while (TB_TRUE == tb_xml_reader_has_next(reader))
	{
		// get event type
		tb_size_t event = tb_xml_reader_get_event(reader);
		switch (event)
		{
		case TB_XML_READER_EVENT_DOCUMENT_BEG: 
			{
				if (TB_NULL == tb_string_assign(&document->version, tb_xml_reader_get_version(reader))) goto fail;
				tb_string_assign(&document->encoding, tb_xml_reader_get_encoding(reader));
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				// get element name
				tb_char_t const* name = tb_string_c_string(tb_xml_reader_get_element_name(reader));
				if (!name) goto fail;

				// create element
				tb_xml_node_t* element = tb_xml_document_create_element(document, name);
				if (!element) goto fail;

				// add attributes
				tb_size_t n = tb_xml_reader_get_attribute_count(reader);
				if (n)
				{
					tb_int_t i = 0;
					for (i = 0; i < n; i++)
					{
						tb_char_t const* name = tb_string_c_string(tb_xml_reader_get_attribute_name(reader, i));
						tb_string_t const* value = tb_xml_reader_get_attribute_value_by_index(reader, i);
						if (name && value) tb_xml_node_attributes_add_string(element, name, value);
					}
				}

				// append element
				tb_xml_node_childs_append(parent, element);

				// enter element
				if (event != TB_XML_READER_EVENT_ELEMENT_EMPTY) parent = element;
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// check
				if (TB_FALSE == tb_string_compare(&parent->name, tb_xml_reader_get_element_name(reader)))
					goto fail;

				// leave element
				parent = parent->parent;

				// check parent
				TB_ASSERT(parent);
				if (!parent) goto fail;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// get text data
				tb_char_t const* data = tb_string_c_string(tb_xml_reader_get_text(reader));

				// create text
				tb_xml_node_t* text = tb_xml_document_create_text(document, data? data : "");
				if (!text) goto fail;

				// append text
				tb_xml_node_childs_append(parent, text);
			}
			break;
		case TB_XML_READER_EVENT_CDATA: 
			{
				// get cdata data
				tb_char_t const* data = tb_string_c_string(tb_xml_reader_get_cdata(reader));

				// create cdata
				tb_xml_node_t* cdata = tb_xml_document_create_cdata(document, data? data : "");
				if (!cdata) goto fail;

				// append cdata
				tb_xml_node_childs_append(parent, cdata);
	
			}
			break;
		case TB_XML_READER_EVENT_COMMENT: 
			{
				// get comment data
				tb_char_t const* data = tb_string_c_string(tb_xml_reader_get_comment(reader));

				// create comment
				tb_xml_node_t* comment = tb_xml_document_create_comment(document, data? data : "");
				if (!comment) goto fail;

				// append comment
				tb_xml_node_childs_append(parent, comment);
			}
			break;
		default: break;
		}

		// next event
		tb_xml_reader_next(reader);
	}

end:

	// close reader
	if (reader) tb_xml_reader_close(reader);

	return TB_TRUE;

fail:

	// clear document
	tb_xml_document_clear(document);

	// close reader
	if (reader) tb_xml_reader_close(reader);

	return TB_FALSE;

}
tb_bool_t tb_xml_document_store(tb_xml_document_t* document, tb_gstream_t* gst)
{
	TB_ASSERT(document);
	if (!document) return TB_FALSE;

	// open writer
	tb_xml_writer_t* writer = tb_xml_writer_open(gst);
	TB_ASSERT(writer);
	if (!writer) return TB_FALSE;

	// check xml header
	if (TB_TRUE == tb_string_is_null(&document->version)) return TB_FALSE;
	if (TB_TRUE == tb_string_is_null(&document->encoding)) return TB_FALSE;

	// begin document
	tb_xml_writer_document_beg(writer, tb_string_c_string(tb_xml_document_version(document)), tb_string_c_string(tb_xml_document_encoding(document)));
	tb_xml_writer_text(writer, "\n");

	// store document tree
	tb_bool_t ret = TB_TRUE;
	tb_xml_document_store_childs(writer, document->base.childs, &ret);

	// end document
	tb_xml_writer_document_end(writer);
	tb_xml_writer_close(writer);

	return ret;
}

void tb_xml_document_clear(tb_xml_document_t* document)
{
	TB_ASSERT(document);
	if (!document) return ;

	// clear version & encoding
	tb_string_assign_c_string_by_ref(&document->version, "2.0");
	tb_string_assign_c_string_by_ref(&document->encoding, "utf-8");

	// clear childs
	if (document->base.childs) tb_xml_nlist_destroy(document->base.childs);
	document->base.childs = TB_NULL;

	// clear attributes
	if (document->base.attributes) tb_xml_nlist_destroy(document->base.attributes);
	document->base.attributes = TB_NULL;
}
tb_string_t* tb_xml_document_version(tb_xml_document_t* document)
{
	if (document) return &document->version;
	else return TB_NULL;
}
tb_string_t* tb_xml_document_encoding(tb_xml_document_t* document)
{
	if (document) return &document->encoding;
	else return TB_NULL;
}
tb_xml_node_t* tb_xml_document_create_element(tb_xml_document_t* document, tb_char_t const* name)
{
	TB_ASSERT(document && name);
	if (!document || !name) return TB_NULL;

	// alloc element
	tb_xml_node_t* element = (tb_xml_node_t*)tb_malloc(sizeof(tb_xml_element_t));
	if (!element) return TB_NULL;

	// init it
	tb_xml_node_init(element, document, TB_XML_NODE_TYPE_ELEMENT);
	tb_string_assign_c_string(&element->name, name);
	tb_xml_node_attributes_clear(element);

	return element;
}
tb_xml_node_t* tb_xml_document_create_text(tb_xml_document_t* document, tb_char_t const* data)
{
	TB_ASSERT(document && data);
	if (!document || !data) return TB_NULL;

	// alloc text
	tb_xml_node_t* text = (tb_xml_node_t*)tb_malloc(sizeof(tb_xml_text_t));
	if (!text) return TB_NULL;

	// init it
	tb_xml_node_init(text, document, TB_XML_NODE_TYPE_TEXT);
	tb_string_assign_c_string_by_ref(&text->name, "#text");
	tb_string_assign_c_string(&text->value, data);

	return text;
}
tb_xml_node_t* tb_xml_document_create_cdata(tb_xml_document_t* document, tb_char_t const* data)
{
	TB_ASSERT(document && data);
	if (!document || !data) return TB_NULL;

	// alloc cdata
	tb_xml_node_t* cdata = (tb_xml_node_t*)tb_malloc(sizeof(tb_xml_cdata_t));
	if (!cdata) return TB_NULL;

	// init it
	tb_xml_node_init(cdata, document, TB_XML_NODE_TYPE_CDATA);
	tb_string_assign_c_string_by_ref(&cdata->name, "#cdata");
	tb_string_assign_c_string(&cdata->value, data);

	return cdata;
}
tb_xml_node_t* tb_xml_document_create_comment(tb_xml_document_t* document, tb_char_t const* data)
{
	TB_ASSERT(document && data);
	if (!document || !data) return TB_NULL;

	// alloc comment
	tb_xml_node_t* comment = (tb_xml_node_t*)tb_malloc(sizeof(tb_xml_comment_t));
	if (!comment) return TB_NULL;

	// init it
	tb_xml_node_init(comment, document, TB_XML_NODE_TYPE_COMMENT);
	tb_string_assign_c_string_by_ref(&comment->name, "#comment");
	tb_string_assign_c_string(&comment->value, data);

	return comment;
}
tb_xml_node_t* tb_xml_document_create_attribute(tb_xml_document_t* document, tb_char_t const* name)
{
	TB_ASSERT(document && name);
	if (!document || !name) return TB_NULL;

	// alloc attribute
	tb_xml_node_t* attribute = (tb_xml_node_t*)tb_malloc(sizeof(tb_xml_attribute_t));
	if (!attribute) return TB_NULL;

	// init it
	tb_xml_node_init(attribute, document, TB_XML_NODE_TYPE_ATTRIBUTE);
	tb_string_assign_c_string(&attribute->name, name);

	return attribute;
}
