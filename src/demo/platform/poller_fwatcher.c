/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_poller_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(poller && object);

    // fwatcher?
    if (object->type == TB_POLLER_OBJECT_FWATCHER)
    {
        tb_fwatcher_event_t* event = (tb_fwatcher_event_t*)events;
        if (event)
        {
            tb_char_t const* status = event->event == TB_FWATCHER_EVENT_CREATE? "created" :
                (event->event == TB_FWATCHER_EVENT_MODIFY? "modified" : "deleted");
            tb_trace_i("watch: %s %s", event->filepath, status);
            if (tb_strstr(event->filepath, "eof"))
                tb_poller_kill(poller);
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_poller_fwatcher_main(tb_int_t argc, tb_char_t** argv)
{
    tb_poller_ref_t poller = tb_null;
    do
    {
        // init poller
        poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(poller);

        // attach poller to the current thread
        tb_poller_attach(poller);

        // init fwatcher
        tb_fwatcher_ref_t fwatcher = tb_fwatcher_init();
        tb_assert_and_check_break(fwatcher);

        // add watch directory
        tb_fwatcher_add(fwatcher, argv[1], tb_true);

        // insert fwatcher
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_FWATCHER;
        object.ref.fwatcher = fwatcher;
        tb_poller_insert(poller, &object, 0, tb_null);

        // wait events
        while (tb_poller_wait(poller, tb_demo_poller_event, -1) >= 0) ;

        // exit fwatcher
        tb_fwatcher_exit(fwatcher);

        // end
        tb_trace_i("finished");

    } while (0);

    // exit poller
    if (poller) tb_poller_exit(poller);
    poller = tb_null;
    return 0;
}
