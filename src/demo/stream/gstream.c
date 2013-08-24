/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_bool_t tb_gstream_demo_hfunc(tb_handle_t http, tb_char_t const* line)
{
	tb_printf("response: %s\n", line);
	return tb_true;
}
static tb_handle_t tb_gstream_demo_sfunc_init(tb_handle_t gst)
{
	tb_printf("ssl: init: %p\n", gst);
	tb_handle_t sock = tb_null;
	if (gst && tb_gstream_type(gst) == TB_GSTREAM_TYPE_SOCK) 
		tb_gstream_ctrl(gst, TB_SSTREAM_CTRL_GET_HANDLE, &sock);
	return sock;
}
static tb_void_t tb_gstream_demo_sfunc_exit(tb_handle_t ssl)
{
	tb_printf("ssl: exit\n");
}
static tb_long_t tb_gstream_demo_sfunc_read(tb_handle_t ssl, tb_byte_t* data, tb_size_t size)
{
	tb_printf("ssl: read: %lu\n", size);
	return ssl? tb_socket_recv(ssl, data, size) : -1;
}
static tb_long_t tb_gstream_demo_sfunc_writ(tb_handle_t ssl, tb_byte_t const* data, tb_size_t size)
{
	tb_printf("ssl: writ: %lu\n", size);
	return ssl? tb_socket_send(ssl, data, size) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */ 
static tb_option_item_t g_options[] = 
{
	{'-', 	"gzip", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"enable gzip" 				}
,	{'-', 	"no-verbose", 	TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"disable verbose info" 	 	}
,	{'d', 	"debug", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"enable debug info" 	 	}
,	{'h', 	"header", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"the custem http header" 	}
,	{'-', 	"post-data", 	TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"set the post data" 		}
,	{'-', 	"post-file", 	TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"set the post file" 		}
,	{'-', 	"timeout", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_INTEGER, 	"set the timeout" 			}
,	{'h', 	"help", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"display this help and exit"}
,	{'-', 	"url", 			TB_OPTION_MODE_VAL, 		TB_OPTION_TYPE_CSTR, 		"the url" 					}
,	{'-', 	tb_null, 		TB_OPTION_MODE_MORE, 		TB_OPTION_TYPE_NONE, 		tb_null 					}

};

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(5 * 1024 * 1024), 5 * 1024 * 1024)) return 0;

	// done
	tb_handle_t 	option = tb_null;
	tb_gstream_t* 	ist = tb_null;
	tb_gstream_t* 	ost = tb_null;
	tb_gstream_t* 	pst = tb_null;
	do
	{
		// init option
		option = tb_option_init("gstream", "the gstream demo", g_options);
		tb_assert_and_check_break(option);

		// done option
		if (tb_option_done(option, argc - 1, &argv[1]))
		{
			// debug & verbose
			tb_bool_t debug = tb_option_find(option, "debug");
			tb_bool_t verbose = tb_option_find(option, "no-verbose")? tb_false : tb_true;
			
			// done url
			if (tb_option_find(option, "url")) 
			{
				// init ist
				ist = tb_gstream_init_from_url(tb_option_item_cstr(option, "url"));
				tb_assert_and_check_break(ist);
	
				// ctrl http
				if (tb_gstream_type(ist) == TB_GSTREAM_TYPE_HTTP) 
				{
					// init hoption
					tb_http_option_t* hoption = tb_null;
					tb_gstream_ctrl(ist, TB_HSTREAM_CTRL_GET_OPTION, &hoption);
					if (hoption)
					{
						// enable gzip?
						if (tb_option_find(option, "gzip"))
						{
							// auto unzip
							hoption->bunzip = 1;

							// need gzip
							tb_hash_set(hoption->head, "Accept-Encoding", "gzip,deflate");
						}

						// enable debug?
						if (debug) hoption->hfunc = tb_gstream_demo_hfunc;

						// custem header?
						if (tb_option_find(option, "header"))
						{
							// init
							tb_pstring_t key;
							tb_pstring_t val;
							tb_pstring_init(&key);
							tb_pstring_init(&val);

							// done
							tb_bool_t 			k = tb_true;
							tb_char_t const* 	p = tb_option_item_cstr(option, "header");
							while (*p)
							{
								// is key?
								if (k)
								{
									if (*p != ':' && !tb_isspace(*p)) tb_pstring_chrcat(&key, *p++);
									else if (*p == ':') 
									{
										// skip ':'
										p++;

										// skip space
										while (*p && tb_isspace(*p)) p++;

										// is val now
										k = tb_false;
									}
									else p++;
								}
								// is val?
								else
								{
									if (*p != ';') tb_pstring_chrcat(&val, *p++);
									else
									{
										// skip ';'
										p++;

										// skip space
										while (*p && tb_isspace(*p)) p++;

										// set header
										if (tb_pstring_size(&key) && tb_pstring_size(&val))
										{
											if (debug) tb_printf("header: %s: %s\n", tb_pstring_cstr(&key), tb_pstring_cstr(&val));
											tb_hash_set(hoption->head, tb_pstring_cstr(&key), tb_pstring_cstr(&val));
										}

										// is key now
										k = tb_true;

										// clear key & val
										tb_pstring_clear(&key);
										tb_pstring_clear(&val);
									}
								}
							}

							// set header
							if (tb_pstring_size(&key) && tb_pstring_size(&val))
							{
								if (debug) tb_printf("header: %s: %s\n", tb_pstring_cstr(&key), tb_pstring_cstr(&val));
								tb_hash_set(hoption->head, tb_pstring_cstr(&key), tb_pstring_cstr(&val));
							}

							// exit 
							tb_pstring_exit(&key);
							tb_pstring_exit(&val);
						}

						// post-data?
						if (tb_option_find(option, "post-data"))
						{
							hoption->method = TB_HTTP_METHOD_POST;
							hoption->post 	= tb_strlen(tb_option_item_cstr(option, "post-data"));
							if (debug) tb_printf("post: %lu\n", hoption->post);
						}
						// post-file?
						else if (tb_option_find(option, "post-file"))
						{
							// exist?
							tb_file_info_t info = {0};
							if (tb_file_info(tb_option_item_cstr(option, "post-file"), &info) && info.type == TB_FILE_TYPE_FILE)
							{
								hoption->method = TB_HTTP_METHOD_POST;
								hoption->post 	= info.size;
								if (debug) tb_printf("post: %lu\n", hoption->post);
							}
						}
					}
				}

				// init ssl
				static tb_gstream_sfunc_t sfunc = 
				{
					tb_gstream_demo_sfunc_init
				,	tb_gstream_demo_sfunc_exit
				,	tb_gstream_demo_sfunc_read
				,	tb_gstream_demo_sfunc_writ
				};
				tb_gstream_ctrl(ist, TB_GSTREAM_CTRL_SET_SFUNC, &sfunc);

				// set timeout
				if (tb_option_find(option, "timeout"))
				{
					tb_size_t timeout = tb_option_item_uint32(option, "timeout");
					tb_gstream_ctrl(ist, TB_GSTREAM_CTRL_SET_TIMEOUT, &timeout);
				}

				// print verbose info
				if (verbose) tb_printf("open: %s: ..\n", tb_option_item_cstr(option, "url"));

				// open ist
				if (!tb_gstream_bopen(ist)) 
				{
					// print verbose info
					if (verbose) tb_printf("open: %s\n", tb_gstream_state_cstr(ist));
					break;
				}

				// print verbose info
				if (verbose) tb_printf("open: ok\n");

				// post-data?
				if (tb_option_find(option, "post-data"))
				{
					tb_char_t const* data = tb_option_item_cstr(option, "post-data");
#if 1
					if (!tb_gstream_bwrit(ist, data, tb_strlen(data))) break;
					if (!tb_gstream_bfwrit(ist, tb_null, 0)) break;
#else
					if (!tb_gstream_bfwrit(ist, data, tb_strlen(data))) break;
#endif
				}
				// post-file?
				else if (tb_option_find(option, "post-file"))
				{
					// init pst
					pst = tb_gstream_init_from_url(tb_option_item_cstr(option, "post-file"));
					tb_assert_and_check_break(pst);

					// open pst
					if (!tb_gstream_bopen(pst)) break;

					// writ ist
					if (!tb_gstream_save(pst, ist)) break;
					if (!tb_gstream_bfwrit(ist, tb_null, 0)) break;
				}

				// init ost
				if (tb_option_find(option, "more0"))
				{
					// the path
					tb_char_t const* path = tb_option_item_cstr(option, "more0");

					// init
					ost = tb_gstream_init_from_url(path);

					// print verbose info
					if (verbose) tb_printf("save: %s\n", path);
				}
				else 
				{
					// the name
					tb_char_t const* name = tb_strrchr(tb_option_item_cstr(option, "url"), '/');
					if (!name) name = tb_strrchr(tb_option_item_cstr(option, "url"), '\\');
					if (!name) name = "/gstream.file";

					// the path
					tb_char_t path[TB_PATH_MAXN] = {0};
					if (tb_directory_curt(path, TB_PATH_MAXN))
						tb_strcat(path, name);
					else break;

					// init file
					ost = tb_gstream_init_from_url(path);

					// print verbose info
					if (verbose) tb_printf("save: %s\n", path);
				}
				tb_assert_and_check_break(ost);

				// ctrl ost
				if (tb_gstream_type(ost) == TB_GSTREAM_TYPE_FILE) 
					tb_gstream_ctrl(ost, TB_FSTREAM_CTRL_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

				// open ost
				if (!tb_gstream_bopen(ost)) break;

				// read data
				tb_byte_t 		data[TB_GSTREAM_BLOCK_MAXN];
				tb_hize_t 		read = 0;
				tb_hize_t 		left = tb_gstream_left(ist);
				tb_hong_t 		base = tb_mclock();
				tb_hong_t 		basc = tb_mclock();
				do
				{
					// read data
					tb_long_t real = tb_gstream_aread(ist, data, TB_GSTREAM_BLOCK_MAXN);
					
					// print debug info
					if (debug) tb_printf("read: %ld\n", real);

					// has data?
					if (real > 0)
					{
						// writ data
						if (!tb_gstream_bwrit(ost, data, real)) break;

						// update read
						read += real;
					}
					// no data?
					else if (!real) 
					{
						// wait
						tb_long_t e = tb_gstream_wait(ist, TB_AIOO_ETYPE_READ, tb_gstream_timeout(ist));
						tb_assert_and_check_break(e >= 0);

						// timeout?
						tb_assert_and_check_break(e);

						// has read?
						tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
					}
					// end
					else break;

					// is end?
					if (left && read >= left) break;

					// print verbose info
					if (tb_mclock() - basc > 1000) 
					{
						if (verbose) tb_printf("save: %llu bytes, speed: %llu bytes / s\n", tb_gstream_offset(ist), (tb_gstream_offset(ist) * 1000) / (tb_mclock() - base));
						basc = tb_mclock();
					}

				} while(1);

				// print verbose info
				if (verbose) tb_printf("save: %llu bytes, size: %llu bytes, time: %llu ms, state: %s\n", read, tb_gstream_size(ist), tb_mclock() - base, tb_gstream_state_cstr(ist));
			}
			else tb_option_help(option);
		}
		else tb_option_help(option);

	} while (0);

	// exit pst
	if (pst) tb_gstream_exit(pst);
	pst = tb_null;

	// exit ist
	if (ist) tb_gstream_exit(ist);
	ist = tb_null;

	// exit ost
	if (ost) tb_gstream_exit(ost);
	ost = tb_null;

	// exit option
	if (option) tb_option_exit(option);
	option = tb_null;

	// exit tbox
	tb_exit();
	return 0;
}
