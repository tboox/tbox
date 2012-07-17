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
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init stream
	tb_gstream_t* gst = tb_gstream_init_from_url(argv[1]);
	if (gst && tb_gstream_bopen(gst))
	{
		// init reader
		tb_handle_t reader = tb_xml_reader_init(gst);
		if (reader)
		{
			// walk
			tb_size_t e = TB_XML_READER_EVENT_NONE;
			while (e = tb_xml_reader_next(reader))
			{
				switch (e)
				{
				case TB_XML_READER_EVENT_DOCUMENT_BEG: 
					{
					//	tb_printf("<?xml version = \"%s\" encoding = \"%s\" ?>\n"
					//		, tb_xml_reader_version(reader), tb_xml_reader_encoding(reader));
					}
					break;
				case TB_XML_READER_EVENT_DOCUMENT_END: 
					{
					}
					break;
				case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
					{
						tb_char_t const* 		name = tb_xml_reader_name(reader);
						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);
						if (!attr) tb_printf("\n<%s/>\n", name);
						else
						{
							tb_printf("\n<%s", name);
							for (; attr; attr = attr->next)
								tb_printf(" %s = \"%s\"", tb_pstring_cstr(&attr->name), tb_pstring_cstr(&attr->data));
							tb_printf("/>\n");
						}
					}
					break;
				case TB_XML_READER_EVENT_ELEMENT_BEG: 
					{
						tb_char_t const* 		name = tb_xml_reader_name(reader);
						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);
						if (!attr) tb_printf("\n<%s>", name);
						else
						{
							tb_printf("\n<%s", name);
							for (; attr; attr = attr->next)
								tb_printf(" %s = \"%s\"", tb_pstring_cstr(&attr->name), tb_pstring_cstr(&attr->data));
							tb_printf(">");
						}
					}
					break;
				case TB_XML_READER_EVENT_ELEMENT_END: 
					{
						tb_printf("</%s>\n", tb_xml_reader_name(reader));
					}
				break;
				case TB_XML_READER_EVENT_TEXT: 
					{
						tb_printf("%s", tb_xml_reader_text(reader));
					}
					break;
				case TB_XML_READER_EVENT_CDATA: 
					{
						tb_printf("<![CDATA[%s]]>\n", tb_xml_reader_cdata(reader));
					}
					break;
				case TB_XML_READER_EVENT_COMMENT: 
					{
						tb_printf("<!--%s-->\n", tb_xml_reader_comment(reader));
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

