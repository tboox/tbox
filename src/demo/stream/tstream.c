/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * func
 */ 
static tb_bool_t tb_demo_tstream_save_func(tb_size_t state, tb_size_t size, tb_size_t rate, tb_pointer_t priv)
{

	return tb_false;
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

		// start tstream
		if (!tb_tstream_start(tstream, -1)) break;

		// loop aicp
		tb_aicp_loop(aicp);

	} while (0);

	// exit tstream
	if (tstream) tb_tstream_exit(tstream);
	tstream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;
	return 0;
}
