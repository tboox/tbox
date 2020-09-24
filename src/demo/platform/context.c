/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the switch count
#define COUNT       (10000000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_platform_context_test_func1(tb_context_from_t from)
{
    // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)from.priv;
    tb_assert_and_check_return(contexts);

    // save main context
    contexts[0] = from.context;

    // jump to context2
    from.context = contexts[2];

    // loop
    tb_size_t count = 10;
    while (count--)
    {
        // trace
        tb_trace_i("func1: %lu", count);

        // switch to the func2
        from = tb_context_jump(from.context, contexts);
    }

    // switch to the main function
    tb_context_jump(contexts[0], tb_null);
}
static tb_void_t tb_demo_platform_context_test_func2(tb_context_from_t from)
{
    // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)from.priv;
    tb_assert_and_check_return(contexts);

    // loop
    tb_size_t count = 10;
    while (count--)
    {
        // trace
        tb_trace_i("func2: %lu", count);

        // switch to the func1
        from = tb_context_jump(from.context, contexts);
    }

    // switch to the main function
    tb_context_jump(contexts[0], tb_null);
}
static tb_context_from_t test()
{
    tb_context_from_t from = {0};
    return from;
}
static tb_void_t tb_demo_platform_context_test()
{
    // the stacks
    static tb_context_ref_t contexts[3];
    static tb_byte_t        stacks1[8192];
    static tb_byte_t        stacks2[8192];

    test();

    // make context1
    contexts[1] = tb_context_make(stacks1, sizeof(stacks1), tb_demo_platform_context_test_func1);

    // make context2
    contexts[2] = tb_context_make(stacks2, sizeof(stacks2), tb_demo_platform_context_test_func2);

    // trace
    tb_trace_i("test: enter");

    // switch to func1
    tb_context_jump(contexts[1], contexts);

    // trace
    tb_trace_i("test: leave");
}
static tb_void_t tb_demo_platform_context_perf_func1(tb_context_from_t from)
{
    // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)from.priv;
    tb_assert_and_check_return(contexts);

    // save main context
    contexts[0] = from.context;

    // jump to context2
    from.context = contexts[2];

    // loop
    __tb_volatile__ tb_size_t count = COUNT >> 1;
    while (count--)
    {
        // switch to the func2
        from = tb_context_jump(from.context, contexts);
    }

    // switch to the main function
    tb_context_jump(contexts[0], tb_null);
}
static tb_void_t tb_demo_platform_context_perf_func2(tb_context_from_t from)
{
    // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)from.priv;
    tb_assert_and_check_return(contexts);

    // loop
    __tb_volatile__ tb_size_t count = COUNT >> 1;
    while (count--)
    {
        // switch to the func1
        from = tb_context_jump(from.context, contexts);
    }

    // switch to the main function
    tb_context_jump(contexts[0], tb_null);
}
static tb_void_t tb_demo_platform_context_perf()
{
    // the stacks
    static tb_context_ref_t contexts[3];
    static tb_byte_t        stacks1[8192];
    static tb_byte_t        stacks2[8192];

    // make context1
    contexts[1] = tb_context_make(stacks1, sizeof(stacks1), tb_demo_platform_context_perf_func1);

    // make context2
    contexts[2] = tb_context_make(stacks2, sizeof(stacks2), tb_demo_platform_context_perf_func2);

    // init start time
    tb_hong_t startime = tb_mclock();

    // switch to func1
    tb_context_jump(contexts[1], contexts);

    // computing time
    tb_hong_t duration = tb_mclock() - startime;

    // trace
    tb_trace_i("perf: %d switches in %lld ms, %lld switches per second", COUNT, duration, (((tb_hong_t)1000 * COUNT) / duration));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_context_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_platform_context_test();
    tb_demo_platform_context_perf();
    return 0;
}
