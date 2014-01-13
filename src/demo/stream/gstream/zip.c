/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_http_test_hfunc(tb_http_option_t* option, tb_char_t const* line)
{
	tb_print("[demo]: response: %s", line);
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_gstream_zip_main(tb_int_t argc, tb_char_t** argv)
{
	// init stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
//	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_INFLATE);
//	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_DEFLATE);
//	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
//	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_DEFLATE);
	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_INFLATE);
//	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_DEFLATE);	
//	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_INFLATE);
//	tb_gstream_t* zst = tb_gstream_init_filter_from_zip(ist, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_DEFLATE);

	if (ist && ost && zst) 
	{
		// save it
		tb_hong_t save = tb_tstream_save_gg(zst, ost, 0, tb_null, tb_null);

		// trace
		tb_print("save: %lld bytes, size: %llu bytes", save, tb_gstream_size(ist));
	}

	// exit stream
	tb_gstream_exit(zst);
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);
	return 0;
}
