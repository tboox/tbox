/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_DEBUG
# 	define tb_hash_test_dump(h) 		tb_hash_dump(h)
#else
# 	define tb_hash_test_dump(h)
#endif

#define tb_hash_test_set_s2i(h, s) 		do {tb_size_t n = tb_strlen(s); tb_hash_set(h, s, n); } while (0);
#define tb_hash_test_get_s2i(h, s) 		do {tb_assert(tb_strlen(s) == (tb_size_t)tb_hash_const_at(h, s)); } while (0);
#define tb_hash_test_del_s2i(h, s) 		do {tb_hash_del(h, s); tb_assert(!tb_hash_const_at(h, s)); } while (0);

#define tb_hash_test_set_i2s(h, i) 		do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); tb_hash_set(h, i, s); } while (0);
#define tb_hash_test_get_i2s(h, i) 		do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); tb_assert(!tb_strcmp(s, tb_hash_const_at(h, i))); } while (0);
#define tb_hash_test_del_i2s(h, i) 		do {tb_hash_del(h, i); tb_assert(!tb_hash_const_at(h, i)); } while (0);

#define tb_hash_test_set_m2m(h, i) 		do {tb_memset_u32(item, i, step >> 2); tb_hash_set(h, item, item); } while (0);
#define tb_hash_test_get_m2m(h, i) 		do {tb_memset_u32(item, i, step >> 2); tb_assert(!tb_memcmp(item, tb_hash_const_at(h, item), step)); } while (0);
#define tb_hash_test_del_m2m(h, i) 		do {tb_memset_u32(item, i, step >> 2); tb_hash_del(h, item); tb_assert(!tb_hash_const_at(h, item)); } while (0);

#define tb_hash_test_set_i2i(h, i) 		do {tb_hash_set(h, i, i); } while (0);
#define tb_hash_test_get_i2i(h, i) 		do {tb_assert(i == tb_hash_const_at(h, i)); } while (0);
#define tb_hash_test_del_i2i(h, i) 		do {tb_hash_del(h, i); tb_assert(!tb_hash_const_at(h, i)); } while (0);

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_hash_test_s2i_func()
{
	// init hash: str => int
	tb_hash_t* hash = tb_hash_init(8, tb_item_func_str(TB_NULL), tb_item_func_int());
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
	// init hash: str => int
	tb_spool_t* pool = tb_spool_init(TB_SPOOL_SIZE_SMALL);
	tb_hash_t* 	hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_str(pool), tb_item_func_int());
	tb_assert_and_check_return(hash);

	// performance
	__tb_volatile__ tb_char_t s[256] = {0};
	__tb_volatile__ tb_size_t n = 100000;
	tb_int64_t t = tb_mclock();
	while (n--) 
	{
		tb_int_t r = snprintf(s, 256, "%x", tb_rand_uint32(0, TB_MAXU32)); 
		s[r] == '\0'; 
		tb_hash_test_set_s2i(hash, s); 
		tb_hash_test_get_s2i(hash, s);
	}
	t = tb_int64_sub(tb_mclock(), t);
	tb_print("time: %lld", t);

	tb_hash_exit(hash);
	tb_spool_exit(pool);
}
static tb_void_t tb_hash_test_i2s_func()
{
	// init hash: int => str
	tb_hash_t* hash = tb_hash_init(8, tb_item_func_int(), tb_item_func_str(TB_NULL));
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
	// init hash: int => str
	tb_spool_t* pool = tb_spool_init(TB_SPOOL_SIZE_SMALL);
	tb_hash_t* 	hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_int(), tb_item_func_str(pool));
	tb_assert_and_check_return(hash);

	// performance
	__tb_volatile__ tb_char_t s[256] = {0};
	__tb_volatile__ tb_size_t n = 100000;
	tb_int64_t t = tb_mclock();
	while (n--) 
	{
		tb_size_t i = tb_rand_uint32(0, TB_MAXU32);
		tb_hash_test_set_i2s(hash, i); 
		tb_hash_test_get_i2s(hash, i);
	}
	t = tb_int64_sub(tb_mclock(), t);
	tb_print("time: %lld", t);

	tb_hash_exit(hash);
	tb_spool_exit(pool);
}
static tb_void_t tb_hash_test_m2m_func()
{
	tb_size_t const	step = 256;
	tb_byte_t 		item[step];
	// init hash: mem => mem
	tb_hash_t* 	hash = tb_hash_init(8, tb_item_func_efm(step, TB_NULL), tb_item_func_efm(step, TB_NULL));
	//tb_hash_t* 	hash = tb_hash_init(8, tb_item_func_ifm(step, TB_NULL, TB_NULL), tb_item_func_ifm(step, TB_NULL, TB_NULL));
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
	tb_size_t const	step = 12;
	tb_byte_t 		item[step];
	// init hash: mem => mem
	tb_fpool_t* 	pool = tb_fpool_init(256, 256, tb_item_func_ifm(step, TB_NULL, TB_NULL));
	//tb_hash_t* 		hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_efm(step, pool), tb_item_func_efm(step, pool));
	tb_hash_t* 		hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_ifm(step, TB_NULL, TB_NULL), tb_item_func_ifm(step, TB_NULL, TB_NULL));
	tb_assert_and_check_return(hash);

	// performance
	__tb_volatile__ tb_size_t n = 100000;
	tb_int64_t t = tb_mclock();
	while (n--) 
	{
		tb_size_t i = tb_rand_uint32(0, TB_MAXU32);
		tb_hash_test_set_m2m(hash, i); 
		tb_hash_test_get_m2m(hash, i);
	}
	t = tb_int64_sub(tb_mclock(), t);
	tb_print("time: %lld", t);

	tb_hash_exit(hash);
	tb_fpool_exit(pool);
}
static tb_void_t tb_hash_test_i2i_func()
{
	// init hash: int => str
	tb_hash_t* hash = tb_hash_init(8, tb_item_func_int(), tb_item_func_int());
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
	// init hash: int => str
	tb_hash_t* 	hash = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_int(), tb_item_func_int());
	tb_assert_and_check_return(hash);

	// performance
	__tb_volatile__ tb_size_t n = 100000;
	tb_int64_t t = tb_mclock();
	while (n--) 
	{
		tb_size_t i = tb_rand_uint32(0, TB_MAXU32);
		tb_hash_test_set_i2i(hash, i); 
		tb_hash_test_get_i2i(hash, i);
	}
	t = tb_int64_sub(tb_mclock(), t);
	tb_print("time: %lld", t);

	tb_hash_exit(hash);
}

/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(5 * 1024 * 1024), 5 * 1024 * 1024)) return 0;

#if 1
	tb_hash_test_s2i_func();
	tb_hash_test_i2s_func();
	tb_hash_test_m2m_func();
	tb_hash_test_i2i_func();
#endif

#if 1
	tb_hash_test_s2i_perf();
	tb_hash_test_i2s_perf();
	tb_hash_test_m2m_perf();
	tb_hash_test_i2i_perf();
#endif

	return 0;
}
