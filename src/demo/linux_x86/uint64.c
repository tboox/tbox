#include "tbox.h"

#ifdef TB_CONFIG_TYPE_INT64
# 	define tb_uint64_to_native(x) 		((unsigned long long)(x))
#else
# 	define tb_uint64_to_native(x) 		(((unsigned long long)x.h << 32) | x.l)
#endif

static tb_uint64_t tb_uint64_test_add(tb_uint32_t x, tb_uint32_t y)
{
	tb_uint64_t r = tb_uint64_add(tb_uint32_to_uint64(x), tb_uint32_to_uint64(y));
	tb_printf("[uint64]: %u + %u = %llu\n", x, y, tb_uint64_to_native(r));
	return r;
}
static tb_uint64_t tb_uint64_test_add_uint32(tb_uint64_t x, tb_uint32_t y)
{
	tb_uint64_t r = tb_uint64_add_uint32(x, y);
	tb_printf("[uint64]: %llu + %u = %llu\n", tb_uint64_to_native(x), y, tb_uint64_to_native(r));
	return r;
}
static tb_uint64_t tb_uint64_test_sub(tb_uint64_t x, tb_uint64_t y)
{
	tb_uint64_t r = tb_uint64_sub(x, y);
	tb_printf("[uint64]: %llu - %llu = %llu\n", tb_uint64_to_native(x), tb_uint64_to_native(y), tb_uint64_to_native(r));
	return r;
}
static tb_uint64_t tb_uint64_test_sub_uint32(tb_uint64_t x, tb_uint32_t y)
{
	tb_uint64_t r = tb_uint64_sub_uint32(x, y);
	tb_printf("[uint64]: %llu - %u = %llu\n", tb_uint64_to_native(x), y, tb_uint64_to_native(r));
	return r;
}
static tb_uint64_t tb_uint64_test_mul(tb_uint64_t x, tb_uint64_t y)
{
	tb_uint64_t r = tb_uint64_mul(x, y);
	tb_printf("[uint64]: %llu * %llu = %llu\n", tb_uint64_to_native(x), tb_uint64_to_native(y), tb_uint64_to_native(r));
	return r;
}
static tb_uint64_t tb_uint64_test_mul_uint32(tb_uint64_t x, tb_uint32_t y)
{
	tb_uint64_t r = tb_uint64_mul_uint32(x, y);
	tb_printf("[uint64]: %llu * %u = %llu\n", tb_uint64_to_native(x), y, tb_uint64_to_native(r));
	return r;
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;


	tb_printf("==================================================\n");
	tb_printf("tb_uint64_test_add:\n");
	tb_uint64_t a0 = tb_uint64_test_add(0x0, 0xf);
	tb_uint64_t a1 = tb_uint64_test_add(0xf, 0xff);
	tb_uint64_t a2 = tb_uint64_test_add(0xff, 0xfff);
	tb_uint64_t a3 = tb_uint64_test_add(0xfff, 0xffff);
	tb_uint64_t a4 = tb_uint64_test_add(0xffff, 0xfffff);
	tb_uint64_t a5 = tb_uint64_test_add(0xfffff, 0xffffff);
	tb_uint64_t a6 = tb_uint64_test_add(0xffffff, 0xfffffff);
	tb_uint64_t a7 = tb_uint64_test_add(0xfffffff, 0xfffffff);
	tb_uint64_t a8 = tb_uint64_test_add(0xffffffff, 0xffffffff);
	
	tb_printf("==================================================\n");
	tb_printf("tb_uint64_test_add_uint32:\n");
	tb_uint64_test_add_uint32(a0, 0xf);
	tb_uint64_test_add_uint32(a1, 0xff);
	tb_uint64_test_add_uint32(a2, 0xfff);
	tb_uint64_test_add_uint32(a3, 0xffff);
	tb_uint64_test_add_uint32(a4, 0xfffff);
	tb_uint64_test_add_uint32(a5, 0xffffff);
	tb_uint64_test_add_uint32(a6, 0xfffffff);
	tb_uint64_test_add_uint32(a7, 0xfffffff);
	tb_uint64_test_add_uint32(a8, 0xffffffff);

	tb_printf("==================================================\n");
	tb_printf("tb_uint64_test_sub:\n");
	tb_uint64_test_sub(a1, a0);
	tb_uint64_test_sub(a0, a1);
	tb_uint64_test_sub(a3, a2);
	tb_uint64_test_sub(a2, a3);
	tb_uint64_test_sub(a5, a4);
	tb_uint64_test_sub(a4, a5);
	tb_uint64_test_sub(a7, a6);
	tb_uint64_test_sub(a6, a7);
	tb_uint64_test_sub(a8, a7);
	tb_uint64_test_sub(a7, a8);
	tb_uint64_test_sub(a8, a1);
	tb_uint64_test_sub(a1, a8);
	tb_uint64_test_sub(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_uint64_test_sub_uint32:\n");
	tb_uint64_test_sub_uint32(a1, 0x0);
	tb_uint64_test_sub_uint32(a0, 0xf);
	tb_uint64_test_sub_uint32(a3, 0xff);
	tb_uint64_test_sub_uint32(a2, 0xfff);
	tb_uint64_test_sub_uint32(a5, 0xffff);
	tb_uint64_test_sub_uint32(a4, 0xfffff);
	tb_uint64_test_sub_uint32(a7, 0xffffff);
	tb_uint64_test_sub_uint32(a6, 0xfffffff);
	tb_uint64_test_sub_uint32(a8, 0xffffffff);
	tb_uint64_test_sub_uint32(a7, 0xfffffff);
	tb_uint64_test_sub_uint32(a8, 0xffffff);
	tb_uint64_test_sub_uint32(a1, 0xfffff);
	tb_uint64_test_sub_uint32(a8, 0xffff);

	tb_printf("==================================================\n");
	tb_printf("tb_uint64_test_mul:\n");
	tb_uint64_test_mul(a1, a0);
	tb_uint64_test_mul(a0, a1);
	tb_uint64_test_mul(a3, a2);
	tb_uint64_test_mul(a2, a3);
	tb_uint64_test_mul(a5, a4);
	tb_uint64_test_mul(a4, a5);
	tb_uint64_test_mul(a7, a6);
	tb_uint64_test_mul(a6, a7);
	tb_uint64_test_mul(a8, a7);
	tb_uint64_test_mul(a7, a8);
	tb_uint64_test_mul(a8, a1);
	tb_uint64_test_mul(a1, a8);
	tb_uint64_test_mul(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_uint64_test_mul_uint32:\n");
	tb_uint64_test_mul_uint32(a1, 0x0);
	tb_uint64_test_mul_uint32(a0, 0xf);
	tb_uint64_test_mul_uint32(a3, 0xff);
	tb_uint64_test_mul_uint32(a2, 0xfff);
	tb_uint64_test_mul_uint32(a5, 0xffff);
	tb_uint64_test_mul_uint32(a4, 0xfffff);
	tb_uint64_test_mul_uint32(a7, 0xffffff);
	tb_uint64_test_mul_uint32(a6, 0xfffffff);
	tb_uint64_test_mul_uint32(a8, 0xffffffff);
	tb_uint64_test_mul_uint32(a7, 0xfffffff);
	tb_uint64_test_mul_uint32(a8, 0xffffff);
	tb_uint64_test_mul_uint32(a1, 0xfffff);
	tb_uint64_test_mul_uint32(a8, 0xffff);

	return 0;
}
