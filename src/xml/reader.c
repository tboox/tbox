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
 * @file		reader.c
 * @ingroup 	xml
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 		"xml"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "reader.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_XML_READER_ATTRIBUTES_MAX 		(256)
#else
# 	define TB_XML_READER_ATTRIBUTES_MAX 		(512)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the xml reader type
typedef struct __tb_xml_reader_t
{
	// the event
	tb_size_t 				event;
	
	// the stream
	tb_gstream_t* 			istream;
	tb_gstream_t* 			tstream;
	tb_gstream_t* 			rstream;

	// the version
	tb_pstring_t 			version;

	// the encoding
	tb_pstring_t 			encoding;

	// the element
	tb_pstring_t 			element;

	// the element name
	tb_pstring_t 			name;

	// the text
	tb_pstring_t 			text;

}tb_xml_reader_t;

/* ///////////////////////////////////////////////////////////////////////
 * parser
 */

static tb_char_t const* tb_xml_reader_element_parse(tb_xml_reader_t* reader)
{
	// clear element
	tb_pstring_clear(&reader->element);

	// parse element
	tb_char_t ch = '\0';
	tb_size_t in = 0;
	while (ch = tb_gstream_bread_s8(reader->rstream))
	{
		// append element
		if (!in && ch == '<') in = 1;
		else if (in)
		{
			if (ch != '>') tb_pstring_chrcat(&reader->element, ch);
			else return tb_pstring_cstr(&reader->element);
		}
	}
	return TB_NULL;
}
static tb_char_t const* tb_xml_reader_text_parse(tb_xml_reader_t* reader)
{
	// clear text
	tb_pstring_clear(&reader->text);

	// parse text
	tb_char_t* pc = TB_NULL;
	while (tb_gstream_bneed(reader->rstream, &pc, 1) && pc)
	{
		// append text
		if (*pc != '<') 
		{
			tb_pstring_chrcat(&reader->text, *pc);
			if (!tb_gstream_bskip(reader->rstream, 1)) return TB_NULL;
		}
		else return tb_pstring_cstr(&reader->text);
	}
	return TB_NULL;
}


/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_xml_reader_init(tb_gstream_t* gst)
{
	// check
	tb_assert_and_check_return_val(gst, TB_NULL);

	// alloc
	tb_xml_reader_t* reader = (tb_xml_reader_t*)tb_malloc0(sizeof(tb_xml_reader_t));
	tb_assert_and_check_return_val(reader, TB_NULL);

	// init
	reader->istream = gst;
	reader->rstream = gst;
	tb_pstring_init(&reader->version);
	tb_pstring_init(&reader->encoding);
	tb_pstring_init(&reader->element);
	tb_pstring_init(&reader->name);
	tb_pstring_init(&reader->text);
	tb_pstring_cstrcpy(&reader->version, "2.0");
	tb_pstring_cstrcpy(&reader->encoding, "utf-8");

	// ok
	return reader;
}

tb_void_t tb_xml_reader_exit(tb_handle_t reader)
{
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	if (xreader)
	{
		// exit stream
		if (xreader->tstream) tb_gstream_exit(xreader->tstream);

		// exit version
		tb_pstring_exit(&xreader->version);

		// exit encoding
		tb_pstring_exit(&xreader->encoding);

		// exit element
		tb_pstring_exit(&xreader->element);

		// exit name
		tb_pstring_exit(&xreader->name);

		// exit text
		tb_pstring_exit(&xreader->text);

		// free it
		tb_free(xreader);
	}
}
tb_size_t tb_xml_reader_next(tb_handle_t reader)
{
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->rstream, TB_XML_READER_EVENT_NONE);

	// reset event
	xreader->event = TB_XML_READER_EVENT_NONE;

	// peek character
	tb_char_t* pc = TB_NULL;
	if (!tb_gstream_bneed(xreader->rstream, &pc, 1) || !pc) goto end;

	// is element?
	if (*pc == '<') 
	{
		// parse element: <...>
		tb_char_t const* element = tb_xml_reader_element_parse(xreader);
		tb_assert_and_check_goto(element, end);

		// is document begin: <?xml version="..." encoding=".." ?>
		tb_size_t size = tb_pstring_size(&xreader->element);
		if (size > 4 
			&& element[0] == '?'
			&& element[1] == 'x'
			&& element[2] == 'm'
			&& element[3] == 'l')
		{
			// update event
			xreader->event = TB_XML_READER_EVENT_DOCUMENT_BEG;

		}
		// is element end: </name>
		else if (size > 1 && element[0] == '/')
		{
			// update event
			xreader->event = TB_XML_READER_EVENT_ELEMENT_END;
		}
		// is comment: <!-- text -->
		else if (size >= 3
			&& element[0] == '!'
			&& element[1] == '-' 
			&& element[2] == '-')
		{
			// no comment end?
			if (element[size - 2] != '-' || element[size - 1] != '-')
			{
				// patch '>'
				tb_pstring_chrcat(&xreader->element, '>');

				// seek to comment end
				tb_char_t ch = '\0';
				tb_int_t n = 0;
				while (ch = tb_gstream_bread_s8(xreader->rstream))
				{
					// -->
					if (n == 2 && ch == '>') break;
					else
					{
						// append it
						tb_pstring_chrcat(&xreader->element, ch);

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
		else if (size >= 8
			&& element[0] == '!'
			&& element[1] == '[' 
			&& element[2] == 'C' 
			&& element[3] == 'D'
			&& element[4] == 'A' 
			&& element[5] == 'T' 
			&& element[6] == 'A'
			&& element[7] == '[')
		{
			if (element[size - 2] != ']' || element[size - 1] != ']')
			{
				// patch '>'
				tb_pstring_chrcat(&xreader->element, '>');

				// seek to cdata end
				tb_char_t ch = '\0';
				tb_int_t n = 0;
				while (ch = tb_gstream_bread_s8(xreader->rstream))
				{
					// ]]>
					if (n == 2 && ch == '>') break;
					else
					{
						// append it
						tb_pstring_chrcat(&xreader->element, ch);

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
		}

		// trace
//		tb_trace_impl("<%s>", element);
	}
	// is text: <> text </>
	else if (*pc)
	{
		// parse text: <> ... <>
		tb_char_t const* text = tb_xml_reader_text_parse(xreader);
		if (text && tb_pstring_cstrcmp(&xreader->text, "\r\n") && tb_pstring_cstrcmp(&xreader->text, "\n"))
			xreader->event = TB_XML_READER_EVENT_TEXT;

		// trace
//		tb_trace_impl("%s", text);
	}

end:
	// ok?
	return xreader->event;
}
tb_bool_t tb_xml_reader_goto(tb_handle_t reader, tb_char_t const* path)
{
	return TB_FALSE;
}
tb_char_t const* tb_xml_reader_comment(tb_handle_t reader)
{
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->event == TB_XML_READER_EVENT_COMMENT, TB_NULL);

	// init
	tb_char_t const* 	p = tb_pstring_cstr(&xreader->element);
	tb_size_t 			n = tb_pstring_size(&xreader->element);
	tb_assert_and_check_return_val(p && n >= 6, TB_NULL);

	// comment
	tb_pstring_cstrncpy(&xreader->text, p + 3, n - 5);
	return tb_pstring_cstr(&xreader->text);
}
tb_char_t const* tb_xml_reader_cdata(tb_handle_t reader)
{
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->event == TB_XML_READER_EVENT_CDATA, TB_NULL);

	// init
	tb_char_t const* 	p = tb_pstring_cstr(&xreader->element);
	tb_size_t 			n = tb_pstring_size(&xreader->element);
	tb_assert_and_check_return_val(p && n >= 11, TB_NULL);

	// comment
	tb_pstring_cstrncpy(&xreader->text, p + 8, n - 10);
	return tb_pstring_cstr(&xreader->text);
}
tb_char_t const* tb_xml_reader_text(tb_handle_t reader)
{
	tb_xml_reader_t* xreader = (tb_xml_reader_t*)reader;
	tb_assert_and_check_return_val(xreader && xreader->event == TB_XML_READER_EVENT_TEXT, TB_NULL);

	// text
	return tb_pstring_cstr(&xreader->text);
}
