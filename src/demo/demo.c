/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the main item
#define TB_DEMO_MAIN_ITEM(name) 		{ #name, tb_demo_##name##_main }

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the demo type
typedef struct __tb_demo_t
{
	// the demo name
	tb_char_t const* 	name;

	// the demo main
	tb_int_t 			(*main)(tb_int_t argc, tb_char_t** argv);

}tb_demo_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the demos
static tb_demo_t g_demo[] = 
{
	// flv
	TB_DEMO_MAIN_ITEM(flv)

	// libc
,	TB_DEMO_MAIN_ITEM(libc_time)
,	TB_DEMO_MAIN_ITEM(libc_wchar)
,	TB_DEMO_MAIN_ITEM(libc_string)
,	TB_DEMO_MAIN_ITEM(libc_stdlib)

	// libm
, 	TB_DEMO_MAIN_ITEM(libm_float)
,	TB_DEMO_MAIN_ITEM(libm_double)
,	TB_DEMO_MAIN_ITEM(libm_integer)

	// asio
,	TB_DEMO_MAIN_ITEM(asio_addr)
,	TB_DEMO_MAIN_ITEM(asio_aiopc)
,	TB_DEMO_MAIN_ITEM(asio_aiopd)
,	TB_DEMO_MAIN_ITEM(asio_aicpc)
, 	TB_DEMO_MAIN_ITEM(asio_aicpd)

	// math
,	TB_DEMO_MAIN_ITEM(math_rand)
,	TB_DEMO_MAIN_ITEM(math_fixed)

	// other
,	TB_DEMO_MAIN_ITEM(other_test)
,	TB_DEMO_MAIN_ITEM(other_charset)

	// container
,	TB_DEMO_MAIN_ITEM(container_heap)
,	TB_DEMO_MAIN_ITEM(container_hash)
,	TB_DEMO_MAIN_ITEM(container_dlist)
,	TB_DEMO_MAIN_ITEM(container_slist)
,	TB_DEMO_MAIN_ITEM(container_queue)
,	TB_DEMO_MAIN_ITEM(container_stack)
,	TB_DEMO_MAIN_ITEM(container_vector)

	// algorithm
,	TB_DEMO_MAIN_ITEM(algorithm_find)
,	TB_DEMO_MAIN_ITEM(algorithm_sort)
};

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024)) return 0;

	// init 
	tb_int_t 			ok = 0;
	tb_char_t const* 	name = tb_null;

	// find the main func from the .demo file
	if (!name)
	{
		// init file
		tb_handle_t file = tb_file_init(".demo", TB_FILE_MODE_RO);
		if (file)
		{
			// read line
			tb_char_t line[8192] = {0};
			if (tb_file_read(file, line, sizeof(line) - 1))
			{
				tb_size_t i = 0;
				tb_size_t n = tb_arrayn(g_demo);
				for (i = 0; i < n; i++)
				{
					// find it?
					if (g_demo[i].name && !tb_strnicmp(g_demo[i].name, line, tb_strlen(g_demo[i].name)))
					{
						// save name
						name = g_demo[i].name;

						// done main
						ok = g_demo[i].main(argc, argv);
						break;
					}
				}
			}

			// exit file
			tb_file_exit(file);
		}
	}

	// find the main func from the first argument
	if (!name && argc > 1 && argv[1])
	{
		tb_size_t i = 0;
		tb_size_t n = tb_arrayn(g_demo);
		for (i = 0; i < n; i++)
		{
			// find it?
			if (g_demo[i].name && !tb_stricmp(g_demo[i].name, argv[1]))
			{
				// save name
				name = g_demo[i].name;

				// done main
				ok = g_demo[i].main(argc - 1, argv + 1);
				break;
			}
		}
	}

	// no this demo? help it
	if (!name)
	{
		tb_print("======================================================================");
		tb_print("help: echo \"name\" > ./.demo");
		tb_print("help:     ./demo.b args ...");
		tb_print("help: or");
		tb_print("help: ./demo.b name args ...");
		tb_print("help: ");
		tb_print("help: example: echo \"stream_gstream\" > ./.demo");
		tb_print("help: example:     ./demo.b --help");
		tb_print("help: example:     ./demo.b http://www.xxxxx.com /tmp/a");
		tb_print("help: example: or");
		tb_print("help: example: ./demo.b stream_gstream http://www.xxxxx.com /tmp/a");
	}

	// exit tbox
	tb_exit();

	// ok?
	return ok;
}
