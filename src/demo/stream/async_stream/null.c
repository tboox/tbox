/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_async_stream_null_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
	// trace
	tb_trace_i("save: %llu bytes, rate: %lu bytes/s, state: %s", save, rate, tb_state_cstr(state));

	// exit wait
	if (state != TB_STATE_OK) tb_event_post((tb_handle_t)priv);

	// ok
	return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_async_stream_null_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_handle_t 		event = tb_null;
	tb_handle_t 		transfer = tb_null;
	tb_async_stream_t* 	istream = tb_null;
	tb_async_stream_t* 	ostream = tb_null;
	tb_async_stream_t* 	fstream = tb_null;
	do
	{
		// init event
		event = tb_event_init();
		tb_assert_and_check_break(event);

		// init istream
		istream = tb_async_stream_init_from_url(tb_aicp(), argv[1]);
		tb_assert_and_check_break(istream);

		// init ostream
		ostream = tb_async_stream_init_from_file(tb_aicp(), argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
		tb_assert_and_check_break(ostream);

		// init fstream
		fstream = tb_async_stream_init_filter_from_null(istream);
		tb_assert_and_check_break(fstream);

		// init transfer
		transfer = tb_transfer_init_aa(fstream, ostream, 0);
		tb_assert_and_check_break(transfer);

		// open and save transfer
		if (!tb_transfer_osave(transfer, tb_demo_async_stream_null_save_func, event)) break;

		// wait it
		tb_event_wait(event, -1);

	} while (0);

	// exit transfer
	if (transfer) tb_transfer_exit(transfer, tb_false);
	transfer = tb_null;

	// exit fstream
	if (fstream) tb_async_stream_exit(fstream, tb_false);
	fstream = tb_null;

	// exit istream
	if (istream) tb_async_stream_exit(istream, tb_false);
	istream = tb_null;

	// exit ostream
	if (ostream) tb_async_stream_exit(ostream, tb_false);
	ostream = tb_null;

	// exit event
	if (event) tb_event_exit(event);
	event = tb_null;
	return 0;
}
