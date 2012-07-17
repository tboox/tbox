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
					//	tb_print("<?xml version = \"%s\" encoding = \"%s\" ?>"
					//		, tb_xml_reader_version(reader), tb_xml_reader_encoding(reader));
					}
					break;
				case TB_XML_READER_EVENT_DOCUMENT_END: 
					{
						//tb_print();
					}
					break;
				case TB_XML_READER_EVENT_TEXT: 
					{
						tb_print("%s", tb_xml_reader_text(reader));
					}
					break;
				case TB_XML_READER_EVENT_CDATA: 
					{
						tb_print("<![CDATA[%s]]>", tb_xml_reader_cdata(reader));
					}
					break;
				case TB_XML_READER_EVENT_COMMENT: 
					{
						tb_print("<!--%s-->", tb_xml_reader_comment(reader));
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

