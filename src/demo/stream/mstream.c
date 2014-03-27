/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */ 
#define TB_DEMO_TEST_AICP 			(1)
#define TB_DEMO_TRACE_ENABLE 		(1)
	
/* ///////////////////////////////////////////////////////////////////////
 * globals
 */ 
#if TB_DEMO_TEST_AICP
static tb_handle_t g_event = tb_null;
#endif
	
/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */ 
#if TB_DEMO_TEST_AICP
static tb_bool_t tb_demo_tstream_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// trace
# 	if TB_DEMO_TRACE_ENABLE
	// percent
	tb_size_t percent = 0;
	if (size > 0) percent = (offset * 100) / size;
	else if (state == TB_STREAM_STATE_OK) percent = 100;

	// trace
	tb_trace_i("save[%s]: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", priv, save, rate, percent, tb_stream_state_cstr(state));
# 	endif
 
	// failed or closed?
	if (state != TB_STREAM_STATE_OK) tb_event_post(g_event);

	// ok
	return tb_true;
}
#else
static tb_bool_t tb_demo_tstream_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// trace
# 	if TB_DEMO_TRACE_ENABLE
	// percent
	tb_size_t percent = 0;
	if (size > 0) percent = (offset * 100) / size;
	else if (state == TB_STREAM_STATE_OK) percent = 100;

	// trace
	tb_trace_i("save[%s]: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", priv, save, rate, percent, tb_stream_state_cstr(state));
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
	tb_handle_t 	mstream = tb_null;
	do
	{
		// init event
		g_event = tb_event_init();
		tb_assert_and_check_break(g_event);

		// init mstream
		mstream = tb_mstream_init(tb_null, 3, 0);
		tb_assert_and_check_break(mstream);

		// init tasks
		tb_char_t** p = &argv[2];
		for (; p && *p; p++)
		{
			// done mstream
			if (!tb_mstream_done(mstream, argv[1], *p, 0, tb_demo_tstream_save_func, *p)) break;
		}

	} while (0);

	// wait mstream
	while (g_event && tb_mstream_size(mstream) && tb_event_wait(g_event, -1) > 0);

	// exit mstream
	if (mstream) tb_mstream_exit(mstream);
	mstream = tb_null;

	// exit event
	if (g_event) tb_event_exit(g_event);
	g_event = tb_null;
#else
	tb_char_t** p = &argv[2];
	for (; p && *p; p++) tb_tstream_save_uu(argv[1], *p, 0, tb_demo_tstream_save_func, *p);
#endif
	return 0;
}
