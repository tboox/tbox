/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef __tb_debug__
#   define tb_hash_test_dump(h)         tb_hash_dump(h)
#else
#   define tb_hash_test_dump(h)
#endif

#define tb_hash_test_set_s2i(h, s)      do {tb_size_t n = tb_strlen((tb_char_t*)(s)); tb_hash_set(h, (tb_char_t*)(s), (tb_pointer_t)n); } while (0);
#define tb_hash_test_get_s2i(h, s)      do {tb_assert(tb_strlen((tb_char_t*)s) == (tb_size_t)tb_hash_get(h, (tb_char_t*)(s))); } while (0);
#define tb_hash_test_del_s2i(h, s)      do {tb_hash_del(h, s); tb_assert(!tb_hash_get(h, s)); } while (0);

#define tb_hash_test_set_i2s(h, i)      do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); tb_hash_set(h, (tb_pointer_t)i, s); } while (0);
#define tb_hash_test_get_i2s(h, i)      do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); tb_assert(!tb_strcmp(s, (tb_char_t const*)tb_hash_get(h, (tb_pointer_t)i))); } while (0);
#define tb_hash_test_del_i2s(h, i)      do {tb_hash_del(h, (tb_pointer_t)i); tb_assert(!tb_hash_get(h, (tb_pointer_t)i)); } while (0);

#define tb_hash_test_set_m2m(h, i)      do {tb_memset_u32(item, i, step >> 2); tb_hash_set(h, item, item); } while (0);
#define tb_hash_test_get_m2m(h, i)      do {tb_memset_u32(item, i, step >> 2); tb_assert(!tb_memcmp(item, tb_hash_get(h, item), step)); } while (0);
#define tb_hash_test_del_m2m(h, i)      do {tb_memset_u32(item, i, step >> 2); tb_hash_del(h, item); tb_assert(!tb_hash_get(h, item)); } while (0);

#define tb_hash_test_set_i2i(h, i)      do {tb_hash_set(h, (tb_pointer_t)i, (tb_pointer_t)i); } while (0);
#define tb_hash_test_get_i2i(h, i)      do {tb_assert(i == (tb_size_t)tb_hash_get(h, (tb_pointer_t)i)); } while (0);
#define tb_hash_test_del_i2i(h, i)      do {tb_hash_del(h, (tb_pointer_t)i); tb_assert(!tb_hash_get(h, (tb_pointer_t)i)); } while (0);

#define tb_hash_test_set_i2t(h, i)      do {tb_hash_set(h, (tb_pointer_t)i, (tb_pointer_t)(tb_size_t)tb_true); } while (0);
#define tb_hash_test_get_i2t(h, i)      do {tb_assert(tb_hash_get(h, (tb_pointer_t)i)); } while (0);
#define tb_hash_test_del_i2t(h, i)      do {tb_hash_del(h, (tb_pointer_t)i); tb_assert(!tb_hash_get(h, (tb_pointer_t)i)); } while (0);

/* //////////////////////////////////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_hash_test_s2i_func()
{
    // init hash
    tb_hash_t* hash = tb_hash_init(8, tb_item_func_str(tb_true, tb_null), tb_item_func_long());
    tb_assert_and_check_return(hash);

    // set
    tb_hash_test_set_s2i(hash, "");
    tb_hash_test_set_s2i(hash, "0");
    tb_hash_test_set_s2i(hash, "01");
    tb_hash_test_set_s2i(hash, "012");
    tb_hash_test_set_s2i(hash, "0123");
    tb_hash_test_set_s2i(hash, "01234");
    tb_hash_test_set_s2i(hash, "012345");
    tb_hash_test_set_s2i(hash, "0123456");
    tb_hash_test_set_s2i(hash, "01234567");
    tb_hash_test_set_s2i(hash, "012345678");
    tb_hash_test_set_s2i(hash, "0123456789");
    tb_hash_test_set_s2i(hash, "9876543210");
    tb_hash_test_set_s2i(hash, "876543210");
    tb_hash_test_set_s2i(hash, "76543210");
    tb_hash_test_set_s2i(hash, "6543210");
    tb_hash_test_set_s2i(hash, "543210");
    tb_hash_test_set_s2i(hash, "43210");
    tb_hash_test_set_s2i(hash, "3210");
    tb_hash_test_set_s2i(hash, "210");
    tb_hash_test_set_s2i(hash, "10");
    tb_hash_test_set_s2i(hash, "0");
    tb_hash_test_set_s2i(hash, "");
    tb_hash_test_dump(hash);

    // get
    tb_hash_test_get_s2i(hash, "");
    tb_hash_test_get_s2i(hash, "01");
    tb_hash_test_get_s2i(hash, "012");
    tb_hash_test_get_s2i(hash, "0123");
    tb_hash_test_get_s2i(hash, "01234");
    tb_hash_test_get_s2i(hash, "012345");
    tb_hash_test_get_s2i(hash, "0123456");
    tb_hash_test_get_s2i(hash, "01234567");
    tb_hash_test_get_s2i(hash, "012345678");
    tb_hash_test_get_s2i(hash, "0123456789");
    tb_hash_test_get_s2i(hash, "9876543210");
    tb_hash_test_get_s2i(hash, "876543210");
    tb_hash_test_get_s2i(hash, "76543210");
    tb_hash_test_get_s2i(hash, "6543210");
    tb_hash_test_get_s2i(hash, "543210");
    tb_hash_test_get_s2i(hash, "43210");
    tb_hash_test_get_s2i(hash, "3210");
    tb_hash_test_get_s2i(hash, "210");
    tb_hash_test_get_s2i(hash, "10");
    tb_hash_test_get_s2i(hash, "0");
    tb_hash_test_get_s2i(hash, "");

    // del
    tb_hash_test_del_s2i(hash, "");
    tb_hash_test_del_s2i(hash, "01");
    tb_hash_test_del_s2i(hash, "012");
    tb_hash_test_del_s2i(hash, "0123");
    tb_hash_test_del_s2i(hash, "01234");
    tb_hash_test_del_s2i(hash, "012345");
    tb_hash_test_del_s2i(hash, "0123456");
    tb_hash_test_del_s2i(hash, "01234567");
    tb_hash_test_del_s2i(hash, "012345678");
    tb_hash_test_del_s2i(hash, "0123456789");
    tb_hash_test_del_s2i(hash, "0123456789");
    tb_hash_test_dump(hash);

    // clear
    tb_hash_clear(hash);
    tb_hash_test_dump(hash);

    tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_s2i_perf()
{
    // init hash
    tb_handle_t     pool = tb_pool_init(TB_POOL_GROW_SMALL, 0);
    tb_hash_t*      hash = tb_hash_init(0, tb_item_func_str(tb_true, pool), tb_item_func_long());
    tb_assert_and_check_return(hash);

    // clear rand
    tb_random_clear(tb_random_generator());

    // performance
    tb_char_t s[256] = {0};
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--) 
    {
        tb_int_t r = tb_snprintf(s, sizeof(s) - 1, "%x", tb_random_range(tb_random_generator(), 0, TB_MAXU32)); 
        s[r] = '\0'; 
        tb_hash_test_set_s2i(hash, s); 
        tb_hash_test_get_s2i(hash, s);
    }
    t = tb_mclock() - t;
    tb_trace_i("s2i: time: %lld", t);

    tb_hash_exit(hash);
    tb_pool_exit(pool);
}
static tb_void_t tb_hash_test_i2s_func()
{
    // init hash
    tb_hash_t* hash = tb_hash_init(8, tb_item_func_long(), tb_item_func_str(tb_true, tb_null));
    tb_assert_and_check_return(hash);

    // set
    tb_hash_test_set_i2s(hash, 0);
    tb_hash_test_set_i2s(hash, 1);
    tb_hash_test_set_i2s(hash, 12);
    tb_hash_test_set_i2s(hash, 123);
    tb_hash_test_set_i2s(hash, 1234);
    tb_hash_test_set_i2s(hash, 12345);
    tb_hash_test_set_i2s(hash, 123456);
    tb_hash_test_set_i2s(hash, 1234567);
    tb_hash_test_set_i2s(hash, 12345678);
    tb_hash_test_set_i2s(hash, 123456789);
    tb_hash_test_set_i2s(hash, 876543210);
    tb_hash_test_set_i2s(hash, 76543210);
    tb_hash_test_set_i2s(hash, 6543210);
    tb_hash_test_set_i2s(hash, 543210);
    tb_hash_test_set_i2s(hash, 43210);
    tb_hash_test_set_i2s(hash, 3210);
    tb_hash_test_set_i2s(hash, 210);
    tb_hash_test_set_i2s(hash, 10);
    tb_hash_test_set_i2s(hash, 0);
    tb_hash_test_dump(hash);

    // get
    tb_hash_test_get_i2s(hash, 0);
    tb_hash_test_get_i2s(hash, 1);
    tb_hash_test_get_i2s(hash, 12);
    tb_hash_test_get_i2s(hash, 123);
    tb_hash_test_get_i2s(hash, 1234);
    tb_hash_test_get_i2s(hash, 12345);
    tb_hash_test_get_i2s(hash, 123456);
    tb_hash_test_get_i2s(hash, 1234567);
    tb_hash_test_get_i2s(hash, 12345678);
    tb_hash_test_get_i2s(hash, 123456789);
    tb_hash_test_get_i2s(hash, 876543210);
    tb_hash_test_get_i2s(hash, 76543210);
    tb_hash_test_get_i2s(hash, 6543210);
    tb_hash_test_get_i2s(hash, 543210);
    tb_hash_test_get_i2s(hash, 43210);
    tb_hash_test_get_i2s(hash, 3210);
    tb_hash_test_get_i2s(hash, 210);
    tb_hash_test_get_i2s(hash, 10);
    tb_hash_test_get_i2s(hash, 0);

    // del
    tb_hash_test_del_i2s(hash, 0);
    tb_hash_test_del_i2s(hash, 1);
    tb_hash_test_del_i2s(hash, 12);
    tb_hash_test_del_i2s(hash, 123);
    tb_hash_test_del_i2s(hash, 1234);
    tb_hash_test_del_i2s(hash, 12345);
    tb_hash_test_del_i2s(hash, 123456);
    tb_hash_test_del_i2s(hash, 1234567);
    tb_hash_test_del_i2s(hash, 12345678);
    tb_hash_test_del_i2s(hash, 123456789);
    tb_hash_test_del_i2s(hash, 123456789);
    tb_hash_test_dump(hash);

    // clear
    tb_hash_clear(hash);
    tb_hash_test_dump(hash);

    tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_i2s_perf()
{
    // init hash
    tb_handle_t pool = tb_pool_init(TB_POOL_GROW_SMALL, 0);
    tb_hash_t*  hash = tb_hash_init(0, tb_item_func_long(), tb_item_func_str(tb_true, pool));
    tb_assert_and_check_return(hash);

    // clear rand
    tb_random_clear(tb_random_generator());

    // performance
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--) 
    {
        tb_size_t i = tb_random_range(tb_random_generator(), 0, TB_MAXU32);
        tb_hash_test_set_i2s(hash, i); 
        tb_hash_test_get_i2s(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("i2s: time: %lld", t);

    tb_hash_exit(hash);
    tb_pool_exit(pool);
}
static tb_void_t tb_hash_test_m2m_func()
{
    // init hash
    tb_size_t const step = 256;
    tb_byte_t       item[step];
    tb_hash_t*  hash = tb_hash_init(8, tb_item_func_mem(step, tb_null, tb_null), tb_item_func_mem(step, tb_null, tb_null));
    tb_assert_and_check_return(hash);

    // set
    tb_hash_test_set_m2m(hash, 0);
    tb_hash_test_set_m2m(hash, 1);
    tb_hash_test_set_m2m(hash, 2);
    tb_hash_test_set_m2m(hash, 3);
    tb_hash_test_set_m2m(hash, 4);
    tb_hash_test_set_m2m(hash, 5);
    tb_hash_test_set_m2m(hash, 6);
    tb_hash_test_set_m2m(hash, 7);
    tb_hash_test_set_m2m(hash, 8);
    tb_hash_test_set_m2m(hash, 9);
    tb_hash_test_set_m2m(hash, 10);
    tb_hash_test_set_m2m(hash, 11);
    tb_hash_test_set_m2m(hash, 12);
    tb_hash_test_set_m2m(hash, 13);
    tb_hash_test_set_m2m(hash, 14);
    tb_hash_test_set_m2m(hash, 15);
    tb_hash_test_set_m2m(hash, 16);
    tb_hash_test_set_m2m(hash, 17);
    tb_hash_test_set_m2m(hash, 18);
    tb_hash_test_set_m2m(hash, 19);
    tb_hash_test_set_m2m(hash, 20);
    tb_hash_test_set_m2m(hash, 21);
    tb_hash_test_set_m2m(hash, 22);
    tb_hash_test_set_m2m(hash, 23);
    tb_hash_test_set_m2m(hash, 24);
    tb_hash_test_set_m2m(hash, 25);
    tb_hash_test_set_m2m(hash, 26);
    tb_hash_test_set_m2m(hash, 27);
    tb_hash_test_set_m2m(hash, 28);
    tb_hash_test_set_m2m(hash, 29);
    tb_hash_test_set_m2m(hash, 30);
    tb_hash_test_set_m2m(hash, 31);
    tb_hash_test_set_m2m(hash, 32);
    tb_hash_test_dump(hash);

    // get
    tb_hash_test_get_m2m(hash, 0);
    tb_hash_test_get_m2m(hash, 1);
    tb_hash_test_get_m2m(hash, 2);
    tb_hash_test_get_m2m(hash, 3);
    tb_hash_test_get_m2m(hash, 4);
    tb_hash_test_get_m2m(hash, 5);
    tb_hash_test_get_m2m(hash, 6);
    tb_hash_test_get_m2m(hash, 7);
    tb_hash_test_get_m2m(hash, 8);
    tb_hash_test_get_m2m(hash, 9);
    tb_hash_test_get_m2m(hash, 10);
    tb_hash_test_get_m2m(hash, 11);
    tb_hash_test_get_m2m(hash, 12);
    tb_hash_test_get_m2m(hash, 13);
    tb_hash_test_get_m2m(hash, 14);
    tb_hash_test_get_m2m(hash, 15);
    tb_hash_test_get_m2m(hash, 16);
    tb_hash_test_get_m2m(hash, 17);
    tb_hash_test_get_m2m(hash, 18);
    tb_hash_test_get_m2m(hash, 19);
    tb_hash_test_get_m2m(hash, 20);
    tb_hash_test_get_m2m(hash, 21);
    tb_hash_test_get_m2m(hash, 22);
    tb_hash_test_get_m2m(hash, 23);
    tb_hash_test_get_m2m(hash, 24);
    tb_hash_test_get_m2m(hash, 25);
    tb_hash_test_get_m2m(hash, 26);
    tb_hash_test_get_m2m(hash, 27);
    tb_hash_test_get_m2m(hash, 28);
    tb_hash_test_get_m2m(hash, 29);
    tb_hash_test_get_m2m(hash, 30);
    tb_hash_test_get_m2m(hash, 31);
    tb_hash_test_get_m2m(hash, 32);

    // del
    tb_hash_test_del_m2m(hash, 10);
    tb_hash_test_del_m2m(hash, 11);
    tb_hash_test_del_m2m(hash, 12);
    tb_hash_test_del_m2m(hash, 13);
    tb_hash_test_del_m2m(hash, 14);
    tb_hash_test_del_m2m(hash, 15);
    tb_hash_test_del_m2m(hash, 16);
    tb_hash_test_del_m2m(hash, 17);
    tb_hash_test_del_m2m(hash, 18);
    tb_hash_test_del_m2m(hash, 19);
    tb_hash_test_del_m2m(hash, 20);
    tb_hash_test_del_m2m(hash, 21);
    tb_hash_test_del_m2m(hash, 22);
    tb_hash_test_del_m2m(hash, 23);
    tb_hash_test_del_m2m(hash, 24);
    tb_hash_test_del_m2m(hash, 25);
    tb_hash_test_del_m2m(hash, 26);
    tb_hash_test_del_m2m(hash, 27);
    tb_hash_test_del_m2m(hash, 28);
    tb_hash_test_del_m2m(hash, 29);
    tb_hash_test_del_m2m(hash, 30);
    tb_hash_test_del_m2m(hash, 31);
    tb_hash_test_del_m2m(hash, 32);
    tb_hash_test_dump(hash);

    // clear
    tb_hash_clear(hash);
    tb_hash_test_dump(hash);

    tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_m2m_perf()
{
    // init hash: mem => mem
    tb_size_t const step = 12;
    tb_byte_t       item[step];
    tb_handle_t     pool = tb_fixed_pool_init(256, step, 0);
    tb_hash_t*      hash = tb_hash_init(0, tb_item_func_mem(step, tb_null, tb_null), tb_item_func_mem(step, tb_null, tb_null));
    tb_assert_and_check_return(hash);

    // clear rand
    tb_random_clear(tb_random_generator());

    // performance
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--) 
    {
        tb_size_t i = tb_random_range(tb_random_generator(), 0, TB_MAXU32);
        tb_hash_test_set_m2m(hash, i); 
        tb_hash_test_get_m2m(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("m2m: time: %lld", t);

    tb_hash_exit(hash);
    tb_fixed_pool_exit(pool);
}
static tb_void_t tb_hash_test_i2i_func()
{
    // init hash
    tb_hash_t* hash = tb_hash_init(8, tb_item_func_long(), tb_item_func_long());
    tb_assert_and_check_return(hash);

    // set
    tb_hash_test_set_i2i(hash, 0);
    tb_hash_test_set_i2i(hash, 1);
    tb_hash_test_set_i2i(hash, 12);
    tb_hash_test_set_i2i(hash, 123);
    tb_hash_test_set_i2i(hash, 1234);
    tb_hash_test_set_i2i(hash, 12345);
    tb_hash_test_set_i2i(hash, 123456);
    tb_hash_test_set_i2i(hash, 1234567);
    tb_hash_test_set_i2i(hash, 12345678);
    tb_hash_test_set_i2i(hash, 123456789);
    tb_hash_test_set_i2i(hash, 876543210);
    tb_hash_test_set_i2i(hash, 76543210);
    tb_hash_test_set_i2i(hash, 6543210);
    tb_hash_test_set_i2i(hash, 543210);
    tb_hash_test_set_i2i(hash, 43210);
    tb_hash_test_set_i2i(hash, 3210);
    tb_hash_test_set_i2i(hash, 210);
    tb_hash_test_set_i2i(hash, 10);
    tb_hash_test_set_i2i(hash, 0);
    tb_hash_test_dump(hash);

    // get
    tb_hash_test_get_i2i(hash, 0);
    tb_hash_test_get_i2i(hash, 1);
    tb_hash_test_get_i2i(hash, 12);
    tb_hash_test_get_i2i(hash, 123);
    tb_hash_test_get_i2i(hash, 1234);
    tb_hash_test_get_i2i(hash, 12345);
    tb_hash_test_get_i2i(hash, 123456);
    tb_hash_test_get_i2i(hash, 1234567);
    tb_hash_test_get_i2i(hash, 12345678);
    tb_hash_test_get_i2i(hash, 123456789);
    tb_hash_test_get_i2i(hash, 876543210);
    tb_hash_test_get_i2i(hash, 76543210);
    tb_hash_test_get_i2i(hash, 6543210);
    tb_hash_test_get_i2i(hash, 543210);
    tb_hash_test_get_i2i(hash, 43210);
    tb_hash_test_get_i2i(hash, 3210);
    tb_hash_test_get_i2i(hash, 210);
    tb_hash_test_get_i2i(hash, 10);
    tb_hash_test_get_i2i(hash, 0);

    // del
    tb_hash_test_del_i2i(hash, 0);
    tb_hash_test_del_i2i(hash, 1);
    tb_hash_test_del_i2i(hash, 12);
    tb_hash_test_del_i2i(hash, 123);
    tb_hash_test_del_i2i(hash, 1234);
    tb_hash_test_del_i2i(hash, 12345);
    tb_hash_test_del_i2i(hash, 123456);
    tb_hash_test_del_i2i(hash, 1234567);
    tb_hash_test_del_i2i(hash, 12345678);
    tb_hash_test_del_i2i(hash, 123456789);
    tb_hash_test_del_i2i(hash, 123456789);
    tb_hash_test_dump(hash);

    // clear
    tb_hash_clear(hash);
    tb_hash_test_dump(hash);

    tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_i2i_perf()
{
    // init hash
    tb_hash_t*  hash = tb_hash_init(0, tb_item_func_long(), tb_item_func_long());
    tb_assert_and_check_return(hash);

    // clear rand
    tb_random_clear(tb_random_generator());

    // performance
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--) 
    {
        tb_size_t i = tb_random_range(tb_random_generator(), 0, TB_MAXU32);
        tb_hash_test_set_i2i(hash, i); 
        tb_hash_test_get_i2i(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("i2i: time: %lld", t);

    tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_i2t_func()
{
    // init hash
    tb_hash_t* hash = tb_hash_init(8, tb_item_func_long(), tb_item_func_true());
    tb_assert_and_check_return(hash);

    // set
    tb_hash_test_set_i2t(hash, 0);
    tb_hash_test_set_i2t(hash, 1);
    tb_hash_test_set_i2t(hash, 12);
    tb_hash_test_set_i2t(hash, 123);
    tb_hash_test_set_i2t(hash, 1234);
    tb_hash_test_set_i2t(hash, 12345);
    tb_hash_test_set_i2t(hash, 123456);
    tb_hash_test_set_i2t(hash, 1234567);
    tb_hash_test_set_i2t(hash, 12345678);
    tb_hash_test_set_i2t(hash, 123456789);
    tb_hash_test_set_i2t(hash, 876543210);
    tb_hash_test_set_i2t(hash, 76543210);
    tb_hash_test_set_i2t(hash, 6543210);
    tb_hash_test_set_i2t(hash, 543210);
    tb_hash_test_set_i2t(hash, 43210);
    tb_hash_test_set_i2t(hash, 3210);
    tb_hash_test_set_i2t(hash, 210);
    tb_hash_test_set_i2t(hash, 10);
    tb_hash_test_set_i2t(hash, 0);
    tb_hash_test_dump(hash);

    // get
    tb_hash_test_get_i2t(hash, 0);
    tb_hash_test_get_i2t(hash, 1);
    tb_hash_test_get_i2t(hash, 12);
    tb_hash_test_get_i2t(hash, 123);
    tb_hash_test_get_i2t(hash, 1234);
    tb_hash_test_get_i2t(hash, 12345);
    tb_hash_test_get_i2t(hash, 123456);
    tb_hash_test_get_i2t(hash, 1234567);
    tb_hash_test_get_i2t(hash, 12345678);
    tb_hash_test_get_i2t(hash, 123456789);
    tb_hash_test_get_i2t(hash, 876543210);
    tb_hash_test_get_i2t(hash, 76543210);
    tb_hash_test_get_i2t(hash, 6543210);
    tb_hash_test_get_i2t(hash, 543210);
    tb_hash_test_get_i2t(hash, 43210);
    tb_hash_test_get_i2t(hash, 3210);
    tb_hash_test_get_i2t(hash, 210);
    tb_hash_test_get_i2t(hash, 10);
    tb_hash_test_get_i2t(hash, 0);

    // del
    tb_hash_test_del_i2t(hash, 0);
    tb_hash_test_del_i2t(hash, 1);
    tb_hash_test_del_i2t(hash, 12);
    tb_hash_test_del_i2t(hash, 123);
    tb_hash_test_del_i2t(hash, 1234);
    tb_hash_test_del_i2t(hash, 12345);
    tb_hash_test_del_i2t(hash, 123456);
    tb_hash_test_del_i2t(hash, 1234567);
    tb_hash_test_del_i2t(hash, 12345678);
    tb_hash_test_del_i2t(hash, 123456789);
    tb_hash_test_del_i2t(hash, 123456789);
    tb_hash_test_dump(hash);

    // clear
    tb_hash_clear(hash);
    tb_hash_test_dump(hash);

    tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_i2t_perf()
{
    // init hash
    tb_hash_t*  hash = tb_hash_init(0, tb_item_func_long(), tb_item_func_true());
    tb_assert_and_check_return(hash);

    // clear rand
    tb_random_clear(tb_random_generator());

    // performance
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--) 
    {
        tb_size_t i = tb_random_range(tb_random_generator(), 0, TB_MAXU32);
        tb_hash_test_set_i2t(hash, i); 
        tb_hash_test_get_i2t(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("i2t: time: %lld", t);

    tb_hash_exit(hash);
}
static tb_bool_t tb_hash_test_walk_item(tb_hash_t* hash, tb_hash_item_t* item, tb_bool_t* bdel, tb_cpointer_t priv)
{
    tb_assert_and_check_return_val(hash && bdel && priv, tb_false);

    tb_hize_t* test = (tb_hize_t*)priv;
    if (item)
    {
        if (!(((tb_size_t)item->data >> 25) & 0x1))
//      if (!(((tb_size_t)item->data) & 0x7))
//      if (1)
//      if (!(tb_random_range(tb_random_generator(), 0, TB_MAXU32) & 0x1))
            *bdel = tb_true;
        else
        {
            test[0] += (tb_size_t)item->name;
            test[1] += (tb_size_t)item->data;

            test[2]++;
        }
    }

    // ok
    return tb_true;
}
static tb_void_t tb_hash_test_walk_perf()
{
    // init hash
    tb_hash_t*  hash = tb_hash_init(0, tb_item_func_long(), tb_item_func_long());
    tb_assert_and_check_return(hash);

    // clear rand
    tb_random_clear(tb_random_generator());

    // add items
    __tb_volatile__ tb_size_t n = 100000;
    while (n--) 
    {
        tb_size_t i = tb_random_range(tb_random_generator(), 0, TB_MAXU32);
        tb_hash_test_set_i2i(hash, i); 
        tb_hash_test_get_i2i(hash, i);
    }

    // performance
    tb_hong_t t = tb_mclock();
    __tb_volatile__ tb_hize_t test[3] = {0};
    tb_hash_walk(hash, tb_hash_test_walk_item, (tb_cpointer_t)test);
    t = tb_mclock() - t;
    tb_trace_i("name: %llx, data: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], test[2], tb_hash_size(hash), t);

    tb_hash_exit(hash);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_hash_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_hash_test_s2i_func();
    tb_hash_test_i2s_func();
    tb_hash_test_m2m_func();
    tb_hash_test_i2i_func();
    tb_hash_test_i2t_func();
#endif

#if 1
    tb_hash_test_s2i_perf();
    tb_hash_test_i2s_perf();
    tb_hash_test_m2m_perf();
    tb_hash_test_i2i_perf();
    tb_hash_test_i2t_perf();
#endif

#if 1
    tb_hash_test_walk_perf();
#endif

    return 0;
}
