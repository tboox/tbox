/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
#include "prefix/packed.h"
typedef struct __tb_xxxxx_t
{
 		tb_byte_t 	a;
 		tb_uint32_t b;

} __tb_packed__ tb_xxxxx_t;
#include "prefix/packed.h"

tb_void_t __tb_inline_force__ add(int a);
tb_void_t __tb_inline_force__ add(int a)
{
}

tb_int_t test2_main(tb_int_t argc, tb_char_t** argv);

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// c++
	test2_main(argc, argv);

	tb_print("%d", sizeof(tb_xxxxx_t));

	tb_exit();
	return 0;
}
