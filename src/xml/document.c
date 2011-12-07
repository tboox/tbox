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
static tb_void_t tb_xml_document_free(tb_xml_node_t* node)
{
	tb_xml_document_t* document = (tb_xml_document_t*)node;
	if (document)
	{
		// clear it
		tb_xml_document_clear(document);

		// free it
		tb_pstring_exit(&document->version);
		tb_pstring_exit(&document->encoding);
	}
}
static tb_void_t tb_xml_document_store_childs(tb_xml_writer_t* writer, tb_xml_nlist_t* childs, tb_bool_t* ret)
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
			// writ attributes
			if (node->attributes)
			{
				tb_xml_node_t* ahead = (tb_xml_node_t*)node->attributes;
				tb_xml_node_t* anode = ahead->next;
				while (anode && anode != ahead)
				{
					tb_xml_writer_attributes_add_string(writer, tb_pstring_cstr(&anode->name), &anode->value);
					anode = anode->next;
				}
			}

			// writ element
			tb_xml_writer_element_beg(writer, tb_pstring_cstr(&node->name));
			tb_xml_document_store_childs(writer, node->childs, ret);
			tb_xml_writer_element_end(writer, tb_pstring_cstr(&node->name));

			if (ret && *ret == TB_FALSE) goto fail;
		}
		else
		{
			// store node
			switch (node->type)
			{
			case TB_XML_NODE_TYPE_ELEMENT:
				{
					// writ attributes
					if (node->attributes)
					{
						tb_xml_node_t* ahead = (tb_xml_node_t*)node->attributes;
						tb_xml_node_t* anode = ahead->next;
						while (anode && anode != ahead)
						{
							tb_xml_writer_attributes_add_string(writer, tb_pstring_cstr(&anode->name), &anode->value);
							anode = anode->next;
						}
					}

					// writer element
					tb_xml_writer_element_empty(writer, tb_pstring_cstr(&node->name));
				}
				break;
			case TB_XML_NODE_TYPE_TEXT:
				tb_xml_writer_text(writer, tb_pstring_cstr(&node->value));
				break;
			case TB_XML_NODE_TYPE_CDATA:
				tb_xml_writer_cdata(writer, tb_pstring_cstr(&node->value));
				break;
			case TB_XML_NODE_TYPE_COMMENT:
				tb_xml_writer_comment(writer, tb_pstring_cstr(&node->value));
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

tb_xml_document_t* tb_xml_document_init()
{
	// alloc document
	tb_xml_document_t* document = tb_xml_node_init(TB_NULL, TB_XML_NODE_TYPE_DOCUMENT);
	tb_assert_and_check_return_val(document, TB_NULL);

	// init node
	document->base.free = tb_xml_document_free;
	tb_pstring_cstrcpy(&document->base.name, "#document");

	// init document
	tb_pstring_init(&document->version);
	tb_pstring_init(&document->encoding);
	tb_pstring_cstrcpy(&document->version, "2.0");
	tb_pstring_cstrcpy(&document->encoding, "utf-8");

	return document;
}
tb_void_t tb_xml_document_exit(tb_xml_document_t* document)
{
	if (document) tb_xml_node_exit((tb_xml_node_t*)document);
}

tb_bool_t tb_xml_document_load(tb_xml_document_t* document, tb_gstream_t* gst)
{
	tb_assert(document);
	if (!document) return TB_FALSE;

	// clear document
	tb_xml_document_clear(document);

	// open reader
	tb_xml_reader_t* reader = tb_xml_reader_open(gst);
	tb_assert(reader);
	if (!reader) return TB_FALSE;

	// the parent node
	tb_xml_node_t* parent = (tb_xml_node_t*)document;

	// has event?
	while (tb_xml_reader_has_next(reader))
	{
		// get event type
		tb_size_t event = tb_xml_reader_get_event(reader);
		switch (event)
		{
		case TB_XML_READER_EVENT_DOCUMENT_BEG: 
			{
				if (!tb_pstring_strcpy(&document->version, tb_xml_reader_get_version(reader))) goto fail;
				tb_pstring_strcpy(&document->encoding, tb_xml_reader_get_encoding(reader));
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				// get element name
				tb_char_t const* name = tb_pstring_cstr(tb_xml_reader_get_element_name(reader));
				if (!name) goto fail;

				// init element
				tb_xml_node_t* element = tb_xml_document_init_element(document, name);
				if (!element) goto fail;

				// add attributes
				tb_size_t n = tb_xml_reader_get_attribute_count(reader);
				if (n)
				{
					tb_int_t i = 0;
					for (i = 0; i < n; i++)
					{
						tb_char_t const* name = tb_pstring_cstr(tb_xml_reader_get_attribute_name(reader, i));
						tb_pstring_t const* value = tb_xml_reader_get_attribute_value_by_index(reader, i);
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
				if (tb_pstring_strcmp(&parent->name, tb_xml_reader_get_element_name(reader)))
					goto fail;

				// leave element
				parent = parent->parent;

				// check parent
				tb_assert(parent);
				if (!parent) goto fail;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// get text data
				tb_char_t const* data = tb_pstring_cstr(tb_xml_reader_get_text(reader));

				// init text
				tb_xml_node_t* text = tb_xml_document_init_text(document, data? data : "");
				if (!text) goto fail;

				// append text
				tb_xml_node_childs_append(parent, text);
			}
			break;
		case TB_XML_READER_EVENT_CDATA: 
			{
				// get cdata data
				tb_char_t const* data = tb_pstring_cstr(tb_xml_reader_get_cdata(reader));

				// init cdata
				tb_xml_node_t* cdata = tb_xml_document_init_cdata(document, data? data : "");
				if (!cdata) goto fail;

				// append cdata
				tb_xml_node_childs_append(parent, cdata);
	
			}
			break;
		case TB_XML_READER_EVENT_COMMENT: 
			{
				// get comment data
				tb_char_t const* data = tb_pstring_cstr(tb_xml_reader_get_comment(reader));

				// init comment
				tb_xml_node_t* comment = tb_xml_document_init_comment(document, data? data : "");
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
	tb_assert(document);
	if (!document) return TB_FALSE;

	// open writer
	tb_xml_writer_t* writer = tb_xml_writer_open(gst);
	tb_assert(writer);
	if (!writer) return TB_FALSE;

	// check xml header
	if (!tb_pstring_size(&document->version)) return TB_FALSE;
	if (!tb_pstring_size(&document->encoding)) return TB_FALSE;

	// begin document
	tb_xml_writer_document_beg(writer, tb_pstring_cstr(tb_xml_document_version(document)), tb_pstring_cstr(tb_xml_document_encoding(document)));
	tb_xml_writer_text(writer, "\n");

	// store document tree
	tb_bool_t ret = TB_TRUE;
	tb_xml_document_store_childs(writer, document->base.childs, &ret);

	// end document
	tb_xml_writer_document_end(writer);
	tb_xml_writer_close(writer);

	return ret;
}

tb_void_t tb_xml_document_clear(tb_xml_document_t* document)
{
	tb_assert(document);
	if (!document) return ;

	// clear version & encoding
	tb_pstring_cstrcpy(&document->version, "2.0");
	tb_pstring_cstrcpy(&document->encoding, "utf-8");

	// clear childs
	if (document->base.childs) tb_xml_nlist_exit(document->base.childs);
	document->base.childs = TB_NULL;

	// clear attributes
	if (document->base.attributes) tb_xml_nlist_exit(document->base.attributes);
	document->base.attributes = TB_NULL;
}
tb_pstring_t* tb_xml_document_version(tb_xml_document_t* document)
{
	if (document) return &document->version;
	else return TB_NULL;
}
tb_pstring_t* tb_xml_document_encoding(tb_xml_document_t* document)
{
	if (document) return &document->encoding;
	else return TB_NULL;
}
tb_xml_node_t* tb_xml_document_init_element(tb_xml_document_t* document, tb_char_t const* name)
{
	tb_assert(document && name);
	if (!document || !name) return TB_NULL;

	// alloc element
	tb_xml_node_t* element = tb_xml_node_init(document, TB_XML_NODE_TYPE_ELEMENT);
	tb_assert_and_check_return_val(element, TB_NULL);

	// init it
	tb_pstring_cstrcpy(&element->name, name);
	tb_xml_node_attributes_clear(element);

	return element;
}
tb_xml_node_t* tb_xml_document_init_text(tb_xml_document_t* document, tb_char_t const* data)
{
	tb_assert(document && data);
	if (!document || !data) return TB_NULL;

	// alloc text
	tb_xml_node_t* text = tb_xml_node_init(document, TB_XML_NODE_TYPE_TEXT);
	tb_assert_and_check_return_val(text, TB_NULL);

	// init it
	tb_pstring_cstrcpy(&text->name, "#text");
	tb_pstring_cstrcpy(&text->value, data);

	return text;
}
tb_xml_node_t* tb_xml_document_init_cdata(tb_xml_document_t* document, tb_char_t const* data)
{
	tb_assert(document && data);
	if (!document || !data) return TB_NULL;

	// alloc cdata
	tb_xml_node_t* cdata = tb_xml_node_init(document, TB_XML_NODE_TYPE_CDATA);
	tb_assert_and_check_return_val(cdata, TB_NULL);

	// init it
	tb_pstring_cstrcpy(&cdata->name, "#cdata");
	tb_pstring_cstrcpy(&cdata->value, data);

	return cdata;
}
tb_xml_node_t* tb_xml_document_init_comment(tb_xml_document_t* document, tb_char_t const* data)
{
	tb_assert(document && data);
	if (!document || !data) return TB_NULL;

	// alloc comment
	tb_xml_node_t* comment = tb_xml_node_init(document, TB_XML_NODE_TYPE_COMMENT);
	tb_assert_and_check_return_val(comment, TB_NULL);

	// init it
	tb_pstring_cstrcpy(&comment->name, "#comment");
	tb_pstring_cstrcpy(&comment->value, data);

	return comment;
}
tb_xml_node_t* tb_xml_document_init_attribute(tb_xml_document_t* document, tb_char_t const* name)
{
	tb_assert(document && name);
	if (!document || !name) return TB_NULL;

	// alloc attribute
	tb_xml_node_t* attribute = tb_xml_node_init(document, TB_XML_NODE_TYPE_ATTRIBUTE);
	tb_assert_and_check_return_val(attribute, TB_NULL);

	// init it
	tb_pstring_cstrcpy(&attribute->name, name);

	return attribute;
}
