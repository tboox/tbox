/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_fwatcher_main(tb_int_t argc, tb_char_t** argv)
{
    tb_fwatcher_ref_t fwatcher = tb_fwatcher_init();
    if (fwatcher)
    {
        tb_trace_i("watching %s", argv[1]);
        if (tb_fwatcher_add(fwatcher, argv[1]))
        {
            tb_bool_t eof = tb_false;
            tb_long_t count = 0;
            tb_fwatcher_event_t events[256];
            while (!eof && (count = tb_fwatcher_wait(fwatcher, events, tb_arrayn(events), -1)) >= 0)
            {
                for (tb_size_t i = 0; i < count && !eof; i++)
                {
                    tb_fwatcher_event_t const* event = &events[i];
                    tb_char_t const* status = event->event == TB_FWATCHER_EVENT_CREATE? "created" :
                        (event->event == TB_FWATCHER_EVENT_MODIFY? "modified" : "deleted");
                    tb_trace_i("watch: %s %s", event->filepath, status);
                    if (tb_strstr(event->filepath, "eof"))
                        eof = tb_true;
                }
            }
        }
        tb_fwatcher_exit(fwatcher);
    }
    return 0;
}
