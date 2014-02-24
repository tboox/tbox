/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_http_demo_head_func(tb_handle_t http, tb_char_t const* line, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(http && line, tb_false);

	// cookies
	tb_cookies_t* cookies = priv;

	// trace
	tb_print("head: %s", line);

	// cookie
	if (cookies && !tb_strnicmp(line, "Set-Cookie", 10))
	{
		// seek to value
		tb_char_t const* p = line;
		while (*p && *p != ':') p++;
		tb_assert_and_check_return_val(*p, tb_false);
		p++; while (tb_isspace(*p)) p++;
		tb_assert_and_check_return_val(*p, tb_false);
	
		// the url
		tb_char_t const* url = tb_null;
		if (tb_http_option(http, TB_HTTP_OPTION_GET_URL, &url) && url)
			tb_cookies_set_from_url(cookies, url, p);
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

	// init cookies
	tb_cookies_t* cookies = tb_cookies_init();
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_HEAD_PRIV, cookies)) goto end;
	
	// init head func
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_HEAD_FUNC, tb_http_demo_head_func)) goto end;
	
	// init url
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_URL, argv[1])) goto end;
	
#if 0
	// init post size
	tb_hize_t post_size = argv[2]? tb_strlen(argv[2]) : 0;
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_POST_SIZE, post_size)) goto end;

	// init method
	if (!tb_http_option(http, TB_HTTP_OPTION_SET_METHOD, post_size? TB_HTTP_METHOD_POST : TB_HTTP_METHOD_GET)) goto end;
#endif

	// init timeout 
	tb_size_t timeout = 0;
//	if (!tb_http_option(http, TB_HTTP_OPTION_SET_TIMEOUT, 10000)) goto end;
	if (!tb_http_option(http, TB_HTTP_OPTION_GET_TIMEOUT, &timeout)) goto end;

	// init redirect maxn
//	if (!tb_http_option(http, TB_HTTP_OPTION_SET_REDIRECT, 0)) goto end;

	// open http
	tb_hong_t t = tb_mclock();
	if (!tb_http_bopen(http)) goto end;
	t = tb_mclock() - t;
	tb_print("open: %llu ms", t);

#if 0
	// writ post
	if (post_size)
	{
		tb_http_bwrit(http, argv[2], post_size);
		tb_http_bfwrit(http, tb_null, 0);
	}
#endif

	// read data
	tb_byte_t 		data[8192];
	tb_size_t 		read = 0;
	tb_hize_t 		size = tb_http_status(http)->content_size;
	do
	{
		// read data
		tb_long_t real = tb_http_aread(http, data, 8192);
		tb_print("read: %d", real);
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
			tb_print("wait");
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

	// exit cookies
	if (cookies) 
	{
		// dump cookies
#ifdef __tb_debug__
		tb_cookies_dump(cookies);
#endif

		tb_cookies_exit(cookies);
	}

	// exit http
	if (http) tb_http_exit(http);
	return 0;
}
