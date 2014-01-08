/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_scache_main(tb_int_t argc, tb_char_t** argv)
{
	// hello
	tb_char_t const* hello = tb_scache_put("hello world");
	tb_print("hello: %s", hello);

	// clear rand
	tb_rand_clear();

	// performance
	__tb_volatile__ tb_char_t s[256] = {0};
	__tb_volatile__ tb_size_t n = 1000000;
	tb_hong_t t = tb_mclock();
	while (n--) 
	{
		tb_int_t r = tb_snprintf(s, 256, "%u", tb_rand_uint32(0, 10000)); 
		s[r] == '\0'; 
#if 1
		tb_scache_put(s); 
		if (!(n & 15)) tb_scache_del(s);
#else
		tb_free(tb_strdup(s));
#endif
	}
	t = tb_mclock() - t;
	tb_print("time: %lld", t);

	// del hello
	tb_scache_del(hello);

	return 0;
}
