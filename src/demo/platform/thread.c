/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_int_t tb_demo_thread_func(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx: %s]: init", self, priv);

    // exit 
    tb_thread_return(-1);

    // trace
    tb_trace_i("thread[%lx: %s]: exit", self, priv);

    // ok
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_thread_main(tb_int_t argc, tb_char_t** argv)
{
    // init thread
    tb_thread_ref_t thread = tb_thread_init(tb_null, tb_demo_thread_func, "hello", 0);
    if (thread)
    {
        // wait thread
        tb_int_t retval = 0;
        if (tb_thread_wait(thread, -1, &retval) > 0)
        {
            // trace
            tb_trace_i("wait: ok, retval: %d", retval);
        }
    
        // exit thread
        tb_thread_exit(thread);
    }

    // wait
    getchar();
    return 0;
}
