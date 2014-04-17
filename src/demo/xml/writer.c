/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_xml_writer_main(tb_int_t argc, tb_char_t** argv)
{
	// init stream
	tb_gstream_t* gst = tb_gstream_init_from_url(argv[1]);
	tb_stream_ctrl(gst, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
	if (gst && tb_gstream_open(gst))
	{
		tb_handle_t writer = tb_xml_writer_init(gst, tb_true);
		if (writer)
		{
			tb_xml_writer_document(writer, "1.0", "utf-8");
			tb_xml_writer_comment(writer, "comments ...");
			tb_xml_writer_attributes_format(writer, "name", "hello %s", "world");
			tb_xml_writer_element_enter(writer, "labels");

				tb_xml_writer_attributes_long(writer, "id", 1);
				tb_xml_writer_attributes_cstr(writer, "label", "1");
				tb_xml_writer_element_enter(writer, "label_1");
				tb_xml_writer_text(writer, "characters");
				tb_xml_writer_element_leave(writer);

				tb_xml_writer_attributes_long(writer, "id", 2);
				tb_xml_writer_attributes_bool(writer, "bool", tb_true);
				tb_xml_writer_element_enter(writer, "label_2");
				tb_xml_writer_text(writer, "characters");
				tb_xml_writer_element_leave(writer);

				tb_xml_writer_attributes_long(writer, "id", 3);
				tb_xml_writer_attributes_format(writer, "label", "%d", 3);
				tb_xml_writer_element_enter(writer, "label_3");
				tb_xml_writer_text(writer, "characters");

					tb_xml_writer_attributes_long(writer, "id", -4);
					tb_xml_writer_attributes_bool(writer, "bool", tb_false);
					tb_xml_writer_element_empty(writer, "label_4");

					tb_xml_writer_element_empty(writer, "label_5");

				tb_xml_writer_element_leave(writer);
				tb_xml_writer_cdata(writer, "datas");

			tb_xml_writer_element_leave(writer);
			tb_xml_writer_exit(writer);
		}

		// exit stream
		tb_gstream_exit(gst);
	}
	
	return 0;
}

