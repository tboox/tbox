/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */ 

#define COUNT   (50)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_void_t tb_demo_poller_event(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(poller && object && object->type == TB_POLLER_OBJECT_PROC);

    // update count
    static tb_size_t count = 0;
    count++;

    // trace
    tb_trace_i("process(%p): exited %p, status: %ld, count: %lu", priv, object->ref.proc, events, count);

    // exit process
    tb_process_exit(object->ref.proc);

    // finished?
    if (count == COUNT)
        tb_poller_kill(poller);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_poller_process_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc >= 2, -1);

    // start file client
    tb_poller_ref_t poller = tb_null;
    do
    {
        // init poller
        poller = tb_poller_init(tb_null);
        tb_assert_and_check_break(poller);

        // attach poller to the current thread
        tb_poller_attach(poller);

        // start processes
        tb_size_t count = COUNT;
        while (count--)
        {
            tb_process_ref_t process = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), tb_null);
            if (process)
            {
                tb_poller_object_t object;
                object.type = TB_POLLER_OBJECT_PROC;
                object.ref.proc = process;
                tb_poller_insert(poller, &object, 0, (tb_cpointer_t)count);
            }
        }

        // wait events
        while (tb_poller_wait(poller, tb_demo_poller_event, -1) >= 0) ;

        // end
        tb_trace_i("finished");

    } while (0);

    // exit poller
    if (poller) tb_poller_exit(poller);
    poller = tb_null;
    return 0;
}
