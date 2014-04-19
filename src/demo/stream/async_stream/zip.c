/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_async_stream_zip_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// percent
	tb_size_t percent = 0;
	if (size > 0) percent = (offset * 100) / size;
	else if (state == TB_STATE_OK) percent = 100;

	// trace
	tb_trace_i("save: %llu bytes, rate: %lu bytes/s, percent: %lu%%, state: %s", save, rate, percent, tb_state_cstr(state));

	// kill aicp
	if (state != TB_STATE_OK) tb_aicp_kill((tb_aicp_t*)priv);

	// ok
	return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_async_stream_zip_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_aicp_t* 		aicp = tb_null;
	tb_handle_t 	tstream = tb_null;
	tb_async_stream_t* 	istream = tb_null;
	tb_async_stream_t* 	ostream = tb_null;
	tb_async_stream_t* 	fstream = tb_null;
	do
	{
		// init aicp
		aicp = tb_aicp_init(2);
		tb_assert_and_check_break(aicp);

		// init istream
		istream = tb_async_stream_init_from_url(aicp, argv[1]);
		tb_assert_and_check_break(istream);

		// init ostream
		ostream = tb_async_stream_init_from_file(aicp, argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
		tb_assert_and_check_break(ostream);

		// filter istream or ostream?
		tb_async_stream_t* iostream = istream;
//		tb_async_stream_t* iostream = ostream;

		// init fstream
//		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_INFLATE);
//		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_DEFLATE);
//		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
//		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_DEFLATE);
//		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_INFLATE);
		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_DEFLATE);
//		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_INFLATE);
//		fstream = tb_async_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_DEFLATE);
		tb_assert_and_check_break(fstream);

		// init tstream
		if (iostream == istream) tstream = tb_transfer_stream_init_aa(fstream, ostream, 0);
		else tstream = tb_transfer_stream_init_aa(istream, fstream, 0);
		tb_assert_and_check_break(tstream);

		// limit rate
//		tb_transfer_stream_limitrate(tstream, 4096);

		// open and save tstream
		if (!tb_transfer_stream_osave(tstream, tb_demo_async_stream_zip_save_func, aicp)) break;

		// done loop
		tb_aicp_loop(aicp);

	} while (0);

	// exit tstream
	if (tstream) tb_transfer_stream_exit(tstream, tb_false);
	tstream = tb_null;

	// exit fstream
	if (fstream) tb_async_stream_exit(fstream, tb_false);
	fstream = tb_null;

	// exit istream
	if (istream) tb_async_stream_exit(istream, tb_false);
	istream = tb_null;

	// exit ostream
	if (ostream) tb_async_stream_exit(ostream, tb_false);
	ostream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;
	return 0;
}
