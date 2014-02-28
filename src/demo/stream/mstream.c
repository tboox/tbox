/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */ 
#define TB_DEMO_TEST_AICP 			(0)
#define TB_DEMO_TRACE_ENABLE 		(1)

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */ 
#if TB_DEMO_TEST_AICP
static tb_atomic_t g_size = 0;
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_loop_stop(tb_pointer_t priv)
{
	tb_atomic_t* size = (tb_atomic_t*)priv;
	return tb_atomic_get(size)? tb_false : tb_true;
}
static tb_void_t tb_demo_tstream_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_handle_t tstream = item? *((tb_handle_t*)item) : tb_null;
	if (tstream) tb_tstream_exit(tstream, tb_false);
}
#if TB_DEMO_TEST_AICP
static tb_bool_t tb_demo_tstream_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// trace
# 	if TB_DEMO_TRACE_ENABLE
	tb_print("save: %llu bytes, rate: %lu bytes/s, state: %s, to: %s", save, rate, tb_stream_state_cstr(state), priv);
# 	endif
 
	// size--
	if (state != TB_STREAM_STATE_OK) tb_atomic_fetch_and_dec(&g_size);

	// ok
	return tb_true;
}
#else
static tb_bool_t tb_demo_tstream_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// trace
# 	if TB_DEMO_TRACE_ENABLE
	tb_print("save: %llu bytes, rate: %lu bytes/s, state: %s, to: %s", save, rate, tb_stream_state_cstr(state), priv);
# 	endif

	// ok
	return tb_true;
}
#endif

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_mstream_main(tb_int_t argc, tb_char_t** argv)
{
#if TB_DEMO_TEST_AICP
	// done
	tb_aicp_t* 		aicp = tb_null;
	tb_vector_t* 	list = tb_null;
	do
	{
		// init aicp
		aicp = tb_aicp_init(256);
		tb_assert_and_check_break(aicp);

		// init list
		list = tb_vector_init(256, tb_item_func_ptr(tb_demo_tstream_free, tb_null));
		tb_assert_and_check_break(list);

		// init tasks
		tb_char_t** p = &argv[2];
		for (; p && *p; p++)
		{
			// init tstream
			tb_handle_t tstream = tb_tstream_init_uu(aicp, argv[1], *p, 0);
			tb_assert_and_check_break(tstream);

			// add tstream
			tb_vector_insert_tail(list, tstream);

			// size++
			tb_atomic_fetch_and_inc(&g_size);

			// open and save tstream
			if (!tb_tstream_osave(tstream, tb_demo_tstream_save_func, *p)) break;
		}
		tb_assert_and_check_break(tb_vector_size(list));

		// done loop
		tb_aicp_loop_util(aicp, tb_demo_loop_stop, (tb_pointer_t)&g_size);

	} while (0);

	// exit list
	if (list) tb_vector_exit(list);
	list = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;
#else
	tb_char_t** p = &argv[2];
	for (; p && *p; p++) tb_tstream_save_uu(argv[1], *p, 0, tb_demo_tstream_save_func, *p);
#endif
	return 0;
}
