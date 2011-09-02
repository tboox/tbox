#include "tbox.h"

static tb_void_t tb_test_bits_swap_u16()
{
	__tb_volatile__ tb_uint16_t x = 0x1234;
	__tb_volatile__ tb_size_t 	n = 100000001;
	__tb_volatile__ tb_int64_t 	dt = tb_mclock();

	while (n--) x = tb_bits_swap_u16(x);

	dt = tb_int64_sub(tb_mclock(), dt);
	tb_printf("[bitops]: swap_u16: %x => %x, %llu ms\n", 0x1234, x, dt);
}
static tb_void_t tb_test_bits_swap_u32()
{
	__tb_volatile__ tb_uint32_t x = 0x12345678;
	__tb_volatile__ tb_size_t 	n = 100000001;
	__tb_volatile__ tb_int64_t 	dt = tb_mclock();

	while (n--) x = tb_bits_swap_u32(x);

	dt = tb_int64_sub(tb_mclock(), dt);
	tb_printf("[bitops]: swap_u32: %x => %x, %llu ms\n", 0x12345678, x, dt);
}
static tb_void_t tb_test_bits_swap_u64()
{
	__tb_volatile__ tb_uint64_t x = tb_uint32_to_uint64(0x12345678);
	__tb_volatile__ tb_size_t 	n = 100000001;
	__tb_volatile__ tb_int64_t 	dt = tb_mclock();

	while (n--) x = tb_bits_swap_u64(x);

	dt = tb_int64_sub(tb_mclock(), dt);
	tb_printf("[bitops]: swap_u64: %x => %llx, %llu ms\n", 0x12345678, x, dt);
}
static tb_void_t tb_test_bits_ubits32(tb_uint32_t x)
{
	__tb_volatile__ tb_size_t 	b = 0;
	__tb_volatile__ tb_size_t 	n = 0;
	__tb_volatile__ tb_int64_t 	ts = TB_INT64_ZERO;
	__tb_volatile__ tb_int64_t 	tg = TB_INT64_ZERO;
	for (b = 0; b < 8; ++b)
	{
		tb_printf("[bitops]: ubits32 b: %d x: %x\n", b, x);
		for (n = 0; n <= 32; ++n)
		{
			__tb_volatile__ tb_byte_t 	p[256] = {0};
			__tb_volatile__ tb_size_t 	n1 = 1000000;
			__tb_volatile__ tb_size_t 	n2 = 1000000;
			__tb_volatile__ tb_uint32_t y = 0;
			__tb_volatile__ tb_int64_t 	t1, t2;
			__tb_volatile__ tb_uint32_t xx = n < 32? (x & ((1 << n) - 1)) : x;

			t1 = tb_mclock();
			while (n1--) tb_bits_set_ubits32(p, b, x, n);
			t1 = tb_int64_sub(tb_mclock(), t1);
			ts = tb_int64_add(ts, t1);

			t2 = tb_mclock();
			while (n2--) y = tb_bits_get_ubits32(p, b, n);
			t2 = tb_int64_sub(tb_mclock(), t2);
			tg = tb_int64_add(tg, t2);

			if (xx != y)
			{
				tb_printf("[bitops]: ubits32 set: b: %u, n: %u, x: %x, %llu ms\n", b, n, xx, t1);
				tb_printf("[bitops]: ubits32 get: b: %u, n: %u, y: %x, %llu ms\n", b, n, y, t2);
			}
		}
	}
	tb_printf("[bitops]: ubits32 set: %llu ms, get: %llu ms\n", ts, tg);
}
static tb_void_t tb_test_bits_sbits32(tb_sint32_t x)
{
	__tb_volatile__ tb_size_t 	b = 0;
	__tb_volatile__ tb_size_t 	n = 0;
	__tb_volatile__ tb_int64_t 	ts = TB_INT64_ZERO;
	__tb_volatile__ tb_int64_t 	tg = TB_INT64_ZERO;
	for (b = 0; b < 8; ++b)
	{
		tb_printf("[bitops]: sbits32 b: %d x: %d\n", b, x);
		for (n = 2; n <= 32; ++n)
		{
			__tb_volatile__ tb_byte_t 	p[256] = {0};
			__tb_volatile__ tb_size_t 	n1 = 1000000;
			__tb_volatile__ tb_size_t 	n2 = 1000000;
			__tb_volatile__ tb_sint32_t y = 0;
			__tb_volatile__ tb_int64_t 	t1, t2;
			__tb_volatile__ tb_sint32_t xx = ((x >> 31) << (n - 1)) | (x & ((1 << (n - 1)) - 1));

			t1 = tb_mclock();
			while (n1--) tb_bits_set_sbits32(p, b, x, n);
			t1 = tb_int64_sub(tb_mclock(), t1);
			ts = tb_int64_add(ts, t1);

			t2 = tb_mclock();
			while (n2--) y = tb_bits_get_sbits32(p, b, n);
			t2 = tb_int64_sub(tb_mclock(), t2);
			tg = tb_int64_add(tg, t2);

			if (xx != y)
			{
				tb_printf("[bitops]: sbits32 set: b: %d, n: %u, x: %d, %llu ms\n", b, n, xx, t1);
				tb_printf("[bitops]: sbits32 get: b: %u, n: %u, y: %d, %llu ms\n", b, n, y, t2);
			}
		}
	}
	tb_printf("[bitops]: sbits32 set: %llu ms, get: %llu ms\n", ts, tg);
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_test_bits_swap_u16();
	tb_test_bits_swap_u32();
	tb_test_bits_swap_u64();

	tb_test_bits_ubits32(0x87654321);
	tb_test_bits_ubits32(0x12345678);
	
	tb_test_bits_sbits32(0x87654321);
	tb_test_bits_sbits32(0x12345678);

	tb_test_bits_sbits32(-300);
	tb_test_bits_sbits32(300);

	tb_exit();
	return 0;
}
