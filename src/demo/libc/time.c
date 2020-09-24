/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libc_time_main(tb_int_t argc, tb_char_t** argv)
{
    // time
    tb_time_t now = tb_time();

    // the local time
    tb_tm_t lt = {0};
    if (tb_localtime(now, &lt))
    {
        tb_trace_i("LMT: %04ld-%02ld-%02ld %02ld:%02ld:%02ld, week: %d, time: %lld ?= %lld"
            , lt.year
            , lt.month
            , lt.mday
            , lt.hour
            , lt.minute
            , lt.second
            , lt.week
            , tb_mktime(&lt)
            , now);
    }

    // the gmt time
    tb_tm_t gt = {0};
    if (tb_gmtime(now, &gt))
    {
        tb_trace_i("GMT: %04ld-%02ld-%02ld %02ld:%02ld:%02ld, week: %d, time: %lld ?= %lld"
            , gt.year
            , gt.month
            , gt.mday
            , gt.hour
            , gt.minute
            , gt.second
            , gt.week
            , tb_gmmktime(&gt)
            , now);
    }

    return 0;
}
