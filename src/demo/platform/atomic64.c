/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_atomic64_main(tb_int_t argc, tb_char_t** argv)
{
	tb_atomic64_t a;
	tb_atomic64_set0(&a);
	tb_trace_i("%lld", tb_atomic64_get(&a));
	tb_trace_i("%lld", tb_atomic64_fetch_and_set(&a, 1));
	tb_trace_i("%lld", tb_atomic64_fetch_and_pset(&a, 1, 2));
	tb_trace_i("%lld", tb_atomic64_fetch_and_inc(&a));
	tb_trace_i("%lld", tb_atomic64_fetch_and_dec(&a));
	tb_trace_i("%lld", tb_atomic64_fetch_and_add(&a, 10));
	tb_trace_i("%lld", tb_atomic64_fetch_and_sub(&a, 10));
	tb_trace_i("%lld", tb_atomic64_fetch_and_and(&a, 0xff));
	tb_trace_i("%lld", tb_atomic64_fetch_and_xor(&a, 0xff));
	tb_trace_i("%lld", tb_atomic64_fetch_and_or(&a, 0xff));
	tb_trace_i("%lld", tb_atomic64_get(&a));
	tb_trace_i("%lld", tb_atomic64_inc_and_fetch(&a));
	tb_trace_i("%lld", tb_atomic64_dec_and_fetch(&a));
	tb_trace_i("%lld", tb_atomic64_add_and_fetch(&a, 10));
	tb_trace_i("%lld", tb_atomic64_sub_and_fetch(&a, 10));
	tb_trace_i("%lld", tb_atomic64_and_and_fetch(&a, 0xff));
	tb_trace_i("%lld", tb_atomic64_xor_and_fetch(&a, 0xff));
	tb_trace_i("%lld", tb_atomic64_or_and_fetch(&a, 0xff));

	return 0;
}
