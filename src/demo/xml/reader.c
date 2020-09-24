/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_xml_reader_main(tb_int_t argc, tb_char_t** argv)
{
    // init reader
    tb_xml_reader_ref_t reader = tb_xml_reader_init();
    if (reader)
    {
        // open reader
        if (tb_xml_reader_open(reader, tb_stream_init_from_url(argv[1]), tb_true))
        {
            // goto
            tb_bool_t ok = tb_true;
            if (argv[2]) ok = tb_xml_reader_goto(reader, argv[2]);

            // walk
            tb_size_t event = TB_XML_READER_EVENT_NONE;
            while (ok && (event = tb_xml_reader_next(reader)))
            {
                switch (event)
                {
                case TB_XML_READER_EVENT_DOCUMENT:
                    {
                        tb_printf("<?xml version = \"%s\" encoding = \"%s\" ?>\n"
                            , tb_xml_reader_version(reader), tb_xml_reader_charset(reader));
                    }
                    break;
                case TB_XML_READER_EVENT_DOCUMENT_TYPE:
                    {
                        tb_printf("<!DOCTYPE>\n");
                    }
                    break;
                case TB_XML_READER_EVENT_ELEMENT_EMPTY:
                    {
                        tb_char_t const*    name = tb_xml_reader_element(reader);
                        tb_xml_node_ref_t   attr = tb_xml_reader_attributes(reader);
                        tb_size_t           t = tb_xml_reader_level(reader);
                        while (t--) tb_printf("\t");
                        if (!attr) tb_printf("<%s/>\n", name);
                        else
                        {
                            tb_printf("<%s", name);
                            for (; attr; attr = attr->next)
                                tb_printf(" %s = \"%s\"", tb_string_cstr(&attr->name), tb_string_cstr(&attr->data));
                            tb_printf("/>\n");
                        }
                    }
                    break;
                case TB_XML_READER_EVENT_ELEMENT_BEG:
                    {
                        tb_char_t const*    name = tb_xml_reader_element(reader);
                        tb_xml_node_ref_t   attr = tb_xml_reader_attributes(reader);
                        tb_size_t           t = tb_xml_reader_level(reader) - 1;
                        while (t--) tb_printf("\t");
                        if (!attr) tb_printf("<%s>\n", name);
                        else
                        {
                            tb_printf("<%s", name);
                            for (; attr; attr = attr->next)
                                tb_printf(" %s = \"%s\"", tb_string_cstr(&attr->name), tb_string_cstr(&attr->data));
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
        }

        // exit reader
        tb_xml_reader_exit(reader);
    }

    return 0;
}

