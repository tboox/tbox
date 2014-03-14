/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the context
typedef struct __tb_demo_context_t
{
	// the option
	tb_handle_t 			option;

	// the base
	tb_hong_t 				base;

	// is debug?
	tb_bool_t 				debug;

	// is verbose?
	tb_bool_t 				verbose;

	// the istream
	tb_astream_t* 			istream;

	// the ostream
	tb_astream_t* 			ostream;

	// the tstream
	tb_handle_t 			tstream;

}tb_demo_context_t;

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_bool_t tb_demo_http_post_func(tb_handle_t http, tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// percent
	tb_size_t percent = 0;
	if (size > 0) percent = (offset * 100) / size;
	else if (state == TB_STREAM_STATE_CLOSED) percent = 100;

	// trace
	tb_trace_i("post: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", save, rate, percent, tb_stream_state_cstr(state));

	// ok
	return tb_true;
}
static tb_bool_t tb_demo_tstream_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(context && context->option, tb_false);
 
	// print verbose info
	if (context->verbose) 
	{	
		// percent
		tb_size_t percent = 0;
		if (size > 0) percent = (offset * 100) / size;
		else if (state == TB_STREAM_STATE_CLOSED) percent = 100;

		// trace
		tb_printf("save: %llu bytes, rate: %lu bytes/s, percent: %lu%%, state: %s\n", save, rate, percent, tb_stream_state_cstr(state));
	}

	// failed? kill aicp
	if (state != TB_STREAM_STATE_OK)
		tb_aicp_kill(tb_astream_aicp(context->istream));

	// ok?
	return (state == TB_STREAM_STATE_OK)? tb_true : tb_false;
}
static tb_bool_t tb_demo_istream_open_func(tb_astream_t* ast, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(ast && context && context->option, tb_false);

	// done
	tb_bool_t 		ok = tb_false;
	do
	{
		// check
		if (state != TB_STREAM_STATE_OK)
		{
			// print verbose info
			if (context->verbose) 
			{
				tb_char_t const* url = tb_null;
				tb_stream_ctrl(ast, TB_STREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: %s\n", url, tb_stream_state_cstr(state));
			}
			break;
		}
	
		// trace
		if (context->verbose) tb_printf("open: ok\n");

		// init ostream
		if (tb_option_find(context->option, "more0"))
		{
			// the path
			tb_char_t const* path = tb_option_item_cstr(context->option, "more0");

			// init
			context->ostream = tb_astream_init_from_file(tb_astream_aicp(ast), path, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);

			// print verbose info
			if (context->verbose) tb_printf("save: %s: ..\n", path);
		}
		else 
		{
			// the name
			tb_char_t const* name = tb_strrchr(tb_option_item_cstr(context->option, "url"), '/');
			if (!name) name = tb_strrchr(tb_option_item_cstr(context->option, "url"), '\\');
			if (!name) name = "/astream.file";

			// the path
			tb_char_t path[TB_PATH_MAXN] = {0};
			if (tb_directory_curt(path, TB_PATH_MAXN))
			{
				// append name
				tb_strcat(path, name);

				// init file
				context->ostream = tb_astream_init_from_file(tb_astream_aicp(ast), path, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);

				// print verbose info
				if (context->verbose) tb_printf("save: %s: ..\n", path);
			}
		}
		tb_assert_and_check_break(context->ostream);

		// init tstream
		context->tstream = tb_tstream_init_aa(ast, context->ostream, 0);
		tb_assert_and_check_break(context->tstream);

		// open and save tstream
		if (!tb_tstream_osave(context->tstream, tb_demo_tstream_save_func, context)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed? kill aicp
	if (!ok) tb_aicp_kill(tb_astream_aicp(ast));

	// ok?
	return ok;
}
static tb_bool_t tb_demo_istream_head_func(tb_handle_t http, tb_char_t const* line, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(http && line, tb_false);

	// trace
	tb_trace_i("head: %s", line);

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */ 
static tb_option_item_t g_options[] = 
{
	{'-', 	"gzip", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"enable gzip" 				}
,	{'-', 	"no-verbose", 	TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"disable verbose info" 	 	}
,	{'d', 	"debug", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"enable debug info" 	 	}
,	{'k', 	"keep-alive", 	TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"keep alive" 				}
,	{'h', 	"header", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"the custem http header" 	}
,	{'-', 	"post-data", 	TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"set the post data" 		}
,	{'-', 	"post-file", 	TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"set the post file" 		}
,	{'-', 	"range", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"set the range" 			}
,	{'-', 	"timeout", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_INTEGER, 	"set the timeout" 			}
,	{'h', 	"help", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"display this help and exit"}
,	{'-', 	"url", 			TB_OPTION_MODE_VAL, 		TB_OPTION_TYPE_CSTR, 		"the url" 					}
,	{'-', 	tb_null, 		TB_OPTION_MODE_MORE, 		TB_OPTION_TYPE_NONE, 		tb_null 					}

};

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_astream_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_aicp_t* 			aicp = tb_null;
	tb_demo_context_t 	context = {0};
	do
	{
		// init option
		context.option = tb_option_init("astream", "the astream demo", g_options);
		tb_assert_and_check_break(context.option);
	
		// done option
		if (tb_option_done(context.option, argc - 1, &argv[1]))
		{
			// debug and verbose
			context.debug 	= tb_option_find(context.option, "debug")? tb_false : tb_true;
			context.verbose = tb_option_find(context.option, "no-verbose")? tb_false : tb_true;
		
			// done url
			if (tb_option_find(context.option, "url")) 
			{
				// init aicp
				aicp = tb_aicp_init(2);
				tb_assert_and_check_break(aicp);

				// init istream
				context.istream = tb_astream_init_from_url(aicp, tb_option_item_cstr(context.option, "url"));
				tb_assert_and_check_break(context.istream);

				// ctrl http
				if (tb_stream_type(context.istream) == TB_STREAM_TYPE_HTTP) 
				{
					// enable gzip?
					if (tb_option_find(context.option, "gzip"))
					{
						// auto unzip
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_AUTO_UNZIP, 1)) break;

						// need gzip
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_HEAD, "Accept-Encoding", "gzip,deflate")) break;
					}

					// enable debug?
					if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_HEAD_FUNC, context.debug? tb_demo_istream_head_func : tb_null)) break;

					// custem header?
					if (tb_option_find(context.option, "header"))
					{
						// init
						tb_pstring_t key;
						tb_pstring_t val;
						tb_pstring_init(&key);
						tb_pstring_init(&val);

						// done
						tb_bool_t 			k = tb_true;
						tb_char_t const* 	p = tb_option_item_cstr(context.option, "header");
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
										if (context.debug) tb_printf("header: %s: %s\n", tb_pstring_cstr(&key), tb_pstring_cstr(&val));
										if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_HEAD, tb_pstring_cstr(&key), tb_pstring_cstr(&val))) break;
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
							if (context.debug) tb_printf("header: %s: %s\n", tb_pstring_cstr(&key), tb_pstring_cstr(&val));
							if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_HEAD, tb_pstring_cstr(&key), tb_pstring_cstr(&val))) break;
						}

						// exit 
						tb_pstring_exit(&key);
						tb_pstring_exit(&val);
					}

					// keep alive?
					if (tb_option_find(context.option, "keep-alive"))
					{
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_HEAD, "Connection", "keep-alive")) break;
					}

					// post-data?
					if (tb_option_find(context.option, "post-data"))
					{
						tb_char_t const* 	post_data = tb_option_item_cstr(context.option, "post-data");
						tb_hize_t 			post_size = tb_strlen(post_data);
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_METHOD, TB_HTTP_METHOD_POST)) break;
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_POST_DATA, post_data, post_size)) break;
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_POST_FUNC, tb_demo_http_post_func)) break;
						if (context.debug) tb_printf("post: %llu\n", post_size);
					}
					// post-file?
					else if (tb_option_find(context.option, "post-file"))
					{
						tb_char_t const* url = tb_option_item_cstr(context.option, "post-file");
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_METHOD, TB_HTTP_METHOD_POST)) break;
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_POST_URL, url)) break;
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_POST_FUNC, tb_demo_http_post_func)) break;
						if (context.debug) tb_printf("post: %s\n", url);
					}
				}

				// set range
				if (tb_option_find(context.option, "range"))
				{
					tb_char_t const* p = tb_option_item_cstr(context.option, "range");
					if (p)
					{
						// the bof
						tb_hize_t eof = 0;
						tb_hize_t bof = tb_atoll(p);
						while (*p && tb_isdigit(*p)) p++;
						if (*p == '-')
						{
							p++;
							eof = tb_atoll(p);
						}
						if (!tb_stream_ctrl(context.istream, TB_STREAM_CTRL_HTTP_SET_RANGE, bof, eof)) break;
					}
				}

				// set timeout
				if (tb_option_find(context.option, "timeout"))
				{
					tb_size_t timeout = tb_option_item_uint32(context.option, "timeout");
					tb_stream_ctrl(context.istream, TB_STREAM_CTRL_SET_TIMEOUT, &timeout);
				}

				// print verbose info
				if (context.verbose) tb_printf("open: %s: ..\n", tb_option_item_cstr(context.option, "url"));

				// open istream
				if (!tb_astream_open(context.istream, tb_demo_istream_open_func, &context)) 
				{
					// print verbose info
					if (context.verbose) tb_printf("open: failed\n");
					break;
				}

				// loop aicp
				tb_aicp_loop(aicp);
			}
			else tb_option_help(context.option);
		}
		else tb_option_help(context.option);

	} while (0);

	// exit tstream
	if (context.tstream) tb_tstream_exit(context.tstream, tb_false);
	context.tstream = tb_null;

	// exit istream
	if (context.istream) tb_astream_exit(context.istream, tb_false);
	context.istream = tb_null;

	// exit ostream
	if (context.ostream) tb_astream_exit(context.ostream, tb_false);
	context.ostream = tb_null;

	// exit option
	if (context.option) tb_option_exit(context.option);
	context.option = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;

	return 0;
}
