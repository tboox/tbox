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
 * @file		reader.c
 * @ingroup 	xml
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 					"xml"
#define TB_TRACE_MODULE_DEBUG 					(0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "reader.h"
#include "../charset/charset.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef __tb_small__
# 	define TB_XML_READER_ATTRIBUTES_MAXN 		(64)
#else
# 	define TB_XML_READER_ATTRIBUTES_MAXN 		(128)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
// the xml reader type
typedef struct __tb_xml_reader_t
{
	// the event
	tb_size_t 				event;

	// the level
	tb_size_t 				level;
	
	// the stream
	tb_basic_stream_t* 		istream;
	tb_basic_stream_t* 		filter;
	tb_basic_stream_t* 		rstream;

	// the version
	tb_scoped_string_t 		version;

	// the charset
	tb_scoped_string_t 		charset;

	// the element
	tb_scoped_string_t 		element;

	// the element name
	tb_scoped_string_t 		name;

	// the text
	tb_scoped_string_t 		text;

	// the attributes
	tb_xml_attribute_t 		attributes[TB_XML_READER_ATTRIBUTES_MAXN];

}tb_xml_reader_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * parser implementation
 */
static tb_char_t const* tb_xml_reader_element_parse(tb_xml_reader_t* reader)
{
	// clear element
	tb_scoped_string_clear(&reader->element);

	// parse element
	tb_char_t ch = '\0';
	tb_size_t in = 0;
	while ((ch = tb_basic_stream_bread_s8(reader->rstream)))
	{
		// append element
		if (!in && ch == '<') in = 1;
		else if (in)
		{
			if (ch != '>') tb_scoped_string_chrcat(&reader->element, ch);
			else return tb_scoped_string_cstr(&reader->element);
		}
	}
	return tb_null;
}
static tb_char_t const* tb_xml_reader_text_parse(tb_xml_reader_t* reader)
{
	// clear text
	tb_scoped_string_clear(&reader->text);

	// parse text
	tb_char_t* pc = tb_null;
	while (tb_basic_stream_need(reader->rstream, (tb_byte_t**)&pc, 1) && pc)
	{
		// is end? </ ..>
		if (pc[0] == '<') return tb_scoped_string_cstr(&reader->text);
		else
		{
			tb_scoped_string_chrcat(&reader->text, *pc);
			if (!tb_basic_stream_skip(reader->rstream, 1)) return tb_null;
		}
	}
	return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_xml_reader_init(tb_basic_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// alloc
	tb_xml_reader_t* reader = (tb_xml_reader_t*)tb_malloc0(sizeof(tb_xml_reader_t));
	tb_assert_and_check_return_val(reader, tb_null);

	// init stream
	reader->istream = stream;
	reader->rstream = stream;

	// init string
	tb_scoped_string_init(&reader->version);
	tb_scoped_string_init(&reader->charset);
	tb_scoped_string_init(&reader->element);
	tb_scoped_string_init(&reader->name);
	tb_scoped_string_init(&reader->text);
	tb_scoped_string_cstrcpy(&reader->version, "2.0");
	tb_scoped_string_cstrcpy(&reader->charset, "utf-8");

	// init attributes
	tb_size_t i = 0;
	for (i = 0; i < TB_XML_READER_ATTRIBUTES_MAXN; i++)
	{
		tb_xml_node_t* node = (tb_xml_node_t*)(reader->attributes + i);
		tb_scoped_string_init(&node->name);
		tb_scoped_string_init(&node->data);
	}

	// ok
	return reader;
}
tb_void_t tb_xml_reader_exit(tb_handle_t reader)
{
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	if (xreader)
	{
		// exit stream
		if (xreader->filter) tb_basic_stream_exit(xreader->filter);

		// exit version
		tb_scoped_string_exit(&xreader->version);

		// exit charset
		tb_scoped_string_exit(&xreader->charset);

		// exit element
		tb_scoped_string_exit(&xreader->element);

		// exit name
		tb_scoped_string_exit(&xreader->name);

		// exit text
		tb_scoped_string_exit(&xreader->text);

		// exit attributes
		tb_long_t i = 0;
		for (i = 0; i < TB_XML_READER_ATTRIBUTES_MAXN; i++)
		{
			tb_xml_node_t* node = (tb_xml_node_t*)(xreader->attributes + i);
			tb_scoped_string_exit(&node->name);
			tb_scoped_string_exit(&node->data);
		}

		// free it
		tb_free(xreader);
	}
}
tb_void_t tb_xml_reader_clear(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return(xreader);

	// reinit level
	xreader->level = 0;

	// reset stream
	if (xreader->rstream) tb_basic_stream_seek(xreader->rstream, 0);

	// clear element
	tb_scoped_string_clear(&xreader->element);

	// clear name
	tb_scoped_string_clear(&xreader->name);

	// clear text
	tb_scoped_string_clear(&xreader->text);

	// clear attributes
	tb_long_t i = 0;
	for (i = 0; i < TB_XML_READER_ATTRIBUTES_MAXN; i++)
	{
		tb_xml_node_t* node = (tb_xml_node_t*)(xreader->attributes + i);
		tb_scoped_string_clear(&node->name);
		tb_scoped_string_clear(&node->data);
	}
}
tb_basic_stream_t* tb_xml_reader_stream(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader, tb_null);

	return xreader->rstream;
}
tb_size_t tb_xml_reader_level(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader, 0);

	return xreader->level;
}
tb_size_t tb_xml_reader_next(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->rstream, TB_XML_READER_EVENT_NONE);

	// reset event
	xreader->event = TB_XML_READER_EVENT_NONE;

	// next
	while (!xreader->event)
	{
		// peek character
		tb_char_t* pc = tb_null;
		if (!tb_basic_stream_need(xreader->rstream, (tb_byte_t**)&pc, 1) || !pc) break;

		// is element?
		if (*pc == '<') 
		{
			// parse element: <...>
			tb_char_t const* element = tb_xml_reader_element_parse(xreader);
			tb_assert_and_check_break(element);

			// is document begin: <?xml version="..." charset=".." ?>
			tb_size_t size = tb_scoped_string_size(&xreader->element);
			if (size > 4 && !tb_strnicmp(element, "?xml", 4))
			{
				// update event
				xreader->event = TB_XML_READER_EVENT_DOCUMENT;

				// remove ?xml, @note: overlap buffer
				tb_scoped_string_cstrncpy(&xreader->element, element + 4, size - 4);

				// update version & charset
				tb_xml_node_t* attr = (tb_xml_node_t*)tb_xml_reader_attributes(reader);	
				for (; attr; attr = attr->next)
				{
					if (!tb_scoped_string_cstricmp(&attr->name, "version")) tb_scoped_string_strcpy(&xreader->version, &attr->data);
					if (!tb_scoped_string_cstricmp(&attr->name, "encoding")) tb_scoped_string_strcpy(&xreader->charset, &attr->data);
				}

				// transform stream => utf-8
				if (tb_scoped_string_cstricmp(&xreader->charset, "utf-8") && tb_scoped_string_cstricmp(&xreader->charset, "utf8"))
				{
					// charset
					tb_size_t charset = TB_CHARSET_TYPE_UTF8;
					if (!tb_scoped_string_cstricmp(&xreader->charset, "gb2312") || !tb_scoped_string_cstricmp(&xreader->charset, "gbk")) 
						charset = TB_CHARSET_TYPE_GB2312;
					else tb_trace_e("the charset: %s is not supported", tb_scoped_string_cstr(&xreader->charset));

					// init transform stream
					if (charset != TB_CHARSET_TYPE_UTF8)
					{
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
						xreader->filter = tb_basic_stream_init_filter_from_charset(xreader->istream, charset, TB_CHARSET_TYPE_UTF8);
						if (xreader->filter && tb_basic_stream_open(xreader->filter))
							xreader->rstream = xreader->filter;
						tb_scoped_string_cstrcpy(&xreader->charset, "utf-8");
#else
						// trace
						tb_trace_e("unicode type is not supported, please enable charset module config if you want to use it!");
#endif
					}
				}
			}
			// is document type: <!DOCTYPE ... >
			else if (size > 8 && !tb_strnicmp(element, "!DOCTYPE", 8))
			{
				// update event
				xreader->event = TB_XML_READER_EVENT_DOCUMENT_TYPE;
			}
			// is element end: </name>
			else if (size > 1 && element[0] == '/')
			{
				// check
				tb_check_break(xreader->level);

				// update event
				xreader->event = TB_XML_READER_EVENT_ELEMENT_END;

				// leave
				xreader->level--;
			}
			// is comment: <!-- text -->
			else if (size >= 3 && !tb_strncmp(element, "!--", 3))
			{
				// no comment end?
				if (element[size - 2] != '-' || element[size - 1] != '-')
				{
					// patch '>'
					tb_scoped_string_chrcat(&xreader->element, '>');

					// seek to comment end
					tb_char_t ch = '\0';
					tb_int_t n = 0;
					while ((ch = tb_basic_stream_bread_s8(xreader->rstream)))
					{
						// -->
						if (n == 2 && ch == '>') break;
						else
						{
							// append it
							tb_scoped_string_chrcat(&xreader->element, ch);

							if (ch == '-') n++;
							else n = 0;
						}
					}

					// update event
					if (ch != '\0') xreader->event = TB_XML_READER_EVENT_COMMENT;
				}
				else xreader->event = TB_XML_READER_EVENT_COMMENT;
			}
			// is cdata: <![CDATA[ text ]]>
			else if (size >= 8 && !tb_strnicmp(element, "![CDATA[", 8))
			{
				if (element[size - 2] != ']' || element[size - 1] != ']')
				{
					// patch '>'
					tb_scoped_string_chrcat(&xreader->element, '>');

					// seek to cdata end
					tb_char_t ch = '\0';
					tb_int_t n = 0;
					while ((ch = tb_basic_stream_bread_s8(xreader->rstream)))
					{
						// ]]>
						if (n == 2 && ch == '>') break;
						else
						{
							// append it
							tb_scoped_string_chrcat(&xreader->element, ch);

							if (ch == ']') n++;
							else n = 0;
						}
					}

					// update event
					if (ch != '\0') xreader->event = TB_XML_READER_EVENT_CDATA;
				}
				else xreader->event = TB_XML_READER_EVENT_CDATA;
			}
			// is empty element: <name/>
			else if (size > 1 && element[size - 1] == '/')
			{
				// update event
				xreader->event = TB_XML_READER_EVENT_ELEMENT_EMPTY;
			}
			// is element begin: <name>
			else
			{
				// update event
				xreader->event = TB_XML_READER_EVENT_ELEMENT_BEG;

				// enter
				xreader->level++;
			}

			// trace
//			tb_trace_d("<%s>", element);
		}
		// is text: <> text </>
		else if (*pc)
		{
			// parse text: <> ... <>
			tb_char_t const* text = tb_xml_reader_text_parse(xreader);
			if (text && tb_scoped_string_cstrcmp(&xreader->text, "\r\n") && tb_scoped_string_cstrcmp(&xreader->text, "\n"))
				xreader->event = TB_XML_READER_EVENT_TEXT;

			// trace
//			tb_trace_d("%s", text);
		}
	}

	// ok?
	return xreader->event;
}
tb_bool_t tb_xml_reader_goto(tb_handle_t reader, tb_char_t const* path)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->rstream && path, tb_false);
	tb_trace_d("goto: %s", path);

	// reset
	xreader->level = 0;
	if (!tb_basic_stream_seek(xreader->rstream, 0)) return tb_false;

	// init
	tb_static_string_t 	s;
	tb_char_t 		data[8192];
	if (!tb_static_string_init(&s, data, 8192)) return tb_false;

	// walk
	tb_bool_t ok = tb_false;
	tb_size_t e = TB_XML_READER_EVENT_NONE;
	tb_hize_t save = tb_stream_offset(xreader->rstream);
	while (!ok && (e = tb_xml_reader_next(reader)))
	{
		switch (e)
		{
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);

				// append 
				tb_size_t n = tb_static_string_size(&s);
				tb_static_string_chrcat(&s, '/');
				tb_static_string_cstrcat(&s, name);

				// ok?
				if (!tb_static_string_cstricmp(&s, path)) ok = tb_true;
				tb_trace_d("path: %s", tb_static_string_cstr(&s));

				// remove 
				tb_static_string_strip(&s, n);

				// restore
				if (ok) if (!(ok = tb_basic_stream_seek(xreader->rstream, save))) goto end;
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				// name
				tb_char_t const* name = tb_xml_reader_element(reader);
				tb_assert_and_check_goto(name, end);

				// append 
				tb_static_string_chrcat(&s, '/');
				tb_static_string_cstrcat(&s, name);

				// ok?
				if (!tb_static_string_cstricmp(&s, path)) ok = tb_true;
				tb_trace_d("path: %s", tb_static_string_cstr(&s));

				// restore
				if (ok) if (!(ok = tb_basic_stream_seek(xreader->rstream, save))) goto end;
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				// remove 
				tb_long_t p = tb_static_string_strrchr(&s, 0, '/');
				if (p >= 0) tb_static_string_strip(&s, p);

				// ok?
				if (!tb_static_string_cstricmp(&s, path)) ok = tb_true;
				tb_trace_d("path: %s", tb_static_string_cstr(&s));

				// restore
				if (ok) if (!(ok = tb_basic_stream_seek(xreader->rstream, save))) goto end;
			}
			break;
		default:
			break;
		}

		// save
		save = tb_stream_offset(xreader->rstream);
	}

end:
	tb_static_string_exit(&s);
	xreader->level = 0;
	if (!ok) tb_basic_stream_seek(xreader->rstream, 0);
	return ok;
}
tb_xml_node_t* tb_xml_reader_load(tb_handle_t reader)
{
	// check
	tb_assert_and_check_return_val(reader, tb_null);

	// node
	tb_xml_node_t* 	node = tb_null;
	
	// walk
	tb_size_t e = TB_XML_READER_EVENT_NONE;
	while ((e = tb_xml_reader_next(reader)))
	{
		// init document node
		if (!node)
		{
			node = tb_xml_node_init_document(tb_xml_reader_version(reader), tb_xml_reader_charset(reader));
			tb_assert_and_check_goto(node && !node->parent, fail);
		}

		switch (e)
		{
		case TB_XML_READER_EVENT_DOCUMENT:
			break;
		case TB_XML_READER_EVENT_DOCUMENT_TYPE:
			{
				// init
				tb_xml_node_t* doctype = tb_xml_node_init_document_type(tb_xml_reader_doctype(reader));
				tb_assert_and_check_goto(doctype, fail);
				
				// append
				tb_xml_node_append_ctail(node, doctype); 
				tb_assert_and_check_goto(doctype->parent, fail);
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
			{
				// init
				tb_xml_node_t* element = tb_xml_node_init_element(tb_xml_reader_element(reader));
				tb_assert_and_check_goto(element, fail);
				
				// attributes
				tb_xml_node_t const* attr = tb_xml_reader_attributes(reader);
				for (; attr; attr = attr->next)
					tb_xml_node_append_atail(element, tb_xml_node_init_attribute(tb_scoped_string_cstr(&attr->name), tb_scoped_string_cstr(&attr->data)));
				
				// append
				tb_xml_node_append_ctail(node, element); 
				tb_assert_and_check_goto(element->parent, fail);
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_BEG: 
			{
				// init
				tb_xml_node_t* element = tb_xml_node_init_element(tb_xml_reader_element(reader));
				tb_assert_and_check_goto(element, fail);

				// attributes
				tb_xml_node_t const* attr = tb_xml_reader_attributes(reader);
				for (; attr; attr = attr->next)
					tb_xml_node_append_atail(element, tb_xml_node_init_attribute(tb_scoped_string_cstr(&attr->name), tb_scoped_string_cstr(&attr->data)));
				
				// append
				tb_xml_node_append_ctail(node, element); 
				tb_assert_and_check_goto(element->parent, fail);

				// enter
				node = element;
			}
			break;
		case TB_XML_READER_EVENT_ELEMENT_END: 
			{
				tb_assert_and_check_goto(node, fail);
				node = node->parent;
			}
			break;
		case TB_XML_READER_EVENT_TEXT: 
			{
				// init
				tb_xml_node_t* text = tb_xml_node_init_text(tb_xml_reader_text(reader));
				tb_assert_and_check_goto(text, fail);
				
				// append
				tb_xml_node_append_ctail(node, text); 
				tb_assert_and_check_goto(text->parent, fail);
			}
			break;
		case TB_XML_READER_EVENT_CDATA: 
			{
				// init
				tb_xml_node_t* cdata = tb_xml_node_init_cdata(tb_xml_reader_cdata(reader));
				tb_assert_and_check_goto(cdata, fail);
				
				// append
				tb_xml_node_append_ctail(node, cdata); 
				tb_assert_and_check_goto(cdata->parent, fail);

			}
			break;
		case TB_XML_READER_EVENT_COMMENT: 
			{
				// init
				tb_xml_node_t* comment = tb_xml_node_init_comment(tb_xml_reader_comment(reader));
				tb_assert_and_check_goto(comment, fail);
				
				// append
				tb_xml_node_append_ctail(node, comment); 
				tb_assert_and_check_goto(comment->parent, fail);
			}
			break;
		default:
			break;
		}
	}

	// ok
	return node;

fail:
	if (node) tb_xml_node_exit(node);
	return tb_null;
}
tb_char_t const* tb_xml_reader_version(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader, tb_null);

	// text
	return tb_scoped_string_cstr(&xreader->version);
}
tb_char_t const* tb_xml_reader_charset(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader, tb_null);

	// text
	return tb_scoped_string_cstr(&xreader->charset);
}
tb_char_t const* tb_xml_reader_comment(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->event == TB_XML_READER_EVENT_COMMENT, tb_null);

	// init
	tb_char_t const* 	p = tb_scoped_string_cstr(&xreader->element);
	tb_size_t 			n = tb_scoped_string_size(&xreader->element);
	tb_assert_and_check_return_val(p && n >= 6, tb_null);

	// comment
	tb_scoped_string_cstrncpy(&xreader->text, p + 3, n - 5);
	return tb_scoped_string_cstr(&xreader->text);
}
tb_char_t const* tb_xml_reader_cdata(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->event == TB_XML_READER_EVENT_CDATA, tb_null);

	// init
	tb_char_t const* 	p = tb_scoped_string_cstr(&xreader->element);
	tb_size_t 			n = tb_scoped_string_size(&xreader->element);
	tb_assert_and_check_return_val(p && n >= 11, tb_null);

	// comment
	tb_scoped_string_cstrncpy(&xreader->text, p + 8, n - 10);
	return tb_scoped_string_cstr(&xreader->text);
}
tb_char_t const* tb_xml_reader_text(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->event == TB_XML_READER_EVENT_TEXT, tb_null);

	// text
	return tb_scoped_string_cstr(&xreader->text);
}
tb_char_t const* tb_xml_reader_element(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && ( xreader->event == TB_XML_READER_EVENT_ELEMENT_BEG
											|| 	xreader->event == TB_XML_READER_EVENT_ELEMENT_END
											|| 	xreader->event == TB_XML_READER_EVENT_ELEMENT_EMPTY), tb_null);

	// init
	tb_char_t const* p = tb_null;
	tb_char_t const* b = tb_scoped_string_cstr(&xreader->element);
	tb_char_t const* e = b + tb_scoped_string_size(&xreader->element);
	tb_assert_and_check_return_val(b, tb_null);

	// </name> or <name ... />
	if (b < e && *b == '/') b++;
	for (p = b; p < e && *p && !tb_isspace(*p) && *p != '/'; p++) ;

	// ok?
	return p > b? tb_scoped_string_cstrncpy(&xreader->name, b, p - b) : tb_null;
}
tb_char_t const* tb_xml_reader_doctype(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->event == TB_XML_READER_EVENT_DOCUMENT_TYPE, tb_null);

	// doctype
	tb_char_t const* p = tb_scoped_string_cstr(&xreader->element);
	tb_assert_and_check_return_val(p, tb_null);

	// skip !DOCTYPE
	return (p + 9);
}
tb_xml_node_t const* tb_xml_reader_attributes(tb_handle_t reader)
{
	// check
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && ( xreader->event == TB_XML_READER_EVENT_DOCUMENT
											|| 	xreader->event == TB_XML_READER_EVENT_ELEMENT_BEG
											|| 	xreader->event == TB_XML_READER_EVENT_ELEMENT_END
											|| 	xreader->event == TB_XML_READER_EVENT_ELEMENT_EMPTY), tb_null);

	// init
	tb_char_t const* p = tb_scoped_string_cstr(&xreader->element);
	tb_char_t const* e = p + tb_scoped_string_size(&xreader->element);

	// init name & data
	tb_scoped_string_t name;
	tb_scoped_string_t data;
	tb_scoped_string_init(&name);
	tb_scoped_string_init(&data);

	// skip name
	while (p < e && *p && !tb_isspace(*p)) p++;
	while (p < e && *p && tb_isspace(*p)) p++;

	// parse attributes
	tb_size_t n = 0;
	while (p < e)
	{
		// parse name
		tb_scoped_string_clear(&name);
		for (; p < e && *p != '='; p++) if (!tb_isspace(*p)) tb_scoped_string_chrcat(&name, *p);
		if (*p != '=') break;

		// parse data
		tb_scoped_string_clear(&data);
		for (p++; p < e && (*p != '\'' && *p != '\"'); p++) ;
		if (*p != '\'' && *p != '\"') break;
		for (p++; p < e && (*p != '\'' && *p != '\"'); p++) tb_scoped_string_chrcat(&data, *p);
		if (*p != '\'' && *p != '\"') break;
		p++;

		// append node
		if (tb_scoped_string_cstr(&name) && tb_scoped_string_cstr(&data))
		{
			// node
			tb_xml_node_t* prev = n > 0? (tb_xml_node_t*)&xreader->attributes[n - 1] : tb_null;
			tb_xml_node_t* node = (tb_xml_node_t*)&xreader->attributes[n];

			// init node
			tb_scoped_string_strcpy(&node->name, &name);
			tb_scoped_string_strcpy(&node->data, &data);

			// append node
			if (prev) prev->next = node;
			node->next = tb_null;

			// next
			n++;
		}
	}

	// exit name & data
	tb_scoped_string_exit(&name);
	tb_scoped_string_exit(&data);

	// ok?
	return n? (tb_xml_node_t*)&xreader->attributes[0] : tb_null;
}
