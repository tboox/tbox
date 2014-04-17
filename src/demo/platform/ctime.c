/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_ctime_main(tb_int_t argc, tb_char_t** argv)
{
	tb_trace_i("%lld %lld", tb_ctime_spak(), tb_ctime_time());
	tb_sleep(1);
	tb_trace_i("%lld %lld", tb_ctime_spak(), tb_ctime_time());
	tb_sleep(1);
	tb_trace_i("%lld %lld", tb_ctime_spak(), tb_ctime_time());
	return 0;
}
