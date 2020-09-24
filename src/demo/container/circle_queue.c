/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_circle_queue_put_and_pop_test()
{
    // init
    tb_circle_queue_ref_t queue = tb_circle_queue_init(10, tb_element_long());
    tb_assert_and_check_return(queue);

    // make queue
    tb_circle_queue_put(queue, (tb_pointer_t)0);
    tb_circle_queue_put(queue, (tb_pointer_t)1);
    tb_circle_queue_put(queue, (tb_pointer_t)2);
    tb_circle_queue_put(queue, (tb_pointer_t)3);
    tb_circle_queue_put(queue, (tb_pointer_t)4);
    tb_circle_queue_put(queue, (tb_pointer_t)5);
    tb_circle_queue_put(queue, (tb_pointer_t)6);
    tb_circle_queue_put(queue, (tb_pointer_t)7);
    tb_circle_queue_put(queue, (tb_pointer_t)8);
    tb_circle_queue_put(queue, (tb_pointer_t)9);

    // done
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++)
    {
        tb_circle_queue_pop(queue);
        tb_circle_queue_put(queue, (tb_pointer_t)0xf);
    }
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_circle_queue_put_and_pop(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_circle_queue_size(queue), tb_circle_queue_maxn(queue));

    // check
    tb_assert(tb_circle_queue_size(queue) == 10);
    tb_assert(tb_circle_queue_head(queue) == (tb_pointer_t)0xf);
    tb_assert(tb_circle_queue_last(queue) == (tb_pointer_t)0xf);

    // clear it
    tb_circle_queue_clear(queue);
    tb_assert(!tb_circle_queue_size(queue));

    // exit
    tb_circle_queue_exit(queue);
}
static tb_void_t tb_circle_queue_iterator_next_test()
{
    // init
    tb_size_t n = 1000000;
    tb_circle_queue_ref_t queue = tb_circle_queue_init(n, tb_element_long());
    tb_assert_and_check_return(queue);

    // make queue
    while (n--) tb_circle_queue_put(queue, (tb_pointer_t)0xf);

    // done
    tb_hong_t t = tb_mclock();
    tb_for_all (tb_char_t*, item, queue) tb_used(item);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_circle_queue_iterator_next(%lu): %lld ms, size: %lu, maxn: %lu", 1000000, t, tb_circle_queue_size(queue), tb_circle_queue_maxn(queue));

    // exit
    tb_circle_queue_exit(queue);
}
static tb_void_t tb_circle_queue_int_dump(tb_circle_queue_ref_t queue)
{
    tb_trace_i("tb_int_t size: %lu, maxn: %lu", tb_circle_queue_size(queue), tb_circle_queue_maxn(queue));
    tb_for_all (tb_char_t*, item, queue)
    {
        tb_trace_i("tb_int_t at[%lu]: %u", item_itor, item);
    }
}
static tb_void_t tb_circle_queue_int_test()
{
    tb_circle_queue_ref_t queue = tb_circle_queue_init(10, tb_element_long());
    tb_assert_and_check_return(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("put:");
    tb_circle_queue_put(queue, (tb_pointer_t)0);
    tb_circle_queue_put(queue, (tb_pointer_t)1);
    tb_circle_queue_put(queue, (tb_pointer_t)2);
    tb_circle_queue_put(queue, (tb_pointer_t)3);
    tb_circle_queue_put(queue, (tb_pointer_t)4);
    tb_circle_queue_put(queue, (tb_pointer_t)5);
    tb_circle_queue_put(queue, (tb_pointer_t)6);
    tb_circle_queue_put(queue, (tb_pointer_t)7);
    tb_circle_queue_put(queue, (tb_pointer_t)8);
    tb_circle_queue_put(queue, (tb_pointer_t)9);
    tb_circle_queue_int_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("pop:");
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_int_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("put:");
    tb_circle_queue_put(queue, (tb_pointer_t)0);
    tb_circle_queue_put(queue, (tb_pointer_t)1);
    tb_circle_queue_put(queue, (tb_pointer_t)2);
    tb_circle_queue_put(queue, (tb_pointer_t)3);
    tb_circle_queue_put(queue, (tb_pointer_t)4);
    tb_circle_queue_int_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("clear:");
    tb_circle_queue_clear(queue);
    tb_circle_queue_int_dump(queue);
    tb_circle_queue_exit(queue);
}
static tb_void_t tb_circle_queue_str_dump(tb_circle_queue_ref_t queue)
{
    tb_trace_i("str size: %lu, maxn: %lu", tb_circle_queue_size(queue), tb_circle_queue_maxn(queue));
    tb_for_all (tb_char_t*, item, queue)
    {
        tb_trace_i("str at[%lu]: %s", item_itor, item);
    }
}
static tb_void_t tb_circle_queue_str_test()
{
    tb_circle_queue_ref_t queue = tb_circle_queue_init(10, tb_element_str(tb_true));
    tb_assert_and_check_return(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("put:");
    tb_circle_queue_put(queue, "0000000000");
    tb_circle_queue_put(queue, "1111111111");
    tb_circle_queue_put(queue, "2222222222");
    tb_circle_queue_put(queue, "3333333333");
    tb_circle_queue_put(queue, "4444444444");
    tb_circle_queue_put(queue, "5555555555");
    tb_circle_queue_put(queue, "6666666666");
    tb_circle_queue_put(queue, "7777777777");
    tb_circle_queue_put(queue, "8888888888");
    tb_circle_queue_put(queue, "9999999999");
    tb_circle_queue_str_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("pop:");
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_str_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("put:");
    tb_circle_queue_put(queue, "0000000000");
    tb_circle_queue_put(queue, "1111111111");
    tb_circle_queue_put(queue, "2222222222");
    tb_circle_queue_put(queue, "3333333333");
    tb_circle_queue_put(queue, "4444444444");
    tb_circle_queue_str_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("clear:");
    tb_circle_queue_clear(queue);
    tb_circle_queue_str_dump(queue);
    tb_circle_queue_exit(queue);
}
static tb_void_t tb_circle_queue_mem_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_trace_i("ifm free: %s, priv: %s", buff, element->priv);
}
static tb_void_t tb_circle_queue_mem_dump(tb_circle_queue_ref_t queue)
{
    tb_trace_i("ifm size: %lu, maxn: %lu", tb_circle_queue_size(queue), tb_circle_queue_maxn(queue));
    tb_for_all (tb_char_t*, item, queue)
    {
        tb_trace_i("ifm at[%lu]: %s", item_itor, item);
    }
}
static tb_void_t tb_circle_queue_mem_test()
{
    tb_circle_queue_ref_t queue = tb_circle_queue_init(10, tb_element_mem(11, tb_circle_queue_mem_free, "ifm"));
    tb_assert_and_check_return(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("put:");
    tb_circle_queue_put(queue, "0000000000");
    tb_circle_queue_put(queue, "1111111111");
    tb_circle_queue_put(queue, "2222222222");
    tb_circle_queue_put(queue, "3333333333");
    tb_circle_queue_put(queue, "4444444444");
    tb_circle_queue_put(queue, "5555555555");
    tb_circle_queue_put(queue, "6666666666");
    tb_circle_queue_put(queue, "7777777777");
    tb_circle_queue_put(queue, "8888888888");
    tb_circle_queue_put(queue, "9999999999");
    tb_circle_queue_mem_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("pop:");
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_pop(queue);
    tb_circle_queue_mem_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("put:");
    tb_circle_queue_put(queue, "0000000000");
    tb_circle_queue_put(queue, "1111111111");
    tb_circle_queue_put(queue, "2222222222");
    tb_circle_queue_put(queue, "3333333333");
    tb_circle_queue_put(queue, "4444444444");
    tb_circle_queue_mem_dump(queue);

    tb_trace_i("=============================================================");
    tb_trace_i("clear:");
    tb_circle_queue_clear(queue);
    tb_circle_queue_mem_dump(queue);
    tb_circle_queue_exit(queue);
}

static tb_void_t tb_circle_queue_perf_test()
{
    tb_circle_queue_put_and_pop_test();
    tb_circle_queue_iterator_next_test();
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_circle_queue_main(tb_int_t argc, tb_char_t** argv)
{
    tb_circle_queue_int_test();
    tb_circle_queue_str_test();
    tb_circle_queue_mem_test();
    tb_circle_queue_perf_test();

    return 0;
}
