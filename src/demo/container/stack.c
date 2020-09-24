/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_stack_put_and_pop_test()
{
    // init
    tb_stack_ref_t stack = tb_stack_init(10, tb_element_long());
    tb_assert_and_check_return_val(stack, 0);

    tb_stack_put(stack, (tb_pointer_t)0);
    tb_stack_put(stack, (tb_pointer_t)1);
    tb_stack_put(stack, (tb_pointer_t)2);
    tb_stack_put(stack, (tb_pointer_t)3);
    tb_stack_put(stack, (tb_pointer_t)4);
    tb_stack_put(stack, (tb_pointer_t)5);
    tb_stack_put(stack, (tb_pointer_t)6);
    tb_stack_put(stack, (tb_pointer_t)7);
    tb_stack_put(stack, (tb_pointer_t)8);
    tb_stack_put(stack, (tb_pointer_t)9);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++)
    {
        tb_stack_pop(stack);
        tb_stack_put(stack, (tb_pointer_t)0xf);
    }
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_stack_put_and_pop(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_stack_size(stack), tb_stack_maxn(stack));

    // check
    tb_assert(tb_stack_size(stack) == 10);
    tb_assert(tb_stack_head(stack) == (tb_pointer_t)0x0);
    tb_assert(tb_stack_last(stack) == (tb_pointer_t)0xf);

    // clear it
    tb_stack_clear(stack);
    tb_assert(!tb_stack_size(stack));

    // exit
    tb_stack_exit(stack);

    return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_stack_iterator_next_test()
{
    // init
    tb_size_t n = 1000000;
    tb_stack_ref_t stack = tb_stack_init(n, tb_element_long());
    tb_assert_and_check_return_val(stack, 0);

    tb_size_t i;
    for (i = 0; i < n; i++) tb_stack_put(stack, (tb_pointer_t)0xf);
    tb_hong_t t = tb_mclock();
    tb_for_all (tb_char_t*, item, stack) tb_used(item);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_stack_iterator_next(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, tb_stack_size(stack), tb_stack_maxn(stack));

    // exit
    tb_stack_exit(stack);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_stack_iterator_prev_test()
{
    // init
    tb_size_t n = 1000000;
    tb_stack_ref_t stack = tb_stack_init(n, tb_element_long());
    tb_assert_and_check_return_val(stack, 0);

    tb_size_t i;
    for (i = 0; i < n; i++) tb_stack_put(stack, (tb_pointer_t)0xf);
    tb_hong_t t = tb_mclock();
    tb_rfor_all (tb_char_t*, item, stack) tb_used(item);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_stack_iterator_prev(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, tb_stack_size(stack), tb_stack_maxn(stack));

    // exit
    tb_stack_exit(stack);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_stack_perf_test()
{
    tb_size_t score = 0;
    tb_trace_i("=============================================================");
    tb_trace_i("put & pop performance:");
    score += tb_stack_put_and_pop_test();

    tb_trace_i("=============================================================");
    tb_trace_i("iterator performance:");
    score += tb_stack_iterator_next_test();
    score += tb_stack_iterator_prev_test();

    tb_trace_i("=============================================================");
    tb_trace_i("score: %lu", score / 100);

}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_stack_main(tb_int_t argc, tb_char_t** argv)
{
    tb_stack_perf_test();

    return 0;
}
