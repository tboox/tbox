#include "tbox.h"
#include "stdio.h"

static tb_bool_t http_head_func(tb_char_t const* line, void* priv)
{
	tb_printf("head: %s\n", line);
	return TB_TRUE;
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// create http
	tb_handle_t http = tb_http_create(TB_NULL);
	if (!http) goto end;

	// create cookies
	tb_cookies_t* cookies = tb_cookies_create();
	if (!cookies) goto end;
	
	// init option
	if (TB_FALSE == tb_http_option_set_url(http, argv[1])) goto end;
	if (TB_FALSE == tb_http_option_set_head_func(http, http_head_func, http)) goto end;
	if (TB_FALSE == tb_http_option_set_cookies(http, cookies)) goto end;
	if (TB_FALSE == tb_http_option_set_kalive(http, TB_TRUE)) goto end;

	while (1)
	{
		// open http
		tb_size_t 		base = (tb_size_t)tb_clock();
		if (TB_FALSE == tb_http_open(http)) goto end;

		// open file
		tb_handle_t hfile = tb_file_open(argv[2], TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);
		if (!hfile) goto end;
		
		// read data
		tb_byte_t 		data[8192];
		tb_size_t 		read = 0;
		tb_size_t 		time = (tb_size_t)tb_clock();
		tb_size_t 		size = tb_http_status_content_size(http);
		do
		{
			tb_int_t ret = tb_http_read(http, data, 8192);
			//TB_DBG("ret: %d", ret);
			if (ret > 0)
			{
				read += ret;
				time = (tb_size_t)tb_clock();

#if 1
				tb_int_t write = 0;
				while (write < ret)
				{
					tb_int_t ret2 = tb_file_write(hfile, data + write, ret - write);
					if (ret2 > 0) write += ret2;
					else if (ret2 < 0) break;
				}
#endif

			}
			else if (!ret) 
			{
				tb_size_t timeout = ((tb_size_t)tb_clock()) - time;
				if (timeout > 10000) break;
			}
			else break;

			// update info
			if (time > base && ((time - base) % 1000)) 
			{
				tb_printf("speed: %5d kb/s, load: %8d kb\r", (read / (time - base)), read / 1000);
				fflush(stdout);
			}

			// is end?
			if (size && read >= size) break;

		} while(1);

end:

		// close file
		tb_file_close(hfile);

		// close it
		tb_http_close(http);

		// time
		tb_printf("\ntime: %d ms\n", ((tb_size_t)tb_clock() - base));
	}

	// destroy it
	if (http) tb_http_destroy(http);

	// dump cookies
#ifdef TB_DEBUG
	tb_cookies_dump(cookies);
#endif

	// destroy it
	if (cookies) tb_cookies_destroy(cookies);

	tb_printf("end\n");
	getchar();

	// exit tplat
	tb_exit();
	return 0;
}
