/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_byte_t data[1024];

	tb_sbuffer_t b;
	tb_sbuffer_init(&b, data, 1024);

	tb_sbuffer_memncpy(&b, "hello ", 6);
	tb_sbuffer_memncat(&b, "world", 6);
	tb_print("%s", tb_sbuffer_data(&b));

	tb_sbuffer_exit(&b);

	return 0;
}
