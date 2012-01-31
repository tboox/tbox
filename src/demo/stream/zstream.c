/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_http_test_hfunc(tb_http_option_t* option, tb_char_t const* line)
{
	tb_print("[demo]: response: %s", line);
	return TB_TRUE;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	//tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_INFLATE);
	//tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_DEFLATE);
	//tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
	//tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_DEFLATE);
	tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_INFLATE);
	//tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_DEFLATE);	
	//tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_INFLATE);
	//tb_gstream_t* zst = tb_gstream_init_from_zip(ist, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_DEFLATE);

	if (!ist || !ost || !zst) goto end;

	// init option
	if (tb_gstream_type(ist) == TB_GSTREAM_TYPE_HTTP) 
	{
		tb_http_option_t* option = TB_NULL;
		tb_gstream_ctrl1(ist, TB_HSTREAM_CMD_GET_OPTION, &option);
		if (option) 
		{
			option->hfunc = tb_http_test_hfunc;
			tb_hash_set(option->head, "Accept-Encoding", "gzip,deflate");
		}
	}
	if (tb_gstream_type(ost) == TB_GSTREAM_TYPE_FILE) tb_gstream_ctrl1(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);

	// open stream
	if (!tb_gstream_bopen(ist)) goto end;
	if (!tb_gstream_bopen(ost)) goto end;
	if (!tb_gstream_bopen(zst)) goto end;
	
	// read data
	tb_byte_t 		data[4096];
	tb_size_t 		read = 0;
	tb_int64_t 		time = tb_mclock();
	do
	{
		// read data
		tb_long_t n = tb_gstream_aread(zst, data, 4096);
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
	tb_gstream_exit(zst);
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);

	tb_printf("end\n");
	getchar();

	return 0;
}
