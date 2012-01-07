/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* /////////////////////////////////////////////////////////
 * main
 */
int main(int argc, char** argv)
{
	if (!tb_init(malloc(30 * 1024 * 1024), 30 * 1024 * 1024)) return 0;

	tb_spool_t* spool = tb_spool_init(TB_SPOOL_SIZE_SMALL);
	tb_assert_and_check_goto(spool, end);

	tb_char_t* s0 = tb_spool_strdup(spool, "0");
	tb_char_t* s1 = tb_spool_strdup(spool, "01");
	tb_char_t* s2 = tb_spool_strdup(spool, "012");
	tb_char_t* s3 = tb_spool_strdup(spool, "0123");
	tb_char_t* s4 = tb_spool_strdup(spool, "01234");
	tb_char_t* s5 = tb_spool_strdup(spool, "012345");
	tb_char_t* s6 = tb_spool_strdup(spool, "0123456");
	tb_char_t* s7 = tb_spool_strdup(spool, "01234567");
	tb_char_t* s8 = tb_spool_strdup(spool, "012345678");
	tb_char_t* s9 = tb_spool_strdup(spool, "0123456789");

	tb_printf("%s\n", s0);
	tb_printf("%s\n", s1);
	tb_printf("%s\n", s2);
	tb_printf("%s\n", s3);
	tb_printf("%s\n", s4);
	tb_printf("%s\n", s5);
	tb_printf("%s\n", s6);
	tb_printf("%s\n", s7);
	tb_printf("%s\n", s8);
	tb_printf("%s\n", s9);

	tb_spool_free(spool, s0);
	tb_spool_free(spool, s1);
	tb_spool_free(spool, s2);
	tb_spool_free(spool, s3);
	tb_spool_free(spool, s4);
	tb_spool_free(spool, s5);
	tb_spool_free(spool, s6);
	tb_spool_free(spool, s7);
	tb_spool_free(spool, s8);
	tb_spool_free(spool, s9);

	// check
	__tb_volatile__ tb_char_t s[256] = {0};
	__tb_volatile__ tb_size_t n = 80000;
	while (n--) 
	{
		tb_int_t r = snprintf(s, 256, "%x", tb_rand_uint32(0, TB_MAXU32)); s[r] == '\0'; 
		tb_assert_and_check_break(!tb_strcmp(tb_spool_strdup(spool, s), s));
	}

end:
	tb_printf("end\n");
	if (spool) tb_spool_exit(spool);
	return 0;
}
