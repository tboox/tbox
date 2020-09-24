/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_thread_local_free(tb_cpointer_t priv)
{
    tb_trace_i("thread[%lx]: free: %p", tb_thread_self(), priv);
}
static tb_int_t tb_demo_thread_local_test(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();

    // trace
    tb_trace_i("thread[%lx]: init", self);

    // init the thread local, only once
    static tb_thread_local_t s_local = TB_THREAD_LOCAL_INIT;
    if (!tb_thread_local_init(&s_local, tb_demo_thread_local_free)) return -1;

    // init start time
    tb_hong_t time = tb_mclock();

    // done
    __tb_volatile__ tb_size_t count = 10000000;
    while (count--)
    {
        // attempt to get local variable
        tb_size_t local;
        if (!(local = (tb_size_t)tb_thread_local_get(&s_local)))
        {
            // init local variable
            if (tb_thread_local_set(&s_local, (tb_cpointer_t)self))
                local = self;
        }

        // check
        if (local != self)
        {
            // trace
            tb_trace_i("thread[%lx]: invalid value: %lx", self, local);
        }
    }

    // compile the interval time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("thread[%lx]: exit: %lld ms", self, time);

    // ok
    return 0;
}
static tb_int_t tb_demo_thread_local_stub(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();

    // trace
    tb_trace_i("thread[%lx]: init", self);

    // trace
    tb_trace_i("thread[%lx]: exit", self);

    // ok
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_thread_local_main(tb_int_t argc, tb_char_t** argv)
{
    tb_thread_init(tb_null, tb_demo_thread_local_test, tb_null, 0);
    tb_thread_init(tb_null, tb_demo_thread_local_test, tb_null, 0);
    tb_thread_init(tb_null, tb_demo_thread_local_test, tb_null, 0);
    tb_thread_init(tb_null, tb_demo_thread_local_test, tb_null, 0);
    tb_thread_init(tb_null, tb_demo_thread_local_stub, tb_null, 0);

    // wait
    tb_getchar();
    return 0;
}
