/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_string_pool_main(tb_int_t argc, tb_char_t** argv)
{
	// hello
	tb_char_t const* hello = tb_string_pool_put(tb_string_pool(), "hello world");
	tb_trace_i("hello: %s", hello);

	// clear rand
	tb_rand_clear();

	// performance
	tb_char_t s[256] = {0};
	tb_hong_t t = tb_mclock();
	__tb_volatile__ tb_size_t n = 1000000;
	while (n--) 
	{
		tb_int_t r = tb_snprintf(s, 256, "%u", tb_rand_uint32(0, 10000)); 
		s[r] = '\0'; 
#if 1
		tb_string_pool_put(tb_string_pool(), s); 
		if (!(n & 15)) tb_string_pool_del(tb_string_pool(), s);
#else
		tb_free(tb_strdup(s));
#endif
	}
	t = tb_mclock() - t;
	tb_trace_i("time: %lld", t);

	// del hello
	tb_string_pool_del(tb_string_pool(), hello);
	return 0;
}
