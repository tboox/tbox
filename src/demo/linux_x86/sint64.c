#include "tbox.h"

static tb_sint64_t tb_sint64_test_add(tb_sint32_t x, tb_sint32_t y)
{
	tb_sint64_t r = tb_sint64_add(tb_sint32_to_sint64(x), tb_sint32_to_sint64(y));
	tb_printf("[sint64]: %d + %d = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_add_sint32(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t r = tb_sint64_add_sint32(x, y);
	tb_printf("[sint64]: %lld + %d = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_sub(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t r = tb_sint64_sub(x, y);
	tb_printf("[sint64]: %lld - %lld = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_sub_sint32(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t r = tb_sint64_sub_sint32(x, y);
	tb_printf("[sint64]: %lld - %d = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_mul(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t r = tb_sint64_mul(x, y);
	tb_printf("[sint64]: %lld * %lld = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_mul_sint32(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t r = tb_sint64_mul_sint32(x, y);
	tb_printf("[sint64]: %lld * %d = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_not(tb_sint64_t x)
{
	tb_sint64_t r = tb_sint64_not(x);
	tb_printf("[sint64]: ~%lld = %lld\n", x, r);
	return r;
}
static tb_sint64_t tb_sint64_test_or(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t r = tb_sint64_or(x, y);
	tb_printf("[sint64]: %lld | %lld = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_or_sint32(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t r = tb_sint64_or_sint32(x, y);
	tb_printf("[sint64]: %lld | %d = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_and(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t r = tb_sint64_and(x, y);
	tb_printf("[sint64]: %lld & %lld = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_and_sint32(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t r = tb_sint64_and_sint32(x, y);
	tb_printf("[sint64]: %lld & %d = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_xor(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t r = tb_sint64_xor(x, y);
	tb_printf("[sint64]: %lld ^ %lld = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_xor_sint32(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t r = tb_sint64_xor_sint32(x, y);
	tb_printf("[sint64]: %lld ^ %d = %lld\n", x, y, r);
	return r;
}
static tb_sint64_t tb_sint64_test_lsh(tb_sint64_t x, tb_sint32_t b)
{
	tb_sint64_t r = tb_sint64_lsh(x, b);
	tb_printf("[sint64]: %lld << %d = %lld\n", x, b, r);
	return r;
}
static tb_sint64_t tb_sint64_test_rsh(tb_sint64_t x, tb_sint32_t b)
{
	tb_sint64_t r = tb_sint64_rsh(x, b);
	tb_printf("[sint64]: %lld >> %d = %lld\n", x, b, r);
	return r;
}
static tb_size_t tb_sint64_test_clz(tb_sint64_t x)
{
	tb_size_t r = tb_sint64_clz(x);
	tb_printf("[sint64]: clz: %lld = %d\n", x, r);
	return r;
}
static tb_sint64_t tb_sint64_test_neg(tb_sint64_t x)
{
	tb_sint64_t r = tb_sint64_neg(x);
	tb_printf("[sint64]: neg: %lld = %lld\n", x, r);
	return r;
}
static tb_sint64_t tb_sint64_test_abs(tb_sint64_t x)
{
	tb_sint64_t r = tb_sint64_abs(x);
	tb_printf("[sint64]: abs: %lld = %lld\n", x, r);
	return r;
}
static tb_void_t tb_sint64_test_tt(tb_sint64_t x, tb_sint64_t y)
{
	tb_printf("[sint64]: ez: %lld : %d\n", x, tb_sint64_ez(x)? 1 : 0);
	tb_printf("[sint64]: nz: %lld : %d\n", x, tb_sint64_nz(x)? 1 : 0);
	tb_printf("[sint64]: lz: %lld : %d\n", x, tb_sint64_lz(x)? 1 : 0);
	tb_printf("[sint64]: gz: %lld : %d\n", x, tb_sint64_gz(x)? 1 : 0);
	tb_printf("[sint64]: et: %lld == %lld : %d\n", x, y, tb_sint64_et(x, y)? 1 : 0);
	tb_printf("[sint64]: nt: %lld != %lld : %d\n", x, y, tb_sint64_nt(x, y)? 1 : 0);
	tb_printf("[sint64]: lt: %lld < %lld : %d\n", x, y, tb_sint64_lt(x, y)? 1 : 0);
	tb_printf("[sint64]: gt: %lld > %lld : %d\n", x, y, tb_sint64_gt(x, y)? 1 : 0);
	tb_printf("\n");
}
static tb_void_t tb_sint64_test_tt_sint32(tb_sint64_t x, tb_sint32_t y)
{
	tb_printf("[sint64]: et: %lld == %lu : %d\n", x, y, tb_sint64_et_sint32(x, y)? 1 : 0);
	tb_printf("[sint64]: nt: %lld != %lu : %d\n", x, y, tb_sint64_nt_sint32(x, y)? 1 : 0);
	tb_printf("[sint64]: lt: %lld < %lu : %d\n", x, y, tb_sint64_lt_sint32(x, y)? 1 : 0);
	tb_printf("[sint64]: gt: %lld > %lu : %d\n", x, y, tb_sint64_gt_sint32(x, y)? 1 : 0);
	tb_printf("\n");
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;


	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_add:\n");
	tb_sint64_t a0 = tb_sint64_test_add(0x0, 0xf);
	tb_sint64_t a1 = tb_sint64_test_add(0xf, 0xff);
	tb_sint64_t a2 = tb_sint64_test_add(0xff, 0xfff);
	tb_sint64_t a3 = tb_sint64_test_add(0xfff, 0xffff);
	tb_sint64_t a4 = tb_sint64_test_add(0xffff, 0xfffff);
	tb_sint64_t a5 = tb_sint64_test_add(0xfffff, 0xffffff);
	tb_sint64_t a6 = tb_sint64_test_add(0xffffff, 0xfffffff);
	tb_sint64_t a7 = tb_sint64_test_add(0xfffffff, 0xfffffff);
	tb_sint64_t a8 = tb_sint64_test_add(0xffffffff, 0xffffffff);
	
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_add_sint32:\n");
	tb_sint64_test_add_sint32(a0, 0xf);
	tb_sint64_test_add_sint32(a1, 0xff);
	tb_sint64_test_add_sint32(a2, 0xfff);
	tb_sint64_test_add_sint32(a3, 0xffff);
	tb_sint64_test_add_sint32(a4, 0xfffff);
	tb_sint64_test_add_sint32(a5, 0xffffff);
	tb_sint64_test_add_sint32(a6, 0xfffffff);
	tb_sint64_test_add_sint32(a7, 0xfffffff);
	tb_sint64_test_add_sint32(a8, 0xffffffff);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_sub:\n");
	a1 = tb_sint64_test_sub(a1, a0);
	a0 = tb_sint64_test_sub(a0, a1);
	a3 = tb_sint64_test_sub(a3, a2);
	a2 = tb_sint64_test_sub(a2, a3);
	a5 = tb_sint64_test_sub(a5, a4);
	a4 = tb_sint64_test_sub(a4, a5);
	a7 = tb_sint64_test_sub(a7, a6);
	a6 = tb_sint64_test_sub(a6, a7);
	a8 = tb_sint64_test_sub(a8, a7);
	a7 = tb_sint64_test_sub(a7, a8);
	a8 = tb_sint64_test_sub(a8, a1);
	a1 = tb_sint64_test_sub(a1, a8);
	a8 = tb_sint64_test_sub(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_sub_sint32:\n");
	tb_sint64_test_sub_sint32(a1, 0x0);
	tb_sint64_test_sub_sint32(a0, 0xf);
	tb_sint64_test_sub_sint32(a3, 0xff);
	tb_sint64_test_sub_sint32(a2, 0xfff);
	tb_sint64_test_sub_sint32(a5, 0xffff);
	tb_sint64_test_sub_sint32(a4, 0xfffff);
	tb_sint64_test_sub_sint32(a7, 0xffffff);
	tb_sint64_test_sub_sint32(a6, 0xfffffff);
	tb_sint64_test_sub_sint32(a8, 0xffffffff);
	tb_sint64_test_sub_sint32(a7, 0xfffffff);
	tb_sint64_test_sub_sint32(a8, 0xffffff);
	tb_sint64_test_sub_sint32(a1, 0xfffff);
	tb_sint64_test_sub_sint32(a8, 0xffff);

#if 0
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_mul:\n");
	tb_sint64_test_mul(a1, a0);
	tb_sint64_test_mul(a0, a1);
	tb_sint64_test_mul(a3, a2);
	tb_sint64_test_mul(a2, a3);
	tb_sint64_test_mul(a5, a4);
	tb_sint64_test_mul(a4, a5);
	tb_sint64_test_mul(a7, a6);
	tb_sint64_test_mul(a6, a7);
	tb_sint64_test_mul(a8, a7);
	tb_sint64_test_mul(a7, a8);
	tb_sint64_test_mul(a8, a1);
	tb_sint64_test_mul(a1, a8);
	tb_sint64_test_mul(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_mul_sint32:\n");
	tb_sint64_test_mul_sint32(a1, 0x0);
	tb_sint64_test_mul_sint32(a0, 0xf);
	tb_sint64_test_mul_sint32(a3, 0xff);
	tb_sint64_test_mul_sint32(a2, 0xfff);
	tb_sint64_test_mul_sint32(a5, 0xffff);
	tb_sint64_test_mul_sint32(a4, 0xfffff);
	tb_sint64_test_mul_sint32(a7, 0xffffff);
	tb_sint64_test_mul_sint32(a6, 0xfffffff);
	tb_sint64_test_mul_sint32(a8, 0xffffffff);
	tb_sint64_test_mul_sint32(a7, 0xfffffff);
	tb_sint64_test_mul_sint32(a8, 0xffffff);
	tb_sint64_test_mul_sint32(a1, 0xfffff);
	tb_sint64_test_mul_sint32(a8, 0xffff);
#endif

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_or:\n");
	tb_sint64_test_or(a1, a0);
	tb_sint64_test_or(a0, a1);
	tb_sint64_test_or(a3, a2);
	tb_sint64_test_or(a2, a3);
	tb_sint64_test_or(a5, a4);
	tb_sint64_test_or(a4, a5);
	tb_sint64_test_or(a7, a6);
	tb_sint64_test_or(a6, a7);
	tb_sint64_test_or(a8, a7);
	tb_sint64_test_or(a7, a8);
	tb_sint64_test_or(a8, a1);
	tb_sint64_test_or(a1, a8);
	tb_sint64_test_or(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_or_sint32:\n");
	tb_sint64_test_or_sint32(a1, 0x0);
	tb_sint64_test_or_sint32(a0, 0xf);
	tb_sint64_test_or_sint32(a3, 0xff);
	tb_sint64_test_or_sint32(a2, 0xfff);
	tb_sint64_test_or_sint32(a5, 0xffff);
	tb_sint64_test_or_sint32(a4, 0xfffff);
	tb_sint64_test_or_sint32(a7, 0xffffff);
	tb_sint64_test_or_sint32(a6, 0xfffffff);
	tb_sint64_test_or_sint32(a8, 0xffffffff);
	tb_sint64_test_or_sint32(a7, 0xfffffff);
	tb_sint64_test_or_sint32(a8, 0xffffff);
	tb_sint64_test_or_sint32(a1, 0xfffff);
	tb_sint64_test_or_sint32(a8, 0xffff);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_and:\n");
	tb_sint64_test_and(a1, a0);
	tb_sint64_test_and(a0, a1);
	tb_sint64_test_and(a3, a2);
	tb_sint64_test_and(a2, a3);
	tb_sint64_test_and(a5, a4);
	tb_sint64_test_and(a4, a5);
	tb_sint64_test_and(a7, a6);
	tb_sint64_test_and(a6, a7);
	tb_sint64_test_and(a8, a7);
	tb_sint64_test_and(a7, a8);
	tb_sint64_test_and(a8, a1);
	tb_sint64_test_and(a1, a8);
	tb_sint64_test_and(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_and_sint32:\n");
	tb_sint64_test_and_sint32(a1, 0x0);
	tb_sint64_test_and_sint32(a0, 0xf);
	tb_sint64_test_and_sint32(a3, 0xff);
	tb_sint64_test_and_sint32(a2, 0xfff);
	tb_sint64_test_and_sint32(a5, 0xffff);
	tb_sint64_test_and_sint32(a4, 0xfffff);
	tb_sint64_test_and_sint32(a7, 0xffffff);
	tb_sint64_test_and_sint32(a6, 0xfffffff);
	tb_sint64_test_and_sint32(a8, 0xffffffff);
	tb_sint64_test_and_sint32(a7, 0xfffffff);
	tb_sint64_test_and_sint32(a8, 0xffffff);
	tb_sint64_test_and_sint32(a1, 0xfffff);
	tb_sint64_test_and_sint32(a8, 0xffff);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_xor:\n");
	tb_sint64_test_xor(a1, a0);
	tb_sint64_test_xor(a0, a1);
	tb_sint64_test_xor(a3, a2);
	tb_sint64_test_xor(a2, a3);
	tb_sint64_test_xor(a5, a4);
	tb_sint64_test_xor(a4, a5);
	tb_sint64_test_xor(a7, a6);
	tb_sint64_test_xor(a6, a7);
	tb_sint64_test_xor(a8, a7);
	tb_sint64_test_xor(a7, a8);
	tb_sint64_test_xor(a8, a1);
	tb_sint64_test_xor(a1, a8);
	tb_sint64_test_xor(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_xor_sint32:\n");
	tb_sint64_test_xor_sint32(a1, 0x0);
	tb_sint64_test_xor_sint32(a0, 0xf);
	tb_sint64_test_xor_sint32(a3, 0xff);
	tb_sint64_test_xor_sint32(a2, 0xfff);
	tb_sint64_test_xor_sint32(a5, 0xffff);
	tb_sint64_test_xor_sint32(a4, 0xfffff);
	tb_sint64_test_xor_sint32(a7, 0xffffff);
	tb_sint64_test_xor_sint32(a6, 0xfffffff);
	tb_sint64_test_xor_sint32(a8, 0xffffffff);
	tb_sint64_test_xor_sint32(a7, 0xfffffff);
	tb_sint64_test_xor_sint32(a8, 0xffffff);
	tb_sint64_test_xor_sint32(a1, 0xfffff);
	tb_sint64_test_xor_sint32(a8, 0xffff);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_clz:\n");
	tb_sint64_test_clz(TB_SINT64_ZERO);
	tb_sint64_test_clz(TB_SINT64_ONE);
	tb_sint64_test_clz(a0);
	tb_sint64_test_clz(a1);
	tb_sint64_test_clz(a2);
	tb_sint64_test_clz(a3);
	tb_sint64_test_clz(a4);
	tb_sint64_test_clz(a5);
	tb_sint64_test_clz(a6);
	tb_sint64_test_clz(a7);
	tb_sint64_test_clz(a8);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_not:\n");
	tb_sint64_test_not(TB_SINT64_ZERO);
	tb_sint64_test_not(TB_SINT64_ONE);
	tb_sint64_test_not(a0);
	tb_sint64_test_not(a1);
	tb_sint64_test_not(a2);
	tb_sint64_test_not(a3);
	tb_sint64_test_not(a4);
	tb_sint64_test_not(a5);
	tb_sint64_test_not(a6);
	tb_sint64_test_not(a7);
	tb_sint64_test_not(a8);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_neg:\n");
	tb_sint64_test_neg(TB_SINT64_ZERO);
	tb_sint64_test_neg(TB_SINT64_ONE);
	tb_sint64_test_neg(a0);
	tb_sint64_test_neg(a1);
	tb_sint64_test_neg(a2);
	tb_sint64_test_neg(a3);
	tb_sint64_test_neg(a4);
	tb_sint64_test_neg(a5);
	tb_sint64_test_neg(a6);
	tb_sint64_test_neg(a7);
	tb_sint64_test_neg(a8);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_abs:\n");
	tb_sint64_test_abs(TB_SINT64_ZERO);
	tb_sint64_test_abs(TB_SINT64_ONE);
	tb_sint64_test_abs(a0);
	tb_sint64_test_abs(a1);
	tb_sint64_test_abs(a2);
	tb_sint64_test_abs(a3);
	tb_sint64_test_abs(a4);
	tb_sint64_test_abs(a5);
	tb_sint64_test_abs(a6);
	tb_sint64_test_abs(a7);
	tb_sint64_test_abs(a8);
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_lsh:\n");
	tb_sint64_test_lsh(a1, 0);
	tb_sint64_test_lsh(a0, 4);
	tb_sint64_test_lsh(a3, 7);
	tb_sint64_test_lsh(a2, 8);
	tb_sint64_test_lsh(a5, 10);
	tb_sint64_test_lsh(a4, 15);
	tb_sint64_test_lsh(a7, 16);
	tb_sint64_test_lsh(a6, 17);
	tb_sint64_test_lsh(a8, 28);
	tb_sint64_test_lsh(a7, 29);
	tb_sint64_test_lsh(a8, 32);
	tb_sint64_test_lsh(a1, 34);
	tb_sint64_test_lsh(a8, 63);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_rsh:\n");
	tb_sint64_test_rsh(a1, 0);
	tb_sint64_test_rsh(a0, 4);
	tb_sint64_test_rsh(a3, 7);
	tb_sint64_test_rsh(a2, 8);
	tb_sint64_test_rsh(a5, 10);
	tb_sint64_test_rsh(a4, 15);
	tb_sint64_test_rsh(a7, 16);
	tb_sint64_test_rsh(a6, 17);
	tb_sint64_test_rsh(a8, 28);
	tb_sint64_test_rsh(a7, 29);
	tb_sint64_test_rsh(a8, 32);
	tb_sint64_test_rsh(a1, 34);
	tb_sint64_test_rsh(a8, 63);

	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_tt:\n");
	tb_sint64_test_tt(a1, a0);
	tb_sint64_test_tt(a0, a1);
	tb_sint64_test_tt(a3, a2);
	tb_sint64_test_tt(a2, a3);
	tb_sint64_test_tt(a5, a4);
	tb_sint64_test_tt(a4, a5);
	tb_sint64_test_tt(a7, a6);
	tb_sint64_test_tt(a6, a7);
	tb_sint64_test_tt(a8, a7);
	tb_sint64_test_tt(a7, a8);
	tb_sint64_test_tt(a8, a1);
	tb_sint64_test_tt(a1, a8);
	tb_sint64_test_tt(a8, a8);
	
	tb_printf("==================================================\n");
	tb_printf("tb_sint64_test_tt_sint32:\n");
	tb_sint64_test_tt_sint32(a1, 0x0);
	tb_sint64_test_tt_sint32(a0, 0xf);
	tb_sint64_test_tt_sint32(a3, 0xff);
	tb_sint64_test_tt_sint32(a2, 0xfff);
	tb_sint64_test_tt_sint32(a5, 0xffff);
	tb_sint64_test_tt_sint32(a4, 0xfffff);
	tb_sint64_test_tt_sint32(a7, 0xffffff);
	tb_sint64_test_tt_sint32(a6, 0xfffffff);
	tb_sint64_test_tt_sint32(a8, 0xffffffff);
	tb_sint64_test_tt_sint32(a7, 0xfffffff);
	tb_sint64_test_tt_sint32(a8, 0xffffff);
	tb_sint64_test_tt_sint32(a1, 0xfffff);
	tb_sint64_test_tt_sint32(a8, 0xffff);

	return 0;
}
