/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef __tb_debug__
#   define tb_hash_map_test_dump(h)         tb_hash_map_dump(h)
#else
#   define tb_hash_map_test_dump(h)
#endif

#define tb_hash_map_test_get_s2i(h, s)          do {tb_assert(tb_strlen((tb_char_t*)s) == (tb_size_t)tb_hash_map_get(h, (tb_char_t*)(s))); } while (0);
#define tb_hash_map_test_insert_s2i(h, s)       do {tb_size_t n = tb_strlen((tb_char_t*)(s)); tb_hash_map_insert(h, (tb_char_t*)(s), (tb_pointer_t)n); } while (0);
#define tb_hash_map_test_remove_s2i(h, s)       do {tb_hash_map_remove(h, s); tb_assert(!tb_hash_map_get(h, s)); } while (0);

#define tb_hash_map_test_get_i2s(h, i)          do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); tb_assert(!tb_strcmp(s, (tb_char_t const*)tb_hash_map_get(h, (tb_pointer_t)i))); } while (0);
#define tb_hash_map_test_insert_i2s(h, i)       do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); tb_hash_map_insert(h, (tb_pointer_t)i, s); } while (0);
#define tb_hash_map_test_remove_i2s(h, i)       do {tb_hash_map_remove(h, (tb_pointer_t)i); tb_assert(!tb_hash_map_get(h, (tb_pointer_t)i)); } while (0);

#define tb_hash_map_test_get_m2m(h, i)          do {tb_memset_u32(item, i, step >> 2); tb_assert(!tb_memcmp(item, tb_hash_map_get(h, item), step)); } while (0);
#define tb_hash_map_test_insert_m2m(h, i)       do {tb_memset_u32(item, i, step >> 2); tb_hash_map_insert(h, item, item); } while (0);
#define tb_hash_map_test_remove_m2m(h, i)       do {tb_memset_u32(item, i, step >> 2); tb_hash_map_remove(h, item); tb_assert(!tb_hash_map_get(h, item)); } while (0);

#define tb_hash_map_test_get_i2i(h, i)          do {tb_assert(i == (tb_size_t)tb_hash_map_get(h, (tb_pointer_t)i)); } while (0);
#define tb_hash_map_test_insert_i2i(h, i)       do {tb_hash_map_insert(h, (tb_pointer_t)i, (tb_pointer_t)i); } while (0);
#define tb_hash_map_test_remove_i2i(h, i)       do {tb_hash_map_remove(h, (tb_pointer_t)i); tb_assert(!tb_hash_map_get(h, (tb_pointer_t)i)); } while (0);

#define tb_hash_map_test_get_i2t(h, i)          do {tb_assert(tb_hash_map_get(h, (tb_pointer_t)i)); } while (0);
#define tb_hash_map_test_insert_i2t(h, i)       do {tb_hash_map_insert(h, (tb_pointer_t)i, (tb_pointer_t)(tb_size_t)tb_true); } while (0);
#define tb_hash_map_test_remove_i2t(h, i)       do {tb_hash_map_remove(h, (tb_pointer_t)i); tb_assert(!tb_hash_map_get(h, (tb_pointer_t)i)); } while (0);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_hash_map_test_s2i_func()
{
    // init hash
    tb_hash_map_ref_t hash = tb_hash_map_init(8, tb_element_str(tb_true), tb_element_long());
    tb_assert_and_check_return(hash);

    // set
    tb_hash_map_test_insert_s2i(hash, "");
    tb_hash_map_test_insert_s2i(hash, "0");
    tb_hash_map_test_insert_s2i(hash, "01");
    tb_hash_map_test_insert_s2i(hash, "012");
    tb_hash_map_test_insert_s2i(hash, "0123");
    tb_hash_map_test_insert_s2i(hash, "01234");
    tb_hash_map_test_insert_s2i(hash, "012345");
    tb_hash_map_test_insert_s2i(hash, "0123456");
    tb_hash_map_test_insert_s2i(hash, "01234567");
    tb_hash_map_test_insert_s2i(hash, "012345678");
    tb_hash_map_test_insert_s2i(hash, "0123456789");
    tb_hash_map_test_insert_s2i(hash, "9876543210");
    tb_hash_map_test_insert_s2i(hash, "876543210");
    tb_hash_map_test_insert_s2i(hash, "76543210");
    tb_hash_map_test_insert_s2i(hash, "6543210");
    tb_hash_map_test_insert_s2i(hash, "543210");
    tb_hash_map_test_insert_s2i(hash, "43210");
    tb_hash_map_test_insert_s2i(hash, "3210");
    tb_hash_map_test_insert_s2i(hash, "210");
    tb_hash_map_test_insert_s2i(hash, "10");
    tb_hash_map_test_insert_s2i(hash, "0");
    tb_hash_map_test_insert_s2i(hash, "");
    tb_hash_map_test_dump(hash);

    // get
    tb_hash_map_test_get_s2i(hash, "");
    tb_hash_map_test_get_s2i(hash, "01");
    tb_hash_map_test_get_s2i(hash, "012");
    tb_hash_map_test_get_s2i(hash, "0123");
    tb_hash_map_test_get_s2i(hash, "01234");
    tb_hash_map_test_get_s2i(hash, "012345");
    tb_hash_map_test_get_s2i(hash, "0123456");
    tb_hash_map_test_get_s2i(hash, "01234567");
    tb_hash_map_test_get_s2i(hash, "012345678");
    tb_hash_map_test_get_s2i(hash, "0123456789");
    tb_hash_map_test_get_s2i(hash, "9876543210");
    tb_hash_map_test_get_s2i(hash, "876543210");
    tb_hash_map_test_get_s2i(hash, "76543210");
    tb_hash_map_test_get_s2i(hash, "6543210");
    tb_hash_map_test_get_s2i(hash, "543210");
    tb_hash_map_test_get_s2i(hash, "43210");
    tb_hash_map_test_get_s2i(hash, "3210");
    tb_hash_map_test_get_s2i(hash, "210");
    tb_hash_map_test_get_s2i(hash, "10");
    tb_hash_map_test_get_s2i(hash, "0");
    tb_hash_map_test_get_s2i(hash, "");

    // del
    tb_hash_map_test_remove_s2i(hash, "");
    tb_hash_map_test_remove_s2i(hash, "01");
    tb_hash_map_test_remove_s2i(hash, "012");
    tb_hash_map_test_remove_s2i(hash, "0123");
    tb_hash_map_test_remove_s2i(hash, "01234");
    tb_hash_map_test_remove_s2i(hash, "012345");
    tb_hash_map_test_remove_s2i(hash, "0123456");
    tb_hash_map_test_remove_s2i(hash, "01234567");
    tb_hash_map_test_remove_s2i(hash, "012345678");
    tb_hash_map_test_remove_s2i(hash, "0123456789");
    tb_hash_map_test_remove_s2i(hash, "0123456789");
    tb_hash_map_test_dump(hash);

    // clear
    tb_hash_map_clear(hash);
    tb_hash_map_test_dump(hash);

    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_s2i_perf()
{
    // init hash
    tb_hash_map_ref_t hash = tb_hash_map_init(0, tb_element_str(tb_true), tb_element_long());
    tb_assert_and_check_return(hash);

    // performance
    tb_char_t s[256] = {0};
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_long_t r = tb_snprintf(s, sizeof(s) - 1, "%ld", tb_random_value());
        s[r] = '\0';
        tb_hash_map_test_insert_s2i(hash, s);
        tb_hash_map_test_get_s2i(hash, s);
    }
    t = tb_mclock() - t;
    tb_trace_i("s2i: time: %lld", t);

    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_i2s_func()
{
    // init hash
    tb_hash_map_ref_t hash = tb_hash_map_init(8, tb_element_long(), tb_element_str(tb_true));
    tb_assert_and_check_return(hash);

    // set
    tb_hash_map_test_insert_i2s(hash, 0);
    tb_hash_map_test_insert_i2s(hash, 1);
    tb_hash_map_test_insert_i2s(hash, 12);
    tb_hash_map_test_insert_i2s(hash, 123);
    tb_hash_map_test_insert_i2s(hash, 1234);
    tb_hash_map_test_insert_i2s(hash, 12345);
    tb_hash_map_test_insert_i2s(hash, 123456);
    tb_hash_map_test_insert_i2s(hash, 1234567);
    tb_hash_map_test_insert_i2s(hash, 12345678);
    tb_hash_map_test_insert_i2s(hash, 123456789);
    tb_hash_map_test_insert_i2s(hash, 876543210);
    tb_hash_map_test_insert_i2s(hash, 76543210);
    tb_hash_map_test_insert_i2s(hash, 6543210);
    tb_hash_map_test_insert_i2s(hash, 543210);
    tb_hash_map_test_insert_i2s(hash, 43210);
    tb_hash_map_test_insert_i2s(hash, 3210);
    tb_hash_map_test_insert_i2s(hash, 210);
    tb_hash_map_test_insert_i2s(hash, 10);
    tb_hash_map_test_insert_i2s(hash, 0);
    tb_hash_map_test_dump(hash);

    // get
    tb_hash_map_test_get_i2s(hash, 0);
    tb_hash_map_test_get_i2s(hash, 1);
    tb_hash_map_test_get_i2s(hash, 12);
    tb_hash_map_test_get_i2s(hash, 123);
    tb_hash_map_test_get_i2s(hash, 1234);
    tb_hash_map_test_get_i2s(hash, 12345);
    tb_hash_map_test_get_i2s(hash, 123456);
    tb_hash_map_test_get_i2s(hash, 1234567);
    tb_hash_map_test_get_i2s(hash, 12345678);
    tb_hash_map_test_get_i2s(hash, 123456789);
    tb_hash_map_test_get_i2s(hash, 876543210);
    tb_hash_map_test_get_i2s(hash, 76543210);
    tb_hash_map_test_get_i2s(hash, 6543210);
    tb_hash_map_test_get_i2s(hash, 543210);
    tb_hash_map_test_get_i2s(hash, 43210);
    tb_hash_map_test_get_i2s(hash, 3210);
    tb_hash_map_test_get_i2s(hash, 210);
    tb_hash_map_test_get_i2s(hash, 10);
    tb_hash_map_test_get_i2s(hash, 0);

    // del
    tb_hash_map_test_remove_i2s(hash, 0);
    tb_hash_map_test_remove_i2s(hash, 1);
    tb_hash_map_test_remove_i2s(hash, 12);
    tb_hash_map_test_remove_i2s(hash, 123);
    tb_hash_map_test_remove_i2s(hash, 1234);
    tb_hash_map_test_remove_i2s(hash, 12345);
    tb_hash_map_test_remove_i2s(hash, 123456);
    tb_hash_map_test_remove_i2s(hash, 1234567);
    tb_hash_map_test_remove_i2s(hash, 12345678);
    tb_hash_map_test_remove_i2s(hash, 123456789);
    tb_hash_map_test_remove_i2s(hash, 123456789);
    tb_hash_map_test_dump(hash);

    // clear
    tb_hash_map_clear(hash);
    tb_hash_map_test_dump(hash);

    // exit
    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_i2s_perf()
{
    // init hash
    tb_hash_map_ref_t hash = tb_hash_map_init(0, tb_element_long(), tb_element_str(tb_true));
    tb_assert_and_check_return(hash);

    // performance
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_size_t i = tb_random_value();
        tb_hash_map_test_insert_i2s(hash, i);
        tb_hash_map_test_get_i2s(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("i2s: time: %lld", t);

    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_m2m_func()
{
    // init hash
    tb_size_t const step = 256;
    tb_byte_t       item[step];
    tb_hash_map_ref_t  hash = tb_hash_map_init(8, tb_element_mem(step, tb_null, tb_null), tb_element_mem(step, tb_null, tb_null));
    tb_assert_and_check_return(hash);

    // set
    tb_hash_map_test_insert_m2m(hash, 0);
    tb_hash_map_test_insert_m2m(hash, 1);
    tb_hash_map_test_insert_m2m(hash, 2);
    tb_hash_map_test_insert_m2m(hash, 3);
    tb_hash_map_test_insert_m2m(hash, 4);
    tb_hash_map_test_insert_m2m(hash, 5);
    tb_hash_map_test_insert_m2m(hash, 6);
    tb_hash_map_test_insert_m2m(hash, 7);
    tb_hash_map_test_insert_m2m(hash, 8);
    tb_hash_map_test_insert_m2m(hash, 9);
    tb_hash_map_test_insert_m2m(hash, 10);
    tb_hash_map_test_insert_m2m(hash, 11);
    tb_hash_map_test_insert_m2m(hash, 12);
    tb_hash_map_test_insert_m2m(hash, 13);
    tb_hash_map_test_insert_m2m(hash, 14);
    tb_hash_map_test_insert_m2m(hash, 15);
    tb_hash_map_test_insert_m2m(hash, 16);
    tb_hash_map_test_insert_m2m(hash, 17);
    tb_hash_map_test_insert_m2m(hash, 18);
    tb_hash_map_test_insert_m2m(hash, 19);
    tb_hash_map_test_insert_m2m(hash, 20);
    tb_hash_map_test_insert_m2m(hash, 21);
    tb_hash_map_test_insert_m2m(hash, 22);
    tb_hash_map_test_insert_m2m(hash, 23);
    tb_hash_map_test_insert_m2m(hash, 24);
    tb_hash_map_test_insert_m2m(hash, 25);
    tb_hash_map_test_insert_m2m(hash, 26);
    tb_hash_map_test_insert_m2m(hash, 27);
    tb_hash_map_test_insert_m2m(hash, 28);
    tb_hash_map_test_insert_m2m(hash, 29);
    tb_hash_map_test_insert_m2m(hash, 30);
    tb_hash_map_test_insert_m2m(hash, 31);
    tb_hash_map_test_insert_m2m(hash, 32);
    tb_hash_map_test_dump(hash);

    // get
    tb_hash_map_test_get_m2m(hash, 0);
    tb_hash_map_test_get_m2m(hash, 1);
    tb_hash_map_test_get_m2m(hash, 2);
    tb_hash_map_test_get_m2m(hash, 3);
    tb_hash_map_test_get_m2m(hash, 4);
    tb_hash_map_test_get_m2m(hash, 5);
    tb_hash_map_test_get_m2m(hash, 6);
    tb_hash_map_test_get_m2m(hash, 7);
    tb_hash_map_test_get_m2m(hash, 8);
    tb_hash_map_test_get_m2m(hash, 9);
    tb_hash_map_test_get_m2m(hash, 10);
    tb_hash_map_test_get_m2m(hash, 11);
    tb_hash_map_test_get_m2m(hash, 12);
    tb_hash_map_test_get_m2m(hash, 13);
    tb_hash_map_test_get_m2m(hash, 14);
    tb_hash_map_test_get_m2m(hash, 15);
    tb_hash_map_test_get_m2m(hash, 16);
    tb_hash_map_test_get_m2m(hash, 17);
    tb_hash_map_test_get_m2m(hash, 18);
    tb_hash_map_test_get_m2m(hash, 19);
    tb_hash_map_test_get_m2m(hash, 20);
    tb_hash_map_test_get_m2m(hash, 21);
    tb_hash_map_test_get_m2m(hash, 22);
    tb_hash_map_test_get_m2m(hash, 23);
    tb_hash_map_test_get_m2m(hash, 24);
    tb_hash_map_test_get_m2m(hash, 25);
    tb_hash_map_test_get_m2m(hash, 26);
    tb_hash_map_test_get_m2m(hash, 27);
    tb_hash_map_test_get_m2m(hash, 28);
    tb_hash_map_test_get_m2m(hash, 29);
    tb_hash_map_test_get_m2m(hash, 30);
    tb_hash_map_test_get_m2m(hash, 31);
    tb_hash_map_test_get_m2m(hash, 32);

    // del
    tb_hash_map_test_remove_m2m(hash, 10);
    tb_hash_map_test_remove_m2m(hash, 11);
    tb_hash_map_test_remove_m2m(hash, 12);
    tb_hash_map_test_remove_m2m(hash, 13);
    tb_hash_map_test_remove_m2m(hash, 14);
    tb_hash_map_test_remove_m2m(hash, 15);
    tb_hash_map_test_remove_m2m(hash, 16);
    tb_hash_map_test_remove_m2m(hash, 17);
    tb_hash_map_test_remove_m2m(hash, 18);
    tb_hash_map_test_remove_m2m(hash, 19);
    tb_hash_map_test_remove_m2m(hash, 20);
    tb_hash_map_test_remove_m2m(hash, 21);
    tb_hash_map_test_remove_m2m(hash, 22);
    tb_hash_map_test_remove_m2m(hash, 23);
    tb_hash_map_test_remove_m2m(hash, 24);
    tb_hash_map_test_remove_m2m(hash, 25);
    tb_hash_map_test_remove_m2m(hash, 26);
    tb_hash_map_test_remove_m2m(hash, 27);
    tb_hash_map_test_remove_m2m(hash, 28);
    tb_hash_map_test_remove_m2m(hash, 29);
    tb_hash_map_test_remove_m2m(hash, 30);
    tb_hash_map_test_remove_m2m(hash, 31);
    tb_hash_map_test_remove_m2m(hash, 32);
    tb_hash_map_test_dump(hash);

    // clear
    tb_hash_map_clear(hash);
    tb_hash_map_test_dump(hash);

    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_m2m_perf()
{
    // init hash: mem => mem
    tb_size_t const     step = 12;
    tb_byte_t           item[step];
    tb_hash_map_ref_t       hash = tb_hash_map_init(0, tb_element_mem(step, tb_null, tb_null), tb_element_mem(step, tb_null, tb_null));
    tb_assert_and_check_return(hash);

    // performance
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_uint32_t i = (tb_uint32_t)tb_random_value();
        tb_hash_map_test_insert_m2m(hash, i);
        tb_hash_map_test_get_m2m(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("m2m: time: %lld", t);

    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_i2i_func()
{
    // init hash
    tb_hash_map_ref_t hash = tb_hash_map_init(8, tb_element_long(), tb_element_long());
    tb_assert_and_check_return(hash);

    // set
    tb_hash_map_test_insert_i2i(hash, 0);
    tb_hash_map_test_insert_i2i(hash, 1);
    tb_hash_map_test_insert_i2i(hash, 12);
    tb_hash_map_test_insert_i2i(hash, 123);
    tb_hash_map_test_insert_i2i(hash, 1234);
    tb_hash_map_test_insert_i2i(hash, 12345);
    tb_hash_map_test_insert_i2i(hash, 123456);
    tb_hash_map_test_insert_i2i(hash, 1234567);
    tb_hash_map_test_insert_i2i(hash, 12345678);
    tb_hash_map_test_insert_i2i(hash, 123456789);
    tb_hash_map_test_insert_i2i(hash, 876543210);
    tb_hash_map_test_insert_i2i(hash, 76543210);
    tb_hash_map_test_insert_i2i(hash, 6543210);
    tb_hash_map_test_insert_i2i(hash, 543210);
    tb_hash_map_test_insert_i2i(hash, 43210);
    tb_hash_map_test_insert_i2i(hash, 3210);
    tb_hash_map_test_insert_i2i(hash, 210);
    tb_hash_map_test_insert_i2i(hash, 10);
    tb_hash_map_test_insert_i2i(hash, 0);
    tb_hash_map_test_dump(hash);

    // get
    tb_hash_map_test_get_i2i(hash, 0);
    tb_hash_map_test_get_i2i(hash, 1);
    tb_hash_map_test_get_i2i(hash, 12);
    tb_hash_map_test_get_i2i(hash, 123);
    tb_hash_map_test_get_i2i(hash, 1234);
    tb_hash_map_test_get_i2i(hash, 12345);
    tb_hash_map_test_get_i2i(hash, 123456);
    tb_hash_map_test_get_i2i(hash, 1234567);
    tb_hash_map_test_get_i2i(hash, 12345678);
    tb_hash_map_test_get_i2i(hash, 123456789);
    tb_hash_map_test_get_i2i(hash, 876543210);
    tb_hash_map_test_get_i2i(hash, 76543210);
    tb_hash_map_test_get_i2i(hash, 6543210);
    tb_hash_map_test_get_i2i(hash, 543210);
    tb_hash_map_test_get_i2i(hash, 43210);
    tb_hash_map_test_get_i2i(hash, 3210);
    tb_hash_map_test_get_i2i(hash, 210);
    tb_hash_map_test_get_i2i(hash, 10);
    tb_hash_map_test_get_i2i(hash, 0);

    // del
    tb_hash_map_test_remove_i2i(hash, 0);
    tb_hash_map_test_remove_i2i(hash, 1);
    tb_hash_map_test_remove_i2i(hash, 12);
    tb_hash_map_test_remove_i2i(hash, 123);
    tb_hash_map_test_remove_i2i(hash, 1234);
    tb_hash_map_test_remove_i2i(hash, 12345);
    tb_hash_map_test_remove_i2i(hash, 123456);
    tb_hash_map_test_remove_i2i(hash, 1234567);
    tb_hash_map_test_remove_i2i(hash, 12345678);
    tb_hash_map_test_remove_i2i(hash, 123456789);
    tb_hash_map_test_remove_i2i(hash, 123456789);
    tb_hash_map_test_dump(hash);

    // clear
    tb_hash_map_clear(hash);
    tb_hash_map_test_dump(hash);

    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_i2i_perf()
{
    // init hash
    tb_hash_map_ref_t  hash = tb_hash_map_init(0, tb_element_long(), tb_element_long());
    tb_assert_and_check_return(hash);

    // performance
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_size_t i = tb_random_value();
        tb_hash_map_test_insert_i2i(hash, i);
        tb_hash_map_test_get_i2i(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("i2i: time: %lld", t);

    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_i2t_func()
{
    // init hash
    tb_hash_map_ref_t hash = tb_hash_map_init(8, tb_element_long(), tb_element_true());
    tb_assert_and_check_return(hash);

    // set
    tb_hash_map_test_insert_i2t(hash, 0);
    tb_hash_map_test_insert_i2t(hash, 1);
    tb_hash_map_test_insert_i2t(hash, 12);
    tb_hash_map_test_insert_i2t(hash, 123);
    tb_hash_map_test_insert_i2t(hash, 1234);
    tb_hash_map_test_insert_i2t(hash, 12345);
    tb_hash_map_test_insert_i2t(hash, 123456);
    tb_hash_map_test_insert_i2t(hash, 1234567);
    tb_hash_map_test_insert_i2t(hash, 12345678);
    tb_hash_map_test_insert_i2t(hash, 123456789);
    tb_hash_map_test_insert_i2t(hash, 876543210);
    tb_hash_map_test_insert_i2t(hash, 76543210);
    tb_hash_map_test_insert_i2t(hash, 6543210);
    tb_hash_map_test_insert_i2t(hash, 543210);
    tb_hash_map_test_insert_i2t(hash, 43210);
    tb_hash_map_test_insert_i2t(hash, 3210);
    tb_hash_map_test_insert_i2t(hash, 210);
    tb_hash_map_test_insert_i2t(hash, 10);
    tb_hash_map_test_insert_i2t(hash, 0);
    tb_hash_map_test_dump(hash);

    // get
    tb_hash_map_test_get_i2t(hash, 0);
    tb_hash_map_test_get_i2t(hash, 1);
    tb_hash_map_test_get_i2t(hash, 12);
    tb_hash_map_test_get_i2t(hash, 123);
    tb_hash_map_test_get_i2t(hash, 1234);
    tb_hash_map_test_get_i2t(hash, 12345);
    tb_hash_map_test_get_i2t(hash, 123456);
    tb_hash_map_test_get_i2t(hash, 1234567);
    tb_hash_map_test_get_i2t(hash, 12345678);
    tb_hash_map_test_get_i2t(hash, 123456789);
    tb_hash_map_test_get_i2t(hash, 876543210);
    tb_hash_map_test_get_i2t(hash, 76543210);
    tb_hash_map_test_get_i2t(hash, 6543210);
    tb_hash_map_test_get_i2t(hash, 543210);
    tb_hash_map_test_get_i2t(hash, 43210);
    tb_hash_map_test_get_i2t(hash, 3210);
    tb_hash_map_test_get_i2t(hash, 210);
    tb_hash_map_test_get_i2t(hash, 10);
    tb_hash_map_test_get_i2t(hash, 0);

    // del
    tb_hash_map_test_remove_i2t(hash, 0);
    tb_hash_map_test_remove_i2t(hash, 1);
    tb_hash_map_test_remove_i2t(hash, 12);
    tb_hash_map_test_remove_i2t(hash, 123);
    tb_hash_map_test_remove_i2t(hash, 1234);
    tb_hash_map_test_remove_i2t(hash, 12345);
    tb_hash_map_test_remove_i2t(hash, 123456);
    tb_hash_map_test_remove_i2t(hash, 1234567);
    tb_hash_map_test_remove_i2t(hash, 12345678);
    tb_hash_map_test_remove_i2t(hash, 123456789);
    tb_hash_map_test_remove_i2t(hash, 123456789);
    tb_hash_map_test_dump(hash);

    // clear
    tb_hash_map_clear(hash);
    tb_hash_map_test_dump(hash);

    // exit
    tb_hash_map_exit(hash);
}
static tb_void_t tb_hash_map_test_i2t_perf()
{
    // init hash
    tb_hash_map_ref_t  hash = tb_hash_map_init(0, tb_element_long(), tb_element_true());
    tb_assert_and_check_return(hash);

    // done
    __tb_volatile__ tb_size_t n = 100000;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_size_t i = tb_random_value();
        tb_hash_map_test_insert_i2t(hash, i);
        tb_hash_map_test_get_i2t(hash, i);
    }
    t = tb_mclock() - t;
    tb_trace_i("i2t: time: %lld", t);

    // exit hash
    tb_hash_map_exit(hash);
}
static tb_bool_t tb_hash_map_test_walk_item(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_hize_t*              test = (tb_hize_t*)value;
    tb_hash_map_item_ref_t  hash_item = (tb_hash_map_item_ref_t)item;
    if (hash_item)
    {
        if (!(((tb_size_t)hash_item->data >> 25) & 0x1)) ok = tb_true;
        else
        {
            test[0] += (tb_size_t)hash_item->name;
            test[1] += (tb_size_t)hash_item->data;
            test[2]++;
        }
    }

    // ok?
    return ok;
}
static tb_void_t tb_hash_map_test_walk_perf()
{
    // init hash
    tb_hash_map_ref_t hash = tb_hash_map_init(0, tb_element_long(), tb_element_long());
    tb_assert_and_check_return(hash);

    // reset random
    tb_random_reset(tb_true);

    // add items
    __tb_volatile__ tb_size_t n = 100000;
    while (n--)
    {
        tb_size_t i = tb_random_value();
        tb_hash_map_test_insert_i2i(hash, i);
        tb_hash_map_test_get_i2i(hash, i);
    }

    // done
    tb_hong_t t = tb_mclock();
    __tb_volatile__ tb_hize_t test[3] = {0};
    tb_remove_if(hash, tb_hash_map_test_walk_item, (tb_cpointer_t)test);
    t = tb_mclock() - t;
    tb_trace_i("name: %llx, data: %llx, size: %llu ?= %u, time: %lld", test[0], test[1], test[2], tb_hash_map_size(hash), t);

    // exit
    tb_hash_map_exit(hash);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_hash_map_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_hash_map_test_s2i_func();
    tb_hash_map_test_i2s_func();
    tb_hash_map_test_m2m_func();
    tb_hash_map_test_i2i_func();
    tb_hash_map_test_i2t_func();
#endif

#if 1
    tb_hash_map_test_s2i_perf();
    tb_hash_map_test_i2s_perf();
    tb_hash_map_test_m2m_perf();
    tb_hash_map_test_i2i_perf();
    tb_hash_map_test_i2t_perf();
#endif

#if 1
    tb_hash_map_test_walk_perf();
#endif

    return 0;
}
