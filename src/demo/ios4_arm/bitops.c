#include "tbox.h"

static tb_void_t tb_test_bits_swap_u16()
{
	__tb_volatile__ tb_uint16_t x = 0x1234;
	__tb_volatile__ tb_size_t n = 100000001;
	__tb_volatile__ tb_int64_t 	dt = tb_mclock();

	while (n--) x = tb_bits_swap_u16(x);

	dt = tb_int64_sub(tb_mclock(), dt);
	tb_printf("[bitops]: swap_u16: %x => %x, %u ms\n", 0x1234, x, tb_int64_to_int32(dt));
}
static tb_void_t tb_test_bits_swap_u32()
{
	__tb_volatile__ tb_uint32_t x = 0x12345678;
	__tb_volatile__ tb_size_t n = 100000001;
	__tb_volatile__ tb_int64_t 	dt = tb_mclock();

	while (n--) x = tb_bits_swap_u32(x);

	dt = tb_int64_sub(tb_mclock(), dt);
	tb_printf("[bitops]: swap_u32: %x => %x, %u ms\n", 0x12345678, x, tb_int64_to_int32(dt));
}
static tb_void_t tb_test_bits_swap_u64()
{
	__tb_volatile__ tb_uint64_t x = tb_uint32_to_uint64(0x12345678);
	__tb_volatile__ tb_size_t n = 100000001;
	__tb_volatile__ tb_int64_t 	dt = tb_mclock();

	while (n--) x = tb_bits_swap_u64(x);

	dt = tb_int64_sub(tb_mclock(), dt);
	tb_printf("[bitops]: swap_u64: %x => %llx, %llu ms\n", 0x12345678, x, dt);
}

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_test_bits_swap_u16();
	tb_test_bits_swap_u32();
	tb_test_bits_swap_u64();

	tb_exit();
	return 0;
}
