/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t g_stop = tb_false;
static tb_fwatcher_ref_t g_fwatcher = tb_null;
static tb_int_t tb_demo_watcher(tb_cpointer_t priv)
{
    tb_char_t const* path = (tb_char_t const*)priv;
    tb_fwatcher_ref_t fwatcher = tb_fwatcher_init();
    if (fwatcher)
    {
        g_fwatcher = fwatcher;
        tb_trace_i("watching %s", path);
        if (tb_fwatcher_add(fwatcher, path, tb_true))
        {
            tb_bool_t eof = tb_false;
            tb_fwatcher_event_t event;
            while (!eof && !g_stop && tb_fwatcher_wait(fwatcher, &event, -1) >= 0)
            {
                tb_char_t const* status = event.event == TB_FWATCHER_EVENT_CREATE? "created" :
                    (event.event == TB_FWATCHER_EVENT_MODIFY? "modified" : "deleted");
                tb_trace_i("watch: %s %s", event.filepath, status);
                if (tb_strstr(event.filepath, "eof"))
                    eof = tb_true;
            }
        }
        tb_fwatcher_exit(fwatcher);
    }
    g_fwatcher = tb_null;
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_fwatcher_main(tb_int_t argc, tb_char_t** argv)
{
    tb_thread_ref_t thread = tb_thread_init(tb_null, tb_demo_watcher, argv[1], 0);
    if (thread)
    {
        tb_getchar();
        g_stop = tb_true;
        if (g_fwatcher) tb_fwatcher_spak(g_fwatcher);
        tb_thread_wait(thread, -1, tb_null);
        tb_thread_exit(thread);
    }
    return 0;
}
