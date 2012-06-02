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

	tb_pstring_t s;
	tb_pstring_init(&s);

	tb_pstring_cstrcpy(&s, "hello");
	tb_pstring_chrcat(&s, ' ');
	tb_pstring_cstrfcat(&s, "%s", "world");
	tb_pstring_chrcat(&s, ' ');
	tb_pstring_chrncat(&s, 'x', 5);
	tb_print("%d: %s", tb_pstring_size(&s), tb_pstring_cstr(&s));

	tb_pstring_exit(&s);

	return 0;
}
