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
    TB_DEMO_MAIN_ITEM(libc_stdlib)

    // utils
,   TB_DEMO_MAIN_ITEM(utils_bits)

    // other
,   TB_DEMO_MAIN_ITEM(other_test)

    // platform
,   TB_DEMO_MAIN_ITEM(platform_addrinfo)

    // container
,   TB_DEMO_MAIN_ITEM(container_list_entry)
,   TB_DEMO_MAIN_ITEM(container_single_list_entry)

#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
    // stackless coroutine
,   TB_DEMO_MAIN_ITEM(lo_coroutine_nest)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_lock)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_sleep)
,   TB_DEMO_MAIN_ITEM(lo_coroutine_switch)
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
#if (defined(__tb_valgrind__) && defined(TB_CONFIG_VALGRIND_HAVE_VALGRIND_STACK_REGISTER)) \
        || defined(__tb_sanitize_address__) || defined(__tb_sanitize_thread__)
    if (!tb_init(tb_null, tb_native_allocator())) return -1;
#else
    if (!tb_init(tb_null, tb_static_allocator((tb_byte_t*)malloc(1024 * 1024), 1024 * 1024))) return -1;
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
