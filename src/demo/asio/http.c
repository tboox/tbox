/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_aicp_http_head_func(tb_handle_t http, tb_char_t const* line, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(http && line, tb_false);

	// trace
	tb_print("head: %s", line);

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_aicp_http_read_func(tb_handle_t handle, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// trace
	tb_print("read: %lu, state: %s", real, tb_stream_state_cstr(state));

	// failed or closed? kill aicp
	if (state != TB_STREAM_STATE_OK) tb_aicp_kill(priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_aicp_http_post_func(tb_handle_t http, tb_size_t state, tb_hize_t size, tb_size_t rate, tb_pointer_t priv)
{
	// trace
	tb_print("post: %llu, rate: %lu bytes/s, state: %s", size, rate, tb_stream_state_cstr(state));

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_http_main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argv[1], 0);

	// init
	tb_handle_t 		aicp = tb_null;
	tb_handle_t 		http = tb_null;
	tb_astream_t* 		post = tb_null;

	// init aicp
	aicp = tb_aicp_init(3);
	tb_assert_and_check_goto(aicp, end);

	// init http
	http = tb_aicp_http_init(aicp);
	tb_assert_and_check_goto(http, end);

	// init url
	if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_URL, argv[1])) goto end;

	// init head 
//	if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_HEAD, "Connection", "keep-alive")) goto end;

	// init head func
	if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_HEAD_FUNC, tb_demo_aicp_http_head_func)) goto end;

	// init post url
	if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_POST_URL, argv[2])) goto end;

	// init post func
	if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_POST_FUNC, tb_demo_aicp_http_post_func)) goto end;

	// init method
	if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_METHOD, argv[2]? TB_HTTP_METHOD_POST : TB_HTTP_METHOD_GET)) goto end;

	// open and read 
	if (!tb_aicp_http_oread(http, 0, tb_demo_aicp_http_read_func, aicp)) goto end;

	// loop aicp
	tb_aicp_loop(aicp);

end:

	// trace
	tb_print("end");

	// exit http
	if (http) tb_aicp_http_exit(http, tb_false);

	// exit post
	if (post) tb_astream_exit(post, tb_false);

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	return 0;
}
