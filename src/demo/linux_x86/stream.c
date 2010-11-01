#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	if (argc < 3) return 0;
	
	tb_generic_stream_t istream;
	tb_stream_t* ist = tb_stream_open(&istream, argv[1], TB_NULL, 0, TB_STREAM_FLAG_RO);
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

	tb_byte_t 		data[TB_STREAM_DATA_MAX];
	tb_size_t 		read = 0;
	tb_size_t 		base = (tb_size_t)tplat_clock();
	tb_size_t 		time = (tb_size_t)tplat_clock();
	tb_size_t 		size = tb_stream_size(ist);
	do
	{
		tb_int_t ret = tb_stream_read(ist, data, TB_STREAM_DATA_MAX);
		//TB_DBG("ret: %d", ret);
		if (ret < 0) break;
		else if (!ret) 
		{
			tb_size_t timeout = ((tb_size_t)tplat_clock()) - time;
			if (timeout > 10000) break;
		}
		else
		{
			read += ret;
			if (tb_stream_write(ost, data, ret) < 0) break;
			time = (tb_size_t)tplat_clock();
		}

		// update info
		if (time > base && ((time - base) % 1000)) 
		{
			tplat_printf("speed: %5d kb/s, load: %8d kb\r", (read / (time - base)), read / 1000);
			fflush(stdout);
		}

		// is end?
		if (size && read >= size) break;

	} while(1);

	tplat_printf("speed: %d kb/s, load: %d kb, time: %d s\n", (read / ((tb_size_t)tplat_clock() - base)), read / 1000, ((tb_size_t)tplat_clock() - base) / 1000);
	tb_stream_close(ist);
	tb_stream_close(ost);


	return 0;
}

