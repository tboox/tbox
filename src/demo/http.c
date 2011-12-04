/* ///////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////
 * macros
 */

/* ///////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_http_test_hfunc(tb_char_t const* line, tb_pointer_t priv)
{
	tb_printf("head: %s\n", line);
	return TB_TRUE;
}

/* ///////////////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init http
	tb_handle_t http = tb_http_init(TB_NULL);
	if (!http) goto end;

	// init cookies
	tb_cookies_t* cookies = tb_cookies_init();
	if (!cookies) goto end;
	
	// init option
	if (!tb_http_option_set_url(http, argv[1])) goto end;
	if (!tb_http_option_set_hfunc(http, tb_http_test_hfunc, http)) goto end;
	if (!tb_http_option_set_cookies(http, cookies)) goto end;
	if (!tb_http_option_set_kalive(http, TB_TRUE)) goto end;

	while (1)
	{
		// open http
		tb_int64_t base = tb_mclock();
		if (!tb_http_bopen(http)) goto end;

		// read data
		tb_byte_t 		data[8192];
		tb_size_t 		read = 0;
		tb_int64_t 		time = tb_mclock();
		tb_uint64_t 	size = tb_http_status_content_size(http);
		do
		{
			// read data
			tb_long_t n = tb_http_aread(http, data, 8192);
			if (n > 0)
			{
				// update read
				read += n;

				// update clock
				time = tb_mclock();
			}
			else if (!n) 
			{
				// timeout?
				if (tb_mclock() - time > 10000) break;

				// sleep some time
				tb_usleep(100);
			}
			else break;

			// is end?
			if (size && read >= size) break;

		} while(1);

end:

		// close it
		tb_http_bclose(http);

		// time
		tb_printf("\ntime: %lld ms\n", tb_mclock() - base);
	}

	// exit http
	if (http) tb_http_exit(http);

	// dump cookies
#ifdef TB_DEBUG
	tb_cookies_dump(cookies);
#endif

	// exit cookies
	if (cookies) tb_cookies_exit(cookies);

	// exit tbox
	tb_exit();

	return 0;
}
