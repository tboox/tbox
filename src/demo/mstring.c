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

	tb_mstring_t s1;
	tb_mstring_t s2;
	tb_mstring_t s3;
	tb_mstring_init(&s1);
	tb_mstring_init(&s2);
	tb_mstring_init(&s3);

	tb_mstring_cstrcpy(&s1, "hello");
	tb_mstring_chrcat(&s1, ' ');
	tb_mstring_cstrfcat(&s1, "%s", "world");
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");

	tb_mstring_strcat(&s2, &s1);
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");

	tb_mstring_chrcat(&s2, '!');
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");

	tb_mstring_strcpy(&s3, &s2);
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");

	tb_mstring_cstrfcat(&s3, "... oh! my god!");
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");

	tb_mstring_exit(&s1);
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");

	tb_mstring_exit(&s2);
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");

	tb_mstring_exit(&s3);	
	tb_print("s1: [%u]: %s", tb_mstring_refn(&s1), tb_mstring_cstr(&s1));
	tb_print("s2: [%u]: %s", tb_mstring_refn(&s2), tb_mstring_cstr(&s2));
	tb_print("s3: [%u]: %s", tb_mstring_refn(&s3), tb_mstring_cstr(&s3));
	tb_print("");


	return 0;
}
