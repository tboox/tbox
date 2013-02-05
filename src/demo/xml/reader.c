/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init stream
	tb_gstream_t* gst = tb_gstream_init_from_url(argv[1]);
	if (gst && tb_gstream_bopen(gst))
	{
		// init reader
		tb_handle_t reader = tb_xml_reader_init(gst);
		if (reader)
		{
			// goto
			tb_bool_t ok = tb_true;
			if (argv[2]) ok = tb_xml_reader_goto(reader, argv[2]);

			// walk
			tb_size_t e = TB_XML_READER_EVENT_NONE;
			while (ok && (e = tb_xml_reader_next(reader)))
			{
				switch (e)
				{
				case TB_XML_READER_EVENT_DOCUMENT: 
					{
						tb_printf("<?xml version = \"%s\" encoding = \"%s\" ?>\n"
							, tb_xml_reader_version(reader), tb_xml_reader_encoding(reader));
					}
					break;
				case TB_XML_READER_EVENT_DOCUMENT_TYPE: 
					{
						tb_printf("<!DOCTYPE>\n");
					}
					break;
				case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
					{
						tb_char_t const* 		name = tb_xml_reader_element(reader);
						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);
						tb_size_t 				t = tb_xml_reader_level(reader);
						while (t--) tb_printf("\t");
						if (!attr) tb_printf("<%s/>\n", name);
						else
						{
							tb_printf("<%s", name);
							for (; attr; attr = attr->next)
								tb_printf(" %s = \"%s\"", tb_pstring_cstr(&attr->name), tb_pstring_cstr(&attr->data));
							tb_printf("/>\n");
						}
					}
					break;
				case TB_XML_READER_EVENT_ELEMENT_BEG: 
					{
						tb_char_t const* 		name = tb_xml_reader_element(reader);
						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);	
						tb_size_t 				t = tb_xml_reader_level(reader) - 1;
						while (t--) tb_printf("\t");
						if (!attr) tb_printf("<%s>\n", name);
						else
						{
							tb_printf("<%s", name);
							for (; attr; attr = attr->next)
								tb_printf(" %s = \"%s\"", tb_pstring_cstr(&attr->name), tb_pstring_cstr(&attr->data));
							tb_printf(">\n");
						}
					}
					break;
				case TB_XML_READER_EVENT_ELEMENT_END: 
					{
						tb_size_t t = tb_xml_reader_level(reader);
						while (t--) tb_printf("\t");
						tb_printf("</%s>\n", tb_xml_reader_element(reader));
					}
					break;
				case TB_XML_READER_EVENT_TEXT: 
					{
						tb_size_t t = tb_xml_reader_level(reader);
						while (t--) tb_printf("\t");
						tb_printf("%s", tb_xml_reader_text(reader));
						tb_printf("\n");
					}
					break;
				case TB_XML_READER_EVENT_CDATA: 
					{
						tb_size_t t = tb_xml_reader_level(reader);
						while (t--) tb_printf("\t");
						tb_printf("<![CDATA[%s]]>", tb_xml_reader_cdata(reader));
						tb_printf("\n");
					}
					break;
				case TB_XML_READER_EVENT_COMMENT: 
					{
						tb_size_t t = tb_xml_reader_level(reader);
						while (t--) tb_printf("\t");
						tb_printf("<!--%s-->", tb_xml_reader_comment(reader));
						tb_printf("\n");
					}
					break;
				default:
					break;
				}
			}

			// exit reader
			tb_xml_reader_exit(reader);
		}
	
		// exit stream
		tb_gstream_exit(gst);
	}
	
	// exit tbox
	tb_exit();
	return 0;
}

