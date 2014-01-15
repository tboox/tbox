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

	// the basc
	tb_hong_t 				basc;

	// the size
	tb_hize_t 				size;

	// is debug
	tb_bool_t 				debug;

	// is verbose
	tb_bool_t 				verbose;

}tb_demo_context_t;

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_bool_t tb_demo_istream_save_func(tb_astream_t* ast, tb_astream_t* ost, tb_size_t state, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(ast && context && context->option, tb_false);

	// ok
	if (state == TB_ASTREAM_STATE_OK)
	{
		// save size
		context->size += size;

		// print verbose info
		if (context->debug) tb_printf("save: %lu\n", size);

		// print verbose info
		if (context->verbose) 
		{
			if (tb_mclock() - context->basc > 1000) 
			{
				tb_printf("save: %llu bytes, speed: %llu bytes / s\n", context->size, (context->size * 1000) / (tb_mclock() - context->base));
				context->basc = tb_mclock();
			}
		}

	}
	// failed
	else
	{
		// print verbose info
		if (context->verbose) tb_printf("save: %llu bytes, speed: %llu bytes / s, state: %s\n", context->size, (context->size * 1000) / (tb_mclock() - context->base), tb_astream_state_cstr(state));

		// kill aicp
		tb_aicp_kill(tb_astream_aicp(ast));
	}

	// closed or failed or finished?
	if (state != TB_ASTREAM_STATE_OK) tb_astream_exit(ost);

	// ok?
	return (state == TB_ASTREAM_STATE_OK)? tb_true : tb_false;
}
static tb_bool_t tb_demo_istream_open_func(tb_astream_t* ast, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(ast && context && context->option, tb_false);

	// done
	tb_bool_t 		ok = tb_false;
	tb_astream_t* 	stream = tb_null;
	do
	{
		// check
		if (state != TB_ASTREAM_STATE_OK)
		{
			// print verbose info
			if (context->verbose) 
			{
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(ast, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: %s\n", url, tb_astream_state_cstr(state));
			}
			break;
		}
	
		// trace
		if (context->verbose) tb_printf("open: ok\n");

		// init stream
		if (tb_option_find(context->option, "more0"))
		{
			// the path
			tb_char_t const* path = tb_option_item_cstr(context->option, "more0");

			// init
			stream = tb_astream_init_from_url(tb_astream_aicp(ast), path);

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
				stream = tb_astream_init_from_url(tb_astream_aicp(ast), path);

				// print verbose info
				if (context->verbose) tb_printf("save: %s: ..\n", path);
			}
		}
		tb_assert_and_check_break(stream);

#if 0
		// ctrl stream
		if (tb_astream_type(stream) == TB_ASTREAM_TYPE_FILE) 
			tb_astream_ctrl(stream, TB_ASTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

		// try to open stream only for file
		if (!tb_astream_try_open(stream)) 
		{
			if (context->verbose) 
			{	
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(stream, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: failed\n", url);
			}
			break;
		}
	
		// save stream
		context->base = tb_mclock();
		context->basc = context->base;
		context->size = 0;
//		if (!tb_astream_save(ast, stream, tb_demo_istream_save_func, context)) break;
#endif

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit stream
		if (stream) tb_astream_exit(stream);

		// kill aicp
		tb_aicp_kill(tb_astream_aicp(ast));
	}

	// ok?
	return ok;
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
tb_int_t tb_demo_stream_astream_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_aicp_t* 			aicp = tb_null;
	tb_astream_t* 		stream = tb_null;
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
			context.debug = tb_option_find(context.option, "debug");
			context.verbose = tb_option_find(context.option, "no-verbose")? tb_false : tb_true;
		
			// done url
			if (tb_option_find(context.option, "url")) 
			{
				// init aicp
				aicp = tb_aicp_init(2);
				tb_assert_and_check_break(aicp);

				// init stream
				stream = tb_astream_init_from_url(aicp, tb_option_item_cstr(context.option, "url"));
				tb_assert_and_check_break(stream);

				// set timeout
				if (tb_option_find(context.option, "timeout"))
				{
					tb_size_t timeout = tb_option_item_uint32(context.option, "timeout");
					tb_astream_ctrl(stream, TB_ASTREAM_CTRL_SET_TIMEOUT, &timeout);
				}

				// print verbose info
				if (context.verbose) tb_printf("open: %s: ..\n", tb_option_item_cstr(context.option, "url"));

				// open stream
				if (!tb_astream_open(stream, tb_demo_istream_open_func, &context)) 
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

	// exit istream
	if (stream) tb_astream_exit(stream);
	stream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;

	// exit option
	if (context.option) tb_option_exit(context.option);
	context.option = tb_null;

	return 0;
}
