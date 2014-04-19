/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_string_scoped_string_main(tb_int_t argc, tb_char_t** argv)
{
	tb_scoped_string_t s;
	tb_scoped_string_init(&s);

	tb_scoped_string_cstrcpy(&s, "hello");
	tb_scoped_string_chrcat(&s, ' ');
	tb_scoped_string_cstrfcat(&s, "%s", "world");
	tb_scoped_string_chrcat(&s, ' ');
	tb_scoped_string_chrncat(&s, 'x', 5);
	tb_trace_i("%d: %s", tb_scoped_string_size(&s), tb_scoped_string_cstr(&s));

	tb_scoped_string_exit(&s);

	return 0;
}
