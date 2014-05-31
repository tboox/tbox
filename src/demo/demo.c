/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the main item
#define TB_DEMO_MAIN_ITEM(name)         { #name, tb_demo_##name##_main }

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the demo type
typedef struct __tb_demo_t
{
    // the demo name
    tb_char_t const*    name;

    // the demo main
    tb_int_t            (*main)(tb_int_t argc, tb_char_t** argv);

}tb_demo_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the demos
static tb_demo_t g_demo[] = 
{
    // libc
    TB_DEMO_MAIN_ITEM(libc_time)
,   TB_DEMO_MAIN_ITEM(libc_wchar)
,   TB_DEMO_MAIN_ITEM(libc_string)
,   TB_DEMO_MAIN_ITEM(libc_stdlib)

    // libm
#ifdef TB_CONFIG_TYPE_FLOAT
,   TB_DEMO_MAIN_ITEM(libm_float)
,   TB_DEMO_MAIN_ITEM(libm_double)
#endif
,   TB_DEMO_MAIN_ITEM(libm_integer)

    // database
#ifdef TB_CONFIG_MODULE_HAVE_DATABASE
,   TB_DEMO_MAIN_ITEM(database_sql)
#endif

    // xml
#ifdef TB_CONFIG_MODULE_HAVE_XML
,   TB_DEMO_MAIN_ITEM(xml_reader)
,   TB_DEMO_MAIN_ITEM(xml_writer)
,   TB_DEMO_MAIN_ITEM(xml_document)
#endif

    // asio
#ifdef TB_CONFIG_MODULE_HAVE_ASIO
,   TB_DEMO_MAIN_ITEM(asio_dns)
,   TB_DEMO_MAIN_ITEM(asio_http)
,   TB_DEMO_MAIN_ITEM(asio_aiopc)
,   TB_DEMO_MAIN_ITEM(asio_aiopd)
,   TB_DEMO_MAIN_ITEM(asio_aicpc)
,   TB_DEMO_MAIN_ITEM(asio_aicpd)
#endif

    // math
#ifdef TB_CONFIG_TYPE_FLOAT
,   TB_DEMO_MAIN_ITEM(math_fixed)
#endif
,   TB_DEMO_MAIN_ITEM(math_random)

    // utils
,   TB_DEMO_MAIN_ITEM(utils_url)
,   TB_DEMO_MAIN_ITEM(utils_crc)
,   TB_DEMO_MAIN_ITEM(utils_md5)
,   TB_DEMO_MAIN_ITEM(utils_sha)
,   TB_DEMO_MAIN_ITEM(utils_bits)
,   TB_DEMO_MAIN_ITEM(utils_dump)
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
,   TB_DEMO_MAIN_ITEM(utils_option)
#endif
,   TB_DEMO_MAIN_ITEM(utils_base32)
,   TB_DEMO_MAIN_ITEM(utils_base64)

    // other
,   TB_DEMO_MAIN_ITEM(other_flv)
,   TB_DEMO_MAIN_ITEM(other_test)
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
,   TB_DEMO_MAIN_ITEM(other_charset)
#endif

    // object
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
,   TB_DEMO_MAIN_ITEM(object_json)
,   TB_DEMO_MAIN_ITEM(object_bin)
,   TB_DEMO_MAIN_ITEM(object_xml)
,   TB_DEMO_MAIN_ITEM(object_bplist)
,   TB_DEMO_MAIN_ITEM(object_xplist)
,   TB_DEMO_MAIN_ITEM(object_dump)
#endif

    // stream
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
,   TB_DEMO_MAIN_ITEM(stream_basic_stream)
#endif
,   TB_DEMO_MAIN_ITEM(stream_basic_stream_null)
,   TB_DEMO_MAIN_ITEM(stream_basic_stream_cache)
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
,   TB_DEMO_MAIN_ITEM(stream_basic_stream_charset)
#endif
#ifdef TB_CONFIG_MODULE_HAVE_ZIP
,   TB_DEMO_MAIN_ITEM(stream_basic_stream_zip)
#endif
#ifdef TB_CONFIG_MODULE_HAVE_ASIO
,   TB_DEMO_MAIN_ITEM(stream_transfer_pool)
,   TB_DEMO_MAIN_ITEM(stream_async_transfer)
,   TB_DEMO_MAIN_ITEM(stream_async_stream_null)
,   TB_DEMO_MAIN_ITEM(stream_async_stream_cache)
#   ifdef TB_CONFIG_MODULE_HAVE_OBJECT
,   TB_DEMO_MAIN_ITEM(stream_async_stream)
#   endif
#   ifdef TB_CONFIG_MODULE_HAVE_CHARSET
,   TB_DEMO_MAIN_ITEM(stream_async_stream_charset)
#   endif
#   ifdef TB_CONFIG_MODULE_HAVE_ZIP
,   TB_DEMO_MAIN_ITEM(stream_async_stream_zip)
#   endif
#endif

    // string
,   TB_DEMO_MAIN_ITEM(string_pool)
,   TB_DEMO_MAIN_ITEM(string_scoped_string)
,   TB_DEMO_MAIN_ITEM(string_static_string)

    // memory
,   TB_DEMO_MAIN_ITEM(memory_check)
,   TB_DEMO_MAIN_ITEM(memory_static_fixed_pool)
,   TB_DEMO_MAIN_ITEM(memory_global_pool)
,   TB_DEMO_MAIN_ITEM(memory_block_pool)
,   TB_DEMO_MAIN_ITEM(memory_tiny_pool)
,   TB_DEMO_MAIN_ITEM(memory_static_block_pool)
,   TB_DEMO_MAIN_ITEM(memory_fixed_pool)
,   TB_DEMO_MAIN_ITEM(memory_memops)
,   TB_DEMO_MAIN_ITEM(memory_scoped_buffer)
,   TB_DEMO_MAIN_ITEM(memory_queue_buffer)
,   TB_DEMO_MAIN_ITEM(memory_static_buffer)

    // network
,   TB_DEMO_MAIN_ITEM(network_dns)
,   TB_DEMO_MAIN_ITEM(network_url)
,   TB_DEMO_MAIN_ITEM(network_ipv4)
,   TB_DEMO_MAIN_ITEM(network_http)
,   TB_DEMO_MAIN_ITEM(network_date)
,   TB_DEMO_MAIN_ITEM(network_whois)
,   TB_DEMO_MAIN_ITEM(network_spider)
,   TB_DEMO_MAIN_ITEM(network_cookies)

    // platform
,   TB_DEMO_MAIN_ITEM(platform_file)
,   TB_DEMO_MAIN_ITEM(platform_lock)
,   TB_DEMO_MAIN_ITEM(platform_path)
,   TB_DEMO_MAIN_ITEM(platform_cache_time)
,   TB_DEMO_MAIN_ITEM(platform_event)
,   TB_DEMO_MAIN_ITEM(platform_utils)
,   TB_DEMO_MAIN_ITEM(platform_timer)
,   TB_DEMO_MAIN_ITEM(platform_thread_store)
,   TB_DEMO_MAIN_ITEM(platform_ltimer)
,   TB_DEMO_MAIN_ITEM(platform_atomic)
,   TB_DEMO_MAIN_ITEM(platform_process)
,   TB_DEMO_MAIN_ITEM(platform_barrier)
,   TB_DEMO_MAIN_ITEM(platform_atomic64)
,   TB_DEMO_MAIN_ITEM(platform_backtrace)
,   TB_DEMO_MAIN_ITEM(platform_directory)
,   TB_DEMO_MAIN_ITEM(platform_exception)
,   TB_DEMO_MAIN_ITEM(platform_semaphore)
,   TB_DEMO_MAIN_ITEM(platform_thread_pool)

    // container
,   TB_DEMO_MAIN_ITEM(container_heap)
,   TB_DEMO_MAIN_ITEM(container_hash)
,   TB_DEMO_MAIN_ITEM(container_list)
,   TB_DEMO_MAIN_ITEM(container_queue)
,   TB_DEMO_MAIN_ITEM(container_stack)
,   TB_DEMO_MAIN_ITEM(container_vector)
,   TB_DEMO_MAIN_ITEM(container_single_list)
,   TB_DEMO_MAIN_ITEM(container_bloom_filter)

    // algorithm
,   TB_DEMO_MAIN_ITEM(algorithm_find)
,   TB_DEMO_MAIN_ITEM(algorithm_sort)
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    // init tbox
#ifdef __tb_debug__
    if (!tb_init(tb_null, malloc(200 * 1024 * 1024), 200 * 1024 * 1024)) return 0;
#else
    if (!tb_init(tb_null, tb_null, 0)) return 0;
#endif

    // init 
    tb_int_t            ok = 0;
    tb_char_t const*    name = tb_null;

    // find the main func from the .demo file
    if (!name)
    {
        // init file
        tb_handle_t file = tb_file_init(".demo", TB_FILE_MODE_RO);
        if (file)
        {
            // read line
            tb_char_t line[8192] = {0};
            if (tb_file_read(file, (tb_byte_t*)line, sizeof(line) - 1))
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
        tb_trace_i("======================================================================");
        tb_trace_i("help: echo \"name\" > ./.demo");
        tb_trace_i("help:     ./demo.b args ...");
        tb_trace_i("help: or");
        tb_trace_i("help: ./demo.b name args ...");
        tb_trace_i("help: ");
        tb_trace_i("help: example: echo \"stream_basic_stream\" > ./.demo");
        tb_trace_i("help: example:     ./demo.b --help");
        tb_trace_i("help: example:     ./demo.b http://www.xxxxx.com /tmp/a");
        tb_trace_i("help: example: or");
        tb_trace_i("help: example: ./demo.b stream_basic_stream http://www.xxxxx.com /tmp/a");
        tb_trace_i("help: ");

        // walk name
        tb_size_t i = 0;
        tb_size_t n = tb_arrayn(g_demo);
        for (i = 0; i < n; i++) tb_trace_i("help: name: %s", g_demo[i].name);
    }

    // exit tbox
    tb_exit();

    // ok?
    return ok;
}
