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
static tb_void_t tb_demo_platform_context_test_func1(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
    // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(contexts);

    // loop
    tb_size_t count = 10;
    while (count--)
    {
        // trace
        tb_trace_i("func1: %lu", count);

        // switch to the func2
        tb_context_swap(contexts[1], contexts[2]);
    }

    // switch to the main function
    tb_context_switch(contexts[0]);
}
static tb_void_t tb_demo_platform_context_test_func2(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
     // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(contexts);

    // loop
    tb_size_t count = 10;
    while (count--)
    {
        // trace
        tb_trace_i("func2: %lu", count);

        // switch to the func1
        tb_context_swap(contexts[2], contexts[1]);
    }

    // switch to the main function
    tb_context_switch(contexts[0]);
}
static tb_void_t tb_demo_platform_context_test()
{ 
    // the buffers
    static tb_byte_t s_buffers[3][8192];

    // the stacks
    static tb_byte_t s_stacks[3][8192];

    // init contexts
    tb_context_ref_t contexts[3];
    contexts[0] = tb_context_init(s_buffers[0], sizeof(s_buffers[0]));
    contexts[1] = tb_context_init(s_buffers[1], sizeof(s_buffers[1]));
    contexts[2] = tb_context_init(s_buffers[2], sizeof(s_buffers[2]));

    // make context1
    tb_context_make(contexts[1], s_stacks[1], sizeof(s_stacks[1]), tb_demo_platform_context_test_func1, contexts);

    // make context2
    tb_context_make(contexts[2], s_stacks[2], sizeof(s_stacks[2]), tb_demo_platform_context_test_func2, contexts);

    // trace
    tb_trace_i("test: enter");

    // switch to func1
    tb_context_swap(contexts[0], contexts[1]);

    // trace
    tb_trace_i("test: leave");

    // exit contexts
    tb_context_exit(contexts[0]);
    tb_context_exit(contexts[1]);
    tb_context_exit(contexts[2]);
}
static tb_void_t tb_demo_platform_context_perf_func1(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
    // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(contexts);

    // loop
    __tb_volatile__ tb_size_t count = COUNT >> 1;
    while (count--)
    {
        // switch to the func2
        tb_context_swap(contexts[1], contexts[2]);
    }

    // switch to the main function
    tb_context_switch(contexts[0]);
}
static tb_void_t tb_demo_platform_context_perf_func2(tb_uint32_t priv_hi, tb_uint32_t priv_lo)
{
     // check
    tb_context_ref_t* contexts = (tb_context_ref_t*)tb_context_priv(priv_hi, priv_lo);
    tb_assert_and_check_return(contexts);

    // loop
    __tb_volatile__ tb_size_t count = COUNT >> 1;
    while (count--)
    {
        // switch to the func1
        tb_context_swap(contexts[2], contexts[1]);
    }

    // switch to the main function
    tb_context_switch(contexts[0]);
}
static tb_void_t tb_demo_platform_context_perf()
{ 
    // the buffers
    static tb_byte_t s_buffers[3][8192];

    // the stacks
    static tb_byte_t s_stacks[3][8192];

    // init contexts
    tb_context_ref_t contexts[3];
    contexts[0] = tb_context_init(s_buffers[0], sizeof(s_buffers[0]));
    contexts[1] = tb_context_init(s_buffers[1], sizeof(s_buffers[1]));
    contexts[2] = tb_context_init(s_buffers[2], sizeof(s_buffers[2]));

    // make context1
    tb_context_make(contexts[1], s_stacks[1], sizeof(s_stacks[1]), tb_demo_platform_context_perf_func1, contexts);

    // make context2
    tb_context_make(contexts[2], s_stacks[2], sizeof(s_stacks[2]), tb_demo_platform_context_perf_func2, contexts);

    // init start time
    tb_hong_t startime = tb_mclock();

    // switch to func1
    tb_context_swap(contexts[0], contexts[1]);

    // computing time
    tb_hong_t duration = tb_mclock() - startime;

    // trace
    tb_trace_i("perf: %d switches in %lld ms, %lld switches per second", COUNT, duration, (((tb_hong_t)1000 * COUNT) / duration));

    // exit contexts
    tb_context_exit(contexts[0]);
    tb_context_exit(contexts[1]);
    tb_context_exit(contexts[2]);
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
