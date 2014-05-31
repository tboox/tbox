/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_aicp_http_head_func(tb_handle_t http, tb_char_t const* line, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(http && line, tb_false);

    // trace
    tb_trace_i("head: %s", line);

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_aicp_http_read_func(tb_handle_t handle, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv);
static tb_void_t tb_demo_aicp_http_clos_func(tb_handle_t handle, tb_size_t state, tb_cpointer_t priv)
{
    // trace
    tb_trace_i("clos: state: %s", tb_state_cstr(state));

#if 1
    // kill loop
    tb_aicp_kill((tb_aicp_t*)priv);
#else
    // reopen and read 
    tb_aicp_http_open_read(handle, 0, tb_demo_aicp_http_read_func, priv);
#endif
}
static tb_bool_t tb_demo_aicp_http_read_func(tb_handle_t handle, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // trace
    tb_trace_i("read: %lu, state: %s", real, tb_state_cstr(state));

    // failed or closed? close it
    if (state != TB_STATE_OK) tb_aicp_http_clos(handle, tb_demo_aicp_http_clos_func, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_aicp_http_post_func(tb_handle_t http, tb_size_t state, tb_hong_t offset, tb_hize_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // trace
    tb_trace_i("post: %llu, rate: %lu bytes/s, state: %s", save, rate, tb_state_cstr(state));

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_http_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argv[1], 0);

    // init
    tb_handle_t         aicp = tb_null;
    tb_handle_t         http = tb_null;
    tb_async_stream_t*  post = tb_null;

    // init aicp
    aicp = tb_aicp_init(3);
    tb_assert_and_check_goto(aicp, end);

    // init http
    http = tb_aicp_http_init(aicp);
    tb_assert_and_check_goto(http, end);

    // init cookies
    if (!tb_http_option(http, TB_HTTP_OPTION_SET_COOKIES, tb_cookies())) goto end;
    if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_HEAD, "Cookie", "H_PS_PSSID=4990_6552_6249_1434_5225_6582_6504_6476_4760_6017_6673_6428_6632_6530_6502; BDRCVFR[C0p6oIjvx-c]=mk3SLVN4HKm; BDSVRTM=0; BAIDUID=344544EE3DE59CCF2BFDDB2EE0F49F7E:FG=1;")) goto end;
    
    // init url
    if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_URL, argv[1])) goto end;

    // init head 
//  if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_HEAD, "Connection", "keep-alive")) goto end;

    // init head func
    if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_HEAD_FUNC, tb_demo_aicp_http_head_func)) goto end;

#if defined(TB_CONFIG_MODULE_HAVE_ZIP) && defined(TB_CONFIG_THIRD_HAVE_ZLIB)
    // need gzip
    if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_HEAD, "Accept-Encoding", "gzip,deflate")) goto end;

    // auto unzip
    if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_AUTO_UNZIP, 1)) goto end;
#endif

    // user agent
    if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_HEAD, "User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.137 Safari/537.36")) goto end;

    // init post
    if (argv[2])
    {
        // init post url
        if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_POST_URL, argv[2])) goto end;

        // init post func
        if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_POST_FUNC, tb_demo_aicp_http_post_func)) goto end;

        // init method
        if (!tb_aicp_http_option(http, TB_HTTP_OPTION_SET_METHOD, TB_HTTP_METHOD_POST)) goto end;
    }

    // open and read 
    if (!tb_aicp_http_open_read(http, 0, tb_demo_aicp_http_read_func, aicp)) goto end;

    // loop aicp
    tb_aicp_loop(aicp);

end:

    // trace
    tb_trace_i("end");

    // exit http
    if (http) tb_aicp_http_exit(http);

    // exit post
    if (post) tb_async_stream_exit(post);

    // exit aicp
    if (aicp) tb_aicp_exit(aicp);
    return 0;
}
