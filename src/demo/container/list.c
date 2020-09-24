/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_list_insert_prev_test()
{
    // init list
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    tb_size_t itor = tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_insert_prev(list, itor, (tb_pointer_t)0xd);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_insert_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(tb_list_size(list) == n + 1);
    tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
    tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

    // clear it
    tb_list_clear(list);

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_insert_next_test()
{
    // init list
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    tb_size_t itor = tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_insert_next(list, itor, (tb_pointer_t)0xd);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_insert_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(tb_list_size(list) == n + 1);
    tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
    tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

    // clear it
    tb_list_clear(list);

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_insert_head_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // done
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_insert_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(tb_list_size(list) == n);
    tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
    tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

    // clear it
    tb_list_clear(list);

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_insert_tail_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // done
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_insert_tail(list, (tb_pointer_t)0xd);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_insert_tail(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(tb_list_size(list) == n);
    tb_assert(tb_list_head(list) == (tb_pointer_t)0xd);
    tb_assert(tb_list_last(list) == (tb_pointer_t)0xd);

    // clear it
    tb_list_clear(list);

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_remove_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_size_t itor = tb_iterator_head(list);
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) itor = tb_list_remove(list, itor);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_remove(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(!tb_list_size(list));

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_remove_head_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_remove_head(list);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_remove_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(!tb_list_size(list));

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_remove_last_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_remove_last(list);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_remove_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(!tb_list_size(list));

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_replace_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_size_t itor = tb_iterator_head(list);
    tb_size_t tail = tb_iterator_tail(list);
    tb_hong_t t = tb_mclock();
    for (; itor != tail; itor = tb_iterator_next(list, itor)) tb_list_replace(list, itor, (tb_pointer_t)0xe);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_replace(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(tb_list_size(list) == n);
    tb_assert(tb_list_head(list) == (tb_pointer_t)0xe);
    tb_assert(tb_list_last(list) == (tb_pointer_t)0xe);

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_replace_head_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_replace_head(list, (tb_pointer_t)0xe);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_replace_head(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(tb_list_size(list) == n);
    tb_assert(tb_list_head(list) == (tb_pointer_t)0xe);

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_replace_last_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_list_replace_last(list, (tb_pointer_t)0xe);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_replace_last(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // check
    tb_assert(tb_list_size(list) == n);
    tb_assert(tb_list_last(list) == (tb_pointer_t)0xe);

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_iterator_next_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_size());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_hong_t t = tb_mclock();
    tb_for_all(tb_size_t, item, list) tb_used(item);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_iterator_next(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_iterator_prev_test()
{
    // init
    tb_list_ref_t list = tb_list_init(0, tb_element_size());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 1000000;
    for (i = 0; i < n; i++) tb_list_insert_head(list, (tb_pointer_t)0xd);

    // done
    tb_hong_t t = tb_mclock();
    tb_rfor_all(tb_size_t, item, list) tb_used(item);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("tb_list_iterator_prev(%d): %lld ms, size: %d, maxn: %d", n, t, tb_list_size(list), tb_list_maxn(list));

    // exit
    tb_list_exit(list);
}
static tb_void_t tb_list_int_dump(tb_list_ref_t list)
{
    // trace
    tb_trace_i("tb_int_t size: %d, maxn: %d", tb_list_size(list), tb_list_maxn(list));

    // done
    tb_for_all(tb_char_t*, item, list)
    {
        // trace
        tb_trace_i("tb_int_t at[%lx]: %x", item_itor, item);
    }
}
static tb_void_t tb_list_int_test()
{
    // done
    tb_list_ref_t   list = tb_null;
    tb_size_t       i;
    tb_size_t       j;
    do
    {
        // init list
        list = tb_list_init(0, tb_element_long());
        tb_assert_and_check_break(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("insert:");

        // insert head
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);
        tb_list_insert_head(list, (tb_pointer_t)0xa);

        // insert tail
        i = tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);
        tb_list_insert_tail(list, (tb_pointer_t)0xf);

        // insert prev
        j = tb_list_insert_prev(list, i, (tb_pointer_t)0);
        tb_list_insert_prev(list, i, (tb_pointer_t)1);
        tb_list_insert_prev(list, i, (tb_pointer_t)2);
        tb_list_insert_prev(list, i, (tb_pointer_t)3);
        tb_list_insert_prev(list, i, (tb_pointer_t)4);
        tb_list_insert_prev(list, i, (tb_pointer_t)5);
        tb_list_insert_prev(list, i, (tb_pointer_t)6);
        tb_list_insert_prev(list, i, (tb_pointer_t)7);
        tb_list_insert_prev(list, i, (tb_pointer_t)8);
        tb_list_insert_prev(list, i, (tb_pointer_t)9);

        // insert head
        tb_list_insert_head(list, (tb_pointer_t)4);
        tb_list_insert_head(list, (tb_pointer_t)3);
        tb_list_insert_head(list, (tb_pointer_t)2);
        tb_list_insert_head(list, (tb_pointer_t)1);
        tb_list_insert_head(list, (tb_pointer_t)0);

        // insert tail
        tb_list_insert_tail(list, (tb_pointer_t)5);
        tb_list_insert_tail(list, (tb_pointer_t)6);
        tb_list_insert_tail(list, (tb_pointer_t)7);
        tb_list_insert_tail(list, (tb_pointer_t)8);
        tb_list_insert_tail(list, (tb_pointer_t)9);

        // dump
        tb_list_int_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("remove:");

        // remove head
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);

        // remove last
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);

        // remove
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);

        // dump
        tb_list_int_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("replace:");

        // replace head
        tb_list_replace_head(list, (tb_pointer_t)0);

        // replace last
        tb_list_replace_last(list, (tb_pointer_t)1);

        // dump
        tb_list_int_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("moveto:");

        // move to head and tail
        tb_list_moveto_head(list, tb_iterator_last(list));
        tb_list_moveto_tail(list, tb_iterator_next(list, tb_iterator_head(list)));

        // dump
        tb_list_int_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("clear:");

        // clear
        tb_list_clear(list);

        // dump
        tb_list_int_dump(list);

    } while (0);

    // exit list
    if (list) tb_list_exit(list);
}

static tb_void_t tb_list_str_dump(tb_list_ref_t list)
{
    // trace
    tb_trace_i("str size: %d, maxn: %d", tb_list_size(list), tb_list_maxn(list));

    // done
    tb_for_all (tb_char_t*, item, list)
    {
        // trace
        tb_trace_i("str at[%lx]: %s", item_itor, item);
    }
}
static tb_void_t tb_list_str_test()
{
    // done
    tb_list_ref_t   list = tb_null;
    tb_size_t       i;
    tb_size_t       j;
    do
    {
        // init list
        list = tb_list_init(0, tb_element_str(tb_true));
        tb_assert_and_check_break(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("insert:");

        // insert head
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");

        // insert tail
        i = tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");

        // insert prev
        j = tb_list_insert_prev(list, i, "0000000000");
        tb_list_insert_prev(list, i, "1111111111");
        tb_list_insert_prev(list, i, "2222222222");
        tb_list_insert_prev(list, i, "3333333333");
        tb_list_insert_prev(list, i, "4444444444");
        tb_list_insert_prev(list, i, "5555555555");
        tb_list_insert_prev(list, i, "6666666666");
        tb_list_insert_prev(list, i, "7777777777");
        tb_list_insert_prev(list, i, "8888888888");
        tb_list_insert_prev(list, i, "9999999999");

        // insert head
        tb_list_insert_head(list, "4444444444");
        tb_list_insert_head(list, "3333333333");
        tb_list_insert_head(list, "2222222222");
        tb_list_insert_head(list, "1111111111");
        tb_list_insert_head(list, "0000000000");

        // insert tail
        tb_list_insert_tail(list, "5555555555");
        tb_list_insert_tail(list, "6666666666");
        tb_list_insert_tail(list, "7777777777");
        tb_list_insert_tail(list, "8888888888");
        tb_list_insert_tail(list, "9999999999");

        // dump
        tb_list_str_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("remove:");

        // remove head
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);

        // remove last
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);

        // remove
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);

        // dump
        tb_list_str_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("replace:");

        // replace head
        tb_list_replace_head(list, "0000000000");

        // replace last
        tb_list_replace_last(list, "1111111111");

        // dump
        tb_list_str_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("moveto:");

        // move to head and tail
        tb_list_moveto_head(list, tb_iterator_last(list));
        tb_list_moveto_tail(list, tb_iterator_next(list, tb_iterator_head(list)));

        // dump
        tb_list_str_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("clear:");

        // clear
        tb_list_clear(list);

        // dump
        tb_list_str_dump(list);

    } while (0);

    // exit list
    if (list) tb_list_exit(list);
}
static tb_void_t tb_list_mem_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_trace_i("ifm free: %s, priv: %s", buff, element->priv);
}
static tb_void_t tb_list_mem_dump(tb_list_ref_t list)
{
    // trace
    tb_trace_i("ifm size: %d, maxn: %d", tb_list_size(list), tb_list_maxn(list));

    // done
    tb_for_all (tb_char_t*, item, list)
    {
        // trace
        tb_trace_i("ifm at[%lx]: %s", item_itor, item);
    }
}
static tb_void_t tb_list_mem_test()
{
    // done
    tb_list_ref_t   list = tb_null;
    tb_size_t       i;
    tb_size_t       j;
    do
    {
        // init list
        list = tb_list_init(0, tb_element_mem(11, tb_list_mem_free, "mem"));
        tb_assert_and_check_break(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("insert:");

        // insert head
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");
        tb_list_insert_head(list, "AAAAAAAAAA");

        // insert tail
        i = tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");
        tb_list_insert_tail(list, "FFFFFFFFFF");

        // insert prev
        j = tb_list_insert_prev(list, i, "0000000000");
        tb_list_insert_prev(list, i, "1111111111");
        tb_list_insert_prev(list, i, "2222222222");
        tb_list_insert_prev(list, i, "3333333333");
        tb_list_insert_prev(list, i, "4444444444");
        tb_list_insert_prev(list, i, "5555555555");
        tb_list_insert_prev(list, i, "6666666666");
        tb_list_insert_prev(list, i, "7777777777");
        tb_list_insert_prev(list, i, "8888888888");
        tb_list_insert_prev(list, i, "9999999999");

        // insert head
        tb_list_insert_head(list, "4444444444");
        tb_list_insert_head(list, "3333333333");
        tb_list_insert_head(list, "2222222222");
        tb_list_insert_head(list, "1111111111");
        tb_list_insert_head(list, "0000000000");

        // insert tail
        tb_list_insert_tail(list, "5555555555");
        tb_list_insert_tail(list, "6666666666");
        tb_list_insert_tail(list, "7777777777");
        tb_list_insert_tail(list, "8888888888");
        tb_list_insert_tail(list, "9999999999");

        // dump
        tb_list_mem_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("remove:");

        // remove head
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);
        tb_list_remove_head(list);

        // remove last
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);
        tb_list_remove_last(list);

        // remove
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);
        j = tb_list_remove(list, j);

        // dump
        tb_list_mem_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("replace:");

        // replace head
        tb_list_replace_head(list, "0000000000");

        // replace last
        tb_list_replace_last(list, "1111111111");

        // dump
        tb_list_mem_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("moveto:");

        // move to head and tail
        tb_list_moveto_head(list, tb_iterator_last(list));
        tb_list_moveto_tail(list, tb_iterator_next(list, tb_iterator_head(list)));

        // dump
        tb_list_mem_dump(list);

        // trace
        tb_trace_i("=============================================================");
        tb_trace_i("clear:");

        // clear
        tb_list_clear(list);

        // dump
        tb_list_mem_dump(list);

    } while (0);

    // exit list
    if (list) tb_list_exit(list);
}
static tb_void_t tb_list_perf_test()
{
    // insert
    tb_list_insert_prev_test();
    tb_list_insert_next_test();
    tb_list_insert_head_test();
    tb_list_insert_tail_test();

    // remove
    tb_list_remove_test();
    tb_list_remove_head_test();
    tb_list_remove_last_test();

    // replace
    tb_list_replace_test();
    tb_list_replace_head_test();
    tb_list_replace_last_test();

    // iterator
    tb_list_iterator_next_test();
    tb_list_iterator_prev_test();
}
static tb_bool_t tb_list_test_walk_item(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // done
    tb_hize_t*  test = (tb_hize_t*)value;
    tb_size_t   i = (tb_size_t)item;
    tb_bool_t   ok = tb_false;
    if (!((i >> 25) & 0x1)) ok = tb_true;
    else
    {
        test[0] += i;
        test[1]++;
    }

    // ok?
    return ok;
}
static tb_void_t tb_list_walk_perf()
{
    // init list
    tb_list_ref_t list = tb_list_init(0, tb_element_long());
    tb_assert_and_check_return(list);

    // make list
    __tb_volatile__ tb_size_t n = 100000;
    while (n--) tb_list_insert_tail(list, (tb_pointer_t)(tb_size_t)tb_random_value());

    // done
    tb_hong_t t = tb_mclock();
    __tb_volatile__ tb_hize_t test[2] = {0};
    tb_remove_if(list, tb_list_test_walk_item, (tb_pointer_t)test);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_list_size(list), t);

    // exit list
    tb_list_exit(list);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_list_main(tb_int_t argc, tb_char_t** argv)
{
    tb_list_int_test();
    tb_list_str_test();
    tb_list_mem_test();

#if 1
    tb_list_perf_test();
#endif

#if 1
    tb_list_walk_perf();
#endif

    return 0;
}
