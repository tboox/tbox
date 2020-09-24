/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_test_writ()
{
    tb_trace_i("=================== test stdfile writ ===================");
    tb_stdfile_writ(tb_stdfile_output(), (tb_byte_t const*)"hello world!\n", tb_strlen("hello world!\n"));
}
static tb_void_t tb_test_read()
{
    tb_trace_i("=================== test stdfile read ===================");
    tb_byte_t data[4096] = {0};
    if (tb_stdfile_read(tb_stdfile_input(), data, 3))
        tb_trace_i("%s", data);
    tb_trace_i("------");

    // ignore left characters
    tb_char_t ch;
    while (tb_stdfile_getc(tb_stdfile_input(), &ch) && ch != '\n') ;
}
static tb_void_t tb_test_puts()
{
    tb_trace_i("=================== test stdfile puts ===================");
    tb_stdfile_puts(tb_stdfile_output(), "hello world!\n");
}
static tb_void_t tb_test_gets()
{
    tb_trace_i("=================== test stdfile gets ===================");
    tb_char_t data[4096] = {0};
    if (tb_stdfile_gets(tb_stdfile_input(), data, sizeof(data)))
        tb_trace_i("%s", data);
    tb_trace_i("------");
}
static tb_void_t tb_test_putc()
{
    tb_trace_i("=================== test stdfile putc ===================");
    tb_char_t const* s = "hello world!\n";
    tb_size_t        n = tb_strlen("hello world!\n");
    tb_size_t        i = 0;
    for (i = 0; i < n; i++)
        tb_stdfile_putc(tb_stdfile_output(), s[i]);
}
static tb_void_t tb_test_getc()
{
    tb_trace_i("=================== test stdfile getc ===================");
    tb_char_t ch;
    tb_int_t  i = 0;
    tb_char_t s[4096] = {0};
    while (tb_stdfile_getc(tb_stdfile_input(), &ch) && ch != '\n')
        s[i++] = (tb_char_t)ch;
    s[i] = '\0';
    tb_trace_i("%s", s);
    tb_trace_i("------");
}
static tb_void_t tb_test_peek()
{
    tb_trace_i("=================== test stdfile peek ===================");
    tb_char_t ch;
    tb_int_t  i = 0;
    tb_char_t s[4096] = {0};
    while (tb_stdfile_getc(tb_stdfile_input(), &ch) && ch != '\n')
    {
        tb_char_t ch2;
        if (tb_stdfile_peek(tb_stdfile_input(), &ch2))
            tb_trace_i("peek: %c", ch2);
        s[i++] = (tb_char_t)ch;
    }
    s[i] = '\0';
    tb_trace_i("%s", s);
    tb_trace_i("------");
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_stdfile_main(tb_int_t argc, tb_char_t** argv)
{
    tb_test_writ();
    tb_test_putc();
    tb_test_puts();
    tb_test_read();
    tb_test_getc();
    tb_test_gets();
    tb_test_peek();
    return 0;
}
