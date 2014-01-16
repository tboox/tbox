/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_tstream_save_func(tb_size_t state, tb_size_t size, tb_size_t rate, tb_pointer_t priv)
{
	// trace
	tb_print("save: %lu bytes, rate: %lu bytes/s, state: %s", size, rate, tb_astream_state_cstr(state));

	// ok
	return tb_true;
}
static tb_pointer_t tb_demo_tstream_loop(tb_pointer_t data)
{
	// the aicp
	tb_aicp_t* aicp = (tb_aicp_t*)data;

	// done loop
	if (aicp) tb_aicp_loop(aicp);

	// exit it
	tb_thread_return(tb_null);
	return tb_null;
}
/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_tstream_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_aicp_t* 		aicp = tb_null;
	tb_handle_t 	tstream = tb_null;
	do
	{
		// init aicp
		aicp = tb_aicp_init(2);
		tb_assert_and_check_break(aicp);

		// init tstream
		tstream = tb_tstream_init_uu(aicp, argv[1], argv[2], tb_demo_tstream_save_func, tb_null);
		tb_assert_and_check_break(tstream);

		// init loop
		if (!tb_thread_init(tb_null, tb_demo_tstream_loop, aicp, 0)) break;

		// start tstream
		if (!tb_tstream_start(tstream, -1)) break;

		// wait
		getchar();

		// pause tstream
		tb_tstream_pause(tstream);

		// wait
		getchar();

		// start tstream
		if (!tb_tstream_start(tstream, -1)) break;

		// wait
		getchar();

	} while (0);

	// exit tstream
	if (tstream) tb_tstream_exit(tstream);
	tstream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;
	return 0;
}
