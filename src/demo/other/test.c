/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_other_test_main(tb_int_t argc, tb_char_t** argv)
{

	tb_hong_t t = tb_uclock();
	__tb_volatile__ tb_size_t n = 0;
	__tb_volatile__ tb_size_t i = 0;
	for (i = 0; i < 18; i++)
	{
		if (i) n++;
	}
	t = tb_uclock() - t;
	tb_trace_i("%lld", t);
	return 0;
}
