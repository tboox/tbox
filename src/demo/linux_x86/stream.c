#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	if (argc < 3) return 0;
	
	tb_generic_stream_t istream;
	tb_stream_t* ist = tb_stream_open(&istream, argv[1], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_RO);
	if (!ist)
	{
		TB_DBG("failed to open url: %s", argv[1]);
		return 0;
	}

	tb_generic_stream_t ostream;
	tb_stream_t* ost = tb_stream_open(&ostream, argv[2], TB_NULL, 0, TB_STREAM_FLAG_BLOCK | TB_STREAM_FLAG_WO | TB_STREAM_FLAG_TRUNC);
	if (!ost)
	{
		TB_DBG("failed to open url: %s", argv[2]);
		return 0;
	}

	tb_byte_t data[256];
	do
	{
		tb_int_t read_n = tb_stream_read(ist, data, 256);
		if (read_n <= 0 || tb_stream_write(ost, data, read_n) < 0) break;

	} while(1);

	tb_stream_close(ist);
	tb_stream_close(ost);


	return 0;
}

