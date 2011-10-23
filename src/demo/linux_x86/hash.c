/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* /////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_DEBUG
# 	define tb_hash_test_dump(h) 				tb_hash_dump(h)
#endif

#define tb_hash_test_set_s2i(h, s) 		do {tb_size_t n = tb_strlen(s); tb_hash_set(h, (tb_void_t const*)s, (tb_void_t const*)&n); } while (0);
#define tb_hash_test_get_s2i(h, s) 		do {tb_size_t const* p = (tb_size_t const*)tb_hash_const_at(h, (tb_void_t const*)s); TB_ASSERT(p && tb_strlen(s) == p[0]); } while (0);
#define tb_hash_test_del_s2i(h, s) 		do {tb_hash_del(h, (tb_void_t const*)s); tb_size_t const* p = (tb_size_t const*)tb_hash_const_at(h, (tb_void_t const*)s); TB_ASSERT(!p); } while (0);

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_void_t tb_hash_test_item_int_free(tb_void_t* item, tb_void_t* priv)
{
	tb_printf("[free]: %s, %u\n", (tb_char_t const*)priv, *((tb_size_t*)item));
}
static tb_char_t const* tb_hash_test_item_int_cstr(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv)
{
	tb_int_t n = tb_snprintf(data, maxn, "%u", *((tb_size_t const*)item));
	if (n > 0) data[n] = '\0';
	return (tb_char_t const*)data;
}
static tb_void_t tb_hash_test_item_str_free(tb_void_t* item, tb_void_t* priv)
{
	tb_printf("[free]: %s, %s\n", (tb_char_t const*)priv, (tb_char_t const*)item);
	if (item) tb_free(item);
}
static tb_char_t const* tb_hash_test_item_str_cstr(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv)
{
	tb_byte_t const* p = item;
	tb_int_t n = tb_snprintf(data, maxn, "%x", tb_bits_get_u32_be(p));
	if (n > 0) data[n] = '\0';
	return (tb_char_t const*)data;
}
static tb_void_t tb_hash_test_item_mem_free(tb_void_t* item, tb_void_t* priv)
{
	tb_char_t const* p = item;
	tb_printf("[free]: %s, %c%c%c%c\n", (tb_char_t const*)priv, p[0], p[1], p[2], p[3]);
}
static tb_char_t const* tb_hash_test_item_mem_cstr(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv)
{
	tb_byte_t const* p = item;
	tb_int_t n = tb_snprintf(data, maxn, "%c%c%c%c", p[0], p[1], p[2], p[3]);
	if (n > 0) data[n] = '\0';
	return (tb_char_t const*)data;
}
static tb_void_t tb_hash_test_s2i()
{
	// init hash: str => int
	tb_hash_item_func_t int_func = {tb_hash_test_item_int_free, tb_hash_test_item_int_cstr, "int"};
	tb_hash_t* hash = tb_hash_init(sizeof(tb_size_t), 8, tb_hash_name_func_str(), &int_func);
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

#if 0
	// performance
	__tb_volatile__ tb_char_t s[256] = {0};
	__tb_volatile__ tb_size_t n = 80000;
	tb_int64_t t = tb_mclock();
	while (n--) 
	{
		tb_int_t r = snprintf(s, 256, "%x", rand()); 
		s[r] == '\0'; 
		tb_hash_test_set_s2i(hash, s); 
		tb_hash_test_get_s2i(hash, s);
	}
	t = tb_int64_sub(tb_mclock(), t);
	tb_hash_test_dump(hash);
	tb_printf("time: %lld\n", t);
#else
	__tb_volatile__ m = 10;
	while (m--)
	{
		// performance
		__tb_volatile__ tb_char_t s[256] = {0};
		__tb_volatile__ tb_size_t n = 32;
		while (n--) 
		{
			tb_int_t r = snprintf(s, 256, "%x", rand()); 
			s[r] == '\0'; 
			tb_hash_test_set_s2i(hash, s); 
			tb_hash_test_get_s2i(hash, s);
		}
	}
	tb_hash_test_dump(hash);
#endif
	// exit
	tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_i2s()
{
	// init hash: int => str
	tb_hash_item_func_t str_func = {tb_hash_test_item_str_free, tb_hash_test_item_str_cstr, "str"};
	tb_hash_t* hash = tb_hash_init(sizeof(tb_char_t const*), TB_HASH_SIZE_MICRO, tb_hash_name_func_int(), &str_func);
	TB_ASSERT_RETURN(hash);

	tb_hash_exit(hash);
}
static tb_void_t tb_hash_test_m2m()
{
	// init hash: mem => mem
	tb_hash_item_func_t mem_func = {tb_hash_test_item_mem_free, tb_hash_test_item_mem_cstr, "mem"};
	tb_hash_t* hash = tb_hash_init(4, TB_HASH_SIZE_SMALL, tb_hash_name_func_mem(4), &mem_func);
	TB_ASSERT_RETURN(hash);

	tb_hash_exit(hash);
}
/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	tb_hash_test_s2i();
//	tb_hash_test_i2s();
//	tb_hash_test_m2m();

	getchar();
	return 0;
}
