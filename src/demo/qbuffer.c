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
