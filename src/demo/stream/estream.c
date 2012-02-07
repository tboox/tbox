#include "tbox.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	//tb_gstream_t* est = tb_gstream_init_from_encoding(ist, TB_ENCODING_UTF8, TB_ENCODING_GB2312);
	tb_gstream_t* est = tb_gstream_init_from_encoding(ist, TB_ENCODING_GB2312, TB_ENCODING_UTF8);
	if (!ist || !ost || !est) goto end;

	// init option
	tb_gstream_ctrl(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);

	// open stream
	if (!tb_gstream_bopen(ist)) goto end;
	if (!tb_gstream_bopen(ost)) goto end;
	if (!tb_gstream_bopen(est)) goto end;
	
	// read data
	tb_byte_t 		data[4096];
	tb_size_t 		read = 0;
	tb_int64_t 		time = tb_mclock();
	do
	{
		// read data
		tb_long_t n = tb_gstream_aread(est, data, 4096);
		if (n > 0)
		{
			// update read
			read += n;

			// update clock
			time = tb_mclock();

			// writ data
			if (!tb_gstream_bwrit(ost, data, n)) break;
		}
		else if (!n) 
		{
			// timeout?
			if (tb_mclock() - time > 5000) break;

			// sleep some time
			tb_usleep(100);
		}
		else break;

	} while(1);

end:

	// destroy stream
	tb_gstream_exit(est);
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);

	tb_printf("end\n");
	getchar();

	tb_exit();
	return 0;
}

