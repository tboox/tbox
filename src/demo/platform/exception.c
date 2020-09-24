/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_int_t tb_demo_exception_test(tb_cpointer_t priv)
{
    // self
    __tb_volatile__ tb_size_t self = tb_thread_self();

    // trace
    tb_trace_i("thread[%lu]: init", self);

    // try0
//  tb_size_t i = 0; // FIXME: maybe restored after exception, will leak memory if i is handle
    __tb_volatile__ tb_size_t i = 0;
    __tb_try
    {
        tb_trace_i("thread[%lu]: try0: b: %lu", self, i++);
        __tb_try
        {
            tb_trace_i("thread[%lu]: try1: b: %lu", self, i++);
            __tb_try
            {
                // trace
                // FIXME: debug: if i is been stored in the stack, it will be modified after exception
                // FIXME: relase: if i is been stored in the register, it will be restored after exception
                tb_trace_i("thread[%lu]: try2: b: %lu", self, i++);

                // abort
    //          tb_memset(&i, 0, 8192); // FIXME
    //          *((__tb_volatile__ tb_size_t*)0) = 0;
                tb_abort();
    //          __tb_volatile__ tb_pointer_t p = tb_malloc0(10); tb_memset(p, 0, 8192);

                // trace
                tb_trace_i("thread[%lu]: try2: e: %lu", self, i++);
            }
            __tb_except(0)
            {
                tb_trace_i("thread[%lu]: except2: %lu", self, i++);
            }
            __tb_end
            tb_trace_i("thread[%lu]: try1: e: %lu", self, i++);
        }
        __tb_except(1)
        {
            tb_trace_i("thread[%lu]: except1: %lu", self, i++);
            tb_backtrace_dump("\t\t", tb_null, 10);
        }
        __tb_end
        tb_trace_i("thread[%lu]: try0: e: %lu", self, i);

        // abort
        tb_abort();

        // end
        tb_trace_i("thread[%lu]: end0: e: %lu", self, i);
    }
    __tb_except(1)
    {
        tb_trace_i("thread[%lu]: except0: %lu", self, i++);
        tb_backtrace_dump("\t", tb_null, 10);
    }
    __tb_end

    // trace
    tb_trace_i("thread[%lu]: exit: %lu", self, i);
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_exception_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_thread_init(tb_null, tb_demo_exception_test, tb_null, 0);
    tb_thread_init(tb_null, tb_demo_exception_test, tb_null, 0);
    tb_thread_init(tb_null, tb_demo_exception_test, tb_null, 0);
    tb_thread_init(tb_null, tb_demo_exception_test, tb_null, 0);

    // wait
    tb_getchar();
    return 0;
}
