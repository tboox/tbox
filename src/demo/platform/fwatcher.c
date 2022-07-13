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
        tb_fwatcher_entry_ref_t entry = tb_fwatcher_entry_add(fwatcher, argv[1], TB_FWATCHER_EVENT_ALL);
        if (entry)
        {
            tb_fwatcher_event_t events[256];
            while (tb_fwatcher_entry_wait(fwatcher, entry, -1) > 0)
            {
                tb_size_t count = tb_fwatcher_entry_events(fwatcher, entry, events, tb_arrayn(events));
                for (tb_size_t i = 0; i < count; i++)
                {
                    tb_fwatcher_event_t const* event = &events[i];
                    tb_char_t const* status = event->event == TB_FWATCHER_EVENT_CREATE? "create" :
                        (event->event == TB_FWATCHER_EVENT_MODIFY? "modify" : "delete");
                    tb_trace_i("watch: %s %s", event->filepath, status);
                }
            }
            tb_fwatcher_entry_remove(fwatcher, entry);
        }
        tb_fwatcher_exit(fwatcher);
    }
    return 0;
}
