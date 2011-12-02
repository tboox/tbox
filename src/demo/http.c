#include "tbox.h"
#include "stdio.h"

static tb_bool_t http_head_func(tb_char_t const* line, tb_pointer_t priv)
{
	tb_printf("head: %s\n", line);
	return TB_TRUE;
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create http
	tb_handle_t http = tb_http_init(TB_NULL);
	if (!http) goto end;

	// init cookies
	tb_cookies_t* cookies = tb_cookies_init();
	if (!cookies) goto end;
	
	// init option
	if (!tb_http_option_set_url(http, argv[1])) goto end;
	if (!tb_http_option_set_head_func(http, http_head_func, http)) goto end;
	if (!tb_http_option_set_cookies(http, cookies)) goto end;
	if (!tb_http_option_set_kalive(http, TB_TRUE)) goto end;

	while (1)
	{
		// open http
		tb_int64_t 		base = tb_mclock();
		if (!tb_http_open(http)) goto end;

		// open file
		tb_handle_t hfile = tb_file_open(argv[2], TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);
		if (!hfile) goto end;
		
		// read data
		tb_byte_t 		data[8192];
		tb_size_t 		read = 0;
		tb_int64_t 		time = tb_mclock();
		tb_uint64_t 	size = tb_http_status_content_size(http);
		do
		{
			tb_long_t ret = tb_http_read(http, data, 8192);
			//tb_print("ret: %d", ret);
			if (ret > 0)
			{
				read += ret;
				time = tb_mclock();

#if 1
				tb_long_t writ = 0;
				while (writ < ret)
				{
					tb_long_t ret2 = tb_file_writ(hfile, data + writ, ret - writ);
					if (ret2 > 0) writ += ret2;
					else if (ret2 < 0) break;
				}
#endif

			}
			else if (!ret) 
			{
				if (tb_mclock() - time > 10000) break;
			}
			else break;

			// is end?
			if (size && read >= size) break;

		} while(1);

end:

		// close file
		tb_file_close(hfile);

		// close it
		tb_http_close(http);

		// time
		tb_printf("\ntime: %lld ms\n", tb_mclock() - base);
	}

	// destroy it
	if (http) tb_http_exit(http);

	// dump cookies
#ifdef TB_DEBUG
	tb_cookies_dump(cookies);
#endif

	// exit it
	if (cookies) tb_cookies_exit(cookies);

	tb_printf("end\n");
	getchar();

	// exit tplat
	tb_exit();
	return 0;
}
