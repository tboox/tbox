/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_atomic_main(tb_int_t argc, tb_char_t** argv)
{
	tb_atomic_t a;
	tb_atomic_set0(&a);
	tb_print("%ld", tb_atomic_get(&a));
	tb_print("%ld", tb_atomic_fetch_and_set(&a, 1));
	tb_print("%ld", tb_atomic_fetch_and_pset(&a, 1, 2));
	tb_print("%ld", tb_atomic_fetch_and_inc(&a));
	tb_print("%ld", tb_atomic_fetch_and_dec(&a));
	tb_print("%ld", tb_atomic_fetch_and_add(&a, 10));
	tb_print("%ld", tb_atomic_fetch_and_sub(&a, 10));
	tb_print("%ld", tb_atomic_fetch_and_and(&a, 0xff));
	tb_print("%ld", tb_atomic_fetch_and_xor(&a, 0xff));
	tb_print("%ld", tb_atomic_fetch_and_or(&a, 0xff));
	tb_print("%ld", tb_atomic_get(&a));
	tb_print("%ld", tb_atomic_inc_and_fetch(&a));
	tb_print("%ld", tb_atomic_dec_and_fetch(&a));
	tb_print("%ld", tb_atomic_add_and_fetch(&a, 10));
	tb_print("%ld", tb_atomic_sub_and_fetch(&a, 10));
	tb_print("%ld", tb_atomic_and_and_fetch(&a, 0xff));
	tb_print("%ld", tb_atomic_xor_and_fetch(&a, 0xff));
	tb_print("%ld", tb_atomic_or_and_fetch(&a, 0xff));

	return 0;
}
