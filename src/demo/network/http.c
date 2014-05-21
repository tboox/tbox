/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_http_demo_head_func(tb_handle_t http, tb_char_t const* line, tb_cpointer_t priv)
{
	// check
	tb_assert_and_check_return_val(http && line, tb_false);

	// trace
	tb_trace_i("head: %s", line);

	// ok
	return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_http_main(tb_int_t argc, tb_char_t** argv)
{
	// init http
	tb_handle_t http = tb_http_init();
	tb_assert_and_check_goto(http, end);

	// init cookies
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_COOKIES, tb_cookies())) goto end;
	
	// init head func
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_HEAD_FUNC, tb_http_demo_head_func)) goto end;
	
	// init url
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_URL, argv[1])) goto end;
	
	// init post 
	if (argv[2])
	{
		// init post url
		if (!tb_http_option(http, TB_HTTP_OPTION_SET_POST_URL, argv[2])) goto end;

		// init method
		if (!tb_http_option(http, TB_HTTP_OPTION_SET_METHOD, TB_HTTP_METHOD_POST)) goto end;
	}

	// init timeout 
	tb_size_t timeout = 0;
//	if (!tb_http_option(http, TB_HTTP_OPTION_SET_TIMEOUT, 10000)) goto end;
	if (!tb_http_option(http, TB_HTTP_OPTION_GET_TIMEOUT, &timeout)) goto end;

	// init redirect maxn
//	if (!tb_http_option(http, TB_HTTP_OPTION_SET_REDIRECT, 0)) goto end;

	// open http
	tb_hong_t t = tb_mclock();
	if (!tb_http_open(http)) goto end;
	t = tb_mclock() - t;
	tb_trace_i("open: %llu ms", t);

	// read data
	tb_byte_t 		data[8192];
	tb_size_t 		read = 0;
	tb_hize_t 		size = tb_http_status(http)->content_size;
	do
	{
		// read data
		tb_long_t real = tb_http_read(http, data, 8192);
		tb_trace_i("read: %d", real);
		if (real > 0)
		{
			// dump data
			tb_char_t const* 	p = (tb_char_t const*)data;
			tb_char_t const* 	e = (tb_char_t const*)data + real;
			tb_char_t 			b[8192 + 1];
			while (p < e && *p)
			{
				tb_char_t* 			q = b;
				tb_char_t const* 	d = b + 4096;
				for (; q < d && p < e && *p; p++, q++) *q = *p;
				*q = '\0';
				tb_printf("%s", b);
			}
			tb_printf("\n");

			// save read
			read += real;
		}
		else if (!real) 
		{
			// wait
			tb_trace_i("wait");
			tb_long_t e = tb_http_wait(http, TB_AIOE_CODE_RECV, timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOE_CODE_RECV);
		}
		else break;

		// is end?
		if (size && read >= size) break;

	} while(1);

end:

	// exit http
	if (http) tb_http_exit(http);
	return 0;
}
