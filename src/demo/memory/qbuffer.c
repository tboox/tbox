/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_memory_qbuffer_main(tb_int_t argc, tb_char_t** argv)
{
	tb_char_t 		d[1024];
	tb_qbuffer_t 	b;
	tb_qbuffer_init(&b, 1024);

	tb_qbuffer_writ(&b, "hello", 5);
	tb_qbuffer_writ(&b, " ", 1);
	tb_qbuffer_writ(&b, "world", 6);
	tb_qbuffer_read(&b, d, 1024);
	tb_print("%s", d);

	tb_qbuffer_exit(&b);

	return 0;
}
