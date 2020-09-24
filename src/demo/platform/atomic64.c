/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_test_atomic64_get()
{
    tb_trace_i("atomic64_get(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 0);
    tb_assert_and_check_return(tb_atomic64_get(&a) == 0);
    tb_atomic64_init(&a, 1);
    tb_assert_and_check_return(tb_atomic64_get(&a) == 1);
    tb_atomic64_init(&a, 1000);
    tb_assert_and_check_return(tb_atomic64_get(&a) == 1000);
    tb_atomic64_init(&a, 0xFFFFFFFFFFFFFFFFULL);
    tb_assert_and_check_return(tb_atomic64_get(&a) == 0xFFFFFFFFFFFFFFFFULL);
    tb_trace_i("atomic64_get(): test ok!");
}
static tb_void_t tb_test_atomic64_fetch_and_set()
{
    tb_trace_i("atomic64_fetch_and_set(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_set(&a, 1) == 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_set(&a, 2) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_set(&a, 0xFFFFFFFFFFFFFFFFULL) == 2);
    tb_assert_and_check_return(tb_atomic64_fetch_and_set(&a, 0xFFFFFFFFFFFFFFFFULL) == 0xFFFFFFFFFFFFFFFFULL);
    tb_trace_i("atomic64_fetch_and_set(): test ok!");
}
static tb_void_t tb_test_atomic64_fetch_and_cmpset()
{
    tb_trace_i("atomic64_fetch_and_cmpset(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_cmpset(&a, 0, 1) == 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_cmpset(&a, 1, 2) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_cmpset(&a, 10, 1000) == 2);
    tb_assert_and_check_return(tb_atomic64_fetch_and_cmpset(&a, 2, 0xFFFFFFFFFFFFFFFFULL) == 2);
    tb_assert_and_check_return(tb_atomic64_fetch_and_cmpset(&a, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL) == 0xFFFFFFFFFFFFFFFFULL);
    tb_trace_i("atomic64_fetch_and_cmpset(): test ok!");
}
static tb_void_t tb_test_atomic64_compare_and_swap()
{
    tb_trace_i("atomic64_compare_and_swap(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 0);
    tb_int64_t p = 0;
    tb_assert_and_check_return(tb_atomic64_compare_and_swap(&a, &p, 1) && p == 0);
    p = 1;
    tb_assert_and_check_return(tb_atomic64_compare_and_swap(&a, &p, 2) && p == 1);
    p = 10;
    tb_assert_and_check_return(!tb_atomic64_compare_and_swap(&a, &p, 1000) && p == 2);
    p = 2;
    tb_assert_and_check_return(tb_atomic64_compare_and_swap(&a, &p, 0xFFFFFFFFFFFFFFFFULL) && p == 2);
    p = 2;
    tb_assert_and_check_return(!tb_atomic64_compare_and_swap(&a, &p, 0xFFFFFFFFFFFFFFFFULL) && p == 0xFFFFFFFFFFFFFFFFULL);
    tb_trace_i("atomic64_compare_and_swap(): test ok!");
}
static tb_void_t tb_test_atomic64_fetch_and_add()
{
    tb_trace_i("atomic64_fetch_and_add(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_add(&a, 1) == 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_add(&a, 2) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_add(&a, 1000) == 3);
    tb_assert_and_check_return(tb_atomic64_fetch_and_add(&a, 1000) == 1003);
    tb_trace_i("atomic64_fetch_and_add(): test ok!");
}
static tb_void_t tb_test_atomic64_fetch_and_sub()
{
    tb_trace_i("atomic64_fetch_and_sub(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_sub(&a, 1) == 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_sub(&a, 2) == -1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_sub(&a, 1000) == -3);
    tb_assert_and_check_return(tb_atomic64_fetch_and_sub(&a, 1000) == -1003);
    tb_trace_i("atomic64_fetch_and_sub(): test ok!");
}
static tb_void_t tb_test_atomic64_fetch_and_and()
{
    tb_trace_i("atomic64_fetch_and_and(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_and(&a, 1) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_and(&a, 2) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_and(&a, 0xff) == 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_and(&a, 1000) == 0);
    tb_trace_i("atomic64_fetch_and_and(): test ok!");
}
static tb_void_t tb_test_atomic64_fetch_and_xor()
{
    tb_trace_i("atomic64_fetch_and_xor(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_xor(&a, 1) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_xor(&a, 2) == 0);
    tb_assert_and_check_return(tb_atomic64_fetch_and_xor(&a, 3) == 2);
    tb_assert_and_check_return(tb_atomic64_fetch_and_xor(&a, 1000) == 1);
    tb_trace_i("atomic64_fetch_and_xor(): test ok!");
}
static tb_void_t tb_test_atomic64_fetch_and_or()
{
    tb_trace_i("atomic64_fetch_and_or(): test ..");
    tb_atomic64_t a;
    tb_atomic64_init(&a, 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_or(&a, 1) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_or(&a, 2) == 1);
    tb_assert_and_check_return(tb_atomic64_fetch_and_or(&a, 3) == 3);
    tb_assert_and_check_return(tb_atomic64_fetch_and_or(&a, 1000) == 3);
    tb_trace_i("atomic64_fetch_and_or(): test ok!");
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_atomic64_main(tb_int_t argc, tb_char_t** argv)
{
    tb_test_atomic64_get();
    tb_test_atomic64_compare_and_swap();
    tb_test_atomic64_fetch_and_set();
    tb_test_atomic64_fetch_and_cmpset();
    tb_test_atomic64_fetch_and_add();
    tb_test_atomic64_fetch_and_sub();
    tb_test_atomic64_fetch_and_and();
    tb_test_atomic64_fetch_and_xor();
    tb_test_atomic64_fetch_and_or();
    return 0;
}
