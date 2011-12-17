/* /////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"

/* /////////////////////////////////////////////////////////////////////////
 * macros
 */ 
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_atomic_t a;
	tb_atomic_set0(&a);
	tb_print("%d", tb_atomic_get(&a));
	tb_print("%d", tb_atomic_fetch_and_set(&a, 1));
	tb_print("%d", tb_atomic_fetch_and_pset(&a, 1, 2));
	tb_print("%d", tb_atomic_fetch_and_inc(&a));
	tb_print("%d", tb_atomic_fetch_and_dec(&a));
	tb_print("%d", tb_atomic_fetch_and_add(&a, 10));
	tb_print("%d", tb_atomic_fetch_and_sub(&a, 10));
	tb_print("%d", tb_atomic_get(&a));
	tb_print("%d", tb_atomic_inc_and_fetch(&a));
	tb_print("%d", tb_atomic_dec_and_fetch(&a));
	tb_print("%d", tb_atomic_add_and_fetch(&a, 10));
	tb_print("%d", tb_atomic_sub_and_fetch(&a, 10));

	return 0;
}
