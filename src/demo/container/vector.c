/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_VECTOR_GROW_SIZE             (256)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_vector_insert_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_insert_prev(vector, tb_vector_size(vector) >> 1, (tb_pointer_t)0xf);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_insert_prev(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xf);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xf);

    // clear it
    tb_vector_clear(vector);
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_insert_head_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_insert_head(vector, (tb_pointer_t)0xf);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_insert_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xf);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xf);

    // clear it
    tb_vector_clear(vector);
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_insert_tail_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_insert_tail(vector, (tb_pointer_t)0xf);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_insert_tail(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xf);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xf);

    // clear it
    tb_vector_clear(vector);
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_ninsert_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    // insert one first
    tb_vector_insert_head(vector, (tb_pointer_t)0xf);

    tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    tb_vector_ninsert_prev(vector, 1, (tb_pointer_t)0xf, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_ninsert_prev(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n + 1);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xf);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xf);

    // clear it
    tb_vector_clear(vector);
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_ninsert_head_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_ninsert_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xf);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xf);

    // clear it
    tb_vector_clear(vector);
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_ninsert_tail_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_hong_t t = tb_mclock();
    tb_vector_ninsert_tail(vector, (tb_pointer_t)0xf, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_ninsert_tail(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xf);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xf);

    // clear it
    tb_vector_clear(vector);
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_vector_remove_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_remove(vector, tb_vector_size(vector) >> 1);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_remove(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_remove_head_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_remove_head(vector);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_remove_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_remove_last_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_remove_last(vector);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_remove_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nremove_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_vector_nremove(vector, tb_iterator_head(vector), n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_nremove(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nremove_head_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_vector_nremove_head(vector, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_nremove_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nremove_last_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_vector_nremove_last(vector, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_nremove_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(!tb_vector_size(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_replace_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    __tb_volatile__ tb_size_t itor = tb_iterator_head(vector);
    __tb_volatile__ tb_size_t tail = tb_iterator_tail(vector);
    tb_hong_t t = tb_mclock();
    for (; itor != tail; itor = tb_iterator_next(vector, itor)) tb_vector_replace(vector, itor, (tb_pointer_t)0xd);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_replace(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xd);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xd);

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_replace_head_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_replace_head(vector, (tb_pointer_t)0xd);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_replace_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xd);

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_replace_last_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) tb_vector_replace_last(vector, (tb_pointer_t)0xd);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_replace_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xd);


    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nreplace_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_vector_nreplace(vector, tb_iterator_head(vector), (tb_pointer_t)0xd, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_nreplace(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xd);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xd);

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nreplace_head_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_vector_nreplace_head(vector, (tb_pointer_t)0xd, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_nreplace_head(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xd);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xd);

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_nreplace_last_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_vector_nreplace_last(vector, (tb_pointer_t)0xd, n);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_nreplace_last(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // check
    tb_assert(tb_vector_size(vector) == n);
    tb_assert(tb_vector_head(vector) == (tb_pointer_t)0xd);
    tb_assert(tb_vector_last(vector) == (tb_pointer_t)0xd);


    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}

static tb_size_t tb_vector_iterator_next_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_for_all (tb_char_t*, item, vector) tb_used(item);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_iterator_next(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_size_t tb_vector_iterator_prev_test()
{
    // init
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return_val(vector, 0);

    tb_size_t n = 10000;
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xf, n);
    tb_hong_t t = tb_mclock();
    tb_rfor_all (tb_char_t*, item, vector) tb_used(item);
    t = tb_mclock() - t;

    // time
    tb_trace_i("tb_vector_iterator_prev(%lu): %lld ms, size: %lu, maxn: %lu", n, t, tb_vector_size(vector), tb_vector_maxn(vector));

    // exit
    tb_vector_exit(vector);

    return n / ((tb_uint32_t)(t) + 1);
}
static tb_void_t tb_vector_int_dump(tb_vector_ref_t vector)
{
    tb_trace_i("tb_int_t size: %lu, maxn: %lu", tb_vector_size(vector), tb_vector_maxn(vector));
    tb_for_all (tb_char_t*, item, vector)
    {
        tb_trace_i("tb_int_t at[%lu]: %x", item_itor, item);
    }
}
static tb_void_t tb_vector_int_test()
{
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("insert:");
    tb_vector_ninsert_head(vector, (tb_pointer_t)0xa, 10);
    tb_vector_ninsert_tail(vector, (tb_pointer_t)0xf, 10);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)0);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)1);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)2);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)3);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)4);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)5);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)6);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)7);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)8);
    tb_vector_insert_prev(vector, 10, (tb_pointer_t)9);
    tb_vector_insert_head(vector, (tb_pointer_t)4);
    tb_vector_insert_head(vector, (tb_pointer_t)3);
    tb_vector_insert_head(vector, (tb_pointer_t)2);
    tb_vector_insert_head(vector, (tb_pointer_t)1);
    tb_vector_insert_head(vector, (tb_pointer_t)0);
    tb_vector_insert_tail(vector, (tb_pointer_t)5);
    tb_vector_insert_tail(vector, (tb_pointer_t)6);
    tb_vector_insert_tail(vector, (tb_pointer_t)7);
    tb_vector_insert_tail(vector, (tb_pointer_t)8);
    tb_vector_insert_tail(vector, (tb_pointer_t)9);
    tb_vector_int_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("remove:");
    tb_vector_nremove_head(vector, 5);
    tb_vector_nremove_last(vector, 5);
    tb_vector_int_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("replace:");
    tb_vector_nreplace_head(vector, (tb_pointer_t)0xf, 10);
    tb_vector_nreplace_last(vector, (tb_pointer_t)0xa, 10);
    tb_vector_replace_head(vector, 0);
    tb_vector_replace_last(vector, 0);
    tb_vector_int_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("clear:");
    tb_vector_clear(vector);
    tb_vector_int_dump(vector);

    tb_vector_exit(vector);
}
static tb_void_t tb_vector_str_dump(tb_vector_ref_t vector)
{
    tb_trace_i("str size: %lu, maxn: %lu", tb_vector_size(vector), tb_vector_maxn(vector));
    tb_for_all (tb_char_t*, item, vector)
        tb_trace_i("str at[%lu]: %s", item_itor, item);
}
static tb_void_t tb_vector_str_test()
{
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_str(tb_true));
    tb_assert_and_check_return(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("insert:");
    tb_vector_ninsert_head(vector, "HHHHHHHHHH", 10);
    tb_vector_ninsert_tail(vector, "TTTTTTTTTT", 10);
    tb_vector_insert_prev(vector, 10, "0000000000");
    tb_vector_insert_prev(vector, 10, "1111111111");
    tb_vector_insert_prev(vector, 10, "2222222222");
    tb_vector_insert_prev(vector, 10, "3333333333");
    tb_vector_insert_prev(vector, 10, "4444444444");
    tb_vector_insert_prev(vector, 10, "5555555555");
    tb_vector_insert_prev(vector, 10, "6666666666");
    tb_vector_insert_prev(vector, 10, "7777777777");
    tb_vector_insert_prev(vector, 10, "8888888888");
    tb_vector_insert_prev(vector, 10, "9999999999");
    tb_vector_insert_head(vector, "4444444444");
    tb_vector_insert_head(vector, "3333333333");
    tb_vector_insert_head(vector, "2222222222");
    tb_vector_insert_head(vector, "1111111111");
    tb_vector_insert_head(vector, "0000000000");
    tb_vector_insert_tail(vector, "5555555555");
    tb_vector_insert_tail(vector, "6666666666");
    tb_vector_insert_tail(vector, "7777777777");
    tb_vector_insert_tail(vector, "8888888888");
    tb_vector_insert_tail(vector, "9999999999");
    tb_vector_str_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("remove:");
    tb_vector_nremove_head(vector, 5);
    tb_vector_nremove_last(vector, 5);
    tb_vector_str_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("replace:");
    tb_vector_nreplace_head(vector, "TTTTTTTTTT", 10);
    tb_vector_nreplace_last(vector, "HHHHHHHHHH", 10);
    tb_vector_replace_head(vector, "OOOOOOOOOO");
    tb_vector_replace_last(vector, "OOOOOOOOOO");
    tb_vector_str_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("clear:");
    tb_vector_clear(vector);
    tb_vector_str_dump(vector);

    tb_vector_exit(vector);
}
static tb_void_t tb_vector_mem_dump(tb_vector_ref_t vector)
{
    tb_trace_i("ifm size: %lu, maxn: %lu", tb_vector_size(vector), tb_vector_maxn(vector));
    tb_for_all (tb_char_t*, item, vector)
    {
        tb_trace_i("ifm at[%lu]: %s", item_itor, item);
    }
}
static tb_void_t tb_vector_mem_free(tb_element_ref_t element, tb_pointer_t buff)
{
    tb_trace_i("ifm free: %s, priv: %s", buff, element->priv);
}
static tb_void_t tb_vector_mem_test()
{
    tb_vector_ref_t vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_mem(11, tb_vector_mem_free, "ifm"));
    tb_assert_and_check_return(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("insert:");
    tb_vector_ninsert_head(vector, "HHHHHHHHHH", 10);
    tb_vector_ninsert_tail(vector, "TTTTTTTTTT", 10);
    tb_vector_insert_prev(vector, 10, "0000000000");
    tb_vector_insert_prev(vector, 10, "1111111111");
    tb_vector_insert_prev(vector, 10, "2222222222");
    tb_vector_insert_prev(vector, 10, "3333333333");
    tb_vector_insert_prev(vector, 10, "4444444444");
    tb_vector_insert_prev(vector, 10, "5555555555");
    tb_vector_insert_prev(vector, 10, "6666666666");
    tb_vector_insert_prev(vector, 10, "7777777777");
    tb_vector_insert_prev(vector, 10, "8888888888");
    tb_vector_insert_prev(vector, 10, "9999999999");
    tb_vector_insert_head(vector, "4444444444");
    tb_vector_insert_head(vector, "3333333333");
    tb_vector_insert_head(vector, "2222222222");
    tb_vector_insert_head(vector, "1111111111");
    tb_vector_insert_head(vector, "0000000000");
    tb_vector_insert_tail(vector, "5555555555");
    tb_vector_insert_tail(vector, "6666666666");
    tb_vector_insert_tail(vector, "7777777777");
    tb_vector_insert_tail(vector, "8888888888");
    tb_vector_insert_tail(vector, "9999999999");
    tb_vector_mem_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("remove:");
    tb_vector_nremove_head(vector, 5);
    tb_vector_nremove_last(vector, 5);
    tb_vector_mem_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("replace:");
    tb_vector_nreplace_head(vector, "TTTTTTTTTT", 10);
    tb_vector_nreplace_last(vector, "HHHHHHHHHH", 10);
    tb_vector_replace_head(vector, "OOOOOOOOOO");
    tb_vector_replace_last(vector, "OOOOOOOOOO");
    tb_vector_mem_dump(vector);

    tb_trace_i("=============================================================");
    tb_trace_i("clear:");
    tb_vector_clear(vector);
    tb_vector_mem_dump(vector);

    tb_vector_exit(vector);
}
static tb_void_t tb_vector_perf_test()
{
    tb_size_t score = 0;
    tb_trace_i("=============================================================");
    tb_trace_i("insert performance:");
    score += tb_vector_insert_test();
    score += tb_vector_insert_head_test();
    score += tb_vector_insert_tail_test();
    score += tb_vector_ninsert_test();
    score += tb_vector_ninsert_head_test();
    score += tb_vector_ninsert_tail_test();

    tb_trace_i("=============================================================");
    tb_trace_i("remove performance:");
    score += tb_vector_remove_test();
    score += tb_vector_remove_head_test();
    score += tb_vector_remove_last_test();
    score += tb_vector_nremove_test();
    score += tb_vector_nremove_head_test();
    score += tb_vector_nremove_last_test();

    tb_trace_i("=============================================================");
    tb_trace_i("replace performance:");
    score += tb_vector_replace_test();
    score += tb_vector_replace_head_test();
    score += tb_vector_replace_last_test();
    score += tb_vector_nreplace_test();
    score += tb_vector_nreplace_head_test();
    score += tb_vector_nreplace_last_test();

    tb_trace_i("=============================================================");
    tb_trace_i("iterator performance:");
    score += tb_vector_iterator_next_test();
    score += tb_vector_iterator_prev_test();

    tb_trace_i("=============================================================");
    tb_trace_i("score: %lu", score / 100);

}
static tb_void_t tb_vector_test_itor_perf()
{
    // init vector
    tb_vector_ref_t    vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_size());
    tb_assert_and_check_return(vector);

    // reset random
    tb_random_reset(tb_true);

    // add items
    __tb_volatile__ tb_size_t n = 10000;
    while (n--) tb_vector_insert_tail(vector, (tb_pointer_t)(tb_size_t)tb_random_value());

    // done
    tb_hong_t t = tb_mclock();
    __tb_volatile__ tb_hize_t test[2] = {0};
    __tb_volatile__ tb_size_t itor = tb_iterator_head(vector);
    for (; itor != tb_iterator_tail(vector); )
    {
        __tb_volatile__ tb_size_t item = (tb_size_t)tb_iterator_item(vector, itor);
        if (!(((tb_size_t)item >> 25) & 0x1))
        {
            // remove, hack: the itor of the same item is mutable
            tb_vector_remove(vector, itor);

            // continue
            continue ;
        }
        else
        {
            test[0] += (tb_size_t)item;
            test[1]++;
        }

        itor = tb_iterator_next(vector, itor);
    }
    t = tb_mclock() - t;
    tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_vector_size(vector), t);

    // exit
    tb_vector_exit(vector);
}
static tb_bool_t tb_vector_test_walk_item(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
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
static tb_void_t tb_vector_test_walk_perf()
{
    // init vector
    tb_vector_ref_t    vector = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    tb_assert_and_check_return(vector);

    // reset random
    tb_random_reset(tb_true);

    // add items
    __tb_volatile__ tb_size_t n = 10000;
    while (n--) tb_vector_insert_tail(vector, (tb_pointer_t)(tb_size_t)tb_random_value());

    // done
    tb_hong_t t = tb_mclock();
    __tb_volatile__ tb_hize_t test[2] = {0};
    tb_remove_if(vector, tb_vector_test_walk_item, (tb_pointer_t)test);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("item: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], tb_vector_size(vector), t);

    // exit
    tb_vector_exit(vector);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_vector_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_vector_int_test();
    tb_vector_str_test();
    tb_vector_mem_test();
#endif

#if 1
    tb_vector_perf_test();
#endif

#if 1
    tb_vector_test_itor_perf();
    tb_vector_test_walk_perf();
#endif

    return 0;
}
