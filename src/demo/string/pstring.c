/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_string_pstring_main(tb_int_t argc, tb_char_t** argv)
{
	tb_pstring_t s;
	tb_pstring_init(&s);

	tb_pstring_cstrcpy(&s, "hello");
	tb_pstring_chrcat(&s, ' ');
	tb_pstring_cstrfcat(&s, "%s", "world");
	tb_pstring_chrcat(&s, ' ');
	tb_pstring_chrncat(&s, 'x', 5);
	tb_trace_i("%d: %s", tb_pstring_size(&s), tb_pstring_cstr(&s));

	tb_pstring_exit(&s);

	return 0;
}
