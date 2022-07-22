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
,   TB_DEMO_MAIN_ITEM(libc_wcstombs)
,   TB_DEMO_MAIN_ITEM(libc_mbstowcs)

    // libm
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
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

    // regex
#ifdef TB_CONFIG_MODULE_HAVE_REGEX
,   TB_DEMO_MAIN_ITEM(regex)
#endif

    // math
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
,   TB_DEMO_MAIN_ITEM(math_fixed)
#endif
,   TB_DEMO_MAIN_ITEM(math_random)

    // utils
,   TB_DEMO_MAIN_ITEM(utils_url)
,   TB_DEMO_MAIN_ITEM(utils_bits)
,   TB_DEMO_MAIN_ITEM(utils_dump)
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
,   TB_DEMO_MAIN_ITEM(utils_option)
#endif
,   TB_DEMO_MAIN_ITEM(utils_base32)
,   TB_DEMO_MAIN_ITEM(utils_base64)

    // hash
#ifdef TB_CONFIG_MODULE_HAVE_HASH
,   TB_DEMO_MAIN_ITEM(hash_md5)
,   TB_DEMO_MAIN_ITEM(hash_sha)
,   TB_DEMO_MAIN_ITEM(hash_uuid)
,   TB_DEMO_MAIN_ITEM(hash_djb2)
,   TB_DEMO_MAIN_ITEM(hash_sdbm)
,   TB_DEMO_MAIN_ITEM(hash_crc8)
,   TB_DEMO_MAIN_ITEM(hash_crc16)
,   TB_DEMO_MAIN_ITEM(hash_crc32)
,   TB_DEMO_MAIN_ITEM(hash_fnv32)
,   TB_DEMO_MAIN_ITEM(hash_fnv64)
,   TB_DEMO_MAIN_ITEM(hash_adler32)
,   TB_DEMO_MAIN_ITEM(hash_benchmark)
#endif

    // other
,   TB_DEMO_MAIN_ITEM(other_test)
,   TB_DEMO_MAIN_ITEM(other_test_cpp)
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
,   TB_DEMO_MAIN_ITEM(other_charset)
#endif

    // object
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
,   TB_DEMO_MAIN_ITEM(object_jcat)
,   TB_DEMO_MAIN_ITEM(object_json)
,   TB_DEMO_MAIN_ITEM(object_bin)
,   TB_DEMO_MAIN_ITEM(object_xml)
,   TB_DEMO_MAIN_ITEM(object_bplist)
,   TB_DEMO_MAIN_ITEM(object_xplist)
,   TB_DEMO_MAIN_ITEM(object_dump)
,   TB_DEMO_MAIN_ITEM(object_writer)
#endif

    // stream
,   TB_DEMO_MAIN_ITEM(stream)
,   TB_DEMO_MAIN_ITEM(stream_null)
,   TB_DEMO_MAIN_ITEM(stream_cache)
,   TB_DEMO_MAIN_ITEM(stream_charset)
,   TB_DEMO_MAIN_ITEM(stream_zip)

    // string
,   TB_DEMO_MAIN_ITEM(string_string)
,   TB_DEMO_MAIN_ITEM(string_static_string)

    // memory
,   TB_DEMO_MAIN_ITEM(memory_check)
,   TB_DEMO_MAIN_ITEM(memory_fixed_pool)
,   TB_DEMO_MAIN_ITEM(memory_string_pool)
,   TB_DEMO_MAIN_ITEM(memory_large_allocator)
,   TB_DEMO_MAIN_ITEM(memory_small_allocator)
,   TB_DEMO_MAIN_ITEM(memory_default_allocator)
,   TB_DEMO_MAIN_ITEM(memory_memops)
,   TB_DEMO_MAIN_ITEM(memory_buffer)
,   TB_DEMO_MAIN_ITEM(memory_queue_buffer)
,   TB_DEMO_MAIN_ITEM(memory_static_buffer)
,   TB_DEMO_MAIN_ITEM(memory_impl_static_fixed_pool)

    // network
,   TB_DEMO_MAIN_ITEM(network_dns)
,   TB_DEMO_MAIN_ITEM(network_url)
,   TB_DEMO_MAIN_ITEM(network_ipv4)
,   TB_DEMO_MAIN_ITEM(network_ipv6)
,   TB_DEMO_MAIN_ITEM(network_unixaddr)
,   TB_DEMO_MAIN_ITEM(network_unix_echo_server)
,   TB_DEMO_MAIN_ITEM(network_unix_echo_client)
,   TB_DEMO_MAIN_ITEM(network_ping)
,   TB_DEMO_MAIN_ITEM(network_ipaddr)
,   TB_DEMO_MAIN_ITEM(network_hwaddr)
,   TB_DEMO_MAIN_ITEM(network_http)
,   TB_DEMO_MAIN_ITEM(network_whois)
,   TB_DEMO_MAIN_ITEM(network_cookies)
,   TB_DEMO_MAIN_ITEM(network_impl_date)

    // platform
,   TB_DEMO_MAIN_ITEM(platform_file)
,   TB_DEMO_MAIN_ITEM(platform_path)
,   TB_DEMO_MAIN_ITEM(platform_utils)
,   TB_DEMO_MAIN_ITEM(platform_sched)
,   TB_DEMO_MAIN_ITEM(platform_atomic)
,   TB_DEMO_MAIN_ITEM(platform_atomic32)
,   TB_DEMO_MAIN_ITEM(platform_atomic64)
,   TB_DEMO_MAIN_ITEM(platform_stdfile)
,   TB_DEMO_MAIN_ITEM(platform_process)
,   TB_DEMO_MAIN_ITEM(platform_ifaddrs)
,   TB_DEMO_MAIN_ITEM(platform_filelock)
,   TB_DEMO_MAIN_ITEM(platform_addrinfo)
,   TB_DEMO_MAIN_ITEM(platform_hostname)
,   TB_DEMO_MAIN_ITEM(platform_backtrace)
,   TB_DEMO_MAIN_ITEM(platform_directory)
,   TB_DEMO_MAIN_ITEM(platform_cache_time)
,   TB_DEMO_MAIN_ITEM(platform_environment)
,   TB_DEMO_MAIN_ITEM(platform_pipe_pair)
,   TB_DEMO_MAIN_ITEM(platform_named_pipe)
,   TB_DEMO_MAIN_ITEM(platform_fwatcher)
,   TB_DEMO_MAIN_ITEM(platform_lock)
,   TB_DEMO_MAIN_ITEM(platform_timer)
,   TB_DEMO_MAIN_ITEM(platform_ltimer)
,   TB_DEMO_MAIN_ITEM(platform_event)
,   TB_DEMO_MAIN_ITEM(platform_semaphore)
,   TB_DEMO_MAIN_ITEM(platform_thread)
,   TB_DEMO_MAIN_ITEM(platform_thread_pool)
,   TB_DEMO_MAIN_ITEM(platform_thread_local)
,   TB_DEMO_MAIN_ITEM(platform_poller_pipe)
,   TB_DEMO_MAIN_ITEM(platform_poller_client)
,   TB_DEMO_MAIN_ITEM(platform_poller_server)
,   TB_DEMO_MAIN_ITEM(platform_poller_process)
,   TB_DEMO_MAIN_ITEM(platform_poller_fwatcher)
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
,   TB_DEMO_MAIN_ITEM(platform_context)
#endif
#ifdef TB_CONFIG_EXCEPTION_ENABLE
,   TB_DEMO_MAIN_ITEM(platform_exception)
#endif

    // container
,   TB_DEMO_MAIN_ITEM(container_heap)
,   TB_DEMO_MAIN_ITEM(container_stack)
,   TB_DEMO_MAIN_ITEM(container_vector)
,   TB_DEMO_MAIN_ITEM(container_hash_map)
,   TB_DEMO_MAIN_ITEM(container_hash_set)
,   TB_DEMO_MAIN_ITEM(container_queue)
,   TB_DEMO_MAIN_ITEM(container_circle_queue)
,   TB_DEMO_MAIN_ITEM(container_list)
,   TB_DEMO_MAIN_ITEM(container_list_entry)
,   TB_DEMO_MAIN_ITEM(container_single_list)
,   TB_DEMO_MAIN_ITEM(container_single_list_entry)
,   TB_DEMO_MAIN_ITEM(container_bloom_filter)

    // algorithm
,   TB_DEMO_MAIN_ITEM(algorithm_find)
,   TB_DEMO_MAIN_ITEM(algorithm_sort)

    // coroutine
#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
,   TB_DEMO_MAIN_ITEM(coroutine_dns)
,   TB_DEMO_MAIN_ITEM(coroutine_nest)
,   TB_DEMO_MAIN_ITEM(coroutine_lock)
,   TB_DEMO_MAIN_ITEM(coroutine_ping)
,   TB_DEMO_MAIN_ITEM(coroutine_pipe)
,   TB_DEMO_MAIN_ITEM(coroutine_sleep)
,   TB_DEMO_MAIN_ITEM(coroutine_stream)
,   TB_DEMO_MAIN_ITEM(coroutine_switch)
,   TB_DEMO_MAIN_ITEM(coroutine_thread)
,   TB_DEMO_MAIN_ITEM(coroutine_channel)
,   TB_DEMO_MAIN_ITEM(coroutine_semaphore)
,   TB_DEMO_MAIN_ITEM(coroutine_process)
,   TB_DEMO_MAIN_ITEM(coroutine_process_pipe)
,   TB_DEMO_MAIN_ITEM(coroutine_fwatcher)
,   TB_DEMO_MAIN_ITEM(coroutine_echo_server)
,   TB_DEMO_MAIN_ITEM(coroutine_echo_client)
,   TB_DEMO_MAIN_ITEM(coroutine_unix_echo_server)
,   TB_DEMO_MAIN_ITEM(coroutine_unix_echo_client)
,   TB_DEMO_MAIN_ITEM(coroutine_file_server)
,   TB_DEMO_MAIN_ITEM(coroutine_file_client)
,   TB_DEMO_MAIN_ITEM(coroutine_http_server)
,   TB_DEMO_MAIN_ITEM(coroutine_spider)

    // stackless coroutine
,   TB_DEMO_MAIN_ITEM(lo_coroutine_nest)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_lock)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_sleep)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_switch)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_process)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_process_pipe)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_echo_server)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_echo_client)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_file_server)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_file_client)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_http_server)
#endif

};

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    // init tbox
#if 0
    if (!tb_init(tb_null, tb_default_allocator((tb_byte_t*)malloc(300 * 1024 * 1024), 300 * 1024 * 1024))) return -1;
#elif 0
    if (!tb_init(tb_null, tb_virtual_allocator())) return -1;
#elif 0
    if (!tb_init(tb_null, tb_static_allocator((tb_byte_t*)malloc(300 * 1024 * 1024), 300 * 1024 * 1024))) return -1;
#elif (defined(__tb_valgrind__) && defined(TB_CONFIG_VALGRIND_HAVE_VALGRIND_STACK_REGISTER)) \
        || defined(__tb_sanitize_address__) || defined(__tb_sanitize_thread__)
    if (!tb_init(tb_null, tb_native_allocator())) return -1;
#else
    if (!tb_init(tb_null, tb_null)) return -1;
#endif

    // find the main func from the first argument
    tb_int_t            ok = 0;
    tb_char_t const*    name = tb_null;
    if (argc > 1 && argv[1])
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
        tb_trace_i("Usages: xmake r demo [testname] arguments ...");
        tb_trace_i("");
        tb_trace_i(".e.g");
        tb_trace_i("    xmake r demo stream http://www.xxxxx.com /tmp/a");
        tb_trace_i("");

        // walk name
        tb_size_t i = 0;
        tb_size_t n = tb_arrayn(g_demo);
        for (i = 0; i < n; i++) tb_trace_i("testname: %s", g_demo[i].name);
    }

    // exit tbox
    tb_exit();

    // ok?
    return ok;
}
