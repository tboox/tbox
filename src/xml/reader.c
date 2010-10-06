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
 * \file		reader.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "reader.h"


/* /////////////////////////////////////////////////////////
 * macros
 */
#ifndef TPLAT_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
#if 1
# 	define TB_READER_DBG(fmt, arg...) 			TB_DBG("[xml]: " fmt, ##arg)
#else
# 	define TB_READER_DBG(fmt, arg...)
#endif

#else
# 	define TB_READER_DBG
#endif


/* /////////////////////////////////////////////////////////
 * details
 */

static tb_char_t tb_xml_reader_get_char(tb_xml_reader_t* reader)
{
	// get character from cache
	if (reader->cache != '\0') 
	{
		tb_char_t ch = reader->cache;
		reader->cache = '\0';
		return ch;
	}

	// get character
	tb_char_t ch[1];
	if (1 == tb_stream_read(reader->st, ch, 1)) return ch[0];
	else return '\0';
}
static tb_char_t tb_xml_reader_peek_char(tb_xml_reader_t* reader)
{
	// get character from cache
	if (reader->cache != '\0') return reader->cache;

	// map character 
	tb_char_t const* p = tb_stream_need(reader->st, 1);

	// get character
	if (p) return *p;
	else return '\0';
}
static void tb_xml_reader_seek_char(tb_xml_reader_t* reader)
{
	tb_stream_seek(reader->st, 1, TB_STREAM_SEEK_CUR);
}
static tb_char_t const* tb_xml_reader_parse_element(tb_xml_reader_t* reader)
{
	// clear element
	tb_string_clear(&reader->element);

	// parse element
	tb_char_t ch = '\0';
	tb_size_t in = 0;
	while (ch = tb_xml_reader_get_char(reader))
	{
		// appedn element
		if (!in && ch == '<') in = 1;
		else if (in)
		{
			if (ch != '>') tb_string_append_char(&reader->element, ch);
			else return tb_string_c_string(&reader->element);
		}
	}
	return TB_NULL;
}
static tb_char_t const* tb_xml_reader_parse_text(tb_xml_reader_t* reader)
{
	// clear text
	tb_string_clear(&reader->text);

	// parse text
	tb_char_t ch = '\0';
	tb_size_t in = 0;
	while (ch = tb_xml_reader_get_char(reader))
	{
		// append text
		if (ch != '<') tb_string_append_char(&reader->text, ch);
		else 
		{
			reader->cache = ch;
			return tb_string_c_string(&reader->text);
		}
	}
	return TB_NULL;
}

/* /////////////////////////////////////////////////////////
 * interfaces: open & close
 */

tb_xml_reader_t* tb_xml_reader_open(tb_stream_t* st)
{
	TB_ASSERT(st);
	if (!st) return TB_NULL;

	// alloc reader
	tb_xml_reader_t* reader = (tb_xml_reader_t*)tb_malloc(sizeof(tb_xml_reader_t));
	if (!reader) return TB_NULL;

	// init it
	memset(reader, 0, sizeof(tb_xml_reader_t));
	reader->st = st;
	reader->event = TB_XML_READER_EVENT_NULL;
	reader->cache = '\0';

	// init string
	tb_string_init(&reader->version);
	tb_string_init(&reader->encoding);
	tb_string_init(&reader->element);
	tb_string_init(&reader->name);
	tb_string_init(&reader->text);

	// init attributes
	tb_int_t i = 0;
	reader->attributes_n = 0;
	for (i = 0; i < TB_XML_READER_ATTRIBUTES_MAX; i++)
	{
		tb_xml_node_t* node = (tb_xml_node_t*)(reader->attributes + i);
		tb_string_init(&node->name);
		tb_string_init(&node->value);
	}

	// reader the first event
	tb_xml_reader_next(reader);

	return reader;
}

void tb_xml_reader_close(tb_xml_reader_t* reader)
{
	if (reader)
	{
		// detach stream
		reader->st = TB_NULL;

		// free element
		tb_string_uninit(&reader->name);
	
		// free element
		tb_string_uninit(&reader->element);
	
		// free text
		tb_string_uninit(&reader->text);

		// free element
		tb_string_uninit(&reader->version);
	
		// free text
		tb_string_uninit(&reader->encoding);

		// free attributes
		tb_int_t i = 0;
		for (i = 0; i < TB_XML_READER_ATTRIBUTES_MAX; i++)
		{
			tb_xml_node_t* node = (tb_xml_node_t*)(reader->attributes + i);
			tb_string_uninit(&node->name);
			tb_string_uninit(&node->value);
		}

		// free it
		tb_free(reader);
	}
}

/* /////////////////////////////////////////////////////////
 * interfaces: iterator
 */
tb_bool_t tb_xml_reader_has_next(tb_xml_reader_t* reader)
{
	if (reader && reader->event != TB_XML_READER_EVENT_NULL)
		return TB_TRUE;
	else return TB_FALSE;
}
tb_size_t tb_xml_reader_next(tb_xml_reader_t* reader)
{
	TB_ASSERT(reader);
	if (!reader) return TB_XML_READER_EVENT_NULL;

	// reset event
	reader->event = TB_XML_READER_EVENT_NULL;

	// peek a character
	tb_char_t ch = tb_xml_reader_peek_char(reader);

	// is element?
	if (ch == '<') 
	{
		// parse element: <...>
		tb_char_t const* element = tb_xml_reader_parse_element(reader);
		if (!element) goto fail;

		// is document begin: <?xml version="..." encoding=".." ?>
		tb_size_t size = tb_string_size(&reader->element);
		if (size > 4 
			&& element[0] == '?'
			&& element[1] == 'x'
			&& element[2] == 'm'
			&& element[3] == 'l')
		{
			// update event
			reader->event = TB_XML_READER_EVENT_DOCUMENT_BEG;

			// parse attributes
			tb_size_t n = tb_xml_reader_get_attribute_count(reader);
			TB_ASSERT(n);
			if (!n) goto fail;

			// get version & encoding
			tb_string_t const* version = tb_xml_reader_get_attribute_value_by_name(reader, "version");
			tb_string_t const* encoding = tb_xml_reader_get_attribute_value_by_name(reader, "encoding");
			TB_ASSERT(version && encoding);
			if (!version || !encoding) goto fail;

			// only support utf-8 encoding now.
			if (TB_FALSE == tb_string_compare_c_string_nocase(encoding, "utf-8"))
			{
				TB_DBG("[warning]: only support xml encoding: utf-8.");
				goto fail;
			}

			// save version & encoding
			tb_string_assign(&reader->version, version);
			tb_string_assign(&reader->encoding, encoding);

			// seek to the first element
			tb_xml_reader_parse_text(reader);
		}
		// is element end: </name>
		else if (size > 1 && element[0] == '/')
		{
			// update event
			reader->event = TB_XML_READER_EVENT_ELEMENT_END;
		}
		// is comment: <!-- text -->
		else if (size >= 5
			&& element[0] == '!'
			&& element[1] == '-' 
			&& element[2] == '-'
			&& element[size - 2] == '-'
			&& element[size - 1] == '-')
		{
			// update event
			reader->event = TB_XML_READER_EVENT_COMMENT;
		}
		// is cdata: <![CDATA[ text ]]>
		else if (size >= 10
			&& element[0] == '!'
			&& element[1] == '[' 
			&& element[2] == 'C' 
			&& element[3] == 'D'
			&& element[4] == 'A' 
			&& element[5] == 'T' 
			&& element[6] == 'A'
			&& element[7] == '['
			&& element[size - 2] == ']' 
			&& element[size - 1] == ']')
		{
			// update event
			reader->event = TB_XML_READER_EVENT_CDATA;
		}
		// is element begin: <name>
		else
		{
			// update event
			reader->event = TB_XML_READER_EVENT_ELEMENT_BEG;
		}

		//TB_READER_DBG("element: %s", element);
	}
	// is text: <> text </>
	else
	{
		// parse text: <> ... <>
		tb_char_t const* text = tb_xml_reader_parse_text(reader);
		if (text) reader->event = TB_XML_READER_EVENT_CHARACTERS;

		//TB_READER_DBG("text: %s", text);
	}

	return reader->event;
fail:
	reader->event = TB_XML_READER_EVENT_NULL;
	return reader->event;
}


/* /////////////////////////////////////////////////////////
 * interfaces: getter
 */
tb_size_t tb_xml_reader_get_event(tb_xml_reader_t* reader)
{
	if (reader) return reader->event;
	else return TB_XML_READER_EVENT_NULL;
}
tb_string_t const* tb_xml_reader_get_version(tb_xml_reader_t* reader)
{
	if (reader) return &reader->version;
	else return TB_NULL;
}
tb_string_t const* tb_xml_reader_get_encoding(tb_xml_reader_t* reader)
{
	if (reader) return &reader->encoding;
	else return TB_NULL;
}
tb_string_t const* tb_xml_reader_get_comment_text(tb_xml_reader_t* reader)
{
	TB_ASSERT(reader);
	if (!reader) return TB_NULL;

	// check event
	TB_ASSERT(reader->event == TB_XML_READER_EVENT_COMMENT);
	if (reader->event != TB_XML_READER_EVENT_COMMENT) return TB_NULL;

	// parse comment
	tb_char_t const* 	p = tb_string_c_string(&reader->element);
	tb_size_t 			n = tb_string_size(&reader->element);
	if (!p || n < 6) return TB_NULL;

	tb_string_assign_c_string_with_size(&reader->text, p + 3, n - 5);
	return &reader->text;
}
tb_string_t const* tb_xml_reader_get_cdata_text(tb_xml_reader_t* reader)
{
	TB_ASSERT(reader);
	if (!reader) return TB_NULL;

	// check event
	TB_ASSERT(reader->event == TB_XML_READER_EVENT_CDATA);
	if (reader->event != TB_XML_READER_EVENT_CDATA) return TB_NULL;

	// parse comment
	tb_char_t const* 	p = tb_string_c_string(&reader->element);
	tb_size_t 			n = tb_string_size(&reader->element);
	if (!p || n < 11) return TB_NULL;

	tb_string_assign_c_string_with_size(&reader->text, p + 8, n - 10);
	return &reader->text;
}
tb_string_t const* tb_xml_reader_get_characters_text(tb_xml_reader_t* reader)
{
	TB_ASSERT(reader);
	if (!reader) return TB_NULL;

	// check event
	TB_ASSERT(reader->event == TB_XML_READER_EVENT_CHARACTERS);
	if (reader->event != TB_XML_READER_EVENT_CHARACTERS) return TB_NULL;

	return &reader->text;
}
tb_string_t const* tb_xml_reader_get_element_name(tb_xml_reader_t* reader)
{
	TB_ASSERT(reader);
	TB_ASSERT(TB_FALSE == tb_string_is_null(&reader->element));

	// check reader
	if (!reader) return TB_NULL;

	// check event
	TB_ASSERT( 	reader->event == TB_XML_READER_EVENT_ELEMENT_BEG
			|| 	reader->event == TB_XML_READER_EVENT_ELEMENT_END);

	if ( 	reader->event != TB_XML_READER_EVENT_ELEMENT_BEG 
		&& 	reader->event != TB_XML_READER_EVENT_ELEMENT_END)
		return TB_NULL;

	// parse the element name
	tb_char_t const* element = tb_string_c_string(&reader->element);
	if (element[0] == '/')
	{
		tb_string_assign_c_string_with_size_by_ref(&reader->name, element + 1, tb_string_size(&reader->element) - 1);
		return &reader->name;
	}
	else
	{
		// find the end position of the element
		tb_char_t const* p = element;
		tb_char_t const* e = p + tb_string_size(&reader->element);
		while (p < e && *p && !isspace(*p)) p++;

		if (p > element) 
		{
			tb_string_assign_c_string_with_size(&reader->name, element, p - element);
			return &reader->name;
		}
	}
	return TB_NULL;
}
tb_size_t tb_xml_reader_get_attribute_count(tb_xml_reader_t* reader)
{
	TB_ASSERT(reader);
	TB_ASSERT(TB_FALSE == tb_string_is_null(&reader->element));

	// check reader
	if (!reader) return 0;

	// check event
	TB_ASSERT( 	reader->event == TB_XML_READER_EVENT_ELEMENT_BEG
			|| 	reader->event == TB_XML_READER_EVENT_DOCUMENT_BEG);

	if ( 	reader->event != TB_XML_READER_EVENT_ELEMENT_BEG 
		&& 	reader->event != TB_XML_READER_EVENT_DOCUMENT_BEG)
		return 0;

	// clear attributes
	reader->attributes_n = 0;

	// get element
	tb_char_t const* p = tb_string_c_string(&reader->element);
	tb_char_t const* e = p + tb_string_size(&reader->element);

	// parser attributes
	tb_int_t is_key = 1;
	tb_string_t s;
	tb_string_init(&s);
	for (; p < e && *p; p++)
	{
		tb_char_t ch = *p;
		if (isspace(ch)|| ch == '=' || p == e - 1)
		{
			tb_xml_node_t* node = (tb_xml_node_t*)&reader->attributes[reader->attributes_n];
			if (is_key) tb_string_assign(&node->name, &s);
			else
			{
				tb_string_assign(&node->value, &s);
				reader->attributes_n++;
				is_key = 1;
			}
			tb_string_clear(&s);
			if (ch == '=') is_key = 0;
		}
		else if (ch != '\"' && ch != '\'') tb_string_append_char(&s, ch);
	}
	tb_string_uninit(&s);

	return reader->attributes_n;
}
tb_string_t const* tb_xml_reader_get_attribute_name(tb_xml_reader_t* reader, tb_int_t index)
{
	TB_ASSERT(reader && index < reader->attributes_n);
	if (!reader || index >= reader->attributes_n) return TB_NULL;

	return &reader->attributes[index].base.name;
}
tb_string_t const* tb_xml_reader_get_attribute_value_by_index(tb_xml_reader_t* reader, tb_int_t index)
{
	TB_ASSERT(reader && index < reader->attributes_n);
	if (!reader || index >= reader->attributes_n) return TB_NULL;

	return &reader->attributes[index].base.value;
}
tb_string_t const* tb_xml_reader_get_attribute_value_by_name(tb_xml_reader_t* reader, tb_char_t const* name)
{
	TB_ASSERT(reader && reader->attributes_n);
	if (!reader || !reader->attributes_n) return TB_NULL;

	tb_int_t i = 0;
	for (i = 0; i < reader->attributes_n; i++)
	{
		if (tb_string_compare_c_string(&reader->attributes[i].base.name, name) == TB_TRUE)
			return &reader->attributes[i].base.value;
	}
	return TB_NULL;
}

