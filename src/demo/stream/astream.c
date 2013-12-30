/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
	// the option 
	tb_handle_t 		option;

	// the istream
	tb_astream_t* 		istream;

	// the ostream
	tb_astream_t* 		ostream;

}tb_demo_context_t;

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_bool_t tb_demo_ostream_open_func(tb_astream_t* ast, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(ast && context && context->option && context->istream, tb_false);
	
	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(ast);
	tb_assert_and_check_return_val(aicp, tb_false);

	// verbose
	tb_bool_t verbose = tb_option_find(context->option, "no-verbose")? tb_false : tb_true;
	
	// done
	tb_bool_t ok = tb_false;
	do
	{
		// open failed?
		if (state != TB_ASTREAM_STATE_OK)
		{
			// print verbose info
			if (verbose) 
			{
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(ast, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: failed\n", url);
			}

			break;
		}

		// read it
		if (tb_astream_read(context->istream) < 0)
		{
			// print verbose info
			if (verbose) 
			{
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(context->istream, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("read: %s: failed\n", url);
			}

			break;
		}

		// ok 
		ok = tb_true;

	} while (0);

	// kill aicp
	if (!ok) tb_aicp_kill(aicp);

	// ok?
	return ok;
}
static tb_long_t tb_demo_ostream_writ_func(tb_astream_t* ast, tb_size_t state, tb_hize_t offset, tb_size_t size, tb_size_t left, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(ast && context && context->option && context->istream, tb_false);

	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(ast);
	tb_assert_and_check_return_val(aicp, tb_false);

	// verbose
	tb_bool_t verbose = tb_option_find(context->option, "no-verbose")? tb_false : tb_true;
	
	// done
	tb_bool_t ok = tb_false;
	do
	{
		// trace
		if (verbose) tb_printf("writ: size: %lu, left: %lu, offset: %llu\n", size, left, offset);

		// ok?
		if (state != TB_ASTREAM_STATE_OK)
		{
			// print verbose info
			if (verbose) tb_printf("writ: state: %lu\n", state);
			break;
		}

		// no left?
		if (!left)
		{
			// continue to read
			if (tb_astream_read(context->istream) < 0)
			{
				// print verbose info
				if (verbose) tb_printf("read: failed\n");
				break;
			}

			// ok
			ok = 1;
		}
		// continue to writ
		else ok = 0;

	} while (0);

	// kill aicp
	if (ok < 0) tb_aicp_kill(aicp);

	// ok?
	return ok;
}
static tb_bool_t tb_demo_istream_open_func(tb_astream_t* ast, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(ast && context && context->option, tb_false);

	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(ast);
	tb_assert_and_check_return_val(aicp, tb_false);

	// verbose
	tb_bool_t verbose = tb_option_find(context->option, "no-verbose")? tb_false : tb_true;
	
	// done
	tb_bool_t ok = tb_false;
	do
	{
		// open failed?
		if (state != TB_ASTREAM_STATE_OK)
		{
			// print verbose info
			if (verbose) 
			{
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(ast, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: failed\n", url);
			}

			break;
		}

		// init func
		tb_astream_func_t func = 
		{
			context
		,	tb_demo_ostream_open_func
		,	tb_null
		,	tb_demo_ostream_writ_func
		, 	tb_null
		, 	tb_null
		};
		
		// init stream
		if (tb_option_find(context->option, "more0"))
		{
			// the path
			tb_char_t const* path = tb_option_item_cstr(context->option, "more0");

			// init
			context->ostream = tb_astream_init_from_url(&func, aicp, path);

			// print verbose info
			if (verbose) tb_printf("save: %s\n", path);
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
				context->ostream = tb_astream_init_from_url(&func, aicp, path);

				// print verbose info
				if (verbose) tb_printf("save: %s\n", path);
			}
		}
		tb_assert_and_check_break(context->ostream);

		// ctrl stream
		if (tb_astream_type(context->ostream) == TB_ASTREAM_TYPE_FILE) 
			tb_astream_ctrl(context->ostream, TB_ASTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

		// open stream
		if (tb_astream_open(context->ostream) < 0)
		{
			// print verbose info
			if (verbose) 
			{
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(context->ostream, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: failed\n", url);
			}

			break;
		}

		// ok
		ok = tb_true;

	} while (0);

	// kill aicp
	if (!ok) tb_aicp_kill(aicp);

	// ok?
	return ok;
}
static tb_long_t tb_demo_istream_read_func(tb_astream_t* ast, tb_size_t state, tb_hize_t offset, tb_byte_t const* data, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_demo_context_t* context = (tb_demo_context_t*)priv;
	tb_assert_and_check_return_val(ast && context && context->option && context->ostream && data, -1);

	// the aicp
	tb_aicp_t* aicp = tb_astream_aicp(ast);
	tb_assert_and_check_return_val(aicp, tb_false);

	// verbose
	tb_bool_t verbose = tb_option_find(context->option, "no-verbose")? tb_false : tb_true;
	
	// done
	tb_long_t ok = -1;
	do
	{
		// trace
		if (verbose) tb_printf("read: size: %lu, offset: %llu\n", size, offset);

		// ok?
		if (state != TB_ASTREAM_STATE_OK)
		{
			// print verbose info
			if (verbose) tb_printf("read: state: %lu\n", state);
			break;
		}

		// writ data
		if ((ok = tb_astream_writ(context->ostream, data, size)) < 0) 
		{
			// print verbose info
			if (verbose) tb_printf("writ: size: %lu: failed\n", size);
			break;
		}

		// writ pending?
		if (!ok)
		{
			// print verbose info
			if (verbose) tb_printf("writ: size: %lu: pending\n", size);

			// break read and wait writ finished
			ok = 1;
		}
		// ok? continue to read it
		else ok = 0;

	} while (0);

	// kill aicp
	if (ok < 0) tb_aicp_kill(aicp);

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
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(5 * 1024 * 1024), 5 * 1024 * 1024)) return 0;

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
			// verbose
			tb_bool_t verbose = tb_option_find(context.option, "no-verbose")? tb_false : tb_true;
		
			// done url
			if (tb_option_find(context.option, "url")) 
			{
				// init aicp
				aicp = tb_aicp_init(2);
				tb_assert_and_check_break(aicp);

				// init func
				tb_astream_func_t func = 
				{
					&context
				,	tb_demo_istream_open_func
				,	tb_demo_istream_read_func
				,	tb_null
				, 	tb_null
				, 	tb_null
				};
				
				// init stream
				context.istream = tb_astream_init_from_url(aicp, &func, tb_option_item_cstr(context.option, "url"));
				tb_assert_and_check_break(context.istream);

				// set timeout
				if (tb_option_find(context.option, "timeout"))
				{
					tb_size_t timeout = tb_option_item_uint32(context.option, "timeout");
					tb_astream_ctrl(context.istream, TB_ASTREAM_CTRL_SET_TIMEOUT, &timeout);
				}

				// print verbose info
				if (verbose) tb_printf("open: %s: ..\n", tb_option_item_cstr(context.option, "url"));

				// open stream
				if (tb_astream_open(context.istream) < 0) 
				{
					// print verbose info
					if (verbose) tb_printf("open: failed\n");
					break;
				}

				// loop aicp
				tb_aicp_loop(aicp);
			}
			else tb_option_help(context.option);
		}
		else tb_option_help(context.option);

	} while (0);

	// exit ostream
	if (context.ostream) tb_astream_exit(context.ostream);
	context.ostream = tb_null;

	// exit istream
	if (context.istream) tb_astream_exit(context.istream);
	context.istream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;

	// exit option
	if (context.option) tb_option_exit(context.option);
	context.option = tb_null;

	// exit tbox
	tb_exit();
	return 0;
}
