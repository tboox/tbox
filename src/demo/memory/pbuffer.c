/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_pbuffer_main(tb_int_t argc, tb_char_t** argv)
{
	tb_pbuffer_t b;
	tb_pbuffer_init(&b);

	tb_pbuffer_memncpy(&b, "hello ", 6);
	tb_pbuffer_memncat(&b, "world", 6);
	tb_trace_i("%s", tb_pbuffer_data(&b));

	tb_pbuffer_exit(&b);

	return 0;
}
