#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);
	
#if 0
	// create document
	tb_xml_document_t* 	document = tb_xml_document_create();
	if (!document) return 0;

	// create stream
	tb_generic_stream_t gst;
	tb_stream_t* st = tb_stream_open(&gst, argv[1], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_RO);
	if (!st)
	{
		TB_DBG("failed to open xml: %s", argv[1]);
		return 0;
	}

	// load & dump xml
	if (TB_FALSE == tb_xml_document_load_dump(document, st))
	{
		TB_DBG("failed to load xml: %s", argv[1]);
		return 0;
	
	}

	tb_xml_document_destroy(document);
	tb_stream_close(st);
#elif 0
	// create document
	tb_xml_document_t* 	document = tb_xml_document_create();
	if (!document) return 0;

	// open input stream
	tb_generic_stream_t istream;
	tb_stream_t* ist = tb_stream_open(&istream, argv[1], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_RO);
	if (!ist)
	{
		TB_DBG("failed to open input xml: %s", argv[1]);
		return 0;
	}

	// open output stream
	tb_generic_stream_t ostream;
	tb_stream_t* ost = tb_stream_open(&ostream, argv[2], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_WO | TB_STREAM_FLAG_TRUNC);
	if (!ist)
	{
		TB_DBG("failed to open output xml: %s", argv[2]);
		return 0;
	}

	// load xml
	if (TB_FALSE == tb_xml_document_load(document, ist))
	{
		TB_DBG("failed to load xml: %s", argv[1]);
		return 0;
	}

	// store xml
	if (TB_FALSE == tb_xml_document_store(document, ost))
	{
		TB_DBG("failed to store xml: %s", argv[2]);
		return 0;
	}

	tb_xml_document_destroy(document);
	tb_stream_close(ist);
	tb_stream_close(ost);
#elif 0
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
		tb_xml_writer_text(writer, "\n");

		tb_xml_writer_comment(writer, "\n\tcomments ...\n\tcomments ...\n\tcomments ...\n");
		tb_xml_writer_text(writer, "\n");

		tb_xml_writer_element_beg(writer, "labels");
		tb_xml_writer_text(writer, "\n");

			tb_xml_writer_attributes_add_int(writer, "id", 1);
			tb_xml_writer_attributes_add_c_string(writer, "name", "label 1");
			tb_xml_writer_element_beg(writer, "label_1");
			tb_xml_writer_text(writer, "characters");
			tb_xml_writer_element_end(writer, "label_1");
			tb_xml_writer_text(writer, "\n");

			tb_xml_writer_attributes_add_int(writer, "id", 2);
			tb_xml_writer_attributes_add_c_string(writer, "name", "label 2");
			tb_xml_writer_element_beg(writer, "label_2");
			tb_xml_writer_text(writer, "characters");
			tb_xml_writer_element_end(writer, "label_2");
			tb_xml_writer_text(writer, "\n");

			tb_xml_writer_attributes_add_int(writer, "id", 3);
			tb_xml_writer_attributes_add_format(writer, "name", "label %d", 3);
			tb_xml_writer_element_beg(writer, "label_3");
			tb_xml_writer_text(writer, "characters");
			tb_xml_writer_element_end(writer, "label_3");
			tb_xml_writer_text(writer, "\n");
			tb_xml_writer_cdata(writer, "datas");
			tb_xml_writer_text(writer, "\n");

		tb_xml_writer_element_end(writer, "labels");
		tb_xml_writer_text(writer, "\n");

		tb_xml_writer_document_end(writer);
		tb_xml_writer_close(writer);
	}

	// free it
	tb_stream_close(st);
#elif 1
	tb_generic_stream_t gst;
	tb_stream_t* st = tb_stream_open(&gst, argv[1], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_WO | TB_STREAM_FLAG_TRUNC);
	if (!st)
	{
		TB_DBG("failed to open xml: %s", argv[1]);
		return 0;
	}

	// create document
	tb_xml_document_t* document = tb_xml_document_create();
	if (!document) return 0;

	// xml header
	tb_string_assign_c_string_by_ref(tb_xml_document_version(document), "1.0");
	tb_string_assign_c_string_by_ref(tb_xml_document_encoding(document), "utf-8");

	// comment
	tb_xml_node_add_comment(document, "\n\tcomments ...\n\tcomments ...\n\tcomments ...\n");
	tb_xml_node_add_text(document, "\n");

	// labels
	tb_xml_node_t* labels = tb_xml_node_add_element(document, "labels");
	tb_xml_node_add_text(labels, "\n");

	// label_1
	tb_xml_node_t* label_1 = tb_xml_node_add_element(labels, "label_1");
	tb_xml_node_attributes_add_int(label_1, "id", 1);
	tb_xml_node_attributes_add_c_string(label_1, "name", "label 1");
	tb_xml_node_add_text(label_1, "characters");
	tb_xml_node_add_text(labels, "\n");

	// label_2
	tb_xml_node_t* label_2 = tb_xml_node_add_element(labels, "label_2");
	tb_xml_node_attributes_add_int(label_2, "id", 2);
	tb_xml_node_attributes_add_c_string(label_2, "name", "label 2");
	tb_xml_node_add_text(label_2, "characters");
	tb_xml_node_add_text(labels, "\n");

	// label_3
	tb_xml_node_t* label_3 = tb_xml_node_add_element(labels, "label_3");
	tb_xml_node_attributes_add_int(label_3, "id", 3);
	tb_xml_node_attributes_add_format(label_3, "name", "label %d", 3);
	tb_xml_node_add_text(label_3, "characters");
	tb_xml_node_add_text(labels, "\n");

	// store xml
	if (TB_FALSE == tb_xml_document_store(document, st))
	{
		TB_DBG("failed to store xml: %s", argv[1]);
		return 0;
	}

	tb_xml_document_destroy(document);
	tb_stream_close(st);
#endif

	return 0;
}

