#include "tbox.h"
#include "stdio.h"

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create stream
	tb_gstream_t* ist = tb_gstream_create_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_create_from_url(argv[2]);
	//tb_gstream_t* est = tb_gstream_create_from_encoding(ist, TB_ENCODING_UTF8, TB_ENCODING_GB2312);
	tb_gstream_t* est = tb_gstream_create_from_encoding(ist, TB_ENCODING_GB2312, TB_ENCODING_UTF8);
	if (!ist || !ost || !est) goto end;

	// init option
	tb_gstream_ioctl1(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);

	// open stream
	if (!tb_gstream_open(ist)) goto end;
	if (!tb_gstream_open(ost)) goto end;
	if (!tb_gstream_open(est)) goto end;
	
	// read data
	tb_byte_t 		data[4096];
	tb_size_t 		read = 0;
	tb_int64_t 		time = tb_mclock();
	do
	{
		tb_long_t ret = tb_gstream_read(est, data, 4096);
		//tb_print("ret: %d", ret);
		if (ret > 0)
		{
			read += ret;
			time = tb_mclock();

#if 1
			tb_long_t write = 0;
			while (write < ret)
			{
				tb_long_t ret2 = tb_gstream_write(ost, data + write, ret - write);
				if (ret2 > 0) write += ret2;
				else if (ret2 < 0) break;
			}
#endif

		}
		else if (!ret) 
		{
			if (tb_mclock() - time > 5000) break;
		}
		else break;

	} while(1);

end:

	// destroy stream
	tb_gstream_destroy(est);
	tb_gstream_destroy(ist);
	tb_gstream_destroy(ost);

	tb_printf("end\n");
	getchar();

	tb_exit();
	return 0;
}

