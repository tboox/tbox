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

	// utils
,	TB_DEMO_MAIN_ITEM(utils_url)
,	TB_DEMO_MAIN_ITEM(utils_crc)
,	TB_DEMO_MAIN_ITEM(utils_md5)
,	TB_DEMO_MAIN_ITEM(utils_sha)
,	TB_DEMO_MAIN_ITEM(utils_bits)
,	TB_DEMO_MAIN_ITEM(utils_dump)
,	TB_DEMO_MAIN_ITEM(utils_option)
,	TB_DEMO_MAIN_ITEM(utils_base32)
,	TB_DEMO_MAIN_ITEM(utils_base64)

	// other
,	TB_DEMO_MAIN_ITEM(other_test)
,	TB_DEMO_MAIN_ITEM(other_charset)

	// object
,	TB_DEMO_MAIN_ITEM(object_jsn)
,	TB_DEMO_MAIN_ITEM(object_bin)
,	TB_DEMO_MAIN_ITEM(object_xml)
,	TB_DEMO_MAIN_ITEM(object_dump)

	// stream
,	TB_DEMO_MAIN_ITEM(stream_tstream)
,	TB_DEMO_MAIN_ITEM(stream_astream)
,	TB_DEMO_MAIN_ITEM(stream_gstream)
,	TB_DEMO_MAIN_ITEM(stream_gstream_zip)
,	TB_DEMO_MAIN_ITEM(stream_gstream_null)
,	TB_DEMO_MAIN_ITEM(stream_gstream_charset)

	// string
,	TB_DEMO_MAIN_ITEM(string_pstring)
,	TB_DEMO_MAIN_ITEM(string_sstring)

	// memory
,	TB_DEMO_MAIN_ITEM(memory_check)
,	TB_DEMO_MAIN_ITEM(memory_fpool)
,	TB_DEMO_MAIN_ITEM(memory_gpool)
,	TB_DEMO_MAIN_ITEM(memory_spool)
,	TB_DEMO_MAIN_ITEM(memory_tpool)
,	TB_DEMO_MAIN_ITEM(memory_vpool)
,	TB_DEMO_MAIN_ITEM(memory_rpool)
,	TB_DEMO_MAIN_ITEM(memory_memops)
,	TB_DEMO_MAIN_ITEM(memory_scache)
,	TB_DEMO_MAIN_ITEM(memory_pbuffer)
,	TB_DEMO_MAIN_ITEM(memory_qbuffer)
,	TB_DEMO_MAIN_ITEM(memory_sbuffer)

	// network
,	TB_DEMO_MAIN_ITEM(network_dns)
,	TB_DEMO_MAIN_ITEM(network_url)
,	TB_DEMO_MAIN_ITEM(network_ipv4)
,	TB_DEMO_MAIN_ITEM(network_http)
,	TB_DEMO_MAIN_ITEM(network_whois)
,	TB_DEMO_MAIN_ITEM(network_cookies)

	// platform
,	TB_DEMO_MAIN_ITEM(platform_file)
,	TB_DEMO_MAIN_ITEM(platform_lock)
,	TB_DEMO_MAIN_ITEM(platform_path)
,	TB_DEMO_MAIN_ITEM(platform_ctime)
,	TB_DEMO_MAIN_ITEM(platform_epool)
,	TB_DEMO_MAIN_ITEM(platform_event)
,	TB_DEMO_MAIN_ITEM(platform_utils)
,	TB_DEMO_MAIN_ITEM(platform_timer)
,	TB_DEMO_MAIN_ITEM(platform_tstore)
,	TB_DEMO_MAIN_ITEM(platform_ltimer)
,	TB_DEMO_MAIN_ITEM(platform_atomic)
,	TB_DEMO_MAIN_ITEM(platform_process)
,	TB_DEMO_MAIN_ITEM(platform_barrier)
,	TB_DEMO_MAIN_ITEM(platform_atomic64)
,	TB_DEMO_MAIN_ITEM(platform_backtrace)
,	TB_DEMO_MAIN_ITEM(platform_directory)
,	TB_DEMO_MAIN_ITEM(platform_exception)
,	TB_DEMO_MAIN_ITEM(platform_semaphore)

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
