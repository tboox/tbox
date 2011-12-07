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

	tb_pstring_t s;
	tb_pstring_init(&s);

	tb_pstring_cstrcpy(&s, "hello");
	tb_pstring_chrcat(&s, ' ');
	tb_pstring_cstrfcat(&s, "%s", "world");
	tb_print("%s", tb_pstring_cstr(&s));

	tb_pstring_exit(&s);

	return 0;
}
