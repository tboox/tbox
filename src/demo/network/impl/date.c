/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"
#include "../../../tbox/network/impl/http/date.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_test_date(tb_char_t const* cstr)
{
    tb_tm_t date = {0};
    if (tb_gmtime(tb_http_date_from_cstr(cstr, tb_strlen(cstr)), &date))
    {
        tb_trace_i("%s => %04ld-%02ld-%02ld %02ld:%02ld:%02ld GMT, week: %d"
            , cstr
            , date.year
            , date.month
            , date.mday
            , date.hour
            , date.minute
            , date.second
            , date.week);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_impl_date_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_test_date("Sun, 06 Nov 1994 08:49:37 GMT");
    tb_demo_test_date("Sun Nov 6 08:49:37 1994");
    tb_demo_test_date("Sun, 06-Nov-1994 08:49:37 GMT");
    tb_demo_test_date("Mon, 19 May 2014 07:21:56 GMT");
    tb_demo_test_date("Thu, 31-Dec-37 23:55:55 GMT");

    return 0;
}
