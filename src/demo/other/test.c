/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_other_test_main(tb_int_t argc, tb_char_t** argv)
{
	// s = m/n = -lnp / (ln2 * ln2) = -log2(p) / ln2
	// k = s * ln2 = -log2(p)
 	// p = (1 - e^(-k/s))^k
	
	tb_double_t p = 0.0001;
	tb_double_t k = -tb_log2(p);
	tb_double_t s = k / (tb_log2(2) / tb_log2(2.718281828459045));
	tb_double_t v = tb_pow((1 - tb_exp(-k/s)), k);
	tb_trace_i("p: %lf, k: %lf, s: %lf, v: %lf", p, k, s, v);

	// p = (1 - e^(-k/s))^k
	// lnp = k * ln(1 - e^(-k/s))
	// (lnp) / k = ln(1 - e^(-k/s))
	// e^((lnp) / k) = 1 - e^(-k/s)
	// e^(-k/s) = 1 - e^((lnp) / k) = 1 - (e^lnp)^(1/k) = 1 - p^(1/k)
	// -k/s = ln(1 - p^(1/k))
	// s = -k / ln(1 - p^(1/k))
	// s ~= -k / (-p^(1/k) + 0.5p^(2/k))
	//
	// c = p^(1/k)
	// s = -k / ln(1 - c))
	// s ~= -k / (-c-0.5c^2) = 2k / (2c + c * c)


	tb_double_t p2 = 0.001;
	tb_double_t k2 = k;
//	tb_double_t tt = 1.0 - tb_pow(p2, 1.0 / k2);
//	tb_double_t s2 = -k2 * tb_log2(2.718281828459045) / tb_log2(tt);

//	tb_double_t s2 = k2 / tb_pow(p2, 1.0 / k2);
	tb_double_t c2 = tb_pow(p2, 1.0 / k2);
	tb_double_t s2 = (k2 + k2) / (c2 + c2 + c2 * c2);
	tb_double_t v2 = tb_pow((1 - tb_exp(-k2/s2)), k2);
	tb_trace_i("p: %lf, k: %lf, s: %lf, v: %lf", p2, k2, s2, v2);

	return 0;
}
