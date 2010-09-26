#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	if (argc < 3) return 0;
	
	tb_generic_stream_t stream;
	tb_stream_t* st = tb_stream_open(&stream, argv[1], TB_NULL, 0, TB_STREAM_FLAG_IS_BLOCK);
	if (!st)
	{
		TB_DBG("failed to open url: %s", argv[1]);
		return 0;
	}

	tplat_handle_t hfile = tplat_file_open(argv[2], TPLAT_FILE_WO | TPLAT_FILE_TRUNC | TPLAT_FILE_BINARY);
	if (hfile == TPLAT_INVALID_HANDLE)
	{
		TB_DBG("failed to open file: %s", argv[2]);
		return 0;
	}

	tb_byte_t data[4096];
	do
	{
		tb_int_t read_n = tb_stream_read(st, data, 4096);
		if (read_n <= 0) break;

		tb_int_t write_n = 0;
		while (write_n < read_n)
		{
			tb_int_t ret = tplat_file_write(hfile, data, read_n);
			if (ret < 0) break;

			write_n += ret;
		}

	} while(1);


	return 0;
}

