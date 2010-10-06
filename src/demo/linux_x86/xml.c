#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);
	
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

#if 1
	// load & dump xml
	if (TB_FALSE == tb_xml_document_load_dump(doc, st))
	{
		TB_DBG("failed to load xml: %s", argv[1]);
		return 0;
	
	}
#endif

	TB_DBG("load ok!");

	// free it
	tb_xml_document_destroy(doc);
	tb_stream_close(st);

	return 0;
}

