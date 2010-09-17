#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);
	
	tb_generic_stream_t stream;
	tb_stream_t* st = tb_stream_open(&stream, argv[1], TB_NULL, 0, TB_STREAM_FLAG_IS_BLOCK);
	if (!st)
	{
		TB_DBG("failed to open url: %s", argv[1]);
		return 0;
	}


	tb_format_t const* format =	tb_format_detect(st, TB_FORMAT_FLAG_ALL);
	if (!format)
	{
		TB_DBG("unsupported format");
		return 0;
	}

	TB_DBG("format: %s", format->name);

	return 0;
}

