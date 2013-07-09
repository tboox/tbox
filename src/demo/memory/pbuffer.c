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

	tb_pbuffer_t b;
	tb_pbuffer_init(&b);

	tb_pbuffer_memncpy(&b, "hello ", 6);
	tb_pbuffer_memncat(&b, "world", 6);
	tb_print("%s", tb_pbuffer_data(&b));

	tb_pbuffer_exit(&b);

	return 0;
}
