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

	tb_sstring_t 	s;
	tb_char_t 		b[4096];
	tb_sstring_init(&s, b, 4096);

	tb_sstring_cstrcpy(&s, "hello");
	tb_sstring_chrcat(&s, ' ');
	tb_sstring_cstrfcat(&s, "%s", "world");
	tb_print("%s", tb_sstring_cstr(&s));

	tb_sstring_exit(&s);

	return 0;
}
