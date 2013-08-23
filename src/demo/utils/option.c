/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */ 
static tb_option_item_t g_options[] = 
{
	{ 	'-'
	, 	"demo"
	, 	TB_OPTION_MODE_VAL
	, 	TB_OPTION_TYPE_CSTR
	, 	"the demo:\n"
		"    demo0: the demo test0\n"
		"    demo1: the demo test1\n"
		"    demo2: the demo test2\n"
		"    demo3: the demo test3\n"
	}
,	{'-', 	"lower", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"display lower string"}
,	{'-', 	"upper", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"display upper string"}
,	{'i', 	"integer", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_INTEGER, 	"display integer value"}
,	{'f', 	"float", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_FLOAT, 		"display float value"}
,	{'b', 	"boolean", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_BOOL, 		"display boolean value"}
,	{'s', 	"string", 		TB_OPTION_MODE_KEY_VAL, 	TB_OPTION_TYPE_CSTR, 		"display string value"}
,	{'h', 	"help", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"display this help and exit"}
,	{'v', 	"version", 		TB_OPTION_MODE_KEY, 		TB_OPTION_TYPE_BOOL, 		"output version information and exit"}
,	{'-', 	"file0", 		TB_OPTION_MODE_VAL, 		TB_OPTION_TYPE_CSTR, 		"the file0 path"}
,	{'-', 	"file1", 		TB_OPTION_MODE_VAL, 		TB_OPTION_TYPE_CSTR, 		"the file1 path"}
,	{'-', 	tb_null, 		TB_OPTION_MODE_MORE, 		TB_OPTION_TYPE_NONE, 		tb_null}

};

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init option
	tb_handle_t option = tb_option_init("option", "the option command test demo", g_options);
	if (option)
	{
		// done option
		if (tb_option_done(option, argc - 1, &argv[1]))
		{
			// done dump
			tb_option_dump(option);

			// done help
			if (tb_option_find(option, "help"))
				tb_option_help(option);
			else
			{
				// done integer
				if (tb_option_find(option, "i"))
					tb_print("integer: %lld", tb_option_item_sint64(option, "i"));
				// done string
				if (tb_option_find(option, "s"))
					tb_print("string: %s", tb_option_item_cstr(option, "s"));
				// done float
				if (tb_option_find(option, "f"))
					tb_print("float: %f", tb_option_item_float(option, "f"));
				// done boolean
				if (tb_option_find(option, "b"))
					tb_print("boolean: %s", tb_option_item_bool(option, "b")? "y" : "n");
				// done demo
				if (tb_option_find(option, "demo"))
					tb_print("demo: %s", tb_option_item_cstr(option, "demo"));
				// done file0
				if (tb_option_find(option, "file0"))
					tb_print("file0: %s", tb_option_item_cstr(option, "file0"));
				// done file1
				if (tb_option_find(option, "file1"))
					tb_print("file1: %s", tb_option_item_cstr(option, "file1"));

				// done more
				tb_size_t more = 0;
				while (1)
				{
					tb_char_t name[64] = {0};
					tb_snprintf(name, 63, "more%lu", more++);
					if (tb_option_find(option, name))
						tb_print("%s: %s", name, tb_option_item_cstr(option, name));
					else break;
				}
			}
		}
		else tb_option_help(option);
	
		// exit option
		tb_option_exit(option);
	}

	// exit tbox
	tb_exit();
	return 0;
}
