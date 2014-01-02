/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_bool_t tb_demo_istream_save_func(tb_astream_t* ast, tb_size_t state, tb_size_t real, tb_hize_t size, tb_pointer_t priv)
{
	// check
	tb_assert_and_check_return_val(ast, tb_false);
	
	// verbose
	tb_bool_t verbose = (tb_bool_t)priv;

	// ok
	if (state == TB_ASTREAM_STATE_OK)
	{
		// print verbose info
		if (verbose) tb_printf("save: %lu, size: %llu\n", real, size);
	}
	// failed
	else
	{
		// print verbose info
		if (verbose) tb_printf("save: %s\n", tb_astream_state_cstr(state));

		// kill aicp
		tb_aicp_kill(tb_astream_aicp(ast));
	}

	// closed or failed or finished?
	if (state != TB_ASTREAM_STATE_OK || real == size)
		tb_astream_exit(ast);

	// ok?
	return (state == TB_ASTREAM_STATE_OK)? tb_true : tb_false;
}
static tb_bool_t tb_demo_istream_open_func(tb_astream_t* ast, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_handle_t option = (tb_handle_t)priv;
	tb_assert_and_check_return_val(ast && option, tb_false);

	// verbose
	tb_bool_t verbose = tb_option_find(option, "no-verbose")? tb_false : tb_true;
	
	// done
	tb_bool_t 		ok = tb_false;
	tb_astream_t* 	stream = tb_null;
	do
	{
		// check
		if (state != TB_ASTREAM_STATE_OK)
		{
			// print verbose info
			if (verbose) 
			{
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(ast, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: %s\n", url, tb_astream_state_cstr(state));
			}
		}

		// init stream
		if (tb_option_find(option, "more0"))
		{
			// the path
			tb_char_t const* path = tb_option_item_cstr(option, "more0");

			// init
			stream = tb_astream_init_from_url(tb_astream_aicp(ast), path);

			// print verbose info
			if (verbose) tb_printf("save: %s\n", path);
		}
		else 
		{
			// the name
			tb_char_t const* name = tb_strrchr(tb_option_item_cstr(option, "url"), '/');
			if (!name) name = tb_strrchr(tb_option_item_cstr(option, "url"), '\\');
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
				if (verbose) tb_printf("save: %s\n", path);
			}
		}
		tb_assert_and_check_break(stream);

		// ctrl stream
		if (tb_astream_type(stream) == TB_ASTREAM_TYPE_FILE) 
			tb_astream_ctrl(stream, TB_ASTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

		// try to open stream only for file
		if (!tb_astream_try_open(stream)) 
		{
			if (verbose) 
			{	
				tb_char_t const* url = tb_null;
				tb_astream_ctrl(stream, TB_ASTREAM_CTRL_GET_URL, &url);
				tb_printf("open: %s: failed\n", url);
			}
			break;
		}

		// save stream
		if (!tb_astream_save(ast, stream, tb_demo_istream_save_func, (tb_pointer_t)verbose)) break;

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
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(5 * 1024 * 1024), 5 * 1024 * 1024)) return 0;

	// done
	tb_aicp_t* 			aicp = tb_null;
	tb_astream_t* 		stream = tb_null;
	tb_handle_t 		option = tb_null;
	do
	{
		// init option
		option = tb_option_init("astream", "the astream demo", g_options);
		tb_assert_and_check_break(option);
	
		// done option
		if (tb_option_done(option, argc - 1, &argv[1]))
		{
			// verbose
			tb_bool_t verbose = tb_option_find(option, "no-verbose")? tb_false : tb_true;
		
			// done url
			if (tb_option_find(option, "url")) 
			{
				// init aicp
				aicp = tb_aicp_init(2);
				tb_assert_and_check_break(aicp);

				// init stream
				stream = tb_astream_init_from_url(aicp, tb_option_item_cstr(option, "url"));
				tb_assert_and_check_break(stream);

				// set timeout
				if (tb_option_find(option, "timeout"))
				{
					tb_size_t timeout = tb_option_item_uint32(option, "timeout");
					tb_astream_ctrl(stream, TB_ASTREAM_CTRL_SET_TIMEOUT, &timeout);
				}

				// print verbose info
				if (verbose) tb_printf("open: %s: ..\n", tb_option_item_cstr(option, "url"));

				// open stream
				if (!tb_astream_open(stream, tb_demo_istream_open_func, option)) 
				{
					// print verbose info
					if (verbose) tb_printf("open: failed\n");
					break;
				}

				// loop aicp
				tb_aicp_loop(aicp);
			}
			else tb_option_help(option);
		}
		else tb_option_help(option);

	} while (0);

	// exit istream
	if (stream) tb_astream_exit(stream);
	stream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;

	// exit option
	if (option) tb_option_exit(option);
	option = tb_null;

	// exit tbox
	tb_exit();
	return 0;
}
