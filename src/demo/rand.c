#include "tbox.h"


static tb_void_t tb_rand_test_uint32(tb_uint32_t b, tb_uint32_t e)
{
	__tb_volatile__ tb_uint32_t rand = 0;
	__tb_volatile__ tb_size_t 	i = 0;
	__tb_volatile__ tb_size_t 	n = 1000000;

	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) rand += tb_rand_uint32(b, e);
	t = tb_mclock() - t;
	tb_print("time: %lld, rand_uint32: %u, range: %u - %u", t, (rand + (n >> 1)) / n, b, e);
}
static tb_void_t tb_rand_test_sint32(tb_sint32_t b, tb_sint32_t e)
{
	__tb_volatile__ tb_sint32_t rand = 0;
	__tb_volatile__ tb_size_t 	i = 0;
	__tb_volatile__ tb_sint32_t n = 1000000;

	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) rand += tb_rand_sint32(b, e);
	t = tb_mclock() - t;
	tb_print("time: %lld, rand_sint32: %d, range: %d - %d", t, (rand + (n >> 1)) / n, b, e);
}
#ifdef TB_CONFIG_TYPE_FLOAT
static tb_void_t tb_rand_test_float(tb_float_t b, tb_float_t e)
{
	__tb_volatile__ tb_float_t 	rand = 0;
	__tb_volatile__ tb_size_t 	i = 0;
	__tb_volatile__ tb_size_t 	n = 1000000;

	tb_int64_t t = tb_mclock();
	for (i = 0; i < n; i++) rand += tb_rand_float(b, e);
	t = tb_mclock() - t;
	tb_print("time: %lld, rand_float: %f, range: %f - %f", t, rand / n, b, e);
}
#endif

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_rand_test_uint32(600, 1000);
	tb_rand_test_uint32(100, 200);

	tb_rand_test_sint32(-600, 1000);
	tb_rand_test_sint32(-600, 200);
	tb_rand_test_sint32(-600, -200);

#ifdef TB_CONFIG_TYPE_FLOAT
	tb_rand_test_float(0., 200.);
	tb_rand_test_float(-200., 0.);
	tb_rand_test_float(-200., 200.);
#endif

	return 0;
}
