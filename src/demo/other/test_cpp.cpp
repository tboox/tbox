#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_int_t test(tb_int_t argc, tb_char_t** argv)
{
	__tb_volatile__ tb_int_t* a = new tb_int_t[10];
	__tb_volatile__ tb_int_t* b = new tb_int_t;
	__tb_volatile__ tb_int_t* c = new tb_int_t;
	
	delete[] a;

	//tb_memory_dump();
	return 0;
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
extern "C"
{
	tb_int_t test_cpp_main(tb_int_t argc, tb_char_t** argv)
	{
		return test(argc, argv);
	}
}
