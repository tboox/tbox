/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_http_demo_head_func(tb_handle_t http, tb_char_t const* line, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(http && line, tb_false);

	// option 
	tb_http_option_t* 	option = tb_http_option(http);

	// cookies
	tb_cookies_t* 		cookies = priv;

	// trace
	tb_print("[demo]: response: %s", line);

	// cookie
	if (cookies && !tb_strnicmp(line, "Set-Cookie", 10))
	{
		// seek to value
		tb_char_t const* p = line;
		while (*p && *p != ':') p++;
		tb_assert_and_check_return_val(*p, tb_false);
		p++; while (tb_isspace(*p)) p++;
		tb_assert_and_check_return_val(*p, tb_false);

		tb_char_t const* url = tb_url_get(&option->url);
		if (url) tb_cookies_set_from_url(cookies, url, p);
	}

	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_http_main(tb_int_t argc, tb_char_t** argv)
{
	// init http
	tb_handle_t http = tb_http_init();
	tb_assert_and_check_goto(http, end);

	// option
	tb_http_option_t* option = tb_http_option(http);
	tb_assert_and_check_goto(option, end);

	// status
	tb_http_status_t const* status = tb_http_status(http);
	tb_assert_and_check_goto(status, end);

	// init cookies
	option->head_priv = tb_cookies_init();
	tb_assert_and_check_goto(option->head_priv, end);
	
	// init url
	if (!tb_url_set(&option->url, argv[1])) goto end;

	// keep-alive
	option->balive = 0;

	// timeout 
//	option->timeout = 10000;

	// redirect
//	option->rdtm = 0;

	// init head func
	option->head_func = tb_http_demo_head_func;

	// init method
	option->method = TB_HTTP_METHOD_POST;

	// init post
	option->post = tb_strlen(argv[2]);

	// open http
	tb_hong_t t = tb_mclock();
	if (!tb_http_bopen(http)) goto end;
	t = tb_mclock() - t;
	tb_print("[demo]: open: %llu ms", t);

	// writ post
	tb_http_bwrit(http, argv[2], option->post);
	tb_http_bfwrit(http, tb_null, 0);

	// read data
	tb_byte_t 		data[8192];
	tb_size_t 		read = 0;
	tb_hize_t 		size = status->content_size;
	do
	{
		// read data
		tb_long_t real = tb_http_aread(http, data, 8192);
		tb_print("[demo]: read: %d", real);
		if (real > 0)
		{
			// dump data
			tb_char_t const* 	p = data;
			tb_char_t const* 	e = data + real;
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
			tb_print("[demo]: wait");
			tb_long_t e = tb_http_wait(http, TB_AIOE_CODE_RECV, option->timeout);
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

	// exit cookies
	if (option && option->head_priv) 
	{
		// dump cookies
#ifdef __tb_debug__
		tb_cookies_dump(option->head_priv);
#endif

		tb_cookies_exit(option->head_priv);
	}

	// exit http
	if (http) tb_http_exit(http);

	return 0;
}
