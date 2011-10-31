/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* /////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_DEBUG
# 	define tb_hash_test_dump(h) 		tb_hash_dump(h)
#else
# 	define tb_hash_test_dump(h)
#endif

#define tb_hash_test_set_s2i(h, s) 		do {tb_size_t n = tb_strlen(s); tb_hash_set(h, s, n); } while (0);
#define tb_hash_test_get_s2i(h, s) 		do {TB_ASSERT(tb_strlen(s) == (tb_size_t)tb_hash_const_at(h, s)); } while (0);
#define tb_hash_test_del_s2i(h, s) 		do {tb_hash_del(h, s); TB_ASSERT(!tb_hash_const_at(h, s)); } while (0);

#define tb_hash_test_set_i2s(h, i) 		do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); tb_hash_set(h, i, s); } while (0);
#define tb_hash_test_get_i2s(h, i) 		do {tb_char_t s[256] = {0}; tb_snprintf(s, 256, "%u", i); TB_ASSERT(!tb_strcmp(s, tb_hash_const_at(h, i))); } while (0);
#define tb_hash_test_del_i2s(h, i) 		do {tb_hash_del(h, i); TB_ASSERT(!tb_hash_const_at(h, i)); } while (0);

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_hash_test_s2i_func()
{
	// init hash: str => int
	tb_hash_t* hash = tb_hash_init(sizeof(tb_size_t), 8, tb_item_func_str(TB_NULL), tb_item_func_int());
	TB_ASSERT_RETURN(hash);

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
	tb_hash_test_dump(hash);

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
	tb_hash_t* hash = tb_hash_init(sizeof(tb_size_t), TB_HASH_SIZE_DEFAULT, tb_item_func_str(tb_spool_init(TB_SPOOL_SIZE_SMALL)), tb_item_func_int());
	TB_ASSERT_RETURN(hash);

	// performance
	__tb_volatile__ tb_char_t s[256] = {0};
	__tb_volatile__ tb_size_t n = 100000;
	tb_int64_t t = tb_mclock();
	while (n--) 
	{
		tb_int_t r = snprintf(s, 256, "%x", rand()); 
		s[r] == '\0'; 
		tb_hash_test_set_s2i(hash, s); 
		tb_hash_test_get_s2i(hash, s);
	}
	t = tb_int64_sub(tb_mclock(), t);
	tb_printf("time: %lld\n", t);

	tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_i2s_func()
{
	// init hash: int => str
	tb_hash_t* hash = tb_hash_init(sizeof(tb_char_t const*), 8, tb_item_func_int(), tb_item_func_str(TB_NULL));
	TB_ASSERT_RETURN(hash);

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
	tb_hash_test_dump(hash);

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
static tb_void_t tb_hash_test_m2m()
{
	// init hash: mem => mem
	tb_hash_t* hash = tb_hash_init(4, TB_HASH_SIZE_SMALL, tb_item_func_mem(4), tb_item_func_mem(4));
	TB_ASSERT_RETURN(hash);

	tb_hash_exit(hash);
}
/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(50 * 1024 * 1024), 50 * 1024 * 1024)) return 0;

	tb_hash_test_s2i_func();
	tb_hash_test_s2i_perf();

	tb_hash_test_i2s_func();
//	tb_hash_test_m2m();

	getchar();
	return 0;
}
