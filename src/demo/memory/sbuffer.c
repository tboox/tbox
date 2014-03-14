/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_sbuffer_main(tb_int_t argc, tb_char_t** argv)
{
	tb_byte_t data[1024];

	tb_sbuffer_t b;
	tb_sbuffer_init(&b, data, 1024);

	tb_sbuffer_memncpy(&b, "hello ", 6);
	tb_sbuffer_memncat(&b, "world", 6);
	tb_trace_i("%s", tb_sbuffer_data(&b));

	tb_sbuffer_exit(&b);

	return 0;
}
