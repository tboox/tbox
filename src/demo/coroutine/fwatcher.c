/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_demo_coroutine_watch(tb_cpointer_t priv)
{
    tb_char_t const* path = (tb_char_t const*)priv;
    tb_fwatcher_ref_t fwatcher = tb_fwatcher_init();
    if (fwatcher)
    {
        tb_trace_i("watching %s", path);
        if (tb_fwatcher_add(fwatcher, path, tb_true))
        {
            tb_bool_t eof = tb_false;
            tb_fwatcher_event_t event;
            while (!eof && tb_fwatcher_wait(fwatcher, &event, -1) >= 0)
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
}

static tb_void_t tb_demo_coroutine_sleep(tb_cpointer_t priv)
{
    while (1)
    {
        tb_trace_i("sleep ..");
        tb_sleep(1);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_fwatcher_main(tb_int_t argc, tb_char_t** argv)
{
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_coroutine_start(scheduler, tb_demo_coroutine_watch, argv[1], 0);
        tb_coroutine_start(scheduler, tb_demo_coroutine_sleep, tb_null, 0);

        // do loop
        tb_co_scheduler_loop(scheduler, tb_true);
        tb_co_scheduler_exit(scheduler);
    }

    return 0;
}
