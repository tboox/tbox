#include "tbox.h"
#include "stdio.h"

static tb_bool_t http_callback_head(tb_char_t const* line, void* priv)
{
	tb_printf("head: %s\n", line);
	return TB_TRUE;
}

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create stream
	tb_gstream_t* ist = tb_gstream_create_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_create_from_url(argv[2]);
	if (!ist || !ost) goto end;
	
	// init option
	tb_gstream_ioctl2(ist, TB_HSTREAM_CMD_SET_HEAD_FUNC, http_callback_head, TB_NULL);
	tb_gstream_ioctl1(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);

	// open stream
	if (TB_FALSE == tb_gstream_open(ist)) goto end;
	if (TB_FALSE == tb_gstream_open(ost)) goto end;
	
	// read data
	tb_byte_t 		data[8192];
	tb_size_t 		read = 0;
	tb_size_t 		base = (tb_size_t)tb_mclock();
	tb_size_t 		time = (tb_size_t)tb_mclock();
	tb_size_t 		size = tb_gstream_size(ist);
	do
	{
		tb_int_t ret = tb_gstream_read(ist, data, 8192);
		//TB_DBG("ret: %d", ret);
		if (ret > 0)
		{
			read += ret;
			time = (tb_size_t)tb_mclock();

#if 1
			tb_int_t write = 0;
			while (write < ret)
			{
				tb_int_t ret2 = tb_gstream_write(ost, data + write, ret - write);
				if (ret2 > 0) write += ret2;
				else if (ret2 < 0) break;
			}
#endif

		}
		else if (!ret) 
		{
			tb_size_t timeout = ((tb_size_t)tb_mclock()) - time;
			if (timeout > 10000) break;
		}
		else break;

		// update info
		if (time > base && ((time - base) % 1000)) 
		{
			tb_printf("speed: %5d kb/s, load: %8d kb\r", (read / (time - base)), read / 1000);
			fflush(stdout);
		}

		// is end?
		if (size && read >= size) break;

	} while(1);

end:

	// destroy stream
	tb_gstream_destroy(ist);
	tb_gstream_destroy(ost);

	tb_printf("end\n");
	getchar();

	tb_exit();
	return 0;
}
