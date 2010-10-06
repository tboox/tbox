#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);
	
#if 0
	// create document
	tb_xml_document_t* 	doc = tb_xml_document_create();
	if (!doc) return 0;

	// create stream
	tb_generic_stream_t gst;
	tb_stream_t* st = tb_stream_open(&gst, argv[1], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_RO);
	if (!st)
	{
		TB_DBG("failed to open xml: %s", argv[1]);
		return 0;
	}

	// load & dump xml
	if (TB_FALSE == tb_xml_document_load_dump(doc, st))
	{
		TB_DBG("failed to load xml: %s", argv[1]);
		return 0;
	
	}

	tb_xml_document_destroy(doc);
#else
	tb_generic_stream_t gst;
	tb_stream_t* st = tb_stream_open(&gst, argv[1], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_WO | TB_STREAM_FLAG_TRUNC);
	if (!st)
	{
		TB_DBG("failed to open xml: %s", argv[1]);
		return 0;
	}
	
	tb_xml_writer_t* writer = tb_xml_writer_open(st);
	if (writer)
	{
		tb_xml_writer_document_beg(writer, "1.0", "utf-8");
		tb_xml_writer_characters(writer, "\n");

		tb_xml_writer_comment(writer, "\n\tcomments ...\n\tcomments ...\n\tcomments ...\n");
		tb_xml_writer_characters(writer, "\n");

		tb_xml_writer_element_beg(writer, "labels");
		tb_xml_writer_characters(writer, "\n");

			tb_xml_writer_attributes_add_int(writer, "id", 1);
			tb_xml_writer_attributes_add_c_string(writer, "name", "label 1");
			tb_xml_writer_element_beg(writer, "label_1");
			tb_xml_writer_characters(writer, "characters");
			tb_xml_writer_element_end(writer, "label_1");
			tb_xml_writer_characters(writer, "\n");

			tb_xml_writer_attributes_add_int(writer, "id", 2);
			tb_xml_writer_attributes_add_c_string(writer, "name", "label 2");
			tb_xml_writer_element_beg(writer, "label_2");
			tb_xml_writer_characters(writer, "characters");
			tb_xml_writer_element_end(writer, "label_2");
			tb_xml_writer_characters(writer, "\n");

			tb_xml_writer_attributes_add_int(writer, "id", 3);
			tb_xml_writer_attributes_add_format(writer, "name", "label %d", 3);
			tb_xml_writer_element_beg(writer, "label_3");
			tb_xml_writer_characters(writer, "characters");
			tb_xml_writer_element_end(writer, "label_3");
			tb_xml_writer_characters(writer, "\n");
			tb_xml_writer_cdata(writer, "datas");
			tb_xml_writer_characters(writer, "\n");

		tb_xml_writer_element_end(writer, "labels");
		tb_xml_writer_characters(writer, "\n");

		tb_xml_writer_document_end(writer);
		tb_xml_writer_close(writer);
	}

#endif

	TB_DBG("load ok!");

	// free it
	tb_stream_close(st);

	return 0;
}

