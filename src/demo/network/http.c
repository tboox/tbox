/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_http_test_hfunc(tb_http_option_t* option, tb_char_t const* line)
{
	tb_print("[demo]: response: %s", line);
	return TB_TRUE;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
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
	option->cookies = tb_cookies_init();
	tb_assert_and_check_goto(option->cookies, end);
	
	// init url
	if (!tb_url_set(&option->url, argv[1])) goto end;

	// keep-alive
	option->balive = 0;

	// redirect
//	option->rdtm = 0;

	// init head func
	option->hfunc = tb_http_test_hfunc;
	option->udata = (tb_pointer_t)http;

	// open http
	tb_int64_t t = tb_mclock();
	if (!tb_http_bopen(http)) goto end;
	t = tb_mclock() - t;
	tb_print("[demo]: open: %llu ms", t);

	// read data
	tb_byte_t 		data[8192];
	tb_size_t 		read = 0;
	tb_bool_t 		wait = TB_FALSE;
	tb_uint64_t 	size = status->content_size;
	do
	{
		// read data
		tb_long_t n = tb_http_aread(http, data, 8192);
		tb_print("[demo]: read: %d", n);
		if (n > 0)
		{
			// update read
			read += n;

			// no waiting
			wait = TB_FALSE;
		}
		else if (!n) 
		{
			// abort?
			tb_check_break(!wait);

			// wait
			tb_print("[demo]: wait");
			tb_long_t e = tb_http_wait(http, TB_AIOO_ETYPE_READ, option->timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);

			// be waiting
			wait = TB_TRUE;
		}
		else break;

		// is end?
		if (size && read >= size) break;

	} while(1);

end:

	// exit cookies
	if (option && option->cookies) 
	{
		// dump cookies
#ifdef TB_DEBUG
		tb_cookies_dump(option->cookies);
#endif

		tb_cookies_exit(option->cookies);
	}

	// exit http
	if (http) tb_http_exit(http);

	// exit tbox
	tb_exit();

	return 0;
}
