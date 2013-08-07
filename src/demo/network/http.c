/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_http_demo_hfunc(tb_handle_t http, tb_char_t const* line)
{
	// check
	tb_assert_and_check_return_val(http && line, tb_false);

	// option 
	tb_http_option_t* 	option = tb_http_option(http);

	// cookies
	tb_cookies_t* 		cookies = option->udata;

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
static tb_handle_t tb_http_demo_sfunc_init(tb_handle_t gst)
{
	tb_print("[demo]: ssl: init: %p", gst);
	tb_handle_t sock = tb_null;
	if (gst && tb_gstream_type(gst)) 
		tb_gstream_ctrl(gst, TB_SSTREAM_CTRL_GET_HANDLE, &sock);
	return sock;
}
static tb_void_t tb_http_demo_sfunc_exit(tb_handle_t ssl)
{
	tb_print("[demo]: ssl: exit");
}
static tb_long_t tb_http_demo_sfunc_read(tb_handle_t ssl, tb_byte_t* data, tb_size_t size)
{
	tb_print("[demo]: ssl: read: %lu", size);
	return ssl? tb_socket_recv(ssl, data, size) : -1;
}
static tb_long_t tb_http_demo_sfunc_writ(tb_handle_t ssl, tb_byte_t const* data, tb_size_t size)
{
	tb_print("[demo]: ssl: writ: %lu", size);
	return ssl? tb_socket_send(ssl, data, size) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

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
	option->udata = tb_cookies_init();
	tb_assert_and_check_goto(option->udata, end);
	
	// init url
	if (!tb_url_set(&option->url, argv[1])) goto end;

	// keep-alive
	option->balive = 0;

	// timeout 
//	option->timeout = 10000;

	// redirect
//	option->rdtm = 0;

	// init ssl func
	option->sfunc.init = tb_http_demo_sfunc_init;
	option->sfunc.exit = tb_http_demo_sfunc_exit;
	option->sfunc.read = tb_http_demo_sfunc_read;
	option->sfunc.writ = tb_http_demo_sfunc_writ;

	// init head func
	option->hfunc = tb_http_demo_hfunc;

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
			tb_long_t e = tb_http_wait(http, TB_AIOO_ETYPE_READ, option->timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
		}
		else break;

		// is end?
		if (size && read >= size) break;

	} while(1);

end:

	// exit cookies
	if (option && option->udata) 
	{
		// dump cookies
#ifdef __tb_debug__
		tb_cookies_dump(option->udata);
#endif

		tb_cookies_exit(option->udata);
	}

	// exit http
	if (http) tb_http_exit(http);

	// exit tbox
	tb_exit();
	return 0;
}
