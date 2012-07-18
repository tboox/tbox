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
			// walk
			tb_size_t d = 0;
			tb_size_t e = TB_XML_READER_EVENT_NONE;
			while (e = tb_xml_reader_next(reader))
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
						tb_char_t const* 		name = tb_xml_reader_name(reader);
						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);
						tb_size_t 				t = d;
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
						tb_char_t const* 		name = tb_xml_reader_name(reader);
						tb_xml_node_t const* 	attr = tb_xml_reader_attributes(reader);	
						tb_size_t 				t = d;
						while (t--) tb_printf("\t");
						if (!attr) tb_printf("<%s>\n", name);
						else
						{
							tb_printf("<%s", name);
							for (; attr; attr = attr->next)
								tb_printf(" %s = \"%s\"", tb_pstring_cstr(&attr->name), tb_pstring_cstr(&attr->data));
							tb_printf(">\n");
						}
						d++;
						
						// for html
						if (!tb_stricmp(name, "hr")) d--;
						if (!tb_stricmp(name, "br")) d--;
						if (!tb_stricmp(name, "img")) d--;
						if (!tb_stricmp(name, "meta")) d--;
						if (!tb_stricmp(name, "base")) d--;
						if (!tb_stricmp(name, "link")) d--;
						if (!tb_stricmp(name, "area")) d--;
						if (!tb_stricmp(name, "input")) d--;
						if (!tb_stricmp(name, "param")) d--;
						if (!tb_stricmp(name, "script") || !tb_stricmp(name, "style"))
						{
							tb_gstream_t* gst = tb_xml_reader_stream(reader);
							if (gst)
							{
								tb_pstring_t s;
								tb_pstring_init(&s);
								tb_char_t* pc = TB_NULL;
								while (tb_gstream_bneed(gst, &pc, 9) && pc)
								{
									// is end? </ ..>
									if (!tb_strncmp(pc, "</script>", 9) || !tb_strncmp(pc, "</style>", 8)) break;
									else
									{
										tb_pstring_chrcat(&s, *pc);
										if (!tb_gstream_bskip(gst, 1)) break;
									}
								}
								if (tb_pstring_cstr(&s))
								{
									tb_size_t t = d;
									while (t--) tb_printf("\t");
									tb_printf("%s", tb_pstring_cstr(&s));
									tb_printf("\n");
								}
								tb_pstring_exit(&s);
							}
						}
					}
					break;
				case TB_XML_READER_EVENT_ELEMENT_END: 
					{
						tb_size_t t = --d;
						while (t--) tb_printf("\t");
						tb_printf("</%s>\n", tb_xml_reader_name(reader));
					}
					break;
				case TB_XML_READER_EVENT_TEXT: 
					{
						tb_size_t t = d;
						while (t--) tb_printf("\t");
						tb_printf("%s", tb_xml_reader_text(reader));
						tb_printf("\n");
					}
					break;
				case TB_XML_READER_EVENT_CDATA: 
					{
						tb_size_t t = d;
						while (t--) tb_printf("\t");
						tb_printf("<![CDATA[%s]]>", tb_xml_reader_cdata(reader));
						tb_printf("\n");
					}
					break;
				case TB_XML_READER_EVENT_COMMENT: 
					{
						tb_size_t t = d;
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

